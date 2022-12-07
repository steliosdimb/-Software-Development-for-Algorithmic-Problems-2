#include "Area_maximization_minimization.hpp"

void handle_input(char **argv)
{
    std::ifstream in(argv[2]); // input file
    std::ofstream outfile;
    outfile.open(argv[4]); // output file
    if (strcmp("local_search", argv[6]) == 0)
    {
        flag_algo = 1; // if -algorithm=local_search
        threshold = atof(argv[11]);
    }
    else if (strcmp("simulated_annealing", argv[6]) == 0)
    {
        flag_algo = 2; // if -algorithm=simulated_annealing
        if (strcmp("local", argv[11]) == 0)
        {
            option = 1; // option=local
        }
        else if (strcmp("global", argv[11]) == 0)
        {
            option = 2; // option=global
        }
        else if (strcmp("subdivision", argv[11]) == 0)
        {
            option = 3;
        }
    }
    L = atoi(argv[8]);
    if (strcmp("-min", argv[9]) == 0)
    {
        flag_min_max = 1; // if -min
    }
    else if (strcmp("-max", argv[9]) == 0)
    {
        flag_min_max = 2; // if -max
    }
}
int create_polygon(char *arg)
{
    int how_many_points;
    std::string input_name = std::string(arg); // finding how many points we have
    while (input_name.at(0) < '1' || input_name.at(0) > '9')
    {
        input_name.erase(input_name.begin());
    }

    int i = 0;
    while (input_name.at(i) >= '0' && input_name.at(i) <= '9')
    {
        i++;
    }
    input_name.erase(input_name.begin() + i, input_name.end());

    how_many_points = stoi(input_name);

    std::string line;
    std::ifstream poly("test_output.txt");
    std::getline(poly, line); // skip first line
    i = 0;
    char *temp;
    int pos;
    int x;
    int y;
    std::string sub1;
    std::string sub2;
    while (i < how_many_points)
    { // extracting the points of the polygon from my output and making the polygon we will use
        std::getline(poly, line);
        pos = line.find(" ");
        sub1 = line.substr(0, pos);
        sub2 = line.substr(pos + 1);
        x = stoi(sub1);
        y = stoi(sub2);
        p.push_back(Point_2(x, y));
        i++;
    }
    return how_many_points;
}
void create_chain(int how_many_points)
{
    int i = 0;
    while (i < how_many_points)
    {
        chain.push_back(Segment_2(p[i], p[i + 1]));
        if (i == how_many_points - 2)
        {
            break;
        }
        i++;
    }
    chain.push_back(Segment_2(p[i + 1], p[0])); // made the segments of the polygon
}
void get_points(int how_many_points)
{
    int i;
    for (i = 0; i < how_many_points; i++)
    {
        points.push_back(p[i]);
    }
}
void local_search(void)
{
    // pana
}
void simulated_annealing(int how_many_points)
{   
    int k;
    int m=3;//define m for subvision 
    int i;
    int random_position; // position of a random point
    int random_position_1;
    int counter=0;
    int point_counter=0;
    int valid_check_counter=0;  //for 10 points for example we may not find a valid move so we stop the program
    double previous_energy;
    double sequent_energy;
    double convex_hull_area;
    double polygon_area;
    double temperature = 1; // initialize T as 1
    double metropolis_criterion;
    double DE;              // energy rate of change
    double R;               // random number between 0,1
    bool do_they_intersect; // find if they intersect
    srand((unsigned)time(NULL));
    Point_2 random_point;      // a random point
    Point_2 random_point_1;     //2nd random point (global)
    Point_2 successor_point;   // next point from the random_point
    Point_2 d_successor_point; // next x2 point from the random_point
    Point_2 predecessor_point; // previous point from the random_point
    Point_2 successor_point_1;   //successor for the 2nd random point(global)
    Points temp;               // holds the points of the fuzzy iso box
    Points tempp;
    Points convex_hull_points;
    Polygon_2 temp_polygon; // keeps a temp polygon
    tree tr;                // kd tree
    std::vector<Segment_2>::iterator it;
    CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points)); // find the convex hull
    CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
    CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
    std::cout << "before" << polygon_area << std::endl;
    if (flag_min_max == 1)
    {                                                                          // min
        previous_energy = how_many_points * (polygon_area / convex_hull_area); // finding initial energy for minimization
        while (temperature >= 0)
        {
            if (option == 1)
            { // local
                tempp.clear();
                temp.clear();
                convex_hull_points.clear();
                tr.clear();
                R = (float)rand() / RAND_MAX;
                temp_polygon = p;
                random_position = 1 + rand() % (how_many_points - 3); // find a random position of a point cant be the first or last point or before last point
                random_point = p[random_position];                    // find a random point
                successor_point = p[random_position + 1];             // find its successor
                d_successor_point = p[random_position + 2];           // fits its x2 successor
                predecessor_point = p[random_position - 1];           // find its predecessor
                p[random_position] = successor_point;
                p[random_position + 1] = random_point; // swap positions,random point with its successor
                random_point = p[random_position];
                successor_point = p[random_position + 1];                                                                                          // new values for succesor point and the random point
                chain.clear();                                                                                                                     // clear the chain because we have a new polygon
                create_chain(how_many_points);                                                                                                     // create the new chain
                do_they_intersect = find_intersection_1(Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point)); // check if pr,qs intersect // check if pr,qs intersect
                if (do_they_intersect == 1)
                { // the new edges intersect each other
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                    continue; // they intersect so we find another point
                }
                else if (do_they_intersect == 0)
                {
                    for (i = 0; i < how_many_points; i++)
                    {
                        tr.insert(p[i]); // construct a kd tree with every point
                    }
                    tr.build();                                               // build the tree
                    tempp.push_back(predecessor_point);
                    tempp.push_back(random_point);
                    tempp.push_back(d_successor_point);
                    tempp.push_back(successor_point);
                    sort(tempp.begin(),tempp.end());
                    int min_x=tempp[0].x();
                    int max_x=tempp[3].x();
                    int arr[]={(int)tempp[0].y(),(int)tempp[1].y(),(int)tempp[2].y(),(int)tempp[3].y()};
                    int *min_y = std::min_element( std::begin(arr), std::end(arr) );
                    int *max_y = std::max_element( std::begin(arr), std::end(arr) );
                    box search_box(Point_2(min_x,*min_y),Point_2(max_x,*max_y)); // construct a fuzzy box with the range of the points you need
                    tr.search(std::back_inserter(temp),search_box);// get the points inside the fuzzy box
                    int j;
                    int flag;
                    int flag1;

                    int result = 0;

                    int result1 = 0;

                    Segment_2 temp1;
                    Segment_2 temp2;

                    for (i = 0; i < temp.size(); i++)
                    {
                        result = 0;
                        result1 = 0;

                        flag = 0;
                        flag1 = 0;
                        if (temp[i] == random_point || temp[i] == successor_point)
                        { // dont need to chekc for points q and r(successor and random) because their edges dont intersect
                            continue;
                        }
                        else
                        { // for any other point
                            for (j = 0; j < chain.size(); j++)
                            {
                                if (chain[j][0] == temp[i] && chain[j][1] != random_point && chain[j][1] != successor_point)
                                { // if i find an edge which does not have q or r as a point i need to check if it intersects
                                    flag = 1;
                                    temp1 = chain[j];
                                }
                                else if (chain[j][1] == temp[i] && chain[j][0] != random_point && chain[j][0] != successor_point)
                                {
                                    flag1 = 1;
                                    temp2 = chain[j];
                                }
                            }
                        }
                        if (flag == 1)
                        {
                            result = find_intersection(temp1, Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point));
                        }
                        if (flag1 == 1)
                        {
                            result1 = find_intersection(temp2, Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point));
                        }

                        if (result == 1 || result1 == 1)
                        {
                            p.clear();
                            p = temp_polygon;
                            chain.clear();
                            create_chain(how_many_points);
                            continue; // they intersect so we find another point
                        }
                    }
                    if(p.is_simple()==0){
                        std::cout << "lol1" << std::endl;
                        break;
                    }

                }

                // if we have a valid change
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (polygon_area / convex_hull_area);                      // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                if (DE < 0 || metropolis_criterion >= R)
                {
                    temperature = temperature - (1 / L);
                    previous_energy = sequent_energy;
                }
                else if (counter < L)
                {
                    counter++;
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                }
                else if (counter == L)
                {
                    break;
                }
            }
            else if (option == 2)
            { // global
                if(how_many_points==10 && valid_check_counter>1000){
                    std::cout << "NO valid moves" << std::endl;
                    break;//we didnt find a valid move because we have only 10 points
                }
                convex_hull_points.clear();
                R = (float)rand() / RAND_MAX;
                temp_polygon = p;
                random_position = 1 + rand() % (how_many_points - 2); // find a random position of a point cant be the first or last point
                random_point = p[random_position];                    // find a random point
                successor_point = p[random_position + 1];             // find its successor
                predecessor_point=p[random_position-1];     //find its predecessor
                random_position_1=rand()%(how_many_points-2);   //find random position for te other point.Can be the first point because we only get its successor cant be the last point
                while(random_position_1==random_position || random_position_1==random_position+1 || random_position_1==random_position-1 || random_position_1==random_position-2){
                //new random point cant be same as the previous random point or its successor or its predecessor or its predecessor -1 because his succesor will be equal to predecessor of the previous point
                   random_position_1=rand()%(how_many_points-2);//find another point 
                }
                random_point_1=p[random_position_1];    //find random point 2
                successor_point_1=p[random_position_1+1];    //find its successor

                chain.erase(std::find(chain.begin(),chain.end(),Segment_2(random_point,successor_point)));//erase segment qp

                for(i=0;i<chain.size();i++){
                    if(chain[i][0]==Point_2(predecessor_point)){
                        chain[i]=Segment_2(Point_2(predecessor_point),Point_2(successor_point));   //make the new edge pr
                    }
                }

                int pos;
                for(i=0;i<chain.size();i++){
                    if(Segment_2(random_point_1,successor_point_1)==chain[i]){
                        chain[i]=Segment_2(Point_2(random_point_1),Point_2(random_point));//connect s and q
                        pos=i;
                        break;
                    }
                }    
                chain.insert(chain.begin()+pos+1,Segment_2(random_point,successor_point_1));//connect q and t
                create_new_polygon();
                int result;
                int result3;
                result=find_intersection_1(Segment_2(predecessor_point,successor_point),Segment_2(random_point_1,random_point));
                result3=find_intersection_1(Segment_2(predecessor_point,successor_point),Segment_2(random_point,successor_point_1));
                //check if pr intersects sq qt
                if(result==1 || result3==1){
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                    valid_check_counter++;
                    continue; // they intersect so we find another point
                }
                else{
                    int result1;
                    int result2;
                    result=find_intersection_2(Segment_2(predecessor_point,successor_point));
                    result1=find_intersection_2(Segment_2(random_point_1,random_point));
                    result2=find_intersection_2(Segment_2(random_point,successor_point_1));
                    if(result==1 || result1==1 || result2==1){
                        p.clear();
                        p = temp_polygon;
                        chain.clear();
                        create_chain(how_many_points);
                        valid_check_counter++;
                        continue; // they intersect so we find another point
                    }
                }
                valid_check_counter=0;
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (polygon_area / convex_hull_area);                      // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                if (DE < 0 || metropolis_criterion >= R)
                {
                    temperature = temperature - (1 / L);
                    previous_energy = sequent_energy;
                }

                else if (counter < L)
                {
                    counter++;
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                }
                else if (counter == L)
                {
                    break;
                }
            }
            else if (option == 3)
            { // subdivision

            }
        }
    }
    else if (flag_min_max == 2)
    {                                                                                // max
        previous_energy = how_many_points * (1 - (polygon_area / convex_hull_area)); // finding energy for maximization
        while (temperature >= 0)
        {
            if (option == 1)
            { // local
                tempp.clear();
                temp.clear();
                convex_hull_points.clear();
                tr.clear();
                R = (float)rand() / RAND_MAX;
                temp_polygon = p;
                random_position = 1 + rand() % (how_many_points - 3); // find a random position of a point cant be the first or last point or before last point
                random_point = p[random_position];                    // find a random point
                successor_point = p[random_position + 1];             // find its successor
                d_successor_point = p[random_position + 2];           // fits its x2 successor
                predecessor_point = p[random_position - 1];           // find its predecessor
                p[random_position] = successor_point;
                p[random_position + 1] = random_point; // swap positions,random point with its successor
                random_point = p[random_position];
                successor_point = p[random_position + 1];                                                                                          // new values for succesor point and the random point
                chain.clear();                                                                                                                     // clear the chain because we have a new polygon
                create_chain(how_many_points);                                                                                                     // create the new chain
                do_they_intersect = find_intersection_1(Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point)); // check if pr,qs intersect // check if pr,qs intersect
                if (do_they_intersect == 1)
                { // the new edges intersect each other
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                    continue; // they intersect so we find another point
                }
                else if (do_they_intersect == 0)
                {
                    for (i = 0; i < how_many_points; i++)
                    {
                        tr.insert(p[i]); // construct a kd tree with every point
                    }
                    tr.build();                                               // build the tree
                    tempp.push_back(predecessor_point);
                    tempp.push_back(random_point);
                    tempp.push_back(d_successor_point);
                    tempp.push_back(successor_point);
                    sort(tempp.begin(),tempp.end());
                    int min_x=tempp[0].x();
                    int max_x=tempp[3].x();
                    int arr[]={(int)tempp[0].y(),(int)tempp[1].y(),(int)tempp[2].y(),(int)tempp[3].y()};
                    int *min_y = std::min_element( std::begin(arr), std::end(arr) );
                    int *max_y = std::max_element( std::begin(arr), std::end(arr) );
                    box search_box(Point_2(min_x,*min_y),Point_2(max_x,*max_y)); // construct a fuzzy box with the range of the points you need
                    tr.search(std::back_inserter(temp),search_box);// get the points inside the fuzzy box
                    int j;
                    int flag;
                    int flag1;

                    int result = 0;

                    int result1 = 0;

                    Segment_2 temp1;
                    Segment_2 temp2;

                    for (i = 0; i < temp.size(); i++)
                    {
                        result = 0;
                        result1 = 0;

                        flag = 0;
                        flag1 = 0;
                        if (temp[i] == random_point || temp[i] == successor_point)
                        { // dont need to chekc for points q and r(successor and random) because their edges dont intersect
                            continue;
                        }
                        else
                        { // for any other point
                            for (j = 0; j < chain.size(); j++)
                            {
                                if (chain[j][0] == temp[i] && chain[j][1] != random_point && chain[j][1] != successor_point)
                                { // if i find an edge which does not have q or r as a point i need to check if it intersects
                                    flag = 1;
                                    temp1 = chain[j];
                                }
                                else if (chain[j][1] == temp[i] && chain[j][0] != random_point && chain[j][0] != successor_point)
                                {
                                    flag1 = 1;
                                    temp2 = chain[j];
                                }
                            }
                        }
                        if (flag == 1)
                        {
                            result = find_intersection(temp1, Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point));
                        }
                        if (flag1 == 1)
                        {
                            result1 = find_intersection(temp2, Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point));
                        }

                        if (result == 1 || result1 == 1)
                        {
                            p.clear();
                            p = temp_polygon;
                            chain.clear();
                            create_chain(how_many_points);
                            continue; // they intersect so we find another point
                        }
                    }
                    if(p.is_simple()==0){
                        std::cout << "lol1" << std::endl;
                        break;
                    }

                }

                // if we have a valid change
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (1 - (polygon_area / convex_hull_area));                     // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                if (DE < 0 || metropolis_criterion >= R)
                {
                    temperature = temperature - (1 / L);
                    previous_energy = sequent_energy;
                }
                else if (counter < L)
                {
                    counter++;
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                }
                else if (counter == L)
                {
                    break;
                }
            }
            else if (option == 2)
            { // global
                if (how_many_points == 10 && valid_check_counter > 1000)
                {
                    std::cout << "NO valid moves" << std::endl;
                    break; // we didnt find a valid move because we have only 10 points
                }
                convex_hull_points.clear();
                R = (float)rand() / RAND_MAX;
                temp_polygon = p;
                random_position = 1 + rand() % (how_many_points - 2); // find a random position of a point cant be the first or last point
                random_point = p[random_position];                    // find a random point
                successor_point = p[random_position + 1];             // find its successor
                predecessor_point=p[random_position-1];     //find its predecessor
                random_position_1=rand()%(how_many_points-2);   //find random position for te other point.Can be the first point because we only get its successor cant be the last point
                while(random_position_1==random_position || random_position_1==random_position+1 || random_position_1==random_position-1 || random_position_1==random_position-2){
                //new random point cant be same as the previous random point or its successor or its predecessor or its predecessor -1 because his succesor will be equal to predecessor of the previous point
                   random_position_1=rand()%(how_many_points-2);//find another point 
                }
                random_point_1=p[random_position_1];    //find random point 2
                successor_point_1=p[random_position_1+1];    //find its successor

                chain.erase(std::find(chain.begin(),chain.end(),Segment_2(random_point,successor_point)));//erase segment qp

                for(i=0;i<chain.size();i++){
                    if(chain[i][0]==Point_2(predecessor_point)){
                        chain[i]=Segment_2(Point_2(predecessor_point),Point_2(successor_point));   //make the new edge pr
                    }
                }

                int pos;
                for(i=0;i<chain.size();i++){
                    if(Segment_2(random_point_1,successor_point_1)==chain[i]){
                        chain[i]=Segment_2(Point_2(random_point_1),Point_2(random_point));//connect s and q
                        pos=i;
                        break;
                    }
                }    
                chain.insert(chain.begin()+pos+1,Segment_2(random_point,successor_point_1));//connect q and t
                create_new_polygon();
                int result;
                int result3;
                result=find_intersection_1(Segment_2(predecessor_point,successor_point),Segment_2(random_point_1,random_point));
                result3=find_intersection_1(Segment_2(predecessor_point,successor_point),Segment_2(random_point,successor_point_1));
                //check if pr intersects sq qt
                if(result==1 || result3==1){
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                    valid_check_counter++;
                    continue; // they intersect so we find another point
                }
                else{
                    valid_check_counter=0;
                    int result1;
                    int result2;
                    result=find_intersection_2(Segment_2(predecessor_point,successor_point));
                    result1=find_intersection_2(Segment_2(random_point_1,random_point));
                    result2=find_intersection_2(Segment_2(random_point,successor_point_1));
                    if(result==1 || result1==1 || result2==1){
                        p.clear();
                        p = temp_polygon;
                        chain.clear();
                        create_chain(how_many_points);
                        valid_check_counter++;
                        continue; // they intersect so we find another point
                    }
                }
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (1 - (polygon_area / convex_hull_area));                     // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                if (DE < 0 || metropolis_criterion >= R)
                {
                    temperature = temperature - (1 / L);
                    previous_energy = sequent_energy;
                }

                else if (counter < L)
                {
                    counter++;
                    p.clear();
                    p = temp_polygon;
                    chain.clear();
                    create_chain(how_many_points);
                }
                else if (counter == L)
                {
                    break;
                }
            }
            else if (option == 3)
            { // subdivision
                p.clear();
                chain.clear();
                int must_end=0;
                sort(points.begin(), points.end()); // sort points by x
                Points keep_that;
                Points temp_points;
                int j;
                k = (how_many_points - 1) / (m - 1); // k subsets

                for (i = 0; i < k; i++)
                { // for every subset
                    if (i == 0)
                    {
                        copy(points.begin() + point_counter, points.begin() + m + point_counter, back_inserter(temp_points));
                    }
                    else if (i > 0 && i<k-1)
                    {
                        copy(points.begin() + point_counter - 1, points.begin() + point_counter + m, back_inserter(temp_points));
                    }
                    if(i!=k-1){
                        point_counter = point_counter + m;
                    }
                    if (i==0)
                    {
                        int important_flag = 0;
                        int important_counter = 0;
                        while (important_flag != 1)
                        {
                            Point_2 last_point = temp_points[m - 1 + important_counter];        // get last point
                            Point_2 before_last_point = temp_points[m - 2 + important_counter]; // get before last point
                            Point_2 next_point = points[point_counter + important_counter];     // next point
                            int last_p_y = (int)last_point.y();                                 // get last point y
                            int before_p_y = (int)before_last_point.y();                        // get before last point y
                            int after_p_y = (int)next_point.y();
                            if (last_p_y > before_p_y && after_p_y < last_p_y)
                            {                       // if we have monotone increasing
                                important_flag = 1; // we have a good subset continue
                            }
                            else
                            {
                                important_counter++;
                                temp_points.push_back(points[(m - 1) + important_counter]); // add one more point till we have monotone increasing edge(thats for first subset)
                            }
                        }
                    }
                    else if(i<=k-2){
                        int important_flag = 0;
                        int important_counter = 0;
                        while (important_flag != 1)
                        {
                            Point_2 last_point = temp_points[m + important_counter];        // get last point
                            Point_2 before_last_point = temp_points[m - 1 + important_counter]; // get before last point
                            Point_2 next_point = points[point_counter + important_counter];     // next point
                            int last_p_y = (int)last_point.y();                                 // get last point y
                            int before_p_y = (int)before_last_point.y();                        // get before last point y
                            int after_p_y = (int)next_point.y();
                            if (last_p_y > before_p_y && after_p_y < last_p_y)
                            {                       // if we have monotone increasing
                                important_flag = 1; // we have a good subset continue
                            }
                            else
                            {
                                important_counter++;
                                if(point_counter-1 + important_counter>how_many_points-1){
                                    must_end=1;
                                }
                                else{
                                    temp_points.push_back(points[point_counter-1 + important_counter]); // add one more point till we have monotone increasing edge(thats for first subset)
                                }
                            }
                        }
                        if (i == k - 2)
                        {
                            keep_that = temp_points;
                        }
                        point_counter = point_counter + important_counter;
                    }
                    else if (i == k - 1)
                    {
                        if (points.size() - point_counter < m)
                        {
                            for (j = point_counter; j < points.size(); j++)
                            {
                                keep_that.push_back(points[j]); // add the rest points to the last subset
                                std::cout << "--------" << std::endl;
                            }
                        }
                        else
                        {
                            for(j = point_counter; j < points.size(); j++){
                                temp_points.push_back(points[j]); // create new subset for rest points
                            }
                        }
                    }
                    if(i!=k-1){
                    for (j = 0; j < temp_points.size(); j++)
                    {
                        std::cout << temp_points[j] << std::endl;
                    }
                    std::cout << temp_points.size() << " "
                              << "------" << std::endl;}
                    else{
                        for (j = 0; j < keep_that.size(); j++)
                    {
                        std::cout << keep_that[j] << std::endl;
                    }
                    }
                    if(must_end==1){
                        break;
                    }
                    temp_points.clear();
                }
            }
        temperature=-1;//NA TO VGALW
        }
    }
    std::cout << "after" << polygon_area << std::endl;
}
int find_intersection(Segment_2 candicate_edge, Segment_2 pr, Segment_2 qs)
{
    int flag;
    flag = 0;
    auto result = CGAL::intersection(candicate_edge, pr); // checking if the candicate edge intersects with pr edge
    if (result)
    {
        if (const Segment_2 *s = boost::get<Segment_2>(&*result)) // if they intersect to a segment then we have an intersection
        {
            flag = 1;
        }
        else
        {
            Point_2 *p = boost::get<Point_2>(&*result); // if they intersect to p point we dont have an intersection
            if (*p == pr[0])
            {
                flag = 0;
            }
            else // if its any other point we have an intersect
            {
                flag = 1;
            }
        }
    }
    if(flag==1){
        return 1;
    }
    // same process for the qs edge
    result = CGAL::intersection(candicate_edge, qs);
    if (result)
    {
        if (Segment_2 *s = boost::get<Segment_2>(&*result))
        {
            flag = 1;
        }
        else
        {
            Point_2 *p = boost::get<Point_2>(&*result);
            if (*p == qs[1]) // if they intersect to s point we dont have an intersection
            {
                flag = 0;
            }
            else
            {
                flag = 1;
            }
        }
    }

    if(flag==1)
    {
        return 1; // we have an intersection
    }
    else{
        return 0;
    }
}
int find_intersection_1(Segment_2 segm1,Segment_2 segm2)
{
    int flag;
    flag = 0;
    auto result = CGAL::intersection(segm1, segm2); // checking if the candicate edge intersects with pr edge
    if (result)
    {
        if (const Segment_2 *s = boost::get<Segment_2>(&*result)) // if they intersect to a segment then we have an intersection
        {
            flag = 1;
        }
        else
        {
            if (const Point_2 *p = boost::get<Point_2>(&*result))
            {
                flag = 1;
            }
        }
    }
    return flag;
}
void create_new_polygon(){
    p.clear();
    int i;
    p.push_back(chain[0][0]);
    for(i=0;i<chain.size()-1;i++){
        p.push_back(chain[i][1]);
    }   
}
int find_intersection_2(Segment_2 segm1)
{  
    int i;
    int flag;
    for (i = 0; i < chain.size(); i++)
    {
        flag=0;
        if (chain[i] != segm1)
        {
            auto result = CGAL::intersection(segm1, chain[i]); // checking if the candicate edge intersects with pr edge
            if (result)
            {
                if (const Segment_2 *s = boost::get<Segment_2>(&*result)) // if they intersect to a segment then we have an intersection
                {
                    flag = 1;
                }
                else
                {
                    Point_2 *p = boost::get<Point_2>(&*result);
                    if (*p == segm1[0] || *p == segm1[1])
                    {
                        flag = 0;
                    }
                    else
                    {
                        flag = 1;
                    }
                }
            }
        }
        else
        {
            continue;
        }
        if(flag==1){
            return 1;
        }
    }
    return 0;
}
//----------------------------------------------------------------------------------


