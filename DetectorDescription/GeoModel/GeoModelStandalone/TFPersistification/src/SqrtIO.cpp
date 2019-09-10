#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/SqrtIO.h"

#include "GeoGenericFunctions/Sqrt.h"

SqrtReader::SqrtReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Sqrt", interpreter) {}


GFPTR  SqrtReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::Sqrt());
}


SqrtRecorder::SqrtRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Sqrt),persistifier) {}

void SqrtRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Sqrt * ptr = dynamic_cast<const GeoGenfun::Sqrt *> (&F);
  if (!ptr) throw std::runtime_error("Error in SqrtRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Sqrt";
}
