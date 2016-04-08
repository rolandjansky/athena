#include "QatPlotWidgets/CustomRangeDivider.h"
// Constructor:
CustomRangeDivider::CustomRangeDivider():
  _min(0),_max(0){
}

// Destructor:
CustomRangeDivider::~CustomRangeDivider(){
}

// Insert a subdivision:
void CustomRangeDivider::add (double x, QTextDocument *label){
  _subdivision.push_back(RangeDivision(x));
  if (label) _subdivision.back().setLabel(label); // internally this copies. 
}

// Get the number of subdivision:
int CustomRangeDivider::getNumSubdivisions() const{
  return _subdivision.size();
}

// Get the location of each subdivision:
const RangeDivision & CustomRangeDivider::getSubdivision ( int i) const{
  return _subdivision[i];
}


// Set the range:
void CustomRangeDivider::setRange(double min, double max){

  // This protection insures 
  if (min==max) return;
  _min=min;
  _max=max;

}

// Set the range:
void CustomRangeDivider::setMin(double min){
  setRange(min,_max);
}

// Set the range:
void CustomRangeDivider::setMax(double max){
  setRange(_min,max);
}

// Get the validity of each subdivision:
bool CustomRangeDivider::isValid(int i) const {
  const RangeDivision & rd=getSubdivision(i);
  return (rd.x()>=_min && rd.x()<=_max);
}
