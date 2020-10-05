// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///
/// \file Serializer.cxx
/// \author Jiri Masik <Jiri.Masik@cern.ch>
///




#include "TrigTSerializer.h"
#include "SerializeCommon.h"
#include "AthenaKernel/getMessageSvc.h"
#include "TROOT.h"
#include "TBufferFile.h"
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
#include "CxxUtils/no_sanitize_undefined.h"

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


#include "TSchemaRuleSet.h"
#include "TExMap.h"
namespace ROOT8367Workaround {
const Int_t  kMapOffset         = 2;   // first 2 map entries are taken by null obj and self obj


// Work around ROOT-8367

class ATLAS_NOT_THREAD_SAFE TBufferFileWorkaround
  : public TBufferFile
{
public:
  void* ReadObjectAnyNV (const TClass*);
  static void* doReadObjectAny NO_SANITIZE_UNDEFINED (TBufferFile* buf, const TClass* cl)
  {
    TBufferFileWorkaround* ba = reinterpret_cast<TBufferFileWorkaround*>(buf);
    return ba->ReadObjectAnyNV(cl);
  }
};


void* TBufferFileWorkaround::ReadObjectAnyNV(const TClass *clCast)
{
   R__ASSERT(IsReading());

   // make sure fMap is initialized
   InitMap();

   // before reading object save start position
   UInt_t startpos = UInt_t(fBufCur-fBuffer);

   // attempt to load next object as TClass clCast
   UInt_t tag;       // either tag or byte count
   TClass *clRef = ReadClass(clCast, &tag);
   TClass *clOnfile = 0;
   Int_t baseOffset = 0;
   if (clRef && (clRef!=(TClass*)(-1)) && clCast) {
      //baseOffset will be -1 if clRef does not inherit from clCast.
      baseOffset = clRef->GetBaseClassOffset(clCast);
      if (baseOffset == -1) {
         // The 2 classes are unrelated, maybe there is a converter between the 2.

         if (!clCast->GetSchemaRules() ||
             !clCast->GetSchemaRules()->HasRuleWithSourceClass(clRef->GetName()))
         {
            // There is no converter
            Error("ReadObject", "got object of wrong class! requested %s but got %s",
                  clCast->GetName(), clRef->GetName());

            CheckByteCount(startpos, tag, (TClass*)0); // avoid mis-leading byte count error message
            return 0; // We better return at this point
         }
         baseOffset = 0; // For now we do not support requesting from a class that is the base of one of the class for which there is transformation to ....
         
         //Info("ReadObjectAny","Using Converter StreamerInfo from %s to %s",clRef->GetName(),clCast->GetName());
         clOnfile = clRef;
         clRef = const_cast<TClass*>(clCast);

      }
      if (clCast->GetState() > TClass::kEmulated && clRef->GetState() <= TClass::kEmulated) {
         //we cannot mix a compiled class with an emulated class in the inheritance
         Error("ReadObject", "trying to read an emulated class (%s) to store in a compiled pointer (%s)",
               clRef->GetName(),clCast->GetName());
         CheckByteCount(startpos, tag, (TClass*)0); // avoid mis-leading byte count error message
         return 0;
      }
   }

   // check if object has not already been read
   // (this can only happen when called via CheckObject())
   char *obj;
   if (fVersion > 0) {
      obj = (char *) (Long_t)fMap->GetValue(startpos+kMapOffset);
      if (obj == (void*) -1) obj = 0;
      if (obj) {
         CheckByteCount(startpos, tag, (TClass*)0);
         return (obj+baseOffset);
      }
   }

   // unknown class, skip to next object and return 0 obj
   if (clRef == (TClass*) -1) {
      if (fBufCur >= fBufMax) return 0;
      if (fVersion > 0)
         MapObject((TObject*) -1, startpos+kMapOffset);
      else
         MapObject((void*)0, 0, fMapCount);
      CheckByteCount(startpos, tag, (TClass*)0);
      return 0;
   }

   if (!clRef) {

      // got a reference to an already read object
      if (fVersion > 0) {
         tag += fDisplacement;
         tag = CheckObject(tag, clCast);
      } else {
         if (tag > (UInt_t)fMap->GetSize()) {
            Error("ReadObject", "object tag too large, I/O buffer corrupted");
            return 0;
            // exception
         }
      }
      obj = (char *) (Long_t)fMap->GetValue(tag);
      clRef = (TClass*) (Long_t)fClassMap->GetValue(tag);

      if (clRef && (clRef!=(TClass*)(-1)) && clCast) {
         //baseOffset will be -1 if clRef does not inherit from clCast.
         baseOffset = clRef->GetBaseClassOffset(clCast);
         if (baseOffset == -1) {
            Error("ReadObject", "Got object of wrong class (Got %s while expecting %s)",
                  clRef->GetName(),clCast->GetName());
            // exception
            baseOffset = 0;
         }
      }

      // There used to be a warning printed here when:
      //   obj && isTObject && !((TObject*)obj)->IsA()->InheritsFrom(clReq)
      // however isTObject was based on clReq (now clCast).
      // If the test was to fail, then it is as likely that the object is not a TObject
      // and then we have a potential core dump.
      // At this point (missing clRef), we do NOT have enough information to really
      // answer the question: is the object read of the type I requested.

   } else {

      // allocate a new object based on the class found
      obj = (char*)clRef->New();
      if (!obj) {
         Error("ReadObject", "could not create object of class %s",
               clRef->GetName());
         // exception
         return 0;
      }

      // add to fMap before reading rest of object
      if (fVersion > 0)
         MapObject(obj, clRef, startpos+kMapOffset);
      else
         MapObject(obj, clRef, fMapCount);

      // let the object read itself
      clRef->Streamer( obj, *this, clOnfile );

      CheckByteCount(startpos, tag, clRef);
   }

   return obj+baseOffset;
}


} // namespace ROOT8367Workaround


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
  std::string extFile = PathResolver::find_file (extStreamerInfos, "DATAPATH");
  ATH_MSG_DEBUG( "Using " << extFile );
  TFile f(extFile.c_str());
  TList* streamersList = f.GetStreamerInfoList();
  TIter nextinfo(streamersList);
  while (TObject* obj = nextinfo()) {
    TStreamerInfo *inf = dynamic_cast<TStreamerInfo*> (obj);
    if (!inf) continue;
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
  streamersList->SetOwner(false);
  streamersList->Clear("nodelete");
  f.Close();
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

  TBufferFile *buff = new TBufferFile(TBuffer::kWrite);

  //std::vector<uint32_t> serialized;

  if (pclass){
    //buff->StreamObject(instance, pclass);
    
    ATH_MSG_DEBUG( "writing instance " << instance << " to buffer for noc " << noc   );
    
    prepareForTBuffer(noc);
    buff->WriteObjectAny(instance, pclass);
    restoreAfterTBuffer(noc);
    
    ATH_MSG_DEBUG( "wrote buffer of length " << buff->Length()  );
	

    const char *pbuff = buff->Buffer();
    const size_t bufsiz = buff->Length();

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
      msg(MSG::VERBOSE) << endmsg;
    }
  }
  else {
    ATH_MSG_ERROR( "gROOT->GetClass failed for" << nameOfClass  );
  } 
  
  delete buff;
  gDebug=rootDebug;
  
  //  return serialized;
}

