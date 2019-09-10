#ifndef _ATanIO_h_
#define _ATanIO_h_
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class ATanReader: public GenFunctionReader {

 public:
  
  ATanReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class ATanRecorder: public GenFunctionRecorder {

 public:
  
  ATanRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

