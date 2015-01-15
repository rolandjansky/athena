/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IIncidentSvc.h"
#include "ByteStreamAttListMetadataSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "ByteStreamData/ByteStreamUserMetadata.h"
//#include "GaudiKernel/ToolHandle.h"
//#include "TBufferFile.h"
//#include "TClass.h"
//#include "TROOT.h"
#include <string>
#include <iostream>
#include <sstream>

ByteStreamAttListMetadataSvc::ByteStreamAttListMetadataSvc( const std::string& name, 
                                                            ISvcLocator* pSvcLocator )
  : AthService( name, pSvcLocator )
{
   //declareInterface<IByteStreamFreeMetadataTool>( this );
   declareProperty("InputStoreName", m_inputStoreName = "InputMetaDataStore");
   declareProperty("OutputStoreName", m_outputStoreName = "DetectorStore");
}

StatusCode ByteStreamAttListMetadataSvc::initialize()
{
   ATH_MSG_DEBUG("ByteStreamAttListMetadataSvc::initialize()");
   return StatusCode::SUCCESS;
}

// For certain clients, need an empty container available to initialize
StatusCode ByteStreamAttListMetadataSvc::readInit(const std::vector<std::string>& keys)
{
   StoreGateSvc* mdStore;
   StatusCode sc = service(m_outputStoreName.toString(),mdStore);
   if (sc.isFailure()) return StatusCode::FAILURE;
   StatusCode allok = StatusCode::SUCCESS;
   std::vector<std::string>::const_iterator it = keys.begin();
   while (it != keys.end()) {
      AthenaAttributeList* attlist;
      sc = mdStore->retrieve(attlist,*it );
      if (sc.isFailure()) {
         coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
         attlist = new AthenaAttributeList(*spec);
         StatusCode sc2 = mdStore->record(attlist, *it );
         if (sc2.isFailure()) {
            msg() << MSG::WARNING 
                  << "Unable to store AthenaAttributeList for " << *it << endreq;
            allok = StatusCode::FAILURE;
         }
      }
      else {
          msg() << MSG::DEBUG << *it << " already present in " 
                << m_outputStoreName.toString() << endreq;
       }
       ++it;
   }
   return allok;
}

/*
// Fails due to no default constructor for coral::Attribute
StatusCode ByteStreamAttListMetadataSvc::serialize(const std::vector<std::string>& keys)
{

   // Retrieve serializer 
   if (!m_serial.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get serial.");
      return(StatusCode::FAILURE);
   }
   msg() << MSG::INFO << "ByteStreamAttListMetadataSvc::serialize" << endreq;

   m_serial->reset();

   const AthenaAttributeList* attList;
   StoreGateSvc* inStore;
   StatusCode sc = service(m_inputStoreName.toString(),inStore);
   if (sc.isFailure()) return StatusCode::FAILURE;
   std::vector<std::string>::const_iterator it = keys.begin();
   while (it != keys.end()) {
      sc = inStore->retrieve( attList, *it );
      if (sc.isSuccess()) {
         msg() << MSG::INFO << "Found " << *it << " in " << m_inputStoreName.toString() << endreq;
         ByteStream::FreeMetadata* metacont = new ByteStream::FreeMetadata();
         std::string cl = "AthenaAttributeList";
         std::string cnvname;
         void *pObj = (void *)&(*attList);
         std::vector<uint32_t> out;
         m_serial->serialize(cl,pObj,out);
      }
      ++it;
   }
   return StatusCode::SUCCESS;
}
*/

StatusCode ByteStreamAttListMetadataSvc::toBSMetadata(const std::vector<std::string>& keys)
{
   msg() << MSG::INFO << "ByteStreamAttListMetadataSvc::toBSMetadata" << endreq;
   const DataHandle< AthenaAttributeList > attList;
   StoreGateSvc* inStore;
   StatusCode sc = service(m_inputStoreName.toString(),inStore);
   if (sc.isFailure()) return StatusCode::FAILURE;
   std::vector<std::string>::const_iterator it = keys.begin();
   while (it != keys.end()) {
      sc = inStore->retrieve( attList, *it );
      if (sc.isSuccess()) {
        msg() << MSG::INFO << "Found " << *it << " in " << m_inputStoreName.toString() << endreq;
        ByteStream::FreeMetadata* metacont = new ByteStream::FreeMetadata();
        coral::AttributeList::const_iterator first1 = attList->begin();
        coral::AttributeList::const_iterator last1  = attList->end();
        for (; first1 != last1; ++first1) {
           const std::string name = (*first1).specification().name();
           const std::string type = (*first1).specification().typeName();
           const std::type_info& typeinfo = (*first1).specification().type();
           std::stringstream buf;
           if ( typeinfo == typeid(bool) ) {
              const bool* value = static_cast<const bool*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(int) ) {
              const int* value = static_cast<const int*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(unsigned int) ) {
              const unsigned int* value = static_cast<const unsigned int*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(long) ) {
              const long* value = static_cast<const long*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(float) ) {
              const float* value = static_cast<const float*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(double) ) {
              const double* value = static_cast<const double*>( (*first1).addressOfData() );
              buf << *value;
           }
           else if ( typeinfo == typeid(std::string) ) {
              const std::string* value = static_cast<const std::string*>( (*first1).addressOfData() );
              buf << *value;
           }
           else ATH_MSG_ERROR("Type " << type << " not supported");
           ATH_MSG_DEBUG(" name " << name << " type " << type << " value " << buf.str());
           //buf << first1->data<(*first1).specification().type()>();
           const std::string value = buf.str();
           msg() << MSG::INFO << "About to push back " << name << ", " << value << endreq;
           metacont->push_back(ByteStreamFreeMetadataString(name+"/"+type,value));
        }
        StoreGateSvc* outStore;
        StatusCode sc2 = service(m_outputStoreName.toString(),outStore);
        if (sc2.isSuccess()) {
           sc2 = outStore->record(metacont, *it );
           if (sc2.isFailure()) {
              msg() << MSG::WARNING << "Unable to store ByteStreamFreeMetadataString for " << *it << endreq;
           }
        }
        else {
           msg() << MSG::WARNING << "Could not find store " << m_outputStoreName.toString() << endreq;
           delete metacont; 
           return StatusCode::FAILURE;
        }
     }
     else {
        msg() << MSG::WARNING << "Did NOT find " << *it << " in "
              << m_inputStoreName.toString() << endreq;
        msg() << MSG::DEBUG << inStore->dump() << endreq;
     }
     ++it;
  }
   
   return StatusCode::SUCCESS;
}

