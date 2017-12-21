/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCoolRecWriter.cxx
// Program to write test data load to COOL database for Athena reconstruction
// Richard Hawkings, started 19/1/06
// Compiles in offline cmt framework to binary executable, needs offline env

#include<vector>
#include<string>
#include<iostream>
#include <fstream>
#include <sstream>

//POOL includes for POOL object write
#include "PersistentDataModel/Placement.h"
#include "PersistentDataModel/Token.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/IPersistencySvc.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ITransaction.h"
#include "StorageSvc/DbType.h"
#include "StorageSvc/DbReflex.h"

#include "FileCatalog/IFileCatalog.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/Record.h"
#include "AthenaDBTestRec/TestCoolRecPoolData.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"

#include "TestCoolRecWriter.h"

TestCoolRecWriter::TestCoolRecWriter(const std::string& cooldb, const int mode,
				     const std::string& configfile,
                    const int nrun, const int run0, const int nevent, 
				     const int time0, const int timerun) :
  m_coolstr(cooldb), m_mode(mode), m_configfile(configfile),
  m_nrun(nrun), m_run0(run0), m_nevent(nevent), 
  m_time0(time0), m_timerun(timerun), m_poolmode(0),m_poolstem("pool"),
  m_tagsuffix("tag1"),
  m_usepool(false),
  m_usecoracool(false),
  m_poolcat(NULL),
  m_persistencySvc(NULL) {
  std::cout << "Starting TestCoolRecWriter" << std::endl 
	    << "Cool database: " << m_coolstr << std::endl
	    << "Mode: " << m_mode << std::endl
	    << "Config file: " << m_configfile << std::endl;
}

int TestCoolRecWriter::execute() {
  // read configuration
  readConfig();
  // now execute
  if (m_mode==1) return fillDB();
  if (m_mode==2) return genJobOpts("CoolReadConfig.py");
  return 1;
}

