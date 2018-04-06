#include "QatPlotWidgets/CustomRangeDivider.h"
// Constructor:
CustomRangeDivider::CustomRangeDivider():
  m_min(0),m_max(0){
}

// Destructor:
CustomRangeDivider::~CustomRangeDivider(){
}

// Insert a subdivision:
void CustomRangeDivider::add (double x, QTextDocument *label){
  m_subdivision.push_back(RangeDivision(x));
  if (label) m_subdivision.back().setLabel(label); // internally this copies. 
}

// Get the number of subdivision:
int CustomRangeDivider::getNumSubdivisions() const{
  return m_subdivision.size();
}

// Get the location of each subdivision:
const RangeDivision & CustomRangeDivider::getSubdivision ( int i) const{
  return m_subdivision[i];
}


// Set the range:
void CustomRangeDivider::setRange(double min, double max){

  // This protection insures 
  if (min==max) return;
  m_min=min;
  m_max=max;

}

// Set the range:
void CustomRangeDivider::setMin(double min){
  setRange(min,m_max);
}

// Set the range:
void CustomRangeDivider::setMax(double max){
  setRange(m_min,max);
}

// Get the validity of each subdivision:
bool CustomRangeDivider::isValid(int i) const {
  const RangeDivision & rd=getSubdivision(i);
  return (rd.x()>=m_min && rd.x()<=m_max);
}
