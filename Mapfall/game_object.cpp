#include "game_object.h"
#include <glm/common.hpp>
#include <glm/common.hpp>
#include <glm/common.hpp>


GameObject::GameObject()
    : Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), Color(1.0f), Rotation(0.0f, 0.0f, 0.0f), model(), Children(), Parent(), HasParent(false), Destroyed(false) { }

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Model model, glm::vec3 color)
    : Position(pos), Size(size), Color(color), Rotation(0.0f, 0.0f, 0.0f), model(model), Children(), Parent(), HasParent(false), Destroyed(false) { }

void GameObject::Draw(Shader& shader)
{
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, Position);
    model_ = glm::scale(model_, Size);
    model_ = glm::rotate(model_, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
	model_ = glm::rotate(model_, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
    shader.SetMatrix4("model", model_);
	shader.SetVector3f("objectColor", Color.x, Color.y, Color.z);
    model.Draw(shader);

    if(!Children.empty())
    {
        for (Child c : Children)
        {
            glm::mat4 model_child = glm::mat4(1.0f);
            model_child = glm::translate(model_child, c.posOffset);
            //model_child = glm::scale(model_child, Size*Size);
            model_child = glm::rotate(model_child, glm::radians(c.rotOffset.y), glm::vec3(0, 1, 0));
            model_child = glm::rotate(model_child, glm::radians(c.rotOffset.x), glm::vec3(1, 0, 0));

            c.ChildGO->DrawChild(model_ * model_child, shader);
        }
    }

    
}

GameObject::Child::Child(GameObject* GO, glm::vec3 pos, glm::vec3 rot)
{
    ChildGO = GO;
    posOffset = pos;
    rotOffset = rot;
}

void GameObject::Child::UpdatePosOffset(glm::vec3 pos)
{
    posOffset = pos;
}

void GameObject::AddChild(GameObject* GO)
{
	Children.push_back(Child(GO, Position - GO->Position, Rotation - GO->Rotation));
}

void GameObject::UpdateChildPos(int c, glm::vec2 pos)
{
    Children[c].posOffset = glm::vec3(pos.x, pos.y, Children[c].posOffset.z);//Position - Children[c].ChildGO->Position;
}

void GameObject::CheckChildPos(int c)
{
    Children[c].posOffset = Position - Children[c].ChildGO->Position;
}

glm::vec3 GameObject::GetChildPos(int c)
{
    return Position + Children[c].posOffset;
}


void GameObject::DrawChild(glm::mat4 model_, Shader& shader)
{
    shader.SetMatrix4("model", model_);
    shader.SetVector3f("objectColor", Color.x, Color.y, Color.z);
    model.Draw(shader);
}
