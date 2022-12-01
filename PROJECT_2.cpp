#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_2_algorithms.h>
#include "include/Area_maximization_minimization.hpp"
#include <cstdlib>
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


int main(int argc,char * argv[]){
    Polygon_2 p;


    std::ifstream in(argv[2]); // input file
    std::ofstream outfile;
    outfile.open(argv[4]); // output file
    int flag_algo; //algorithm we choose
    int flag_min_max;   //minimization or maximixation
    int option; //global local or subdivision
    int L;  //path length
    int how_many_points;
    double threshold;
    if (strcmp("local_search", argv[6]) == 0)
    {
        flag_algo = 1; // if -algorithm=local_search
        threshold=atof(argv[11]);
    }
    else if (strcmp("simulated_annealing", argv[6]) == 0)
    {
        flag_algo = 2; // if -algorithm=simulated_annealing
        if(strcmp("local", argv[11]) == 0){
            option=1; //option=local
        }
        else if(strcmp("global", argv[11]) == 0){
            option=2; //option=global
        }
        else if(strcmp("subdivision", argv[11]) == 0){
            option=3;
        }
    }
    L=atoi(argv[8]);
    if(strcmp("-min", argv[9]) == 0){
        flag_min_max=1;  //if -min
    }
    else if(strcmp("-max", argv[9]) == 0){
        flag_min_max=2; //if -max
    }
    std::string input_name = std::string(argv[2]);  //finding how many points we have
    while(input_name.at(0)<'1' || input_name.at(0)>'9'){
        input_name.erase(input_name.begin());

    }

    int i=0;
    while(input_name.at(i)>='0' && input_name.at(i)<='9'){
        i++;
    }
    input_name.erase(input_name.begin()+i,input_name.end());
    how_many_points=stoi(input_name);
    std::string line;
    std::ifstream poly("test_output.txt");
    std::getline(poly, line); //skip first line
    i=0;
    char * temp;
    int pos;
    int x;
    int y;
    while(i<how_many_points){   //extracting the points of the polygon from my output and making the polygon we will use
        std::getline(poly,line);
        pos = line.find(" ");
        std::string sub1 = line.substr(0 , pos);
        std::string sub2 = line.substr(pos + 1);
        x=stoi(sub1);
        y=stoi(sub2);
        p.push_back(Point_2(x,y));
        i++;
    }

}
