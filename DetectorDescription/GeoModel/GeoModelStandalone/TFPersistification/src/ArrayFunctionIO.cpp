#define private public
#include "GeoGenericFunctions/ArrayFunction.h"
#undef private

#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/ArrayFunctionIO.h"
ArrayFunctionReader::ArrayFunctionReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ArrayFunction", interpreter) {}


GFPTR  ArrayFunctionReader::execute(std::string::const_iterator cStart, std::string::const_iterator cEnd) const {
  std::vector<double> elements;
  auto cNext=std::find(cStart, cEnd,',');
  while (cNext!=cEnd) {
    std::string aNumberStr(cStart,cNext);
    std::istringstream stream(aNumberStr);
    double d;
    stream >> d;
    elements.push_back(d);
    cStart=cNext+1;
    cNext=std::find(cStart, cEnd,',');
  }

  std::string aNumberStr(cStart,cNext);
  std::istringstream stream(aNumberStr);
  double d;
  stream >> d;
  elements.push_back(d);
  
  return GFPTR(new GeoGenfun::ArrayFunction(&elements.front(), &elements.back() + 1));
}


ArrayFunctionRecorder::ArrayFunctionRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ArrayFunction),persistifier) {}

void ArrayFunctionRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ArrayFunction * ptr = dynamic_cast<const GeoGenfun::ArrayFunction *> (&F);
  if (!ptr) throw std::runtime_error("Error in ArrayFunctionRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "ArrayFunction";
  stream << "(";
  for (size_t i=0;i<ptr->_values.size();i++){
    stream<<ptr->_values[i];
    if (i!=ptr->_values.size()-1) stream << ",";
  }
  stream << ")";
}
