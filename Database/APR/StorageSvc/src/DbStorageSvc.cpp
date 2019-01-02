/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /cvs/PF/pool/StorageSvc/src/DbStorageSvc.cpp,v 1.81 2010/05/05 11:43:26 frankb Exp $
//  ====================================================================
//
//  DbStorageSvc.cpp
//  --------------------------------------------------------------------
//
//  Package   : StorageSvc (POOL)
//  Author    : Markus Frank
//
//  ====================================================================

// Framework include files
#include "PersistentDataModel/Token.h"
#include "DbStorageSvc.h"
#include "DbStorageExplorer.h"
#include "POOLCore/DbPrint.h"
#include "StorageSvc/DbToken.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbReflex.h"
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbContainer.h"
#include "StorageSvc/Transaction.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbTransform.h"
#include "StorageSvc/DbConnection.h"
#include "DbDatabaseObj.h"
#include "StorageSvc/DbSection.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/FileDescriptor.h"
#include "StorageSvc/DatabaseConnection.h"

#include <vector>
#include <memory>
#include <map>

using namespace std;


namespace pool  {

  // factory function implementation
  IStorageSvc* createStorageSvc(const string& componentName){
    return new DbStorageSvc(componentName);
  }
  
  typedef DbObjectHandle<DbObject> ObjHandle;
  typedef const DbTypeInfo    *DbTypeInfoH;
  typedef const DbDatabaseObj *DbDatabaseH;

