/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//    Root Database container implementation
//--------------------------------------------------------------------
//
//    Package    : pool (The POOL project)
//
//    Author     : M.Frank
//====================================================================

// Framework include files
#include "StorageSvc/DbObject.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/DbSelect.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbColumn.h"
#include "StorageSvc/DbTypeInfo.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/DataCallBack.h"
#include "StorageSvc/DbArray.h"
#include "POOLCore/DbPrint.h"
#include "StorageSvc/Transaction.h"
#include "StorageSvc/DbReflex.h"

// Local implementation files
#include "RootTreeContainer.h"
#include "RootDataPtr.h"
#include "RootDatabase.h"

// Root include files
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TLeaf.h"
#include "TBranch.h"
#include "RootUtils/TBranchElementClang.h"
#include "TTreeFormula.h"

#include "AthContainersInterfaces/IAuxStoreIO.h"
#include "AthContainersInterfaces/IAuxTypeVector.h"
#include "AthContainersInterfaces/IAuxStoreHolder.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"
#include "RootAuxDynIO/RootAuxDynIO.h"

using namespace pool;
using namespace std;


namespace {


class TBranchAccess : public TBranch
{
public:
  static int doNotUseBufferMap() { return kDoNotUseBufferMap; }
};


/**
 * @brief Fix up branch status bits after possible schema evolution
 *        from std::vector to SG::PackedContainer.
 * @param br The branch to process.
 *
 * Some background is needed here.
 *
 * ROOT can keep a map associated with a buffer giving the location of objects
 * in the root file.  This is used to handle C pointer resolution on I/O.
 * The use of this is controlled by two flags in fBits.
 * If kDoNotUseBufferMap is _clear_, then the map get cleared on each
 * event (when reading); otherwise, it is not.  This bit is set by default
 * when a branch is created.  When Fill() is called, then it is cleared
 * if the map is empty.  The bit is then saved and restored along
 * with the TBranch.
 *
 * But a branch also has the kBranchAny bit.  If this is set, then ROOT
 * will actually try to make entries in the map.  This is set when
 * a branch is initialized and also when SetAddress is called.  The bit
 * is set if the class _might_ contain pointers.  Specifically, the bit
 * is clear for branches with fundamental types, set::string, and std::vector
 * of any of these.  It is turned on for anything else, in particular
 * for any user types.
 *
 * This logic causes a problem when we schema evolve from std::vector
 * to SG::PackedContainer.  When the branch is written, the map is not
 * used.  So kDoNotUseBufferMap is set, and kBranchAny is clear.
 * The branch is saved, and read back in with the same flag settings.
 * But then when we go to do the SetAddress, ROOT sees that the type
 * we have is SG::PackedContainer --- a user type.  So kBranchAny
 * is turned on.  This means that the map gets filled when we read.
 * But because kDoNotUseBufferMap is set, the map is not cleared
 * when we go to a new event.  This results in errors from TExMap
 * about duplicate keys.
 *
 * The easiest thing to do to resolve this seems to be to turn off
 * the kDoNotUseBufferMap so that the map will in fact get cleared.
 * (The arguably more consistent scheme of turning off kBranchAny
 * is not preferred since we'd have to do that every time after
 * SetAddress is called.)  Ideally, we'd like to be able to tell
 * ROOT that a given class does not use pointers, but there's no way
 * to do that at the moment.
 */
void fixupPackedConversion (TBranch* br)
{
  TIter next( br->GetListOfBranches() );
  while ( TBranch* b = (TBranch*)next() ) {
    fixupPackedConversion (b);
  }

  TBranchElement* belt = dynamic_cast<TBranchElement*> (br);
  if (belt) {
    TClass* cl = belt->GetCurrentClass();
    if (cl && strncmp (cl->GetName(), "SG::PackedContainer<", 20) == 0) {
      br->ResetBit (TBranchAccess::doNotUseBufferMap());
    }
  }
}


} // anonymous namespace

static UCharDbArrayAthena  s_char_Blob;
static IntDbArray   s_int_Blob;

//#include <iostream>
//using namespace std;


RootTreeContainer::RootTreeContainer()
: m_tree(nullptr), m_type(0), m_dbH(POOL_StorageType), 
  m_rootDb(nullptr), m_branchName(), m_ioBytes(0), m_treeFillMode(false),
  m_isDirty(false)
{
  DbInstanceCount::increment(this);
}

/// Standard destructor
RootTreeContainer::~RootTreeContainer()   {
  DbInstanceCount::decrement(this);
  close();
}

/// Ask if a given shape is supported
DbStatus RootTreeContainer::isShapeSupported(const DbTypeInfo* typ) const  {
  return typ == m_type;
}

long long int RootTreeContainer::size()    {
  if (m_tree == nullptr) return 0;
  long long int s = DbContainerImp::size();
  if( isBranchContainer() ) {
     TBranch * pBranch = m_tree->GetBranch(m_branchName.c_str());
     if (pBranch == nullptr) return 0;
     s += pBranch->GetEntries();
  } else {     
     s += m_tree->GetEntries();
  }
  return s;
}

/// Access branch by name
TBranch* RootTreeContainer::branch(const std::string& nam)  const  {
  Branches::const_iterator k;
  for(k=m_branches.begin(); k !=m_branches.end(); ++k) {
    if ( (*k).branch->GetName() == nam )  {
      return (*k).branch;
    }
  }
  return nullptr;
}


