//
// Created by alumno on 13/12/22.
//

#ifndef FORCEFIELD_STATE_MACHINE_H
#define FORCEFIELD_STATE_MACHINE_H

#include <Eigen/Dense>
#include <QtGui>
#include <QGraphicsItem>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>
#include <genericworker.h>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>
#include <timer/timer.h>
#include <Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <dynamic_window.h>
#include <timer/timer.h>
#include "robot.h"
#include "camera.h"
#include "preobject.h"
#include "graph.h"

class State_machine {
private:
    bool first_time = true;
    int actual_node = 0;
public:
    enum class State {IDLE, SEARCHING, APPROACHING, CROSSING};
    State state = State::SEARCHING;
    void statemachine(const std::vector<rc::PreObject> objects, rc::Robot &robot, Graph &graph);

    void search_state(const std::vector<rc::PreObject> objects, rc::Robot &robot, Graph &graph);
    void approach_state(const std::vector<rc::PreObject> objects, rc::Robot &robot);
    void cross_state(rc::Robot &robot, Graph &graph);
    void idle_state(Graph &graph);
};


#endif //FORCEFIELD_STATE_MACHINE_H
