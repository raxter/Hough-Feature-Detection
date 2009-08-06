
#include <QApplication>

#include "gui/hough_window.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  Hough::Gui::HoughWindow houghWin;//(app);
  
  houghWin.show();
  
  return app.exec();
}
