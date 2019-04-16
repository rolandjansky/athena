#ifndef _GENFUNCTIONINTERPRETER_H_
#define _GENFUNCTIONINTERPRETER_H_
#include <map>
#include <string>
#include <typeinfo>
#include <sstream>
#include <memory>

//
// Forward definition of "Genfunction"
//
namespace GeoGenfun {
  class AbsFunction;
}

//
// Forward definition of GenFunctionRecorder;
//
class GenFunctionReader;

typedef std::unique_ptr<const GeoGenfun::AbsFunction> GFPTR;

class GenFunctionInterpreter {

 public:

  // Constructor
  GenFunctionInterpreter();

  // Destructor
  ~GenFunctionInterpreter();
  
  // Registers a reader. Assumes ownership and will delete
  void add(const std::string &str, const GenFunctionReader * reader); 

  // Interprets a string.  
  GFPTR interpret(std::string::const_iterator begin,
		  std::string::const_iterator end  ) const;

 private:

  // Store the actual interpreters, map them into the type name
  std::map<std::string, const GenFunctionReader *> m_readerMap;

  // Deleted methods:
  GenFunctionInterpreter(const GenFunctionInterpreter &)               = delete;
  GenFunctionInterpreter & operator = (const GenFunctionInterpreter &) = delete;
  
};


#endif


