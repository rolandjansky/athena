#ifndef _SqrtIO_h_
#define _SqrtIO_h_

#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class SqrtReader: public GenFunctionReader {

 public:
  
  SqrtReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class SqrtRecorder: public GenFunctionRecorder {

 public:
  
  SqrtRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

