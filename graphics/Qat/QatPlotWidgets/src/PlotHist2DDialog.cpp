#include "QatPlotWidgets/PlotHist2DDialog.h"
#include "QatPlotting/PlotHist2D.h"
#include "ui_PlotDirDialog.h"
class PlotHist2DDialog::Clockwork {
public:
  PlotHist2D *synchedPlot;
  Ui::PlotDirDialog ui;
};

PlotHist2DDialog::PlotHist2DDialog(QWidget *parent): QDialog(parent),m_c(new Clockwork()) {
  m_c->synchedPlot=NULL;
  m_c->ui.setupUi(this);
  m_c->ui.tabWidget->removeTab(5);
  m_c->ui.tabWidget->removeTab(3);
  m_c->ui.tabWidget->removeTab(1);
  m_c->ui.tabWidget->removeTab(0);

  // Unveil later (note, do this also on 1D, Dir dialog);
  m_c->ui.moreLineColorsToolButton1D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();
  m_c->ui.moreLineColorsToolButton2D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();

  connect(m_c->ui.lineWidthSpinBox2D,  SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));


  connect(m_c->ui.blackLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.redLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.greenLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.blueLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.cyanLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.purpleLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.grayLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));


  connect(m_c->ui.blackBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.redBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.greenBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.blueBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.cyanBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.purpleBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.grayBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));


  connect(m_c->ui.solidLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.noLineToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));

  connect(m_c->ui.solidBrushToolButton2D,  SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense1BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense2BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense3BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense4BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense5BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense6BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense7BrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.noBrushToolButton2D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));


}

PlotHist2DDialog::~PlotHist2DDialog() {
  delete m_c;
}   
 

#include <iostream>
void PlotHist2DDialog::synchTo(PlotHist2D *plot) {
  
  m_c->synchedPlot=plot;

  if (!plot) return;

  PlotHist2D::Properties prop=plot->properties();
  QPen pen=prop.pen;
  QBrush brush = prop.brush;

  int width=pen.width();
  m_c->ui.lineWidthSpinBox2D->setValue(width);


  {
    QColor color = pen.color();
    if (color==QColor("black"))  m_c->ui.blackLineToolButton2D->setChecked(true);
    else if (color==QColor("red"))    m_c->ui.redLineToolButton2D->setChecked(true);
    else if (color==QColor("green"))  m_c->ui.greenLineToolButton2D->setChecked(true);
    else if (color==QColor("blue"))   m_c->ui.blueLineToolButton2D->setChecked(true);
    else if (color==QColor("cyan"))   m_c->ui.cyanLineToolButton2D->setChecked(true);
    else if (color==QColor("purple")) m_c->ui.purpleLineToolButton2D->setChecked(true);
    else if (color==QColor("gray"))   m_c->ui.grayLineToolButton2D->setChecked(true);
    else {
      m_c->ui.moreLineColorsToolButton2D->setChecked(true);
    }
    Qt::PenStyle style = pen.style();
    if (style==Qt::SolidLine)           m_c->ui.solidLineToolButton2D->setChecked(true);
    else if (style==Qt::NoPen)          m_c->ui.noLineToolButton2D->setChecked(true);
  }


  {
    QColor color = brush.color();
    if (color==QColor("black"))  m_c->ui.blackBrushToolButton2D->setChecked(true);
    else if (color==QColor("red"))    m_c->ui.redBrushToolButton2D->setChecked(true);
    else if (color==QColor("green"))  m_c->ui.greenBrushToolButton2D->setChecked(true);
    else if (color==QColor("blue"))   m_c->ui.blueBrushToolButton2D->setChecked(true);
    else if (color==QColor("cyan"))   m_c->ui.cyanBrushToolButton2D->setChecked(true);
    else if (color==QColor("purple")) m_c->ui.purpleBrushToolButton2D->setChecked(true);
    else if (color==QColor("gray"))   m_c->ui.grayBrushToolButton2D->setChecked(true);
    else {
      m_c->ui.moreBrushColorsToolButton2D->setChecked(true);
    }
    Qt::BrushStyle style = brush.style();
    if (style==Qt::SolidPattern)             m_c->ui.solidBrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense1Pattern)       m_c->ui.dense1BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense2Pattern)       m_c->ui.dense2BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense3Pattern)       m_c->ui.dense3BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense4Pattern)       m_c->ui.dense4BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense5Pattern)       m_c->ui.dense5BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense6Pattern)       m_c->ui.dense6BrushToolButton2D->setChecked(true);
    else if (style==Qt::Dense7Pattern)       m_c->ui.dense7BrushToolButton2D->setChecked(true);
    else if (style==Qt::NoBrush)             m_c->ui.noBrushToolButton2D->setChecked(true);
  }


}

