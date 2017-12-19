/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//        Root Database file implementation
//--------------------------------------------------------------------
//
//        Package    : RootStorageSvc (The POOL project)
//
//====================================================================
#include "RootDatabase.h"
#include "RootTreeContainer.h"
#include "StorageSvc/DbInstanceCount.h"
#include "StorageSvc/IOODatabase.h"
#include "StorageSvc/DbDatabase.h"
#include "StorageSvc/DbOption.h"
#include "StorageSvc/DbDomain.h"
#include "StorageSvc/DbPrint.h"
#include "StorageSvc/DbTransaction.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IFileMgr.h"

#include <iostream>
#include <string>
#include <cerrno>

// Root include files
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "TTreeCache.h"

using namespace pool;
using namespace std;
//namespace pool {
//  void patchStreamers(RootDatabase* db, TFile* file);
//}

/// Standard Constuctor
RootDatabase::RootDatabase(IOODatabase* idb)
: DbDatabaseImp(idb), m_file(nullptr), 
  m_defCompression(1),
  m_defCompressionAlg(1),
  m_defSplitLevel(99),
  m_defAutoSave(16*1024*1024),
  m_defBufferSize(16*1024),
  m_defWritePolicy(TObject::kOverwrite),   // On write create new versions
  m_branchOffsetTabLen(0),
  m_defTreeCacheLearnEvents(100),
  m_fileMgr(nullptr)
{
  m_version = "1.1";
  DbInstanceCount::increment(this);
  m_counters[READ_COUNTER] = m_counters[WRITE_COUNTER] = m_counters[OTHER_COUNTER] = 0;
}

RootDatabase::~RootDatabase()  {
  DbInstanceCount::decrement(this);
  deletePtr(m_file);
}

bool RootDatabase::exists(const std::string& nam)   {
  Bool_t result = gSystem->AccessPathName(nam.c_str(), kFileExists);
  return (result == kFALSE) ? true : false;
}

/// Access the size of the database: May be undefined for some technologies
long long int RootDatabase::size()  const   {
  if ( m_file )  {
    return m_file->GetSize();
  }
  return -1;
}

/// Callback after successful open of a database object
DbStatus RootDatabase::onOpen(const DbDatabase& dbH, DbAccessMode mode)  {
  std::string par_val;
  DbPrint log("RootDatabase.onOpen");
  if ( !dbH.param("FORMAT_VSN", par_val).isSuccess() )  {
    if ( mode&pool::CREATE || mode&pool::UPDATE ) {
      return dbH.addParam("FORMAT_VSN", m_version);
    }
    log << DbPrintLvl::Warning << "No ROOT data format parameter present "
        << "and file not opened for update." << DbPrint::endmsg;
  }
  else {
    m_version = par_val;
  }
  if ( m_file )  {
    log << DbPrintLvl::Debug << dbH.name() << " File version:" << int(m_file->GetVersion())
        << DbPrint::endmsg;
  }
  else  {
    log << DbPrintLvl::Error << "Unknown Root file ..."
        << DbPrint::endmsg;
  }
  //patchStreamers(this, m_file);
  return Success;
}

