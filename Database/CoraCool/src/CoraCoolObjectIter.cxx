/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CoraCoolObjectIter.cxx
// Implementation for CoraCoolObjectIter
// Richard Hawkings, started 10/2006

#include <sstream>
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoraCool/CoraCoolException.h"
#include "CoraCool/CoraCoolObjectIter.h"

// performance tuning parameters
// maximum number of ranges and query terms to use for CORAL query
#define QB_SIZE 16
#define QB_TERMS 20
// buffer length for number of COOL channels and DB backend rowcache size
// tuned to be slightly larger than number of SCT modules
#define OI_BUFLEN 4200
#define OI_ROWCACHE 4200

CoraCoolObjectIter::CoraCoolObjectIter(CoraCoolFolder* coracoolfolder,
				       cool::IObjectIteratorPtr coolitr) :
  m_buflen(OI_BUFLEN),m_inbuf(0),m_nextobj(0),
  m_refValid(false),m_allDone(false),
  m_folder(coracoolfolder),m_coolitr(coolitr) {
  m_data.resize(m_buflen);
  // assemble an AttributeListSpecification to share with the data objects
  const cool::RecordSpecification recspec=m_folder->payloadSpecification();
  const coral::AttributeList speclist ATLAS_THREAD_SAFE = cool::Record(recspec).attributeList(); // Not shared, ok
  m_spec=new coral::AttributeListSpecification();
  for (coral::AttributeList::const_iterator itr=speclist.begin();
       itr!=speclist.end();++itr) {
    const coral::AttributeSpecification& aspec=itr->specification();
    m_spec->extend(aspec.name(),aspec.typeName());
   }
}

CoraCoolObjectIter::~CoraCoolObjectIter() {
  if (m_spec!=0) m_spec->release();
}

CoraCoolObjectPtr CoraCoolObjectIter::next() {
  if (m_inbuf>m_nextobj) {
    // return data directly from the buffer - just increment counter
    ++m_nextobj;
  } else if (iHasNext()) {
    readDataToBuffer();
  } else {
    throw CoraCoolException("No more data in CoraCoolObjectIter",
			    "CoraCoolObjectIter::next");
  }
  return m_data.at(m_nextobj-1);
}

void  CoraCoolObjectIter::readDataToBuffer(){
   // read some data from the database - initialise buffer to empty
    m_inbuf=0;
    // coloumn name of the FK in the CORAL table
    const std::string& coralkey=m_folder->coralFKey();
    // buffer to build the query
    QueryBuilder qbuf(QB_SIZE);
    coral::AttributeList fktype ATLAS_THREAD_SAFE; // Not shared, ok
    std::string fktypestr;
    // keep map of FKvalue to list of associated COOL channels
    typedef std::vector<unsigned int> KeyVec;
    typedef std::map<long long,KeyVec > KeyMap;
    KeyMap keymap;

    while (m_inbuf<m_buflen && iHasNext() && 
	   qbuf.size()<QB_SIZE && qbuf.terms()<QB_TERMS) {
      // get the COOL object
      const cool::IObject& coolobj=m_coolitr->currentRef();
      // reset so that next call to iHasNext will read the next one
      m_refValid=false;
      // prepare CoraCoolObject to receive the payload data
      CoraCoolObjectPtr data(new CoraCoolObject(coolobj.since(),
		      coolobj.until(),coolobj.channelId(),m_spec));
      m_data[m_inbuf]=data;
      // setup the foreign key reference
      const coral::AttributeList& payload=coolobj.payload().attributeList();
      // remember the type of the FK
      if (m_inbuf==0) {
	fktypestr=payload[0].specification().typeName();
	fktype=payload;
      }
      long long ikeyval=numAttr(fktypestr,payload[0]);
      qbuf.add(ikeyval);
      KeyMap::iterator kdata=keymap.find(ikeyval);
      if (kdata==keymap.end()) {
	// new FK value - create vector of buffer positions for this one
	kdata=keymap.insert(kdata,
	      KeyMap::value_type(ikeyval,KeyVec(8)));
	KeyVec& keyvec=kdata->second;
	keyvec.resize(1);
	keyvec[0]=m_inbuf;
      } else {
	// add to list of buffers using this key
	KeyVec& keyvec=kdata->second;
	keyvec.push_back(m_inbuf);
      }
      ++m_inbuf;
    }
    // setup the query 
    std::string bwhere;
    coral::AttributeList bfkeys ATLAS_THREAD_SAFE; // Not shared, ok
    qbuf.getQuery(bwhere,coralkey,bfkeys,fktype[0].specification());
    // execute the query
    try {
      m_folder->proxy()->transaction().start(true);
      coral::IQuery* query=m_folder->table()->newQuery();
      query->setCondition(bwhere,bfkeys);
      query->setRowCacheSize(OI_ROWCACHE);
      m_folder->setOutputSpec(query);
      coral::ICursor& cursor=query->execute();
      KeyMap::const_iterator kdata=keymap.end();
      long long okeyval=0;
      while (cursor.next()) {
        const coral::AttributeList& res=cursor.currentRow();
	// check which buffer(s) this data is associated with
	// maybe associated with more than one if referenceObject has been used
	// so have to check all - no optimisation possible
	const long long keyval=numAttr(fktypestr,res[coralkey]);
	if (keyval!=okeyval || kdata==keymap.end()) {
  	  kdata=keymap.find(keyval);
	  okeyval=keyval;
	}
	if (kdata!=keymap.end()) {
	  KeyVec keyvec=kdata->second;
	  for (KeyVec::const_iterator kitr=keyvec.begin();
	       kitr!=keyvec.end();++kitr) {
	    m_data[*kitr]->add(res);
	  }
	} else {
	  std::ostringstream skey;
	  skey << "CoraCool key remapping error for key " << keyval;
	  throw CoraCoolException(skey.str(),"CoraCoolObjectIter::next");
	}
      }
      delete query;
      m_folder->proxy()->transaction().commit();
    }
    catch (coral::Exception& e) {
      throw CoraCoolException(e.what(),"CoraCoolObjectIter::next");
    }
    // next read will access next object from buffer (if existing)
    // this invocation will return first object
    m_nextobj=1;
    return;
}

