/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database container implementation
//--------------------------------------------------------------------
//
//        Package    : pool (The POOL project)
//
//        Author     : M.Frank
//====================================================================
// Framework include files
#include "POOLCore/DbPrint.h"

#include "StorageSvc/DbOption.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DataCallBack.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DbArray.h"
#include "StorageSvc/DbReflex.h"

// Local implementation files
#include "RootDatabase.h"
#include "RootCallEnv.h"
#include "RootKeyContainer.h"
#include "RootKeyIOHandler.h"

// Root include files
#include "TROOT.h"
#include "TFile.h"
#include "TClass.h"
#include "TKey.h"

#include "RootDataPtr.h"

using namespace pool;

RootKeyContainer::RootKeyContainer() : m_dbH(POOL_StorageType) {
  DbInstanceCount::increment(this);
  m_ioHandler  = new RootKeyIOHandler;
  m_policy     = TObject::kOverwrite;    // On update write new versions
  m_canDestroy = true;
  m_canUpdate  = true;
  m_ioBytes = -1;
  m_rootDb = 0;
  m_dir = 0;
}

/// Standard destructor
RootKeyContainer::~RootKeyContainer()   {
  DbInstanceCount::decrement(this);
  releasePtr(m_ioHandler);
  close();
}

long long int RootKeyContainer::nextRecordId()    {
  if ( m_dir )  {
    TList* list = m_dir->GetListOfKeys();
    if ( list ) {
      int s1 = 0;
      TKey* k = (TKey*)list->Last();
      if ( k ) {
        ::sscanf(k->GetName(), "_pool_valid_%08d", &s1);
        ++s1;
      }
      long long int s2 = DbContainerImp::size(); // Number of objects on commit stack
      return s1+s2;
    }
  }
  return -1;
}

long long int RootKeyContainer::size()    {
  if ( m_dir )  {
    TList* list = m_dir->GetListOfKeys();
    if ( list ) {
      // Number of committed objects
      long long int s1 = list->GetSize();
      // Number of objects on commit write stack
      long long int s2 = DbContainerImp::size();
      return s1+s2;
    }
  }
  return -1;
}

/// Execute transaction action
DbStatus RootKeyContainer::transAct(Transaction::Action action) 
{
   // execure action on the base class first
   DbStatus status = DbContainerImp::transAct(action);
   if( !status.isSuccess() ) return status;

   if( action != Transaction::TRANSACT_FLUSH ) return Success;
   if( !m_dir ) return Error;
   m_dir->SaveSelf();
   return Success;
}
   

// Interface Implementation: Find entry in container
DbStatus RootKeyContainer::fetch(const Token::OID_t& linkH, Token::OID_t& stmt) {
  char txt[64];
  ::sprintf(txt, "_pool_valid_%08d", int(linkH.second));
  TKey* key = (TKey*)m_dir->GetListOfKeys()->FindObject(txt);
  if ( key )    {
    stmt = linkH;
    return Success;
  }
  return Error;
}

// Fetch next object address of the selection to set token
DbStatus RootKeyContainer::fetch(DbSelect& sel)   {
  if ( sel.criteria().length() == 0 || sel.criteria() == "*" )  {
    char txt[64];
    Token::OID_t lnk = sel.link();
    const long long int stk_size = DbContainerImp::size();
    const long long int cnt_size = nextRecordId()-stk_size;
    for(int j=lnk.second; j < cnt_size; ++j) {
      ++lnk.second;
      ::sprintf(txt, "_pool_valid_%08d", int(lnk.second));
      const TKey* key = (TKey*)m_dir->GetListOfKeys()->FindObject(txt);
      if ( key )    {
        const char* class_name = key->GetClassName();
        const DbTypeInfo* typ = m_dbH.objectShape( DbReflex::forTypeName(class_name) );
        if ( typ )  {
            sel.setShapeID(typ->shapeID());
            sel.link() = lnk;
            return Success;
        }
        DbPrint err(m_name);
        err << DbPrintLvl::Error 
            << "Failed to find the correct shape identifier for class:"
            << class_name << DbPrint::endmsg;
        return Error;
      }
      else {
        // Here we are if key names have holes due to deletes
        // Try to get the next one.
      }
    }
    // The object was not yet saved and is still on the
    // commit stack.
    lnk = sel.link();
    for(long long int i=0; i < stk_size; ++i)  {
      ActionList::value_type* ent = stackEntry(size_t(i));
      bool take_it = ent->link.second > lnk.second;
      if ( ent->action == WRITE && take_it )  {
        ShapeH shape = ent->shape;
        if ( shape )  {
          sel.setShapeID(shape->shapeID());
          sel.link() = ent->link;
          return Success;
        }
      }
    }
    return Error;
  }
  DbPrint log( m_name);
  log << DbPrintLvl::Error << "The chosen implementation does not allow to "
      << "refine container scans." << DbPrint::endmsg
      << "The only valid selection criterium is: \"\" (empty string)"
      << DbPrint::endmsg;
  return Error;
} 

