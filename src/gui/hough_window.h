#ifndef __HOUGH_GUI_HOUGHWINDOW_H__
#define __HOUGH_GUI_HOUGHWINDOW_H__

#include <QMainWindow>

#include "ui_hough_window.h"


namespace Hough
{

namespace Gui
{


class HoughWindow : public QMainWindow, private Ui::HoughWindow
{
  Q_OBJECT

  public: /* class specific */

  HoughWindow();
  ~HoughWindow();


  private slots:
  void openFiles ();
  
  
  private: /* overloaded functions */
  void closeEvent(QCloseEvent *event);
  
  private: /* functions */
  void createRawPicWindow(const QString & filename);
 

};


} /* end namespace Gui */

} /* end namespace Hough */

#endif