bool CoraCoolObjectIter::hasNext() {
  return (m_inbuf>m_nextobj || iHasNext());
}

bool CoraCoolObjectIter::goToNext() {
  const bool nextExists=hasNext();
  if (m_inbuf>m_nextobj){
    ++m_nextobj;//just update the index
  } else if (iHasNext()){
    readDataToBuffer();
  } else {
    //throw CoraCoolException("No more data in CoraCoolObjectIter","CoraCoolObjectIter::goToNext");
  }
  return nextExists;
}

CoraCoolObject & CoraCoolObjectIter::currentRef(){
  return *(m_data.at(m_nextobj-1));
}

void CoraCoolObjectIter::close() {
  // release any resources associated with the query 
  // close underlying COOL query
  m_coolitr->close();
  // release ownership of any CoraCoolObjects we have
  for (unsigned int i=0;i<m_buflen;++i) m_data[i].reset();
}

bool CoraCoolObjectIter::equalAttr(const coral::Attribute& a1, const coral::Attribute& a2) const 
{
  // compare two Attributes based on type and value
  // deal with some special cases where the types are not the same
  // the easy part - if types are same
  std::string spec1=a1.specification().typeName();
  std::string spec2=a2.specification().typeName();
  if (spec1==spec2) {
    return (a1==a2);
  } else if (isNumAttr(spec1) && isNumAttr(spec2)) {
    // convert to long long and compare
    long long v1=numAttr(spec1,a1);
    long long v2=numAttr(spec2,a2);
    return (v1==v2);
  } else {
   // unhandled case 
    return false;
  }
}

bool CoraCoolObjectIter::iHasNext() {
  // implement old-COOL-style iterator behaviour - return if next is valid
  // can be called multiple times - after call currentRef will give data
  if (m_refValid) return true;
  if (m_allDone) return false;
  if (m_coolitr->goToNext()) {
    m_refValid=true;
    return true;
  } else {
    m_refValid=false;
    m_allDone=true;
    return false;
  }
}

bool CoraCoolObjectIter::isNumAttr(const std::string& spec) const {
  return (spec=="long long" || spec=="unsigned long long" ||
	  spec=="int" || spec=="unsigned int" ||
	  spec=="long" || spec=="unsigned long");
}

long long CoraCoolObjectIter::numAttr(const std::string& spec,
				      const coral::Attribute& a) const {
  long long v=0;
  if (spec=="long long") {
    v=a.data<long long>();
  } else if (spec=="unsigned long long") {
    v=static_cast<long long>(a.data<unsigned long long>());
  } else if (spec=="int") {
    v=static_cast<long long>(a.data<int>());
  } else if (spec=="unsigned int") {
    v=static_cast<long long>(a.data<unsigned int>());
  } else if (spec=="long") {
    v=static_cast<long long>(a.data<long>());
  } else if (spec=="unsigned long") {
    v=static_cast<long long>(a.data<unsigned long>());
  }
  return v;
}

