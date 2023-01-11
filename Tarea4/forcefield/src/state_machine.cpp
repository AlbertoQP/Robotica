//
// Created by alumno on 13/12/22.
//

#include "state_machine.h"
#include "specificworker.h"

int UMBRAL_OBJETIVO = 150;

void State_machine::statemachine(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    switch (state)
    {
        case State::IDLE:
            // GO TO IDLE STATE
            break;
        case State::SEARCHING:
            search_state(objects, robot);
            break;
        case State::APPROACHING:
            approach_state(objects, robot);
            break;
        // ADD CROSS STATE
    }
}

void State_machine::search_state(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    // SEARCHING STATE
    std::cout << "Searching" << std::endl;

    if(auto it = std::ranges::find_if_not(objects.begin(), objects.end(),
                                  [r = robot](auto &a){return a.type == r.get_current_target().type;}); it != objects.end() && it->type == 80)
    {
        robot.set_current_target(*it); // Selecciona el objetivo
        state = State::APPROACHING; // Cambia al estado de acercamiento al objetivo
        std::cout << "Nuevo target: " << robot.get_current_target().type;
    }
    else
        robot.set_pure_rotation(0.7f);
}

void State_machine::approach_state(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    // APPROACH STATE
    std::cout << "Approach State" << std::endl;
    robot.set_pure_rotation(0.0f);

    if (robot.get_distance_to_target() < UMBRAL_OBJETIVO)
    {
        // CROSS STATE
        std::cout << "Crossing" << std::endl;
    }
    else
    if(auto it = std::ranges::find_if(objects.begin(), objects.end(),
                              [r = robot](auto &a){return a.type == r.get_current_target().type;}); it != objects.end())
    {
        robot.set_current_target(*it); // Actualiza el objetivo actual
        std::cout << "Nuevo target: " << robot.get_current_target().type;
    }
}
