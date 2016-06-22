// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///
/// \file Serializer.cxx
/// \author Jiri Masik <Jiri.Masik@cern.ch>
///




#include "TrigTSerializer.h"
#include "SerializeCommon.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TROOT.h"
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,17,6)
#include "TBufferFile.h"
#else
#include "TBuffer.h"
#endif
#include "TClass.h"
#include "TError.h"
#include "TMethodCall.h"
#include <vector>
#include <string>
#include <iostream>

//
#include "TFile.h"
#include "TList.h"
#include "TStreamerInfo.h"
#include "PathResolver/PathResolver.h"

#include "DataModelRoot/RootType.h"

#define  TRIGTSERHEADER  0xf00dbeef
//#define  TRIGTSERTRAILER 0xbeeff00d
#define  TRIGTSERVERSION 0x0005U

 /*
  *  Format description:
  *   1) versioning starts with version 0x0001
  *   2) before that the root buffer was copied to ints
  *   3) version 0001 introduces
  *         4-byte header,
  *	    4-byte version,
  *         4-bytes of the original Root buffer length buffersize
  *         buffersize/4 + bufsiz%4 integers of payload,
  * 
  *  version 2: attempt to decode non-versioned streams as 
  *               version 0x0000 of the TrigTSerializer
  * 
  *  version 3: fix sending of bufsiz%4 integers (pointed out by Brian), 
  *             in version 2 they do not contain useful data
  *
  *  version 4: store an additional unsigned int identifying TP converter
  *             used to persistify the object (was never used)
  *
  *  version 5: stores GUID along of the persistent class in the preambule             
  * 
  */



bool TrigTSerializer::s_decodingError;

TrigTSerializer::TrigTSerializer(const std::string& toolname, const std::string& type, const IInterface* parent) : AthAlgTool(toolname, type, parent) {
  declareInterface<ITrigSerializerToolBase>( this );
  declareProperty("OnlineMode", m_onlineMode=false, "avoid initializations not needed in the online");

  for (size_t i=0; i<4; i++) m_guid[i]=0;
}

TrigTSerializer::~TrigTSerializer(){
}

StatusCode TrigTSerializer::initialize(){
  ATH_MSG_DEBUG( "TrigTSerializer::initialize " << name() );

  if (!m_onlineMode)
    add_previous_streamerinfos();

  m_IgnoreErrLvl = gErrorIgnoreLevel;

  return StatusCode::SUCCESS;

}

StatusCode TrigTSerializer::finalize(){

  std::map<std::string,uint32_t>::const_iterator mitr(m_errCount.begin());
  std::map<std::string,uint32_t>::const_iterator mend(m_errCount.end());
  bool reported(false);
  while (mitr!=mend){
    reported=true;
    ATH_MSG_WARNING( "Could not interpret persistent object " << (*mitr).first
                     << " /" << (*mitr).second << " times." );
    ++mitr;
  }
  if (!reported)
    ATH_MSG_INFO( name() << " no problems encountered" );

  return StatusCode::SUCCESS;
}

void TrigTSerializer::add_previous_streamerinfos(){
  //temporary 
  std::string extStreamerInfos = "bs-streamerinfos.root";
  std::string m_extFile = PathResolver::find_file (extStreamerInfos, "DATAPATH");
  ATH_MSG_DEBUG( "Using " << m_extFile );
  TFile f(m_extFile.c_str());
  TList *a = f.GetStreamerInfoList();
  TIter nextinfo(a);
  TStreamerInfo *inf;
  while ((inf = (TStreamerInfo *)nextinfo()) != 0){

    TString t_name=inf->GetName();

    if (t_name.BeginsWith("listOfRules")){
        ATH_MSG_WARNING( "Could not re-load  class " << t_name );
    	continue;
    }

    inf->BuildCheck();
    //this triggers a crash on lcg60
    TClass *cl = inf->GetClass();
    if (cl)
      ATH_MSG_DEBUG( "external TStreamerInfo for " << cl->GetName()
                     << " checksum: " << inf->GetCheckSum()  );
  }

}


