/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  DbContainerObj class implementation
//--------------------------------------------------------------------
//
//  Package    : StorageSvc (The POOL project)
//
//  Description: Generic data persistency
//
//  @author      M.Frank
//====================================================================

// Framework include files
#include "DbContainerObj.h"

#include "POOLCore/DbPrint.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/IDbContainer.h"
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbInstanceCount.h"
#include <memory>
using namespace std;
using namespace pool;

int DbObjectHolder::release() {  return 1;    }

// Enable this to force regular retirement
void retireDatabase(DbContainerObj* c)  {
  static int i=0;
  if ( (++i%2)==0 )  {
    c->database().setAge(20);
    c->database().containedIn().closeAgedDbs();
  }
}

static DbInstanceCount::Counter* s_count = DbInstanceCount::getCounter(typeid(DbContainerObj));

/// Constructor
DbContainerObj::DbContainerObj( const DbDatabase& dbH,
                                const string&     nam, 
                                const DbType&     dbtyp,
                                DbAccessMode      mod)   
: Base(nam, mod, dbtyp, dbH.db()), m_info(0), m_tokH(0)
{
  DbPrint log( dbH.logon() );
  m_isOpen    = false;
  s_count->increment();

  if ( 0 != db() && dbtyp == dbH.type() )   {
    if ( dbH.isValid() )  {
      if ( dbH.add( name(), this).isSuccess() )    {
        m_dbH  = dbH;
        setMode(mod);
        if ( mod & pool::UPDATE ) {
          setMode(mod |= pool::CREATE);
        }
        log << DbPrintLvl::Debug << "--> Access   DbContainer  " 
            << accessMode(mode())
            << " [" << type().storageName() << "] " 
            << name() 
            << DbPrint::endmsg;
        return;
      }
    }
  }
  log << DbPrintLvl::Error
      << "--> Access   DbContainer  "
      << " Mode:" << accessMode(mode()) 
      << "  " << name()
      << " impossible."
      << " [" << type().storageName() << "] " 
      << DbPrint::endmsg;
  type().missingDriver(log);
}

// Destructor
DbContainerObj::~DbContainerObj()     {
  string id = m_dbH.isValid() ? m_dbH.logon() : name();
  DbPrint log( id );
  s_count->decrement();
  clearEntries();
  releasePtr(m_info);
  m_dbH.remove(this);
  log << DbPrintLvl::Debug
      << "--> Deaccess DbContainer  " 
      << accessMode(mode()) 
      << " [" << type().storageName() << "] " 
      << name()
      << DbPrint::endmsg;
}

// Check database access
bool DbContainerObj::hasAccess()    {
  if ( 0 == m_info )  {
    const DbTypeInfo* typ = m_dbH.contShape(name());
    if ( typ )  {
      if ( open(typ).isSuccess() )  {
        return true;
      }
      releasePtr(m_info);
    }
  }
  return m_info != 0;
}

// Retrieve container size
long long int DbContainerObj::size()   {
  if ( !hasAccess() )    {
    string id = database().isValid() ? database().logon() : name();
    DbPrint log( id );
    log << DbPrintLvl::Error
        << "--> Access   DbContainer::size()" 
        << "  " << name()
        << " impossible - invalid object!"
        << DbPrint::endmsg;
    return -1;
  }
  database().setAge(0);
  return m_info->size();
}

/// Open Database container
DbStatus DbContainerObj::open(const DbTypeInfo* typ)   {
  if ( !m_isOpen )    {
    if ( 0 == m_info )  {
      m_info = db()->createContainer(type());
    }
    if ( 0 != m_info && 0 != typ && database().isValid() )  {
      DbStatus sc = info()->open(database(), name(), typ, mode());
      if ( sc.isSuccess())  {
        if ( mode() != pool::READ )  {
          Token tok;
          tok.setDb(database().name());
          tok.setCont(name());
          tok.setTechnology(type().type());
          tok.setClassID(typ->shapeID());
          sc = database().makeLink(&tok, tok.oid());
          if ( !sc.isSuccess() )   {
            m_info->close();
            return sc;
          }
          sc = database().addShape(typ);
          if ( !sc.isSuccess() )  {
            m_info->close();
            return sc;
          }
        }
        m_tokH = database().cntToken(name());
        if ( m_tokH )    {
	  m_info->setSections(database().sections(name()));
          m_isOpen = true;
          return Success;
        }
      }
      return sc;
    }
  }
  else if ( m_isOpen && typ ) {
    return m_dbH.addShape(typ);
  }
  return Error;
}

/// Close Database container
DbStatus DbContainerObj::close()   {
  if ( retire().isSuccess() )    {
    m_dbH.remove(this);
    return Success;
  }
  return Error;
}
/// Close Database container
DbStatus DbContainerObj::retire()   {
  if ( m_isOpen )    {
    if ( 0 != m_info )    {
      if ( m_info->close() )   {
        m_isOpen = false;
        releasePtr(m_info);
        return Success;
      }
      releasePtr(m_info);
    }
    m_isOpen = false;
    return Error;
  }
  return Success;
}

/// Query the size of the pending transaction stack
bool DbContainerObj::updatesPending() const  {
  return m_info != 0 ? m_info->updatesPending() : false;
}

/// Execute Transaction Action
DbStatus DbContainerObj::transAct(Transaction::Action action) {
   return m_info?  m_info->transAct(action) : Success;
}

/// Pass options to the implementation
DbStatus DbContainerObj::setOption(const DbOption& refOpt) {
  return hasAccess() ? m_info->setOption(refOpt) : Error;
}

