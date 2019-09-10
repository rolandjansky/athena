#ifndef _GENFUNCTIONREADER_H_
#define _GENFUNCTIONREADER_H_

#include "GeoGenericFunctions/AbsFunction.h"

#include <memory>
#include <tuple>
#include <string>

typedef std::tuple<std::string::const_iterator, std::string::const_iterator> BeginEnd;

typedef std::unique_ptr<const GeoGenfun::AbsFunction> GFPTR;
//
// Forward definition of GenFunctionInterpreter;
//
class GenFunctionInterpreter;

class GenFunctionReader {

 public:

  // Constructor.  Keep a pointer to the interpreter for purposes of recusion.
  GenFunctionReader(const std::string & name, GenFunctionInterpreter *interpreter);

  // Destructor
  virtual ~GenFunctionReader();

  // Execute
  virtual GFPTR execute(std::string::const_iterator , std::string::const_iterator) const=0;

  // Access to the interpreter:
  const GenFunctionInterpreter *getInterpreter() const;

  // Access to the input string name;
  const std::string & getName() const;
  
 protected:

  // Some of the subclasses require two arguments, separated by a |. This
  // splits the argument into its two pieces:
  std::string::const_iterator split(std::string::const_iterator begin, std::string::const_iterator end) const;
  
  private:

  // Name of the text string found on the input:
  const std::string                                           m_name;

  // A pointer to the interpreter:
  const GenFunctionInterpreter                               *m_interpreter;

  // Deleted operations:
  GenFunctionReader(const GenFunctionReader &)               = delete;
  GenFunctionReader & operator = (const GenFunctionReader &) = delete;

  
};

#endif
