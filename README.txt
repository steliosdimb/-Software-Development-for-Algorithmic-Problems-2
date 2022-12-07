1. cmake -DCGAL_DIR=$CMAKE_INSTALLED_PREFIX/lib/CGAL -DCMAKE_BUILD_TYPE=Release .


2. make


3. ./PROJECT_2 -i uniform-0000100-1.instance -o output.txt -algorithm simulated_annealing -L 500 -max -annealing subdivision ./PROJECT -i uniform-0000100-1.instance -o test_output.txt -algorithm incremental -edge_selection 1 -init 1a