// Interface Implementation: Find entry in container
DbStatus RootKeyContainer::load( DataCallBack* call,
                                 const Token::OID_t& linkH,
                                 Token::OID_t& oid,
                                 DbAccessMode  mode,
                                 bool          any_next)
{
  DbStatus sc = Error;
  oid.second = linkH.second;
  for(long long int cnt = oid.second,last=nextRecordId(); cnt <= last; ++cnt) {
    char txt[64];
    ::sprintf(txt, "_pool_valid_%08d", int(oid.second));
    const TKey* key = (TKey*)m_dir->GetListOfKeys()->FindObject(txt);
    if ( key )    {
      sc = loadObject(call, oid, mode);
      if ( sc.isSuccess() )  {
        return sc;
      }
      return sc;
    }
    if ( !any_next )  {
      return Error;
    }
    oid.second++;
  }
  if ( linkH.second < 0 || linkH.second <= size() ) {
    DbPrint log( m_name );
    log << DbPrintLvl::Debug << "No objects passing selection criteria..." 
        << " Container has " << size() << " Entries in total." << DbPrint::endmsg;
  }
  return sc;
}

/// Destroy persistent object in the container; does not touch transient!
DbStatus RootKeyContainer::destroyObject(ActionList::value_type& entry) {
  char txt[64];
  const Token::OID_t& lnkH = entry.link;
  // Does not work, because container size is changed...
  ::sprintf(txt, "_pool_valid_%08d", int(lnkH.second));
  TDirectory::TContext dirCtxt(m_dir);
  TKey* key = (TKey*)m_dir->GetListOfKeys()->FindObject(txt);
  if ( key )    {
    if ( m_policy == 0 || m_policy == TObject::kSingleKey )  {
      strcat(txt,";*");
    }
    if ( 0 != m_ioHandler->destroy(txt) ) {
      return Success;
    }
  }
  DbPrint err( m_name);
  err << DbPrintLvl::Error 
      << "Could not delete object \"" << txt << "\" from directory \"" << m_dir->GetName()
      << "\"" << DbPrint::endmsg;
  return Error;
}

DbStatus RootKeyContainer::loadObject( DataCallBack*   call,
                                       Token::OID_t&   oid,
                                       DbAccessMode /* mode */ )
{
   char txt[64];
   ::sprintf(txt, "_pool_valid_%08d", int(oid.second));
   TDirectory::TContext dirCtxt(m_dir->GetFile());
   TKey* key = (TKey*)m_dir->GetListOfKeys()->FindObject(txt);
   if ( key && call )    {
      const char* class_name = key->GetClassName();
      const DbTypeInfo* typ = (const DbTypeInfo*)call->shape();
      if( typ )  {
         TClass* cl = TClass::GetClass(class_name);
         if( 0 != cl ) {
            if ( typ->columns().size() == 1) {
               RootDataPtr user(0), context(cl->New());
               DbStatus status = call->start(DataCallBack::GET, context.ptr, &user.ptr);
               if ( status.isSuccess() )  {
                  RootDataPtr p(context.ptr); 
                  const DbColumn* col = *(typ->columns().begin());
                  status = call->bind(DataCallBack::GET,col,0,context.ptr,&p.ptr);
                  *p.pptr = context.ptr;
                  if ( status.isSuccess() )  {
                     int nbyte = m_ioHandler->read(key, p.pptr);
                     // *p.pptr = key->ReadObjectAny(cl);
                     // int nbyte = key->GetObjlen() + key->GetKeylen();
                     if ( nbyte > 1 ) {
                        /// Update statistics
                        m_ioBytes = nbyte;
                        m_rootDb->addByteCount(RootDatabase::READ_COUNTER, nbyte);
                        /// Terminate callback
                           return call->end(DataCallBack::GET, context.ptr);
                     }
                  }
               }
            }
            else  {
               RootDataPtr context(0);
               RootCallEnv env(call, context);
               // void* ctxt = key->ReadObjectAny(cl);
               // int nbyte = key->GetObjlen() + key->GetKeylen();
               void* ctxt = &env;
               int nbyte = m_ioHandler->read(key, &ctxt);
               if ( nbyte > 0 )  {
                  m_ioBytes = nbyte;
                  m_rootDb->addByteCount(RootDatabase::READ_COUNTER, nbyte);
                  return Success;
               }
            }
         }
      }
   }
  m_ioBytes = -1;
  DbPrint err( m_name);
  err << DbPrintLvl::Error << "Could not read object \"" << txt 
      << "\" from directory \"" << m_dir->GetName() << "\"" << DbPrint::endmsg;
  return Error;
}