void TrigTSerializer::streamerErrorHandler(int level, bool abort_bool,
					   const char* location, const char *msg){
   if( level > kInfo ) {
      //MN: ignore stuff below kWarning
      s_decodingError = true;
   }
   int oldLvl = gErrorIgnoreLevel;
   if( gDebug > 0 ) {
      gErrorIgnoreLevel = kPrint;
   }
   ::DefaultErrorHandler(level,abort_bool, location, msg);
   gErrorIgnoreLevel = oldLvl;
}

void TrigTSerializer::prepareForTBuffer(const std::string &nameOfClass){
  m_IgnoreErrLvl = gErrorIgnoreLevel;
  //MN: this setting does not play well with ROOTDEBUG:
  //  gErrorIgnoreLevel = kInfo+1;
  //had this class a problem before?
  if (m_errCount.find(nameOfClass)!=m_errCount.end()){
    gErrorIgnoreLevel = kError+1;   
  } 
  s_decodingError = false;

  m_defaultHandler = ::SetErrorHandler(streamerErrorHandler);
}

void TrigTSerializer::restoreAfterTBuffer(const std::string &nameOfClass){
  if (s_decodingError){
    s_decodingError = false;
    

    if (m_errCount.find(nameOfClass)!=m_errCount.end()){
      ++m_errCount[nameOfClass];
    } else {
      ATH_MSG_ERROR( "Errors while decoding " << nameOfClass
                     << " any further ROOT messages for this class will be suppressed"  );
      m_errCount[nameOfClass] = 1;
    }
  }

  ::SetErrorHandler(m_defaultHandler);
  gErrorIgnoreLevel = m_IgnoreErrLvl;
}

std::vector<uint32_t> TrigTSerializer::serialize(const std::string &nameOfClass, void* instance){
  std::vector<uint32_t> serialized;
  serialize(nameOfClass, instance, serialized);
  return serialized;
}

void TrigTSerializer::serialize(const std::string &nameOfClass, void* instance, std::vector<uint32_t> &serialized){

  serialized.clear();

  size_t rootDebug = gDebug;

  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "in serialize for " << nameOfClass   );
    /*
    //cannot be used in production 
    //higher gDebug seems to interfere with ErrorHandler
    if (m_outputlevel<=MSG::VERBOSE && rootDebug<5)
      gDebug=5;
    */
  }

  std::string noc= TrigSerializeResult::remapToDictName(nameOfClass);
  TClass *pclass = gROOT->GetClass(noc.c_str());

  //  do_persistify(noc, instance);

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,17,6)
  TBufferFile *m_buff = new TBufferFile(TBuffer::kWrite);
#else
  TBuffer *m_buff = new TBuffer(TBuffer::kWrite);
#endif

  //std::vector<uint32_t> serialized;

  if (pclass){
    //m_buff->StreamObject(instance, pclass);
    
    ATH_MSG_DEBUG( "writing instance " << instance << " to buffer for noc " << noc   );
    
    prepareForTBuffer(noc);
    m_buff->WriteObjectAny(instance, pclass);
    restoreAfterTBuffer(noc);
    
    ATH_MSG_DEBUG( "wrote buffer of length " << m_buff->Length()  );
	

    const char *pbuff = m_buff->Buffer();
    const size_t bufsiz = m_buff->Length();

    serialized.push_back(TRIGTSERHEADER);
    serialized.push_back(TRIGTSERVERSION);
    serialized.push_back(bufsiz);
    serialized.push_back(m_guid[0]);
    serialized.push_back(m_guid[1]);
    serialized.push_back(m_guid[2]);
    serialized.push_back(m_guid[3]);

    //inefficient - to be compatible with Serializer for the moment can be avoided later
    uint32_t pbytes;
    char *pp = (char *)&pbytes;
    size_t nints = bufsiz/4 + 1;
    if (bufsiz%4) nints++;

    for (size_t i=0; i<bufsiz/4; i++){
      pbytes = 0;
      for (size_t j=0; j<4; j++){
	*(pp+3-j) = pbuff[4*i+j];
      }
      // ATH_MSG_DEBUG( "packed " << std::hex << pbytes <<  std::dec  );
      
      serialized.push_back(pbytes);
    }

    //send rest of chars as one int each
    const size_t modb = bufsiz%4;
    for (size_t i=0; i<modb; i++){
      pbytes = 0;
      *pp = pbuff[bufsiz-modb+i];
      serialized.push_back(pbytes);
    }

    if (msgLvl(MSG::VERBOSE)){
      msg(MSG::VERBOSE) << "serialized dump: ";
      for (size_t i=0; i<serialized.size(); i++){
	msg(MSG::VERBOSE) << std::hex << serialized.at(i) <<std::dec << " ";
      }
      msg(MSG::VERBOSE) << endreq;
    }
  }
  else {
    ATH_MSG_ERROR( "gROOT->GetClass failed for" << nameOfClass  );
  } 
  
  delete m_buff;
  gDebug=rootDebug;
  
  //  return serialized;
}

