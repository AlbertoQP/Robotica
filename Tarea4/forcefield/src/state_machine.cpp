//
// Created by alumno on 13/12/22.
//

#include "state_machine.h"
#include "specificworker.h"

int UMBRAL_OBJETIVO = 100;

void State_machine::statemachine(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    switch (state)
    {
        case State::IDLE:
            first_time = true;
            state = State::SEARCHING;
            break;
        case State::SEARCHING:
            search_state(objects, robot);
            break;
        case State::APPROACHING:
            approach_state(objects, robot);
            break;
        case State::CROSSING:
            cross_state(robot);
            break;
    }
}

void State_machine::search_state(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    // SEARCHING STATE
    std::cout << "Searching" << std::endl;
    std::cout << objects.size() << std::endl;

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
        state = State::CROSSING;
    }
    else
    if(auto it = std::ranges::find_if(objects.begin(), objects.end(),
                              [r = robot](auto &a){return a.type == r.get_current_target().type;}); it != objects.end())
    {
        robot.set_current_target(*it); // Actualiza el objetivo actual
        std::cout << "Nuevo target: " << robot.get_current_target().type << "; ";
    }
}
void State_machine::cross_state(rc::Robot &robot)
{
    std::cout << "Crossing" << std::endl;

    static std::chrono::time_point<std::chrono::system_clock> start_chrono;
    //static bool first_time = true;

    if (first_time)
    {
        start_chrono = std::chrono::system_clock::now();
        first_time = false;
    }

    auto end_chrono = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> duration = end_chrono - start_chrono;

    if (duration.count() > 1600)
    {
        robot.set_has_target(false);
        state = State::SEARCHING;
        first_time = true;
    }
}