DbStatus RootKeyContainer::writeObject(ActionList::value_type& action)   {
   DbStatus status = Error;
   if ( m_dir )  {
      char knam[64];
      ::sprintf(knam, "_pool_valid_%08d", int(action.link.second));
      auto typ = static_cast<const DbTypeInfo*>(action.shape);
      if ( 0 == typ )   {
         DbPrint log(  m_name);
         log << DbPrintLvl::Error << "No type information present when writing an object!"
             << DbPrint::endmsg;
         return Error;
      }
      else {
         TDirectory::TContext dirCtxt(m_dir);
         if( typ->columns().size() == 1 ) {
            const DbColumn* col = *(typ->columns().begin());
            const std::string& typ_nam = col->typeName();
            TClass*  cl  = TClass::GetClass(typ_nam.c_str());
            if( !cl ) {
               DbPrint log(  m_name);
               log << DbPrintLvl::Error << "No handler for " << typ_nam
                   << DbPrint::endmsg;
               return Error;
            }
            const void* p = action.dataAtOffset( col->offset() );
            int nbyte = m_ioHandler->write(cl, knam, p, m_policy);
            if ( nbyte > 1) {
               m_ioBytes = nbyte;
               m_rootDb->addByteCount(RootDatabase::WRITE_COUNTER, nbyte);
               return Success;
            }
         }
         DbPrint err(m_name);
         err << DbPrintLvl::Error 
             << "[RootKeyContainer] Could not write an object" << DbPrint::endmsg;
      }
   }
   else {
      DbPrint err(m_name);
      err << DbPrintLvl::Error << "[RootKeyContainer] " 
          << "Not a valid directory or callback when writing an object" 
          << DbPrint::endmsg;
   }
   m_ioBytes = -1;
   return Error;
}

DbStatus RootKeyContainer::close()   {
  m_dbH = DbDatabase(POOL_StorageType);
  m_rootDb = 0;
  m_dir = 0;
  return DbContainerImp::close();
}

DbStatus RootKeyContainer::open(const DbDatabase&     dbH, 
                                const std::string&    dir_nam, 
                                const DbTypeInfo*  /* info */, 
                                DbAccessMode          mode)  
{
  DbPrint log( dir_nam );
  m_name = dir_nam;
  if ( dbH.isValid() && dir_nam.length() > 0 )    {
    std::string nam = (dir_nam[0]=='/') ? dir_nam.substr(1) : dir_nam;
    size_t idx1     = std::string::npos, idx2 = nam.find('/',1);
    TDirectory::TContext dirCtxt(0);
    IDbDatabase* idb = dbH.info();
    m_rootDb = dynamic_cast<RootDatabase*>(idb);
    if (!m_rootDb) {
      m_dir = 0;
      return Error;
    }
    m_dir  = m_rootDb->file();
    do  {
      std::string s = nam.substr(idx1+1, idx2-idx1-1); 
      m_dir->cd();
      TDirectory* dir = (TDirectory*)m_dir->Get(s.c_str());
      if ( 0==dir && mode&pool::CREATE )    {
        dir = m_dir->mkdir(s.c_str());
      }
      else if ( 0==dir ) {
        m_dir = 0;
        return Error;
      }
      m_dir = dir;
      if ( m_dir )    {
        TClass* cl = m_dir->IsA();
        if ( !cl->InheritsFrom(TDirectory::Class()) )    {
          log << DbPrintLvl::Error << "Cannot open container. "
              << "Object with name found, but of the wrong type." << DbPrint::endmsg
              << "True type is :" << cl->GetName() << " rather than TDirectory."
              << DbPrint::endmsg;
          return Error;
        }
        idx1  = idx2;
        idx2  = nam.find('/', idx1+1);
      }
    } while ( m_dir && idx1 != std::string::npos );
    if (m_dir)
      m_dir->cd();
    DbOption opt1("DEFAULT_WRITEPOLICY","");
    dbH.getOption(opt1);
    opt1._getValue(m_policy);
    /// Parent Database handle
    m_dbH = dbH;
    log << DbPrintLvl::Debug << "Opened container " << m_name << " of type "
        << ROOTKEY_StorageType.storageName() << " with policy:" << m_policy
        << DbPrint::endmsg;
    return Success;
  }
  log << DbPrintLvl::Error << "Cannot open container, invalid Database handle." 
      << DbPrint::endmsg;
  return Error;
}

