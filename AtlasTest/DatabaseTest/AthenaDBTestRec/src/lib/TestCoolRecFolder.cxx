/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TestCoolRecFolder.cxx
// impl class to hold details of COOL folders in reconstruction tests
// Richard Hawkings, started 3/3/06

#include <math.h>
#include <fstream>
#include <sstream>
#include "AthenaDBTestRec/TestCoolRecFolder.h"
#include "CoralBase/Blob.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaDBTestRec/TestCoolRecPoolDataColl.h"

TestCoolRecFolder::TestCoolRecFolder(const std::string key, int type, 
				     const SG::DataProxy* proxy,
				     const int dumpchan,std::ofstream* dumpf) :
  m_key(key), m_type(type), m_proxy(proxy),
  m_dumpchan(dumpchan),m_dumpf(dumpf),
  m_naccess(0), m_nerror(0), m_nmismatch(0),
  m_nitems(0),m_ncallback(0)
{}

StatusCode TestCoolRecFolder::regCallBack(StoreGateSvc* detstore) {
  if (m_type==1) {
    const DataHandle<TestCoolRecPoolDataColl> poolcol;
    return (detstore->regFcn(&TestCoolRecFolder::callBack,this,poolcol,m_key));
  } else if (m_type==2) {
    const DataHandle<CondAttrListVec> atrvec;
    return (detstore->regFcn(&TestCoolRecFolder::callBack,this,atrvec,m_key));
  } else if (m_type==3) {
    // no callback registered for type 3 (generic)
    return StatusCode::SUCCESS;
  } else {
    // no callback registered for type 3 (generic)
    const DataHandle<CondAttrListCollection> atrcol;
    return (detstore->regFcn(&TestCoolRecFolder::callBack,this,atrcol,m_key));
  }
}

