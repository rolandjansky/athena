#include "QatPlotWidgets/PlotFunction1DDialog.h"
#include "QatPlotWidgets/ParameterDialog.h"
#include "QatPlotting/PlotFunction1D.h"
#include "ui_PlotDirDialog.h"
#include <sstream>
  
class PlotFunction1DDialog::Clockwork {
public:
  Ui::PlotDirDialog ui;
  PlotFunction1D *synchedPlot;
  std::vector<Genfun::Parameter *> parameters;
  std::map<Genfun::Parameter *, ParameterDialog *> paramDialogMap;
};

PlotFunction1DDialog::PlotFunction1DDialog(QWidget *parent): QDialog(parent),m_c(new Clockwork()) {
  m_c->synchedPlot=NULL;
  m_c->ui.setupUi(this);
  m_c->ui.tabWidget->removeTab(4);
  m_c->ui.tabWidget->removeTab(3);
  m_c->ui.tabWidget->removeTab(2);
  m_c->ui.tabWidget->removeTab(0);

  // Unveil later (note, do this also on  2D, Dir dialog);
  m_c->ui.moreLineColorsToolButton1D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();
  m_c->ui.moreLineColorsToolButton2D->hide();
  m_c->ui.moreBrushColorsToolButton1D->hide();


  connect(m_c->ui.lineWidthSpinBox1D,  SIGNAL(valueChanged(int)), this, SLOT(setLineWidth(int)));

  connect(m_c->ui.blackLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.redLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.greenLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.blueLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.cyanLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.purpleLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));
  connect(m_c->ui.grayLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineColor(bool)));

  connect(m_c->ui.solidLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashDotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.dashDotDotLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));
  connect(m_c->ui.noLineToolButton1D, SIGNAL(toggled(bool)), this, SLOT(toggleLineStyle(bool)));


  connect(m_c->ui.parameterTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(changeParameter(int, int)));
  connect(m_c->ui.parameterTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT (popParameterDial(int,int)));

  m_c->ui.buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
  m_c->ui.buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);
  m_c->ui.buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
  m_c->ui.buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

}

PlotFunction1DDialog::~PlotFunction1DDialog() {
  delete m_c;
}   
 

#include <iostream>
void PlotFunction1DDialog::synchTo(PlotFunction1D *plot) {
  
  m_c->synchedPlot=plot;

  if (!plot) return;

  PlotFunction1D::Properties prop=plot->properties();
  QPen pen=prop.pen;

  int width=pen.width();
  m_c->ui.lineWidthSpinBox1D->setValue(width);

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


}

void PlotFunction1DDialog::setLineWidth (int i) {
  if (m_c->synchedPlot) {
    PlotFunction1D::Properties prop = m_c->synchedPlot->properties();
    prop.pen.setWidth(i);
    m_c->synchedPlot->setProperties(prop);
    emit updateFunction();
  }
}


void PlotFunction1DDialog::toggleLineColor (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotFunction1D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.blackLineToolButton1D) prop.pen.setColor(QColor("black"));
      else if (theSender==m_c->ui.redLineToolButton1D) prop.pen.setColor(QColor("red"));
      else if (theSender==m_c->ui.greenLineToolButton1D) prop.pen.setColor(QColor("green"));
      else if (theSender==m_c->ui.blueLineToolButton1D) prop.pen.setColor(QColor("blue"));
      else if (theSender==m_c->ui.cyanLineToolButton1D) prop.pen.setColor(QColor("cyan"));
      else if (theSender==m_c->ui.purpleLineToolButton1D) prop.pen.setColor(QColor("purple"));
      else if (theSender==m_c->ui.grayLineToolButton1D) prop.pen.setColor(QColor("gray"));
      m_c->synchedPlot->setProperties(prop);
      emit updateFunction();
    }
  }
}