void* TrigTSerializer::deserialize(const std::string &nameOfClass, std::vector<uint32_t>& v){

  size_t rootDebug = gDebug;
  if (msgLvl(MSG::DEBUG)){
    ATH_MSG_DEBUG( "in deserialize for " << nameOfClass   );
    /*
    //cannot be used in production 
    //higher gDebug seems to interfere with ErrorHandler
    if (m_outputlevel<=MSG::VERBOSE &&rootDebug<5)
      gDebug=5;
    */
  }

  const size_t vsize = v.size();
  uint32_t rVersion(0);
  size_t bufsiz(0);
  bool newFormatOK=true;
  bool decodeOldFormat=true;
  size_t pBuffOffset(3);      //adjust below
  //
  //

  if (vsize>2){
    if (v.at(0)==TRIGTSERHEADER){
      rVersion = v.at(1);
      bufsiz = v.at(2);
      if (rVersion>3)
	pBuffOffset++;
      if (rVersion>4)
	pBuffOffset += 3;   //wrt ver 4

      //check size with vsize
      size_t expectsize = pBuffOffset + bufsiz/4 + bufsiz%4;

      if (expectsize!=vsize){
	newFormatOK = false;
	ATH_MSG_WARNING( "expected payload length "<<expectsize
                         << "  does not match the container size " << vsize
                         << " for " << nameOfClass << " fallback to the old format:" );
      }
      ATH_MSG_DEBUG( bufsiz << "bytes of payload version " << std::hex << rVersion << std::dec
                     << " for " << nameOfClass   );
    }
    else {
      ATH_MSG_WARNING( "not a versioned payload of "
                       <<  nameOfClass << " trying initial TrigTSerializer"  );
      newFormatOK = false;
    }
 
  } else {
    ATH_MSG_WARNING( "zero bytes of payload for " << nameOfClass   );
    gDebug=rootDebug;
    return NULL;
  }

  //
  char *pbuf = NULL;

  if (newFormatOK){
    //  const size_t bufsiz = v.size();
    pbuf = new char[bufsiz];
    size_t bufpos=0;
    const size_t nints = bufsiz/4;
    for (size_t i=pBuffOffset; i<nints+pBuffOffset; i++){
      uint32_t pbytes = v.at(i);
      char *pch = (char *)&pbytes;
      for (size_t c=0; c<4; c++){
	pbuf[bufpos] = *(char *)(pch+3-c);
	bufpos++;
      }
    }

    for (size_t i=nints+pBuffOffset; i<vsize; i++){
      pbuf[bufpos] = (char)v.at(i);
      bufpos++;
    }

    if (msgLvl(MSG::VERBOSE)){
      msg(MSG::VERBOSE) << "deserialized dump: ";
      for (size_t i=0; i<v.size(); i++){
	msg(MSG::VERBOSE) << std::hex << v.at(i) <<std::dec << " ";
      }
      msg(MSG::VERBOSE) << endreq;
    }

  } else if (!newFormatOK && decodeOldFormat){
    bufsiz = v.size();
    pbuf = new char[bufsiz];
    
    for (size_t i=0; i<bufsiz; i++)
      pbuf[i] = v.at(i);
  } else {
    gDebug=rootDebug;
    return NULL;
  }

  /*
  if (newFormatOK && rVersion >3)
    setCLID(v.at(3));
  */

  //common part
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,17,6)
  TBufferFile *m_buff = new TBufferFile(TBuffer::kRead, bufsiz, pbuf, kTRUE);
