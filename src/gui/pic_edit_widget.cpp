#include "pic_edit_widget.h"
#include "pic_display_widget.h"

#include <QDebug>
#include <QColor>
#include <QFileDialog>


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
  
  int val = raw_image.width();
  if (val > raw_image.height())
    val = raw_image.height();
  maxSpinBox->setValue(val/2);
  
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
  
  connect(saveButton, SIGNAL(released()), this, SLOT(saveFile()));
  
  //connect(thresholdDoubleSpinBox, SIGNAL(valueChanged ( double )), this , SLOT(thresholdDoubleSpinBoxValueChanged( double )));
  //connect(displayImageCheckBox, SIGNAL(stateChanged ( int )), this , SLOT(displayImageCheckBoxStateChanged( int )));
  connect(thresholdDoubleSpinBox, SIGNAL(valueChanged ( double )), this , SLOT(emitEnableFindButton( double )));
  connect(displayImageCheckBox, SIGNAL(stateChanged ( int )), this , SLOT(emitEnableFindButton( int )));
  
  connect(this, SIGNAL(enableFindButton ( bool )), findCirclesButton , SLOT(setEnabled( bool )));
  
  //connect(this, SIGNAL(performCircleFind ()), this , SLOT(findCircles()));
  //connect(this, SIGNAL(performCircleFind ()), finalPicWidget , SLOT(repaint()));
  
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