StatusCode TestCoolRecFolder::access(StoreGateSvc* detstore, const int run,
				     const int event,const IOVTime iovkey) {

  // access the data for the folder
  int modrun=run;
  if (m_type>3) {
    // for folders with type > 3, have to modify the run to reflect the
    // run modularity
    modrun=(run-(run % m_type));
  }
  ++m_naccess;
  if (m_type==1) {
    // branch for POOL file objects
    const TestCoolRecPoolDataColl* poolcol;
    if (StatusCode::SUCCESS==
      detstore->retrieve(poolcol,m_key)) {
    // loop over objects in collection, keeping parallel channel iterator
      TestCoolRecPoolDataColl::chan_const_iterator 
	chanitr=poolcol->chan_begin();
      for (DataVector<TestCoolRecPoolData>::const_iterator 
          citr=poolcol->begin();citr!=poolcol->end();++citr,++chanitr) {
        // access the data and check consistency for run,folder and channel
        ++m_nitems;
	if (modrun!=(*citr)->run() || 
	    static_cast<int>(*chanitr)!=(*citr)->chan()
	    || m_key!=(*citr)->folder()) {
	  ++m_nmismatch;
	}
      }
    } else {
      ++m_nerror;
      return StatusCode::FAILURE;
    }
  } else if (m_type==2) {
    // branch for CoraCool/CoolVector objects (both appear as CondAttrListVec)
    const CondAttrListVec* atrvec;
    if (StatusCode::SUCCESS==detstore->retrieve(atrvec,m_key)) {
      // loop over objects in list
      for (CondAttrListVec::const_iterator citr=atrvec->begin();
	   citr!=atrvec->end();++citr) {
	// access the data - put in string
	int chan=citr->first;
	const coral::AttributeList& adata=citr->second;
  	std::ostringstream atrstring;
	adata.toOutputStream(atrstring);
	++m_nitems;
	// check the attribute list - fields 2 and 3 should be run/channel
	// and size should be at least 4 (0/1 are PK/FK)
	if (adata.size()<4 || adata[2].data<int>()!=modrun ||
	    adata[3].data<int>()!=chan) ++m_nmismatch;
      }
      // alternative access via index
      const std::vector<unsigned int>& channels=atrvec->channelIDs();
      std::cout << "ChannelIDs vector for " << m_key << " has " << 
	channels.size() << " elements" << std::endl;
      // loop over the channels and get the attributes
      for (std::vector<unsigned int>::const_iterator ci=channels.begin();
	   ci!=channels.end();++ci) {
	int chan=(*ci);
	bool haschan=atrvec->hasChannel(chan);
	if (haschan) {
	  const std::vector<const coral::AttributeList*>& atrv=
	    atrvec->attrListVec(chan);
	  std::cout << "Check channel " << chan << " has info " << haschan <<
	  " vecsize " << atrv.size() << std::endl;
	  // dump attributelist 0 for first two channels
	  if (chan<2) {
	    const coral::AttributeList& atrlist=*(atrv[0]);
	    std::cout << "Dumping AtrList for channel " << chan;
	    atrlist.toOutputStream(std::cout);
	    std::cout << std::endl;
	  }
	} else {
	  std::cout << "Check channel " << chan << " has no info" << std::endl;
	}
      }
    } else {
      ++m_nerror;
      return StatusCode::FAILURE;
    }
  } else if (m_type==3) {
    // use DataProxy for generic folder without knowing type
    if (m_proxy) {
      try {
	(const_cast<SG::DataProxy*>(m_proxy))->accessData();
	++m_nitems;
      }
      catch (std::exception& e) {
	++m_nerror;
      }
    } else {
      ++m_nerror;
    }
  } else {
    // branch for basic CondAttrListCollection
    const CondAttrListCollection* atrlistcol;
    if (StatusCode::SUCCESS==
      detstore->retrieve(atrlistcol,m_key)) {
      // loop over objects in collection
      int nobj=0;
      for (CondAttrListCollection::const_iterator citr=atrlistcol->begin();
           citr!=atrlistcol->end();++citr) {
        // access the data - put into a string
  	std::ostringstream atrstring;
  	(*citr).second.toOutputStream(atrstring);
	++m_nitems;
	// loop over the attributes to check possible mismatches
	// first two are checked in case of int or float
	// attr 0=run, attr 1=channel number
	int natr=citr->second.size();
	if (natr>2) natr=2;
	for (int icol=0;icol<natr;++icol) {
	  int target=modrun;
	  if (icol==1) target=citr->first;
	  const coral::Attribute& atr=(*citr).second[icol];
	  if ((atr.specification().typeName()=="int" && 
	       atr.data<int>()!=target) || 
	      (atr.specification().typeName()=="float" &&
	       fabsf(atr.data<float>()-target)>1.E-3)) ++m_nmismatch;
          if ((atr.specification().typeName()=="string" && icol==0)) {
	    // for first string, check begins with run,chan, sep by commas
	    std::string::size_type iofs1,iofs2;
	    const std::string& sdata=atr.data<std::string>();
	    iofs1=sdata.find(",");
	    iofs2=sdata.find(",",iofs1+1);
	    if (iofs1!=std::string::npos && iofs2!=std::string::npos) {
	      // correctly formed string, check numbers
	      if (atoi(sdata.substr(0,iofs1).c_str())!=modrun || 
		  atoi(sdata.substr(iofs1+1,iofs2-iofs1-1).c_str())!=
		  static_cast<int>(citr->first))
		++m_nmismatch;
	    } else {
	      // malformed string - counts as a mismatch
	      ++m_nmismatch;
	    }
	  }
	  if (atr.specification().typeName()=="blob") {
	    const coral::Blob& blob=atr.data<coral::Blob>();
	    const int* p=static_cast<const int*>
	      (blob.startingAddress());
	    if (*p!=modrun || *(p+1)!=static_cast<int>(citr->first)) 
	      ++m_nmismatch;
	  }
	}
	// dump information into file
	if (nobj<abs(m_dumpchan)) {
	  // find the IOV of this element
	  unsigned int channum=citr->first;
	  CondAttrListCollection::iov_const_iterator 
	    ciov=atrlistcol->chanIOVPair(channum);
	  const IOVRange& iovrange=ciov->second;
	  const IOVTime& start=iovrange.start();
	  const IOVTime& stop=iovrange.stop();
	  bool isTS=start.isTimestamp();
	  std::string checkstr="";
	  if (start>iovkey || stop<=iovkey) {
	    checkstr=" BAD";
	    ++m_nerror;
	  }
	  *m_dumpf << m_key << " " << event << " " << channum << " " << 
	    iovprint(iovkey,isTS) << " [" << iovprint(start,isTS) << "," 
		 << iovprint(stop,isTS) << "]" << checkstr;
	  // only dump payload if number of channels is set negative
	  if (m_dumpchan<0) *m_dumpf << " : " << atrstring.str();
	  *m_dumpf << std::endl;
	}
	++nobj;
      }
    } else {
      ++m_nerror;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TestCoolRecFolder::callBack( IOVSVC_CALLBACK_ARGS_P( /* I */ ,keys) ) {
  // use callbacks to count the number of times the object actually changes
  for (std::list<std::string>::const_iterator itr=keys.begin();
       itr!=keys.end(); ++itr) {
    if (*itr==m_key) {
      ++m_ncallback; 
      if (m_dumpchan!=0) *m_dumpf << "Callback " << m_key << std::endl;
    }
  }
  return StatusCode::SUCCESS;
}

std::string TestCoolRecFolder::iovprint(const IOVTime& time,const bool isTS) {
  // return the IOVTime as run:event or TS
  std::ostringstream out;
  if (isTS) {
    out << time.timestamp();
  } else {
    out << time.run() << ":" << time.event();
  }
  return out.str();
}
