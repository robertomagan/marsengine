include_directories(${CMAKE_SOURCE_DIR}/includes) 

add_executable(marsengine marsengine.cpp)
target_link_libraries(marsengine inc ${LCM_LIBRARY} ${LCM_LIBRARY} ${MATLABLIB_LIBRARY} -pthread)