void PicEditWidget::saveFile () {
  
  QFileDialog file;
  file.setAcceptMode ( QFileDialog::AcceptSave );
  
  QStringList fileNames;

  if (file.exec()) {
    fileNames = file.selectedFiles();
  }
  
  Q_FOREACH(const QString& str, fileNames) {
    QString name;
    if (str.endsWith(".png"))
      name = str;
    else
      name = str+".png";
      
    QFile  file( name );
    if (file.exists())
      file.remove();
    final_image.save(name);
  }

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
void PicEditWidget::emitEnableFindButton(double d) {
  emit enableFindButton(true);
}

void PicEditWidget::emitEnableFindButton(int i) {
  emit enableFindButton(true);
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


void accumulateDraw(QImage& img, int x, int y) {
  
  if (x >= 0 && x < img.width() && y >= 0 && y < img.height())
    img.setPixel(x, y, 255);
}

void plot4pointsDraw(QImage& img, int cx, int cy, int x, int y)
{
	accumulateDraw(img, cx + x, cy + y);
	if (x != 0) accumulateDraw(img, cx - x, cy + y);
	if (y != 0) accumulateDraw(img, cx + x, cy - y);
	if (x != 0 && y != 0) accumulateDraw(img, cx - x, cy - y);
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



inline double sideAdj (unsigned int radius, int x, int y, int width, int height) {
  double circleShowingX = 1;
  
  bool offleft = false;
  bool offright = false;
  bool offtop = false;
  bool offbottom = false;
  
  int px = x;
  int nx = width - x ;
  int py = y;
  int ny = height - y;
  int cuts = 0;
  
  if (px < radius)  {offleft   = true; cuts ++;}
  if (nx < radius)  {offright  = true; cuts ++;}
  if (py < radius)  {offtop    = true; cuts ++;}
  if (ny < radius)  {offbottom = true; cuts ++;}
  
  double circleNeeded = 1;
  
  
  if (cuts == 0) {
    // do nothing
  }
  else if (cuts == 1 || (cuts == 2 && ((offleft && offright) || ( offtop && offbottom)))) {
  
  
    if (offleft)
      circleNeeded -= 2*acos((double)px/radius)/(2*M_PI);
    if (offright)
      circleNeeded -= 2*acos((double)nx/radius)/(2*M_PI);
      
    if (offtop)
      circleNeeded -= 2*acos((double)py/radius)/(2*M_PI);
    if (offbottom)
      circleNeeded -= 2*acos((double)ny/radius)/(2*M_PI);
      
  }
  else if (cuts == 2 /*&& !((offleft && offright) || ( offtop && offbottom))*/) {
    
    //if ((offleft && offright) || (offtop && offbottom)) {} // handled in cuts == 1
    
    //else
    {
      // one of the corners
      double theta1 = 2*asin((double)(offleft?px:nx)/radius)/(2*M_PI);
      double theta2 = 2*asin((double)(offtop ?py:ny)/radius)/(2*M_PI);
        
      circleNeeded = theta1 + theta2+(M_PI/2);
    }
  }
  else if (cuts == 3) {
    double theta1;
    double theta2;
    if (offleft || offright && offtop && offbottom) {
      //circle choped on top and bottom and one of left and right
      theta1 = 2*asin((double)(py)/radius)/(2*M_PI);
      theta2 = 2*asin((double)(ny)/radius)/(2*M_PI);
    }
    else if (offtop || offbottom && offleft && offright) {
      //circle choped on left and right and one of top and bottom
      theta1 = 2*asin((double)(px)/radius)/(2*M_PI);
      theta2 = 2*asin((double)(nx)/radius)/(2*M_PI);
    }
    circleNeeded = theta1 + theta2;
  }
  if (cuts == 4) {
    //circle chopped off on all edges
    // do nothing for now FIXME TODO
  }
  
  return circleNeeded;
  
}


inline unsigned int PicEditWidget::minThreshold (unsigned int radius) {
  return thresholdDoubleSpinBox->value()*radius;
}


inline bool PicEditWidget::checkThreshold (int radius, int x, int y, int width, int height) {
  
  return hough_output[radius][x][y] > minThreshold(radius)*sideAdj(radius, x, y, width, height);
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicEditWidget::findCircles() {
  
  if (displayImageCheckBox->checkState () == Qt::Unchecked) {
    final_image = QImage (final_image.size(), QImage::Format_RGB888);
    final_image.fill(0);
  }
  else
    final_image = raw_image.copy();
    
    
  QSet<unsigned long long> ignoreList;
  //Q_FOREACH(unsigned int radius, hough_output.keys () ) {
  QListIterator<unsigned int> i(hough_output.keys ());
  i.toBack();
  while (i.hasPrevious()) {
    unsigned int radius = i.previous();
    //QVector<QVector < unsigned int > >& img = hough_output[radius];
    
    qDebug() << "checking radius " << radius;
    
    int width = hough_output[radius].size();
    int height = hough_output[radius][0].size();
    
    for (int x = 0 ; x < width ; x++) {
      for (int y = 0 ; y < height ; y++) {
      
        //double checkValue = minThreshold(radius)*sideAdj(radius, x, y, width, height);
      
        if (!ignoreList.contains(x+(y*width)+(radius*width*height)) && checkThreshold(radius, x, y, width, height)/*img[x][y] > checkValue*/) {
          //qDebug() << "WTF " << hough_output[58][11][398];
          qDebug() << "value of " << hough_output[radius][x][y] << " for radius " << radius << "(checked against " << minThreshold(radius)*sideAdj(radius, x, y, width, height) << " -> " << ((double)hough_output[radius][x][y]/radius) << ") found at (" << x << ", "<< y << ")";
          
          qDebug() << "circleShowingX = " << sideAdj(radius, x, y, width, height);
          
          int averageX = 0;
          int averageY = 0;
          int simCount = 0;
          
          for (int r = -1 ; r <= 1 ; r++){
            for (int i = -2 ; i <= 2 ; i++){
              for (int j = -2 ; j <= 2 ; j++){
                int nradius = radius+r;
                int nx = x+i;
                int ny = y+j;
                if (hough_output.contains(nradius) && nx >= 0 && nx < width && ny >= 0 && ny < height) {
                  //qDebug() << "checking area " << nx << ":" << ny << " -- " << hough_output[nradius][nx][ny] << " > " <<  minThreshold(nradius)*sideAdj(nradius, nx, ny, width, height);
                  //checkValue = minThreshold(radius)*sideAdj(radius, nx, ny, width, height); /*TODO precalculate radius adjustment*/
                  if (checkThreshold(nradius, nx, ny, width, height)) {
                    //qDebug() << "found at " << nx << ":" << ny;
                    
                    simCount++;
                    averageX+=nx;
                    averageY+=ny;
                  }
                  ignoreList.insert(nx+(ny*width)+(nradius*width*height)); /*TODO move outside this if statement since if it's checked already it need not be checked again*/
                }
              }
            }
          }
          qDebug() << QString("Circle Radius %1 found at %2 %3").arg(radius+1).arg(averageX/simCount).arg(averageY/simCount);
          
          plotFinalCircle(averageX/simCount, averageY/simCount, radius+1);
        }
      }
    }
    
  }
  
  findCirclesButton->setEnabled(false);
  //plotFinalCircle(40,40,30);
}


} /* end namespace Gui */

} /* end namespace Hough */


