// Open a new root Database: Access the TFile
DbStatus RootDatabase::open(const DbDomain& domH,const std::string& nam,DbAccessMode mode)   {
  DbPrint log("RootDatabase.open");
  const char* fname = nam.c_str();
  Bool_t result = ( mode == pool::READ ) ? kFALSE : gSystem->AccessPathName(fname, kFileExists);
  DbOption opt1("DEFAULT_COMPRESSION","");
  DbOption opt2("DEFAULT_COMPRESSIONALG","");
  DbOption opt3("DEFAULT_SPLITLEVEL","");
  DbOption opt4("DEFAULT_AUTOSAVE","");
  DbOption opt5("DEFAULT_BUFFERSIZE","");
  DbOption opt6("TREE_BRANCH_OFFSETTAB_LEN","");
  domH.getOption(opt1);
  domH.getOption(opt2);
  domH.getOption(opt3);
  domH.getOption(opt4);
  domH.getOption(opt5);
  domH.getOption(opt6);
  opt1._getValue(m_defCompression);
  opt2._getValue(m_defCompressionAlg);
  opt3._getValue(m_defSplitLevel);
  opt4._getValue(m_defAutoSave);
  opt5._getValue(m_defBufferSize);
  opt6._getValue(m_branchOffsetTabLen);
  //gDebug = 2;
  TDirectory::TContext dirCtxt(0);


  if (m_fileMgr == nullptr) {
    IService *is(nullptr);

    if (Gaudi::svcLocator()->getService("FileMgr",is,true).isFailure()) {
      log << DbPrintLvl::Error
	  << "unable to get the FileMgr, will not manage TFiles"
	  << DbPrint::endmsg;
    } else {
      m_fileMgr = dynamic_cast<IFileMgr*>(is);
    }
    
  }


  // FIXME: hack to avoid issue with setting up RecExCommon links
  if (m_fileMgr != nullptr && m_fileMgr->hasHandler(Io::ROOT).isFailure()) {
    log << DbPrintLvl::Info
	<< "Unable to locate ROOT file handler via FileMgr. "
	<< "Will use default TFile::Open" 
	<< DbPrint::endmsg;
    m_fileMgr = nullptr;
  }

  if ( mode == pool::READ )   {
    if (m_fileMgr == nullptr) {
      m_file = TFile::Open(fname);
    } else {
      void *vf(nullptr);
      int r =  m_fileMgr->open(Io::ROOT,"RootDatabase",fname,Io::READ,vf,"POOL",false);
      if (r < 0) {
	log << DbPrintLvl::Error << "unable to open \"" << fname 
	    << "\" for READ"
	    << DbPrint::endmsg;
      } else {      
	m_file = (TFile*)vf;
      }
    }
  }
  else if ( mode&pool::UPDATE && result == kFALSE )    {
    if (m_fileMgr == nullptr) {
      m_file = TFile::Open(fname, "UPDATE", fname);
    } else {
      void *vf(nullptr);
      int r =  m_fileMgr->open(Io::ROOT,"RootDatabase",fname,Io::APPEND,vf,"POOL",true);
      if (r < 0) {
	log << DbPrintLvl::Error << "unable to open \"" << fname 
	    << "\" for UPDATE"
	    << DbPrint::endmsg;
      } else {      
	m_file = (TFile*)vf;
      }
    }
    if (m_file != nullptr) {
      m_file->SetCompressionLevel(m_defCompression);
      m_file->SetCompressionAlgorithm(m_defCompressionAlg);
    }
  }
  else if ( pool::RECREATE == (mode&pool::RECREATE) )   {
    if (m_fileMgr == nullptr) {
      m_file = TFile::Open(fname, "RECREATE", fname);
    } else {
      void *vf(nullptr);
      int r =  m_fileMgr->open(Io::ROOT,"RootDatabase",fname,Io::WRITE|Io::CREATE,vf,"POOL",true);
      if (r < 0) {
	log << DbPrintLvl::Error << "unable to open \"" << fname 
	    << "\" for RECREATE"
	    << DbPrint::endmsg;
      } else {      
	m_file = (TFile*)vf;
      }
    }
    if (m_file != nullptr) {
      m_file->SetCompressionLevel(m_defCompression);
      m_file->SetCompressionAlgorithm(m_defCompressionAlg);
    }
  }
  else if ( mode&pool::CREATE && result == kTRUE )   {
    if (m_fileMgr == nullptr) {
      m_file = TFile::Open(fname, "RECREATE", fname);
    } else {
      void *vf(nullptr);
      int r =  m_fileMgr->open(Io::ROOT,"RootDatabase",fname,Io::WRITE|Io::CREATE,vf,"POOL",true);
      if (r < 0) {
	log << DbPrintLvl::Error << "unable to open \"" << fname 
	    << "\" for RECREATE"
	    << DbPrint::endmsg;
      } else {      
	m_file = (TFile*)vf;
      }
    }
    if (m_file != nullptr) {
      m_file->SetCompressionLevel(m_defCompression);
      m_file->SetCompressionAlgorithm(m_defCompressionAlg);
    }
  }
  else if ( mode&pool::CREATE && result == kFALSE )   {
    log << DbPrintLvl::Error << "You cannot open a ROOT file in mode CREATE"
        << " if it already exists. " << DbPrint::endmsg
        << "[" << nam << "]" << DbPrint::endmsg
        << "Use the RECREATE flag if you wish to overwrite."
        << DbPrint::endmsg;
  }
  else if ( mode&pool::UPDATE && result == kTRUE )   {
    log << DbPrintLvl::Error << "You cannot open a ROOT file in mode UPDATE"
        << " if it does not exists. " << DbPrint::endmsg
        << "[" << nam << "]" << DbPrint::endmsg
        << "Use the CREATE or RECREATE flag."
        << DbPrint::endmsg;
  }
  if ( m_file )   {
    log << DbPrintLvl::Info <<  fname << " File version:" << m_file->GetVersion() << DbPrint::endmsg;
    if ( !m_file->IsOpen() )   {
      log << DbPrintLvl::Error << "Failed to open file:" << nam << DbPrint::endmsg;
      deletePtr(m_file);
    }
  }
  else if ( mode == pool::READ )   {
    log << DbPrintLvl::Error << "You cannot open the ROOT file [" << nam << "] in mode READ"
        << " if it does not exists. " << DbPrint::endmsg;
  }

  return (nullptr == m_file) ? Error : Success;
}

