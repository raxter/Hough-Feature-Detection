#include "pic_edit_widget.h"
#include "pic_display_widget.h"

#include <QDebug>
#include <QColor>


#include <cmath>

namespace Hough
{

namespace Gui
{

/****************************************************************************
**
** Author: Richard Baxter
**
** Default Constructor
**
****************************************************************************/
PicEditWidget::PicEditWidget(const QString& filename) : raw_image(QImage(filename)) {
  
  setupUi(this);
  
  setAllowedAreas ( Qt::NoDockWidgetArea );
  
  raw_image.convertToFormat(QImage::Format_RGB888);
  
  PicDisplayWidget* rawPicWidget = new PicDisplayWidget(raw_image);
  rawPicArea->layout()->addWidget(rawPicWidget);
  
  PicDisplayWidget* filteredPicWidget = new PicDisplayWidget(filtered_image);
  filteredPicArea->layout()->addWidget(filteredPicWidget);
  
  PicDisplayWidget* houghPicWidget = new PicDisplayWidget(raw_image);
  houghPicArea->layout()->addWidget(houghPicWidget);
  
  connect(filterButton, SIGNAL(released ()), this , SLOT(createFilteredImage()));
  
  connect(houghTranButton, SIGNAL(released ()), this , SLOT(houghTransform()));
  connect(houghTranButton, SIGNAL(released ()), houghPicWidget , SLOT(repaint()));
  
  connect(this, SIGNAL(updateHoughPic( const QImage& )), houghPicWidget, SLOT( setImage( const QImage& ) ) );
  connect(currentHoughSpinBox, SIGNAL (valueChanged ( int )), this, SLOT(displayHough( int )));
  
  show();
}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/

PicEditWidget::~PicEditWidget() {

}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::createFilteredImage() {
  
  filtered_image = QImage(raw_image.size(), QImage::Format_Mono);
  //filtered_image.convertToFormat(QImage::Format_RGB888);
  QVector< QVector <double> > sobel_x(3);
  sobel_x[0].resize(3);
  sobel_x[1].resize(3);
  sobel_x[2].resize(3);
  
  sobel_x [0][0] = +1;  sobel_x [0][1] = +0;  sobel_x [0][1] = -1;
  sobel_x [1][0] = +2;  sobel_x [1][1] = +0;  sobel_x [1][1] = -2;
  sobel_x [2][0] = +1;  sobel_x [2][1] = +0;  sobel_x [2][1] = -1;
  
  QVector< QVector <double> > sobel_y(3);
  sobel_y[0].resize(3);
  sobel_y[1].resize(3);
  sobel_y[2].resize(3);
  
  sobel_y [0][0] = +1;  sobel_y [0][1] = +2;  sobel_y [0][1] = +1;
  sobel_y [1][0] = +0;  sobel_y [1][1] = +0;  sobel_y [1][1] = +0;
  sobel_y [2][0] = -1;  sobel_y [2][1] = -2;  sobel_y [2][1] = -1;
  
  int height = filtered_image.size().height();
  int width = filtered_image.size().width();
  
  for (int x = 0 ; x < width ; x++) for (int y = 0 ; y < height ; y++) 
  {
    double new_x_value = 0;
    double new_y_value = 0;
    for (int r = -1 ; r <= 1 ; r++) for (int c = -1 ; c <= 1 ; c++) {
    
      int nx = x + r;
      int ny = y + c;
      
      if (nx < 0) nx = -nx;
      if (ny < 0) ny = -ny;
      if (nx >= width ) nx = width  + width  - nx - 2;
      if (ny >= height) ny = height + height - ny - 2;
      
      int gray_val = qGray(raw_image.pixel(nx, ny));
      new_x_value += sobel_x[r+1][c+1]*gray_val;
      new_y_value += sobel_y[r+1][c+1]*gray_val;
    }
    int new_value = sqrt((new_x_value*new_x_value) + (new_y_value*new_y_value));
    
    
    if (new_value > 128)
      new_value = 1;
    else
      new_value = 0;
    
    filtered_image.setPixel ( x, y, new_value);
  }
  
  filteredPicArea->repaint();
  
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::houghTransform() {

  hough_images.clear();
  for (int radius = minSpinBox->value() ; radius < maxSpinBox->value() ; radius ++) {
    qDebug() << "performing " << radius << "/" << maxSpinBox->value();
    hough_images[radius] = QImage(filtered_image.size(), QImage::Format_RGB888);
    QImage& im = hough_images[radius];
    
    im.fill(0);

    int height = im.size().height();
    int width = im.size().width();
    for (int x = 0 ; x < width ; x++) for (int y = 0 ; y < height ; y++) 
    {
      if (filtered_image.pixelIndex(x,y) == 1) {
        plotCircle(im, x, y, radius);
        
      }
    }

  }

}

void PicEditWidget::displayHough( int value) {
  
  if(hough_images.contains ( value ))
    emit updateHoughPic( hough_images[value]);
}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
//http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html
void PicEditWidget::plotCircle(QImage& image, int xCenter, int yCenter, int radius)
{
  int r2 = radius * radius;
  int height = image.size().height();
  int width = image.size().width();
  
  for (int x = -radius; x <= radius; x++) {
    int y = (int) (sqrt(r2 - x*x) + 0.5);
    
    int nx = xCenter + x;
    if (nx >= 0 && nx < width ) {
    
      int ny = yCenter + y;
      if (ny >= 0 && ny < height) {
        QColor c (image.pixel(nx,ny));
        c.setRgb ( c.red()+1, c.green()+1, c.blue()+1 );
        image.setPixel(nx, ny, c.rgb());
        
      }
        
      ny = yCenter - y;
      if (ny >= 0 && ny < height) {
      
        QColor c (image.pixel(nx,ny));
        c.setRgb ( c.red()+1, c.green()+1, c.blue()+1 );
        image.setPixel(nx, ny, c.rgb());
        
      }
    }
  }
}



} /* end namespace Gui */

} /* end namespace Hough */


























