#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <sys/types.h>
#include "include/Area_maximization_minimization.hpp"
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std::chrono;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;              // Point_2 object
typedef K::Segment_2 Segment_2;          // Segment_2 object
typedef CGAL::Polygon_2<K> Polygon_2;    // Polygon_2 object
typedef std::vector<Point_2> Points;     // vector with Point_2 objects
typedef std::vector<Segment_2> segments; // vector with Segment_2 objects
typedef std::vector<Polygon_2> Polygon_v; // vector with Polygon_2 objects

typedef CGAL::Search_traits_2<K> T;
typedef CGAL::Fuzzy_iso_box<T> box;
typedef CGAL::Kd_tree<T> tree;

typedef std::vector<double> dist; // vector with distances from a point to an edge
typedef std::vector<int> areas;   // vector with polugon areas
typedef std::vector<int> findd;   // vector with position of visible edges from an interior point(position in polygon chain)
typedef std::vector<int> Areas;
typedef std::vector<Point_2>::iterator pveciterator;  // iterator gia vector apo points
typedef std::vector<Segment_2>::iterator segiterator; // iterator gia segments
typedef std::vector<double> distance;

int flag_algo = -1;    // algorithm we choose
int flag_min_max = -1; // minimization or maximixation
int option = -1;       // global local or subdivision
int L = -1;            // path length
int flagalgo=-1;
int flaginit=-1;
int flagedge=-1;
double threshold = -1; // threshold
Polygon_2 p;           // polygon
segments chain;        // chain
Points points;         // points


int main(int argc, char *argv[])
{
    int how_many_points;
    Points result2;
    std::string line;
    auto start = high_resolution_clock::now(); // start counting execution time for the program
    std::ifstream in(argv[14]);                // input file
    std::ofstream outfile;
    outfile.open(argv[16]); // output file
    handle_input_p1(argv);//handle input for project 1
    srand((unsigned)time(NULL)); // for the random number i create for random edge selection
    std::getline(in, line); // skip the first two lines from input(we dont need them)
    std::getline(in, line);
    result2 = handleinput(in, result2); // function tha returns a vector with all the points given from the input file
    if (flagalgo == 2)  //if convex_hull
    {
        convex_hull_fun(result2,outfile);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
        outfile << "construction time: " << duration.count() << " ms " << std::endl;
    }
      
    else if (flagalgo == 1)
    {                                                                          // if incremental
        incremental_fun(result2,outfile);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = duration_cast<std::chrono::milliseconds>(stop - start);
        outfile << "construction time: " << duration.count() << " ms " << std::endl;
    }


    //--------------------------

    auto start1 = high_resolution_clock::now();    
    handle_input(argv);
    how_many_points = create_polygon(argv[2]);
    get_points(how_many_points);
    create_chain(how_many_points);
    std::ofstream outf;
    outf.open(argv[4]);
    if (flag_algo == 1)
    { // if algorithm is local search
        local_search(flag_min_max,outf);
    }
    else if (flag_algo == 2)
    { // if algorithm is simulated_annealing

        simulated_annealing(how_many_points,outf);
    }

    auto stop1 = std::chrono::high_resolution_clock::now();
    auto duration1 = duration_cast<std::chrono::milliseconds>(stop1 - start1);
    outf << "construction time: " << duration1.count() << " ms " << std::endl;

}