/// Re-open database with changing access permissions
DbStatus RootDatabase::reopen(DbAccessMode mode)   {
  int result = -1;
  if ( m_file )   {
    TDirectory::TContext dirCtxt(0);
    if ( mode == pool::READ ) {
      result = m_file->ReOpen("READ");
    }
    else if ( mode == pool::UPDATE )  {
      result = m_file->ReOpen("UPDATE");
    }
    else  {
      const char* nam = (m_file) ? m_file->GetName() : "UNKNOWN";
      DbPrint log("RootDatabase.reopen");
      log << DbPrintLvl::Error << "Failed to reopen file: " << nam;
      log << " in mode " << accessMode(mode) << DbPrint::endmsg;
    }
  }
  return (0 == result) ? Success : Error;
}

static void printErrno(DbPrint& log, const char* nam, int err) {
  switch( err )  {
    case 0:      /// No error - all fine.
      break;
    case EXDEV:  /// Cross-device link. Attempt to move a file to a different device 
    case ENOENT: /// No such file or directory.
    case EINVAL: /// Invalid argument. An invalid value was given
    case EACCES: /// Permission denied. The permission setting does not allow 
    case EBADF:  /// file descriptor is invalid or file not open for writing
    case ENOSPC: /// not enough space left on the device for the operation.
    default:
      {
        log << DbPrintLvl::Error << "Failed to modify file: " << nam
          << " Errno=" << err << " " << strerror(err) << DbPrint::endmsg;
        break;
      }
  }
}

/// Close the root Database: in CREATE/Update mode write the file header...
DbStatus RootDatabase::close(DbAccessMode /* mode */ )  {
  int n(0);
  if ( m_file )   {
    if ( m_file->IsOpen() )     {
      DbPrint log("RootDatabase.close");
      const char* nam = (m_file) ? m_file->GetName() : "UNKNOWN";
      bool closed(false);
      if ( byteCount(WRITE_COUNTER) > 0 )   {
        TDirectory::TContext dirCtxt(0);
        m_file->ResetErrno();
        m_file->Write("0", m_defWritePolicy);
        printErrno(log, nam, m_file->GetErrno());
        //m_file->ls();
        m_file->ResetErrno();

	if (m_fileMgr == nullptr) {
	  m_file->Close();
	} else {
	  n = m_fileMgr->close(m_file,"RootDatabase");
	}
	closed = true;
        printErrno(log, nam, m_file->GetErrno());
        if (m_file->GetSeekKeys() == 0)   {
          log << DbPrintLvl::Error << "Failed to close file: " << nam
            << " something wrong happened in Close." << DbPrint::endmsg;
        }
      }
      log << DbPrintLvl::Debug
          << "I/O READ  Bytes: " << byteCount(READ_COUNTER)  << DbPrint::endmsg
          << "I/O WRITE Bytes: " << byteCount(WRITE_COUNTER) << DbPrint::endmsg
          << "I/O OTHER Bytes: " << byteCount(OTHER_COUNTER) << DbPrint::endmsg;

      if (!closed && m_fileMgr != nullptr) {
	n = m_fileMgr->close(m_file,"RootDatabase");
      }      
    }    
  }
  if (n == 0) {
    return deletePtr(m_file);
  } else {
    return Success;
  }
}

