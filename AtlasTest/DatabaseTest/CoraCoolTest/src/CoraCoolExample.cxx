/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <exception>
#include <math.h>
#include <vector>
#include <string>
#include <sstream>
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoralBase/Blob.h"

// declarations for CoraCoolExample class

class CoraCoolExample {
 public:
  CoraCoolExample(const std::string& dbstring, const std::string& options,
		  const std::string& folder,const int nfolder,
		  const int nobj, const int nchan, const int niov,
		  const int iov1,const int iovinc);
  int execute();
  bool write();
  bool read();
  bool checkAPI();
  coral::AttributeList makeAttrList(const CoraCoolFolderPtr fld,
				    const int obj, const int chan, 
				    const int iov);

 private:
  const std::string m_dbstring;
  std::vector<std::string> m_foldername;
  const int m_nobj;
  const int m_nchan;
  const int m_niov;
  const int m_iov1;
  const int m_iovinc;
  bool m_read;
  bool m_write;
  bool m_check;
  bool m_delete;
  bool m_blob;
  bool m_writeref;
  bool m_writeadj;
  bool m_readonly;
  bool m_bulk;
  std::vector<std::string> m_coraltable;

  CoraCoolDatabasePtr m_coradb;
  cool::IDatabasePtr m_cooldb;

  cool::RecordSpecification payloadspec;
  cool::RecordSpecification fkspec;
  int m_blobsize;
};

CoraCoolExample::CoraCoolExample(const std::string& dbstring, 
				 const std::string& options,
				 const std::string& folder,
				 const int nfolder,
				 const int nobj, const int nchan, 
				 const int niov,
				 const int iov1, const int iovinc) :
  m_dbstring(dbstring),
  m_nobj(nobj), m_nchan(nchan), m_niov(niov),m_iov1(iov1),m_iovinc(iovinc) {

  std::cout << "Option string: " << options << std::endl;
  std::cout << "Objects per channel=" << m_nobj << " Channels=" << m_nchan
	    << " IOVs=" << m_niov << std::endl;
  std::cout << "IOVstart=" << m_iov1 << " inc " << m_iovinc << std::endl;

  // define folders
  m_foldername.clear();
  if (nfolder<2) {
    m_foldername.push_back(folder);
  } else if (nfolder<1000) {
    std::cout << "Defining multiple folders" << std::endl;
    for (int i=1;i<=nfolder;++i) {
      std::ostringstream o;
      o << folder << "_" << i;
      std::cout << "Foldername " << o.str() << std::endl;
      m_foldername.push_back(o.str());
    }
  } else {
    std::cout << "Attempt to define too many folders " << std::endl;
  }
  //  parse options
  m_read=(options.find('r')!=std::string::npos);
  m_write=(options.find('w')!=std::string::npos);
  m_check=(options.find('c')!=std::string::npos);
  m_delete=(options.find('d')!=std::string::npos);
  m_blob=(options.find('b')!=std::string::npos);
  m_writeref=(options.find('a')!=std::string::npos);
  m_writeadj=(options.find('p')!=std::string::npos);
  m_bulk=(options.find('j')!=std::string::npos);
  m_readonly=(!(m_write || m_check));

  if (m_writeref) std::cout << 
       "Second reference will be included for each object" << std::endl;
  if (m_writeadj) std::cout <<
        "Additional rows will be added for each object" << std::endl;
  if (m_bulk) std::cout << "Writing will use bulk updates" << std::endl;

  // print dbstring without password if present
  const unsigned int idx=m_dbstring.find("password");
  std::cout << "Initialise database for " << m_dbstring.substr(0,idx) << 
    " readonly " << m_readonly << std::endl;

  // access the CoraCool database service
  // this ensures readonly connections will be shared
  // the SEAL context is created by the CoraCoolDatabaseSvcFactory
  // which takes care of loading all the required libraries

  CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();

  std::cout << "Done the CoraCool initialisation" << std::endl;

  // open the CORAL database connection which implicitly opens
  // the corresponding COOL connection
  // you can also pass in the COOL connection explicitly
  std::cout << "Opening CORAL database" << std::endl;
  try {
    m_coradb=corasvc.openDatabase(m_dbstring,m_readonly);
    std::cout << "Database connections open OK" << std::endl;
  }
  catch (std::exception&e) {
    std::cout << "Problem opening CORAL database: " << e.what() << std::endl;
  }
  std::cout << "Done the database opening" << std::endl;

  // list the COOL folders found in the database
  cool::IDatabasePtr cooldb=m_coradb->coolDatabase();
  const std::vector<std::string>& folders=cooldb->listAllNodes();
  std::cout << "COOL database has " << folders.size() << " folders defined"
	    << std::endl;
  for (std::vector<std::string>::const_iterator itr=folders.begin();
       itr!=folders.end();++itr) std::cout << *itr << std::endl;

  // create the AttributeListSpecification for the payload
  // primary / foreign keys
  payloadspec.extend("PrimKey",cool::StorageType::Int32);
  payloadspec.extend("ForeignKey",cool::StorageType::Int32);
  // integers to check the data integrity on readback - keep details of the
  // object origin
  payloadspec.extend("IntObj",cool::StorageType::Int32);
  payloadspec.extend("IntChan",cool::StorageType::Int32);
  payloadspec.extend("IntIOV",cool::StorageType::Int32);
  // other data members
  payloadspec.extend("UInt1",cool::StorageType::UInt32);
  payloadspec.extend("Float1",cool::StorageType::Float);
  payloadspec.extend("Double1",cool::StorageType::Double);
  payloadspec.extend("LongLong1",cool::StorageType::Int64);
  payloadspec.extend("ULongLong1",cool::StorageType::UInt63);
  payloadspec.extend("String1",cool::StorageType::String4k);
  // optional declaration of CORAL Blob type object
  if (m_blob) {
    payloadspec.extend("Blob1",cool::StorageType::Blob64k);
    m_blobsize=1000;
    std::cout << "Including blob of size " << m_blobsize << " in data " << 
      std::endl;
  }
  // create COOL FK spec
  fkspec.extend("CoolKey",cool::StorageType::Int32);

  // set payload table names from foldername leaf name
  m_coraltable.clear();
  for (std::vector<std::string>::const_iterator itr=m_foldername.begin();
       itr!=m_foldername.end();++itr) {
    std::string::size_type iofs=itr->rfind("/");
    if (iofs!=std::string::npos) {
      m_coraltable.push_back(itr->substr(iofs+1));
    } else {
      m_coraltable.push_back(*itr);
    }
  }
}

