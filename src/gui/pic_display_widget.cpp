#include "pic_display_widget.h"


#include <QPixmap>
#include <QLayout>

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
PicDisplayWidget::PicDisplayWidget(const QString& filename) {
  
  image = QImage(filename);
  
  label = new QLabel();
  label->setPixmap ( QPixmap::fromImage(image) );
  
  layout()->addWidget(label);
  
  show();
}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
PicDisplayWidget::~PicDisplayWidget() {


}



} /* end namespace Gui */

} /* end namespace Hough */


