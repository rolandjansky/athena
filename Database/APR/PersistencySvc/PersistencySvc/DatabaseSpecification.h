/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_DATABASESPECIFICATION_H
#define INCLUDE_PERSISTENCYSVC_DATABASESPECIFICATION_H

namespace pool {
  class DatabaseSpecification {
  public:
    /** Enumeration type specifying the database name field,
     * wherever the latter is used in methods accessing a database.
     */
    typedef enum {
    UNDEFINED,
      PFN,    /// Physical File Name
      FID,    /// File IDentifier
      LFN     /// Logical File Name
      } NameType;
  };
}

#endif