#else
  TBuffer *m_buff = new TBuffer(TBuffer::kRead, bufsiz, pbuf, kTRUE);
#endif

  
  std::string noc= TrigSerializeResult::remapToDictName(nameOfClass);
  TClass *pclass = gROOT->GetClass(noc.c_str());
  
  TObject *pobj = NULL;
  if (pclass){
    prepareForTBuffer(noc);
    pobj = (TObject *)(m_buff->ReadObjectAny(pclass));
    restoreAfterTBuffer(noc);
    //ErrorHandlerFunc_t oldhandler= ::SetErrorHandler(streamerErrorHandler);
    //SetErrorHandler(oldhandler);
    //m_buff->StreamObject(instance, pclass);
  }
  else {
    ATH_MSG_ERROR( "gROOT->GetClass failed for" << nameOfClass  );
  }
  
  delete m_buff;   //this also deletes pbuf owned by m_buff
  gDebug=rootDebug;

  return (void *)pobj;
}


StatusCode TrigTSerializer::initClass(const std::string &nameOfClass) const {

  TClass *pclass = gROOT->GetClass(nameOfClass.c_str());


  StatusCode sc(StatusCode::FAILURE); 

  if (pclass)
    sc = StatusCode::SUCCESS;


  return sc;

}



void TrigTSerializer::reset(){
  m_guid[0] = m_guid[1] = m_guid[2] = m_guid[3] = 0;
}

