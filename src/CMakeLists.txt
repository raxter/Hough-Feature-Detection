PROJECT(HOUGH)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${HOUGH_SOURCE_DIR} ${HOUGH_BINARY_DIR} )


INCLUDE(${QT_USE_FILE})

# .cxx sources
SET(HOUGH_SRCS_CXX

  gui/hough_window.cpp
  gui/pic_display_widget.cpp
  gui/pic_edit_widget.cpp
  
  
  main.cpp
)


# files which need to be moc'd by Qt
SET(HOUGH_MOC_SRCS

  
  gui/hough_window.h
  gui/pic_display_widget.h
  gui/pic_edit_widget.h
)


SET(HOUGH_UI

  gui/hough_window.ui
  gui/pic_edit_widget.ui

)


# build ui_XXX files from the XML-style .ui files
QT4_WRAP_UI(HOUGH_SRCS_CXX ${HOUGH_UI})
# this moc's the above variable and appends to the cxx sources
QT4_WRAP_CPP(HOUGH_SRCS_CXX ${HOUGH_MOC_SRCS})


ADD_EXECUTABLE(hough ${HOUGH_SRCS_CXX})


TARGET_LINK_LIBRARIES(hough ${QT_LIBRARIES})



