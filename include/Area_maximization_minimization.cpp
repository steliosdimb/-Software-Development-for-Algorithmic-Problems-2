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
    int i;
    int random_position; // position of a random point
    int counter=0;
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
    Point_2 successor_point;   // next point from the random_point
    Point_2 d_successor_point; // next x2 point from the random_point
    Point_2 predecessor_point; // previous point from the random_point
    Points temp;               // holds the points of the fuzzy iso box
    Points convex_hull_points;
    Polygon_2 temp_polygon; // keeps a temp polygon
    tree tr;                // kd tree
    std::vector<Segment_2>::iterator it;
    CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points)); // find the convex hull
    CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
    CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area

    if (flag_min_max == 1)
    {                                                                          // min
        previous_energy = how_many_points * (polygon_area / convex_hull_area); // finding initial energy for minimization
        while (temperature >= 0)
        {
            if (option == 1)
            { // local
                temp.clear();
                convex_hull_points.clear();

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
                do_they_intersect = CGAL::do_intersect(Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point)); // check if pr,qs intersect
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
                    box search_box(predecessor_point, random_point, 0.0);     // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box);          // get the points inside the fuzzy box
                    box search_box1(random_point, successor_point, 0.0);      // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box1);         // get the points inside the fuzzy box
                    box search_box2(successor_point, d_successor_point, 0.0); // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box2);         // get the points inside the fuzzy box
                    sort(temp.begin(), temp.end());
                    temp.erase(unique(temp.begin(), temp.end()), temp.end()); // removing duplicates
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
                }

                // if we have a valid change
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (polygon_area / convex_hull_area);                      // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                // std::cout << DE << " " << previous_energy << " " << sequent_energy << " " << " " << metropolis_criterion << " " << R << std::endl;
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
                temp.clear();
                convex_hull_points.clear();

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
                do_they_intersect = CGAL::do_intersect(Segment_2(predecessor_point, random_point), Segment_2(successor_point, d_successor_point)); // check if pr,qs intersect
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
                    box search_box(predecessor_point, random_point, 0.0);     // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box);          // get the points inside the fuzzy box
                    box search_box1(random_point, successor_point, 0.0);      // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box1);         // get the points inside the fuzzy box
                    box search_box2(successor_point, d_successor_point, 0.0); // construct a fuzzy box with the range of the points you need from predecessor to x2 successor point
                    tr.search(std::back_inserter(temp), search_box2);         // get the points inside the fuzzy box
                    sort(temp.begin(), temp.end());
                    temp.erase(unique(temp.begin(), temp.end()), temp.end()); // removing duplicates
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

                }

                // if we have a valid change
                CGAL::convex_hull_2(p.begin(), p.end(), std::back_inserter(convex_hull_points));           // find the convex hull
                CGAL::area_2(convex_hull_points.begin(), convex_hull_points.end(), convex_hull_area, K()); // compute convex hull area
                CGAL::area_2(p.begin(), p.end(), polygon_area, K());                                       // compute polygons area
                sequent_energy = how_many_points * (1 - (polygon_area / convex_hull_area));                      // new energy
                DE = sequent_energy - previous_energy;                                                     // DE
                metropolis_criterion = exp(-DE / temperature);
                // std::cout << DE << " " << previous_energy << " " << sequent_energy << " " << " " << metropolis_criterion << " " << R << std::endl;
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
            }
            else if (option == 3)
            { // subdivision
            }
        }
    }
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
    if (flag == 0)
    {
        return 0; // we dont have an intersection
    }
    else
    {
        return 1; // we have an intersection
    }
}
