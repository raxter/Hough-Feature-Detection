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
  
  final_image = raw_image.copy();
  final_image.convertToFormat(QImage::Format_RGB888);
  
  PicDisplayWidget* rawPicWidget = new PicDisplayWidget(raw_image);
  rawPicArea->layout()->addWidget(rawPicWidget);
  
  PicDisplayWidget* filteredPicWidget = new PicDisplayWidget(filtered_image);
  filteredPicArea->layout()->addWidget(filteredPicWidget);
  
  PicDisplayWidget* houghPicWidget = new PicDisplayWidget(raw_image);
  houghPicArea->layout()->addWidget(houghPicWidget);
  
  PicDisplayWidget* finalPicWidget = new PicDisplayWidget(final_image);
  finalPicArea->layout()->addWidget(finalPicWidget);
  
  connect(filterButton, SIGNAL(released ()), this , SLOT(createFilteredImage()));
  
  connect(houghTranButton, SIGNAL(released ()), this , SLOT(houghTransform()));
  connect(houghTranButton, SIGNAL(released ()), houghPicWidget , SLOT(repaint()));
  
  
  connect(thresholdDoubleSpinBox, SIGNAL(valueChanged ( double )), this , SLOT(thresholdDoubleSpinBoxValueChanged( double )));
  connect(displayImageCheckBox, SIGNAL(stateChanged ( int )), this , SLOT(displayImageCheckBoxStateChanged( int )));
  
  connect(this, SIGNAL(performCircleFind ()), this , SLOT(findCircles()));
  connect(this, SIGNAL(performCircleFind ()), finalPicWidget , SLOT(repaint()));
  
  connect(findCirclesButton, SIGNAL(released ()), this , SLOT(findCircles()));
  connect(findCirclesButton, SIGNAL(released ()), finalPicWidget , SLOT(repaint()));
  
  connect(this, SIGNAL(updateHoughPic( const QImage& )), houghPicWidget, SLOT( setImage( const QImage& ) ) );
  connect(currentHoughSpinBox, SIGNAL (valueChanged ( int )), this, SLOT(displayHough( int )));
  
  show();
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::thresholdDoubleSpinBoxValueChanged(double d) {
  emit performCircleFind();
}