StatusCode ByteStreamAttListMetadataSvc::fromBSMetadata(const std::vector<std::string>& keys)
{
  // First check for metadata from the bytestream file
  StoreGateSvc* inStore;
  StatusCode sc = service(m_inputStoreName.toString(),inStore);
  if (sc.isFailure()) return StatusCode::FAILURE;
  std::vector<std::string>::const_iterator it = keys.begin();
  while (it != keys.end()) {
    const DataHandle< ByteStream::FreeMetadata > metacont;
    sc = inStore->retrieve( metacont, *it );

    if (sc.isSuccess()) {
       msg() << MSG::DEBUG << "Found " << *it << " in " << m_inputStoreName.toString() << endreq;

       // Check for attlist in detector store (should be there from initialize)
       AthenaAttributeList* attlist;
       bool found(false);
       StoreGateSvc* outStore;
       sc = service(m_outputStoreName.toString(),outStore);
       if (sc.isFailure()) return StatusCode::FAILURE;
       sc = outStore->retrieve(attlist, *it);
       if (sc.isFailure()) {
          // build spec
          // fill att list spec
          coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
          attlist = new AthenaAttributeList(*spec);
       }
       else found=true;

       // Add attributes name,types to the spec through attlist
       ByteStream::FreeMetadata::const_iterator first1 = metacont->begin();
       ByteStream::FreeMetadata::const_iterator last1  = metacont->end();
       for (; first1 != last1; ++first1) {
          std::string name = first1->first.substr(0,first1->first.find('/'));
          std::string type = first1->first.substr(first1->first.find('/')+1);
          msg() << MSG::INFO
                << "name = " << name << ", type = " << type << ", value = " << first1->second << endreq;
          attlist->extend(name,type);
       }

       // Yes we have to loop twice to add the data
       first1 = metacont->begin();
       for (; first1 != last1; ++first1) {
          std::string name = first1->first.substr(0,first1->first.find('/'));
          std::string type = first1->first.substr(first1->first.find('/')+1);
          std::istringstream s(first1->second);
          if (type == "string") {
             (*attlist)[name].data<std::string>() = first1->second;
          }
          else if (type == "bool") {
             bool temp;
             s >> temp;
             (*attlist)[name].data<bool>() = temp;
          }
          else if (type == "int") {
             int temp;
             s >> temp;
             (*attlist)[name].data<int>() = temp;
          }
          else if (type == "unsigned int") {
             unsigned int temp;
             s >> temp;
             (*attlist)[name].data<unsigned int>() = temp;
          }
          else if (type == "long") {
             long temp;
             s >> temp;
             (*attlist)[name].data<long>() = temp;
          }
          else if (type == "float") {
             float temp;
             s >> temp;
             (*attlist)[name].data<float>() = temp;
          }
          else if (type == "double") {
             double temp;
             s >> temp;
             (*attlist)[name].data<double>() = temp;
          }
          else ATH_MSG_ERROR("Type " << type << " not supported");
       }

       // If it was not found (ERROR) recover by recording it
       if (!found) {
          sc = outStore->record(attlist, *it );
          if (sc.isFailure()) {
             msg() << MSG::WARNING << "Unable to store AthenaAttributeList for " << *it << endreq;
          }
       }
    }
    else {
       msg() << MSG::WARNING << "Did NOT find " << *it << " in " << m_outputStoreName.toString() << endreq;
    }
    ++it;
  }

  return StatusCode::SUCCESS;
}

//________________________________________________________________________________
StatusCode ByteStreamAttListMetadataSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   if (IByteStreamFreeMetadataSvc::interfaceID().versionMatch(riid)) {
      *ppvInterface = dynamic_cast<IByteStreamFreeMetadataSvc*>(this);
   } else {
     // Interface is not directly available: try out a base class
     return(::AthService::queryInterface(riid, ppvInterface));
   }
   addRef();
   return(StatusCode::SUCCESS);
}

