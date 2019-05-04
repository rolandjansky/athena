#ifndef _FixedConstantIO_h_
#define _FixedConstantIO_h_
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class FixedConstantReader: public GenFunctionReader {

 public:
  
  FixedConstantReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class FixedConstantRecorder: public GenFunctionRecorder {

 public:
  
  FixedConstantRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

