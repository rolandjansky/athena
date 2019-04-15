#define private public
#include "GeoGenericFunctions/FixedConstant.h"
#undef private
#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/FixedConstantIO.h"
FixedConstantReader::FixedConstantReader(GenFunctionInterpreter *interpreter):GenFunctionReader("FixedConstant", interpreter) {}


GFPTR  FixedConstantReader::execute(std::string::const_iterator begin, std::string::const_iterator end) const {
  double v;
  std::string aNumberStr(begin,end);
  std::istringstream stream(aNumberStr);
  stream >> v;
  return GFPTR(new GeoGenfun::FixedConstant(v));
}


FixedConstantRecorder::FixedConstantRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::FixedConstant),persistifier) {}

void FixedConstantRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::FixedConstant * ptr = dynamic_cast<const GeoGenfun::FixedConstant *> (&F);
  if (!ptr) throw std::runtime_error("Error in FixedConstantRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "FixedConstant";
  stream << "(";
  stream<<ptr->_value;
  stream << ")";
}
