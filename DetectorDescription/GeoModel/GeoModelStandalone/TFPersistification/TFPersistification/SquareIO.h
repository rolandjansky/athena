#ifndef _SquareIO_h_
#define _SquareIO_h_

#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class SquareReader: public GenFunctionReader {

 public:
  
  SquareReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class SquareRecorder: public GenFunctionRecorder {

 public:
  
  SquareRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

