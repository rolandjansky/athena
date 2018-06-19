#include "QatPlotWidgets/PlotHist1DDialog.h"
#include "QatPlotting/PlotHist1D.h"
#include "ui_PlotDirDialog.h"
class PlotHist1DDialog::Clockwork {
public:
  PlotHist1D *synchedPlot;
  Ui::PlotDirDialog ui;
};

PlotHist1DDialog::PlotHist1DDialog(QWidget *parent): QDialog(parent),m_c(new Clockwork()) {
  m_c->synchedPlot=NULL;
  m_c->ui.setupUi(this);
  m_c->ui.tabWidget->removeTab(5);
  m_c->ui.tabWidget->removeTab(4);
  m_c->ui.tabWidget->removeTab(2);

  // Unveil later (note, do this also on  2D, Dir dialog);
  m_c->ui.moreLineColorsToolButton1D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();
  m_c->ui.moreLineColorsToolButton2D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();


  connect(m_c->ui.lineWidthSpinBox1D,  SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));
  connect(m_c->ui.symbolSizeSpinBox1D, SIGNAL(valueChanged(int)), this, SLOT(setSymbolSize(int)));

  connect(m_c->ui.blackLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.redLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.greenLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.blueLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.cyanLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.purpleLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.grayLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));


  connect(m_c->ui.blackBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.redBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.greenBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.blueBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.cyanBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.purpleBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));
  connect(m_c->ui.grayBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushColor(bool)));


  connect(m_c->ui.solidLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashDotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashDotDotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.noLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));

  connect(m_c->ui.solidBrushToolButton1D,  SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense1BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense2BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense3BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense4BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense5BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense6BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.dense7BrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.horizontalBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.verticalBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.crossPatternBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.bDiagBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.fDiagBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.diagCrossPatternBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));
  connect(m_c->ui.noBrushToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleBrushStyle(bool)));


  connect(m_c->ui.circleSymbolToolButton1D,       SIGNAL(toggled(bool)), this, SLOT(togglePlotStyle(bool)));
  connect(m_c->ui.squareSymbolToolButton1D,       SIGNAL(toggled(bool)), this, SLOT(togglePlotStyle(bool)));
  connect(m_c->ui.upTriangleSymbolToolButton1D,   SIGNAL(toggled(bool)), this, SLOT(togglePlotStyle(bool)));
  connect(m_c->ui.downTriangleSymbolToolButton1D, SIGNAL(toggled(bool)), this, SLOT(togglePlotStyle(bool)));
  connect(m_c->ui.noSymbolToolButton1D, SIGNAL(toggled(bool)), this, SLOT(togglePlotStyle(bool)));

}

PlotHist1DDialog::~PlotHist1DDialog() {
  delete m_c;
}   
 