Segment_2 error = Segment_2(Point_2(-1,-1), Point_2(-1,-1));



Points handleinput(std::ifstream &in, Points result2)
{
    std::string line;
    std::string temp;
    char ccc;
    int i;
    int n;
    int n1;
    int length;
    while (in.peek() != EOF)
    {
        //get each line 
        std::getline(in, line);
        i = 0;
        ccc = line.at(i);
        while (ccc != '\t')
        {
            i++;
            ccc = line.at(i);
        }
        line.erase(0, i + 1); //finding the first characters that i dont want 0,1,2,3,4,5 etc and erase them
        temp = line;
        i = 0;
        ccc = line.at(i);
        while (ccc != '\t')
        {
            i++;
            ccc = line.at(i);
        }
        line.erase(line.begin() + i, line.end());   //finding the first point of the line
        n = stoi(line); //converting it to int
        line = temp;
        length = trunc(log10(n)) + 1;
        line.erase(0, length + 1);  //finding the second point of the line
        n1 = stoi(line);    //converting it to int
        result2.push_back(Point_2(n, n1));  //pushing each point to the vector
    }
    return result2;
}
Point_2 pointdistance1(Points interior, segments ch, dist d, int count)
{ 
    //finding the count closest interior point from an edge
    int i;
    Point_2 temppoint;
    d.clear();
    for (i = 0; i < interior.size(); i++)
    {
        d.push_back(CGAL::squared_distance(ch.front(), interior[i]));   //for every point find the distance from an edge
    }
    std::sort(d.begin(), d.end());  //sort the distances
    for (i = 0; i < interior.size(); i++)
    {
        if (CGAL::squared_distance(ch.front(), interior[i]) == d[count])    //finding the point i want
        { 
            temppoint = interior[i];
        }
    }
    return temppoint;
}
Point_2 pointdistance(Points interior, segments ch, dist d)
{   //same as pointdistance 1 but finds the closest point from an edge
    int i;
    Point_2 temppoint;
    d.clear();
    for (i = 0; i < interior.size(); i++)
    {
        d.push_back(CGAL::squared_distance(ch.front(), interior[i]));
    }
    std::sort(d.begin(), d.end()); 
    for (i = 0; i < interior.size(); i++)
    {
        if (CGAL::squared_distance(ch.front(), interior[i]) == d.front())
        { 
            temppoint = interior[i];
        }
    }
    return temppoint;
}
segments findvisible(Point_2 t, segments temp, segments chain1, segments visible)
{
    //takes one edge and two edges that conect 1 interior point with this edge and test for every edge if they intersect with any other edge
    int c = 0;
    int flag;
    while (c < chain1.size()) 
    {
        temp.push_back(Segment_2(t, chain1[c][0]));                  //this is an edge from interior point to a peak of a polygons edge
        temp.push_back(Segment_2(t, chain1[c][1]));                  //this is an edge from interior point to the other peak of a polygons edge
        flag = findintersection(temp[0], temp[1], chain1, chain1[c]); //check if its visible
        if (flag == 0)                                              //no intersections so its a visible edge
        {
            visible.push_back(chain1[c]);
        }
        temp.clear();
        c++;
    }
    return visible;
}
int findintersection(Segment_2 interioredge1, Segment_2 interioredge2, segments pchain, Segment_2 chainedge)
{
    //finds if two edges intersect to a point or to an edge
    int c = 0;
    int flag;
    while (c < pchain.size())
    {
        flag = 0;
        auto result = CGAL::intersection(interioredge1, pchain[c]); //giving one edge from a point to a peak of a polygons edge
        if (result)
        {
            if (const Segment_2 *s = boost::get<Segment_2>(&*result))   //if they intersect to a segment then we have an intersection
            {
                flag = 1;
            }
            else
            {
                Point_2 *p = boost::get<Point_2>(&*result); //if they intersect to a point and the point is the peak of the edge then we dont have an intersection
                if (pchain[c] == chainedge && *p == interioredge1[1])
                {
                    flag = 0;
                }
                else    //if its any other point we have an intersect
                {
                    flag = 1;
                }
            }
        }
        //same process for the edge from interior point to the other peak of the edge
        result = CGAL::intersection(interioredge2, pchain[c]);
        if (result)
        {
            if (Segment_2 *s = boost::get<Segment_2>(&*result))
            {
                flag = 1;
            }
            else
            {
                Point_2 *p = boost::get<Point_2>(&*result);
                if (pchain[c] == chainedge && *p == interioredge2[1])
                {
                    flag = 0;
                }
                else
                {
                    flag = 1;
                }
            }
        }
        c++;
    }
    return flag;
}
int check_inside(Point_2 pt, Point_2 *pgn_begin, Point_2 *pgn_end, K traits)
{
    //checks if the polygon surrounds every point or not 
    int flag = 0;
    switch (CGAL::bounded_side_2(pgn_begin, pgn_end, pt, traits))
    {
    case CGAL::ON_BOUNDED_SIDE:
        break;
    case CGAL::ON_BOUNDARY:
        break;
    case CGAL::ON_UNBOUNDED_SIDE:
        flag = 1;
        break;
    }
    return flag;
}


