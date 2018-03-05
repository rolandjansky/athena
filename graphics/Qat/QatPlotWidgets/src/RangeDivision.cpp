#include "QatPlotWidgets/RangeDivision.h"
QTextDocument* RangeDivision::label() const {
  return m_label;
}

void RangeDivision::setLabel(QTextDocument *doc) {
  delete m_label;
  m_label=doc->clone();
}

const double & RangeDivision::x() const { 
  return m_x;
}


RangeDivision::RangeDivision(double value): m_x(value),m_label(new QTextDocument) {
}
    
RangeDivision::~RangeDivision() {
  delete m_label;
}

RangeDivision::RangeDivision ( const RangeDivision & right):
  m_x(right.m_x),
  m_label(right.m_label->clone())
{
}

RangeDivision & RangeDivision::operator=(const RangeDivision & right) {
  if (&right!=this) {
    if (m_label) delete m_label;
    m_label=right.m_label->clone();
    m_x=right.m_x;
  }
  return *this;
}