/// Do some statistics: add number of bytes read/written/other
void RootDatabase::addByteCount(int which, long long int num_bytes)  {
  switch(which)   {
  case OTHER_COUNTER:
  case WRITE_COUNTER:
  case READ_COUNTER:
    m_counters[which] += num_bytes;
    break;
  default:
    break;
  }
}

/// Do some statistics: retrieve number of bytes read/written/other
long long int RootDatabase::byteCount(int which) const   {
  switch(which)   {
  case OTHER_COUNTER:
  case WRITE_COUNTER:
  case READ_COUNTER:
    return m_counters[which];
  default:
    return -1;
  }
}

/// Access options
DbStatus RootDatabase::getOption(DbOption& opt)  const   { 
  const char* n = opt.name().c_str();
  switch( ::toupper(n[0]) )  {
    case 'C':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n, "COMPRESSION_LEVEL") )     // int
        return opt._setValue(int(m_file->GetCompressionLevel()));
      else if ( !strcasecmp(n, "COMPRESSION_ALGORITHM") ) // int
        return opt._setValue(int(m_file->GetCompressionAlgorithm()));
      else if ( !strcasecmp(n, "COMPRESSION_FACTOR") )    // float
        return opt._setValue(double(m_file->GetCompressionFactor()));
      else if ( !strcasecmp(n, "CONTAINER_SPLITLEVEL") )  {
        if (!opt.option().size()) {
          DbPrint log("RootDatabase.getOption");
          log << DbPrintLvl::Error << "Must set option to container name to set CONTAINER_SPLITLEVEL" << DbPrint::endmsg;
          return Error;
        }
        const string containerName = opt.option();
        int containerSplitLevel=m_defSplitLevel;
        map< string, int >::const_iterator  cspit = m_customSplitLevel.find( containerName );
        if ( cspit != m_customSplitLevel.end() ) {
          containerSplitLevel = cspit->second;
        }
        return opt._setValue(int(containerSplitLevel));
      }
      break;
    case 'B':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"BEST_BUFFER") )            // int
        return opt._setValue(int(m_file->GetBestBuffer()));
      else if ( !strcasecmp(n,"BYTES_WRITTEN") )          // double
        return opt._setValue(double(m_file->GetBytesWritten()));
      else if ( !strcasecmp(n,"BYTES_READ") )             // double
        return opt._setValue(double(m_file->GetBytesRead()));
      else if ( !strcasecmp(n,"BYTES_FREE") )             // int
        return opt._setValue(int(m_file->GetNbytesFree()));
      else if ( !strcasecmp(n,"BYTES_INFO") )             // int
        return opt._setValue(int(m_file->GetNbytesInfo()));
      else if ( !strcasecmp(n,"BYTES_KEYS") )             // int
        return opt._setValue(int(m_file->GetNbytesKeys()));
      break;
    case 'D':
      if ( !strcasecmp(n,"DEFAULT_COMPRESSION") )         // int
        return opt._setValue(int(m_defCompression));
      else if ( !strcasecmp(n,"DEFAULT_COMPRESSIONALG") ) // int
        return opt._setValue(int(m_defCompressionAlg));
      else if ( !strcasecmp(n, "DEFAULT_SPLITLEVEL") )    // int
        return opt._setValue(int(m_defSplitLevel));
      else if ( !strcasecmp(n, "DEFAULT_AUTOSAVE") )      // int
        return opt._setValue(int(m_defAutoSave));   
      else if ( !strcasecmp(n, "DEFAULT_BUFFERSIZE") )    // int
        return opt._setValue(int(m_defBufferSize));
      else if ( !strcasecmp(n, "DEFAULT_WRITEPOLICY") )   // int
        return opt._setValue(int(m_defWritePolicy));
      break;
    case 'N':
      //if ( !strcasecmp(n,"NFREE") )                     // int
      //  return opt._setValue(int(m_file->GetNfree()));
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"NKEYS") )                  // int
        return opt._setValue(int(m_file->GetNkeys()));
      break;
    case 'F':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"FILEBYTES_WRITTEN") )      // double
        return opt._setValue(double(m_file->GetFileBytesWritten()));
      else if ( !strcasecmp(n,"FILEBYTES_READ") )         // double
        return opt._setValue(double(m_file->GetFileBytesRead()));
      else if ( !strcasecmp(n,"FILE_READ_CALLS") )        // int
        return opt._setValue(int(m_file->GetFileReadCalls()));
      else if ( !strcasecmp(n,"FILE_DESCRIPTOR") )        // int
        return opt._setValue(int(m_file->GetFd()));
      else if ( !strcasecmp(n,"FILE_VERSION") )           // int
        return opt._setValue(int(m_file->GetVersion()));
      else if ( !strcasecmp(n,"FILE_SIZE") )              // int
        return opt._setValue((long long int)m_file->GetSize());
      else if ( !strcasecmp(n,"FILE_ERROR") )             // int
        return opt._setValue(int(m_file->GetErrno()));
      else if ( !strcasecmp(n,"FILE_KEYS") )              // void*
        return opt._setValue((void*)m_file->GetListOfKeys());
      else if ( !strcasecmp(n,"FILE_END") )               // int
        return opt._setValue((long long int)m_file->GetEND());
      break;
    case 'G':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"GET_OBJECT") )  {         // void*
        const char* key = "";
        opt._getValue(key);
        return opt._setValue((void*)m_file->Get(key));
      }
      break;
    case 'I':
      if ( !strcasecmp(n,"IOBYTES_WRITTEN") )      // int
        return opt._setValue((long long int)(byteCount(WRITE_COUNTER)));
      else if ( !strcasecmp(n,"IOBYTES_READ") )         // int
        return opt._setValue((long long int)(byteCount(READ_COUNTER)));
      break;
    case 'R':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"READ_CALLS") )        // int
        return opt._setValue(int(m_file->GetReadCalls()));
      break;
    case 'T':
      if( !strcasecmp(n+5,"BRANCH_OFFSETTAB_LEN") )  {
        return opt._setValue(int(m_branchOffsetTabLen));
      } else if( !strcasecmp(n,"TFILE") )  {                  // void*
          return opt._setValue((void*)m_file);
      } else if( !strcasecmp(n+5,"MAX_SIZE") )  {
          return opt._setValue((long long int)TTree::GetMaxTreeSize());
      } else if( !strcasecmp(n+5,"CACHE_SIZE") ) {
          if (!m_treeNameWithCache.size())
              return opt._setValue((int)0);
          if ( !m_file ) return Error;
          TTree* tr = (TTree*)m_file->Get(m_treeNameWithCache.c_str());
          if (tr) return opt._setValue((int)tr->GetCacheSize());
          return opt._setValue((int)0);
      } else if( !strcasecmp(n+5,"CACHE_LEARN_EVENTS") ) {
          return opt._setValue((int)TTreeCache::GetLearnEntries());
      } else if( !strcasecmp(n+5,"NAME_WITH_CACHE") ) {
          return opt._setValue(m_treeNameWithCache.c_str());
      }
      break;
    default:
      break;
  }
  return Error;  
}

