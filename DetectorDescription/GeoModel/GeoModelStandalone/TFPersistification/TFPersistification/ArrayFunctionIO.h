#ifndef _ArrayFunctionIO_h_
#define _ArrayFunctionIO_h_
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class ArrayFunctionReader: public GenFunctionReader {

 public:
  
  ArrayFunctionReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class ArrayFunctionRecorder: public GenFunctionRecorder {

 public:
  
  ArrayFunctionRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