/// Access options
DbStatus DbContainerObj::getOption(DbOption& refOpt) {
  return hasAccess() ? m_info->getOption(refOpt) : Error;
}

/// In place allocation of raw memory
void* DbContainerObj::allocate(unsigned long siz, 
                               DbContainer& cntH,
                               ShapeH shape)
{
  if ( !isReadOnly() && hasAccess() )  {
    m_dbH.setAge(0);
    return m_info->allocate(siz, cntH, shape);
  }
  return 0;
}

/// In place allocation of raw memory
DbStatus DbContainerObj::allocate(DbContainer& cntH,
                                  const void* object,
                                  ShapeH shape,
                                  Token::OID_t& oid) 
{
  if ( !isReadOnly() && hasAccess() )  {
    m_dbH.setAge(0);
    if ( object )  {
      return m_info->allocate(cntH, object, shape, oid);
    }
  }
  throw bad_alloc();
}

/// In place free of raw memory
DbStatus DbContainerObj::free(void* ptr, DbContainer& cntH)   {
  m_dbH.setAge(0);
  return hasAccess() ? m_info->free(ptr, cntH) : Error;
}

/// Add persistent type. Returns error if the type is not supported.
DbStatus DbContainerObj::addShape(const DbTypeInfo* typ)  {
  if ( hasAccess() )   {
    if ( m_info->isShapeSupported(typ) )  {
      return m_dbH.addShape(typ);
    }
  }
  return Error;
}

/// Retrieve persistent type information
const DbTypeInfo* DbContainerObj::objectShape(const Guid& guid) const {
  return m_dbH.objectShape(guid);
}

/// Add entry to container
DbStatus DbContainerObj::save(const DbObjectHandle<DbObject>& objH, const DbTypeInfo* typ) {
  if ( !isReadOnly() && hasAccess() && m_isOpen && objH.isValid() ) {
    if ( m_info->save(objH).isSuccess() ) {
      if ( m_dbH.addShape(typ).isSuccess() ) {
        m_dbH.setAge(0);
        return Success;
      }
    }
  }
  return Error;
}

/// Remove transient object representation 
DbStatus DbContainerObj::remove(ObjHandle& objH) {
  if ( hasAccess() && objH.isValid() && m_isOpen )    {
    DbObject* it = objH.ptr();
    DbObjectHolder holder(it);
    objH._setObject(0);
    Base::remove(&holder);
    m_dbH.setAge(0);
    return Success;
  }
  return Error;
}

/// Destroy an existing persistent object identified by its handle
DbStatus DbContainerObj::destroy(const Token::OID_t& linkH) {
  if ( !isReadOnly() && hasAccess() && m_isOpen )  {
    m_dbH.setAge(0);
    return m_info->destroy(linkH);
  }
  return Error;
}

/// Save new object in the container and return its handle
DbStatus DbContainerObj::save(DbContainer&  cntH, const void* object, ShapeH shape, Token::OID_t& linkH)  {
  if ( !isReadOnly() && hasAccess() && object && m_isOpen ) {
    m_dbH.setAge(0);
    return m_info->save(cntH, object, shape, linkH);
  }
  return Error;
}

/// Update an object to the container identified by its handle
DbStatus DbContainerObj::update(DbContainer& cntH, const void* object, ShapeH shape, const Token::OID_t& linkH)  {
  if ( !isReadOnly() && hasAccess() && object && m_isOpen ) {
    m_dbH.setAge(0);
    return m_info->update(cntH, object, shape, linkH);
  }
  return Error;
}

/// Update an object to the container identified by its handle
DbStatus DbContainerObj::update(DbContainer& cntH, const void* object, ShapeH shape, const DbObjectHandle<DbObject>& objH)
{
  if ( !isReadOnly() && hasAccess() && m_isOpen ) {
    m_dbH.setAge(0);
    return m_info->update(cntH, object, shape, objH);
  }
  return Error;
}

/// Select object in the container identified by its handle
DbStatus DbContainerObj::load(DataCallBack* call,
                              const Token::OID_t& linkH, 
                              Token::OID_t& oid,
                              bool          any_next)
{
  if ( hasAccess() && call && m_isOpen )  {
    m_dbH.setAge(0);
    oid.first  = token()->oid().first;
    // Specific implementation may overwrite OID
    return m_info->load(call, linkH, oid, any_next);
  }
  return Error;
}

/// Perform UPDATE select
DbStatus DbContainerObj::update(DbSelect& sel)  {
  if ( !isReadOnly() && hasAccess() )  {
    m_dbH.setAge(0);
    return m_info->update(sel);
  }
  return Error;
}

/// Perform DELETE statement
DbStatus DbContainerObj::destroy(DbSelect& sel) {
  if ( !isReadOnly() && hasAccess() )  {
    m_dbH.setAge(0);
    return m_info->destroy(sel);
  }
  return Error;
}

/// Perform selection
DbStatus DbContainerObj::select(DbSelect& sel) {
  if ( hasAccess() )   {
    m_dbH.setAge(0);
    sel.setShapeID(token()->classID());
    sel.link() = token()->oid();
    return m_info->select(sel);
  }
  return Error;
}

/// Fetch next object address of the selection to set token
DbStatus DbContainerObj::fetch(DbSelect& sel) {
  // retireDatabase(this);  // Uncomment to enforce database retirement.
  if ( hasAccess() )   {
    m_dbH.setAge(0);
    return m_info->fetch(sel);
  }
  return Error;
}
