cmake_minimum_required(VERSION 3.10)
project(
 	iWorkParser	
	VERSION 1.0.0
	LANGUAGES C CXX
)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(CMAKE_BUILD_TYPE Release)
set(protobuf_BUILD_TESTS OFF)

SET(sources)
list(
	APPEND 
	sources
	${CMAKE_CURRENT_SOURCE_DIR}/src/iworks_parser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
)

file(GLOB_RECURSE protobuf_src proto/*.cc)
list(APPEND sources ${protobuf_src})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/proto)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/vcpkg_installed/arm64-osx/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/vcpkg_installed/arm64-osx/lib)


add_executable(iWorkParser ${sources})
TARGET_LINK_LIBRARIES(iWorkParser -lsnappy -lprotobuf -lprotobuf-lite -lprotoc)
