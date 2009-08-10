#ifndef __HOUGH_GUI_PICDISPLAYWINDOW_H__
#define __HOUGH_GUI_PICDISPLAYWINDOW_H__

#include <QDockWidget>
#include <QImage>
#include <QPaintEvent>


namespace Hough
{

namespace Gui
{


class PicDisplayWidget : public QWidget
{
  Q_OBJECT

  public: /* class specific */

  PicDisplayWidget(const QImage & image);
  ~PicDisplayWidget();

  public slots:
  void setImage( const QImage& image );

  private: /* functions */
  void paintEvent( QPaintEvent *e );

  public: /* variables */
  
  const QImage* image;
  
  private slots:
  
  private: /* overloaded functions */
  

};


} /* end namespace Gui */

} /* end namespace Hough */

#endif

