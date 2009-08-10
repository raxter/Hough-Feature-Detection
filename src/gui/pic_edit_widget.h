#ifndef __HOUGH_GUI_PICEDITWIDGET_H__
#define __HOUGH_GUI_PICEDITWIDGET_H__

#include <QDockWidget>

#include "ui_pic_edit_widget.h"


namespace Hough
{

namespace Gui
{


class PicEditWidget : public QDockWidget, private Ui::PicEditWidget
{
  Q_OBJECT

  public: /* class specific */

  PicEditWidget(const QString& filename);
  ~PicEditWidget();

  signals:
  void updateHoughPic( const QImage& );

  private slots:
  void createFilteredImage();
  void houghTransform();
  void displayHough( int value );
  
  private: /* overloaded functions */
  
  private: /* functions */
  void plotCircle(QImage& image, int xCenter, int yCenter, int radius);
 
  private: /* variables */
  QImage raw_image;
  QImage filtered_image;
  QMap<int, QImage> hough_images;
 

};


} /* end namespace Gui */

} /* end namespace Hough */

#endif

