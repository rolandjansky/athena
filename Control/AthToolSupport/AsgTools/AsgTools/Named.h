// Named.h

#ifndef asg_Named_H
#define asg_Named_H

// David Adams
// January 2014
//
// Class to provide names for tools.
// Needed to add Gaudi interface to non-Athena tools.

#include "AsgTools/INamed.h"

namespace asg {

class Named : virtual public INamed {

public:

  // Ctor from name.
  Named(std::string name ="");

  // Dtor.
  ~Named();

  // Return the name.
  std::string name() const;

  // Set the name.
  void setName(std::string name);

private:

  std::string m_name;

};

}  // end namespace asg

#endif
