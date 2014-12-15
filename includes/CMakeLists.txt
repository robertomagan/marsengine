add_library(inc SHARED
  thread/threadengine.cpp
  lcm/lcmthread.cpp
  lcm/lcmhandler.cpp
  beans/node.cpp
  lcm/protectedmutex.cpp 
  mengine/mEngine.cpp
  mengine/MEthread.cpp
  motion/MotionMobility.cpp
  motion/Coord.cpp
)
target_link_libraries(inc)