coral::AttributeList CoraCoolExample::makeAttrList(const CoraCoolFolderPtr fld,
						   const int obj, 
				   const int chan, const int iov) {
  // create a payload object according to the specification generated
  // previously 
  coral::AttributeList data=fld->emptyAttrList();
  // setup iov, modulo number of IOVs generated (to deal with referenced objs)
  int iov2=iov;
  if (iov2>=m_niov*m_iovinc+m_iov1) iov2-=m_niov*m_iovinc;
  // set keys to zero - will be overwritten
  data["PrimKey"].data<int>()=0;
  data["ForeignKey"].data<int>()=0;
  // set next 3 depending on object identity, so can check on read back
  data["IntObj"].data<int>()=obj;
  data["IntChan"].data<int>()=chan;
  data["IntIOV"].data<int>()=iov2;
  // other data
  data["Float1"].data<float>()=0.5+obj+chan+iov2;
  data["Double1"].data<double>()=0.6+obj+chan+iov2;
  data["UInt1"].data<unsigned int>()=iov2+chan+obj;
  // use SEAL platform independent-types to access 64 bit ints
  // works on 32 and 64 bit machines
  data["LongLong1"].data<cool::Int64>()=iov2*100000+chan*1000+obj;
  data["ULongLong1"].data<cool::UInt63>()=iov2+chan+2*obj;
  data["String1"].data<std::string>()="A test string of data";
  // setting up data for a blob object
  if (m_blob) {
    coral::Blob& blob=data["Blob1"].data<coral::Blob>();
    blob.resize(m_blobsize);
    unsigned char* p=static_cast<unsigned char*>(blob.startingAddress());
    for (int i=0;i<m_blobsize;++i,++p) *p=(i+iov2+chan+obj) % 256;
  }
  return data;
}

int CoraCoolExample::execute() {
  int retcode=0;
  if (m_write) {
    if (write()==false) {
      std::cout << "CoraCoolExample FAILURE from write" << std::endl;
      retcode+=1;
    }
  }
  if (m_read) {
    if (read()==false) {
      std::cout << "CoraCoolExample FAILURE from read" << std::endl;
      retcode+=2;
    }
  }
  if (m_check) {
    if (checkAPI()==false) retcode+=4;
  }
  return retcode;
}

