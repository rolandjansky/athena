#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/ATanIO.h"

#include "GeoGenericFunctions/ATan.h"

ATanReader::ATanReader(GenFunctionInterpreter *interpreter):GenFunctionReader("ATan", interpreter) {}


GFPTR  ATanReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::ATan());
}


ATanRecorder::ATanRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::ATan),persistifier) {}

void ATanRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::ATan * ptr = dynamic_cast<const GeoGenfun::ATan *> (&F);
  if (!ptr) throw std::runtime_error("Error in ATanRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "ATan";
}