StatusCode TrigTSerializer::peekCLID(const std::vector<uint32_t>& v, uint32_t *guid) const {
  if (v.size()>2){
    if (v.at(0)==TRIGTSERHEADER){
      const uint32_t rVersion = v.at(1);
      if (rVersion>4){
	guid[0] = v.at(3);
	guid[1] = v.at(4);
	guid[2] = v.at(5);
	guid[3] = v.at(6);
	return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}
  
void TrigTSerializer::setCLID(const uint32_t *guid){
  for (size_t i=0; i<4; i++)
    m_guid[i] = guid[i];
}


/*
StatusCode persistifyEL(const TClass *cl, void* instance){
  if (cl){
    TMethodCall method(cl, "toPersistent", "");
    meth.Execute(instance);
    return StatusCode::SUCCESS;
  }
  else
    return StatusCode::FAILURE;
}

*/



#ifdef ROOT_6

#include <boost/current_function.hpp>
using namespace std;

void TrigTSerializerUnimplError() {
   cerr << endl
        << "ERROR!  method TrigTSerializer::" << BOOST_CURRENT_FUNCTION
        << " was removed during migration to ROOT6" << endl;
   abort();
}

void TrigTSerializer::do_persistify(const std::string , void* ) { TrigTSerializerUnimplError(); }
void TrigTSerializer::do_persistify_obj(const std::string , void*) { TrigTSerializerUnimplError(); }
void TrigTSerializer::do_stl_workaround(const Type *, const Object *) { TrigTSerializerUnimplError(); }

#else // ROOT 5
#include "Reflex/Base.h"

void TrigTSerializer::do_persistify(const std::string nameOfClass, void* instance){

  Type t = Type::ByName(nameOfClass);
  if (t.IsComplete()){
    ATH_MSG_VERBOSE( nameOfClass << ": type.IsComplete "  );
  }

  if (t.IsPointer()){

    do_follow_ptr(nameOfClass, instance);

  } else {
    
    if (t.IsFundamental()){
      //do nothing
    }
    
    else if (t.IsEnum()){
      //do nothing
    }
    else if (t.IsTypedef()){
      ATH_MSG_VERBOSE( "is typedef "  );
      Type underlying = t.ToType();
      if (underlying){
	do_persistify(underlying.Name(), instance);
      }
      else {
	ATH_MSG_WARNING( "typedef " << nameOfClass 
                         <<  " has no underlying type"  );
      }
    }
    else if (t.IsArray()){
      //arrays
      /*
      StatusCode sc = do_serialize_array(&t1, instance);
      if (sc.isFailure())
        ATH_MSG_WARNING( "do_serialize_array "
	       <<  nameOfClass << " failed "  );
      */
    }
    else {

      //this is a class object
	/*
	//this bit can change dictionaries for Root I/O
	if (t.BaseSize())
        t.UpdateMembers();     //get members of base classes too
	*/

      ATH_MSG_VERBOSE( " class with " << t.BaseSize() << " bases"  );

      //iterate over bases and the class 
      for (size_t ib=0; ib<=t.BaseSize(); ib++){

        Reflex::Base bc = t.BaseAt(ib);
	Type tt = bc.ToType();
	std::string nameOfBase = tt.Name();

	if (ib==t.BaseSize()){
	  tt = t;
	  nameOfBase = nameOfClass;
	}
	  
        ATH_MSG_VERBOSE( " basename "  );

	if (!tt.IsComplete()){
	  //typically this happens for templates
	  ATH_MSG_DEBUG( "Incomplete type " << nameOfBase  );
	  
	  do_persistify_obj(nameOfBase, instance);
	  
	  if (tt.IsTemplateInstance()){
	    
	    
	    // do serialize stl
	    Object stlobj(tt, instance);
	    do_stl_workaround(&tt, &stlobj); 	  
	  } 
	}      
	
	const int nmembers = tt.DataMemberSize();
	
	
	Object obj(tt, instance); 
	Member member;
	std::string membername, stype;
	//loop over class members  
	for (int i=0; i< nmembers; i++){
	  member = tt.DataMemberAt(i); 
	  membername = member.Name();
	  Type type = member.TypeOf();
	  
	  stype = type.Name();
          ATH_MSG_VERBOSE( "membername: " << membername << " type: "   );
	  
	  //////////////////////////////       skip transient     //////////////////////////////
	  if (member.IsTransient()){
	    ATH_MSG_VERBOSE( membername << " isTransient " << stype  );
	    continue;
	  }
	  //////////////////////////////       pointer     //////////////////////////////
	  else if (type.IsPointer()){
	    //pointers
            ATH_MSG_VERBOSE( membername << " isPointer " << stype  );
	    void *ptr = Reflex::Object_Cast <void *>(obj.Get(membername));
	    do_follow_ptr(stype, ptr);
	  } 
	  //////////////////////////////       template      //////////////////////////////
	  else if (type.IsTemplateInstance()){
	    std::string classname = type.Name();
            ATH_MSG_VERBOSE( " isTemplateInstance "  << classname  );
	    
	    Object o3 = member.Get(obj);
	    void *ptr = o3.Address();
	    do_persistify_obj(classname, ptr);
	    do_stl_workaround(&type, &o3);
	  }
	  
	  //////////////////////////////       object      ////////////////////////////// 
	  else if (type.IsClass()){
	    
	    Object o3 = member.Get(obj);
	    void *ptr = o3.Address();
	    
	    std::string classname = type.Name();
            ATH_MSG_VERBOSE( " isClass "  << type.Name()  );
	    do_persistify(classname, ptr );
	    
	  }
	  //////////////////////////////       array       //////////////////////////////
	  else if (type.IsArray()) {
            ATH_MSG_VERBOSE( " isArray "  << stype  );
	    /*
	      Object oa = member.Get(obj);
	      void *po=     (obj.Get(membername)).Address();
	      StatusCode sarr = do_serialize(stype, po);
	    */
	  }
	  
	  //////////////////////////////       basic types //////////////////////////////
	  else if (type.IsFundamental()){
	    //do nothing
	  }
	  
	  else if (type.IsEnum()){
	    //do nothing
	  }
	  
	  else if (type.IsTypedef()){
	    ATH_MSG_VERBOSE( stype << " is typedef "  );
	    void *pobj = (obj.Get(membername)).Address();
	    do_persistify(stype, pobj);
	    
	    
	    //////////////////////////////       what else?  //////////////////////////////
	  } else if (type.IsPointerToMember()){
            ATH_MSG_VERBOSE( "unsupported IsPointerToMember" << stype  );
	    
	  } else if (type.IsReference()){
            ATH_MSG_VERBOSE( "unsupported IsReference" << stype  );
	    
	  } else if (type.IsStruct()){
            ATH_MSG_VERBOSE( "unsupported IsStruct" << stype  );
	    
	  } else if (type.IsUnqualified()){
            ATH_MSG_VERBOSE( "unsupported IsUnqualified" << stype  );
	  }
	  
	  //////////////////////////////       unhandled   //////////////////////////////
	  else {
	    ATH_MSG_WARNING( "unsupported type " << stype  );
	  }
	}
      }
    }
  }
}


void TrigTSerializer::do_persistify_obj(const std::string nameOfClass, void* instance){

  Type t = Type::ByName(nameOfClass);
  if (t.IsComplete()){
    ATH_MSG_DEBUG( nameOfClass << ": type.IsComplete failed"  );
  }
  
  const std::string method = "toPersistent";
  Member toPers = t.FunctionMemberByName(method);

  if (toPers.Name() != method){
    ATH_MSG_DEBUG( nameOfClass << ": has no toPersistent()"  );
    return;
  }

  Object obj(t, instance);
  toPers.Invoke(obj);
  ATH_MSG_DEBUG( nameOfClass << ": toPersistent() invoked on " << instance  );

}



void TrigTSerializer::do_stl_workaround(const Type *mytype,
					 const Object *myobject){


  size_t ntempargs = mytype->TemplateArgumentSize();
  const std::string classname = mytype->Name();
  
  if (ntempargs==1) {

    size_t nfunc = mytype->FunctionMemberSize();
	    
    if (msgLvl(MSG::VERBOSE)){
      ATH_MSG_DEBUG( classname << ": trying STL container workaround"  );
      ATH_MSG_DEBUG( "with " << nfunc << " function members"  );
    
      for (size_t jf=0; jf<nfunc; jf++)
	msg(MSG::DEBUG) << mytype->FunctionMemberAt(jf).Name() << " ";
    
      msg() << endreq;
    }
    
    Member msize = mytype->FunctionMemberByName("size");
#if ROOT_VERSION_CODE < ROOT_VERSION(5,22,0)
    Object osize = msize.Invoke(*myobject);
    const size_t cont_siz = *(size_t *)(osize.Address());
#else
    size_t cont_siz = 0;
    msize.Invoke(*myobject, cont_siz);
#endif
    ATH_MSG_DEBUG( "container size  " << cont_siz  );

    Member mat = mytype->FunctionMemberByName("at");
    Type templtype = mytype->TemplateArgumentAt(0);
    const std::string templtypename = templtype.Name();

    for (size_t is=0; is<cont_siz; is++){
      std::vector< void * > vec_args;
      vec_args.push_back(&is);
#if ROOT_VERSION_CODE < ROOT_VERSION(5,22,0)
     Object elem = mat.Invoke(*myobject, vec_args);
#else
     Object elem;
     mat.Invoke(*myobject, elem, vec_args);
#endif
      Type elemtype = elem.TypeOf();
      ATH_MSG_DEBUG( "container elem is type of " << elemtype.Name()  );
      ATH_MSG_DEBUG( "invoking at(" << is << ")=" << templtypename  );

      //CHECK
      // ATH_MSG_DEBUG( "ptr should yield " << (void *)(*(int *)rptr)  );

      void *rptr = elem.Address();
      if (elemtype.IsPointer()){
	void **optr = reinterpret_cast<void**>(elem.Address());
      	rptr = *optr;
      }

      do_persistify(templtypename, rptr);
    }

  }
}

#endif // ROOT 5

void TrigTSerializer::do_follow_ptr(const std::string name, void *ptr){

  ATH_MSG_VERBOSE("Entering do_follow_ptr for "  << name << " at " << ptr  );
  
  
  if (ptr){
    const std::string classname = name.substr(0, name.find_last_of("*"));
    ATH_MSG_DEBUG( "going deeper for " << classname << " at " << ptr  );
    do_persistify(classname, ptr);
  }
  
}