DbStatus RootTreeContainer::writeObject( ActionList::value_type& action )
{
   //clear aux branches write marker
   for( auto &descMapElem: m_auxBranchMap ) {
      descMapElem.second.written = false;
   }      
   int icol;
   int num_bytes = 0;
   Branches::iterator k;
   for(k=m_branches.begin(), icol=0; k !=m_branches.end(); ++k, ++icol) {
      BranchDesc& dsc = (*k);
      RootDataPtr p( nullptr );
      p.ptr = const_cast<void*>( action.dataAtOffset( dsc.column->offset() ) );
      switch( dsc.column->typeID() ) {
       case DbColumn::ANY:
       case DbColumn::POINTER:
          dsc.object            = p.ptr;
          p.ptr                 = &dsc.object;
          if( dsc.aux_iostore_IFoffset >= 0 ) {
             // cout << "---  write object IAuxStoreIO "<<endl;
             // DbPrint::setMsgVerbosity(coral::Debug);
             DbPrint log(m_name);
             log << DbPrintLvl::Debug << " SG::IAuxStoreIO* detected in " << dsc.branch->GetName() << DbPrint::endmsg;
             auto *store = reinterpret_cast<SG::IAuxStoreIO*>( (char*)dsc.object + dsc.aux_iostore_IFoffset );
             // cout << "---    store object= " <<hex << store <<dec << " in " << dsc.branch->GetName()  <<endl;
             // cout << "       obj=" << hex << dsc.object << dec << "  offset=" <<  dsc.aux_iostore_IFoffset << endl;
             log << DbPrintLvl::Debug << "       Attributes= " << store->getSelectedAuxIDs().size() << DbPrint::endmsg;
             for(SG::auxid_t id : store->getSelectedAuxIDs()) {
                BranchDesc&       newBrDsc( m_auxBranchMap[id] );
                if( !newBrDsc.branch ) {
                   auto &reg = SG::AuxTypeRegistry::instance();
                   // we have a new attribute, create a branch for it
                   log << "   Creating branch for new dynamic attribute, Id=" << id << ": type="
                       << SG::normalizedTypeinfoName( *(store->getIOType(id)) )
                       << ", " << reg.getName(id) << DbPrint::endmsg;
                   if( ! addAuxBranch(reg.getName(id), store->getIOType(id), newBrDsc) .isSuccess() )  {
                      p.ptr = nullptr;  // trigger an Error
                      break;
                   }
                   if( dsc.rows_written ) {
                      // catch up with the rows written by other branches
                      newBrDsc.object = nullptr;
                      newBrDsc.branch->SetAddress( nullptr );
                      for( size_t r=0; r<dsc.rows_written; ++r ) {
                         num_bytes += newBrDsc.branch->BackFill();
                      }
                   }
                }
                newBrDsc.object = (void*)store->getIOData(id);
                newBrDsc.branch->SetAddress( newBrDsc.objectAddr() );
                newBrDsc.written = true;  // marking that branch address was set, even if Fill is delayed
                if( isBranchContainer() && !m_treeFillMode ) {
                   size_t bytes_out = newBrDsc.branch->Fill();
                   num_bytes += bytes_out;
                }
             }
             store->selectAux( std::set<std::string>() );
          }
          dsc.rows_written++;
          break;
       case DbColumn::BLOB:
          s_char_Blob.m_size    = p.blobSize();
          s_char_Blob.m_buffer  = (unsigned char*)p.blobData();
          dsc.object            = &s_char_Blob;
          p.ptr                 = &dsc.object;
          break;
       case DbColumn::STRING:
       case DbColumn::LONG_STRING:
          p.cptr                = p.string();
          break;
       case DbColumn::NTCHAR:
       case DbColumn::LONG_NTCHAR:
          //case DbColumn::TOKEN: PvG not sure wether we should pass *char[]
          p.ptr                 = p.deref();
          break;
       default:
          break;
      }
      if ( nullptr == p.ptr )   {
         DbPrint err( m_name);
         err << DbPrintLvl::Error 
             << "[RootTreeContainer] Could not write an object" 
             << DbPrint::endmsg;
         return Error;
      }
      //if (p.ptr != dsc.branch->GetAddress()) {
      dsc.branch->SetAddress(p.ptr);
      //}
      if( isBranchContainer() && !m_treeFillMode ) {
         num_bytes += dsc.branch->Fill();
      }
   }

   // check if some AUX branches were not set
   for( auto &descMapElem: m_auxBranchMap ) {
      BranchDesc& dsc = descMapElem.second;
      if( !dsc.written ) {
         dsc.object = nullptr;
         dsc.branch->SetAddress( nullptr );
         // cout << "   Branch " <<  SG::AuxTypeRegistry::instance().getName(descMapElem.first) << " filled out with NULL" << endl;
         if( isBranchContainer() && !m_treeFillMode ) {
            size_t bytes_out = dsc.branch->Fill();
            num_bytes += bytes_out;
         }
      }
   }
      
   if( !isBranchContainer() ) {
      // Single Container per TTree - just Fill it now
      num_bytes = m_tree->Fill();
   } else if( m_treeFillMode ) {
      // Multiple containers per TTree - mark TTree for later Fill at commit
      // cout << "----- " << m_name << " : TTree=" << m_tree->GetName() << " : marking DIRTY " << endl;       
      if( m_isDirty ) {
         DbPrint log(m_name);
         log << DbPrintLvl::Error << "Attempt to write to a Branch Container twice in the same transaction! "
             << "This conflicts with TTree AUTO_FLUSH option. "
             << DbPrint::endmsg;
         m_ioBytes = -1;
         return Error;
      }
      m_isDirty = true;
      m_ioBytes = 0;  // no information per container available!
      return Success;
   }
   // else (branch container NOT in tree fill mode)
   // do nothing, the branch was filled in the previous block already
    
   for(k=m_branches.begin(); k !=m_branches.end(); ++k) {
      switch ( (*k).column->typeID() )    {
       case DbColumn::BLOB:
          s_char_Blob.release(false);
          break;
       default:
          break;
      }
   }
   if ( num_bytes > 0 )  {
      m_ioBytes = num_bytes;
      m_rootDb->addByteCount(RootDatabase::WRITE_COUNTER, num_bytes);
      return Success;
   }
   DbPrint err( m_name);
   err << DbPrintLvl::Error 
       << "[RootTreeContainer] Could not write an object" 
       << DbPrint::endmsg;
   m_ioBytes = -1;
   return Error;
}

/// Fetch refined object address. Default implementation returns identity
DbStatus RootTreeContainer::fetch(const Token::OID_t& linkH, Token::OID_t& stmt)  {
  return DbContainerImp::fetch(linkH, stmt);
}

