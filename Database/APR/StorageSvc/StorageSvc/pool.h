/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//  Package    : StorageSvc (The pool framework)
//
//  Description: Management of the object Databases
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_POOL_H
#define POOL_POOL_H 1

// Framework include files
#include "StorageSvc/DbStatus.h"

// STL include files
#include <string>
#include <utility>

/* Turn this switch if it is possible to agree on a common 
 * base class for all persistent objects.
 */
//#define POOL_HAVE_BASE

#ifdef POOL_HAVE_BASE
#define POOL_BASE               : public pool::DbObject
#define POOL_BASE1(b1)          POOL_BASE, b1
namespace pool {  class DbObject; }
#else
#define POOL_BASE
#define POOL_BASE1(b1)          : b1
namespace pool {  typedef void DbObject; }

#endif
#define POOL_BASE2(b1,b2)       POOL_BASE1(b1), b2
#define POOL_BASE3(b1,b2,b3)    POOL_BASE2(b1,b2), b3
#define POOL_BASE4(b1,b2,b3,b4) POOL_BASE3(b1,b2,b3), b4

#define POOL_BEGIN_STATIC_BLOCK(x) namespace { struct __init  {  __init()  {
#define POOL_END_STATIC_BLOCK(x)   }}; static __init _i;} void* __init_##x ()  {return &_i;}

namespace pool  {
  // Type defintions and forward declarations
  typedef int                   DbAccessMode;
  typedef std::pair< long long, long long > DbLink;
}

/* namespace poolDb
 *
 *  Description:
 *  All specific constants for a storage technology
 *
 *  @author  M.Frank
 *  @version 1.0
 */
namespace pool   {
  class DbInstanceCount;

  static const long long int INVALID = ~0x0LL;

  /// Definition of access modes
  enum AccessMode {
    NONE        =  0,
    NOT_OPEN    =  1<<0,
    READ        =  1<<1, 
    UPDATE      =  1<<2, 
    CREATE      =  1<<3, 
    RECREATE    = (1<<4)+(1<<3),
    WRITE       =  1<<3,
    DESTROY     =  1<<5
  };

  static const DbStatus Success      (DbStatus::Success);
  static const DbStatus Warning      (DbStatus::Warning);
  static const DbStatus Error        (DbStatus::Error);
  static const DbStatus ConnTimeout  (DbStatus::Error+2);

  /// Issue a debug break
  void      debugBreak();

  /// Issue a debug break with error message (to std::cout !!)
  void      debugBreak( const std::string& src, 
                        const std::string& msg,
                        bool rethrow=true);

  /// Debug break with printout and exception chaining
  void      debugBreak( const std::string& src, 
                        const std::string& msg,
                        const std::exception& e,
                        bool rethrow=true);

  /// Check for tracing
  bool      doTrace();

  /// Translate access mode to string
  const char* accessMode(pool::DbAccessMode access_mode);

  /// Delete a pointer
  template<class T> inline DbStatus deletePtr(T*& p)  {
    if ( 0 != p )    {
      delete p;
      p = 0;
    }
    return Success;
  }
  /// Release a pointer
  template<class T> inline DbStatus releasePtr(T*& p)  {
    if ( 0 != p )    {
      p->release();
      p = 0;
    }
    return Success;
  }

  /// Release Reference countable pointer
  template<class T> inline int decrementPtr(T*& p)  {
    if ( p )    {
      int cnt = p->release();
      if ( 0 >= cnt )  {
        p = 0;
      }
      return cnt;
    }
    return ~0x0;
  }

  class RefCounter {
  private: 
    int m_count = 1;
  public:
    RefCounter() {}
    RefCounter( const RefCounter& ) { m_count = 1; }
    RefCounter& operator= (const RefCounter&) { return *this; }
    /// Increase the reference count
    int addRef()   { return ++m_count; }
    /// Decrease the reference count 
    int subRef()   { return --m_count; }
  };
    
}
#endif  // POOL_POOL_H
