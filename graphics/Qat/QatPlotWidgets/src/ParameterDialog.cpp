#include "QatPlotWidgets/ParameterDialog.h"
#include "ui_ParameterDialog.h"
class ParameterDialog::Clockwork {
public:
  Genfun::Parameter *p;
  Ui::ParameterDialog ui;
};

ParameterDialog::ParameterDialog(QWidget *parent, Genfun::Parameter * p): QDialog(parent), m_c(new Clockwork()){
  m_c->p=p;
  m_c->ui.setupUi(this);

  double xmin=m_c->p->getLowerLimit();
  double xmax=m_c->p->getUpperLimit();
  double val =m_c->p->getValue();
  double frac=double(val-xmin)/double(xmax-xmin);

  int min=m_c->ui.dial->minimum();
  int max=m_c->ui.dial->maximum();
  int dialVal = min + int(frac*(max-min));
  m_c->ui.dial->setValue(dialVal);
  connect(m_c->ui.dial, SIGNAL(valueChanged(int)), this, SLOT(updateParameter(int)));
}


ParameterDialog::~ParameterDialog() {
  delete m_c;
}

void ParameterDialog::updateParameter(int val) {

  int min=m_c->ui.dial->minimum();
  int max=m_c->ui.dial->maximum();
  
  double frac=double(val-min)/double(max-min);
  
  double xmin=m_c->p->getLowerLimit();
  double xmax=m_c->p->getUpperLimit();

  m_c->p->setValue(frac*(xmax-xmin)+xmin);

  emit updateFunction();
}
