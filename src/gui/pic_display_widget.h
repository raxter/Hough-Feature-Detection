#ifndef __HOUGH_GUI_PICDISPLAYWINDOW_H__
#define __HOUGH_GUI_PICDISPLAYWINDOW_H__

#include <QDockWidget>
#include <QLabel>


namespace Hough
{

namespace Gui
{


class PicDisplayWidget : public QDockWidget
{
  Q_OBJECT

  public: /* class specific */

  PicDisplayWidget(const QString& filename);
  ~PicDisplayWidget();


  public: /* variables */
 
  QImage image;
  
  
  private slots:
  
  private: /* overloaded functions */
  
  QLabel* label;

};


} /* end namespace Gui */

} /* end namespace Hough */

#endif

