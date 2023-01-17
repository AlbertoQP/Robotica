//
// Created by alumno on 13/12/22.
//

#ifndef FORCEFIELD_GRAPH_H
#define FORCEFIELD_GRAPH_H

#include <Eigen/Dense>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>
#include <preobject.h>

class Graph
{
public:
    Graph() = default;
    struct Node
    {
        Node(int id_) : id(id_){};
        int id;
        std::set<std::string> objects;
        QPointF draw_pos{0.f, 0.f};
    };
    struct Edge
    {
        Edge(int n1_, int n2_) : n1(n1_), n2(n2_){};
        int n1, n2;
    };
    int add_node();
    int add_node(int node_dest);
    void add_edge(int n1, int n2);
    void add_tags(int id, const std::vector<rc::PreObject> &objects);
    bool isEmpty();
    void remove_duplicate_node(int id);
    void show();

    //void draw(AbstractGraphicViewer *viewer);

private:
    int id_counter = 0;
    std::map<int, Node> nodes;
    std::map<std::pair<int, int>,  Edge> edges;

};

#endif //FORCEFIELD_GRAPH_H