// Fetch next object address of the selection to set token
DbStatus RootTreeContainer::fetch(DbSelect& sel)  {
  if ( sel.criteria().length() == 0 || sel.criteria() == "*" )  {
    sel.link().second++;
    return DbContainerImp::fetch(sel.link(), sel.link());
  }
  DbSelect::Ptr<TTreeFormula>* stmt = 
    dynamic_cast<DbSelect::Ptr<TTreeFormula>* >(sel.statement());
  if ( stmt ) {
    TTreeFormula* selStmt = stmt->m_ptr;
    if ( selStmt )  {
      std::lock_guard<std::mutex>   lock( m_rootDb->ioMutex() );
      Branches::iterator k;
      long long cur  = sel.link().second;
      Long64_t last = m_tree->GetEntries();
      for(k=m_branches.begin(); k != m_branches.end(); ++k)  {
        BranchDesc& dsc = (*k);
        int typ = dsc.column->typeID();
        switch ( typ )    {
        case DbColumn::POINTER:
        case DbColumn::BLOB:
          dsc.branch->SetAddress(&dsc.buffer);
        default:
          break;
        }
      }
      selStmt->SetTree(m_tree);
      // loop on all selected entries
      for( ++cur; cur < last; ++cur) {
        m_tree->LoadTree(cur);
        selStmt->GetNdata();
        if ( selStmt->EvalInstance(0) != 0 ) {
          sel.link().second = cur;
          return Success;
        }
      }
    }
  }
  return Error;
}

#include <iostream>
using namespace std;

DbStatus 
RootTreeContainer::loadObject(DataCallBack* call, Token::OID_t& oid)
{
  long long evt_id = oid.second;
  void *object = const_cast<void*>(call->object());
  // lock access to this DB for MT safety
  std::lock_guard<std::mutex>     lock( m_rootDb->ioMutex() );
  try {
     int numBytesBranch, numBytes = 0;
     bool hasRead(false);
     for( auto& dsc : m_branches ) {
        RootDataPtr p(nullptr), q(nullptr);
        p.ptr = object;
        
        int typ = dsc.column->typeID();
        // cout << "LOAD: object=" << object << " p.ptr=" << p.ptr << " col offset=" << dsc.column->offset() << endl;
        // associate branch with an object
        switch ( typ )    {
         case DbColumn::STRING:
         case DbColumn::LONG_STRING:
         case DbColumn::NTCHAR:
         case DbColumn::LONG_NTCHAR:
         case DbColumn::TOKEN:
            // set data pointer to the object data member for this branch
            p.c_str += dsc.column->offset(); 
            break;
         case DbColumn::BLOB:
            dsc.object = &s_char_Blob;
            p.ptr      = &dsc.object;
            dsc.branch->SetAddress(p.ptr);
            break;
         case DbColumn::ANY:
         case DbColumn::POINTER:
         default:
            dsc.branch->SetAddress( &p.ptr );
            break;
        }
        // Must move tree entry to correct value
        if (m_tree) {
           if (m_tree->GetReadEntry() < evt_id) m_tree->LoadTree(evt_id);
        } else if (dsc.branch->GetTree()->GetReadEntry() < evt_id) {
           dsc.branch->GetTree()->LoadTree(evt_id);
        }
        // read the object
        numBytesBranch = dsc.branch->GetEntry(evt_id);
        TTree::TClusterIterator clusterIterator = dsc.branch->GetTree()->GetClusterIterator(evt_id);
        clusterIterator.Next();
        if (evt_id == clusterIterator.GetStartEntry() && dsc.branch->GetTree()->GetMaxVirtualSize() != 0) {
           for (int i = dsc.branch->GetReadBasket(); i < dsc.branch->GetMaxBaskets()
                   && dsc.branch->GetBasketEntry()[i] < clusterIterator.GetNextEntry(); i++) {
              dsc.branch->GetBasket(i);
           }
        }
        numBytes += numBytesBranch;
        if ( numBytesBranch >= 0 )     {
           hasRead=true;
           switch ( typ )    {
            case DbColumn::STRING:
            case DbColumn::LONG_STRING:
               // assign to std::string
               *p.str = (char*) dsc.leaf->GetValuePointer();
               break;
            case DbColumn::NTCHAR:
            case DbColumn::LONG_NTCHAR:
            case DbColumn::TOKEN:
               q.ptr = dsc.leaf->GetValuePointer();
               ::strcpy(q.c_str, p.c_str);
               break;
            case DbColumn::BLOB:
               q.blob->adopt((char*)s_char_Blob.m_buffer,s_char_Blob.m_size);
               s_char_Blob.release(false);
               break;
            case DbColumn::POINTER:
               call->setObject(p.ptr);
               // for AUX store objects with the right interface supply a special store object
               // that will read branches on demand
               if( dsc.aux_reader ) {
                  // cout << " ***  AuxDyn reader detected in " << dsc.clazz->GetName() << ", entry# " << evt_id << endl;
                  dsc.aux_reader->addReaderToObject(p.ptr, evt_id, &m_rootDb->ioMutex());
               }
               break;
           }
        } else {
           DbPrint log(m_name);
           log << DbPrintLvl::Error << "Cannot load branch " << dsc.branch->GetName()
               << " for entry No." << evt_id << DbPrint::endmsg;
           m_ioBytes = -1;
           return Error;
        }
     }
     if ( hasRead )   {
        /// Update statistics
        m_ioBytes = numBytes;
        m_rootDb->addByteCount(RootDatabase::READ_COUNTER, numBytes);
        return Success;
     }
  }
  catch( const std::exception& e )    {
     DbPrint err(m_name);
     err << DbPrintLvl::Fatal << "[RootTreeContainer] "
         << "STL C++ Exception: " << e.what() << DbPrint::endmsg;
  }
  catch (...)   {
     DbPrint err(m_name);
     err << DbPrintLvl::Fatal << "[RootTreeContainer] "
         << "Unknown exception occurred. Cannot give more details." 
         << DbPrint::endmsg;
  }
  DbPrint log(m_name);
  log << DbPrintLvl::Info << "Cannot load entry No." << evt_id << "..." 
      << (m_branchName.empty() ? " Tree has " : " Branch has " ) 
      << size() << " Entries in total." << DbPrint::endmsg;
  m_ioBytes = -1;
  return Error;
}



