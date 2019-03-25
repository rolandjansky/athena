/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//
//  Package    : StorageSvc (The POOL project)
//
//  @author      M.Frank
//
//====================================================================

/// Framework include files
#include "POOLCore/DbPrint.h"
#include "StorageSvc/DbHeap.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DbContainerImp.h"

using namespace std;
using namespace pool;

/// Standard Constructor
DbContainerImp::DbContainerImp()
: m_size(0), m_writeSize(0), m_name("UNKNOWN"),
  m_canUpdate(false),
  m_canDestroy(false)
{
  DbInstanceCount::increment(this);
  m_stackType = NONE;
}

/// Standard Destructor
DbContainerImp::~DbContainerImp() {
  DbInstanceCount::decrement(this);
  m_stack.clear();
}

/// Size of the container
long long int DbContainerImp::size()  {
  return m_writeSize;
}

/// Number of next record in the container (=size if no delete is allowed)
long long int DbContainerImp::nextRecordId()   {
  return size();
}

/// Access options
DbStatus DbContainerImp::getOption(DbOption& /* opt */) const { 
  return Error;  
}

/// Set options
DbStatus DbContainerImp::setOption(const DbOption& /* opt */){ 
  return Success;
}

/// Close the container and deallocate resources
DbStatus DbContainerImp::close()   {
  return clearStack();
}

/// In place allocation of raw memory for the transient object
void* DbContainerImp::allocate(unsigned long siz, DbContainer& cntH, ShapeH shape)  {
  DbObjectHandle<DbObject> objH(cntH.type());
  Token::OID_t objLink(cntH.token()->oid().first, nextRecordId());
  DbHeap::allocate(siz, &cntH, &objLink, &objH);
  if ( m_stack.size() < m_size+1 )  {
    m_stack.resize(m_size+1024);
  }
  m_stack[m_size] = DbAction( objH.ptr(), shape, objLink, pool::WRITE );
  m_stackType |= pool::WRITE;
  m_writeSize++;
  m_size++;
  return objH.ptr();
}

/// In place allocation of raw memory for the transient object
DbStatus DbContainerImp::allocate(DbContainer& cntH, const void* object, ShapeH shape, Token::OID_t& oid) {
  if ( object )  {
    oid.first  = cntH.token()->oid().first;
    oid.second = nextRecordId();
    if ( m_stack.size() < m_size+1 )  {
      m_stack.resize(m_size+1024);
    }
    m_stack[m_size] = DbAction( object, shape, oid, WRITE );
    m_stackType |= pool::WRITE;
    m_writeSize++;
    m_size++;
    return Success;
  }
  throw bad_alloc();
}

/// In place deletion of raw memory
DbStatus DbContainerImp::free(void* ptr, DbContainer& cntH) {
  return DbHeap::free(ptr, &cntH);
}

/// Reset action list
DbStatus DbContainerImp::clearStack()   {
  m_size = 0;
  m_writeSize = 0;
  m_stackType = NONE;
  return Success;
}

/// Execute object modification requests during a transaction
DbStatus DbContainerImp::commitTransaction() {
  DbStatus iret   = Success;
  DbStatus status = Success;
  ActionList::iterator i = m_stack.begin();
  for(size_t j=0; j < m_size; ++j, ++i )  {
    switch( (*i).action )  {
      case pool::DESTROY:
        status = destroyObject(*i);
        break;
      case pool::UPDATE:
        status = updateObject(*i);
        break;
      case pool::WRITE:
        status = writeObject(*i);
        break;
      default:
        status = Error;
        break;
    }
    if ( !status.isSuccess() ) {
      iret = status;
      DbPrint log( m_name);
      log << DbPrintLvl::Error << "The Transaction cannot be committed..." 
          << " Container has " << size() << " Entries in total."
          << DbPrint::endmsg;
      break;
    }
  }
  return iret;
}


/// Execute Database Transaction action
DbStatus DbContainerImp::transAct(Transaction::Action action)
{
  DbStatus status = Success;
  if( action==Transaction::TRANSACT_COMMIT || action==Transaction::TRANSACT_FLUSH ) {
     status = commitTransaction();
  }
  clearStack();
  return status;
}

DbStatus 
DbContainerImp::save(const DbObjectHandle<DbObject>& objH)  {
  // Can only be done if no Transaction is ongoing...
  // i.e. exactly one object was allocated
  if ( m_writeSize == 1 )   {
     if ( m_stack.begin()->object == objH.ptr() )   {
        objH.oid() = m_stack.begin()->link;
        DbStatus status = writeObject( *m_stack.begin() );
        clearStack();
        return status;
     }
  }
  return Error;
}

DbStatus
DbContainerImp::save(DbContainer& /* cntH */, const void* object, ShapeH shape, Token::OID_t& linkH)
{
  // Only possible if no open transaction, i.e. No object was allocated
  if ( m_stack.empty() )  {
     DbAction act(object, shape, linkH, WRITE);
     return writeObject( act );
  }
  return Error;
}

