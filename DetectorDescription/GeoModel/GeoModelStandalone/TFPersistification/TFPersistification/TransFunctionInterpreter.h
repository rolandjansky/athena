#ifndef _TRANSFUNCTIONINTERPRETER_H_
#define _TRANSFUNCTIONINTERPRETER_H_
#include "TFPersistification/GenFunctionInterpreter.h"
#include <map>
#include <string>
#include <typeinfo>
#include <sstream>
#include <memory>

//
// Forward definition of "Transfunction"
//
namespace GeoXF {
  class Function;
}

//
// Forward definition of TransFunctionRecorder;
//
class TransFunctionReader;
typedef std::unique_ptr<const GeoXF::Function> TFPTR;

class TransFunctionInterpreter {

 public:

  // Constructor
  TransFunctionInterpreter();

  // Destructor
  ~TransFunctionInterpreter();
  
  // Registers a reader:
  void add(const std::string &str, const TransFunctionReader * reader); 

  // Interprets a string.  
  TFPTR interpret(const std::string & str) const;

  // Retrieves the genfunction persistitier:
  const GenFunctionInterpreter * getGenFunctionInterpreter() const;
  
 private:

  // Store the actual interpreters, map them into the type name
  std::map<std::string, const TransFunctionReader *> m_readerMap;

  // A pointer to the GenFunctionInterpreter:
  const GenFunctionInterpreter m_fInterpreter;

  // Deleted methods:
  TransFunctionInterpreter(const TransFunctionInterpreter &)               = delete;
  TransFunctionInterpreter & operator = (const TransFunctionInterpreter &) = delete;
  
};


#endif


