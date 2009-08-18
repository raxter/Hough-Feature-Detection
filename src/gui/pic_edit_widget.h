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
  
  void performCircleFind();

  private slots:
  void createFilteredImage();
  void houghTransform();
  void displayHough( int value );
  void findCircles();
  
  void thresholdDoubleSpinBoxValueChanged(double d);
  void displayImageCheckBoxStateChanged(int i);
  
  unsigned int minThreshold (unsigned int radius);
  
  private: /* overloaded functions */
  
  private: /* functions */
  void plotHoughCircle(int xCenter, int yCenter, int radius);
  void plotFinalCircle(int xCenter, int yCenter, int radius);
 
  private: /* variables */
  QImage raw_image;
  QImage final_image;
  QImage filtered_image;
  QImage hough_image;
  
  //QMap<int, QImage> hough_images;
  QMap< unsigned int, QVector<QVector< unsigned int> > > hough_output;
 

};


} /* end namespace Gui */

} /* end namespace Hough */

#endif

