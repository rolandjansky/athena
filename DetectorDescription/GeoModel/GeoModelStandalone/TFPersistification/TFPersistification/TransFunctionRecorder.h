#ifndef _TRANSFUNCTIONRECORDER_H_
#define _TRANSFUNCTIONRECORDER_H_
#include <typeinfo>

//
// Forward definition of "Transfunction"
//
namespace GeoXF {
  class Function;
}

//
// Forward definition of TransFunctionPersistifier;
//
class TransFunctionPersistifier;

class TransFunctionRecorder {

 public:

  // Constructor.  Keep a pointer to the persistifier for purposes of recusion.
  TransFunctionRecorder(const std::type_info & info, TransFunctionPersistifier *persistifier);

  // Destructor
  virtual ~TransFunctionRecorder();

  // Execute
  virtual void execute(const GeoXF::Function & transfunction) const=0;

  // Access to the persistifier:
  const TransFunctionPersistifier *getPersistifier() const;
  
 private:

  // A pointer to the persistifier:
  const TransFunctionPersistifier                               *m_persistifier;

  // Deleted operations:
  TransFunctionRecorder(const TransFunctionRecorder &)               = delete;
  TransFunctionRecorder & operator = (const TransFunctionRecorder &) = delete;

  
};

// Now we add the pre-existing types of transfunction (=GeoXF::Function).  These
// may well be the only types we ever need!.

class ProductRecorder: public TransFunctionRecorder {

 public:
  
  ProductRecorder(TransFunctionPersistifier * persistifier);
  virtual void execute(const GeoXF::Function &) const;
  
};


class PreMultRecorder: public TransFunctionRecorder {

 public:
  
  PreMultRecorder(TransFunctionPersistifier * persistifier);
  virtual void execute(const GeoXF::Function & ) const;
  
};

class PostMultRecorder: public TransFunctionRecorder {

 public:
  
  PostMultRecorder(TransFunctionPersistifier * persistifier);
  virtual void execute(const GeoXF::Function & ) const;
  
};


class PowRecorder: public TransFunctionRecorder {

 public:
  
  PowRecorder(TransFunctionPersistifier * persistifier);
  virtual void execute(const GeoXF::Function &) const;
  
};




#endif
