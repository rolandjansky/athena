#ifndef _TRANSFUNCTIONREADER_H_
#define _TRANSFUNCTIONREADER_H_

#include "GeoModelKernel/GeoXF.h"

#include <memory>
//


// Forward definition of "Transfunction"
//
namespace GeoXF {
  class Function;
}

typedef std::unique_ptr<const GeoXF::Function> TFPtr;


//
// Forward definition of TransFunctionInterpreter;
//
class TransFunctionInterpreter;

class TransFunctionReader {

 public:

  // Constructor.  Keep a pointer to the interpreter for purposes of recusion.
  TransFunctionReader(const std::string & name, TransFunctionInterpreter *interpreter);

  // Destructor
  virtual ~TransFunctionReader();

  // Execute
  virtual TFPTR execute(const std::string & arg) const=0;

  // Access to the interpreter:
  const TransFunctionInterpreter *getInterpreter() const;

 protected:

  // Some of the subclasses require two arguments, separated by a |. This
  // splits the argument into its two pieces:
  std::pair<std::string, std::string> split(const std::string & arg) const;
  
  // Help scan transforms
  GeoTrf::Transform3D scanT(const std::string & exprString) const;

  private:


  // A pointer to the interpreter:
  const TransFunctionInterpreter* m_interpreter;

  // Deleted operations:
  TransFunctionReader(const TransFunctionReader &)               = delete;
  TransFunctionReader & operator = (const TransFunctionReader &) = delete;

  
};

// Now we add the pre-existing types of transfunction (=GeoXF::Function).  These
// may well be the only types we ever need!.

class ProductReader: public TransFunctionReader {

 public:
  
  ProductReader(TransFunctionInterpreter * interpreter);
  virtual TFPTR execute(const std::string & arg) const;
  
};


class PreMultReader: public TransFunctionReader {

 public:
  
  PreMultReader(TransFunctionInterpreter * interpreter);
  virtual TFPTR execute(const std::string & arg) const;
  
};

class PostMultReader: public TransFunctionReader {

 public:
  
  PostMultReader(TransFunctionInterpreter * interpreter);
  virtual TFPTR  execute(const std::string & arg) const;
  
};


class PowReader: public TransFunctionReader {

 public:
  
  PowReader(TransFunctionInterpreter * interpreter);
  virtual TFPTR execute(const std::string & arg) const;
  
};




#endif
