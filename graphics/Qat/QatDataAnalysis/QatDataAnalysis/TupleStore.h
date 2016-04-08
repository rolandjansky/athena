#ifndef _TupleStore_h_
#define _TupleStore_h_
#include "CLHEP/GenericFunctions/RCBase.hh"
#include "QatDataAnalysis/TupleConstLink.h"
#include "QatDataAnalysis/TupleLink.h"

class TupleStore : public Genfun::RCBase {


public:

  // Constructor:
  TupleStore();

  // Read
  virtual TupleConstLink operator [](size_t i) const = 0;

  // Write (if implemented)
  virtual void push_back(TupleConstLink) {
    throw std::runtime_error ("Write only tuple store");
  }

  // Get the size:
  virtual size_t size() const = 0;

  // lock
  void lock() const;

  // check lock:
  bool isLocked() const;

protected:

  // Destructor:
  virtual ~TupleStore();

  // Data:
  mutable bool _lock;

  // Silence compiler warnings about solitude
  friend class ImaginaryFriend;


};

typedef Link< TupleStore  > TupleStoreLink;
typedef ConstLink< TupleStore  > TupleStoreConstLink;

class SimpleStore: public TupleStore {

 public:

  // Constructor
  SimpleStore();
  
  // Read
  virtual TupleConstLink operator [](size_t i) const;

  // Write
  virtual void push_back(TupleConstLink);

  // Get the size:
  virtual size_t size() const;

 private:
  
  // Destructor:
  ~SimpleStore();

  // Illegal Operations:
  SimpleStore(const SimpleStore &);
  SimpleStore & operator = (const SimpleStore & );
  
  // Data:
  std::vector<TupleConstLink> _tVector;

  // Silence compiler warnings about solitude
  friend class ImaginaryFriend;

};


class CompoundStore: public TupleStore {
 public:

  // Constructor
  CompoundStore();
  
  // Read
  virtual TupleConstLink operator [](size_t i) const;

  // Extend
  virtual void push_back(TupleStoreLink);

  // Write
  virtual void push_back(TupleConstLink);

  // Get the size:
  virtual size_t size() const;

 private:
  
  // Destructor:
  ~CompoundStore();

  // Illegal Operations:
  CompoundStore(const CompoundStore &);
  CompoundStore & operator = (const CompoundStore & );
  

  // Data:
  std::vector<TupleStoreLink> _sVector;
  
  // Silence compiler warnings about solitude
  friend class ImaginaryFriend;

};


#endif