DbStatus RootTreeContainer::close()   {
  m_dbH = DbDatabase(POOL_StorageType);
  if ( m_tree )   {
    //m_tree->Print();
  }
  for(Branches::iterator k=m_branches.begin(); k != m_branches.end(); ++k)  {
    BranchDesc& dsc = (*k);
    if ( dsc.buffer && dsc.clazz )  {
      // This somehow fails for templates.
      dsc.clazz->Destructor(dsc.buffer);
    }
  }
  for( auto &descMapElem: m_auxBranchMap ) {
     BranchDesc& dsc = descMapElem.second;
     if ( dsc.buffer && dsc.clazz )  {
      // This somehow fails for templates.
      dsc.clazz->Destructor(dsc.buffer);
    }
  }      
  m_branches.clear();
  m_auxBranchMap.clear();
  m_rootDb = nullptr;
  m_tree = nullptr;
  return DbContainerImp::close();
}


DbStatus RootTreeContainer::open( const DbDatabase& dbH, 
                                  const std::string& nam, 
                                  const DbTypeInfo* info, 
                                  DbAccessMode mode)  
{
   DbPrint log(nam);
   m_branches.clear();
   m_name = nam;
   log << DbPrintLvl::Debug << "Opening"  << DbPrint::endmsg;
   if ( dbH.isValid() && info )    {
      const DbTypeInfo::Columns& cols = info->columns();
      DbTypeInfo::Columns::const_iterator i;
      std::string treeName(nam);
      for(std::string::iterator j = treeName.begin(); j != treeName.end(); ++j )    {
         if ( *j == '/' ) *j = '_';
      }
      log << DbPrintLvl::Debug << "   attributes# = " << cols.size() << DbPrint::endmsg;
      if (cols.size() == 1) {
         // extract tree and branch name for branch containers, notation: "tree(branch)"
         std::string::size_type inx = nam.find('(');
         if (inx != std::string::npos) {
            std::string::size_type inx2 = nam.find(')');
            if (inx2 == std::string::npos || inx2 != nam.size()-1) {
               log << DbPrintLvl::Error << "Misspecified branch name in " << m_name << "."
                   << DbPrint::endmsg;
               return Error;
            }
            m_branchName = treeName.substr(inx+1, inx2-inx-1);
            treeName = treeName.substr(0, inx);
            log << DbPrintLvl::Debug << "Branch container '" << m_branchName << "'" << DbPrint::endmsg;
         }
      }
      IDbDatabase* idb = dbH.info();
      m_rootDb = dynamic_cast<RootDatabase*>(idb);
      TDirectory::TContext dirCtxt(m_rootDb ? m_rootDb->file() : gDirectory);
      if (m_rootDb)
         m_tree = (TTree*)m_rootDb->file()->Get(treeName.c_str());

      bool hasBeenCreated = (m_branchName.empty() 
                             ? m_tree != nullptr 
                             : (m_tree && m_tree->GetBranch(m_branchName.c_str()) != nullptr));
      if ( hasBeenCreated && (mode&pool::READ || mode&pool::UPDATE) )   {
         int count;
         if ( !m_tree->InheritsFrom(TTree::Class()) )   {
            log << DbPrintLvl::Error << "Cannot open the container " << m_name << " of type "
                << ROOTTREE_StorageType.storageName() << "." << DbPrint::endmsg
                << "The specified container is not a ROOT " << (m_branchName.empty() ? "Tree" : "Branch")
                << ", but rather of class " << m_tree->IsA()->GetName() << "."
                << DbPrint::endmsg;
            return Error;
         }
         m_branches.resize(cols.size());
         for(i = cols.begin(), count = 0; i != cols.end(); ++i, ++count )   {
            std::string colnam  = (m_branchName.empty() ? (*i)->name() : m_branchName);
            TBranch* pBranch = m_tree->GetBranch(colnam.c_str());
            if( !pBranch && m_branchName.empty() ) {
               for( char& c : colnam )
                  if( !::isalnum(c) ) c = '_';
               pBranch = m_tree->GetBranch(colnam.c_str());
            }
            if ( pBranch )    {
               fixupPackedConversion (pBranch);

               const DbColumn* c = *i;
               BranchDesc& dsc = m_branches[count];
               TClass* cl = nullptr;
               TLeaf* leaf = pBranch->GetLeaf(colnam.c_str());
               switch ( (*i)->typeID() )    {
                case DbColumn::ANY:
                case DbColumn::BLOB:
                case DbColumn::POINTER:
                   cl = TClass::GetClass(pBranch->GetClassName());
                   if ( nullptr == cl )  {
                      log << DbPrintLvl::Debug << "Cannot open the container " << m_name << " of type "
                          << ROOTTREE_StorageType.storageName()
                          << " Class " << pBranch->GetClassName() << " is unknown."
                          << DbPrint::endmsg;
                      return Error;
                   }
                   dsc = BranchDesc(cl, pBranch, leaf, cl->New(), c);
                   dsc.aux_reader = RootAuxDynIO::getReaderForBranch(pBranch);
                   if (dsc.aux_reader) {
                     // If we set up a reader, then disable aging
                     // for this file.  That will prevent POOL from
                     // deleting the file while we still have
                     // references to its branches.
                     dbH.setAge (-10);
                   }
                   break;
                case DbColumn::CHAR:
                case DbColumn::UCHAR:
                case DbColumn::BOOL:
                case DbColumn::SHORT:
                case DbColumn::USHORT:
                case DbColumn::INT:
                case DbColumn::LONG:
                case DbColumn::UINT:
                case DbColumn::ULONG:
                case DbColumn::FLOAT:
                case DbColumn::DOUBLE:
                case DbColumn::LONGLONG:
                case DbColumn::ULONGLONG:
                case DbColumn::STRING:
                case DbColumn::LONG_STRING:
                case DbColumn::NTCHAR:
                case DbColumn::LONG_NTCHAR:
                case DbColumn::TOKEN:
                   dsc.clazz = nullptr;
                   dsc.leaf = leaf;
                   dsc.branch = pBranch;
                   dsc.buffer = nullptr;
                   dsc.object = nullptr;
                   dsc.column = *i;
                   break;
                default:
                   return Error;
               }
            }
            else  {
               log << DbPrintLvl::Warning << "Branch with name:" << colnam 
                   << " not present in container:" << m_name << " of type "
                   << ROOTTREE_StorageType.storageName()
                   << DbPrint::endmsg;
            }
         }
         log << DbPrintLvl::Debug << "Opened container " << m_name << " of type "
             << ROOTTREE_StorageType.storageName()
             << DbPrint::endmsg;
         m_dbH = dbH;
         m_type = info;
         if( mode&pool::UPDATE ) {
            m_rootDb->registerBranchContainer(this);
         }
         return Success;
      }
      else if ( !hasBeenCreated && mode&pool::CREATE )    {
         int count, defSplitLevel=99, 
            defAutoSave=16*1024*1024, defBufferSize=16*1024,
            branchOffsetTabLen=0, containerSplitLevel=defSplitLevel, auxSplitLevel=defSplitLevel;
         DbStatus res = Success;
         try   {
            DbOption opt1("DEFAULT_SPLITLEVEL","");
            DbOption opt2("DEFAULT_AUTOSAVE","");
            DbOption opt3("DEFAULT_BUFFERSIZE","");
            DbOption opt4("TREE_BRANCH_OFFSETTAB_LEN","");
            DbOption opt5("CONTAINER_SPLITLEVEL", m_name);
            DbOption opt6("CONTAINER_SPLITLEVEL", RootAuxDynIO::AUX_POSTFIX);
            dbH.getOption(opt1);
            dbH.getOption(opt2);
            dbH.getOption(opt3);
            dbH.getOption(opt4);
            dbH.getOption(opt5);
            dbH.getOption(opt6);
            opt1._getValue(defSplitLevel);
            opt2._getValue(defAutoSave);
            opt3._getValue(defBufferSize);
            opt4._getValue(branchOffsetTabLen);
            opt5._getValue(containerSplitLevel);
            opt6._getValue(auxSplitLevel);
            if (containerSplitLevel == defSplitLevel) {
               if ( (m_name.size() >= 5 && m_name.substr(m_name.size()-5, 4) == RootAuxDynIO::AUX_POSTFIX)
                    || info->clazz().Properties().HasProperty("IAuxStore") ) containerSplitLevel = auxSplitLevel;
            }
            if (!m_tree) {
               m_tree = new TTree(treeName.c_str(), treeName.c_str(), (m_branchName.empty() ? containerSplitLevel : defSplitLevel));
               if (m_rootDb)
                  m_tree->SetDirectory(m_rootDb->file());
               // Autosave every mega byte...
               m_tree->SetAutoSave(defAutoSave);
            }
            //            - C : a character string terminated by the 0 character
            //            - B : an 8 bit signed integer (Char_t)
            //            - b : an 8 bit unsigned integer (UChar_t)
            //            - S : a 16 bit signed integer (Short_t)
            //            - s : a 16 bit unsigned integer (UShort_t)
            //            - I : a 32 bit signed integer (Int_t)
            //            - i : a 32 bit unsigned integer (UInt_t)
            //            - F : a 32 bit floating point (Float_t)
            //            - D : a 64 bit floating point (Double_t)
            m_branches.resize(cols.size());
            for (i = cols.begin(), count = 0; i != cols.end(); ++i, ++count )   {
               DbStatus iret = Success;
               BranchDesc& dsc = m_branches[count];
               switch ( (*i)->typeID() )    {
                case DbColumn::CHAR:       iret=addBranch(*i,dsc,"/B"); break;
                case DbColumn::UCHAR:      iret=addBranch(*i,dsc,"/b"); break;
                case DbColumn::BOOL:       iret=addBranch(*i,dsc,"/O"); break;
                case DbColumn::SHORT:      iret=addBranch(*i,dsc,"/S"); break;
                case DbColumn::USHORT:     iret=addBranch(*i,dsc,"/s"); break;
                case DbColumn::INT:        iret=addBranch(*i,dsc,"/I"); break;
                case DbColumn::LONG:       iret=addBranch(*i,dsc,"/L"); break;
                case DbColumn::UINT:       iret=addBranch(*i,dsc,"/i"); break;
                case DbColumn::ULONG:      iret=addBranch(*i,dsc,"/l"); break;
                case DbColumn::FLOAT:      iret=addBranch(*i,dsc,"/F"); break;
                case DbColumn::DOUBLE:     iret=addBranch(*i,dsc,"/D"); break;
                case DbColumn::LONGLONG:   iret=addBranch(*i,dsc,"/L"); break;
                case DbColumn::ULONGLONG:  iret=addBranch(*i,dsc,"/l"); break;
                case DbColumn::STRING:
                case DbColumn::LONG_STRING:
                case DbColumn::NTCHAR:
                case DbColumn::LONG_NTCHAR:
                case DbColumn::TOKEN:      
                   iret=addBranch(*i,dsc,"/C");
                   break;
                case DbColumn::BLOB:
                   iret=addObject(*i,dsc,"UCharDbArrayAthena", defSplitLevel, defBufferSize, branchOffsetTabLen);
                   break;
                case DbColumn::ANY:
                case DbColumn::POINTER:
                   iret=addObject(*i, dsc, (*i)->typeName(), containerSplitLevel, defBufferSize, branchOffsetTabLen); 
                   break;
                default:
                   return Error;
               }
               if( !iret.isSuccess() )  {
                  res = iret;
               }
            }
            if( res.isSuccess() )    {
               log << DbPrintLvl::Debug << "Opened container " << m_name << " of type "
                   << ROOTTREE_StorageType.storageName()
                   << DbPrint::endmsg;
               m_dbH  = dbH;
               m_type = info;
               m_rootDb->registerBranchContainer(this);
               return Success;
            }
            debugBreak(nam, "Cannot open ROOT container(Tree/Branch)", false);
            return res;
         }
         catch( const std::exception& e )    {
            debugBreak(nam, "Cannot open ROOT container(Tree/Branch)", e, false);
            res = Error;
         }
         catch (...)   {
            DbPrint err( m_name);
            err << DbPrintLvl::Fatal << "Unknown exception occurred. Cannot give more details." 
                << DbPrint::endmsg;
            debugBreak(nam, "Cannot open ROOT container(Tree/Branch)");
            res = Error;
         }
      }
   }
   log << DbPrintLvl::Error << "Cannot open container '" << nam << "', invalid Database handle." 
       << DbPrint::endmsg;
   return Error;
}


