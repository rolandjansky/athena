#ifndef _CosIO_h_
#define _CosIO_h_

#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class CosReader: public GenFunctionReader {

 public:
  
  CosReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class CosRecorder: public GenFunctionRecorder {

 public:
  
  CosRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

