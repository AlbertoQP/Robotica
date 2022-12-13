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

class State_machine {
public:
    enum class State {IDLE, SEARCHING, APPROACHING, WAITING};
    State state = State::SEARCHING;
    void statemachine(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line, const rc::Robot robot);
    void search_state(const RoboCompYoloObjects::TObjects &objects, rc::Robot robot);
    void approach_state(const RoboCompYoloObjects::TObjects &objects, const std::vector<Eigen::Vector2f> &line, rc::Robot robot);
};


#endif //FORCEFIELD_STATE_MACHINE_H
