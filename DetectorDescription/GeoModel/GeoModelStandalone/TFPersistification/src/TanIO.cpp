#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/TanIO.h"

#include "GeoGenericFunctions/Tan.h"

TanReader::TanReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Tan", interpreter) {}


GFPTR  TanReader::execute(std::string::const_iterator, std::string::const_iterator) const {
  return GFPTR(new GeoGenfun::Tan());
}


TanRecorder::TanRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Tan),persistifier) {}

void TanRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Tan * ptr = dynamic_cast<const GeoGenfun::Tan *> (&F);
  if (!ptr) throw std::runtime_error("Error in TanRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Tan";
}
