cmake_minimum_required(VERSION 2.8)
project ("TextProc" CXX)

set(CUSTOM_COMPILE_FLAGS "-m64 -std=c++11")

option (TARGET_IS_SHARED_LIB "Make this libarary as SHARED" true)

set(DIST_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../dist")


file (GLOB Sources
  "${PROJECT_SOURCE_DIR}/src/details/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/details/*.h"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
    )
  
include_directories (${PROJECT_SOURCE_DIR})

if (${TARGET_IS_SHARED_LIB})
    add_library (${PROJECT_NAME} SHARED ${Sources})
else (${TARGET_IS_SHARED_LIB})
    add_library (${PROJECT_NAME} ${Sources})
endif(${TARGET_IS_SHARED_LIB})

#target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CUSTOM_COMPILE_FLAGS}")

# CPM_ExportAdditionalLibraryTarget(${PROJECT_NAME})

#only needed for shared libraries
if (${TARGET_IS_SHARED_LIB})
    install (CODE "file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/libs)")
    install (TARGETS ${PROJECT_NAME} DESTINATION libs EXPORT ${PROJECT_NAME}_targets)
endif (${TARGET_IS_SHARED_LIB})
