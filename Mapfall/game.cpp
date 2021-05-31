#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/compatibility.hpp>

#include "camera.h"
#include "game_object.h"
#include <queue>
#include <random>
#include "text_renderer.h"
#include <iomanip>

Camera camera(glm::vec3(-0.5f, 8.0f, 0.3f), glm::vec3(0.0f, 1.0f, 0.0f), -90, -89);
glm::vec3 cameraOffset;

GameObject* Player;
GameObject* Eyes;
GameObject* Beak;
GameObject* Wings[2];
GameObject* Feet[2];
GameObject* Countries[45];
GameObject* Flags[45];

TextRenderer* TopLeftText;
TextRenderer* TopRightText;
TextRenderer* CenterText;
TextRenderer* TimeText;

int TargetCountry = 0;
std::vector<int> countryNumbers;

std::queue<int> countriesQueue[40];

int checkpoint = 0;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), LightPos(1.2f, 5.0f, 2.0f)
{

}

Game::~Game()
{

}

void Game::Init()
{
    //seed random country gen
    std::srand(std::time(0));

    // lighting
    glm::vec3 lightPos(1.2f, 5.0f, 2.0f);

    // build and compile shaders
    // -------------------------
    ResourceManager::LoadShader("C:/Users/Aaron/source/repos/Mapfall/Mapfall/shaders/vert.vert",
        "C:/Users/Aaron/source/repos/Mapfall/Mapfall/shaders/frag.frag", nullptr, "Country");

    // load models
    // -----------
    //ResourceManager::LoadModel("C:/Users/Aaron/source/repos/Mapfall/Mapfall/resources/Player.fbx", false, "Player");

    //Player Body
    Player = new GameObject(glm::vec3(0.0f, 0.189f, 0.0f), glm::vec3(0.03f, 0.03f, 0.03f),
        Model("player/pigeon_body.fbx"), glm::vec3(0.7f, 0.7f, 0.7f));
    //Player Wing Left
    Wings[0] = new GameObject(glm::vec3(0.0f, 4.0f, -0.1f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_wing.fbx"), glm::vec3(0.7f, 0.7f, 0.7f));
    //Player Wing Right
	Wings[1] = new GameObject(glm::vec3(-0.7f, 4.0f, 1.2f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_wing.fbx"), glm::vec3(0.7f, 0.7f, 0.7f));
	//Player Foot Left
    Feet[0] = new GameObject(glm::vec3(0.0f, 4.0f, -0.4f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_foot.fbx"), glm::vec3(1.0f, 0.5f, 0.0f));
    //Player Foot Right
	Feet[1] = new GameObject(glm::vec3(0.0f, 4.0f, 2.2f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_foot.fbx"), glm::vec3(1.0f, 0.5f, 0.0f));
    //Player Eyes
	Eyes = new GameObject(glm::vec3(-1.45f, -1.3f, -1.0f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_eyes.fbx"), glm::vec3(0.1f, 0.1f, 0.1f));
    //Player Beak
	Beak = new GameObject(glm::vec3(-2.1f, -1.0f, -0.0f), glm::vec3(0.5f, 0.5f, 0.5f),
        Model("player/pigeon_beak.fbx"), glm::vec3(1.0f, 0.5f, 0.0f));
	Wings[1]->Rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    Player->AddChild(Wings[0]);
	Player->AddChild(Wings[1]);
    Player->AddChild(Feet[0]);
	Player->AddChild(Feet[1]);
	Player->AddChild(Eyes);
	Player->AddChild(Beak);
    

    cameraOffset = glm::vec3(0.0f, 1.5f, 2.0f) + Player->Position;

    CreateCountries();

    //text renderer
    TopLeftText = new TextRenderer(this->Width, this->Height);
    TopLeftText->Load("C:/Users/Aaron/source/repos/Mapfall/Mapfall/fonts/arial.ttf", 24);
	TopRightText = new TextRenderer(this->Width, this->Height);
    TopRightText->Load("C:/Users/Aaron/source/repos/Mapfall/Mapfall/fonts/arial.ttf", 24);
	CenterText = new TextRenderer(this->Width, this->Height);
    CenterText->Load("C:/Users/Aaron/source/repos/Mapfall/Mapfall/fonts/arial.ttf", 96);
	TimeText = new TextRenderer(this->Width, this->Height);
    TimeText->Load("C:/Users/Aaron/source/repos/Mapfall/Mapfall/fonts/arial.ttf", 48);
}

float t = 0;
int f = -1;
bool pausedTime = false;

void Game::Update(float dt)
{
	
    if (State == GAME_MENU)
    {
        if (checkpoint == 1)
        {
            
            t = t + (0.25* dt);
            glm::vec3 lerp = glm::lerp(glm::vec3(0.0f, 8.0f, 0.0f), Player->Position + cameraOffset, t);
            float rotLerp = glm::lerp(-90.0f, -45.0f, t);
            camera.SetPos(lerp);
            camera.SetPitch(rotLerp);
            //cout << rotLerp << endl;
            if(lerp.y <= Player->Position.y + cameraOffset.y)
            {
                cout << "Flag 2" << endl;
                checkpoint = 2;
                camera.SetPitch(-45.0f);
                State = GAME_ACTIVE;
                for (int i = 0; i < 40; i++) { countryNumbers.push_back(i); }
                

                
            }
            //cout << lerp.x << ", " << lerp.y << ", " << lerp.z << " - Target: " << cameraOffset.x << ", " << cameraOffset.y << ", " << cameraOffset.z << endl;
        }
    }
    else if (State == GAME_ACTIVE)
    {
	    if(checkpoint == 2)
	    {

            SetCountry();
            
            checkpoint = 3;
            t = 0;
	    }
        else if (checkpoint == 3)
        {
            t = t + dt;

	        if (f != -1) {
                if (Flags[f]->Color == glm::vec3(1, 1, 0))
					Flags[f]->Color = glm::vec3(1, 1, 1);
            }
            f = GetClosestFlag();
            if (f != -1) {
                if(Flags[f]->Color == glm::vec3(1,1,1))
					Flags[f]->Color = glm::vec3(1, 1, 0);
            }

            if (t >= 14.96)
            {
                checkpoint = 4;
                pausedTime = false;
            }

        }
        else if (checkpoint == 4)
        {
            cout << "Location: " << f << endl;
            cout << "Target: " << TargetCountry << endl;

	        if (f == -1 || f != TargetCountry)
	        {
                if(f != -1)
					Flags[f]->Color = glm::vec3(1, 0, 0);
                cout << "Wrong!" << endl;
	        }
            else
            {
                Flags[f]->Color = glm::vec3(0, 1, 0);
                Countries[f]->Color = glm::vec3(0.7f, 0.7f, 0.7f);
                cout << Flags[f]->Color.x << endl;
	            cout << "Correct!" << endl;
                
            }
            t = 0;
        	checkpoint = 5;
        }
        else if (checkpoint == 5)
        {
            t = t + dt;
            float speed = 1;
            cout << to_string(t) << endl;
	        for(int i = 0; i < 40; i++)
	        {
	        	cout << to_string(i) << endl;
                if (i == TargetCountry) continue;
                Countries[i]->Position.y = Countries[i]->Position.y - (t*speed);
                
	        }

            if (t < 3) 
            {
                if (f == -1 || f != TargetCountry)
                {
                    Player->Position.y = Player->Position.y - (t*speed);
                }
            }
            else
            {
                if (f == -1 || f != TargetCountry)
                {
                    checkpoint = 7;
                }
                else
                {
                    checkpoint = 6;
                    t = 0;
                }
            }
        }
        else if (checkpoint == 6)
        {
            t = t + (0.25f * dt);
            float lerp = glm::lerp(20.0f, 0.0f, t);
            for (int i = 0; i < 40; i++)
            {
                if (i == TargetCountry) continue;
				Countries[i]->Position.y = lerp;

            }

            if(lerp < 0.001)
            {
                for (int i = 0; i < 40; i++)
                {
                    if (i == TargetCountry) continue;
                    Countries[i]->Position.y = 0.0f;

                }
                checkpoint = 2;
            }
        }
        else if (checkpoint == 7)
        {
			camera.SetPos(glm::vec3(-0.5f, 8.0f, 0.3f));
            camera.SetPitch(-89.0f);
            for (int i = 0; i < 40; i++) {
                Countries[i]->Color = glm::vec3(0.7f, 0.7f, 0.7f);
                Countries[i]->Position.y = 0.0f;
            }
            Countries[TargetCountry]->Color = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

float a = 0;
int countryFlag = 0;
bool flagPress = false;


void Game::ProcessInput(float dt, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(State == GAME_MENU)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && checkpoint == 0)
        {
            checkpoint = 1;
            cout << "Flag 1" << endl;
        }
    }
    else if (State == GAME_ACTIVE && checkpoint == 3) {

        
        
        /*
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, dt);*/
            
        int x = 0, z = 0, xRot = -1, yRot = -1;
        float speed = 1 * dt;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            z = -1;
            yRot = 90;
            //cout << "X: " << Player->Position.x << "   Z: " << Player->Position.z << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            z = 1;
            yRot = 270;
            //cout << "X: " << Player->Position.x << "   Z: " << Player->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            x = -1;
            xRot = 180;
            //cout << "X: " << Player->Position.x << "   Z: " << Player->Position.z << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            x = 1;
            xRot = 0;
            //cout << "X: " << Player->Position.x << "   Z: " << Player->Position.z << endl;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && pausedTime == false) {
            t = 14.5f;
            pausedTime = true;
        }

        if (x != 0 || z != 0) 
        {
            if (x != 0 && z != 0)
                speed = speed / 1.5;

            Player->Position.x = Player->Position.x + (speed * x);
            Player->Position.z = Player->Position.z + (speed * z);

            a = a + (10 * dt);
            if (a >= 360)
                a = 0;

            float feetX = 0.0f + cos(-a) * 0.5f;
            float feetY = -3.0f + sin(-a) * 0.5f;
            Player->UpdateChildPos(2, glm::vec2(feetX, feetY));

            feetX = 0.0f + cos(-a-180) * 0.5f;
            feetY = -3.0f + sin(-a-180) * 0.5f;
            Player->UpdateChildPos(3, glm::vec2(feetX, feetY));
        }

        if (xRot > -1)
        {
            if (yRot > -1)
            {
                if (xRot == 0 && yRot == 270)
                    xRot = 360;

                Player->Rotation.y = (xRot + yRot) / 2;
            }
            else
            {
                Player->Rotation.y = xRot;
            }
        }
        else if (yRot > -1)
        {
            Player->Rotation.y = yRot;
        }

        camera.SetPos(Player->Position + cameraOffset);
        /*
        GameObject* DebugGO = Countries[31];
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            DebugGO->Position.z = DebugGO->Position.z - 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            DebugGO->Position.z = DebugGO->Position.z + 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            DebugGO->Position.x = DebugGO->Position.x - 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            DebugGO->Position.x = DebugGO->Position.x + 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
            DebugGO->Position.y = DebugGO->Position.y + 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
            DebugGO->Position.y = DebugGO->Position.y - 0.001;
            cout << "X: " << DebugGO->Position.x << "   Y: " << DebugGO->Position.y << "   Z: " << DebugGO->Position.z  << endl;
        }
        //int n = 37;*/
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            Flags[countryFlag]->Position.z = Flags[countryFlag]->Position.z + 0.001;
            Countries[countryFlag]->CheckChildPos(0);
            cout << "X: " << Flags[countryFlag]->Position.x << "   Z: " << Flags[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            Flags[countryFlag]->Position.z = Flags[countryFlag]->Position.z - 0.001;
            Countries[countryFlag]->CheckChildPos(0);
            cout << "X: " << Flags[countryFlag]->Position.x << "   Z: " << Flags[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            Flags[countryFlag]->Position.x = Flags[countryFlag]->Position.x + 0.001;
            Countries[countryFlag]->CheckChildPos(0);
            cout << "X: " << Flags[countryFlag]->Position.x << "   Z: " << Flags[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            Flags[countryFlag]->Position.x = Flags[countryFlag]->Position.x - 0.001;
            Countries[countryFlag]->CheckChildPos(0);
            cout << "X: " << Flags[countryFlag]->Position.x << "   Z: " << Flags[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && !flagPress) {
            flagPress = true;
            countryFlag = countryFlag + 1;
            if (countryFlag > 39)
                countryFlag = 39;
            cout << "Country Flag: " << countryFlag << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && !flagPress) {
            flagPress = true;
            countryFlag = countryFlag - 1;
            if (countryFlag < 0)
                countryFlag = 0;
            cout << "Country Flag: " << countryFlag << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_RELEASE 
            && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE)
        {
            flagPress = false;
        }
        /*
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            Countries[countryFlag]->Position.z = Countries[countryFlag]->Position.z + 0.001;
            cout << "X: " << Countries[countryFlag]->Position.x << "   Z: " << Countries[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            Countries[countryFlag]->Position.z = Countries[countryFlag]->Position.z - 0.001;
            cout << "X: " << Countries[countryFlag]->Position.x << "   Z: " << Countries[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            Countries[countryFlag]->Position.x = Countries[countryFlag]->Position.x + 0.001;
            cout << "X: " << Countries[countryFlag]->Position.x << "   Z: " << Countries[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            Countries[countryFlag]->Position.x = Countries[countryFlag]->Position.x - 0.001;
            cout << "X: " << Countries[countryFlag]->Position.x << "   Z: " << Countries[countryFlag]->Position.z << endl;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && !flagPress) {
            flagPress = true;
            countryFlag = countryFlag + 1;
            if (countryFlag > 39)
                countryFlag = 39;
            cout << "Country Flag: " << countryFlag << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS && !flagPress) {
            flagPress = true;
            countryFlag = countryFlag - 1;
            if (countryFlag < 0)
                countryFlag = 0;
            cout << "Country Flag: " << countryFlag << endl;
        }
        else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_RELEASE
            && glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE)
        {
            flagPress = false;
        }*/
    }
}

void Game::Render()
{
    // don't forget to enable shader before setting uniforms
    ResourceManager::GetShader("Country").Use();
    Shader shader = ResourceManager::GetShader("Country");
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)this->Width / this->Height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightColor", 1.0f, 1.0f, 1.0f);
    shader.SetVector3f("lightPos", this->LightPos);
    shader.SetVector3f("viewPos", camera.Position);

    Player->Draw(shader);
    

    for (int i = 0; i < 40; i++) 
    {
        Countries[i]->Draw(shader);
    	Flags[i]->Draw(shader);
    }
    
    if (State == GAME_MENU)
    {
        if(checkpoint == 0)
			CenterText->RenderText("Press enter to start", Width/2 - 500.0f, Height/2 - 100.0f, 1.0f);
    }
    else if (State == GAME_ACTIVE)
    {
        TopLeftText->RenderText(Countries[TargetCountry]->model.name, 5.0f, 5.0f, 1.0f);
    	TopRightText->RenderText(to_string(size(countryNumbers)+1) + "/40", Width - 100.0f, 5.0f, 1.0f);
        if (checkpoint == 3) {
            std::stringstream time;
            time << std::fixed << std::setprecision(1) << 15 - t;
            TimeText->RenderText(time.str(), 5.0f, 25.0f, 1.0f);
        }
        else if (checkpoint == 7)
            CenterText->RenderText("Incorrect! " + Countries[TargetCountry]->model.name + " is here.", Width / 2 - 500.0f, Height / 2 - 100.0f, 1.0f);
    }
}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

int Game::GetClosestFlag()
{
    float min = 0.3f;
    int closestFlag = -1;
    bool closestFlagSet = false;
    for (int i = 0; i < 40; i++)
    {

        //cout << i << ": " << std::to_string(FlagDistance(Player->Position, Flags[i]->Position)) << endl;
	    if(FlagDistance(Player->Position, Countries[i]->GetChildPos(0)) < min)
	    {
		    if(!closestFlagSet)
		    {
                closestFlag = i;
                closestFlagSet = true;
		    }
            else if(FlagDistance(Player->Position, Countries[i]->GetChildPos(0)) < FlagDistance(Player->Position, Countries[closestFlag]->GetChildPos(0)))
            {
                closestFlag = i;
            }
	    }
    }
    return closestFlag;
}

float Game::FlagDistance(glm::vec3 pointA, glm::vec3 pointB)
{
    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2) + pow(pointA.z - pointB.z, 2));
}

void Game::SetCountry()
{
    std::random_shuffle(countryNumbers.begin(), countryNumbers.end());
    TargetCountry = countryNumbers[0];
    countryNumbers.erase(countryNumbers.begin());
}


void Game::CreateCountries()
{
    //Poland
    Countries[0] = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Poland.fbx"), glm::vec3(1.0f, 0.0f, 0.0f));
    //^ Flag
    Flags[0] = new GameObject(glm::vec3(-0.30f, -0.1f, 0.02f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Germany
    Countries[1] = new GameObject(glm::vec3(-0.15f, 0.0f, 0.15f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Germany.fbx"), glm::vec3(1.0f, 1.0f, 0.0f));
    //^ Flag
    Flags[1] = new GameObject(glm::vec3(0.63f, -0.1f, 0.35f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Czech Republic
    Countries[2] = new GameObject(glm::vec3(-0.475f, 0.0f, 0.4f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Czech Republic.fbx"), glm::vec3(0.0f, 0.0f, 1.0f));
    //^ Flag
    Flags[2] = new GameObject(glm::vec3(-0.33f, -0.1f, 0.45f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Lithuania
    Countries[3] = new GameObject(glm::vec3(0.64f, 0.0f, -0.6f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Lithuania.fbx"), glm::vec3(0.0f, 0.0f, 1.0f));
    //^ Flag
    Flags[3] = new GameObject(glm::vec3(0.45f, -0.1f, -0.65f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Latvia
    Countries[4] = new GameObject(glm::vec3(0.74f, 0.0f, -0.92f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Latvia.fbx"), glm::vec3(0.0f, 1.0f, 0.0f));
    //^ Flag
    Flags[4] = new GameObject(glm::vec3(0.79f, -0.1f, -0.91f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Estonias
    Countries[5] = new GameObject(glm::vec3(0.86f, 0.0f, -1.25f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Estonia.fbx"), glm::vec3(0.0f, 1.0f, 1.0f));
    //^ Flag
    Flags[5] = new GameObject(glm::vec3(0.98f, -0.1f, -1.14f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Belarus
    Countries[6] = new GameObject(glm::vec3(1.18f, 0.0f, -0.33f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Belarus.fbx"), glm::vec3(0.0f, 1.0f, 1.0f));
    //^ Flag
    Flags[6] = new GameObject(glm::vec3(0.99f, -0.1f, -0.26f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Kosovo
    Countries[7] = new GameObject(glm::vec3(-0.47f, 0.0f, -0.09f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Kosovo.fbx"), glm::vec3(1.0f, 0.7f, 0.0f));
    //^ Flag
    Flags[7] = new GameObject(glm::vec3(-1.21f, -0.1f, -1.78f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Ukraine
    Countries[8] = new GameObject(glm::vec3(1.62f, 0.0f, 0.63f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Ukraine.fbx"), glm::vec3(0.5f, 0.0f, 1.0f));
    //^ Flag
    Flags[8] = new GameObject(glm::vec3(1.68f, -0.1f, 0.87f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Russia
    Countries[9] = new GameObject(glm::vec3(0.885f, 0.0f, -0.386f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Russia.fbx"), glm::vec3(0.5f, 1.0f, 0.5f));
    //^ Flag
    Flags[9] = new GameObject(glm::vec3(-1.08f, -0.1f, -0.04f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Romania
    Countries[10] = new GameObject(glm::vec3(0.79f, 0.0f, 1.05f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Romania.fbx"), glm::vec3(0.5f, 1.0f, 0.5f));
    //^ Flag
    Flags[10] = new GameObject(glm::vec3(0.66f, -0.1f, 0.81f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Hungary
    Countries[11] = new GameObject(glm::vec3(0.06f, 0.0f, 0.85f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Hungary.fbx"), glm::vec3(0.5f, 0.2f, 0.5f));
    //^ Flag
    Flags[11] = new GameObject(glm::vec3(0.09f, -0.1f, 0.90f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Austria
    Countries[12] = new GameObject(glm::vec3(-0.79f, 0.0f, 0.76f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Austria.fbx"), glm::vec3(0.1f, 0.5f, 0.2f));
    //^ Flag
    Flags[12] = new GameObject(glm::vec3(-1.11f, -0.1f, 0.85f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Moldova
    Countries[13] = new GameObject(glm::vec3(1.24f, 0.0f, 0.88f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Moldova.fbx"), glm::vec3(0.9f, 0.2f, 0.1f));
    //^ Flag
    Flags[13] = new GameObject(glm::vec3(1.19f, -0.1f, 0.87f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Slovakia
    Countries[14] = new GameObject(glm::vec3(0.08f, 0.0f, 0.59f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Slovakia.fbx"), glm::vec3(0.1f, 0.7f, 0.5f));
    //^ Flag
    Flags[14] = new GameObject(glm::vec3(0.31f, -0.1f, 0.48f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Switzerland
    Countries[15] = new GameObject(glm::vec3(-0.44f, 0.0f, -0.10f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Switzerland.fbx"), glm::vec3(0.3f, 0.1f, 1.0f));
    //^ Flag
    Flags[15] = new GameObject(glm::vec3(0.64f, -0.1f, -1.09f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Netherlands
    Countries[16] = new GameObject(glm::vec3(-1.83f, 0.0f, -0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Netherlands.fbx"), glm::vec3(0.5f, 0.1f, 0.5f));
    //^ Flag
    Flags[16] = new GameObject(glm::vec3(-1.77f, -0.1f, 0.08f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Belgium
    Countries[17] = new GameObject(glm::vec3(-0.47f, 0.0f, -0.07f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Belgium.fbx"), glm::vec3(1.0f, 0.7f, 0.1f));
    //^ Flag
    Flags[17] = new GameObject(glm::vec3(1.01f, -0.1f, -0.42f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Italy
    Countries[18] = new GameObject(glm::vec3(-0.84f, 0.0f, 1.60f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Italy.fbx"), glm::vec3(0.0f, 0.7f, 0.1f));
    //^ Flag
    Flags[18] = new GameObject(glm::vec3(-0.87f, -0.1f, 1.46f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //France
    Countries[19] = new GameObject(glm::vec3(-2.31f, 0.0f, 0.91f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/France.fbx"), glm::vec3(1.0f, 0.0f, 0.1f));
    //^ Flag
    Flags[19] = new GameObject(glm::vec3(-2.39f, -0.1f, 1.19f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Spain
    Countries[20] = new GameObject(glm::vec3(-2.93f, 0.0f, 2.02f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Spain.fbx"), glm::vec3(1.0f, 0.7f, 0.5f));
    //^ Flag
    Flags[20] = new GameObject(glm::vec3(-2.89f, -0.1f, 2.1f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //UK
    Countries[21] = new GameObject(glm::vec3(-1.79f, 0.0f, -0.24f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/United Kingdom.fbx"), glm::vec3(0.0f, 0.7f, 0.7f));
    //^ Flag
    Flags[21] = new GameObject(glm::vec3(-1.0f, -0.1f, -0.58f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Denmark
    Countries[22] = new GameObject(glm::vec3(-1.18f, 0.0f, -0.75f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Denmark.fbx"), glm::vec3(0.7f, 0.7f, 0.5f));
    //^ Flag
    Flags[22] = new GameObject(glm::vec3(-1.34f, -0.1f, -0.88f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Ireland
    Countries[23] = new GameObject(glm::vec3(-3.66f, 0.0f, -0.23f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Ireland.fbx"), glm::vec3(0.0f, 0.1f, 0.5f));
    //^ Flag
    Flags[23] = new GameObject(glm::vec3(-3.85f, -0.1f, -0.28f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Portugal
    Countries[24] = new GameObject(glm::vec3(-3.56f, 0.0f, 2.06f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Portugal.fbx"), glm::vec3(1.0f, 0.7f, 0.8f));
    //^ Flag
    Flags[24] = new GameObject(glm::vec3(-3.39f, -0.1f, 1.93f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Finland
    Countries[25] = new GameObject(glm::vec3(-0.47f, 0.0f, -0.11f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Finland.fbx"), glm::vec3(0.0f, 0.7f, 0.8f));
    //^ Flag
    Flags[25] = new GameObject(glm::vec3(-1.68f, -0.1f, 1.32f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Sweden
    Countries[26] = new GameObject(glm::vec3(-0.22f, 0.0f, -1.86f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Sweden.fbx"), glm::vec3(1.0f, 0.0f, 0.8f));
    //^ Flag
    Flags[26] = new GameObject(glm::vec3(-0.28f, -0.1f, -2.31f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Norway
    Countries[27] = new GameObject(glm::vec3(-1.12f, 0.0f, -2.12f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Norway.fbx"), glm::vec3(1.0f, 0.7f, 0.0f));
    //^ Flag
    Flags[27] = new GameObject(glm::vec3(-1.15f, -0.1f, -2.52f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Slovenia
    Countries[28] = new GameObject(glm::vec3(-0.46f, 0.0f, -0.11f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Slovenia.fbx"), glm::vec3(1.0f, 0.3f, 0.0f));
    //^ Flag
    Flags[28] = new GameObject(glm::vec3(-0.3f, -0.1f, -1.25f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Bosnia
    Countries[29] = new GameObject(glm::vec3(-0.46f, 0.0f, -0.10f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Bosnia & Herzegovina.fbx"), glm::vec3(0.0f, 0.7f, 0.0f));
    //^ Flag
    Flags[29] = new GameObject(glm::vec3(-0.82f, -0.1f, -1.6f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Croatia
    Countries[30] = new GameObject(glm::vec3(-0.46f, 0.0f, -0.12f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Croatia.fbx"), glm::vec3(0.0f, 0.7f, 0.3f));
    //^ Flag
    Flags[30] = new GameObject(glm::vec3(-0.54f, -0.1f, -1.28f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Serbia
    Countries[31] = new GameObject(glm::vec3(-0.48f, 0.0f, -0.10f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Serbia.fbx"), glm::vec3(1.0f, 0.4f, 0.2f));
    //^ Flag
    Flags[31] = new GameObject(glm::vec3(-1.12f, -0.1f, -1.45f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Montenegro
    Countries[32] = new GameObject(glm::vec3(-0.48f, 0.0f, -0.07f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Montenegro.fbx"), glm::vec3(1.0f, 0.0f, 0.2f));
    //^ Flag
    Flags[32] = new GameObject(glm::vec3(-0.98f, -0.1f, -1.76f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Bulgaria
    Countries[33] = new GameObject(glm::vec3(0.86f, 0.0f, 1.57f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Bulgaria.fbx"), glm::vec3(1.0f, 0.4f, 0.0f));
    //^ Flag
    Flags[33] = new GameObject(glm::vec3(1.07f, -0.1f, 1.57f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Albania
    Countries[34] = new GameObject(glm::vec3(-0.48f, 0.0f, -0.07f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Albania.fbx"), glm::vec3(1.0f, 0.0f, 0.8f));
    //^ Flag
    Flags[34] = new GameObject(glm::vec3(-1.04f, -0.1f, -2.02f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Macedonia
    Countries[35] = new GameObject(glm::vec3(-0.46f, 0.0f, -0.09f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/North Macedonia.fbx"), glm::vec3(0.0f, 0.0f, 0.8f));
    //^ Flag
    Flags[35] = new GameObject(glm::vec3(-1.23f, -0.1f, -1.91f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Greece
    Countries[36] = new GameObject(glm::vec3(-0.47f, 0.0f, -0.10f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Greece.fbx"), glm::vec3(0.4f, 0.0f, 0.5f));
    //^ Flag
    Flags[36] = new GameObject(glm::vec3(-1.50f, -0.1f, -2.52f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Turkey
    Countries[37] = new GameObject(glm::vec3(-0.46f, 0.0f, -0.12f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Turkey.fbx"), glm::vec3(0.0f, 1.0f, 0.8f));
    //^ Flag
    Flags[37] = new GameObject(glm::vec3(-2.76f, -0.1f, -2.27f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Iceland
    Countries[38] = new GameObject(glm::vec3(-0.71f, 0.0f, -0.27f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Iceland.fbx"), glm::vec3(0.8f, 0.0f, 0.5f));
    //^ Flag
    Flags[38] = new GameObject(glm::vec3(2.8f, -0.1f, 2.01f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));
    //Cyprus
    Countries[39] = new GameObject(glm::vec3(1.76f, 0.0f, 2.80f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("countries/Cyprus.fbx"), glm::vec3(1.0f, 0.0f, 0.6f));
    //^ Flag
    Flags[39] = new GameObject(glm::vec3(1.74f, -0.1f, 2.79f), glm::vec3(0.0001f, 0.0001f, 0.0001f),
        Model("flag/Flag1.fbx"), glm::vec3(1.0f, 1.0f, 1.0f));

    for (int i = 0; i < 40; i++)
        Countries[i]->AddChild(Flags[i]);

}
