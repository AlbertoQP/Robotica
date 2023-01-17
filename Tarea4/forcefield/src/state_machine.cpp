//
// Created by alumno on 13/12/22.
//

#include "state_machine.h"
#include "specificworker.h"

int UMBRAL_OBJETIVO = 600; //550

void State_machine::statemachine(const std::vector<rc::PreObject> objects, rc::Robot &robot, Graph &graph)
{
    switch (state)
    {
        case State::IDLE:
            idle_state(graph);
            break;
        case State::SEARCHING:
            search_state(objects, robot, graph);
            break;
        case State::APPROACHING:
            approach_state(objects, robot);
            break;
        case State::CROSSING:
            cross_state(robot, graph);
            break;
    }
}

void State_machine::idle_state(Graph &graph)
{
    first_time = true;
    actual_node = graph.add_node();
    state = State::SEARCHING;
}

void State_machine::search_state(const std::vector<rc::PreObject> objects, rc::Robot &robot, Graph &graph)
{
    // SEARCHING STATE
    std::cout << "Search State" << std::endl;
    std::cout << objects.size() << std::endl;

    graph.add_tags(actual_node, objects);

    if(auto it = std::ranges::find_if_not(objects.begin(), objects.end(),
                                  [r = robot](auto &a){return a.type == r.get_current_target().type;}); it != objects.end() && it->type == 80)
    {
        robot.set_current_target(*it); // Selecciona el objetivo
        state = State::APPROACHING; // Cambia al estado de acercamiento al objetivo
        //std::cout << "Nuevo target: " << robot.get_current_target().type;
    }
    else
        robot.set_pure_rotation(0.4f);
}

void State_machine::approach_state(const std::vector<rc::PreObject> objects, rc::Robot &robot)
{
    // APPROACH STATE
    std::cout << "Approach State" << std::endl;
    robot.set_pure_rotation(0.0f);
    //std::cout << "Distancia al target: " << robot.get_distance_to_target() << std::endl;

    if (robot.get_distance_to_target() < UMBRAL_OBJETIVO)
    {
        state = State::CROSSING;
    }
    else
    if(auto it = std::ranges::find_if(objects.begin(), objects.end(),
                              [r = robot](auto &a){return a.type == r.get_current_target().type;}); it != objects.end())
    {
        robot.set_current_target(*it); // Actualiza el objetivo actual
        //std::cout << "Nuevo target: " << robot.get_current_target().type << "; ";
    }
}
void State_machine::cross_state(rc::Robot &robot, Graph &graph)
{
    std::cout << "Crossing" << std::endl;

    static std::chrono::time_point<std::chrono::system_clock> start_chrono;

    if (first_time)
    {
        start_chrono = std::chrono::system_clock::now();
        first_time = false;
    }

    auto end_chrono = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> duration = end_chrono - start_chrono;

    if (duration.count() > 1650)//1600
    {
        actual_node = graph.add_node(actual_node + 1);
        graph.show();

        robot.set_has_target(false);
        state = State::SEARCHING;
        first_time = true;
    }
}

