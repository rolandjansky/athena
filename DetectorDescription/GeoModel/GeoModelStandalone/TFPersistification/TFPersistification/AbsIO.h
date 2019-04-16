#ifndef _AbsIO_h_
#define _AbsIO_h_
#include "TFPersistification/GenFunctionReader.h"
#include "TFPersistification/GenFunctionRecorder.h"

#include "GeoGenericFunctions/AbsFunction.h"

class AbsReader: public GenFunctionReader {

 public:
  
  AbsReader(GenFunctionInterpreter * interpreter);
  virtual GFPTR execute(std::string::const_iterator begin, std::string::const_iterator end) const;
  
};


class AbsRecorder: public GenFunctionRecorder {

 public:
  
  AbsRecorder(GenFunctionPersistifier * persistifier);
  virtual void execute(const GeoGenfun::AbsFunction & F) const;
  
};
#endif

