/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbString.h 726071 2016-02-25 09:23:05Z krasznaa $
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