bool comp1a(Point_2 pt1, Point_2 pt2) { 
    if(pt1.x()==pt2.x()){
        return pt1.y()<pt2.y();
    }
    else
        return (pt1.x() < pt2.x());}
bool comp1b(Point_2 pt1, Point_2 pt2) {
    if(pt1.x()==pt2.x()){
        return pt1.y()>pt2.y();
    }
    else 
        return (pt1.x() > pt2.x());}
bool comp2a(Point_2 pt1, Point_2 pt2) { 
    if(pt1.y()==pt2.y()){
        return pt1.x()<pt2.x();
    }
    else
    return (pt1.y() < pt2.y());}
bool comp2b(Point_2 pt1, Point_2 pt2) {
    if(pt1.y()==pt2.y()){
        return pt1.x()>pt2.x();
    }
    else 
        return (pt1.y() > pt2.y());}


Points init_1a(Points p){
    std::sort(p.begin(), p.end(), comp1a);
    return p;
}

Points init_1b(Points p){
    std::sort(p.begin(), p.end(), comp1b);
    return p;
}
Points init_2a(Points p){
    std::sort(p.begin(), p.end(), comp2a);
    return p;
}
Points init_2b(Points p){
    std::sort(p.begin(), p.end(), comp2b);
    return p;
}


