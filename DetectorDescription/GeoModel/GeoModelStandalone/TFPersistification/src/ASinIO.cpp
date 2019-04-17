#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/ASinIO.h"

#include "GeoGenericFunctions/ASin.h"

ASinReader::ASinReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ASin", interpreter) {}


GFPTR  ASinReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::ASin());
}


ASinRecorder::ASinRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ASin),persistifier) {}

void ASinRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ASin * ptr = dynamic_cast<const GeoGenfun::ASin *> (&F);
  if (!ptr) throw std::runtime_error("Error in ASinRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "ASin";
}
