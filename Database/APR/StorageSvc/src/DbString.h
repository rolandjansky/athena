/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbString.h 593515 2014-04-17 22:47:40Z gemmeren $
//====================================================================
//
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBSTRING_H
#define POOL_DBSTRING_H 1

#include <string>

/*
 * POOL namespace declaration
 */
namespace pool  {
  /// Declaration of DbString class
  class DbString : public std::string  {
  public:
    /// Standard Constructor
    DbString() {}
    /// Constructor with initializer
    DbString(const std::string& s) {  assign(s); }
    /// Standard destructor
    virtual ~DbString() {}
  };
}
#endif // POOL_DBSTRING_H
