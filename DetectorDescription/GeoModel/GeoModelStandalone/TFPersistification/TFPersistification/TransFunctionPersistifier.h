#ifndef _GEOPERSISTIFIER_H_
#define _GEOPERSISTIFIER_H_
#include "TFPersistification/GenFunctionPersistifier.h"
#include <map>
#include <string>
#include <typeinfo>
#include <sstream>

//
// Forward definition of "Transfunction"
//
namespace GeoXF {
  class Function;
}

//
// Forward definition of TransFunctionRecorder;
//
class TransFunctionRecorder;
class GenFunctionPersistifier;


class TransFunctionPersistifier {

 public:

  // Constructor
  TransFunctionPersistifier();

  // Destructor
  ~TransFunctionPersistifier();
  
  // Registers a recorder:
  void add(const std::type_info & tInfo, const TransFunctionRecorder * recorder); 

  // Persistifies the function.  Result goes into the coded string.
  void persistify(const GeoXF::Function & f) const;

  // Retrieves the coded string after the persistify operation:
  const std::string & getCodedString() const;

  // Retrieves the stream used to build the expression:
  std::ostringstream & getStream() const;

  // Retrieves the genfunction persistitier:
  const GenFunctionPersistifier * getGenFunctionPersistifier() const;
  
 private:

  // Store the actual recorders, map them into the type name
  std::map<std::string, const TransFunctionRecorder *> m_recorderMap;

  // Here is the result:
  mutable std::string m_codedString;

  // An ostringstream is used to build the coded string:
  mutable std::ostringstream *m_stream;

  // The GenFunctionPersistifier:
  GenFunctionPersistifier m_persistifier;
  
  // Deleted methods:
  TransFunctionPersistifier(const TransFunctionPersistifier &)               = delete;
  TransFunctionPersistifier & operator = (const TransFunctionPersistifier &) = delete;
  
};


#endif


