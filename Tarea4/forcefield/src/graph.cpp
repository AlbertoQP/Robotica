#include <graph.h>
#include <cppitertools/enumerate.hpp>

int Graph::add_node()
{
    Node node = Node(id_counter);
    nodes.insert({id_counter, node});
    return id_counter;
}

int Graph::add_node(int node_dest)
{
    Node node = Node(node_dest);
    nodes.insert({node_dest, node});

    add_edge(id_counter, node_dest);
    id_counter = node_dest;

    return node_dest;
}

void Graph::add_edge(int n1, int n2)
{
    Edge edge = Edge(n1, n2);
    std::pair<int, int> aux = std::make_pair(n1, n2);
    edges.insert({aux, edge});
}

void Graph::add_tags(int id, const std::vector<rc::PreObject> &preobjects)
{
    if(nodes.contains(id))
    {
        for(const auto &po: preobjects)
        {
            nodes.at(id).objects.insert(po.label);
        }
    }
}

void Graph::show()
{
    std::cout << "----------------------------------------------------------------" << std::endl;

    for(const auto &graph_node : nodes)
    {
        int id = graph_node.first;
        std::cout << "Node: " << id << std::endl;

        for(const auto &graph_edge : edges)
        {
            if (graph_edge.first.first == id)
            {
                std::cout << "Enlaces del nodo: " << std::endl;
                std::cout << "Origen: " << graph_edge.first.first << std::endl;
                std::cout << "Destino: " << graph_edge.first.second << std::endl << std::endl;
            }
        }
    }

    std::cout << "----------------------------------------------------------------" << std::endl;
}