CoraCoolObjectIter::QueryBuilder::QueryBuilder(const unsigned int size) :
  m_terms(0), m_size(0), m_maxsize(size) {
  m_lower=new long long[m_maxsize];
  m_upper=new long long[m_maxsize];
}

CoraCoolObjectIter::QueryBuilder::~QueryBuilder() {
  delete [] m_lower;
  delete [] m_upper;
}

unsigned int CoraCoolObjectIter::QueryBuilder::add(const long long value) {
  // add the given value to the query sequence
  // find where it must be inserted
  unsigned int i=0;
  unsigned int j=0;
  bool done=false;
  for (unsigned int i=0;i<m_size;++i) {
    if (m_lower[i]-value<2 && value-m_upper[i]<2) {
      // current range contains object or can be extended by one to contain it
      // value is in current range or extends it
      if (m_lower[i]-value==1) --m_lower[i];
      if (value-m_upper[i]==1) ++m_upper[i];
      // if range is of size 1, just changed from one to two-term query
      if (m_upper[i]-m_lower[i]==1) ++m_terms;
      done=true;
      break;
    }
    // check if value has to be inserted after this position, if not found
    if (m_upper[i]<value) ++j;
  }
  if (not done) {
    // did not add to an existing range - make a new one at j
    // move existing values up
    if (m_size<m_maxsize) {
      for (i=m_size;i!=j;--i) {
        m_lower[i]=m_lower[i-1];
        m_upper[i]=m_upper[i-1];
      }
      ++m_size;
      // add new range at j
      m_lower[j]=value;
      m_upper[j]=value;
      ++m_terms;
    } else {
      // if not enough room, return zero to signal error
      return 0;
    }
  }
  return m_terms;
}

void CoraCoolObjectIter::QueryBuilder::getQuery(std::string& where,
						const std::string& coralkey,
     coral::AttributeList& fkeys,const coral::AttributeSpecification& spec) {
  // try to compress the query by joining existing ranges if possible
  unsigned int j=0;
  for (unsigned int i=0;i<m_size;++i) {
    long long lower=m_lower[i];
    long long upper=m_upper[i];
    // check if this range can be absorbed into previous one
    if (i>0) {
      if (m_lower[i]-m_upper[j]<2) {
        // if so, adjust upper limit of previous range to encompass this one
        // and do not copy current range
        m_upper[j]=m_upper[i];
      } else {
        // note this never gets executed on first iteration - j lags 1 behind i
        ++j;
        m_lower[j]=lower;
        m_upper[j]=upper;
      }
    }
  }
  m_size=j+1;
  // build the query string and list of FK values to be used
  unsigned int ikey=0;
  for (unsigned int i=0;i<m_size;++i) {
    if (where.size()>0) where+=" OR ";
    if (m_lower[i]==m_upper[i]) {
      // bounds are equal - simple equivalence term
      std::string keyname=addKey(ikey,fkeys,spec,m_lower[i]);
      where+=coralkey+"=:"+keyname;
    } else {
      // bounds are not equal - need  A>=B and A<=C
      std::string keyname=addKey(ikey,fkeys,spec,m_lower[i]);
      where+="("+coralkey+">=:"+keyname+" AND "+coralkey+"<=:";
      keyname=addKey(ikey,fkeys,spec,m_upper[i]);
      where+=keyname+")";
    }
  }
}

std::string CoraCoolObjectIter::QueryBuilder::addKey(unsigned int& ikey,
   coral::AttributeList& fkeys,const coral::AttributeSpecification& spec,
						     const long long value) {
  // return the name of the FK and add it to the list of FK bind variables
  std::ostringstream skey;
  skey << "B" << ikey;
  std::string keyname=skey.str();
  fkeys.extend(keyname,spec.type());
  std::string typeName=spec.typeName();
  if (typeName=="long long") {
    fkeys[keyname].data<long long>()=value;
  } else if (typeName=="unsigned long long") {
    fkeys[keyname].data<unsigned long long>()=value;
  } else if (typeName=="int") {
    fkeys[keyname].data<int>()=value;
  } else if (typeName=="unsigned int") {
    fkeys[keyname].data<unsigned int>()=value;
  } else if (typeName=="long") {
    fkeys[keyname].data<long>()=value;
  } else if (typeName=="unsigned long") {
    fkeys[keyname].data<unsigned long>()=value;
  } else {
    throw CoraCoolException("Unregognised type "+typeName,
			    "CoraCoolObjectIter::QueryBuilder::addKey");
  }
  ++ikey;
  return keyname;
}