void PlotHist2DDialog::setLineWidth (int i) {
  if (m_c->synchedPlot) {
    PlotHist2D::Properties prop = m_c->synchedPlot->properties();
    prop.pen.setWidth(i);
    m_c->synchedPlot->setProperties(prop);
    emit updateHisto();
  }
}



void PlotHist2DDialog::toggleLineColor (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist2D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.blackLineToolButton2D) prop.pen.setColor(QColor("black"));
      else if (theSender==m_c->ui.redLineToolButton2D) prop.pen.setColor(QColor("red"));
      else if (theSender==m_c->ui.greenLineToolButton2D) prop.pen.setColor(QColor("green"));
      else if (theSender==m_c->ui.blueLineToolButton2D) prop.pen.setColor(QColor("blue"));
      else if (theSender==m_c->ui.cyanLineToolButton2D) prop.pen.setColor(QColor("cyan"));
      else if (theSender==m_c->ui.purpleLineToolButton2D) prop.pen.setColor(QColor("purple"));
      else if (theSender==m_c->ui.grayLineToolButton2D) prop.pen.setColor(QColor("gray"));
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist2DDialog::toggleBrushColor (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist2D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.blackBrushToolButton2D) prop.brush.setColor(QColor("black"));
      else if (theSender==m_c->ui.redBrushToolButton2D) prop.brush.setColor(QColor("red"));
      else if (theSender==m_c->ui.greenBrushToolButton2D) prop.brush.setColor(QColor("green"));
      else if (theSender==m_c->ui.blueBrushToolButton2D) prop.brush.setColor(QColor("blue"));
      else if (theSender==m_c->ui.cyanBrushToolButton2D) prop.brush.setColor(QColor("cyan"));
      else if (theSender==m_c->ui.purpleBrushToolButton2D) prop.brush.setColor(QColor("purple"));
      else if (theSender==m_c->ui.grayBrushToolButton2D) prop.brush.setColor(QColor("gray"));
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist2DDialog::toggleLineStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist2D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.solidLineToolButton2D)           prop.pen.setStyle(Qt::SolidLine);
      else if (theSender==m_c->ui.noLineToolButton2D)         prop.pen.setStyle(Qt::NoPen);
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist2DDialog::toggleBrushStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender(); 
      PlotHist2D::Properties prop = m_c->synchedPlot->properties();
      if      (theSender==m_c->ui.solidBrushToolButton2D)            prop.brush.setStyle(Qt::SolidPattern);
      else if (theSender==m_c->ui.dense1BrushToolButton2D)           prop.brush.setStyle(Qt::Dense1Pattern);
      else if (theSender==m_c->ui.dense2BrushToolButton2D)           prop.brush.setStyle(Qt::Dense2Pattern);
      else if (theSender==m_c->ui.dense3BrushToolButton2D)           prop.brush.setStyle(Qt::Dense3Pattern);
      else if (theSender==m_c->ui.dense4BrushToolButton2D)           prop.brush.setStyle(Qt::Dense4Pattern);
      else if (theSender==m_c->ui.dense5BrushToolButton2D)           prop.brush.setStyle(Qt::Dense5Pattern);
      else if (theSender==m_c->ui.dense6BrushToolButton2D)           prop.brush.setStyle(Qt::Dense6Pattern);
      else if (theSender==m_c->ui.dense7BrushToolButton2D)           prop.brush.setStyle(Qt::Dense7Pattern);
      else if (theSender==m_c->ui.noBrushToolButton2D)               prop.brush.setStyle(Qt::NoBrush);
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}


