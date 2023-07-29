1. cmake -DCGAL_DIR=$CMAKE_INSTALLED_PREFIX/lib/CGAL -DCMAKE_BUILD_TYPE=Release .


2. make


3. ./PROJECT_2 -i uniform-0000015-1.instance -o output.txt -algorithm simulated_annealing -L 500 -max -annealing local ./PROJECT -i uniform-0000015-1.instance -o test_output.txt -algorithm incremental -edge_selection 1 -init 1a

or

./PROJECT_2 -i uniform-0000015-1.instance -o output.txt -algorithm local_search -L 500 -max -threshold 0.1 ./PROJECT -i uniform-0000015-1.instance -o test_output.txt -algorithm convex_hull -edge_selection 1
