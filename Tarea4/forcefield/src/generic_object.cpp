//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"

Object::Object(const Door_detector::Door &d)
{
    obj.id = 81;
    obj.x = d.pcenter.x();
    obj.y = d.pcenter.y();
    obj.z = 0;
}

Object::Object(const RoboCompYoloObjects::TBox &Tbox)
{
    obj.id = Tbox.id;
    obj.type = Tbox.type;
    obj.left = Tbox.left;
    obj.top = Tbox.top;
    obj.right = Tbox.right;
    obj.bot = Tbox.bot;
    obj.score = Tbox.score;
    obj.depth = Tbox.depth;
    obj.x = Tbox.x;
    obj.y = Tbox.y;
    obj.z = Tbox.z;
}

std::vector<Object> Object::createList(const std::vector<RoboCompYoloObjects::TBox> &tObjects)
{
    std::vector<Object> objects;
    for(const auto t: tObjects)
        objects.emplace_back(Object(t));

    return objects;

}

std::vector<Object> Object::createList(const std::vector<Door_detector::Door> &doors){

    std::vector<Object> objects;
    for(const auto d: doors)
        objects.emplace_back(Object(d));

    return objects;
}

std::vector<Object> Object::createFinalList(std::vector<Object> doors, std::vector<Object> tObjects)
{
    std::vector<Object> objects;

    for(const auto o: doors)
        objects.emplace_back(Object(o));

    for(const auto t: tObjects)
        objects.emplace_back(Object(t));

    return objects;
}

std::tuple<float, float> Object::getObjectCoords()
{
    return std::make_tuple(obj.x, obj.y);
}

const Object::Obj Object::get_obj() { return obj; }