// Define selection criteria
DbStatus  RootTreeContainer::select(DbSelect& sel)    {
  if ( nullptr != m_tree )  {
    if( sel.criteria().length() == 0 || sel.criteria() == "*" )  {
      sel.link().second = -1;
      return Success;
    }
    else  {
      TTreeFormula* stmt = new TTreeFormula("RootSelect", sel.criteria().c_str(), m_tree);
      sel.setStatement(new DbSelect::Ptr<TTreeFormula>(stmt));
      sel.link().second = -1;
      return Success;
    }
  }
  return Error;
}




DbStatus  RootTreeContainer::addObject(const DbColumn* col,
                                       BranchDesc& dsc,
                                       const std::string& typ,
                                       int defSplitLevel, 
                                       int defBufferSize,
                                       int branchOffsetTabLen)
{
   try {
      dsc.buffer  = nullptr;
      dsc.object  = nullptr;
      dsc.column  = col;
      dsc.clazz = TClass::GetClass(typ.c_str());
      if ( nullptr != dsc.clazz )  {
         if ( dsc.clazz->GetStreamerInfo() )  {
            std::string nam  = (m_branchName.empty() ? col->name() : m_branchName);
            if (m_branchName.empty()) {
               for ( std::string::iterator j = nam.begin(); j != nam.end(); ++j )    {
                  if ( !::isalnum(*j) ) *j = '_';
               }
            }
            dsc.branch  = m_tree->Branch(nam.c_str(),   // Branch name
                                         dsc.clazz->GetName(), // Object class
                                         (void*)&dsc.buffer,   // Object address
                                         defBufferSize, // Buffer size
                                         defSplitLevel);        // Split Mode (Levels)
            if ( dsc.branch )  {
               dsc.leaf = dsc.branch->GetLeaf(nam.c_str());
               dsc.branch->SetAutoDelete(kFALSE);
               // AUTO-DELETE is now OFF. 
               // This ensures, that all objects can be deleted
               // by the framework. Keep the created object in the
               // branch descriptor to allow selections
               setBranchOffsetTabLen( dsc.branch, branchOffsetTabLen );

               // AUX STORE specifics
               if ( (nam.size() >= 4 && nam.substr(nam.size()-4) == RootAuxDynIO::AUX_POSTFIX)
	            || RootType(dsc.clazz->GetName()).Properties().HasProperty("IAuxStore") ) {
                  TClass *storeTClass = dsc.clazz->GetBaseClass("SG::IAuxStoreIO");
                  if( storeTClass ) {
                     // This is a class implementing SG::IAuxStoreIO
                     dsc.aux_iostore_IFoffset = dsc.clazz->GetBaseClassOffset( storeTClass );               
                  }
               }
               return Success;
            }
         }
      }
   }
   catch( const std::exception& e )    {
      debugBreak(m_name, "Cannot attach ROOT object branch.", e);
   }
   catch (...)   {
      DbPrint err( m_name);
      err << DbPrintLvl::Fatal << "Unknown exception occurred. Cannot give more details." 
          << DbPrint::endmsg;
      debugBreak(m_name, "Cannot attach ROOT object branch.", true);
   }
   DbPrint log( m_name);
   log << DbPrintLvl::Error << "Failed to open the container " << m_name << " of type "
       << ROOTTREE_StorageType.storageName()
       << " Class " << typ << " is unknown."
       << DbPrint::endmsg;
   return Error;
}