bool CoraCoolExample::write() {
  CoraCoolFolderPtr folder;
  // loop over all folders
  std::vector<std::string>::const_iterator ctitr=m_coraltable.begin();
  for (std::vector<std::string>::const_iterator fitr=m_foldername.begin();
       fitr!=m_foldername.end();++fitr,++ctitr) {
    // check if previous attempt needs to be deleted
    // does the folder exist already?
    if (m_delete && m_coradb->existsFolder(*fitr)) {
      try {
        std::cout << "Deleting existing folder " << *fitr << std::endl;
          m_coradb->deleteFolder(*fitr);
        std::cout << "Folder deleted" << std::endl;
      }
      catch (cool::Exception& e) {
        std::cout << "Problem deleting folder: " << e.what() << std::endl;
        return false;
      }
    }
    if (!m_coradb->existsFolder(*fitr)) {
      // create the test folder
      std::cout << "Creating folder with name " << *fitr << std::endl;
      // description string to allow folder to be read in Athena
      std::string desc="<timeStamp>run-event</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>CondAttrListVec</typeName>";
      try {
        folder=m_coradb->createFolder(
			 *fitr,*ctitr,fkspec,payloadspec,
			 "ForeignKey","PrimKey",desc,
			 cool::FolderVersioning::SINGLE_VERSION,true);
        std::cout << "Folder " << *fitr << " created" << std::endl;
      }
      catch (cool::Exception& e) {
        std::cout << "Problem creating folder: " << e.what() << std::endl;
        return false;
      }
    } else {
      folder=m_coradb->getFolder(*fitr);
    }

    // now write data
    int nobj=0;
    for (int iiov=0;iiov<m_niov;++iiov) {
      // bulk operation is inside loop to avoid problems with SV multichannel
      // insert
      if (m_bulk) folder->setupStorageBuffer();
      int iovx=iiov*m_iovinc+m_iov1;
      for (int ichan=0;ichan<m_nchan;++ichan) {
        // prepare data
        std::vector<coral::AttributeList> datavec;
        for (int iobj=0;iobj<m_nobj;++iobj) {
          coral::AttributeList adata=makeAttrList(folder,iobj,ichan,iovx);
  	  datavec.push_back(adata);
        }
        try {
          int fkey=folder->storeObject(iovx,iovx+m_iovinc,datavec.begin(),
				       datavec.end(),ichan);
	  ++nobj;
	  if (m_writeadj) {
	    // add aditional rows
	    std::vector<coral::AttributeList> datavec2;
	    for (int iobj=0;iobj<m_nobj;++iobj) {
              coral::AttributeList adata=
		makeAttrList(folder,iobj+m_nobj,ichan,iovx);
	      adata["ForeignKey"].data<int>()=fkey;
  	      datavec2.push_back(adata);
	    }
	    folder->addPayload(datavec2.begin(),datavec2.end());
	  }
	  if (m_writeref) {
	    // add additional IOV for later run, also referencing this data
	    int iov2=iovx+m_iovinc*m_niov;
	    folder->referenceObject(iov2,iov2+m_iovinc,fkey,ichan);
	  }
	}
        catch (cool::Exception& e) {
  	  std::cout << "Problem writing data: " << e.what() << std::endl;
	  return false;
	}
      }
      if (m_bulk) folder->flushStorageBuffer();
    }
    std::cout << "Written " << nobj << " objects to folder " << *fitr << 
      std::endl;
  }
  return true;
}