/// Set options
DbStatus RootDatabase::setOption(const DbOption& opt)  {
  const char* n = opt.name().c_str();
  switch( ::toupper(n[0]) )  {
    case 'C':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n, "CD") )  {
        const char* key = "";
        opt._getValue(key);
        m_file->cd(key);
        return Success;
      }
      else if ( !strcasecmp(n, "COMPRESSION_LEVEL") )  {
        int val=1;
        opt._getValue(val);
        m_file->SetCompressionLevel(val);
        return Success;
      }
      else if ( !strcasecmp(n, "COMPRESSION_ALGORITHM") )  {
        int val=1;
        opt._getValue(val);
        m_file->SetCompressionAlgorithm(val);
        return Success;
      }
      else if ( !strcasecmp(n, "CONTAINER_SPLITLEVEL") )  {
        if (!opt.option().size()) {
          DbPrint log("RootDatabase.setOption");
          log << DbPrintLvl::Error << "Must set option to container name to set CONTAINER_SPLITLEVEL" << DbPrint::endmsg;
          return Error;
        }
        string containerName = opt.option();
        int val=m_defSplitLevel;
        opt._getValue(val);
        m_customSplitLevel.insert(pair< std::string, int>(containerName, val) );
        return Success;
      }
      break;
    case 'D':
      if (      !strcasecmp(n, "DEFAULT_COMPRESSION") )   // int
        return opt._getValue(m_defCompression);
      else if ( !strcasecmp(n, "DEFAULT_COMPRESSIONALG") )// int
        return opt._getValue(m_defCompressionAlg);
      else if ( !strcasecmp(n, "DEFAULT_SPLITLEVEL") )    // int
        return opt._getValue(m_defSplitLevel);
      else if ( !strcasecmp(n, "DEFAULT_AUTOSAVE") )      // int
        return opt._getValue(m_defAutoSave);   
      else if ( !strcasecmp(n, "DEFAULT_BUFFERSIZE") )    // int
        return opt._getValue(m_defBufferSize);
      else if ( !strcasecmp(n, "DEFAULT_WRITEPOLICY") )   // int
        return opt._getValue(m_defWritePolicy);
      break;
    case 'F':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"FILEBYTES_WRITTEN") )   {
        double val = 0;
        opt._getValue(val);
        Long64_t v = (Long64_t)val;
        m_file->SetFileBytesWritten(v);
        return Success;
      }
      else if ( !strcasecmp(n,"FILEBYTES_READ") )  {
        double val = 0;
        opt._getValue(val);
        Long64_t v = (Long64_t)val;
        m_file->SetFileBytesRead(v);
        return Success;
      }
      else if ( !strcasecmp(n,"FILE_FLUSH") )  {
        m_file->Flush();
        return Success;
      }
      else if ( !strncasecmp(n, "FILE_READSTREAMERINFO",15) )  {
        m_file->ReadStreamerInfo();
        return Success;
      }
      break;
    case 'P':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"PRINT") )  {
        m_file->Print();
        return Success;
      }
      break;
    case 'R':
      if ( !m_file )
        return Error;
      else if ( !strncasecmp(n, "READSTREAMERINFO",10) )  {
        m_file->ReadStreamerInfo();
        return Success;
      }
      break;
    case 'S':
      if ( !m_file )
        return Error;
      else if ( !strcasecmp(n,"SETMODIFIED") )  {
        m_file->SetModified();
        return Success;
      }
      break;
    case 'T':
       if ( !strcasecmp(n+5,"BRANCH_OFFSETTAB_LEN") )  {
	  return opt._getValue(m_branchOffsetTabLen);
       }
       else if ( !strcasecmp(n+5,"MAX_SIZE") )  {
	  long long int max_size = TTree::GetMaxTreeSize();
	  DbStatus sc = opt._getValue(max_size);
	  if ( sc.isSuccess() )  {
	     TTree::SetMaxTreeSize(max_size);
          }
          return sc;
       }
       else if ( !strcasecmp(n+5,"MAX_VIRTUAL_SIZE") )  {
          DbPrint log("RootDatabase.setOption");
          log << DbPrintLvl::Debug << "Request virtual tree size" << DbPrint::endmsg;
          if ( !m_file ) return Error;
          log << DbPrintLvl::Debug << "File name " << m_file->GetName() << DbPrint::endmsg;

          int virtMaxSize = 0;
          opt._getValue(virtMaxSize);
          if (!opt.option().size()) {
             log << DbPrintLvl::Error << "Must set option to tree name to start TREE_MAX_VIRTUAL_SIZE " << DbPrint::endmsg;
             return Error;
          }
          TTree* tree = (TTree*)m_file->Get(opt.option().c_str());
          if (!tree) {
             log << DbPrintLvl::Error << "Could not find tree " << opt.option() << DbPrint::endmsg;
             return Error;
          }
          log << DbPrintLvl::Debug << "Got tree " << tree->GetName() << DbPrint::endmsg;
          tree->SetMaxVirtualSize(virtMaxSize);
          return Success;
       }
       else if ( !strcasecmp(n+5,"AUTO_FLUSH") )  {
          return setAutoFlush(opt);
       }
       else if ( !strcasecmp(n+5,"CACHE_LEARN_EVENTS") )  {
          DbStatus s = opt._getValue(m_defTreeCacheLearnEvents);
          if( s.isSuccess() ) {
             DbPrint log("RootDatabase.setOption");
             if ( m_file->GetListOfKeys()->Contains("CollectionTree") )  {
                TTree *tree = (TTree*)m_file->Get("CollectionTree");
                if (tree != nullptr && tree->GetAutoFlush() > 0) {
                   if (m_defTreeCacheLearnEvents < tree->GetAutoFlush()) {
                      log << DbPrintLvl::Info << n << ": Overwriting LearnEvents with CollectionTree AutoFlush" << DbPrint::endmsg;
                      m_defTreeCacheLearnEvents = tree->GetAutoFlush();
                   }
                }
             }
             TTreeCache::SetLearnEntries(m_defTreeCacheLearnEvents);
             log << DbPrintLvl::Debug << n << " = " << m_defTreeCacheLearnEvents << DbPrint::endmsg;
          }
          return s;
       }
       else if ( !strcasecmp(n+5,"CACHE") )  {
           DbPrint log("RootDatabase.setOption");
           log << DbPrintLvl::Debug << "Request tree cache " << DbPrint::endmsg;
           if ( !m_file ) return Error;
           log << DbPrintLvl::Debug << "File name " << m_file->GetName() << DbPrint::endmsg;

           int cacheSize = 0;
           opt._getValue(cacheSize);
           if (!opt.option().size()) {
               log << DbPrintLvl::Error << "Must set option to tree name to start TREE_CACHE " << DbPrint::endmsg;
               return Error;
           }
           m_treeNameWithCache = opt.option();
           TTree* tr = (TTree*)m_file->Get(m_treeNameWithCache.c_str());
           if (!tr) {
               log << DbPrintLvl::Error << "Could not find tree " << m_treeNameWithCache << DbPrint::endmsg;
               return Error;
           }
           else log << DbPrintLvl::Debug << "Got tree " << tr->GetName() 
                    << " read entry " << tr->GetReadEntry() << DbPrint::endmsg;
           tr->SetCacheSize(cacheSize);
           TTreeCache::SetLearnEntries(m_defTreeCacheLearnEvents);
           TTreeCache* cache = (TTreeCache*)m_file->GetCacheRead();
           if (cache) {
               cache->SetEntryRange(0, tr->GetEntries());
               log << DbPrintLvl::Debug << "Using Tree cache. Size: " << cacheSize 
                   << " Nevents to learn with: " << m_defTreeCacheLearnEvents << DbPrint::endmsg;
           }
           else if (cacheSize != 0) {
               log << DbPrintLvl::Error << "Could not get cache " << DbPrint::endmsg;
               return Error;
           }
           return Success;
       }
      break;
    default:
      break;
  }
  return Error;  
}