bool TestCoolRecWriter::readConfig() {
  // read lines from config file
  std::cout << "Start to read config from " << m_configfile << std::endl;
  FILE* p_inp=fopen(m_configfile.c_str(),"r");
  if (p_inp==0) {
    std::cout << "ERROR: Cannot open config file" << std::endl;
    return false;
  }
  // set initial database connection
  m_dbconn.clear();
  m_dbconn.push_back(new DBConnection(m_coolstr));
  int idbconn=0;
  DBConnection* dbconn=m_dbconn[0];
  m_folders.clear();
  bool paytable=false;

  char* p_buf=new char[999];
  while (!feof(p_inp)) {
    char* p_line=fgets(p_buf,999,p_inp);
    if (p_line!=NULL) {
    // tokenise input line based on spaces
    std::string buf=std::string(p_line);
    std::vector<std::string> tokens;
    unsigned int iofs1=0;
    while (iofs1<buf.size()) {
      std::string::size_type iofs2=buf.find(" ",iofs1);
      // allow for trailing linefeed
      if (iofs2==std::string::npos) iofs2=buf.size()-1;
      tokens.push_back(buf.substr(iofs1,iofs2-iofs1));
      iofs1=iofs2+1;
    }
    if (tokens.size()>0) {
      // definition of single folder
      if (tokens[0]=="folder") {
	if (tokens.size()<8) {
	  std::cout << "Folder definition syntax: " << std::endl << 
     "folder <path> <nchan> <ptime> <ptype> <ncolumns> <size> <period>"
		    << std::endl;

	} else {
	  // define single folder
	  std::cout << "Define folder " << tokens[1] << std::endl;
          m_folders.push_back(new FolderInfo(tokens[1],dbconn,idbconn,
	     atoi(tokens[2].c_str()),
             string2ptime(tokens[3]),string2ptype(tokens[4]),
	     atoi(tokens[5].c_str()),atoi(tokens[6].c_str()),
					 atoi(tokens[7].c_str()),
					 m_tagsuffix,paytable));
	}			      
      } else if (tokens[0]=="multi") {
	if (tokens.size()<9) {
	  std::cout << "Multi-Folder definition syntax: " << std::endl << 
     "multi <n> <path> <nchan> <ptime> <ptype> <ncolumns> <size> <period>"
		    << std::endl;

	} else {
	  // define multiple folder
	  int nfold=atoi(tokens[1].c_str());
	  std::cout << "Define " << nfold << " folders based on  " 
            << tokens[2] << std::endl;
          for (int ifold=0;ifold<nfold;++ifold) {
            std::ostringstream strs;
            strs << tokens[2] << "_" << ifold;
            m_folders.push_back(new FolderInfo(strs.str(),dbconn,idbconn,
	     atoi(tokens[3].c_str()),
             string2ptime(tokens[4]),string2ptype(tokens[5]),
	     atoi(tokens[6].c_str()),atoi(tokens[7].c_str()),
					 atoi(tokens[8].c_str()),m_tagsuffix));
          }
	}			      
      } else if (tokens[0]=="schema") {
	if (tokens.size()!=2) {
	  std::cout << "Schema definition syntax:" << std::endl <<
	    "schema <schema_name>" << std::endl;
	} else {
	  std::cout << "Define new schema : " << tokens[1]
		    << std::endl;
	  dbconn=new DBConnection(m_coolstr,tokens[1]);
          m_dbconn.push_back(dbconn);
	  ++idbconn;
	}
      } else if (tokens[0]=="poolfile") {
	if (tokens.size()!=3) {
	  std::cout << "Poolfile definition syntax: " << std::endl <<
	    "poolfile <mode> <file_stem>" << std::endl;
	} else {
	  m_poolmode=atoi(tokens[1].c_str());
	  m_poolstem=tokens[2];
	}
      } else if (tokens[0]=="tag") {
	if (tokens.size()!=2) {
	  std::cout << "Tag definition syntax: " << std::endl <<
	    "tag <tagsiffix>" << std::endl;
	} else {
	  m_tagsuffix=tokens[1];
	}
      } else if (tokens[0]=="payloadtable") {
	if (tokens.size()!=2) {
	  std::cout << "Payloadtable syntax: " <<std::endl <<
	    "payloadtable 0|1" << std::endl;
	} else {
	  paytable=(atoi(tokens[1].c_str())!=0);
	}
      } else if (tokens[0]!="#" && tokens[0]!="//") {
	std::cout << "Bad input line: " << buf << std::endl;
      }
    }
  }
  }
  std::cout << "Defined total of " << m_folders.size() << " folders from input"
	    << std::endl;
  // check if any folders are POOL references
  for (std::vector<FolderInfo*>::const_iterator i=m_folders.begin();
       i!=m_folders.end();++i) {
    if ((*i)->ptype()==FolderInfo::PoolRef) m_usepool=true;
    if ((*i)->ptype()==FolderInfo::CoraCool) m_usecoracool=true;
  }
  fclose(p_inp);
  delete [] p_buf;
  return true;
}