void PicEditWidget::displayImageCheckBoxStateChanged(int i) {
  emit performCircleFind();
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
  houghTranButton->setEnabled(true);
  
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::houghTransform() {

  //hough_images.clear();
  hough_output.clear();
  for (int radius = minSpinBox->value() ; radius < maxSpinBox->value() ; radius ++) {
    qDebug() << "performing " << radius << "/" << maxSpinBox->value();
    //hough_images[radius] = QImage(filtered_image.size(), QImage::Format_RGB888);
    hough_output[radius].resize(filtered_image.size().width());
    
    for (int i = 0 ; i < hough_output[radius].size() ; i++)
      hough_output[radius][i].resize(filtered_image.size().height());
    
    //QImage& im = hough_images[radius];
    //im.fill(0);

    int height = filtered_image.size().height();
    int width = filtered_image.size().width();
    for (int x = 0 ; x < width ; x++) {
      for (int y = 0 ; y < height ; y++) {
        if (filtered_image.pixelIndex(x,y) == 1) {
          plotHoughCircle(x, y, radius);
        }
      }
    }
          
  }
  
  if (hough_output.size() > 0) {
    findCirclesButton->setEnabled(true);
    thresholdDoubleSpinBox->setEnabled(true);
    displayHough( currentHoughSpinBox->value() );
  }
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::displayHough( int value) {
  
  if(hough_output.contains ( value )) {
    
    int width = hough_output[value].size();
    int height = hough_output[value][0].size();
    hough_image = QImage ( width, height, QImage::Format_RGB888 );
    for (int x = 0 ; x < width ; x++) {
      for (int y = 0 ; y < height ; y++) {
        QColor c (hough_output[value][x][y], hough_output[value][x][y], hough_output[value][x][y]);
        hough_image.setPixel(x, y, c.rgb());
      }
    }
    emit updateHoughPic( hough_image );
  }
}


 
/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/


void accumulate(QVector <QVector <unsigned int> >& img, int x, int y) {
  
  if (x >= 0 && x < img.size() && y >= 0 && y < img[0].size())
    img[x][y]++;
}

void plot4pointsAccumulate(QVector <QVector <unsigned int> >& img, int cx, int cy, int x, int y)
{
	accumulate(img, cx + x, cy + y);
	if (x != 0) accumulate(img, cx - x, cy + y);
	if (y != 0) accumulate(img, cx + x, cy - y);
	if (x != 0 && y != 0) accumulate(img, cx - x, cy - y);
}
void plot8pointsAccumulate(QVector <QVector <unsigned int> >& img, int cx, int cy, int x, int y)
{
	plot4pointsAccumulate(img, cx, cy, x, y);
	if (x != y) plot4pointsAccumulate(img, cx, cy, y, x);
}
 
void circleAccumulate(QVector <QVector <unsigned int> >& img, int cx, int cy, int radius)
{
	int error = -radius;
	int x = radius;
	int y = 0;
 
	while (x >= y)
	{
		plot8pointsAccumulate(img, cx, cy, x, y);
 
		error += y;
		++y;
		error += y;
 
		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
//http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html

void PicEditWidget::plotHoughCircle(int xCenter, int yCenter, int radius)
{
  QVector <QVector <unsigned int> >& img = hough_output[radius];
  circleAccumulate(img, xCenter, yCenter, radius);
  /*int r2 = radius * radius;
  
  QVector <QVector <unsigned int> >& img = hough_output[radius];
  
  int height = img.size();
  int width = img[height-1].size();
  
  
  for (int x = -radius; x <= radius; x++) {
    int y = (int) (sqrt(r2 - x*x) + 0.5);
    
    //qDebug() << "------------";
    //qDebug() << y << "/" << height << ":" << x << "/" << width;
    int nx = xCenter + x;
    if (nx >= 0 && nx < width ) {
    
      int ny = yCenter + y;
      //qDebug() << ny << "/" << height << ":" << nx << "/" << width;
      if (ny >= 0 && ny < height) {
        img[ny][nx]++;
        //QColor c (image.pixel(nx,ny));
        //c.setRgb ( c.red()+1, c.green()+1, c.blue()+1 );
        //image.setPixel(nx, ny, c.rgb());
      }
        
      ny = yCenter - y;
      //qDebug() << ny << "/" << height << ":" << nx << "/" << width;
      if (ny >= 0 && ny < height) {
        img[ny][nx]++;
        
        //QColor c (image.pixel(nx,ny));
        //c.setRgb ( c.red()+1, c.green()+1, c.blue()+1 );
        //image.setPixel(nx, ny, c.rgb());
        
      }
    }
  }*/
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void plot4pointsDraw(QImage& img, int cx, int cy, int x, int y)
{
	img.setPixel(cx + x, cy + y, 255);
	if (x != 0) img.setPixel(cx - x, cy + y, 255);
	if (y != 0) img.setPixel(cx + x, cy - y, 255);
	if (x != 0 && y != 0) img.setPixel(cx - x, cy - y, 255);
}
void plot8pointsDraw(QImage& img, int cx, int cy, int x, int y)
{
	plot4pointsDraw(img, cx, cy, x, y);
	if (x != y) plot4pointsDraw(img, cx, cy, y, x);
}
 
void circleDraw(QImage& img, int cx, int cy, int radius)
{
	int error = -radius;
	int x = radius;
	int y = 0;
 
	while (x >= y)
	{
		plot8pointsDraw(img, cx, cy, x, y);
 
		error += y;
		++y;
		error += y;
 
		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}


void PicEditWidget::plotFinalCircle(int xCenter, int yCenter, int radius)
{
  circleDraw(final_image, xCenter, yCenter, radius);
  
  /*int r2 = radius * radius;
  
  int height = final_image.size().height();
  int width =  final_image.size().width();
  
  QColor c;
  c.setRgb ( 0, 255, 0);
  
  for (int x = -radius; x <= radius; x++) {
    int y = (int) (sqrt(r2 - x*x) + 0.5);
    
    //qDebug() << "------------";
    //qDebug() << y << "/" << height << ":" << x << "/" << width;
    int nx = xCenter + x;
    if (nx >= 0 && nx < width ) {

      int ny = yCenter + y;
      //qDebug() << ny << "/" << height << ":" << nx << "/" << width;
      if (ny >= 0 && ny < height) {
        final_image.setPixel(nx, ny, 255);
      }
        
      ny = yCenter - y;
      //qDebug() << ny << "/" << height << ":" << nx << "/" << width;
      if (ny >= 0 && ny < height) {
        final_image.setPixel(nx, ny, 255);
        
      }
    }
  }*/
}

inline unsigned int PicEditWidget::minThreshold (unsigned int radius) {
  return thresholdDoubleSpinBox->value()*radius;
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::findCircles() {
  
  final_image = raw_image.copy();
  if (displayImageCheckBox->checkState () == Qt::Unchecked)
    final_image.fill(0);
    
  Q_FOREACH(unsigned int radius, hough_output.keys () ) {
    QVector<QVector < unsigned int > >& img = hough_output[radius];
    
    qDebug() << "checking radius " << radius;
    for (int x = 0 ; x < img.size() ; x++) {
      for (int y = 0 ; y < img[x].size() ; y++) {
        if ( img[x][y] > minThreshold(radius)) {
          qDebug() << "value of " << img[x][y] << " for radius " << radius << "(checked against " << minThreshold(radius) << " -> " << ((double)img[x][y]/radius) << ") found at (" << x << ", "<< y << ")";
          plotFinalCircle(x,y,radius);
        }
      }
    }
    
  }
  
  //plotFinalCircle(40,40,30);
}


} /* end namespace Gui */

} /* end namespace Hough */


























