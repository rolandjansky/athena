/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArFlatFromFile.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArCOOLConditions/LArOFCFlat.h"


#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"

#include <fstream>

LArFlatFromFile::LArFlatFromFile( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_hashMax(0),
  m_onlineID(0), m_onlineSCID(0)
{
  
}

StatusCode LArFlatFromFile::initialize()
{
  ATH_CHECK(m_cablingKey.initialize());
  return StatusCode::SUCCESS;
}



void LArFlatFromFile::singleFloatFlat(const char* blobName, const std::string& input, 
							   const std::string& outputName, const unsigned nGain) {

  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend(blobName, "blob");
  spec->extend<unsigned>("version");
  std::unique_ptr<CondAttrListCollection> coll= std::make_unique<CondAttrListCollection>(true);
  // we expect line per channel with Id, hash and value
  std::ifstream myfile(input);
  unsigned long id;
  unsigned hash;
  std::string line;
  while (std::getline(myfile, line)) {
    for (unsigned gain=0;gain<nGain;++gain) {
      auto attrList = std::make_unique<coral::AttributeList>(*spec);
      (*attrList)["version"].setValue(0U);
      coral::Blob& blob=(*attrList)[blobName].data<coral::Blob>();
      blob.resize(m_hashMax*sizeof(float));
      float* pblob=static_cast<float*>(blob.startingAddress());
      for (unsigned hs=0;hs<m_hashMax;++hs) {
        const HWIdentifier chid=m_onlineID->channel_Id(hs);
        float value;
        std::stringstream st(line);
        st>>std::hex>>id>>std::dec>>hash>>value;
        if (value < 0) {
          errIfConnected(chid,gain,blobName);
          value=1.0; //Default vaue is 1.0, since these are multiplicative constants
          ++nDefault;
        } 
        pblob[hs]=value;
        ++nChannels;
      }
      unsigned coolChan=gain;
      //Special case: Store single-gain constant in channel 1 
      //To avoid AttrList vs AttrListCollection confusion in Athena DB infrastructure
      if (nGain==1) coolChan=1;
 
      coll->add(coolChan,*attrList.release());
    }
  }

  ATH_MSG_INFO( "Converted " << blobName << " to inline storage. Total number of channels=" << nChannels );
  ATH_MSG_INFO( "Number of channels filled with default value (1.0) " << nDefault << " (including disconnected)" );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain" <<  nCopiedEMPS );  
  StatusCode sc=detStore()->record(std::move(coll),outputName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection with key" << outputName );
    return;
  }
  return;
}


void LArFlatFromFile::ofcFlat(const std::string& input, const std::string& outputName) {

  ATH_MSG_INFO("LArFlatFromFile::ofcFlat, starting");
  // parsing the file, fill OFC arrays
  std::vector<std::vector<std::vector<float> > > vofc_a; //[ngain][nchannel][nsamples]
  std::vector<std::vector<std::vector<float> > > vofc_b;
  vofc_a.resize(m_ngain);
  vofc_b.resize(m_ngain);
  for (unsigned i=0; i<m_ngain; ++i) {
     vofc_a[i].resize(m_hashMax);
     vofc_b[i].resize(m_hashMax);
  }
  
  std::ifstream myfile(input);
  // we expect two lines per channel with Id, hash and values
  // it contains particular pattern, change it if formatting has changed
  std::string line, line1;
  unsigned long id, id1;
  unsigned hash, hash1;
  char mychar, mychar1;
  while (std::getline(myfile, line),std::getline(myfile, line1)) {
     std::stringstream st(line);
     st>>std::hex>>id>>std::dec>>hash;
     std::stringstream st1(line1);
     st>>std::hex>>id1>>std::dec>>hash1;
     // check equality of Id and has in both lines
     if(id != id1 || hash != hash1) {
        ATH_MSG_ERROR("Different input id or hash: "<<line<<" "<<line1);
        continue;
     }
     // Now it should contain a and b chars
     st>>mychar;
     st1>>mychar1;
     if(mychar != 'a' || mychar1 != 'b') {
        ATH_MSG_ERROR("Wrong OFC type: "<<line<<" "<<line1);

     }
     
     // check if nsamples corresponds to asked one, we expect the same number in each gain
  }

  unsigned nChannels=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("OFCa", "blob");
  spec->extend("OFCb", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  auto  collOFC = std::make_unique<CondAttrListCollection>(true);
  
  float timeOffset=0.;

  for (unsigned gain=0;gain<m_ngain;++gain) {

    auto attrList = std::make_unique<coral::AttributeList>(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& ofcaBlob=(*attrList)["OFCa"].data<coral::Blob>();
    coral::Blob& ofcbBlob=(*attrList)["OFCb"].data<coral::Blob>();
    coral::Blob& toBlob=(*attrList)["TimeOffset"].data<coral::Blob>();

    (*attrList)["nSamples"].setValue(m_nsample);
      
    ofcaBlob.resize(m_hashMax*sizeof(float)*m_nsample);
    ofcbBlob.resize(m_hashMax*sizeof(float)*m_nsample);
    toBlob.resize(m_hashMax*sizeof(float));
    float* pOfca=static_cast<float*>(ofcaBlob.startingAddress());
    float* pOfcb=static_cast<float*>(ofcbBlob.startingAddress());
    float* pTimeOffset=static_cast<float*>(toBlob.startingAddress());
    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      LArOFCFlat::OFCRef_t ofca(vofc_a[gain][hs]);
      LArOFCFlat::OFCRef_t ofcb(vofc_b[gain][hs]);

      if (ofca.size()==m_nsample) {
	for (unsigned i=0;i<m_nsample;++i) {
	  pOfca[hs*m_nsample+i]=ofca[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << m_nsample << ", got " << ofca.size() << "."; 
	errIfConnected(chid,gain,"OFCa", message.str());
	for (unsigned i=0;i<m_nsample;++i) {
	  pOfca[hs*m_nsample+i]=1.0;
	}
	++nDefault;
      } 
     
      if (ofcb.size()==m_nsample) {
	for (unsigned i=0;i<m_nsample;++i) {
	  pOfcb[hs*m_nsample+i]=ofcb[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << m_nsample << ", got " << ofcb.size() << "."; 
	errIfConnected(chid,gain,"OFCb", message.str());
	for (unsigned i=0;i<m_nsample;++i) {
	  pOfcb[hs*m_nsample+i]=0.0;
	}
      }
      pTimeOffset[hs]=timeOffset;
      ++nChannels;

    }//end loop over hash ids
    collOFC->add(gain,*(attrList.release()));
  }//end loop over gains
    
  StatusCode sc=detStore()->record(std::move(collOFC),outputName);//"/LAR/ElecCalibFlat/OFC");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection OFC with key " << outputName );
    return;
  }

  ATH_MSG_INFO( "Converted OFCs to inline storage. Total number of channels=" << nChannels );
  ATH_MSG_INFO( "Number of channels filled with default OFCs {1,1,1,1} " << nDefault << " (including disconnected)" );
  return;
}

 

StatusCode LArFlatFromFile::stop() {  


  ATH_CHECK( detStore()->retrieve(m_onlineID));
  ATH_CHECK( detStore()->retrieve(m_onlineSCID));

  if(m_isSC) 
     m_hashMax=m_onlineSCID->channelHashMax();
  else 
     m_hashMax=m_onlineID->channelHashMax();
  


  //OFC:
  if (m_OFCInput.size()) {
    std::ifstream myfile(m_OFCInput);
    // new lines will be skipped unless we stop it from happening:    
    myfile.unsetf(std::ios_base::skipws);

    // count the newlines with an algorithm specialized for counting:
    unsigned line_count = std::count( std::istream_iterator<char>(myfile),
                                      std::istream_iterator<char>(), '\n');
    myfile.close();
    if (2*m_hashMax != line_count) {
	ATH_MSG_ERROR( "Failed to check file with OFCs" );
	return StatusCode::FAILURE;
    } else {
      if(m_isSC) 
         ofcFlat(m_OFCInput,"/LAR/ElecCalibSC/OFC");
      else
         ofcFlat(m_OFCInput,"/LAR/ElecCalibFlat/OFC");
    }
  }//end have m_OFCInput


  return StatusCode::SUCCESS; 
}





void LArFlatFromFile::errIfConnected(const HWIdentifier chid, const int gain, const std::string& objName, const std::string& message) const{

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
     return;
  }

  if (cabling->isOnlineConnected(chid)) {
    if (! (gain==2 && m_onlineID->isEMBPS(chid))) { //No LG Presampler calibration
      ATH_MSG_ERROR( "No valid " << objName << " found for channel "  << m_onlineID->channel_name(chid) << ", gain " << gain << ". ");
      if (message.size()>0) ATH_MSG_ERROR( message );
      ATH_MSG_ERROR( " Filling with default value." );
    }
  }
  return;
}