#include <iostream>
void PlotHist1DDialog::synchTo(PlotHist1D *plot) {
  
  m_c->synchedPlot=plot;

  if (!plot) return;

  PlotHist1D::Properties prop=plot->properties();
  QPen pen=prop.pen;
  QBrush brush = prop.brush;

  int width=pen.width();
  m_c->ui.lineWidthSpinBox1D->setValue(width);

  int symSize=prop.symbolSize;
  m_c->ui.symbolSizeSpinBox1D->setValue(symSize);

  {
    QColor color = pen.color();
    if (color==QColor("black"))  m_c->ui.blackLineToolButton1D->setChecked(true);
    else if (color==QColor("red"))    m_c->ui.redLineToolButton1D->setChecked(true);
    else if (color==QColor("green"))  m_c->ui.greenLineToolButton1D->setChecked(true);
    else if (color==QColor("blue"))   m_c->ui.blueLineToolButton1D->setChecked(true);
    else if (color==QColor("cyan"))   m_c->ui.cyanLineToolButton1D->setChecked(true);
    else if (color==QColor("purple")) m_c->ui.purpleLineToolButton1D->setChecked(true);
    else if (color==QColor("gray"))   m_c->ui.grayLineToolButton1D->setChecked(true);
    else {
      m_c->ui.moreLineColorsToolButton1D->setChecked(true);
    }
    Qt::PenStyle style = pen.style();
    if (style==Qt::SolidLine)           m_c->ui.solidLineToolButton1D->setChecked(true);
    else if (style==Qt::DashLine)       m_c->ui.dashLineToolButton1D->setChecked(true);
    else if (style==Qt::DotLine)        m_c->ui.dotLineToolButton1D->setChecked(true);
    else if (style==Qt::DashDotLine)    m_c->ui.dashDotLineToolButton1D->setChecked(true);
    else if (style==Qt::DashDotDotLine) m_c->ui.dashDotDotLineToolButton1D->setChecked(true);
    else if (style==Qt::NoPen)          m_c->ui.noLineToolButton1D->setChecked(true);
  }


  {
    QColor color = brush.color();
    if (color==QColor("black"))  m_c->ui.blackBrushToolButton1D->setChecked(true);
    else if (color==QColor("red"))    m_c->ui.redBrushToolButton1D->setChecked(true);
    else if (color==QColor("green"))  m_c->ui.greenBrushToolButton1D->setChecked(true);
    else if (color==QColor("blue"))   m_c->ui.blueBrushToolButton1D->setChecked(true);
    else if (color==QColor("cyan"))   m_c->ui.cyanBrushToolButton1D->setChecked(true);
    else if (color==QColor("purple")) m_c->ui.purpleBrushToolButton1D->setChecked(true);
    else if (color==QColor("gray"))   m_c->ui.grayBrushToolButton1D->setChecked(true);
    else {
      m_c->ui.moreBrushColorsToolButton1D->setChecked(true);
    }
    Qt::BrushStyle style = brush.style();
    if (style==Qt::SolidPattern)             m_c->ui.solidBrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense1Pattern)       m_c->ui.dense1BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense2Pattern)       m_c->ui.dense2BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense3Pattern)       m_c->ui.dense3BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense4Pattern)       m_c->ui.dense4BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense5Pattern)       m_c->ui.dense5BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense6Pattern)       m_c->ui.dense6BrushToolButton1D->setChecked(true);
    else if (style==Qt::Dense7Pattern)       m_c->ui.dense7BrushToolButton1D->setChecked(true);
    else if (style==Qt::HorPattern)          m_c->ui.horizontalBrushToolButton1D->setChecked(true);
    else if (style==Qt::VerPattern)          m_c->ui.verticalBrushToolButton1D->setChecked(true);
    else if (style==Qt::CrossPattern)        m_c->ui.crossPatternBrushToolButton1D->setChecked(true);
    else if (style==Qt::BDiagPattern)        m_c->ui.bDiagBrushToolButton1D->setChecked(true);
    else if (style==Qt::FDiagPattern)        m_c->ui.fDiagBrushToolButton1D->setChecked(true);
    else if (style==Qt::DiagCrossPattern)    m_c->ui.diagCrossPatternBrushToolButton1D->setChecked(true);
    else if (style==Qt::NoBrush)             m_c->ui.noBrushToolButton1D->setChecked(true);
  }

  PlotHist1D::Properties::PlotStyle     plotStyle   = prop.plotStyle;
  PlotHist1D::Properties::SymbolStyle   symbolStyle = prop.symbolStyle;

  if (plotStyle==PlotHist1DProperties::LINES) {
    m_c->ui.noSymbolToolButton1D->setChecked(true);
  }
  else {
    if      (symbolStyle==PlotHist1D::Properties::CIRCLE)     m_c->ui.circleSymbolToolButton1D->setChecked(true);
    else if (symbolStyle==PlotHist1D::Properties::SQUARE)     m_c->ui.squareSymbolToolButton1D->setChecked(true);
    else if (symbolStyle==PlotHist1D::Properties::TRIANGLE_U) m_c->ui.upTriangleSymbolToolButton1D->setChecked(true);
    else if (symbolStyle==PlotHist1D::Properties::TRIANGLE_L) m_c->ui.downTriangleSymbolToolButton1D->setChecked(true);
  }


}

void PlotHist1DDialog::setLineWidth (int i) {
  if (m_c->synchedPlot) {
    PlotHist1D::Properties prop = m_c->synchedPlot->properties();
    prop.pen.setWidth(i);
    m_c->synchedPlot->setProperties(prop);
    emit updateHisto();
  }
}

void PlotHist1DDialog::setSymbolSize (int i) {
  if (m_c->synchedPlot) {
    PlotHist1D::Properties prop = m_c->synchedPlot->properties();
    prop.symbolSize=i;
    m_c->synchedPlot->setProperties(prop);
    emit updateHisto();
  }
}


