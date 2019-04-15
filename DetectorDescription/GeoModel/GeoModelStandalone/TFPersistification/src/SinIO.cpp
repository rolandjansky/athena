#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/SinIO.h"

#include "GeoGenericFunctions/Sin.h"

SinReader::SinReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Sin", interpreter) {}


GFPTR  SinReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::Sin());
}


SinRecorder::SinRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Sin),persistifier) {}

void SinRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Sin * ptr = dynamic_cast<const GeoGenfun::Sin *> (&F);
  if (!ptr) throw std::runtime_error("Error in SinRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Sin";
}
