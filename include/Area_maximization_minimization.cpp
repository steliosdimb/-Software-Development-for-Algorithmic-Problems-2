#include "Area_maximization_minimization.hpp"



void handle_input(char ** argv){
    std::ifstream in(argv[2]); // input file
    std::ofstream outfile;
    outfile.open(argv[4]); // output file
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
}
int create_polygon(char * arg){
    int how_many_points;
    std::string input_name = std::string(arg);  //finding how many points we have
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
    std::string sub1;
    std::string sub2;
    while(i<how_many_points){   //extracting the points of the polygon from my output and making the polygon we will use
        std::getline(poly,line);
        pos = line.find(" ");
        sub1 = line.substr(0 , pos);
        sub2 = line.substr(pos + 1);
        x=stoi(sub1);
        y=stoi(sub2);
        p.push_back(Point_2(x,y));
        i++;
    }
    return how_many_points;
}
void create_chain(int how_many_points){
    int i=0;
    std::string sub3;
    std::string sub4;
    int x1;
    int y1;
    while(i<how_many_points){
        chain.push_back(Segment_2(p[i],p[i+1]));
        if(i==how_many_points-2){
            break;
        }
    i++;
    }
    chain.push_back(Segment_2(p[i+1],p[0])); //made the segments of the polygon
}
void get_points(int how_many_points){
    int i;
    for(i=0;i<how_many_points;i++){
        points.push_back(p[i]);
    }
}
void local_search(void){
    //pana
}
void simulated_annealing(int how_many_points){
    double energy;
    double convex_hull_area;
    double polygon_area;
    Points temp;
    Points convex_hull_points;  
    CGAL::convex_hull_2(points.begin(),points.end(),std::back_inserter(convex_hull_points));    //find the convex hull
    CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K());  //compute convex hull area
    CGAL::area_2(points.begin(),points.end(),polygon_area,K()); //compute polygons area
    tree tr;    
    int i;
    if (flag_min_max == 1)
    { // min
        energy=how_many_points*(polygon_area/convex_hull_area); //finding energy for minimization
        if (option == 1)
        { // local
            for(i=0;i<how_many_points;i++){
                tr.insert(points[i]);
            }
            box search_box(points[2],points[3],0.0);
            tr.search(std::back_inserter(temp),search_box);
            for(i=0;i<temp.size();i++){
                std::cout << temp[i] << std::endl;
            }
        }
        else if (option == 2)
        { // global
        }
        else if (option == 3)
        { // subdivision
        }
    }
    else if (flag_min_max == 2)
    { // max
        energy=how_many_points*(1-(polygon_area/convex_hull_area)); //finding energy for maximization
        if (option == 1)
        { // local
            
        }
        else if (option == 2)
        { // global
        }
        else if (option == 3)
        { // subdivision
        }
    }
}