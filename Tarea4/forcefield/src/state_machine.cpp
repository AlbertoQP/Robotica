//
// Created by alumno on 13/12/22.
//

#include "state_machine.h"
#include "specificworker.h"

int UMBRAL_OBJETIVO = 520;

void State_machine::statemachine(const std::vector<Object> &objects, const rc::Robot robot)
{
    switch (state)
    {
        case State::IDLE:
            state = State::SEARCHING;
            break;
        case State::SEARCHING:
            search_state(objects, robot);
            break;
        case State::APPROACHING:
            approach_state(objects, robot);
            break;
    }
}

void State_machine::search_state(const std::vector<Object> &objects, rc::Robot robot)
{
    // SEARCHING STATE
    std::cout << "Searching" << std::endl;

    if(auto it = std::find_if_not(objects.begin(), objects.end(),
                                  [r = robot](Object a){return a.get_obj().type == r.get_current_target_obj().get_obj().type;}); it != objects.end())
    {
        robot.set_current_target_obj(*it); // Selecciona el objetivo
        state = State::APPROACHING; // Cambia al estado de acercamiento al objetivo
    }
    robot.set_pure_rotation(0.7f);
}

void State_machine::approach_state(const std::vector<Object> &objects, rc::Robot robot)
{
    // APPROACH STATE
    std::cout << "Approach State" << std::endl;
    robot.set_pure_rotation(0.0f);

    if(robot.get_distance_to_target() < UMBRAL_OBJETIVO) {
        robot.set_current_advance_speed(0.0f);
        state = State::SEARCHING; // Cambia al estado de búsqueda
    }
    else
    if(auto it = std::find_if(objects.begin(), objects.end(),
                              [r = robot](Object a){return a.get_obj().type == r.get_current_target().type;}); it != objects.end())
    {
        robot.set_current_target_obj(*it); // Actualiza el objetivo actual
    }
}