  class DbClassMap : public map<TypeH, Guid> {};


   
/// Standard Constructor.
DbStorageSvc::DbStorageSvc(const string& name)
: m_name(name),
  m_refCount(0),
  m_sesH(),
  m_domH(POOL_StorageType),
  m_ageLimit(2),
  m_type(POOL_StorageType)
{
  static char *als = getenv("POOL_STORAGESVC_DB_AGE_LIMIT");  
  m_explorer = new DbStorageExplorer(name+".Explorer", m_domH, this);
  if ( als )    {
    int alimit = 2;
    istringstream buf(als);
    buf >> alimit;
    if ( alimit > 0 && buf.good() )  {
      DbPrint log(name);
      m_ageLimit = alimit;
      log << DbPrintLvl::Info    
          << ">   User defined db age limit ($POOL_STORAGESVC_DB_AGE_LIMIT) set to: " 
          << m_ageLimit << DbPrint::endmsg;      
    }
  }  
  // declareProperty("AgeLimit", m_ageLimit);
}

/// Standard destructor.
DbStorageSvc::~DbStorageSvc() {
  delete m_explorer;
  m_domH.close();
  m_domH = 0;
  m_sesH = 0;
}

//--- IInterface::addRef
unsigned int DbStorageSvc::addRef()   {
  m_refCount++;
  return m_refCount;
}

//--- IInterface::release
unsigned int DbStorageSvc::release()   {
  int count = --m_refCount;
  if( count <= 0) {
    delete this;
  }
  return count;
}

//--- IInterface::queryInterface
DbStatus DbStorageSvc::queryInterface(const Guid& riid, void** ppvInterface)  {
  if ( IStorageSvc::interfaceID() == riid )  {
    *ppvInterface = (IStorageSvc*)this;
  }
  else if ( IStorageExplorer::interfaceID() == riid )  {
    return m_explorer->queryInterface(riid, ppvInterface);
  }
  addRef();
  return DbStatus::Success;
}

/// IService implementation: Initilize Service                          
DbStatus DbStorageSvc::initialize()   {
  return Success;
}

/// IService implementation: Finalize Service                           
DbStatus DbStorageSvc::finalize()   {
  m_domH.close();
  m_domH = 0;
  m_sesH = 0;
  return Success;
}

std::string DbStorageSvc::getContName(const FileDescriptor& refDB, const Token& persToken)  {
  if ( m_domH.isValid() )   {
    DbDatabase dbH(DbDatabaseH(refDB.dbc()->handle()));
    if ( dbH.isValid() )  {
      return dbH.cntName(persToken);
    }
  }
  return "";
}

/// Retrieve persistent shape from Storage manager.
DbStatus DbStorageSvc::getShape( const FileDescriptor& fDesc,
                                 const Guid&           objType,
                                 ShapeH&               shape)
{
  shape = 0;
  if ( /*0 != &fDesc &&*/ m_domH.isValid() )   {
    DbDatabase dbH(DbDatabaseH(fDesc.dbc()->handle()));
    if ( !dbH.isValid() )  {
      DbStatus sc = dbH.open(m_domH, fDesc.PFN(), fDesc.FID(), pool::READ);
      if ( !sc.isSuccess() )    {
        DbPrint log( name());
        log << DbPrintLvl::Error << "Failed to open the Database!" << DbPrint::endmsg;
        return sc;
      }
    }
    if ( dbH.isValid() )  {
      shape = dbH.objectShape(objType);
      if ( shape )  {
        return IS_PERSISTENT_SHAPE;
      }
      return SHAPE_NOT_AVAILIBLE;
    }
  }
  DbPrint err( name());
  err << DbPrintLvl::Error << "The storage service is not properly initialized." 
      << DbPrint::endmsg;
  return Error;
}

/// Create a persistent shape.
DbStatus DbStorageSvc::createShape( const FileDescriptor&  /*fDesc   */,
                                    const string&          /*cntName */,
                                    const Guid&              shapeID,
                                    ShapeH&                  shapeH)
{
  DbStatus sc = DbTransform::getShape(shapeID, (const DbTypeInfo*&)shapeH);
  if ( !sc.isSuccess() )    {
    DbPrint log(name());
    DbTypeInfo* typ = DbTypeInfo::create(shapeID);
    if ( 0 != typ )   {
      log << DbPrintLvl::Info 
          << "Building shape according to reflection information using "
          << "shape ID for:" << DbPrint::endmsg
          << typ->clazz().Name() 
          << " [" << shapeID.toString() << "]" 
          << DbPrint::endmsg;
      shapeH = typ;
      return Success;
    }
    log << DbPrintLvl::Error << "The shape with ID=" << shapeID.toString() 
        << " is unknown." << DbPrint::endmsg;
  }
  return sc;
}

/// Register object for write
DbStatus DbStorageSvc::allocate( const FileDescriptor& fDesc,
                                 const string&         refCont,
                                 int                   technology,
                                 const void*           object,
                                 ShapeH                shape,
                                 Token*&               refpToken)
{
   DbStatus sc = Error;
   refpToken = 0;

   if( shape && object ) {
      void* handle = fDesc.dbc()->handle();
      DbDatabase dbH((DbDatabaseH)handle);
      DbContainer cntH(dbH.type());
      sc = cntH.open( dbH, 
                      refCont,
                      DbTypeInfoH(shape),
                      DbType(technology),
                      pool::CREATE|pool::UPDATE);
      if ( sc.isSuccess() ) {
         Token* t = new Token(cntH.token());
         t->setClassID(shape->shapeID());
         sc = cntH.allocate(object, shape, t->oid());
         if ( sc.isSuccess() )  {
            sc = dbH.makeLink(t, t->oid());
            if ( sc.isSuccess() )  {
               refpToken = t;
               return Success;
            }
         }
         t->release();
      }
      return INVALID_CONNECTION_TOKEN;
   }
   DbPrint err( name());
   err << DbPrintLvl::Error 
       << "Cannot allocate persistent object." << DbPrint::endmsg
       << " Shape Handle :"      << (void*)shape
       << " FID="                << fDesc.FID() 
       << " Cnt="                << refCont
       << DbPrint::endmsg;
   return sc;
}

/// In place update of an existing object.
DbStatus DbStorageSvc::update( const FileDescriptor& fDesc,
                               const void*           object,
                               ShapeH                shape,
                               const Token&          refToken )
{
   DbStatus sc = Error;
   if( shape && object ) {
      void* handle = fDesc.dbc()->handle();
      DbDatabase dbH((DbDatabaseH)handle);
      if ( dbH.isValid() ) {
         DbContainer cntH(dbH.find(dbH.cntName(refToken)));
         if ( !cntH.isValid() )  {
            DbType db_typ(refToken.technology());
            const DbTypeInfo* db_info = dbH.objectShape(shape->shapeID());
            cntH.open(dbH, dbH.cntName(refToken), db_info, db_typ, dbH.openMode());
         }
         if ( cntH.isValid() )  {
            return cntH.update(object, shape, refToken.oid());
         }
         DbPrint log( name());
         log << DbPrintLvl::Error << "The container " 
             << refToken.dbID() << "::" << dbH.cntName(refToken)
             << " cannot be accessed." << DbPrint::endmsg
             << "Was this object really read from the container "
             << "the object token claims?"
             << DbPrint::endmsg;
         return INVALID_OBJ_TOKEN;
      }
      return INVALID_CONNECTION_TOKEN;
   }
   DbPrint err( name());
   err << DbPrintLvl::Error 
       << "Cannot update persistent object." << DbPrint::endmsg
       << " Shape Handle :"      << (void*)shape
       << " DB:" << refToken.dbID()
       << DbPrint::endmsg;
   return sc;
}

/// Destroy an existing persistent object.
DbStatus 
DbStorageSvc::destroy( const FileDescriptor& refDB, const Token& refToken )
{
   void* handle = refDB.dbc()->handle();
   DbDatabase dbH((DbDatabaseH)handle);
   if ( dbH.isValid() )  {
      DbContainer cntH(dbH.find(dbH.cntName(refToken)));
      if ( !cntH.isValid() )  {
         DbType db_typ(refToken.technology());
         const DbTypeInfo* db_info = dbH.objectShape(refToken.classID());
         cntH.open(dbH, dbH.cntName(refToken), db_info, db_typ, dbH.openMode());
      }
      if ( cntH.isValid() )  {
         return cntH.destroy(refToken.oid());
      }
      DbPrint log( name());
      log << DbPrintLvl::Error << "The container " 
          << refToken.dbID() << "::" << dbH.cntName(refToken)
          << " cannot be accessed." << DbPrint::endmsg
          << "Was this object really read from the container "
          << "the object token claims?"
          << DbPrint::endmsg;
      return INVALID_OBJ_TOKEN;
   }
   return INVALID_CONNECTION_TOKEN;
}


/// Read a persistent object from the medium.
DbStatus DbStorageSvc::read( const FileDescriptor& fDesc,
                             const Token&          token,
                             ShapeH                shape,
                             void**                object)
{
  string err = "";
  pool::AccessMode mode = pool::READ;
  if ( m_domH.isValid() ) {
    DbType typ(token.technology());
    if ( m_domH.type() == typ ) {
      DbDatabase dbH(m_domH.type());
      const string& fid = fDesc.FID();
      if ( dbH.open(m_domH, fDesc.PFN(), fid, mode).isSuccess() ) {
        DbContainer cntH(dbH.type());
        const DbTypeInfo* typ_info = dbH.objectShape(token.classID());
        if ( cntH.open(dbH, dbH.cntName(token), typ_info, typ, dbH.openMode()).isSuccess() )  {
          if ( typ_info && typ_info == shape ) {
	    Token::OID_t link;
	    if ( dbH.getRedirection(token,link).isSuccess() ) {
	      Token* pt = const_cast<Token*>(&token);
	      pt->oid() = link;
              return ObjHandle::openEx(cntH, token, object, shape, mode);
	    }
          }
          err = " The object shape "+token.classID().toString()+
                " is unknown for this container!";
        }
        else  {
          err = "The requested Database:"+token.dbID().toString()+" cannot be opened!";
        }
      }
      else  {
        err = "The requested Database:"+token.dbID().toString()+" cannot be opened!";
      }
    }
    else  {
      err = "Wait a minute...You cannot mix the technologies:"+
        typ.storageName()+" and "+m_domH.type().storageName();
    }
  }
  DbPrint log( name());
  log << DbPrintLvl::Error << err << DbPrint::endmsg;
  return Error;
}

/// Start a new Database Session.
DbStatus DbStorageSvc::startSession(int accessmode,int technology,SessionH& refSession)  {
  DbPrint err( name());
  m_type   = DbType(technology).majorType();
  int typ  = DbType(technology).majorType();
  if ( m_type.majorType() == typ )  {  // Maybe implement this later
    refSession = 0;
    if ( m_sesH.open().isSuccess() )  {
      if ( m_domH.open(m_sesH, m_type, accessmode).isSuccess() )  {
        m_domH.setAgeLimit(m_ageLimit);
        refSession = SessionH(m_domH.ptr());
        return Success;
      }
      err << DbPrintLvl::Error << "Cannot connect to the domain:"
          << DbType(technology).storageName() << DbPrint::endmsg;
      return Error;
    }
    err << DbPrintLvl::Error << "Cannot start the Database session." << DbPrint::endmsg;
    return Error;
  }
  err << DbPrintLvl::Error 
      << "Cannot start database session, the technology type does not match." 
      << DbPrint::endmsg;
  return Error;
}

/// End the Database session.
DbStatus DbStorageSvc::endSession(const SessionH session) {
  if( session == SessionH(m_domH.ptr()) )  {
    m_domH.close();
    m_sesH = 0;
    return Success;
  }
  return Error;
}

/// Check the existence of a logical Database unit.
DbStatus 
DbStorageSvc::existsConnection( const SessionH session, int /* mode */,const FileDescriptor& fDesc) {
  if ( m_domH.isValid() && session == SessionH(m_domH.ptr()) )   {
    DbDatabase dbH = m_domH.find(fDesc.FID());
    if ( dbH.isValid() )  {  // Already connected to database ...
      return Success;
    }
    bool result = m_domH.existsDbase(fDesc.PFN());
    if ( result )  {
      return Success;
    }
    return CONNECTION_NOT_EXISTING;
  }
  return INVALID_SESSION_TOKEN;
}

/// Connect to a logical Database unit.
DbStatus DbStorageSvc::connect(const SessionH session,int mod,FileDescriptor& fDesc)  {
  DbStatus sc = Error;
  fDesc.setDbc(0);
  if ( m_domH.isValid() && session == SessionH(m_domH.ptr()) )   {
    DbDatabase dbH = m_domH.find(fDesc.FID());
    if ( dbH.isValid() )  {
      int all = pool::READ + pool::CREATE + pool::UPDATE;
      int wr  = pool::CREATE + pool::UPDATE;
      int m   = dbH.openMode();
      if ( (m&all) && mod == pool::READ )
        sc = Success;
      else if ( m&wr && mod&pool::CREATE )
        sc = Success;
      else if ( m&wr && mod&pool::UPDATE )
        sc = Success;
      else
        dbH.close();
    }
    // No Else!
    if ( !dbH.isValid() )  {
      sc = dbH.open(m_domH, fDesc.PFN(), fDesc.FID(), mod);
      if ( !sc.isSuccess() )    {
        DbPrint err( name());
        err << DbPrintLvl::Error 
            << "Cannot connect to Database: FID=" << fDesc.FID()
            << " PFN=" << fDesc.PFN() << DbPrint::endmsg;
        return sc;
      }
    }
    else {
      sc = Success;
    }
    DbConnection* dbc = new DbConnection(dbH.type().type(), dbH.name(), dbH.ptr());
    fDesc.setDbc(dbc);
  }
  return sc;
}

/// Reconnect to a logical Database unit with different access mode
DbStatus DbStorageSvc::reconnect(FileDescriptor& refDb, int mode)  {
  DbConnection* dbc = dynamic_cast<DbConnection*>(refDb.dbc());
  if ( dbc )   {
    DbDatabase dbH(DbDatabaseH(dbc->handle()));
    return dbH.reopen(mode);
  }
  return Error;
}

/// Disconnect from a logical Database unit.
DbStatus DbStorageSvc::disconnect(FileDescriptor& fDesc) {
  DbConnection* dbc = dynamic_cast<DbConnection*>(fDesc.dbc());
  if ( dbc )   {
    DbDatabase  dbH(DbDatabaseH(dbc->handle()));
    dbc->release();
    fDesc.setDbc(0);
    return dbH.close();
  }
  return Error;
}

/// Query the access mode of a Database unit.
DbStatus DbStorageSvc::openMode(FileDescriptor& refDB, int& mode) {
  DbConnection* dbc = dynamic_cast<DbConnection*>(refDB.dbc());
  if ( dbc )   {
    DbDatabase  dbH(DbDatabaseH(dbc->handle()));
    if ( dbH.isOpen() )  {
      mode = dbH.openMode();
      return Success;
    }
  }
  mode = pool::NOT_OPEN;
  return Error;
}


/// End/Finish an existing Transaction sequence.
DbStatus DbStorageSvc::endTransaction( ConnectionH connection, Transaction::Action typ)
{
   return ( (DbDatabaseObj*)connection->handle() )->transAct( typ );
}

/// Access options for a given database domain.
DbStatus DbStorageSvc::getDomainOption(const SessionH  sessionH, DbOption& opt)  {
  if ( m_domH.isValid() && sessionH == SessionH(m_domH.ptr()) )   {
    return m_domH.getOption(opt);
  }
  DbPrint err( name());
  err << DbPrintLvl::Error 
      << "Cannot connect to proper technology domain." << DbPrint::endmsg;
  return Error;
}

/// Set options for a given database domain.
DbStatus
DbStorageSvc::setDomainOption(const SessionH  sessionH, const DbOption& opt)  {
  if ( m_domH.isValid() && sessionH == SessionH(m_domH.ptr()) )   {
    return m_domH.setOption(opt);
  }
  DbPrint err( name());
  err << DbPrintLvl::Error 
      << "Cannot connect to proper technology domain." << DbPrint::endmsg;
  return Error;
}

} // namespace pool