segments incremental_min(Points result, Points curr_points, segments convex_seg, segments chain_seg){    
  Points convex_hull;
  Points red_points;
  segments red_segments_final;
  Polygon_2 p1;
  Polygon_2 keep;
  Areas areas;
  Areas find;

  while (result.size() != 0)
  {
    red_points.clear();
    convex_hull.clear();
    convex_seg.clear();
    red_segments_final.clear();
    
    CGAL::convex_hull_2(curr_points.begin(), curr_points.end(), std::back_inserter(convex_hull)); // create convex hull for curr points
    
    int numberofsegments = convex_hull.size();
    int i;

    for (i = 0; i < numberofsegments; i++) // initialise triangle
    {
      if (i == numberofsegments - 1)
      {
        convex_seg.push_back(Segment_2(convex_hull[numberofsegments - 1], convex_hull[0]));
        break;
      }

      convex_seg.push_back(Segment_2(convex_hull[i], convex_hull[i + 1]));
    }
    Point_2 k = result[0]; //find new point k 

    for (int i = 0; i < convex_hull.size(); i++)
    { 
      int res = find_red_segments(Segment_2(k, convex_hull[i]), convex_hull, convex_seg,0);   //find red segments 
      if (!res)
      { 
        red_points.push_back(convex_hull[i]);
      }
      
    }


    if (!red_points.empty())    //if some edges of the convex hull are red try to find if the match any side of the polygon
    { 
      for (int i = 0; i < red_points.size(); i++)
      {
        for (int j = 1; j < red_points.size(); j++)
        {
          Segment_2 temp = edge_exists(red_points[i], red_points[j], convex_seg);
          if(edge_exists(temp[0], temp[1], chain_seg) != error){
            if(std::find(red_segments_final.begin(), red_segments_final.end(), edge_exists(temp[0], temp[1], chain_seg))==red_segments_final.end()){  //check if segment of 2 red points exists in polygon
              int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), convex_hull, convex_seg,1);   //check if middle of the red segment is visible
              if(!res){   
                red_segments_final.push_back(edge_exists(temp[0], temp[1], chain_seg));
              }  
            }
          }
        }
      }
    }
    
    red_points.clear();
    Points tempp=curr_points;
    // if no visible edges in convex hull then search in polygon chain
    if (red_segments_final.size() == 0)
    {
      for (int i = 0; i < curr_points.size(); i++)
      { 
        int res = find_red_segments(Segment_2(k, curr_points[i]), curr_points, chain_seg,0);
        if (!res)
        {                                                   
          red_points.push_back(curr_points[i]);       //find red edges 
        }
      } 
      if (!red_points.empty())      //for every combination of these red edges find if they match a segment of the polygon chain 
      { 
        for (int i = 0; i < red_points.size(); i++)
        {
          for (int j = 1; j < red_points.size(); j++)
          {
            Segment_2 temp = edge_exists(red_points[i], red_points[j], chain_seg);
            if (temp!=error)
            { 
              if(std::find(red_segments_final.begin(), red_segments_final.end(), temp)==red_segments_final.end())
              {
                int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), curr_points, chain_seg,1);  //check if middle of the segment is visible to the new point
                if(!res){
                  red_segments_final.push_back(temp);
                }
              }
            }
          }
        }
      }
    }

    int min=0;
    int pos;
    int min_index=0;
    while (min<red_segments_final.size())
    {

      int i = 0;
      while (i < chain_seg.size())
      {
        if (red_segments_final[min] == chain_seg[i])
        {
          pos = i;
          break;
        }
        i++;
      }
      Segment_2 tempppp = chain_seg[pos];
      Point_2 temppp = Point_2(chain_seg[pos][1]);
      curr_points.push_back(k);
      chain_seg.insert(chain_seg.begin() + pos, Segment_2(chain_seg[pos][0], k));
      // insert in chain at the position that the visible edge was found the new edge connecting with the interior point
      chain_seg.insert(chain_seg.begin() + pos + 1, Segment_2(k, temppp));
      chain_seg.erase(chain_seg.begin() + pos + 2);
      result.erase(result.begin());

      p1.clear();
      keep.clear();
      int e = 0;
      p1.push_back(chain_seg[e][0]);    // initialize the new polygon
      keep.push_back(chain_seg[e][0]); // make a copy of this polygon
      p1.push_back(chain_seg[e][1]); 
      keep.push_back(chain_seg[e][1]);
      e++;
      while (e < chain_seg.size() - 1)
      {
        p1.push_back(chain_seg[e][1]);
        keep.push_back(chain_seg[e][1]);
        e++;
      }

      if (p1.is_simple() == 0) // if with the new edges the polugon is not simple or the polugon does not surround every point i am backtracking
      {
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.insert(chain_seg.begin() + pos, tempppp);                                                 // backtracking deleting the edges i created and bringing back the previous edge
        result.insert(result.begin(),k) ;                                  
        curr_points.pop_back();                                           // pushing back again the interior point
      }
      else
      {
        double re;
        CGAL::area_2(p1.begin(),p1.end(),re,K());
        areas.push_back(re);
        find.push_back(min);     //save the indexes of the red segments 
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.insert(chain_seg.begin() + pos, tempppp);                                                 // backtracking deleting the edges i created and bringing back the previous edge
        result.insert(result.begin(),k) ;                                  
        curr_points.pop_back(); 
      }
      min++;
    }

    i = 0;
    int index=std::distance(std::begin(areas), std::min_element(std::begin(areas), std::end(areas)));   //find the of the segment which creates the minimum area
    min_index=find[index];
    while (i < chain_seg.size())
    {
      if (red_segments_final[min_index] == chain_seg[i])
      {
        pos = i;
        break;
      }
      i++;
    }
    Segment_2 tempppp = chain_seg[pos];

    Point_2 temppp = Point_2(chain_seg[pos][1]);

    curr_points.push_back(k);
    chain_seg.insert(chain_seg.begin() + pos, Segment_2(chain_seg[pos][0], k));
    // insert in chain at the position that the visible edge was found the new edge connecting with the interior point
    chain_seg.insert(chain_seg.begin() + pos + 1, Segment_2(k, temppp));
    chain_seg.erase(chain_seg.begin() + pos + 2);
    result.erase(result.begin());

    p1.clear();
    keep.clear();
    int e = 0;
    p1.push_back(chain_seg[e][0]);    // initialize the new polygon
    keep.push_back(chain_seg[e][0]); // make a copy of this polygon
    p1.push_back(chain_seg[e][1]);
    keep.push_back(chain_seg[e][1]);
    e++;
    while (e < chain_seg.size() - 1)
    {
      p1.push_back(chain_seg[e][1]);
      keep.push_back(chain_seg[e][1]);
      e++;
    }
    find.clear();
    areas.clear();
  }
  return chain_seg;
}


