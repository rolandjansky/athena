#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/AbsIO.h"

#include "GeoGenericFunctions/Abs.h"

AbsReader::AbsReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Abs", interpreter) {}


GFPTR  AbsReader::execute(std::string::const_iterator, std::string::const_iterator) const {
  return GFPTR(new GeoGenfun::Abs());
}


AbsRecorder::AbsRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Abs),persistifier) {}

void AbsRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Abs * ptr = dynamic_cast<const GeoGenfun::Abs *> (&F);
  if (!ptr) throw std::runtime_error("Error in AbsRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Abs";
}
