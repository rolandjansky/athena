#ifndef _GENFUNCTIONRECORDER_H_
#define _GENFUNCTIONRECORDER_H_

#include  <typeinfo>


namespace GeoGenfun {
  class AbsFunction;
}


//
// Forward definition of GenFunctionPersistifier;
//
class GenFunctionPersistifier;

class GenFunctionRecorder {

 public:

  // Constructor.  Keep a pointer to the persistifier for purposes of recusion.
  GenFunctionRecorder(const std::type_info & info, GenFunctionPersistifier *persistifier);

  // Destructor:
  virtual ~GenFunctionRecorder();

  // Execute
  virtual void execute(const GeoGenfun::AbsFunction & transfunction) const=0;

  // Access to the persistifier:
  const GenFunctionPersistifier *getPersistifier() const;

 private:

  // A pointer to the persistifier:
  const GenFunctionPersistifier                               *m_persistifier;

  // Deleted operations:
  GenFunctionRecorder(const GenFunctionRecorder &)               = delete;
  GenFunctionRecorder & operator = (const GenFunctionRecorder &) = delete;


};


#endif
