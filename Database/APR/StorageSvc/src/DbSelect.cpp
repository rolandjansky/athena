/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DbSelect.cpp 726071 2016-02-25 09:23:05Z krasznaa $
//====================================================================
//
//  Package    : System (The POOL project)
//
//  Description: Generate iteration context with selective object access
//
//  @author      M.Frank
//====================================================================
// Framework include files
#include "PersistentDataModel/Guid.h"
#include "PersistentDataModel/Token.h"
#include "StorageSvc/DbPrint.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbContainer.h"

// C++ include files
#include <memory>

namespace pool {
  /** @class DbSelectContext
    *
    * Private class for use of DbSelects.
    */
  class DbSelectContext   {
  public:
    DbSelectContext() 
      : dbH(POOL_StorageType), cntH(POOL_StorageType), 
        stmH(0), shape(Guid::null())
    {
      linkH.first = linkH.second = INVALID;
    }
    DbDatabase            dbH;
    DbContainer           cntH;
    DbSelect::Pointer*    stmH;
    Token::OID_t          linkH;
    Guid                  shape;
  };
}
using namespace pool;

/// Constructor taking selection criteria
DbSelect::DbSelect(const std::string& crit) : m_context(0), m_criteria(crit)  { 
  m_context = new DbSelectContext();
}

/// Standard destructor
DbSelect::~DbSelect()   {
  deletePtr(m_context->stmH);
  deletePtr(m_context);
}

/// Access referring database context
DbDatabase& DbSelect::database() const  {
  return m_context->dbH;
}

/// Access referring container context
DbContainer& DbSelect::container() const  {
  return m_context->cntH;
}

/// Access to link handle
Token::OID_t& DbSelect::link() const  {
  return m_context->linkH;
}

/// Access to existing selection statement
DbSelect::Pointer* DbSelect::statement() const  {
  return m_context->stmH;
}

/// Set selection statement
void DbSelect::setStatement(Pointer* p) {
  m_context->stmH = p;
}

/// Set database context
void DbSelect::setDatabase(const DbDatabase& dbcH)  {
  m_context->dbH = dbcH;
}

/// Set container context
void DbSelect::setContainer(DbContainer& cntH)  {
  m_context->cntH = cntH;
}

/// Access reflection class
const Guid& DbSelect::shapeID() const {
  return m_context->shape;
}

/// Set reflection class
void DbSelect::setShapeID(const Guid& guid) {
  m_context->shape = guid;
}

/// Start selective iterator over a container in a database
DbStatus DbSelect::start(DbDatabase& dbH, const std::string& cntName)   {
  if ( dbH.isValid() )  {
    setDatabase(dbH);
    const Token* cntToken = dbH.cntToken(cntName);
    if ( cntToken )  {
      DbType typ(cntToken->technology());
      DbContainer cntH(typ);
      DbStatus sc = cntH.open(dbH, cntName, 0, typ, pool::READ);
      if ( sc.isSuccess() )  {
        // Now select according to criteria
        setContainer(cntH);
        if ( cntH.select(*this).isSuccess() )  {
          return Success;          
        }
        // What to do ?
        DbPrint err0(cntName);
        err0 << DbPrintLvl::Error
             << "Cannot issue select criteria on container:" << cntName
             << DbPrint::endmsg;
        return Error;
      }
      DbPrint err1(cntName);
      err1 << DbPrintLvl::Error
           << "The requested container:" << cntName << " cannot be opened!" 
           << DbPrint::endmsg;
      return Error;
    }
    DbPrint err2(cntName);
    err2 << DbPrintLvl::Error
         << "The requested container:" << cntName 
         << " seems not to be part of this database!" << DbPrint::endmsg;
    return Error;
  }
  DbPrint err3(cntName);
  err3 << DbPrintLvl::Error
       << "The requested database for the container:" << cntName 
       << " is invalid!" << DbPrint::endmsg;
  return Error;
}

/// Access the next token of a sequence
DbStatus DbSelect::next(Token*& refpToken)    {
  refpToken = 0;
  DbDatabase& dbH = database();
  if ( dbH.isValid() )  {
    DbContainer& cntH = container();
    if ( cntH.isValid() )  {
      DbStatus sc = cntH.fetch(*this);
      if ( sc.isSuccess() )  {
        /// Create token according to OID
        std::auto_ptr<Token> new_token(new Token(cntH.token()));
        new_token->setClassID(shapeID());
        new_token->oid() = link();
        sc = dbH.makeLink(new_token.get(), new_token->oid());
        if ( sc.isSuccess() )  {
          refpToken = new_token.release();
          return sc;
        }
      }
      return sc;
    }
  }
  return Error;
}
