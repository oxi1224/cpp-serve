cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(cpp-serve)

include_directories("include")

set(INCLUDE_SRC
	"include/constants.hpp"
	"include/cookie.hpp"
	"include/http.hpp"
	"include/request.hpp"
	"include/response.hpp"
	"include/util.hpp"
)

set(SOURCES
	"src/constants.cpp"
	"src/cookie.cpp"
	"src/http.cpp"
	"src/request.cpp"
	"src/response.cpp"
	"src/util.cpp"
)

# add_executable(${CMAKE_PROJECT_NAME} "main.cpp" ${SOURCES} ${INCLUDE_SRC})

add_library(${CMAKE_PROJECT_NAME} ${INCLUDE_SRC} ${SOURCES})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)

install(TARGETS ${CMAKE_PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY include/ DESTINATION include)