// INamed.h

#ifndef asg_INamed_H
#define asg_INamed_H

// David Adams
// January 2014
//
// Class to provide name interface for tools.
// Needed to add Gaudi interface to non-Athena tools.

#include <string>

namespace asg {

class INamed {

public:

  // Dtor.
  virtual ~INamed();

  // Return the name.
  virtual std::string name() const =0;

  // Set the name.
  virtual void setName(std::string name) =0;

};

}  // end namespace asg

#endif
