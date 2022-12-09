# Created by the script cgal_create_CMakeLists
# This is the CMake script for compiling a set of CGAL applications.

cmake_minimum_required(VERSION 3.1...3.23)

project( PROJECT_2 )


# CGAL and its components

find_package( CGAL QUIET COMPONENTS  )


if ( NOT CGAL_FOUND )

  message(STATUS "This project requires the CGAL library, and will not be compiled.")
  return()  

endif()


# Boost and its components
find_package( Boost REQUIRED )

if ( NOT Boost_FOUND )

  message(STATUS "This project requires the Boost library, and will not be compiled.")

  return()  

endif()


# include for local directory
add_subdirectory(include)
list(APPEND EXTRA_LIBS Area_maximization_minimization)
# include for local package



# ############################

add_executable( PROJECT_2 PROJECT_2.cpp )

add_to_cached_list( CGAL_EXECUTABLE_TARGETS PROJECT_2 )

# Link the executable to CGAL and third-party libraries

target_link_libraries(PROJECT_2 PRIVATE CGAL::CGAL ${EXTRA_LIBS})

