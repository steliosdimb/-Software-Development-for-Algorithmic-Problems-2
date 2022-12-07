#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_iso_box.h>
#include <CGAL/draw_polygon_2.h>
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
double threshold = -1; // threshold
Polygon_2 p;           // polygon
segments chain;        // chain
Points points;         // points
int main(int argc, char *argv[])
{
    auto start = high_resolution_clock::now(); // start counting execution time for the program
    std::ifstream in(argv[14]);                // input file
    std::ofstream outfile;
    outfile.open(argv[16]); // output file
    int flagalgo;
    int flaginit;
    if (strcmp("incremental", argv[18]) == 0)
    {
        flagalgo = 1; // if -algorithm=incremental
        if (strcmp("1a", argv[22]) == 0)
            flaginit = 1;
        else if (strcmp("1b", argv[22]) == 0)
            flaginit = 2;
        else if (strcmp("2a", argv[22]) == 0)
            flaginit = 3;
        else if (strcmp("2b", argv[22]) == 0)
            flaginit = 4;
    }
    else if (strcmp("convex_hull", argv[18]) == 0)
    {
        flagalgo = 2; // if -algorithm=convex_hull
    }
    int flagedge;
    if (strcmp("1", argv[20]) == 0)
    {
        flagedge = 1; // if -edgeselection=random
    }
    else if (strcmp("2", argv[20]) == 0)
    {
        flagedge = 2; // if -edgeselection=min
    }
    else
    {
        flagedge = 3; // if -edgeselection=max
    }
    srand((unsigned)time(NULL)); // for the random number i create for random edge selection
    int j;
    int i;
    int e;
    int k;                // variables for for and while
    int random;           // random number i create for random edge selection
    int counterr;         // using this counter to check each visible edge in min max edge selection
    int numberofsegments; // number of segments
    int pointcount;       // if its 0 then i found a visible edge for this point if its more than zero then i went to the next closest point of an edge to searh visible edges
    int flag1;            // value 0 if the interior point is inside the polygon and 1 if outside the polygon
    int pos;              // position of an edge in the polygon chain
    int index;            // position of an edge in the polygon chain
    double area;          // area of a polygon
    double re;            // area of a polygon
    double conarea;       // convec hull area
    Points result;        // init vector with convex hull's points
    Points result1;       // init vector with interior points
    Points result2;       // init vector with every point given from input
    Points keep;          // init vector that keeps the temporary pollygon i make
    segments chain1;      // init vector that keeps the polygon chain
    segments temp;        // init vector with temporary edges needed
    Points temppoint;
    Points convex_hull;
    segments convex_seg;
    pveciterator p1;
    pveciterator p2;
    Polygon_2 poly;   // init my polygon
    segments visible; // init vector that keeps the visible edges
    dist dis;         // init vector that keeps the distances between interior point and edges
    areas areas;      // init vector with area of polygons
    findd finde;      // init vector with position of visible edges from an interior point(position in polygon chain)
    std::string line;
    std::getline(in, line); // skip the first two lines from input(we dont need them)
    std::getline(in, line);
    result2 = handleinput(in, result2); // function tha returns a vector with all the points given from the input file
    if (flagalgo == 2)
    {
        std::copy(result2.begin(), result2.end(), std::back_inserter(result1));          // creating a copy of vector result2
        CGAL::convex_hull_2(result2.begin(), result2.end(), std::back_inserter(result)); // geting the convex hull from all the points i have
        CGAL::area_2(result.begin(), result.end(), conarea, K());
        for (j = 0; j < result.size(); j++)
        {
            result1.erase(std::remove(result1.begin(), result1.end(), result[j]), result1.end()); // erase the points from the convex hull so i can keep the interior points
        }
        numberofsegments = result.size();
        for (i = 0; i < numberofsegments - 1; i++)
        {
            chain1.push_back(Segment_2(result[i], result[i + 1])); // initialize the polugon chain,polugon chain should be equal with the convex hull at first
        }
        chain1.push_back(Segment_2(result[i], result[0]));
        for (i = 0; i < result.size(); i++)
        {
            poly.push_back(result[i]); // initializing polygon
        }

        pointcount = 0;             // we are looking for the closest interior point from an edge
        while (result1.size() != 0) // while my polygon does not contain every interior point
        {
            Point_2 t; // t is the closest point from an edge
            i = 0;
            if (pointcount != 0)
            {
                t = pointdistance1(result1, chain1, dis, pointcount); // couldnt find visible edge from this point find the next closest point
            }
            else
            {
                t = pointdistance(result1, chain1, dis); // function returns closest point from an edge
            }
            if (flagedge == 1) // if edge selection random is selected
            {
                flag1 = 0;
                visible = findvisible(t, temp, chain1, visible); // function returns visible edges from the interior point t
                while (visible.size() != 0)                      // check every visible edge
                {
                    random = rand() % visible.size(); // find a ranodm number
                    e = 0;
                    while (e < chain1.size())
                    {
                        if (visible[random] == chain1[e])
                        {
                            pos = e; // find visible edge in the polygon chain1
                            break;
                        }
                        e++;
                    }
                    Segment_2 tempppp = chain1[pos];                                   // temp store the visible edge
                    Point_2 temppp = Point_2(chain1[pos][1]);                          // temp store the second point of the edge
                    chain1.insert(chain1.begin() + pos, Segment_2(chain1[pos][0], t)); // insert in chain1 at the position that the visible edge was found the new edge connecting with the interior point
                    chain1.insert(chain1.begin() + pos + 1, Segment_2(t, temppp));
                    chain1.erase(chain1.begin() + pos + 2);
                    result1.erase(std::find(result1.begin(), result1.end(), t)); // delete the interior point
                    poly.clear();
                    keep.clear();
                    e = 0;
                    poly.push_back(chain1[e][0]); // initialize the new polygon
                    keep.push_back(chain1[e][0]); // make a copy of this polygon
                    poly.push_back(chain1[e][1]);
                    keep.push_back(chain1[e][1]);
                    e++;
                    while (e < chain1.size() - 1)
                    {
                        poly.push_back(chain1[e][1]);
                        keep.push_back(chain1[e][1]);
                        e++;
                    }
                    e = 0;
                    while (e < result1.size())
                    {
                        flag1 = check_inside(result1[e], &*keep.begin(), &*keep.end(), K()); // check if with the new edges i made the polygon surrrounds every point
                        e++;
                    }
                    if (flag1 == 1 || poly.is_simple() == 0) // if with the new edges the polugon is not simple or the polugon does not surround every point i am backtracking
                    {
                        chain1.erase(chain1.begin() + pos);
                        chain1.erase(chain1.begin() + pos);
                        chain1.insert(chain1.begin() + pos, tempppp);                      // backtracking deleting the edges i created and bringing back the previous edge
                        result1.push_back(t);                                              // pushing back again the interior point
                        visible.erase(std::find(visible.begin(), visible.end(), tempppp)); // deleting the visible edges because it doesnt meet the criteria
                        if (visible.size() == 0)
                        {
                            pointcount++; // if there are not visible edges that meet the criteria need to search visible edge for the next close point
                        }
                    }
                    else
                    {
                        pointcount = 0;
                        break;
                    }
                }
                visible.clear();
            }
            else if (flagedge == 2 || flagedge == 3) // if edge selection is min or max
            {
                flag1 = 0;
                counterr = 0;
                visible = findvisible(t, temp, chain1, visible); // finds visible edges
                while (visible.size() > counterr)                // checks every visible edge
                {
                    random = counterr;
                    e = 0;
                    while (e < chain1.size())
                    {
                        if (visible[random] == chain1[e])
                        {
                            pos = e;
                            break;
                        }
                        e++;
                    } // finds visible edge position in chain1
                    // same prosess as for random edge selection
                    Segment_2 tempppp = chain1[pos];
                    Point_2 temppp = Point_2(chain1[pos][1]);
                    chain1.insert(chain1.begin() + pos, Segment_2(chain1[pos][0], t));
                    chain1.insert(chain1.begin() + pos + 1, Segment_2(t, temppp));
                    chain1.erase(chain1.begin() + pos + 2);
                    result1.erase(std::find(result1.begin(), result1.end(), t));
                    poly.clear();
                    keep.clear();
                    e = 0;
                    poly.push_back(chain1[e][0]);
                    keep.push_back(chain1[e][0]);
                    poly.push_back(chain1[e][1]);
                    keep.push_back(chain1[e][1]);
                    e++;
                    while (e < chain1.size() - 1)
                    {
                        poly.push_back(chain1[e][1]);
                        keep.push_back(chain1[e][1]);
                        e++;
                    }
                    e = 0;
                    while (e < result1.size())
                    {
                        flag1 = check_inside(result1[e], &*keep.begin(), &*keep.end(), K());
                        e++;
                    }
                    // same process
                    if (flag1 == 1 || poly.is_simple() == 0)
                    {
                        chain1.erase(chain1.begin() + pos);
                        chain1.erase(chain1.begin() + pos);
                        chain1.insert(chain1.begin() + pos, tempppp);
                        result1.push_back(t);
                        if (visible.size() == 0)
                        {
                            pointcount++;
                        }
                    }
                    else
                    {
                        // found visible edge that meets the criteria
                        pointcount = 0;
                        CGAL::area_2(poly.begin(), poly.end(), re, K()); // compute polugons area with the current edges
                        areas.push_back(re);
                        finde.push_back(counterr); // store the position of the visible edge that meets the criteria
                        chain1.erase(chain1.begin() + pos);
                        chain1.erase(chain1.begin() + pos);
                        chain1.insert(chain1.begin() + pos, tempppp); // backtrack for more visible edges that meet the criteria
                        result1.push_back(t);
                    }
                    counterr++; // go to the next edge
                }
                if (pointcount == 0) // found visible edge
                {
                    if (flagedge == 2) // if edge selection =min
                    {
                        index = std::distance(std::begin(areas), std::min_element(std::begin(areas), std::end(areas))); // find min area
                    }
                    else if (flagedge == 3) // if edge selection =max
                    {
                        index = std::distance(std::begin(areas), std::max_element(std::begin(areas), std::end(areas))); // find max area
                    }
                    random = finde[index]; // this is the index of the visible edge that makes a polygon with min/max area
                    e = 0;
                    while (e < chain1.size()) // finding the edge in polygon chain1
                    {
                        if (visible[random] == chain1[e])
                        {
                            pos = e;
                            break;
                        }
                        e++;
                    }
                    // same process creating the edges that make a polygon with min/max area
                    Segment_2 tempppp = chain1[pos];
                    Point_2 temppp = Point_2(chain1[pos][1]);
                    chain1.insert(chain1.begin() + pos, Segment_2(chain1[pos][0], t));
                    chain1.insert(chain1.begin() + pos + 1, Segment_2(t, temppp)); // ta kanw ola auta gia na exw mia swsth seira me ta edges gia na ftiaxw eukola to polugwno
                    chain1.erase(chain1.begin() + pos + 2);
                    result1.erase(std::find(result1.begin(), result1.end(), t));
                    poly.clear();
                    keep.clear();
                    e = 0;
                    poly.push_back(chain1[e][0]);
                    keep.push_back(chain1[e][0]);
                    poly.push_back(chain1[e][1]);
                    keep.push_back(chain1[e][1]);
                    e++;
                    while (e < chain1.size() - 1)
                    {
                        poly.push_back(chain1[e][1]);
                        keep.push_back(chain1[e][1]);
                        e++;
                    }
                }
                finde.clear();
                areas.clear();
                visible.clear();
            }
        }
        // printing the resutlts
        k = 0;
        outfile << "Polygonization" << std::endl;
        while (k < poly.size())
        {
            outfile << poly[k] << std::endl;
            k++;
        }
        k = 0;
        while (k < chain1.size())
        {
            outfile << chain1[k][0] << "<----" << chain1[k][1] << std::endl;
            k++;
        }
        outfile << "Algorithm: convex_hull edge selection " << flagedge << std::endl;
        CGAL::area_2(poly.begin(), poly.end(), area, K());
        outfile << "Calculated area: " << abs(area) << std::endl;
        outfile << "ratio: " << conarea / abs(area) << std::endl;
        outfile << "ratio: " << abs(area) / abs(conarea) << std::endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        outfile << "construction time: " << duration.count() << " ms " << std::endl;
    }

    else if (flagalgo == 1)
    {                                                                          // if incremental
        std::copy(result2.begin(), result2.end(), std::back_inserter(result)); // kanw copy sto vector result1 ta points                                                // paw xana sthn arxh tou arxeiou
        if (flaginit == 1)
            result = init_1a(result);
        else if (flaginit == 2)
        {
            result = init_2a(result);
        }
        else if (flaginit == 3)
        {
            result = init_1b(result);
        }
        else if (flaginit == 4)
        {
            result = init_2b(result);
        }

        if (result.size() >= 3)
        { // build first triangle to temp
            for (int i = 0; i < 3; i++)
            {
                Point_2 t = result[i];
                temppoint.push_back(t);
            }
        }

        p1 = result.begin(); // delete three points from result
        p2 = result.begin() + 3;
        result.erase(p1, p2);

        int numberofsegments = temppoint.size();
        int i;

        for (i = 0; i < numberofsegments; i++) // initialise triangle
        {
            if (i == numberofsegments - 1)
            {
                chain1.push_back(Segment_2(temppoint[numberofsegments - 1], temppoint[0]));
                break;
            }
            chain1.push_back(Segment_2(temppoint[i], temppoint[i + 1]));
        }

        CGAL::convex_hull_2(temppoint.begin(), temppoint.end(), std::back_inserter(convex_hull));
        int d = convex_hull.size();
        for (int i = 0; i < convex_hull.size(); i++)
        {
            poly.push_back(convex_hull[i]);
        }

        convex_seg = create_segments(temppoint);
        segments chain11;
        if (flagedge == 1)
        {
            chain11 = incremental(result, temppoint, convex_seg, chain1);
        }
        else if (flagedge == 2)
        {
            chain11 = incremental_min(result, temppoint, convex_seg, chain1);
        }
        else if (flagedge == 3)
        {
            chain11 = incremental_max(result, temppoint, convex_seg, chain1);
        }
        poly.clear();
        int e = 0;
        poly.push_back(chain11[e][0]); // initialize the new polygon
        poly.push_back(chain11[e][1]);
        e++;
        while (e < chain11.size() - 1)
        {
            poly.push_back(chain11[e][1]);
            e++;
        }
        outfile << "Polygonization" << std::endl;
        k = 0;
        while (k < poly.size())
        {
            outfile << poly[k] << std::endl;
            k++;
        }
        k = 0;
        while (k < chain11.size())
        {
            outfile << chain11[k][0] << "<----" << chain11[k][1] << std::endl;
            k++;
        }
        outfile << "Algorithm: incremental edge selection " << flagedge << " initialization " << flaginit << std::endl;
        double re;
        CGAL::area_2(poly.begin(), poly.end(), re, K());
        outfile << "Calculated area " << abs(re) << std::endl;
        Points convex_hull_area;
        CGAL::convex_hull_2(result2.begin(), result2.end(), std::back_inserter(convex_hull_area));
        double convex_hull_re;
        CGAL::area_2(convex_hull_area.begin(), convex_hull_area.end(), convex_hull_re, K());
        outfile << "ratio: " << abs(re) / abs(convex_hull_re) << std::endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        outfile << "construction time: " << duration.count() << " ms " << std::endl;
    }
    //--------------------------
    int how_many_points;
    handle_input(argv);
    how_many_points = create_polygon(argv[2]);
    get_points(how_many_points);
    create_chain(how_many_points);
    if (flag_algo == 1)
    { // if algorithm is local search

        local_search();
    }
    else if (flag_algo == 2)
    { // if algorithm is simulated_annealing
        CGAL::draw(p);

        simulated_annealing(how_many_points);
    }
    CGAL::draw(p);
}
