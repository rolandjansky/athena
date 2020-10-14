/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// This is a warapper class for uuid.
// It is copied from POOL because I am not allowed to have this
// dependence in tdaq-common.
// We need to have a unique ID represented in strings in the same way as in POOL.
// Szymon, January 2007

#ifndef OFFLINE_V5_POOL_GUID_H
#define OFFLINE_V5_POOL_GUID_H

#include <string>

/*
 *  POOL namespace (changed for DAQ use)
 */
namespace offline_poolCopy_v5  {

  /** @class Guid Guid.h POOLCore/Guid.h
    *
    * Description:
    *
    * Encapsulation of a GUID/UUID/CLSID/IID data structure (128 bit number).
    * Note: This class may not have a virual destructor
    *
    * @author   M.Frank          Initial version using COM on WIN32
    * @author   Zhen Xie         Include DCE implementation for linux
    * @version  1.1
    * @date     03/09/2002
    */
  class Guid {          // size is 16
  public:
    unsigned int  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];

    /// Standard constructor (No initialization of data for performance reasons)
    Guid()             {                                 }
    /// Standard constructor (With possible initialization)
    explicit Guid(bool assign)  { if ( assign ) create(*this);    }
    /// Constructor for Guid from char*
    explicit Guid(const char* s)        { fromString(s);          }
    /// Constructor for Guid from string
    explicit Guid(const std::string& s) { fromString(s);          }
    /// Copy constructor
    Guid(const Guid& c)                 { *this = c;              }
    /// Assignment operator
    Guid& operator=(const Guid& g)    {
      if ( this != &g )  {
        Data1 = g.Data1;
        Data2 = g.Data2;
        Data3 = g.Data3;
        for (size_t i = 0; i < 8; i++) {
          Data4[i] = g.Data4[i];
        }
      }
      return *this;
    }
    /// Smaller operator
    bool operator<(const Guid& g)  const;
    /// Equality operator
    bool operator==(const Guid& g)  const  {
      if ( this != & g )  {
        if (Data1 != g.Data1 ) return false;
        if (Data2 != g.Data2 ) return false;
        if (Data3 != g.Data3 ) return false;
        const unsigned int *p = (const unsigned int*)&Data4[0], 
                            *q = (const unsigned int*)&g.Data4[0];
        return *p++ == *q++ && *p == *q;
      }
      return true;
    }
    /// Non-equality operator
    bool operator!=(const Guid& g)  const  {
      return !(this->operator==(g));
    }
    /// Automatic conversion from string reprentation
    const std::string toString() const;
    /// Automatic conversion to string representation
    const Guid& fromString(const std::string& s);
    /// NULL-Guid: static class method
    static const Guid& null();
    /// Create a new Guid
    static void create(Guid& guid);
  };
}
#endif // OFFLINE_V5_POOL_GUID_H
