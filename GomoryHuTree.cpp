#include <bits/stdc++.h>

using namespace std;

// Number of vertices
const int sz = 9;
// Adjacency matrix of the graph (specified as a constant for better visual perception).
// Nevertheless, a "vector" by "vector" is passed to the tree-building function for universality.
const int graph[sz][sz] =
    {
        {0,2,4,0,0,0,1,0,0},
        {2,0,6,11,0,0,0,0,0},
        {4,6,0,0,9,0,0,0,0},
        {0,11,0,0,7,0,2,0,0},
        {0,0,9,7,0,9,3,1,0},
        {0,0,0,0,9,0,0,8,0},
        {1,0,0,2,3,0,0,9,4},
        {0,0,0,0,1,8,9,0,3},
        {0,0,0,0,0,0,4,3,0}
    };

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Gomory-Hu
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct Edge;
struct Vertex;

typedef vector<int> Row;             // A row in the adjacency matrix
typedef vector<Row> Matrix;          // Adjacency matrix
typedef vector<Vertex *> VertexList; // List of vertices
typedef vector<Edge *> EdgeList;     // List of edges
vector<pair<string,string>> cutEdges;

// ----------------------------------------------------------------------------

// Vertex or a group of vertices

struct Vertex
{
    int id;           // Vertex identifier, used for displaying the algorithm's log and sorting vertices in the final tree
    VertexList group; // Vertices in the group (empty for a simple vertex)
    EdgeList edges;   // List of edges

    int flag;       // Auxiliary fields used in searches
    Vertex *parent; // Used in breadth-first search and elsewhere for simplifying operations

    // Constructors for a simple vertex and a group of vertices
    Vertex(int id) : id(id) {}
    Vertex(const VertexList &group) : id(-1), group(group) {}
};

// ----------------------------------------------------------------------------

// Edge (since edges are stored in vertex lists, only the second vertex is indicated;
// correspondingly, the other vertex points to a similar edge in the opposite direction)

struct Edge
{
    Vertex *vertex; // Adjacent vertex
    int c;          // Capacity
    int f;          // Flow

    Edge(Vertex *vertex, int c) : vertex(vertex), c(c), f(0) {}
};

// ----------------------------------------------------------------------------

// Result of finding the maximum flow

struct MinCut
{
    int f;        // Maximum flow between s and t
    Vertex *s;    // Pointer to s
    Vertex *t;    // Pointer to t
    VertexList A; // "Cut" half containing s
    VertexList B; // "Cut" half containing t

    MinCut(Vertex *s, Vertex *t) : s(s), t(t), f(0) {}
};


// ----------------------------------------------------------------------------

// Find a vertex in a list, returning an iterator (pointer)

VertexList::iterator findVertex(VertexList &set, Vertex *v)
{
    for (VertexList::iterator i = set.begin(); i != set.end(); ++i)
    {
        if (*i == v)
        {
            return i;
        }
    }
    return set.end();
}

// ----------------------------------------------------------------------------

// Set difference

VertexList setMinus(VertexList &set1, VertexList &set2)
{
    VertexList result;
    for (Vertex *v : set1)
    {
        // If a vertex from the left set is not found in the right set, add it to the result
        if (findVertex(set2, v) == set2.end())
        {
            result.push_back(v);
        }
    }
    return result;
}

// ----------------------------------------------------------------------------

// Set intersection

VertexList setMul(VertexList &set1, VertexList &set2)
{
    VertexList result;
    for (Vertex *v : set2)
    {
        // If a vertex from the right set is found in the left set, add it to the result
        if (findVertex(set1, v) != set1.end())
        {
            result.push_back(v);
        }
    }
    return result;
}

// ----------------------------------------------------------------------------

// Find an edge in a list, returning an iterator (pointer)

EdgeList::iterator findEdge(Vertex *v1, Vertex *v2)
{
    for (EdgeList::iterator i = v1->edges.begin(); i != v1->edges.end(); ++i)
    {
        if ((*i)->vertex == v2)
        {
            return i;
        }
    }
    return v1->edges.end();
}

// ----------------------------------------------------------------------------

// Create an edge V1 --[c]--> V2
// If addReverse == true, V2 --[c]--> V1 is also created
// If the edge already exists, c is added to its weight

void addEdge(Vertex *v1, Vertex *v2, const int c, bool addReverse = true)
{
    EdgeList::iterator i = findEdge(v1, v2);
    if (i != v1->edges.end())
    {
        (*i)->c += c;
    }
    else
    {
        v1->edges.push_back(new Edge(v2, c));
    }

    if (!addReverse)
    {
        return;
    }

    i = findEdge(v2, v1);
    if (i != v2->edges.end())
    {
        (*i)->c += c;
    }
    else
    {
        v2->edges.push_back(new Edge(v1, c));
    }
}

// ----------------------------------------------------------------------------

// Get the edge (if it exists) from V1 to V2

Edge *getEdge(Vertex *v1, Vertex *v2)
{
    EdgeList::iterator i = findEdge(v1, v2);
    return (i == v1->edges.end() ? nullptr : *i);
}

// ----------------------------------------------------------------------------

// Delete the edge from V1 to V2

void deleteEdge(Vertex *v1, Vertex *v2)
{
    EdgeList::iterator i = findEdge(v1, v2);
    if (i == v1->edges.end())
    {
        return;
    }

    delete (*i);
    v1->edges.erase(i);
}

// ----------------------------------------------------------------------------





int main(int argc, char *argv[])
{
    // Check if the graph is undirected
    for (int j = 0; j < sz; j++)
    {
        if (graph[j][j] != 0)
        {
            cout << "Error: Loop in vertex " << j << endl;
            return 0;
        }

        for (int i = 0; i < j; i++)
        {
            if (graph[i][j] != graph[j][i])
            {
                cout << "Error: Different weights in edge " << i << "-" << j << endl;
                return 0;
            }

            if (graph[i][j] < 0)
            {
                cout << "Error: Negative weight in edge " << i << "-" << j << endl;
                return 0;
            }
        }
    }

    Matrix g(sz, Row(sz));
    for (int i = 0; i < sz; i++)
    {
        for (int j = 0; j < sz; j++)
        {
            g[i][j] = graph[i][j];
        }
    }

    Matrix m = buildGomoryHuTree(g);
    findMinKCut(m,3);
    findMinCut_(m,0,2);

    // cout << cutEdges.size() << endl;
    // for(int i=0;i<cutEdges.size();i++){
        
    //     cout << "[ " <<  cutEdges[i].first << " , " << cutEdges[i].second << " ]" << endl;
    // }

    return 0;
}
