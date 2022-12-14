//
// Created by alumno on 13/12/22.
//

#ifndef FORCEFIELD_GRAPH_H
#define FORCEFIELD_GRAPH_H

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

class Graph {
private:
    int graphSize;
    std::vector<int> nodeArray;
    std::vector<int> edge1Array;
    std::vector<int> edge2Array;
    void addEdge(int node1, int node2);
    void deleteEdge(int node1, int node2);
public:
    void addNode(int node);
    void addUEdge(int node1, int node2);
    int connectedComponents(void);
    int checkNode(int node);
    int checkEdge(int node1 , int node2);
    void deleteNode(int node);
    void deleteUEdge(int node1, int node2);
    void printNodeList(void);
    Graph(int size); //Constructor of the class
};


#endif //FORCEFIELD_GRAPH_H