bool CoraCoolExample::read() {
  // loop over folders
  int nerr=0;
  for (std::vector<std::string>::const_iterator fitr=m_foldername.begin();
       fitr!=m_foldername.end();++fitr) {
    std::cout << "Read back data from folder " << *fitr << std::endl;
    CoraCoolFolderPtr folder;
    try {
      folder=m_coradb->getFolder(*fitr);
    } 
    catch (cool::Exception& e) {
      std::cout << "Exception during getFolder: " << e.what() << std::endl;
      return false;
    }
    int nobj=0;
    // printout specification of folder
    const cool::IRecordSpecification& spec=
      folder->payloadSpecification();
    for (unsigned int i=0;i<spec.size();++i) {
      std::cout << "Payload column: " << spec[i].name() << " type: " << 
        spec[i].storageType().name() << std::endl;
    }
    int nrep=1;
    if (m_writeref) nrep=2;
    for (int iiov=0;iiov<m_niov*nrep;++iiov) {
      int iovx=iiov*m_iovinc+m_iov1;
      // bulk read at a point in time - get all channels valid at this time
      CoraCoolObjectIterPtr itr=folder->browseObjects(iovx,cool::ChannelSelection::all());
      while (itr->hasNext()) {
        CoraCoolObjectPtr obj=itr->next();
        // check the returned object against what was stored in the write method
        int iobj=0;
        for (CoraCoolObject::const_iterator payitr=obj->begin();
	   payitr!=obj->end();++payitr,++iobj) {
          // construct the expected object to compare
	  int iobj2=(*payitr)["IntObj"].data<int>();
          coral::AttributeList expect=makeAttrList(folder,iobj2,
						 obj->channelId(),iovx);
	  // check all fields except the first two (primary and foreign keys)
	  // which are reassigned by CoraCool
	  bool match=(expect.size()==payitr->size() && expect.size()>2);
	  if (match) {
  	    for (unsigned int i=2;i<expect.size();++i) {
	      std::string dtype=expect[i].specification().typeName();
	      bool failmatch=false;
	      if (dtype=="int" || dtype=="unsigned int" || dtype=="long long" ||
		dtype=="std::string") {
	        failmatch=(expect[i]!=(*payitr)[i]);
	      } else if (dtype=="float") {
	        failmatch=(fabsf(expect[i].data<float>()-
			      (*payitr)[i].data<float>())>1.E-3);
	      } else if (dtype=="double") {
	        failmatch=(fabs(expect[i].data<double>()-
			      (*payitr)[i].data<double>())>1.E-3);
	      } else if (dtype=="blob") {
	        // coral defines comparison operator for blobs internally
	        failmatch=(expect[i]!=(*payitr)[i]);
	      }
	      if (failmatch) {
	        match=false;
	        std::cout << expect[i].specification().name() << " does not match" << std::endl;
	      }
	    }
	  }
	  if (!match) {
	    std::cout << "Objects do not match for object/channel/iov "
		    << iobj << "/" << obj->channelId() << "/" << iiov <<
	      std::endl;
	    std::cout << "Read from DB: ";
	    (*payitr).toOutputStream(std::cout);
	    std::cout << std::endl << "Expected object: ";
	    expect.toOutputStream(std::cout);
	    std::cout << std::endl;
	    ++nerr;
 	  }
        }
        ++nobj;
      }
    }
    std::cout << "Read a total of " << nobj << " objects from CoraCool folder "
  << *fitr << " with " << nerr << " errors" << std::endl;
  }
  return (nerr==0);
}

bool CoraCoolExample::checkAPI() {
  // this routine is suppposed to check all calls of the CoraCool API
  std::cout << "Check API not implemented yet" << std::endl;
  return true;
}

int main (int argc, const char* argv[]) {
  if (argc<2) {
    std::cout << "Syntax: CoraCoolExample.exe <dbConnection> { <options> ...}"
	      << std::endl;
    std::cout << "See package mainpage.h for more information" << std::endl;
    return 1;
  }
  std::string options="rw";
  if (argc>2) options=argv[2];
  std::string folder="/CoraCool/Test1";
  if (argc>3) folder=argv[3];
  int nfolder=1;
  if (argc>4) nfolder=atoi(argv[4]);
  int nobj=20;
  if (argc>5) nobj=atoi(argv[5]);
  int nchan=10;
  if (argc>6) nchan=atoi(argv[6]);
  int niov=5;
  if (argc>7) niov=atoi(argv[7]);
  int iov1=0;
  if (argc>8) iov1=atoi(argv[8]);
  int iovinc=1;
  if (argc>9) iovinc=atoi(argv[9]);
  int retcode=0;
  try {
    CoraCoolExample myexample(argv[1],options,folder,nfolder,
			      nobj,nchan,niov,iov1,iovinc);
    retcode=myexample.execute();
  }
  catch (std::exception& e) {
    std::cout << "Exception thrown from CoraCoolExample: " << e.what() 
	      << std::endl;
    retcode=16;
  }
  if (retcode==0) {
    std::cout << "CoraCoolExample SUCCESS" << std::endl;
  } else {
    std::cout << "CoraCoolExample FAILURE with return code " <<
    retcode << std::endl;
  }
  return retcode;
}