void RootTreeContainer::createBasicAuxBranch(const std::string& branchname, const std::string& leafname, BranchDesc& dsc)
{
   DbPrint log( m_name); 
   log << DbPrintLvl::Debug << "createBasicAuxBranch: " << branchname << ", leaf:" << leafname << DbPrint::endmsg;
   dsc.is_basic_type = true;
   dsc.branch = m_tree->Branch(branchname.c_str(), dsc.buffer, leafname.c_str(), 2048);         
//   if( dsc.branch )  dsc.leaf = dsc.branch->GetLeaf(bnam);            
}


DbStatus  RootTreeContainer::addAuxBranch(const std::string& attribute,
                                          const type_info* typeinfo,
                                          BranchDesc& dsc)
{
   string error_type("is unknown");
   string typenam = SG::normalizedTypeinfoName(*typeinfo);
   string branch_name = RootAuxDynIO::auxBranchName(attribute, m_branchName);
   dsc.branch = nullptr;
   try {
      if( *typeinfo == typeid(UInt_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/i", dsc);
      else if( *typeinfo == typeid(Int_t) )
         createBasicAuxBranch(branch_name, attribute + "/I", dsc);
      else if( *typeinfo == typeid(Double_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/D", dsc);
      else if( *typeinfo == typeid(Float_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/F", dsc);
      else if( *typeinfo == typeid(Long64_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/L", dsc);
      else if( *typeinfo == typeid(ULong64_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/l", dsc);
      else if( *typeinfo == typeid(Short_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/S", dsc);
      else if( *typeinfo == typeid(UShort_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/s", dsc);
      else if( *typeinfo == typeid(Char_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/B", dsc);
      else if( *typeinfo == typeid(UChar_t) ) 
         createBasicAuxBranch(branch_name, attribute + "/b", dsc);
      else if( *typeinfo == typeid(bool) ) 
         createBasicAuxBranch(branch_name, attribute + "/O", dsc);
      else if( *typeinfo == typeid(char*) || *typeinfo == typeid(unsigned char*) )  
         createBasicAuxBranch(branch_name, attribute + "/C", dsc);
      else {
         TClass* cl = TClass::GetClass(typenam.c_str());
         if( !cl ) {
            error_type =" has no TClass";
         } else if( !cl->GetStreamerInfo() )  {
            error_type =" has no streamer";
         } else if( !cl->HasDictionary() ) {
            error_type =" has no dictionary";
         } else {
            dsc.clazz = cl;
            int split = cl->CanSplit() ? 1 : 0;
            dsc.branch  = m_tree->Branch(branch_name.c_str(),   // Branch name
                                         cl->GetName(),         // Object class
                                         (void*)&dsc.buffer,    // Object address
                                         8192,                  // Buffer size
                                         split);                // Split Mode (Levels)
         }
      }
   }
   catch( const std::exception& e )    {
      debugBreak(branch_name, "Cannot attach ROOT branch for AUX attribute.", e);
   }
   catch (...)   {
      DbPrint err( branch_name );
      err << DbPrintLvl::Fatal << "Unknown exception occurred. Cannot give more details." 
          << DbPrint::endmsg;
      debugBreak(branch_name, "Cannot attach ROOT branch for AUX attribute.", true);
   }

   if( dsc.branch )  {
      dsc.branch->SetAutoDelete(kFALSE);
      // AUTO-DELETE is now OFF. 
      // This ensures, that all objects can be deleted
      // by the framework. Keep the created object in the
      // branch descriptor to allow selections

      //setBranchOffsetTabLen( dsc.branch, branchOffsetTabLen );
      return Success;
   }
   
   DbPrint log("RootStorageSvc::addAuxBranch");
   log << DbPrintLvl::Error << "Failed to create Auxiliary branch '" << branch_name << "'."
       << " Class " << typenam << error_type << DbPrint::endmsg;
   return Error;
}




DbStatus 
RootTreeContainer::addBranch(const DbColumn* col,BranchDesc& dsc,const std::string& desc) {
  const char* nam  = (m_branchName.empty() ? col->name().c_str() : m_branchName.c_str());
  std::string  coldesc = col->name() + desc;
  char buff[32];
  dsc.branch = m_tree->Branch(nam, buff, coldesc.c_str(), 4096);
  if ( dsc.branch )  {
    dsc.leaf = dsc.branch->GetLeaf(nam);
    dsc.clazz = nullptr;
    dsc.column = col;
    dsc.buffer = nullptr;
    dsc.object = nullptr;
    return Success;
  }
  return Error;
}



void RootTreeContainer::setBranchOffsetTabLen(TBranch* b, int offsettab_len)
{
   if( offsettab_len > 0 ) {
      if( b->GetEntryOffsetLen() > 0 )
         b->SetEntryOffsetLen( offsettab_len ); 
      TIter biter( b->GetListOfBranches() );
      TBranch* subbranch(nullptr);
      while( (subbranch = (TBranch*)biter.Next()) ) {
         setBranchOffsetTabLen( subbranch, offsettab_len ); 
      }
   }
}



/// Access options
DbStatus RootTreeContainer::getOption(DbOption& opt)  const  { 
  if ( m_tree )  {
    const char* n = opt.name().c_str();
    if ( !strcasecmp(n,"BYTES_IO") )  {
       for( auto& branch: m_branches ) {
          if( branch.aux_reader ) {
             const_cast<RootTreeContainer*>(this)->m_ioBytes += branch.aux_reader->getBytesRead();
             branch.aux_reader->resetBytesRead();
          }
       }                                          
       return opt._setValue((int)m_ioBytes);
    }
    else if ( !strcasecmp(n,"BRANCH") )  {
      TBranch* b = branch(opt.option());
      return opt._setValue((void*)b);
    }
    else if ( ::toupper(n[0])=='B' && opt.name().length() > 7 ) {
      TBranch* b = branch(opt.option());
      if ( b )  {
        switch(::toupper(n[7]))  {
        case 'B':
          if ( !strcasecmp(n+7,"BASKET_SIZE") )
            return opt._setValue(int(b->GetBasketSize()));
          break;
        case 'C':
          if ( !strcasecmp(n+7,"COMPRESSION_LEVEL") )
            return opt._setValue(int(b->GetCompressionLevel()));
          if ( !strcasecmp(n+7,"COMPRESSION_ALGORITHM") )
            return opt._setValue(int(b->GetCompressionAlgorithm()));
          break;
        case 'E':
          if ( !strcasecmp(n+7,"ENTRIES") )
            return opt._setValue(int(b->GetEntries()));
          break;
        case 'F':
          if ( !strcasecmp(n+7,"FILE_NAME") )
            return opt._setValue(b->GetFileName());
          else if ( !strcasecmp(n+7,"FILE") )
            return opt._setValue((void*)b->GetFile());
          break;
        case 'M':
          if ( !strcasecmp(n+7,"MAX_BASKETS") )
            return opt._setValue(int(b->GetMaxBaskets()));
          break;
        case 'N':
          if ( !strcasecmp(n+7,"NLEAVES") )
            return opt._setValue(int(b->GetNleaves()));
          break;
        case 'S':
          if ( !strcasecmp(n+7,"SPLIT_LEVEL") )
            return opt._setValue(int(b->GetSplitLevel()));
          break;
        case 'T':
          if ( !strcasecmp(n+7,"TOTAL_SIZE") )
            return opt._setValue(double(b->GetTotalSize()));
          else if ( !strcasecmp(n+7,"TOTAL_BYTES") )
            return opt._setValue(double(b->GetTotBytes()));
          break;
        case 'Z':
          if ( !strcasecmp(n+7,"ZIP_BYTES") )
            return opt._setValue(double(b->GetZipBytes()));
          break;
        }
      }
    }
    else if ( !strcasecmp(n,"TREE") )  {
      return opt._setValue((void*)m_tree);
    }
    else if ( ::toupper(n[0]=='T') && opt.name().length() > 6 ) {
      switch(::toupper(n[5]))   {
      case 'B':
        if ( !strcasecmp(n+5,"BRANCH_IDX") )  {
          int idx = 0;
          opt._getValue(idx);
          TObjArray* arr = m_tree->GetListOfBranches();
          return opt._setValue((void*)arr->At(idx));
        }
        if ( !strcasecmp(n+5,"BRANCH_NAME") )  {
          const char* br_nam = nullptr;
          opt._getValue(br_nam);
          if ( br_nam )  {
            return opt._setValue((void*)m_tree->GetBranch(br_nam));
          }
          opt._setValue((void*)nullptr);
        }
        if ( !strcasecmp(n+5,"BRANCH_IDX_NAME") )  {
          int idx = 0;
          opt._getValue(idx);
          TObjArray* arr = m_tree->GetListOfBranches();
          TBranch* br = (TBranch*)arr->At(idx);
          if ( br )  {
            return opt._setValue((char*)br->GetName());
          }
          opt._setValue((char*)nullptr);
        }
        break;
      case 'E':
        if ( !strcasecmp(n+5,"ENTRIES") )
          return opt._setValue(int(m_tree->GetEntries()));
        break;
      case 'F':
        if ( !strcasecmp(n+5,"FILE_NUMBER") )
          return opt._setValue(int(m_tree->GetFileNumber()));
        break;
      case 'M':
        if ( !strcasecmp(n+5,"MAX_SIZE") )
          return opt._setValue((long long int)m_tree->GetMaxTreeSize());
        else if ( !strcasecmp(n+5,"MAX_VIRTUAL_SIZE") )
          return opt._setValue(int(m_tree->GetMaxVirtualSize()));
        break;
      case 'N':
        if ( !strcasecmp(n+5,"NBRANCHES") )
          return opt._setValue(int(m_tree->GetNbranches()));
        break;
      case 'T':
        if ( !strcasecmp(n+5,"TOTAL_BYTES") )
          return opt._setValue(double(m_tree->GetTotBytes()));
        break;
      case 'Z':
        if ( !strcasecmp(n+5,"ZIP_BYTES") )
          return opt._setValue(double(m_tree->GetZipBytes()));
        break;
      }
    }
  }
  return Error;  
}

/// Set options
DbStatus RootTreeContainer::setOption(const DbOption& opt)  { 
  if ( m_tree )  {
    const char* n = opt.name().c_str();
    if ( ::toupper(n[0]) == 'B' )  {
      TBranch* b = branch(opt.option());
      if ( b )  {
        switch(::toupper(n[7]))   {
        case 'A':
          if ( !strcasecmp(n+7,"AUTODELETE") )  {
            int val=0;
            opt._getValue(val);
            b->SetAutoDelete(val!=0);
            return Success;
          }
          break;
        case 'C':
          if ( !strcasecmp(n+7,"COMPRESSION_LEVEL") )  {
            int val=1;
            opt._getValue(val);
            b->SetCompressionLevel(val);
            return Success;
          }
          if ( !strcasecmp(n+7,"COMPRESSION_ALGORITHM") )  {
            int val=1;
            opt._getValue(val);
            b->SetCompressionAlgorithm(val);
            return Success;
          }
          break;
        case 'P':
          if ( !strcasecmp(n+7,"PRINT") )  {
            b->Print();
            std::cout << std::endl;
            return Success;
          }
          break;
        case 'B':
          if ( !strcasecmp(n+7,"BASKET_SIZE") )  {
            int value = 16*1024;
            opt._getValue(value);
            b->SetBasketSize(value);
            return Success;
          }
          break;
        default:
          break;
        }
      }
    }
    else if ( ::toupper(n[0]) == 'T' )  {
      switch(::toupper(n[5]))   {
      case 'A':
        if ( !strcasecmp(n+5,"AUTO_SAVE") )  {
          int val=1;
          opt._getValue(val);
          m_tree->SetAutoSave(val);
          return Success;
        } else if ( !strcasecmp(n+5,"AUTO_FLUSH") )  {
          int val=1;
          opt._getValue(val);
          m_tree->SetAutoFlush(val);
          // cout << "----------- setting AUTO_FLUSH for " << m_tree->GetName() << endl;
          return Success;
        }
        break;
      case 'M':
        if ( !strcasecmp(n+5,"MAX_SIZE") )  {
          long long int val=1;
          opt._getValue(val);
          m_tree->SetMaxTreeSize(val);
          return Success;
        }
        break;
      case 'P':
        if ( !strcasecmp(n+5,"PRINT") )  {
          m_tree->Print();
          std::cout << std::endl;
          return Success;
        }
        break;
      }
    }
  }
  return Error;  
}


/// Execute transaction action
DbStatus RootTreeContainer::transAct(Transaction::Action action) 
{
   // execure action on the base class first
   DbStatus status = DbContainerImp::transAct(action);
   if( !status.isSuccess() ) return status;

   if( action != Transaction::TRANSACT_FLUSH ) return Success;
   if( !m_tree ) return Error;
   if( !isBranchContainer() ) {
      m_tree->AutoSave();
      return Success;
   }
   // check if all TTree branches were filled and write the TTree
   Branches::const_iterator k;
   for(k=m_branches.begin(); k !=m_branches.end(); ++k) {
      Long64_t branchEntries = k->branch->GetEntries();
      Long64_t treeEntries = m_tree->GetEntries();
      if (branchEntries > treeEntries) {
         TIter next(m_tree->GetListOfBranches());
         TBranch * b = nullptr;
         while((b = (TBranch*)next())){
            if (b->GetEntries() != branchEntries) {
               DbPrint log(m_name);
               log << DbPrintLvl::Error << "Every branch must have the same number of entries."
                   << "  branch " << b->GetName() << " " << b->GetEntries()
                   << DbPrint::endmsg;
               return Error;
            }
         }
         m_tree->SetEntries(branchEntries);
         m_tree->AutoSave();
      } else if (branchEntries < treeEntries) {
         DbPrint log(m_name);
         log << DbPrintLvl::Error << "Every branch must have the same number of entries." 
             << DbPrint::endmsg;
         return Error;
      }
   }
   return Success;
}