int TestCoolRecWriter::fillDB() {
  // setup POOL if needed
  if (m_usepool) setupPool();
  if (checkFolders()!=0) return 1;
  
  std::cout << "Starting write loop with following paramters" << std::endl <<
    "Number of runs: " << m_nrun << " first run: " << m_run0 << 
   " events/run: " << m_nevent << std::endl << "Initial time: " << m_time0 <<
    " time per run: " <<  m_timerun << std::endl;
  bool allIOV=false;
  if (m_nrun==0) {
    allIOV=true;
    m_run0=0;
    m_time0=0;
    m_nrun=1;
    std::cout << "Writing single IOV for all runs" << std::endl;
  }
  Placement* placement=0;

  for (int irun=m_run0; irun<m_run0+m_nrun; ++irun) {
    std::cout << "Fill database for run " << irun << 
    " events [0," << m_nevent-1 << "] time {" << (irun-m_run0)*m_timerun << ","
	      << (irun-m_run0+1)*m_timerun << "}" << std::endl;

    // folder sequence number for POOL files
    unsigned int fseq=0;
    for (std::vector<FolderInfo*>::const_iterator ifold=m_folders.begin();
	 ifold!=m_folders.end();++ifold,++fseq) {
      const FolderInfo* folderi=*ifold;
      std::cout << "Run " << irun << " folder " << folderi->name() <<
	std::endl;

      const FolderInfo::PayloadTime ptime=folderi->ptime();
      const bool coracool=(folderi->ptype()==FolderInfo::CoraCool);
      const bool coolvec=(folderi->ptype()==FolderInfo::CoolVector);

      // access the database
      cool::IDatabasePtr cooldb=folderi->dbConnection()->open();
      cool::IFolderPtr folder;
      CoraCoolFolderPtr corafolder;
      if (coracool) {
	CoraCoolDatabasePtr coradb=folderi->dbConnection()->coradbptr();
	corafolder=coradb->getFolder(folderi->name());
      } else {
        folder=cooldb->getFolder(folderi->name());
      }

      const std::string tag=folderi->tag();
      bool utonly=(tag!="");
      int nobj=0;
      // setup buffer to receive updates
      if (coracool) {
	corafolder->setupStorageBuffer();
      } else {
        folder->setupStorageBuffer();
      }
      // setup POOL if needed
      if (folderi->ptype()==FolderInfo::PoolRef && folderi->poolplace()==0) {
	if (placement==0 || (m_poolmode & 1)) {
  	  std::cout << "Setup POOL for folder " << folderi->name() << 
	    std::endl;
    	  placement=new Placement;
  	  std::ostringstream sfile;
	  sfile << m_poolstem;
          if (m_poolmode & 1) sfile << fseq;
	  if (m_poolmode & 2) sfile << "_run" << irun;
          sfile << ".root";
	  std::cout << "Setup new POOL file: " << sfile.str() << std::endl;
 	  placement->setFileName(sfile.str());
	  placement->setTechnology(pool::ROOTTREE_StorageType.type());
	  placement->setContainerName("POOLContainer_TestCoolRecPoolData");
	}
	folderi->setpoolplace(placement);
      } 

      // loop over all channels
      for (int ichan=0;ichan<folderi->nchan();++ichan) {
	// different time structures depending on folder type
	if (ptime==FolderInfo::DCSC || ptime==FolderInfo::DCSP) {
	  // loop in time
	  int itime=(irun-m_run0)*m_timerun+m_time0;
	  int itime1=itime+m_timerun;
	  while (itime<itime1) {
	    // convert time to nanoseconds
	    cool::ValidityKey since=itime*1000000000LL;
	    try {
	      if (coracool) {
		std::vector<coral::AttributeList> payloads;
		setCoraPayload(folderi,corafolder->payloadSpecification(),
			       payloads,irun,ichan);
		corafolder->storeObject(since,cool::ValidityKeyMax,
					payloads.begin(),payloads.end(),
					ichan,tag,utonly);
	    } else if (coolvec) {
	      std::vector<cool::IRecordPtr> payloads;
	      setCoolVecPayload(folderi,folder->payloadSpecification(),
				payloads,irun,ichan);
	      folder->storeObject(since,cool::ValidityKeyMax,payloads,
				  ichan,tag,utonly);
	      } else {
	        cool::Record payload(folder->payloadSpecification());
  	        setPayload(folderi,payload,irun,ichan);
                folder->storeObject(since,cool::ValidityKeyMax,payload,
				  ichan,tag,utonly);
	      }
 	      ++nobj;
	    } catch (std::exception& e) {
	      std::cout << "Exception thrown from storeObject: " << e.what()
			<< std::endl;
	    }
	    int itinc=folderi->period();
	    if (ptime==FolderInfo::DCSP) {
	      // uniform (0 to itinc*2) random time increment
	      itime+=1+static_cast<int>(2.*static_cast<float>(itinc)*rand()/
                  (RAND_MAX+1.0));
	    } else {
	      // constant time increment
    	      itime+=itinc;
	    }
	  }
	} else if (ptime==FolderInfo::RUNC || ptime==FolderInfo::RUNP) {
	  // loop over events within run
	  int ievent=0;
	  int ieventinc;
	  // period interpreted as number of events per update (+ve)
	  // or number of runs per update, only done on some runs (-ve)
	  if (folderi->period()>0) {
  	    ieventinc=m_nevent/folderi->period();
	  } else {
	    ieventinc=-m_nevent*folderi->period();
	    // if not run zero of group, skip updating
	    if ((irun % folderi->period())!=0) ievent+=m_nevent;
	  }
	  while (ievent<m_nevent) {
	    cool::ValidityKey since=(static_cast<unsigned long long>(irun) 
                  << 32)+ievent;
	    cool::ValidityKey till;
	    if (folderi->period()>0) {
	      till=(static_cast<unsigned long long>(irun) 
				    << 32)+ievent+ieventinc;
	    } else {
	      till=(static_cast<unsigned long long>(irun-folderi->period())
		    << 32);
	    }
	    if (allIOV) till=cool::ValidityKeyMax;
	    if (coracool) {
	      std::vector<coral::AttributeList> payloads;
	      setCoraPayload(folderi,corafolder->payloadSpecification(),
			     payloads,irun,ichan);
	      corafolder->storeObject(since,till,payloads.begin(),
				      payloads.end(),ichan,tag,utonly);
	    } else if (coolvec) {
	      std::vector<cool::IRecordPtr> payloads;
	      setCoolVecPayload(folderi,folder->payloadSpecification(),
				payloads,irun,ichan);
	      folder->storeObject(since,till,payloads,ichan,tag,utonly);
	    } else {
	      cool::Record payload(folder->payloadSpecification());
	      if (folderi->ptype()==FolderInfo::PoolRef) {
	        setPoolPayload(folderi,payload,irun,ichan);
	      } else {
  	        setPayload(folderi,payload,irun,ichan);
	      }
              folder->storeObject(since,till,payload,ichan,tag,utonly);
	    }
	    ++nobj;
            ievent+=ieventinc;
	  }
	}
      }

      // flush buffer - write updates to COOL
      try {
	if (coracool) {
	  corafolder->flushStorageBuffer();
	} else {
          folder->flushStorageBuffer();
	}
	std::cout << "Stored " << nobj << " objects in " << folderi->nchan() 
		  << " channels" << std::endl;
      }
      catch (std::exception& e) {
	std::cout << "ERROR: Exception thrown from COOL bulk insert: " <<
	  e.what() << std::endl;
      }
    }
    // loop over folders again to finish with any POOL files as needed
    bool first=true;
    for (std::vector<FolderInfo*>::const_iterator ifold=m_folders.begin();
	 ifold!=m_folders.end();++ifold) {
      const FolderInfo* folderi=*ifold;
      if (m_usepool && folderi->poolplace()!=0 && (m_poolmode &2)) {
	if ((m_poolmode & 1) || first) {
	  Placement* placement=folderi->poolplace();
	  delete placement;
	  folderi->setpoolplace(0);
	} else {
	  // folder sharing placement - delete only reference
	  folderi->setpoolplace(0);
	}
      }
      first=false;
    }
  }
  // finalise POOL if needed
  if (m_usepool) finalizePool();
  std::cout << "Finalised POOL" << std::endl;

  // close all database connections
  for (std::vector<DBConnection*>::iterator itr=m_dbconn.begin();
       itr!=m_dbconn.end();++itr) {
    (*itr)->close();
    delete *itr;
  }
  std::cout << "Finished fillDB" << std::endl;
  return 0;
}