void* TrigTSerializer::deserialize(const std::string &nameOfClass, const std::vector<uint32_t>& v){

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
      msg(MSG::VERBOSE) << endmsg;
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
  TBufferFile *buff = new TBufferFile(TBuffer::kRead, bufsiz, pbuf, kTRUE);
  
  std::string noc= TrigSerializeResult::remapToDictName(nameOfClass);
  TClass *pclass = gROOT->GetClass(noc.c_str());
  
  TObject *pobj = NULL;
  if (pclass){
    prepareForTBuffer(noc);
    //pobj = (TObject *)(buff->ReadObjectAny(pclass));
    pobj = (TObject *)(ROOT8367Workaround::TBufferFileWorkaround::doReadObjectAny (buff, pclass));
    restoreAfterTBuffer(noc);
    //ErrorHandlerFunc_t oldhandler= ::SetErrorHandler(streamerErrorHandler);
    //SetErrorHandler(oldhandler);
    //buff->StreamObject(instance, pclass);
  }
  else {
    ATH_MSG_ERROR( "gROOT->GetClass failed for" << nameOfClass  );
  }
  
  delete buff;   //this also deletes pbuf owned by buff
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


void TrigTSerializer::do_follow_ptr(const std::string name, void *ptr){

  ATH_MSG_VERBOSE("Entering do_follow_ptr for "  << name << " at " << ptr  );
  
  
  if (ptr){
    const std::string classname = name.substr(0, name.find_last_of("*"));
    ATH_MSG_DEBUG( "going deeper for " << classname << " at " << ptr  );
    do_persistify(classname, ptr);
  }
  
}