// Define selection criteria
DbStatus RootKeyContainer::select(DbSelect& /* crit */) {
  if ( 0 != m_dir )    {
    return Success;
  }
  return Error;
}

/// Access options
DbStatus RootKeyContainer::getOption(DbOption& opt)  const  { 
  if ( m_dir )  {
    const char* n = opt.name().c_str();
    if ( !strcasecmp(n,"BYTES_IO") )  {
      return opt._setValue((int)m_ioBytes);
    }
    else if ( !strcasecmp(n,"DIRECTORY") )  {
      return opt._setValue((void*)m_dir);
    }
    else if ( !strcasecmp(n, "DEFAULT_WRITEPOLICY") ) {
      return opt._setValue(int(m_policy));
    }
    else if ( ::toupper(n[0])=='D' && opt.name().length() > 4 ) {
      switch(::toupper(n[4]))  {
      case 'B':
        if ( !strncasecmp(n+4,"BYTES",5) )
          return opt._setValue((int)m_ioBytes);
        break;
      case 'F':
        if ( !strcasecmp(n+4,"FILE") )  {
          return opt._setValue((void*)m_dir->GetFile());
        }
        break;
      case 'G':
        if ( !strcasecmp(n+4,"GETOBJ") )  {
          return opt._setValue((void*)m_dir->Get(opt.option().c_str()));
        }
        break;
      case 'L':
        if ( !strcasecmp(n+4,"LIST_KEYS") )  {
          return opt._setValue((void*)m_dir->GetListOfKeys());
        }
        else if ( !strcasecmp(n+4,"LS") )  {
          m_dir->ls();
          return opt._setValue(int(1));
        }
        break;
      case 'N':
        if ( !strcasecmp(n+4,"NKEYS") )  {
          return opt._setValue(int(m_dir->GetNkeys()));
        }
        else if ( !strcasecmp(n+4,"NBYTESKEYS") )  {
          return opt._setValue(int(m_dir->GetNbytesKeys()));
        }
        break;
      case 'M':
        if ( !strcasecmp(n+4,"MOTHER") )  {
          return opt._setValue((void*)m_dir->GetMother());
        }
        else if ( !strcasecmp(n+4,"MODIFIED") )  {
          return opt._setValue((int)m_dir->IsModified() ? 1 : 0);
        }
        break;
      case 'P':
        if ( !strcasecmp(n+4,"PRINT") )  {
          m_dir->Print(opt.option().c_str());
          std::cout << std::endl;
          return opt._setValue(1);
        }
        break;
      case 'W':
        if ( !strcasecmp(n+4,"WRITABLE") )  {
          return opt._setValue((int)m_dir->IsWritable() ? 1 : 0);
        }
        break;
      }
    }
  }
  return Error;  
}


/// Set options
DbStatus RootKeyContainer::setOption(const DbOption& opt)  { 
  if ( m_dir )  {
    const char* n = opt.name().c_str();
    if ( !strcasecmp(n, "DEFAULT_WRITEPOLICY") ) {
      opt._getValue(m_policy);
      return Success;
    }
    else if ( ::toupper(n[0]) == 'D' )  {
      switch(::toupper(n[4]))   {
      case 'C':
        if ( !strcasecmp(n+4,"CLOSE") )  {
          m_dir->Close(opt.option().c_str());
          return Success;
        }
        break;
      case 'D':
        if ( !strcasecmp(n+4,"DELETEOBJ") )  {
          m_dir->Delete(opt.option().c_str());
          return Success;
        }
        break;
      case 'P':
        if ( !strcasecmp(n+4,"PRINT") )  {
          m_dir->Print(opt.option().c_str());
          std::cout << std::endl;
          return Success;
        }
        else if ( !strcasecmp(n+4,"PURGE") )  {
          int val=1;
          opt._getValue(val);
          if ( val > 0 )  {
            m_dir->Purge(val);
            return Success;
          }
        }
        break;
      }
    }
  }
  return Error;  
}
