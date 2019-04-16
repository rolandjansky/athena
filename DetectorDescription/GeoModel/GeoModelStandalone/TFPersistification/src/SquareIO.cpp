#include "TFPersistification/GenFunctionPersistifier.h"
#include "TFPersistification/GenFunctionInterpreter.h"
#include "TFPersistification/SquareIO.h"

#include "GeoGenericFunctions/Square.h"

SquareReader::SquareReader(GenFunctionInterpreter *interpreter):GenFunctionReader("Square", interpreter) {}


GFPTR  SquareReader::execute(std::string::const_iterator, std::string::const_iterator ) const {
  return GFPTR(new GeoGenfun::Square());
}


SquareRecorder::SquareRecorder(GenFunctionPersistifier *persistifier):
  GenFunctionRecorder(typeid(GeoGenfun::Square),persistifier) {}

void SquareRecorder::execute(const GeoGenfun::AbsFunction & F) const {
  const GeoGenfun::Square * ptr = dynamic_cast<const GeoGenfun::Square *> (&F);
  if (!ptr) throw std::runtime_error("Error in SquareRecorder:: wrong function type");
  std::ostringstream & stream = getPersistifier()->getStream();
  stream << "Square";
}
