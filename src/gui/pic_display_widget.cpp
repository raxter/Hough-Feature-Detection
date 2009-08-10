#include "pic_display_widget.h"


#include <QPixmap>
#include <QLayout>
#include <QPainter>

#include <QDebug>


namespace Hough
{

namespace Gui
{


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
PicDisplayWidget::PicDisplayWidget(const QImage & image ) : image(&image) {
  
  setMinimumSize(image.size());
  resize ( image.size() );
  
  show();
  
  repaint();
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
PicDisplayWidget::~PicDisplayWidget() {


}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void PicDisplayWidget::setImage( const QImage & image ) {
  this->image = &image;
  repaint();
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/

void PicDisplayWidget::paintEvent( QPaintEvent *e )
{
  if ( image->size() != QSize( 0, 0 ) ) {         // is an image loaded?
    QPainter painter(this);
    painter.setClipRect(e->rect());
    painter.drawImage(QPoint(0,0), *image , image->rect ());
  }
}



} /* end namespace Gui */

} /* end namespace Hough */


