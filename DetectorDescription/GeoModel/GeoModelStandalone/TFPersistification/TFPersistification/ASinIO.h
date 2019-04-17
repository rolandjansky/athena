#ifndef _ASinIO_h_
#define _ASinIO_h_
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class ASinReader: public GenFunctionReader {

 public:
  
  ASinReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class ASinRecorder: public GenFunctionRecorder {

 public:
  
  ASinRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