void TestCoolRecWriter::setPayload(const FolderInfo* folderi,
				   cool::Record& payload,
				   const int irun,const int ichan,
				   const int iarray) {
  // setup the payload appropriate for this folder (types already set)
  FolderInfo::PayloadType ptype=folderi->ptype();
  for (unsigned int iat=0;iat<payload.size();++iat) {
    if (ptype==FolderInfo::Int) {
      // for integers, first one is run, second, channel, then random
      int ipay;
      if (iat==0) {
	ipay=irun;
      } else if (iat==1) {
	ipay=ichan;
      } else {
	ipay=static_cast<int>(1000.*rand()/RAND_MAX+1);
      }
      payload[iat].setValue(ipay);
    } else if (ptype==FolderInfo::Float) {
      // for floats, first one is run, second, channel, then random
      float fpay;
      if (iat==0) {
	fpay=irun;
      } else if (iat==1) {
	fpay=ichan;
      } else {
	fpay=static_cast<int>(1000.*rand()/RAND_MAX+1);
      }
      payload[iat].setValue(fpay);
    } else if (ptype==FolderInfo::String) {
      // start by writing the run and channel numbers into the string
      // separated by commas
      std::ostringstream strs;
      strs << irun << "," << ichan << ",";
      // generate string of random characters to pad upto required length
      std::string pstring=strs.str();
      int nextra=folderi->size()-pstring.size();
      if (nextra>0) {
        for (int i=0;i<nextra;++i) {
	  int iasc=32+static_cast<int>(90.*rand()/RAND_MAX+1);
          pstring+=static_cast<char>(iasc);
        }
      }
      payload[iat].setValue(pstring);
    } else if (ptype==FolderInfo::Blob) {
      coral::Blob& blob=
        const_cast<coral::Blob&>(payload[iat].data<coral::Blob>());
      unsigned int blobsize=folderi->size();
      blob.resize(blobsize);
      unsigned char* p=static_cast<unsigned char*>(blob.startingAddress());
      int* q=static_cast<int*>(blob.startingAddress());
      // first two 32-bit ints are run and channel number
      *q=irun;
      *(q+1)=ichan;
      p+=8;
      // the rest is random values
      for (int i=8;i<folderi->size();++i,++p)
	*p=(static_cast<int>(256.*rand()/RAND_MAX)) % 256;
    } else if (ptype==FolderInfo::CoraCool || ptype==FolderInfo::CoolVector) {
      // for CoraCool, payload are integers
      // first two are keys to be overwritten (coracool only ), 
      // next are run/chan/array then random
      int ipay;
      if (iat==2) {
	ipay=irun;
      } else if (iat==3) {
	ipay=ichan;
      } else if (iat==4) {
	ipay=iarray;
      } else {
	ipay=static_cast<int>(1000.*rand()/RAND_MAX+1);
      }
      payload[iat].setValue(ipay);
    }
  }
}

