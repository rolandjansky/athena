/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FileDescriptor.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Storage service file descriptor class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_FILEDESCRIPTOR_H
#define POOL_FILEDESCRIPTOR_H 1

#include "uuid/uuid.h"
#include "CxxUtils/MD5.h"
#include "PersistentDataModel/Guid.h"

#include <string>

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // as from IFileCatalog
  typedef std::string FileID;
  typedef class DatabaseConnection *ConnectionH;

  /** @class FileDescriptor FileDescriptor.h POOLCore/FileDescriptor.h
    *
    * Description:
    *
    * Definition of a small class capable to encapsulate
    * a file in terms of file handle, of the generic Db data handle.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class FileDescriptor   {
  private:
    /// File Identifier (UNIQUE)
    FileID            m_FID;
    /// Physical file name
    std::string       m_PFN;
    /// Handle to the Database connection
    ConnectionH       m_DBC;

  public:
    /// Standard Constructor
    FileDescriptor() : m_DBC(0)             {                     }
    /// Constructor with initializing arguments
    FileDescriptor(const FileID& fid, const std::string& pfn)
      : m_FID(fid), m_PFN(pfn), m_DBC(0)    {
      if (!m_FID.empty() && Guid(m_FID).toString() != m_FID) {
        MD5 checkSum((unsigned char*)m_FID.c_str(), m_FID.size());
        uuid_t checkSumUuid;
        checkSum.raw_digest((unsigned char*)(&checkSumUuid));
        char text[37];
        uuid_unparse_upper(checkSumUuid, text);
        m_FID = text;
      }
    }

    /// Access to physical file name (READ)
    const std::string& PFN()  const         { return m_PFN;       }
    /// Access to physical file name (WRITE)
    void setPFN(const std::string& val)     { m_PFN = val;        }

    /// Access to file identifier (READ)
    const FileID& FID()  const              { return m_FID;       }
    /// Access to file identifier (WRITE)
    void setFID(const FileID& val)          { m_FID = val;        }
    
    /// Access to file descriptor (READ)
    ConnectionH dbc()  const                { return m_DBC;       }
    /// Access to file descriptor (WRITE)
    void setDbc(ConnectionH val)            { m_DBC = val;        }
  };
}       // End namespace pool
#endif  // POOL_FILEDESCRIPTOR_H
