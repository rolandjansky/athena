#ifndef _RectangularIO_h_
#define _RectangularIO_h_

#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class RectangularReader: public GenFunctionReader {

 public:
  
  RectangularReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class RectangularRecorder: public GenFunctionRecorder {

 public:
  
  RectangularRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