void TestCoolRecWriter::setCoraPayload(const FolderInfo* folderi,
				       const cool::IRecordSpecification& rspec,
		      std::vector<coral::AttributeList>& payloads,
				       const int irun,const int ichan) {
  cool::Record payload(rspec);
  unsigned int nobj=folderi->size();
  for (unsigned int i=0;i<nobj;++i) {
    setPayload(folderi,payload,irun,ichan,i);
    payloads.push_back(payload.attributeList());
  }
}

void TestCoolRecWriter::setCoolVecPayload(const FolderInfo* folderi,
		    const cool::IRecordSpecification& rspec,
		    std::vector<cool::IRecordPtr>& payloads,
					 const int irun, const int ichan) {
  unsigned int nobj=folderi->size();
  for (unsigned int i=0;i<nobj;++i) {
    cool::Record* payload=new cool::Record(rspec);
    setPayload(folderi,*payload,irun,ichan,i);
    payloads.push_back(cool::IRecordPtr(payload));
  }
}

int TestCoolRecWriter::genJobOpts(const std::string& file) {
  // generate joboption file for Athena
  std::cout << "Generating Athena joboptions into file: " << file << std::endl;

  std::ofstream* outfile;
  outfile=new std::ofstream(file.c_str());
  if (outfile==0) {
    std::cout << "Problem opening output file" << std::endl;
    return 1;
  }
  *outfile << "# AthenaDBTestRec config generated from input " << m_configfile
	   << std::endl;
  // write list of connections
  for (unsigned int iconn=0;iconn<m_dbconn.size();++iconn)
    *outfile << "CondDBCool_Schema" << iconn << "=\"" << 
     (m_dbconn[iconn])->iovdbStr() << "\"" << std::endl;

  // write list of folders and options
  for (unsigned int ifold=0;ifold<m_folders.size();++ifold) {
    const FolderInfo* folderi=m_folders[ifold];
    *outfile << "svcMgr.IOVDbSvc.Folders+=[CondDBCool_Schema";
    *outfile << folderi->ndbconn() << " + \"";
    *outfile << folderi->name();
    if (folderi->tag()!="") 
      *outfile << " <tag>" << folderi->tag() << "</tag>";
    *outfile << "\"]" << std::endl;
    *outfile << "theTestCoolRecRead.Folders+=[\"" << folderi->name() << "\"]" 
	     << std::endl;
    int iftype=0;
    if (folderi->ptype()==FolderInfo::PoolRef) iftype=1;
    if (folderi->ptype()==FolderInfo::CoraCool) iftype=2;
    if (folderi->ptype()==FolderInfo::CoolVector) iftype=2;
    // for folders spanning more than one run, use ftype to store run period
    // which will be > 1
    if (folderi->period()<-1) iftype=-folderi->period();
    *outfile << "theTestCoolRecRead.FTypes+=[" << iftype << "]" << std::endl;
  }

  outfile->close();
  delete outfile;
  return 0;
}