// Update existing object in the container
DbStatus 
DbContainerImp::update(DbContainer& /* cntH */, const void* object, ShapeH shape, const DbObjectHandle<DbObject>& objH) {
  if ( m_canUpdate )  {
    if ( true || object )  {
      if ( m_stack.size() < m_size+1 )  {
        m_stack.resize(m_size+1024);
      }
      m_stack[ m_size ] = DbAction(objH.ptr(), shape, objH.oid(), pool::UPDATE);
      m_stackType |= pool::UPDATE;
      m_size++;
      return Success;
    }
    return Error;
  }
  DbPrint log( m_name );
  log << DbPrintLvl::Error << "The chosen implementation does not allow to "
      << "update existing objects."
      << "The requested action is not supported." << DbPrint::endmsg;
  return Error;
}

// Update existing object in the container
DbStatus
DbContainerImp::update(DbContainer& /* cntH */, const void* object, ShapeH shape, const Token::OID_t& linkH)
{
  if ( m_canUpdate )  {
    if ( object )  {
      if ( m_stack.size() < m_size+1 )  {
        m_stack.resize(m_size+1024);
      }
      m_stack[ m_size ] = DbAction(object, shape, linkH, pool::UPDATE);
      m_stackType |= pool::UPDATE;
      m_size++;
      return Success;
    }
    return Error;
  }
  DbPrint log( m_name );
  log << DbPrintLvl::Error << "The chosen implementation does not allow to "
      << "update existing objects."
      << "The requested action is not supported." << DbPrint::endmsg;
  return Error;
}

// Fetch next object address of the selection to set token
DbStatus DbContainerImp::fetch(DbSelect& sel) {
  if ( sel.criteria().length() == 0 || sel.criteria() == "*" )  {
    Token::OID_t lnk = sel.link();
    while ( lnk.second < size() ) {
      if ( fetch(lnk, lnk).isSuccess() )  {
        sel.link() = lnk;
        return Success;
      }
      lnk.second++;
    }
    return Error;
  }
  DbPrint log( m_name );
  log << DbPrintLvl::Error << "The chosen implementation does not allow to "
      << "refine container scans."
      << "The only valid selection criterium is: \"\" (empty string)"
      << DbPrint::endmsg;
  return Error;
} 

/// Destroy persistent object in the container; does not touch transient!
DbStatus DbContainerImp::destroy(const Token::OID_t& linkH) {
  if ( m_canDestroy )  {
    if ( m_stack.size() < m_size+1 )  {
      m_stack.resize(m_size+1024);
    }
    m_stack[ m_size ] = DbAction(0, 0, linkH, DESTROY);
    m_stackType |= DESTROY;
    m_size++;
    return Success;
  }
  DbPrint log( m_name );
  log << DbPrintLvl::Error << "The chosen implementation does not allow to "
      << "destroy existing objects from the container."
      << "The requested action is not supported." << DbPrint::endmsg;
  return Error;
}

// Fetch refined object address. Default implementation returns identity
DbStatus DbContainerImp::fetch(const Token::OID_t& linkH, Token::OID_t& stmt)  {
  stmt.second = linkH.second;
  return linkH.second >= 0 && linkH.second < size() ? Success : Error;
}

// Interface Implementation: Find entry in container
DbStatus DbContainerImp::load( void** ptr, ShapeH shape, 
                               const Token::OID_t& linkH,
                               Token::OID_t& oid,
                               bool          any_next)
{
  DbStatus sc = Error;
  oid.second = linkH.second;
  if ( any_next ) {
    while ( oid.second < size() ) {
      sc = fetch(linkH, oid);
      if ( sc.isSuccess() )  {
         sc = loadObject(ptr, shape, oid);
         if ( sc.isSuccess() )  {
            return sc;
         }
      }
      oid.second++;
    }
    if ( linkH.second < 0 || linkH.second <= size() ) {
      DbPrint log( m_name );
      log << DbPrintLvl::Debug << "No objects passing selection criteria..." 
	  << " Container has " << size() << " Entries in total." << DbPrint::endmsg;
    }
  }
  else {
     sc = loadObject(ptr, shape, oid);
  }
  return sc;
}

/// Find object by object identifier and load it into memory
DbStatus DbContainerImp::loadObject( void**, ShapeH,
                                    Token::OID_t& /* oid  */)
{
  return Error;
}

/// Access section identifier from OID
const DbSection& DbContainerImp::getSection(const Token::OID_t& oid)  const {
  for(size_t i=0; i < m_sections.size(); ++i) {
    const DbSection& s = m_sections[i];
    if ( s.length != INVALID && s.start+s.length > oid.second ) {
      return m_sections[i];
    }
    else if ( s.length == INVALID && s.start > oid.second && i>0 ) {
      return m_sections[i-1];
    }
  }
  if ( m_sections.size() > 0 ) return m_sections[0];
  static DbSection s;
  return s;
}
