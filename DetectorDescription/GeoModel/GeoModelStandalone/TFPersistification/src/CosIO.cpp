#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/CosIO.h"

#include "GeoGenericFunctions/Cos.h"

CosReader::CosReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Cos",interpreter) {}


GFPTR  CosReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::Cos());
}

CosRecorder::CosRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Cos), persistifier) {}

void CosRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Cos * ptr = dynamic_cast<const GeoGenfun::Cos *> (&F);
  if (!ptr) throw std::runtime_error("Error in CosRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Cos";
}