int TestCoolRecWriter::checkFolders() {
  std::cout << "Checking existance of " << m_folders.size() << " COOL folders"
	    << std::endl;
  for (std::vector<FolderInfo*>::iterator itr=m_folders.begin();
       itr!=m_folders.end();++itr) {
    FolderInfo* folder=*itr;
    cool::IDatabasePtr cooldb=folder->dbConnection()->open();
    const std::string& name=folder->name();
    if (cooldb->existsFolder(name)) {
      std::cout << "Connected to existing folder " << name << std::endl;
    } else {
      std::cout << "Attempting to create folder " << name << std::endl;
      if (!folder->createFolder()) return 1;
    }
  }
  return 0;
}

bool TestCoolRecWriter::setupPool() {
  std::cout << "Setup POOL for referenced object output" << std::endl;
  m_poolcat=new pool::IFileCatalog;
  try {
    m_poolcat->setWriteCatalog("file:PoolFileCatalog.xml");
    m_poolcat->connect();
  }
  catch (std::exception& e) {
    std::cout << "Could not setup POOL catalogues, exception:" << e.what() 
	      << std::endl;
  }
  m_poolcat->start();
  m_persistencySvc = pool::IPersistencySvc::create(*m_poolcat).get();
  pool::DatabaseConnectionPolicy policy;
  policy.setWriteModeForNonExisting(pool::DatabaseConnectionPolicy::CREATE);
  policy.setWriteModeForExisting(pool::DatabaseConnectionPolicy::UPDATE);
  m_persistencySvc->session().setDefaultConnectionPolicy(policy);
  m_persistencySvc->session().transaction().start(pool::ITransaction::UPDATE);
  return true;
}

bool TestCoolRecWriter::finalizePool() {
   m_persistencySvc->session().transaction().commit();
   m_persistencySvc->session().disconnectAll();
   m_poolcat->commit();
   m_poolcat->disconnect();
   delete m_persistencySvc;
   delete m_poolcat;
   return true;
}

void TestCoolRecWriter::setPoolPayload(const FolderInfo* folderi,
		      cool::Record& payload, const int irun,
				       const int ichan) {
   // create object, mark for output
   // note: this could be considered a memory leak, but the objects need to be kept till commit anyhow
   TestCoolRecPoolData *data = new TestCoolRecPoolData(irun,ichan,folderi->name(),folderi->size());
   // fill vector of floats in object
   for( std::vector<float>::iterator it = data->dbegin(); it != data->dend(); ++it) {
      *it=rand()/(float(RAND_MAX)+1);
   }
   Token *tok = m_persistencySvc->registerForWrite(*(folderi->poolplace()), data, pool::DbReflex::forTypeInfo(typeid(TestCoolRecPoolData)) );
   // set payload data for COOL to Pool object reference
   payload["PoolRef"].setValue<std::string>( tok->toString() );
}


int main(int argc, const char* argv[]) {
  if (argc<4) {
    std::cout << "Syntax: TestCoolRecWriter <coolDBconnection> <mode> <config_file> {<nrun> <run0> <nevent> <time0> <timerun>}" << std::endl;
    return 1;
  }
  // run paramter defaults
  int nrun=1;
  int run0=10000;
  int nevent=720000;
  int time0=0;
  int timerun=3600;
  if (argc>4) nrun=atoi(argv[4]);
  if (argc>5) run0=atoi(argv[5]);
  if (argc>6) nevent=atoi(argv[6]);
  if (argc>7) time0=atoi(argv[7]);
  if (argc>8) timerun=atoi(argv[8]);

  TestCoolRecWriter tester(argv[1],atoi(argv[2]),argv[3],
			   nrun,run0,nevent,time0,timerun);
  return tester.execute();
}
