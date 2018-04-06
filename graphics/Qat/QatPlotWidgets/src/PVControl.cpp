#include "QatPlotWidgets/PVControl.h"
#include <QtGui/QDoubleValidator>
#include "ui_PVControl.h"
#include <sstream>
#include <iostream>
#include <limits>

class PVControl::Clockwork {
public:
  Ui::PVControl ui;
  QDoubleValidator *validator;
  QRectF rectI;
  QRectF rectII;
  PlotView *plotView;

};

QRectF PVControl::rect() {
  return m_c->rectII;
}

PVControl::~PVControl() {
  delete m_c;
}

Ui::PVControl & PVControl::ui() {
  return m_c->ui;
}

PVControl::PVControl (PlotView *parent, const QRectF & rect):QDialog(parent), m_c(new Clockwork()){
  m_c->validator=NULL;
  m_c->plotView=parent;
  
  m_c->ui.setupUi(this);
  setRect(rect);

  QObject::connect(m_c->ui.xZeroCheckBox, SIGNAL(clicked(bool)), parent, SLOT(setXZero(bool)));
  QObject::connect(m_c->ui.yZeroCheckBox, SIGNAL(clicked(bool)), parent, SLOT(setYZero(bool)));
  QObject::connect(m_c->ui.gridCheckBox,  SIGNAL(clicked(bool)), parent, SLOT(setGrid(bool)));
  QObject::connect(m_c->ui.logXCheckBox,  SIGNAL(clicked(bool)), parent, SLOT(setLogX(bool)));
  QObject::connect(m_c->ui.logYCheckBox,  SIGNAL(clicked(bool)), parent, SLOT(setLogY(bool)));
  QObject::connect(m_c->ui.labelBoxBox,   SIGNAL(clicked(bool)), parent, SLOT(setBox(bool)));

  QObject::connect(m_c->ui.xMaxDial,   SIGNAL(valueChanged(int)), this, SLOT(setXMax(int)));
  QObject::connect(m_c->ui.xMinDial,   SIGNAL(valueChanged(int)), this, SLOT(setXMin(int)));
  QObject::connect(m_c->ui.yMaxDial,   SIGNAL(valueChanged(int)), this, SLOT(setYMax(int)));
  QObject::connect(m_c->ui.yMinDial,   SIGNAL(valueChanged(int)), this, SLOT(setYMin(int)));


  QObject::connect(m_c->ui.xMinBox, SIGNAL(editingFinished()), this, SLOT(changeXMin()));
  QObject::connect(m_c->ui.xMaxBox, SIGNAL(editingFinished()), this, SLOT(changeXMax()));
  QObject::connect(m_c->ui.yMinBox, SIGNAL(editingFinished()), this, SLOT(changeYMin()));
  QObject::connect(m_c->ui.yMaxBox, SIGNAL(editingFinished()), this, SLOT(changeYMax()));

  QObject::connect(m_c->ui.labelHomePushButton, SIGNAL(clicked()), this, SIGNAL(labelHome()));
  QObject::connect(m_c->ui.xAxisFontButton, SIGNAL(clicked()), this, SIGNAL(editXAxisFont()));
  QObject::connect(m_c->ui.yAxisFontButton, SIGNAL(clicked()), this, SIGNAL(editYAxisFont()));

  QObject::connect(m_c->ui.labelWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLabelXSizePercentage(int)));
  QObject::connect(m_c->ui.labelHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setLabelYSizePercentage(int)));

  m_c->validator=new QDoubleValidator(this);
  m_c->validator->setRange(-std::numeric_limits<double>::max(),
		      +std::numeric_limits<double>::max(),
		      10);


  m_c->ui.xMinBox->setValidator(m_c->validator);
  m_c->ui.xMaxBox->setValidator(m_c->validator);
  m_c->ui.yMinBox->setValidator(m_c->validator);
  m_c->ui.yMaxBox->setValidator(m_c->validator);
}

void PVControl::setRect( const QRectF & rect) {
  
  QObject *sender = QObject::sender();
 
  std::ostringstream leftStream, rightStream, topStream, bottomStream;
  leftStream   << rect.left();
  rightStream  << rect.right();
  topStream    << rect.top();
  bottomStream << rect.bottom();

  if (sender== m_c->ui.xMinBox || sender==m_c->ui.xMaxBox || sender==m_c->ui.yMinBox || sender==m_c->ui.yMaxBox ||
      sender== m_c->ui.xMinDial || sender==m_c->ui.xMaxDial || sender==m_c->ui.yMinDial || sender==m_c->ui.yMaxDial) {
  }
  else {
    m_c->ui.xMinBox->setText(leftStream.str().c_str());
    m_c->ui.yMinBox->setText(topStream.str().c_str());
    m_c->ui.xMaxBox->setText(rightStream.str().c_str());
    m_c->ui.yMaxBox->setText(bottomStream.str().c_str());
    m_c->rectI =rect;

    m_c->ui.yMinDial->setValue(0);
    m_c->ui.yMaxDial->setValue(99);
    m_c->ui.xMinDial->setValue(0);
    m_c->ui.xMaxDial->setValue(99);
    m_c->rectII=rect;
    m_c->ui.logXCheckBox->setEnabled(rect.left()>0.0);
    m_c->ui.logYCheckBox->setEnabled(rect.top()>0.0);
  }


}

