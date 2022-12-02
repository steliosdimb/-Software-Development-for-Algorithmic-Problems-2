#include <cstdlib>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/convex_hull_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;     // antikeimeno tupou point
typedef K::Segment_2 Segment_2; // antikeimeno typou pleuras 
typedef CGAL::Polygon_2<K> Polygon_2;
typedef std::vector<Point_2> Points;                    // vector me stoixeia point
typedef std::vector<Segment_2> segments;                // vector me stoixeia pleurwn
typedef std::vector<int> Areas;


extern int flag_algo;    // algorithm we choose
extern int flag_min_max; // minimization or maximixation
extern int option;       // global local or subdivision
extern int L;            // path length
extern double threshold; // threshold
extern Polygon_2 p; // polygon
extern segments chain;  // chain
 
void handle_input(char **);
int create_polygon(char *);
void create_chain(int);
void local_search(void);
void simulated_annealing(void);