/// Set TTree autoFlush value.  For Branch Containers enable TTree Fill mode
DbStatus RootDatabase::setAutoFlush(const DbOption& opt)
{
   DbPrint log("RootDatabase.setOption");
   log << DbPrintLvl::Debug << "Request TREE_AUTO_FLUSH " << DbPrint::endmsg;
   if ( !m_file ) return Error;
   log << DbPrintLvl::Debug << "File name " << m_file->GetName() << DbPrint::endmsg;

   if (!opt.option().size()) {
      log << DbPrintLvl::Error
          << "TREE_AUTO_FLUSH database option requires TTree name in option parameter"
          << DbPrint::endmsg;
      return Error;
   } 
   string treeName = opt.option();
   int val=0;
   DbStatus sc = opt._getValue(val);
   if( sc.isSuccess() )  {
      log << DbPrintLvl::Debug << "Demand to set AUTO_FLUSH for TTree: " << treeName
          << " with value: " << val << DbPrint::endmsg;
      map< string, int >::iterator  tafit = m_autoFlushTrees.find( treeName );
      if( tafit != m_autoFlushTrees.end() && tafit->second == val ) {
         log << DbPrintLvl::Debug << " -- AUTO_FLUSH already set, skipping " << DbPrint::endmsg;
         return Success;
      }
      m_autoFlushTrees[treeName] = val;
      // set Tree Fill mode for any branch containers already registered
      for( map< TTree*, ContainerSet_t >::iterator it = m_containersInTree.begin();
           it != m_containersInTree.end(); ++it ) {
         TTree* tree = it->first;
         if( tree->GetName() == treeName ) {              
            ContainerSet_t &contset = it->second;
            // already some branch containers created, set TTree AUTO_FLUSH
            // and mark them for TTree Fill mode
            log << DbPrintLvl::Debug << "Setting AUTO_FLUSH for TTree: "
                << treeName << " to value: " << val << DbPrint::endmsg; 
            tree->SetAutoFlush( val );
            for( ContainerSet_t::iterator it = contset.begin(); it != contset.end(); ++it ) {
               log << DbPrintLvl::Debug << "Setting TTree fill mode for container: " << (**it).getName()
                   << DbPrint::endmsg; 
               (**it).setTreeFillMode(true);
            }
            break;
         }
      } 
   } else {
      log << DbPrintLvl::Error << "Could not get AUTO_FLUSH value for TTree: " << treeName
          << DbPrint::endmsg;
   }
   return sc;
}