segments incremental_max(Points result, Points curr_points, segments convex_seg, segments chain_seg){   //same exact function as the above but creates the polygon with the max area
  Points convex_hull;
  Points red_points;
  segments red_segments_final;
  Polygon_2 p1;
  Polygon_2 keep;

  Areas areas;
  Areas find;

  while (result.size() != 0)
  {
    red_points.clear();
    convex_hull.clear();
    convex_seg.clear();
    red_segments_final.clear();
    
    CGAL::convex_hull_2(curr_points.begin(), curr_points.end(), std::back_inserter(convex_hull)); // create convex hull for curr points
    
    int numberofsegments = convex_hull.size();
    int i;

    for (i = 0; i < numberofsegments; i++) // arxikopoiw to convex_seg me ta 3 segments
    {
      if (i == numberofsegments - 1)
      {
        convex_seg.push_back(Segment_2(convex_hull[numberofsegments - 1], convex_hull[0]));
        break;
      }

      convex_seg.push_back(Segment_2(convex_hull[i], convex_hull[i + 1]));
    }


    Point_2 k = result[0]; // brhskw to neo shmeio k
    for (int i = 0; i < convex_hull.size(); i++)
    { 
      int res = find_red_segments(Segment_2(k, convex_hull[i]), convex_hull, convex_seg,0);
      
      if (!res)
      {
        red_points.push_back(convex_hull[i]);
      }
      
    }

    if (!red_points.empty())
    {
      for (int i = 0; i < red_points.size(); i++)
      {
        for (int j = 1; j < red_points.size(); j++)
        {
          Segment_2 temp = edge_exists(red_points[i], red_points[j], convex_seg);
          if(edge_exists(temp[0], temp[1], chain_seg) != error){
            if(std::find(red_segments_final.begin(), red_segments_final.end(), edge_exists(temp[0], temp[1], chain_seg))==red_segments_final.end()){
              int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), convex_hull, convex_seg,1);
              if(!res){
                red_segments_final.push_back(edge_exists(temp[0], temp[1], chain_seg));
              }  
            }
          }
        }
      }
    }
    
    red_points.clear();
    Points tempp=curr_points;
    // if no visible edges in convex hull then search in polygon chain
    if (red_segments_final.size() == 0)
    {
      for (int i = 0; i < curr_points.size(); i++)
      { 
        int res = find_red_segments(Segment_2(k, curr_points[i]), curr_points, chain_seg,0);
        if (!res)
        { 
          red_points.push_back(curr_points[i]);
        }
      } 
      if (!red_points.empty())
      { 
        for (int i = 0; i < red_points.size(); i++)
        {
          for (int j = 1; j < red_points.size(); j++)
          {
            Segment_2 temp = edge_exists(red_points[i], red_points[j], chain_seg);
            if (temp!=error)
            { // for every combination of red edges check if they match a segment in polygon chain
              if(std::find(red_segments_final.begin(), red_segments_final.end(), temp)==red_segments_final.end())
              {
                int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), curr_points, chain_seg,1);
                if(!res){
                  red_segments_final.push_back(temp);
                }
              }
            }
          }
        }
      }
    }
    int max=0;
    int pos;
    int max_index=0;

    while (max<red_segments_final.size())
    {
      int i = 0;
      while (i < chain_seg.size())
      {
        if (red_segments_final[max] == chain_seg[i])
        {
          pos = i;
          break;
        }
        i++;
      }
      Segment_2 tempppp = chain_seg[pos];
      Point_2 temppp = Point_2(chain_seg[pos][1]);
      curr_points.push_back(k);
      chain_seg.insert(chain_seg.begin() + pos, Segment_2(chain_seg[pos][0], k));
 // insert in chain at the position that the visible edge was found the new edge connecting with the interior point
      chain_seg.insert(chain_seg.begin() + pos + 1, Segment_2(k, temppp));
      chain_seg.erase(chain_seg.begin() + pos + 2);
      result.erase(result.begin());

      p1.clear();
      keep.clear();
      int e = 0;
      p1.push_back(chain_seg[e][0]);    // initialize the new polygon
      keep.push_back(chain_seg[e][0]); // make a copy of this polygon
      p1.push_back(chain_seg[e][1]); 
      keep.push_back(chain_seg[e][1]);
      e++;
      while (e < chain_seg.size() - 1)
      {
        p1.push_back(chain_seg[e][1]);
        keep.push_back(chain_seg[e][1]);
        e++;
      }

      if (p1.is_simple() == 0) // if with the new edges the polugon is not simple or the polugon does not surround every point i am backtracking
      {
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.insert(chain_seg.begin() + pos, tempppp);                                                 // backtracking deleting the edges i created and bringing back the previous edge
        result.insert(result.begin(),k) ;                                  
        curr_points.pop_back();                                           // pushing back again the interior point
      }
      else
      {
        double re;
        CGAL::area_2(p1.begin(),p1.end(),re,K());
        areas.push_back(re);
        find.push_back(max);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.insert(chain_seg.begin() + pos, tempppp);                                                 // backtracking deleting the edges i created and bringing back the previous edge
        result.insert(result.begin(),k) ;                                  
        curr_points.pop_back(); 
      }
      max++;
    }
    int index = std::distance(std::begin(areas), std::max_element(std::begin(areas), std::end(areas)));
    max_index = find[index];
    i = 0;
    while (i < chain_seg.size())
    {
      if (red_segments_final[max_index] == chain_seg[i])
      {
        pos = i;
        break;
      }
      i++;
    }
    Segment_2 tempppp = chain_seg[pos];
    Point_2 temppp = Point_2(chain_seg[pos][1]);

    curr_points.push_back(k);
    chain_seg.insert(chain_seg.begin() + pos, Segment_2(chain_seg[pos][0], k));
    // insert in chain at the position that the visible edge was found the new edge connecting with the interior point
    chain_seg.insert(chain_seg.begin() + pos + 1, Segment_2(k, temppp));
    chain_seg.erase(chain_seg.begin() + pos + 2);
    result.erase(result.begin());

    p1.clear();
    keep.clear();
    int e = 0;
    p1.push_back(chain_seg[e][0]);    // initialize the new polygon
    keep.push_back(chain_seg[e][0]); // make a copy of this polygon
    p1.push_back(chain_seg[e][1]);
    keep.push_back(chain_seg[e][1]);
    e++;
    while (e < chain_seg.size() - 1)
    {
      p1.push_back(chain_seg[e][1]);
      keep.push_back(chain_seg[e][1]);
      e++;
    }
    find.clear();
    areas.clear();
  }


  return chain_seg;
}


