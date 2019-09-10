#define private public
#include "GeoGenericFunctions/Rectangular.h"
#undef private
#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/RectangularIO.h"

RectangularReader::RectangularReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Rectangular", interpreter) {}


GFPTR  RectangularReader::execute(std::string::const_iterator begin, std::string::const_iterator end) const {
  std::vector<double> elements;
  auto cStart=begin;
  auto cNext=std::find(cStart, end,',');
  while (cNext!=end) {
    std::string aNumberStr(cStart,cNext);
    std::istringstream stream(aNumberStr);
    double d;
    stream >> d;
    elements.push_back(d);
    cStart=cNext+1;
    cNext=std::find(cStart, end,',');
  }
  std::string aNumberStr(cStart,cNext);
  std::istringstream stream(aNumberStr);
  double d;
  stream >> d;
  elements.push_back(d);
  if (elements.size()!=4) throw std::runtime_error("Error in Rectangular: illegal value list");
  
  GeoGenfun::Rectangular *ptr=new GeoGenfun::Rectangular();
  ptr->x0().setValue(elements[0]);
  ptr->x1().setValue(elements[1]);
  ptr->baseline().setValue(elements[2]);
  ptr->height().setValue(elements[3]);
  return GFPTR(ptr);
}


RectangularRecorder::RectangularRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Rectangular),persistifier) {}

void RectangularRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Rectangular * ptr = dynamic_cast<const GeoGenfun::Rectangular *> (&F);
  if (!ptr) throw std::runtime_error("Error in RectangularRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Rectangular";
  stream << "(";
  stream << ptr->x0().getValue();
  stream << ",";
  stream << ptr->x1().getValue();
  stream << ",";
  stream << ptr->baseline().getValue();
  stream << ",";
  stream << ptr->height().getValue();
  stream << ")";
}