PlotView::Style PVControl::yAxisStyle() {
  return m_c->ui.logYCheckBox->isChecked() ? PlotView::LOG : PlotView::LINEAR;
}


PlotView::Style PVControl::xAxisStyle() {
  return m_c->ui.logXCheckBox->isChecked() ? PlotView::LOG : PlotView::LINEAR;
}

// called when XMax is changed:
void PVControl::setXMax(int value) {
  if (m_c->ui.xMinDial->value()>= value) m_c->ui.xMinDial->setValue(value-1);
  m_c->rectII.setRight(m_c->rectI.left()+value/99.0*m_c->rectI.width());
  emit rectChanged(m_c->rectII);
}

// called when XMin is changed:
void PVControl::setXMin(int value) {
  if (m_c->ui.xMaxDial->value()<= value) m_c->ui.xMaxDial->setValue(value+1);
  m_c->rectII.setLeft(m_c->rectI.left()+value/99.0*m_c->rectI.width());
  emit rectChanged(m_c->rectII);
}

// called when YMax is changed:
void PVControl::setYMax(int value) {
  if (m_c->ui.yMinDial->value()>= value) m_c->ui.yMinDial->setValue(value-1);
  m_c->rectII.setBottom(m_c->rectI.top()+value/99.0*m_c->rectI.height());
  emit rectChanged(m_c->rectII);
}

// called when YMin is changed:
void PVControl::setYMin(int value) {
  if (m_c->ui.yMaxDial->value()<= value) m_c->ui.yMaxDial->setValue(value+1);
  m_c->rectII.setTop(m_c->rectI.top()+value/99.0*m_c->rectI.height());
  emit rectChanged(m_c->rectII);
}

void PVControl::changeXMin() {

  std::string txt=m_c->ui.xMinBox->text().toStdString();

  double value;
  std::istringstream valueStream(txt.c_str());
  if ((valueStream >> value)) {
    if (value < m_c->rectI.right() && (!m_c->plotView->isLogX() || value > 0)) {
      m_c->rectI.setLeft(value);
      m_c->ui.xMinDial->setValue(0);
      setXMin(0);
      m_c->ui.logXCheckBox->setEnabled(m_c->rectI.left()>0.0);
    }
    else {
      QApplication::beep();
      std::ostringstream oldValueStream;
      oldValueStream << m_c->rectI.left();
      m_c->ui.xMinBox->setText(oldValueStream.str().c_str());
    }
  }
}
void PVControl::changeYMin() {

  std::string txt=m_c->ui.yMinBox->text().toStdString();

  double value;
  std::istringstream valueStream(txt.c_str());
  if ((valueStream >> value)) {
    if (value < m_c->rectI.bottom() && (!m_c->plotView->isLogY() || value>0)) {
      m_c->rectI.setTop(value);
      m_c->ui.yMinDial->setValue(0);
      setYMin(0);
      m_c->ui.logYCheckBox->setEnabled(m_c->rectI.top()>0.0);
    }
    else {
      QApplication::beep();
      std::ostringstream oldValueStream;
      oldValueStream << m_c->rectI.top();
      m_c->ui.yMinBox->setText(oldValueStream.str().c_str());
    }
  }
}



void PVControl::changeXMax() {

  std::string txt=m_c->ui.xMaxBox->text().toStdString();

  double value;
  std::istringstream valueStream(txt.c_str());
  if ((valueStream >> value)) {
    if (value > m_c->rectI.left()) {
      m_c->rectI.setRight(value);
      m_c->ui.xMaxDial->setValue(99);
      setXMax(99);
    }
    else {
      QApplication::beep();
      std::ostringstream oldValueStream;
      oldValueStream << m_c->rectI.right();
      m_c->ui.xMaxBox->setText(oldValueStream.str().c_str());
    }
  }


}


void PVControl::changeYMax() {

  std::string txt=m_c->ui.yMaxBox->text().toStdString();

  double value;
  std::istringstream valueStream(txt.c_str());
  if ((valueStream >> value)) {
    if (value > m_c->rectI.top()) {
      m_c->rectI.setBottom(value);
      m_c->ui.yMaxDial->setValue(99);
      setYMax(99);
    }
    else {
      QApplication::beep();
      std::ostringstream oldValueStream;
      oldValueStream << m_c->rectI.bottom();
      m_c->ui.yMaxBox->setText(oldValueStream.str().c_str());
    }
  }

  

}


void PVControl::setLabelXSizePercentage(int pc) {
  QObject *sender = QObject::sender();

  if (sender!=m_c->ui.labelWidthSpinBox) m_c->ui.labelWidthSpinBox->setValue(pc);
  qreal f = pc;
  emit labelXSizePercentageChanged(f);
}

void PVControl::setLabelYSizePercentage(int pc) {
  QObject *sender = QObject::sender();
  if (sender != m_c->ui.labelHeightSpinBox) m_c->ui.labelHeightSpinBox->setValue(pc);
  qreal f = pc;
  emit labelYSizePercentageChanged(f);
}
