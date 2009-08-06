#include "hough_window.h"
#include "pic_display_widget.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QString>

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
HoughWindow::HoughWindow() {
  setupUi(this);

  
  connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFiles()));
  connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/

HoughWindow::~HoughWindow() {

}

/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/

void HoughWindow::openFiles () {
  
  QFileDialog file;
  file.setAcceptMode ( QFileDialog::AcceptOpen );
  
  QStringList fileNames;

  if (file.exec()) {
    fileNames = file.selectedFiles();
  }
  
  Q_FOREACH(const QString& str, fileNames) {
    createRawPicWindow(str);
    
  }

}


/****************************************************************************
**
** Author: Richard Baxter
**
****************************************************************************/
void HoughWindow::createRawPicWindow(const QString & filename) {
  

  PicDisplayWidget* dw = new PicDisplayWidget(filename);
  addDockWidget ( Qt::AllDockWidgetAreas, dw );

}

/****************************************************************************
**
** Author: Richard Baxter
**
** The close event so that it displays the close dialog before closeing
**
****************************************************************************/

void HoughWindow::closeEvent(QCloseEvent *event)
{
  QMessageBox::StandardButton answer = QMessageBox::question ( 
                              this ,/*QWidget * parent*/
                              "Quit?", /* const QString & title */
                              "Are you sure you want to quit?", /* const QString & text */
                              QMessageBox::Ok|QMessageBox::Cancel /*StandardButtons buttons = Ok*/ 
                              /*StandardButton defaultButton = NoButton */);
                              
  if (answer == QMessageBox::Ok) 
    event->accept();
  else
    event->ignore();

}


} /* end namespace Gui */

} /* end namespace Hough */






