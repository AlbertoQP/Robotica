//
// Created by alumno on 14/12/22.
//

#include "generic_object.h"

Object::Object(const Door_detector::Door &d)
{
    id = 81;
    x = d.pcenter.x();
    y = d.pcenter.y();
    z = 0;
}

Object::Object(const RoboCompYoloObjects::TBox &Tbox)
{
     id = Tbox.id;
     type = Tbox.type;
     left = Tbox.left;
     top = Tbox.top;
     right = Tbox.right;
     bot = Tbox.bot;
     score = Tbox.score;
     depth = Tbox.depth;
     x = Tbox.x;
     y = Tbox.y;
     z = Tbox.z;

}
Object::Object() {}

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
    return std::make_tuple(x, y);
}