void RootDatabase::registerBranchContainer(RootTreeContainer* cont)
/* register creation (or opening for update) of a Branch Container.
   If it is located in a TTree with AUTO_FLUSH option, the whole TTree must be Filled
   (instead of the container branch, or the AUTO_FLUSH option will not work).
   TTree::Fill() is done in commit in transAct()
*/
{
   TTree* tree = cont->getTTree();
   // cout << "------ registerBranchContainer: " << cont->getName() << endl;
   map<string,int>::const_iterator flushIt = m_autoFlushTrees.find( tree->GetName() );
   if( !cont->isBranchContainer() ) {
      if( flushIt != m_autoFlushTrees.end() ) {
         // normal container option but set on the DB level
         DbPrint log(cont->getName());
         log << DbPrintLvl::Debug << "Setting AUTO_FLUSH for Container TTree: "
             << tree->GetName() << " to value: " << flushIt->second << DbPrint::endmsg; 
         tree->SetAutoFlush( flushIt->second );
      }
      return;
   }
   // group branch containers from the same TTree
   ContainerSet_t &contset = m_containersInTree[tree];
   contset.insert(cont);
   if( flushIt == m_autoFlushTrees.end() )
      return; // this branch container is not in auto_flushed tree
   if( contset.size() == 1 ) {
      // first container for a given tree -> set the tree option 
      DbPrint log(cont->getName());
      log << DbPrintLvl::Debug << "Setting AUTO_FLUSH for TTree: "
        << tree->GetName() << " to value: " << flushIt->second << DbPrint::endmsg; 
      tree->SetAutoFlush( flushIt->second );
   }
   cont->setTreeFillMode(true);
}


