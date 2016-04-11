/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbToken.h 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//  Pool DbToken class definitions
//--------------------------------------------------------------------
//
//  Package    : StorageSvc  (The POOL project)
//  @author      M.Frank
//====================================================================
#ifndef POOL_DBTOKEN_H
#define POOL_DBTOKEN_H 1

// Framework include files
#include "PersistentDataModel/Token.h"

#include <vector>

/*
 *   POOL namespace declaration
 */
namespace pool    {

  // Forward declarations
  class DbTypeInfo;

  /** Db objects: DbToken, DbToken.h, POOLCore/DbToken.h
    *
    * Description:
    *
    * Definition of an optimized token used internally in the Database.
    * This token implementation generates an MD5 checksum over the
    * token information. This should be advantageous for long tokens
    * compared to the horrible full lookup of the string exceeding
    * typically 300 chars.
    *
    * @author  M.Frank
    * @version 1.0
    */
  class DbToken : public Token  {
  public:
    /// Default token key enumeration
    enum {TOKEN_FULL_KEY     = 1<<0, 
          TOKEN_CONT_KEY     = 1<<1,
          TOKEN_ITERATOR     = 1<<2
    };
  private:
    /// Buffer to hold user key identifying full token
    mutable Guid                m_keyFull;
    /// Buffer to hold user key #1
    mutable Guid                m_contKey;
    /// Optional other user keys
    mutable std::vector< Guid > m_ukeys;
    /// Database type information cache
    mutable const DbTypeInfo*   m_info;
    /// Flag to indicate if toekn is local or foreign
    bool                        m_local;
  public:
    /// Produce the token keys only on demand for export
    static void makeKey(const Token* tok, int which, Guid& guid);
    /// Standard constructor
    DbToken() : m_info(0), m_local(false)    {}
    /// Fast token comparison: operator equal
    virtual bool equal(const Token& pTok) const;
    /// Fast token comparison: operator less
    virtual bool less(const Token& pTok) const;
    /// Produce the keys only on demand
    void setKey(int which);
    /// Copy the key 
    void setKeyEx(int which, const Guid& guid);
    /// Set a user key
    void setUserKey(int i, const Guid& guid);
    /// Update type information
    void setTypeInfo(const DbTypeInfo* t) const { m_info = t;     }
    /// Access type information
    const DbTypeInfo* typeInfo() const    { return m_info;        }
    /// Set local flag
    void setLocal(bool val)               { m_local = val;        }
    /// Access local flag
    bool isLocal() const                  { return m_local;       }
    /// Add type setting
    void addTypeSetting(int val)          { setType(type() | val); }
    /// Add type setting
    void removeTypeSetting(int val)       { setType(type() & ~val); }
    /// Access to user key identifying full token
    const Guid& fullKey()  const          { return m_keyFull;     }
    /// Access to user key #1
    const Guid& contKey()  const          { return m_contKey;     }
    /// Access to optional user keys #2
    const Guid& userkey(int i)  const     { return m_ukeys[i];    }
  };
}       // End namespace pool
#endif  // POOL_DBTOKEN_H
