#include "QatPlotWidgets/RangeDivision.h"
QTextDocument* RangeDivision::label() const {
  return _label;
}

void RangeDivision::setLabel(QTextDocument *doc) {
  delete _label;
  _label=doc->clone();
}

const double & RangeDivision::x() const { 
  return _x;
}


RangeDivision::RangeDivision(double value): _x(value),_label(new QTextDocument) {
}
    
RangeDivision::~RangeDivision() {
  delete _label;
}

RangeDivision::RangeDivision ( const RangeDivision & right):
  _x(right._x),
  _label(right._label->clone())
{
}

RangeDivision & RangeDivision::operator=(const RangeDivision & right) {
  if (&right!=this) {
    if (_label) delete _label;
    _label=right._label->clone();
    _x=right._x;
  }
  return *this;
}