/// Start/Commit/Rollback Database Transaction
DbStatus RootDatabase::transAct(DbTransaction&  refTr )
{
   // process flush to write file
   if( refTr.state() == Transaction::TRANSACT_FLUSH && m_file != nullptr && m_file->IsWritable()) m_file->Write();
   // process commits only
   if( refTr.state() != Transaction::TRANSACT_COMMIT )
      return Success;

   // check all TTrees with branch containers, if they need Filling
   for( map< TTree*, ContainerSet_t >::iterator treeIt = m_containersInTree.begin(),
           mapEnd = m_containersInTree.end(); treeIt != mapEnd; ++treeIt ) {
      ContainerSet_t &containers = treeIt->second;
      if( (*containers.begin())->usingTreeFillMode() ) {
         TTree *tree = treeIt->first;
         // cout << "------- TTree " << tree->GetName() << " - checking containers for commit" << endl;
         int    clean = 0, dirty = 0;
         for( ContainerSet_t::const_iterator cIt = containers.begin(); cIt != containers.end(); ++cIt ) {
            if( (*cIt)->isDirty() ) dirty++;
            else clean++;
         }
         if( !dirty ) continue;   // nothing to write, go to the next TTree
         if( clean == 0 ) {
            // good - all containers were updated.  Fill the TTree and clean dirty status
            int num_bytes = tree->Fill();
            if( num_bytes > 0 ) {
               addByteCount( RootDatabase::WRITE_COUNTER, num_bytes );
               // cout << "-----XXXX---  Filled branch container TTree: " << tree->GetName() << endl;
            } else {
               DbPrint err( m_file->GetName() );
               err << DbPrintLvl::Error << "Write to " <<  tree->GetName() << " failed"
                   << DbPrint::endmsg;
               return Error;
            }
            for( ContainerSet_t::iterator cIt = containers.begin(); cIt != containers.end(); ++cIt ) {
               (*cIt)->clearDirty();
            } 
         } else {
            // error - some containers in this TTree were not updated
            DbPrint err( m_file->GetName() );
            for( ContainerSet_t::const_iterator cIt = containers.begin(); cIt != containers.end(); ++cIt ) {
               if( !(*cIt)->isDirty() ) {
                  err << DbPrintLvl::Error << "Branch Container " <<  (*cIt)->getName()
                      << " was not filled in this transaction. This is required by TREE_AUTO_FLUSH option."
                      << DbPrint::endmsg;
               }
            }
            return Error;
         }
      }
   }
   return Success;
}

     
