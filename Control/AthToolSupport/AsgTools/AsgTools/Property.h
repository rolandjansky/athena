// Property.h

#ifndef asg_Property_H
#define asg_Property_H

// David Adams
// January 2014
//
// Support class for PropertyMgr.

#include <string>
#include <vector>
#include "AsgTools/ToolHandle.h"
#include "AsgTools/ToolHandleArray.h"

class Property {

public:

  enum Type {
    UNKNOWNTYPE,
    BOOL,
    INT,
    FLOAT,
    DOUBLE,
    STRING,
    INTVECTOR,
    FLOATVECTOR,
    STRINGVECTOR,
    TOOLHANDLE,
    TOOLHANDLEARRAY
  };

  typedef std::vector<int>         IntVector;
  typedef std::vector<float>       FloatVector;
  typedef std::vector<std::string> StringVector;

  static std::string name(Type type);

public:

  // Default ctor.
  Property();

  // Full ctor.
  Property(Type type);

  // Dtor.
  virtual ~Property();

  // Return if this is a valid property.
  // Must have a known type.
  bool isValid() const;

  // Return the type index.
  Type type() const;

  // Return the type name.
  std::string typeName() const;

  // Set the value from another property.
  // Returns 0 for success.
  // The implementation here fails with error -1.
  virtual int setFrom(const Property& rhs);

private:

  Type m_type;

};

#endif
