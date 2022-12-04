#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <CGAL/draw_polygon_2.h>
#include "include/Area_maximization_minimization.hpp"
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2; //Point_2 object
typedef K::Segment_2 Segment_2; //Segment_2 object
typedef CGAL::Polygon_2<K> Polygon_2; //Polygon_2 object 
typedef std::vector<Point_2> Points;  //vector with Point_2 objects
typedef std::vector<Segment_2> segments;  //vector with Segment_2 objects
typedef std::vector<int> Areas;

typedef CGAL::Search_traits_2<K> T;
typedef CGAL::Fuzzy_iso_box<T> box;
typedef CGAL::Kd_tree<T> tree;

int flag_algo=-1;    // algorithm we choose
int flag_min_max=-1; // minimization or maximixation
int option=-1;       // global local or subdivision
int L=-1;            // path length
double threshold=-1;   // threshold
Polygon_2 p;    // polygon
segments chain; // chain
Points points;  // points
int main(int argc,char * argv[]){
    int how_many_points;
    handle_input(argv);
    how_many_points=create_polygon(argv[2]);
    get_points(how_many_points);
    create_chain(how_many_points);
    if(flag_algo==1){//if algorithm is local search
        
        local_search();

    }
    else if(flag_algo==2){//if algorithm is simulated_annealing
                            CGAL::draw(p);

        simulated_annealing(how_many_points);

    }
                    CGAL::draw(p);

}