segments incremental(Points result, Points curr_points, segments convex_seg, segments chain_seg){   //random icremental 
  srand((unsigned)time(NULL));

  Points convex_hull;
  Points red_points;

  segments red_segments_convex;
  segments red_segments_chain;
  segments red_segments_final;
  Polygon_2 p1;
  Polygon_2 keep;

  while (result.size() != 0)
  {
    red_points.clear();
    convex_hull.clear();
    convex_seg.clear();
    red_segments_final.clear();
    
    CGAL::convex_hull_2(curr_points.begin(), curr_points.end(), std::back_inserter(convex_hull)); // create convex hull for curr points
    
    int numberofsegments = convex_hull.size();
    int i;

    for (i = 0; i < numberofsegments; i++) //initialise triangle
    {
      if (i == numberofsegments - 1)
      {
        convex_seg.push_back(Segment_2(convex_hull[numberofsegments - 1], convex_hull[0]));
        break;
      }

      convex_seg.push_back(Segment_2(convex_hull[i], convex_hull[i + 1]));
    }


    Point_2 k = result[0]; //new point k
    for (int i = 0; i < convex_hull.size(); i++)
    { 
      int res = find_red_segments(Segment_2(k, convex_hull[i]), convex_hull, convex_seg,0);
      
      if (!res)
      { 
        red_points.push_back(convex_hull[i]);
      }
      
    }

    if (!red_points.empty())
    {
      for (int i = 0; i < red_points.size(); i++)
      {
        for (int j = 1; j < red_points.size(); j++)
        {
          Segment_2 temp = edge_exists(red_points[i], red_points[j], convex_seg);
          if(edge_exists(temp[0], temp[1], chain_seg) != error){
            if(std::find(red_segments_final.begin(), red_segments_final.end(), edge_exists(temp[0], temp[1], chain_seg))==red_segments_final.end()){
              int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), convex_hull, convex_seg,1);
              if(!res){
                red_segments_final.push_back(edge_exists(temp[0], temp[1], chain_seg));
              }  
            }
          }
        }
      }
    }
    
    red_points.clear();
    Points tempp=curr_points;
    // if no visible edges in convex hull then search in polygon chain
    if (red_segments_final.size() == 0)
    {
      for (int i = 0; i < curr_points.size(); i++)
      { 
        int res = find_red_segments(Segment_2(k, curr_points[i]), curr_points, chain_seg,0);
        if (!res)
        { 
          red_points.push_back(curr_points[i]);
        }
      } 
      if (!red_points.empty())
      { 
        for (int i = 0; i < red_points.size(); i++)
        {
          for (int j = 1; j < red_points.size(); j++)
          {
            Segment_2 temp = edge_exists(red_points[i], red_points[j], chain_seg);
            if (temp!=error)
            { // for every combination of red edges check if they match a segment in CH
              if(std::find(red_segments_final.begin(), red_segments_final.end(), temp)==red_segments_final.end())
              {
                int res = find_red_segments(Segment_2(k, midpoint(edge_exists(temp[0], temp[1], chain_seg))), curr_points, chain_seg,1);
                if(!res){
                  red_segments_final.push_back(temp);
                }
              }
            }
          }
        }
      }
    }
    while (red_segments_final.size() > 0)
    {
      int random = rand() % red_segments_final.size();   //find a random segment

      int i = 0;
      int pos;
      while (i < chain_seg.size())
      {
        if (red_segments_final[random] == chain_seg[i])
        {
          pos = i;
          break;
        }
        i++;
      }
      Segment_2 tempppp = chain_seg[pos];
      Point_2 temppp = Point_2(chain_seg[pos][1]);
      curr_points.push_back(k);
      chain_seg.insert(chain_seg.begin() + pos, Segment_2(chain_seg[pos][0], k));
 // insert in chain at the position that the visible edge was found the new edge connecting with the interior point
      chain_seg.insert(chain_seg.begin() + pos + 1, Segment_2(k, temppp));
      chain_seg.erase(chain_seg.begin() + pos + 2);
      result.erase(result.begin());
      p1.clear();
      keep.clear();
      int e = 0;
      p1.push_back(chain_seg[e][0]);    // initialize the new polygon
      keep.push_back(chain_seg[e][0]); // make a copy of this polygon
      p1.push_back(chain_seg[e][1]);
      keep.push_back(chain_seg[e][1]);
      e++;
      while (e < chain_seg.size() - 1)
      {
        p1.push_back(chain_seg[e][1]);
        keep.push_back(chain_seg[e][1]);
        e++;
      }

      if (p1.is_simple() == 0) // if with the new edges the polugon is not simple or the polugon does not surround every point i am backtracking
      {
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.erase(chain_seg.begin() + pos);
        chain_seg.insert(chain_seg.begin() + pos, tempppp);                                                 // backtracking deleting the edges i created and bringing back the previous edge
        result.insert(result.begin(),k) ;                                  
        curr_points.pop_back();                                           // pushing back again the interior point
        red_segments_final.erase(std::find(red_segments_final.begin(), red_segments_final.end(), tempppp)); // deleting the visible edges because it doesnt meet the criteria
      }
      else
      {
        break;
      }
    }
  }

  return chain_seg;
}

