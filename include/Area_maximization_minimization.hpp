#include <cstdlib>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/draw_polygon_2.h>

#include <time.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;     // antikeimeno tupou point
typedef K::Segment_2 Segment_2; // antikeimeno typou pleuras 
typedef CGAL::Polygon_2<K> Polygon_2;
typedef std::vector<Point_2> Points;                    // vector me stoixeia point
typedef std::vector<Segment_2> segments;                // vector me stoixeia pleurwn
typedef std::vector<int> Areas;

typedef CGAL::Search_traits_2<K> T;
typedef CGAL::Fuzzy_iso_box<T> box;
typedef CGAL::Kd_tree<T> tree;

extern int flag_algo;    // algorithm we choose
extern int flag_min_max; // minimization or maximixation
extern int option;       // global local or subdivision
extern int L;            // path length
extern double threshold; // threshold
extern Polygon_2 p; // polygon
extern segments chain;  // chain
extern Points points;   // points


void handle_input(char **);
int create_polygon(char *);
void create_chain(int);
void get_points(int);
void local_search(void);
void simulated_annealing(int);
int find_intersection(Segment_2,Segment_2,Segment_2);
int find_intersection_1(Segment_2,Segment_2);
void create_new_polygon(void);
int find_intersection_2(Segment_2);