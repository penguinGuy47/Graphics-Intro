cmake_minimum_required(VERSION 3.25)

project(final_project2)

LINK_DIRECTORIES( ${CMAKE_BINARY_DIR}/lib )
LINK_DIRECTORIES( ${CMAKE_BINARY_DIR}/bin )

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# For multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

find_package(OpenGL REQUIRED )
if (OpenGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    link_libraries(${OpenGL_LIBRARIES})
endif()	

find_package(ASSIMP REQUIRED)
if (ASSIMP_FOUND)
    include_directories(${ASSIMP_INCLUDE_DIR})
    link_libraries(${ASSIMP_LIBRARIES})
endif()	

find_package(GLM REQUIRED)
if (GLM_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
    link_libraries(${GLM_LIBRARIES})
endif()

find_package(GLFW3 REQUIRED)
include_directories(${GLFW3_INCLUDE_DIR})
link_libraries(${GLFW3_LIBRARY})

find_package(GLAD REQUIRED)
include_directories(${GLAD_INCLUDE_PATH})
link_libraries(${GLAD_LIBRARIES})

find_package(STBI REQUIRED)
include_directories(${STBI_INCLUDE_DIR})
link_libraries(${STBI_LIBRARY})

set(COMMON_LIBS optimized ${OpenGL_LIBRARIES} ${GLFW3_LIBRARY} ${STBI_LIBRARY} ${GLM_LIBRARIES} ${ASSIMP_LIBRARIES})
set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET(APP_SRCS1
  source/question01.cpp
)

SET(APP_SRCS2
  source/final.cpp
)

SET(APP_SRCS3
  source/question03.cpp
)

SET(APP_COMMON
  common/loadShaders.cpp
)

SET(APP_HDRS
	include/mat.h
	include/vec.h
	include/shader.h
	include/shader_m.h
	include/shader_s.h
	include/camera.h
	include/filesystem.h
	include/loadShaders.h
	include/model.h
	include/mesh.h
)

SET(APP_SHADERS1
	shader/1.model_loading.vert
	shader/1.model_loading.frag
	shader/lighting_maps.vert
	shader/lighting_maps.frag
	shader/lamp.vert
	shader/lamp.frag
)

SET(APP_SHADERS2
	shader/2.skybox.frag
	shader/2.skybox.vert
	shader/1.model_loading.vert
	shader/1.model_loading.frag
	shader/planeShader.vert
	shader/planeShader.frag
)

SET(APP_SHADERS3
	shader/2.skybox.frag
	shader/2.skybox.vert
	shader/1.model_loading.vert
	shader/1.model_loading.frag
)

SOURCE_GROUP("Common Files" FILES
  ${APP_COMMON}
)

SOURCE_GROUP("Shader Files" FILES
 ${APP_SHADERS1} 
 ${APP_SHADERS2} 
 ${APP_SHADERS3}
)

SOURCE_GROUP("Source Files" FILES
 ${APP_SRCS1}   
 ${APP_SRCS2}  
 ${APP_SRCS3} 
)

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

add_executable(question01 ${APP_SRCS1} ${APP_COMMON}  ${APP_HDRS}  ${APP_SHADERS1})
target_link_libraries(question01  ${COMMON_LIBS})

add_executable(final ${APP_SRCS2} ${APP_COMMON}  ${APP_HDRS}  ${APP_SHADERS2})
target_link_libraries(final  ${COMMON_LIBS})

add_executable(question03 ${APP_SRCS3} ${APP_COMMON}  ${APP_HDRS}  ${APP_SHADERS3})
target_link_libraries(question03  ${COMMON_LIBS})

include_directories( include )

ADD_CUSTOM_TARGET(debug ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${project_binary_dir})
ADD_CUSTOM_TARGET(release ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${project_binary_dir})