int find_red_segments(Segment_2 k, Points convex_hull, segments chain1, int mid){   //returns 0 if there is no intersection of k with any of the segments of the chain1
    int flag;
    int counter=0;

    for(int i=0; i<chain1.size(); i++){
      auto result = CGAL::intersection(k, chain1[i]);
      if(result)
            {
            if (const Segment_2 *s = boost::get<Segment_2>(&*result))
            {
              flag = 1;
              counter++;
              break;
            }
            else
            {
              Point_2 *p = boost::get<Point_2>(&*result);
              if (std::find(convex_hull.begin(), convex_hull.end(), *p) != convex_hull.end() && mid==0) //check if p is an edge of the chain1
              {
                flag = 0;
              }
              else if(mid == 1 && *p == k[1]){   //if we check the middle point of a segment the only point of intersection between that and the new point should be only itself
                flag=0;
              }
              else
                {
                flag = 1;
                counter++;
                break;
                }
              }
            }

    }

    return counter == 0 ? 0 : 1;
    
}


Segment_2 edge_exists(Point_2 a, Point_2 b, segments chain1){     //returns segment if seg(a,b) exists in chain1 else returns error segment
  if (std::find(chain1.begin(), chain1.end(), Segment_2(a, b)) != chain1.end())
      return Segment_2(a, b);
  else if (std::find(chain1.begin(), chain1.end(), Segment_2(b, a)) != chain1.end())
      return Segment_2(b, a);
  else
    return error;
}


segments create_segments(Points p){   //give ordered set of points to create vector of segments
  segments s;
  for(int i=0; i<p.size(); i++){
    if(i!=p.size()-1) 
      s.push_back(Segment_2(p[i],p[i+1]));
  }
  s.push_back(Segment_2(p[p.size()-1], p[0]));

  return s;
}