cmake_minimum_required(VERSION 2.6)


set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMAKE_Modules/")
find_package(Qt4 REQUIRED QtXml QtGui QtCore)
find_package(OpenCV REQUIRED)
add_definitions(-DEMULATE_CAMERA)
include_directories(${QT_INCLUDES} 
      ${CMAKE_CURRENT_BINARY_DIR}
      ${OPENCV_INCLUDE_DIR}
      )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")		    
add_definitions(${QT_DEFINITIONS})

set(QtGigEEmulator_SRCS qtgigeemulator.cc)

qt4_automoc(${QtGigEEmulator_SRCS})

ADD_LIBRARY(QtGigEEmulator STATIC ${QtGigEEmulator_SRCS})
TARGET_LINK_LIBRARIES(QtGigEEmulator
	${OpenCV_LIBS} 
	${QT_QTCORE_LIBRARY} 
	${QT_QTGUI_LIBRARY} 
	${PTHREAD_LIB}
	)
