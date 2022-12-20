//
// Created by alumno on 14/12/22.
//

#ifndef FORCEFIELD_GENERIC_OBJECT_H
#define FORCEFIELD_GENERIC_OBJECT_H


#include "robot.h"
#include "door_detector.h"

class Object {
    struct Obj {
    int id;
    int type;
    int left;
    int top;
    int right;
    int bot;
    float score;
    float depth;
    float x;
    float y;
    float z;
};

private:
    Obj obj;
public:
    Object(const Door_detector::Door &d);
    Object(const RoboCompYoloObjects::TBox &Tbox);
    Object();

    static std::vector<Object> createList(const std::vector<RoboCompYoloObjects::TBox> &objects);
    static std::vector<Object> createList(const std::vector<Door_detector::Door> &doors);
    static std::vector<Object> createFinalList(std::vector<Object> doors, std::vector<Object> tobjects);
    const Obj get_obj();
    std::tuple<float, float> getObjectCoords();
};

#endif //FORCEFIELD_GENERIC_OBJECT_H