void PlotHist1DDialog::toggleLineColor (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist1D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.blackLineToolButton1D) prop.pen.setColor(QColor("black"));
      else if (theSender==m_c->ui.redLineToolButton1D) prop.pen.setColor(QColor("red"));
      else if (theSender==m_c->ui.greenLineToolButton1D) prop.pen.setColor(QColor("green"));
      else if (theSender==m_c->ui.blueLineToolButton1D) prop.pen.setColor(QColor("blue"));
      else if (theSender==m_c->ui.cyanLineToolButton1D) prop.pen.setColor(QColor("cyan"));
      else if (theSender==m_c->ui.purpleLineToolButton1D) prop.pen.setColor(QColor("purple"));
      else if (theSender==m_c->ui.grayLineToolButton1D) prop.pen.setColor(QColor("gray"));
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist1DDialog::toggleBrushColor (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist1D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.blackBrushToolButton1D) prop.brush.setColor(QColor("black"));
      else if (theSender==m_c->ui.redBrushToolButton1D) prop.brush.setColor(QColor("red"));
      else if (theSender==m_c->ui.greenBrushToolButton1D) prop.brush.setColor(QColor("green"));
      else if (theSender==m_c->ui.blueBrushToolButton1D) prop.brush.setColor(QColor("blue"));
      else if (theSender==m_c->ui.cyanBrushToolButton1D) prop.brush.setColor(QColor("cyan"));
      else if (theSender==m_c->ui.purpleBrushToolButton1D) prop.brush.setColor(QColor("purple"));
      else if (theSender==m_c->ui.grayBrushToolButton1D) prop.brush.setColor(QColor("gray"));
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist1DDialog::toggleLineStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist1D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.solidLineToolButton1D)           prop.pen.setStyle(Qt::SolidLine);
      else if (theSender==m_c->ui.dashLineToolButton1D)       prop.pen.setStyle(Qt::DashLine);
      else if (theSender==m_c->ui.dotLineToolButton1D)        prop.pen.setStyle(Qt::DotLine);
      else if (theSender==m_c->ui.dashDotLineToolButton1D)    prop.pen.setStyle(Qt::DashDotLine);
      else if (theSender==m_c->ui.dashDotDotLineToolButton1D) prop.pen.setStyle(Qt::DashDotDotLine);
      else if (theSender==m_c->ui.noLineToolButton1D)         prop.pen.setStyle(Qt::NoPen);
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

void PlotHist1DDialog::toggleBrushStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender(); 
      PlotHist1D::Properties prop = m_c->synchedPlot->properties();
      if      (theSender==m_c->ui.solidBrushToolButton1D)            prop.brush.setStyle(Qt::SolidPattern);
      else if (theSender==m_c->ui.dense1BrushToolButton1D)           prop.brush.setStyle(Qt::Dense1Pattern);
      else if (theSender==m_c->ui.dense2BrushToolButton1D)           prop.brush.setStyle(Qt::Dense2Pattern);
      else if (theSender==m_c->ui.dense3BrushToolButton1D)           prop.brush.setStyle(Qt::Dense3Pattern);
      else if (theSender==m_c->ui.dense4BrushToolButton1D)           prop.brush.setStyle(Qt::Dense4Pattern);
      else if (theSender==m_c->ui.dense5BrushToolButton1D)           prop.brush.setStyle(Qt::Dense5Pattern);
      else if (theSender==m_c->ui.dense6BrushToolButton1D)           prop.brush.setStyle(Qt::Dense6Pattern);
      else if (theSender==m_c->ui.dense7BrushToolButton1D)           prop.brush.setStyle(Qt::Dense7Pattern);
      else if (theSender==m_c->ui.horizontalBrushToolButton1D)       prop.brush.setStyle(Qt::HorPattern);
      else if (theSender==m_c->ui.verticalBrushToolButton1D)         prop.brush.setStyle(Qt::VerPattern);
      else if (theSender==m_c->ui.crossPatternBrushToolButton1D)     prop.brush.setStyle(Qt::CrossPattern);
      else if (theSender==m_c->ui.bDiagBrushToolButton1D)            prop.brush.setStyle(Qt::BDiagPattern);
      else if (theSender==m_c->ui.fDiagBrushToolButton1D)            prop.brush.setStyle(Qt::FDiagPattern);
      else if (theSender==m_c->ui.diagCrossPatternBrushToolButton1D) prop.brush.setStyle(Qt::DiagCrossPattern);
      else if (theSender==m_c->ui.noBrushToolButton1D)               prop.brush.setStyle(Qt::NoBrush);
      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}


void PlotHist1DDialog::togglePlotStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotHist1D::Properties prop = m_c->synchedPlot->properties();

      if (theSender==m_c->ui.noSymbolToolButton1D)            {
	prop.plotStyle=PlotHist1D::Properties::LINES;
      }
      else {
	prop.plotStyle=PlotHist1D::Properties::SYMBOLS;
      }

      
      if       (theSender==m_c->ui.circleSymbolToolButton1D)         prop.symbolStyle=PlotHist1D::Properties::CIRCLE;
      else if  (theSender==m_c->ui.squareSymbolToolButton1D)         prop.symbolStyle=PlotHist1D::Properties::SQUARE;
      else if  (theSender==m_c->ui.upTriangleSymbolToolButton1D)     prop.symbolStyle=PlotHist1D::Properties::TRIANGLE_U;
      else if  (theSender==m_c->ui.downTriangleSymbolToolButton1D)   prop.symbolStyle=PlotHist1D::Properties::TRIANGLE_L;

      m_c->synchedPlot->setProperties(prop);
      emit updateHisto();
    }
  }
}

