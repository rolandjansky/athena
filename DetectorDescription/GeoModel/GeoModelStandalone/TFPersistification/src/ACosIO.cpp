#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/ACosIO.h"

#include "GeoGenericFunctions/ACos.h"

ACosReader::ACosReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ACos", interpreter) {}


GFPTR  ACosReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::ACos());
}


ACosRecorder::ACosRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ACos),persistifier) {}

void ACosRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ACos * ptr = dynamic_cast<const GeoGenfun::ACos *> (&F);
  if (!ptr) throw std::runtime_error("Error in ACosRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "ACos";
}