void PlotFunction1DDialog::toggleLineStyle (bool checked) {
  if (m_c->synchedPlot) {
    if (checked) {
      QObject *theSender = sender();
      PlotFunction1D::Properties prop = m_c->synchedPlot->properties();
      if (theSender==m_c->ui.solidLineToolButton1D)           prop.pen.setStyle(Qt::SolidLine);
      else if (theSender==m_c->ui.dashLineToolButton1D)       prop.pen.setStyle(Qt::DashLine);
      else if (theSender==m_c->ui.dotLineToolButton1D)        prop.pen.setStyle(Qt::DotLine);
      else if (theSender==m_c->ui.dashDotLineToolButton1D)    prop.pen.setStyle(Qt::DashDotLine);
      else if (theSender==m_c->ui.dashDotDotLineToolButton1D) prop.pen.setStyle(Qt::DashDotDotLine);
      else if (theSender==m_c->ui.noLineToolButton1D)         prop.pen.setStyle(Qt::NoPen);
      m_c->synchedPlot->setProperties(prop);
      emit updateFunction();
    }
  }
}

void PlotFunction1DDialog::clearParameters() {
  m_c->parameters.erase(m_c->parameters.begin(),m_c->parameters.end());
  m_c->ui.parameterTableWidget->clear();
  m_c->ui.parameterTableWidget->hide();
}

void PlotFunction1DDialog::addParameter(Genfun::Parameter *p) {
  m_c->parameters.push_back(p);
  m_c->ui.parameterTableWidget->setRowCount(m_c->parameters.size());


  std::ostringstream stream;
  stream << p->getValue();
  
  {
    QTableWidgetItem *item = new QTableWidgetItem(p->getName().c_str());
    item->setFlags(0);
    m_c->ui.parameterTableWidget->setItem(m_c->parameters.size()-1, 0, item);
  }

  {
    QTableWidgetItem *item = new QTableWidgetItem(stream.str().c_str());
    m_c->ui.parameterTableWidget->setItem(m_c->parameters.size()-1, 1, item);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
  }
  m_c->ui.parameterTableWidget->show();
    
}

void PlotFunction1DDialog::changeParameter(int row, int column) {

  std::istringstream inStream(m_c->ui.parameterTableWidget->item(row, column)->text().toStdString().c_str());
  double x; 
  if (inStream >> x) {
    if (x > m_c->parameters[row]->getLowerLimit() && x < m_c->parameters[row]->getUpperLimit()) {
      m_c->parameters[row]->setValue(x);
      emit updateFunction();
      return;
    }
  }
  std::ostringstream outStream;
  outStream << m_c->parameters[row]->getValue();
  QTableWidgetItem *valueItem=m_c->ui.parameterTableWidget->item(row,1);
  if (valueItem) valueItem->setText(outStream.str().c_str());
}


void PlotFunction1DDialog::popParameterDial(int row, int column) {
  if (column==1) {
    
    ParameterDialog *d=NULL;
    std::map<Genfun::Parameter *, ParameterDialog *>::iterator pd=m_c->paramDialogMap.find(m_c->parameters[row]);
    if (pd==m_c->paramDialogMap.end()) {
      
      
      d = new ParameterDialog(0, m_c->parameters[row]);
      d->setWindowTitle(m_c->parameters[row]->getName().c_str());
      connect(d, SIGNAL(updateFunction()), this, SIGNAL(updateFunction()));
      connect(d, SIGNAL(updateFunction()), this, SLOT(synchToFunction()));
      m_c->paramDialogMap[m_c->parameters[row]]=d;
    }
    else {
      d = (*pd).second;
    }
    d->show();
    d->raise();
  }

}

void PlotFunction1DDialog::synchToFunction() {
  blockSignals(true);

  for (unsigned int row=0;row<m_c->parameters.size();row++) {
    std::ostringstream outStream;
    outStream << m_c->parameters[row]->getValue();
    QTableWidgetItem *valueItem=m_c->ui.parameterTableWidget->item(row,1);
    if (valueItem) valueItem->setText(outStream.str().c_str());
  }
  blockSignals(false);
}
