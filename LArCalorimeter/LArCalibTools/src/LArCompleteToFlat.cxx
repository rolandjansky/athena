/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArCompleteToFlat.h"

#include "LArRawConditions/LArPedestalComplete.h"
#include "LArCOOLConditions/LArPedestalFlat.h"

#include "LArRawConditions/LArOFCComplete.h"
#include "LArCOOLConditions/LArOFCFlat.h"

#include "LArRawConditions/LArDAC2uAMC.h"
#include "LArRawConditions/LAruA2MeVMC.h"
#include "LArRawConditions/LArHVScaleCorrComplete.h"
#include "LArRawConditions/LArMphysOverMcalComplete.h"

#include "LArRawConditions/LArShapeComplete.h"
#include "LArCOOLConditions/LArShapeFlat.h"

#include "LArRawConditions/LArRampComplete.h"

#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/Blob.h"


#include "LArElecCalib/LArCalibErrorCode.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
LArCompleteToFlat::LArCompleteToFlat( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_hashMax(0),
  m_onlineID(0),
  m_cablingSvc("LArCablingService")
{

  declareProperty("uA2MeVInput",m_uA2MeVInput);//="LAruA2MeV");
  declareProperty("DAC2uAVInput",m_DAC2uAInput);//="LArDAC2uA");
  declareProperty("HVScaleCorrInput",m_HVScaleCorrInput);//="LArHVScaleCorr");
  declareProperty("PedestalInput",m_PedestalInput);//="Pedestal");
  declareProperty("RampInput",m_RampInput);//="LArRamp");
  declareProperty("MphysOverMcalInput",m_MphysOverMcalInput);//="LArMphysOverMcal");
  declareProperty("OFCInput",m_OFCInput);//="LArOFC");
  declareProperty("ShapeInput",m_ShapeInput);//="LArShape");
  declareProperty("DSPThresholdsInput",m_DSPThresholdsInput);//="LArDSPThresholds");
  declareProperty("NameOfSet",m_nameOfSet); // for DSPThreshold

  declareProperty("ForceStop",m_forceStop=true);

  declareProperty("FakeEMBPSLowGain",m_fakeEMBPSLowGain=false);
  
}

// Destructor
///////////////
LArCompleteToFlat::~LArCompleteToFlat()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode LArCompleteToFlat::initialize()
{
  CHECK(m_cablingSvc.retrieve());
  return StatusCode::SUCCESS;
}



CondAttrListCollection* LArCompleteToFlat::singleFloatFlat(const char* blobName, const LArConditionsContainer<LArSingleFloatP>* input, 
							   const std::string& outputName, const unsigned nGain,const bool withFCAL) {

  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend(blobName, "blob");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  for (unsigned gain=0;gain<nGain;++gain) {
    coral::AttributeList* attrList = new coral::AttributeList(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& blob=(*attrList)[blobName].data<coral::Blob>();
    blob.resize(m_hashMax*sizeof(float));
    float* pblob=static_cast<float*>(blob.startingAddress());
    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      float value;
      if (!withFCAL && m_onlineID->isFCALchannel(chid)) {
	value=1.0; //Fill fcal channels with 1.0. for MPhysOverMcal
      }
      else
	value=input->get(chid,gain).m_data;

      if (value<0 && gain==2 && m_fakeEMBPSLowGain) { 
	//Fill medium gain for EMB PS low gain
	value=input->get(chid,1).m_data;
	++nCopiedEMPS;
      }
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

    coll->add(coolChan,*attrList);
    //delete attrList;//???
  }

  ATH_MSG_INFO( "Converted " << blobName << " to inline storage. Total number of channels=" << nChannels );
  ATH_MSG_INFO( "Number of channels filled with default value (1.0) " << nDefault << " (including disconnected)" );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain" <<  nCopiedEMPS );  
  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection with key" << outputName );
    return NULL;
  }
  return coll;
}



CondAttrListCollection* LArCompleteToFlat::pedestalFlat(const ILArPedestal* input, const std::string& outputName) { 
  ATH_MSG_INFO("LArCompleteToFlat::pedestalFlat, starting");

  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("Pedestal", "blob");
  spec->extend("PedestalRMS", "blob");
  spec->extend<unsigned>("version");

  CondAttrListCollection* collPed=new CondAttrListCollection(true);

  for (unsigned gain=0;gain<3;++gain) {
    coral::AttributeList* attrList = new coral::AttributeList(*spec);               
    (*attrList)["version"].setValue(0U);                               
    coral::Blob& blobPed=(*attrList)["Pedestal"].data<coral::Blob>();
    coral::Blob& blobRMS=(*attrList)["PedestalRMS"].data<coral::Blob>();
    blobPed.resize(m_hashMax*sizeof(float));
    blobRMS.resize(m_hashMax*sizeof(float));
    float* pblobPed=static_cast<float*>(blobPed.startingAddress());
    float* pblobRMS=static_cast<float*>(blobRMS.startingAddress());

    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      float ped=input->pedestal(chid,gain);
      float pedRMS=input->pedestalRMS(chid,gain);
      if (ped<0 && gain==2 && m_fakeEMBPSLowGain) {
	ped=input->pedestal(chid,1);
	pedRMS=input->pedestalRMS(chid,1);
	++nCopiedEMPS;
      }

      pblobPed[hs]=ped;
      pblobRMS[hs]=pedRMS;
      ++nChannels; 
    }//end loop over hash ids
    collPed->add(gain,*attrList);
  }//end loop over gains
   
  StatusCode sc=detStore()->record(collPed,outputName);//"/LAR/ElecCalibFlat/Pedestal");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection for pedestal with key " << outputName );
    return NULL;
  }
  ATH_MSG_INFO( "Converted Pedestal to inline storage. Total number of channels=" << nChannels );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain" <<  nCopiedEMPS );    

  return collPed;
}


CondAttrListCollection* LArCompleteToFlat::ofcFlat(const ILArOFC* input, const std::string& outputName) {

  ATH_MSG_INFO("LArCompleteToFlat::ofcFlat, starting");
  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("OFCa", "blob");
  spec->extend("OFCb", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  CondAttrListCollection* collOFC=new CondAttrListCollection(true);
  
  for (unsigned gain=0;gain<3;++gain) {

    //Auto-detect the number of samples (at least in theory, could be different for each gain)
    unsigned nSamples=0;
    for (unsigned hs=0;hs<m_hashMax && nSamples==0;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      LArOFCFlat::OFCRef_t ofca= input->OFC_a(chid,gain);
      nSamples=ofca.size();
    }
    if (nSamples==0) {
      ATH_MSG_ERROR( "All input OFCs for gain " << gain << " have 0 samples!" );
      continue;//jump to the next gain
    }
    
    ATH_MSG_INFO( "Gain " << gain <<": Found " << nSamples << " OFC samples in input data" );

    coral::AttributeList* attrList = new coral::AttributeList(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& ofcaBlob=(*attrList)["OFCa"].data<coral::Blob>();
    coral::Blob& ofcbBlob=(*attrList)["OFCb"].data<coral::Blob>();
    coral::Blob& toBlob=(*attrList)["TimeOffset"].data<coral::Blob>();

    (*attrList)["nSamples"].setValue(nSamples);
      
    ofcaBlob.resize(m_hashMax*sizeof(float)*nSamples);
    ofcbBlob.resize(m_hashMax*sizeof(float)*nSamples);
    toBlob.resize(m_hashMax*sizeof(float));
    float* pOfca=static_cast<float*>(ofcaBlob.startingAddress());
    float* pOfcb=static_cast<float*>(ofcbBlob.startingAddress());
    float* pTimeOffset=static_cast<float*>(toBlob.startingAddress());
    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      LArOFCFlat::OFCRef_t ofca= input->OFC_a(chid,gain);
      LArOFCFlat::OFCRef_t ofcb= input->OFC_b(chid,gain);
      float timeOffset=input->timeOffset(chid,gain);
      if (ofca.size()==0 && gain==2 && m_fakeEMBPSLowGain) {
	ofca= input->OFC_a(chid,1);
	ofcb= input->OFC_b(chid,1);
	timeOffset=input->timeOffset(chid,1);
	++nCopiedEMPS;
      }

      if (ofca.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfca[hs*nSamples+i]=ofca[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << nSamples << ", got " << ofca.size() << "."; 
	errIfConnected(chid,gain,"OFCa", message.str().c_str());
	for (unsigned i=0;i<nSamples;++i) {
	  pOfca[hs*nSamples+i]=1.0;
	}
	++nDefault;
      } 
     
      if (ofcb.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfcb[hs*nSamples+i]=ofcb[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << nSamples << ", got " << ofcb.size() << "."; 
	errIfConnected(chid,gain,"OFCb", message.str().c_str());
	for (unsigned i=0;i<nSamples;++i) {
	  pOfcb[hs*nSamples+i]=0.0;
	}
      }
      pTimeOffset[hs]=timeOffset;
      ++nChannels;

    }//end loop over hash ids
    collOFC->add(gain,*attrList);
  }//end loop over gains
    
  StatusCode sc=detStore()->record(collOFC,outputName);//"/LAR/ElecCalibFlat/OFC");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection OFC with key " << outputName );
    return NULL;
  }

  ATH_MSG_INFO( "Converted OFCs to inline storage. Total number of channels=" << nChannels );
  ATH_MSG_INFO( "Number of channels filled with default OFCs {1,1,1,1} " << nDefault << " (including disconnected)" );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain" <<  nCopiedEMPS );  
  return collOFC;
}

 
CondAttrListCollection* LArCompleteToFlat::shapeFlat(const LArShapeComplete* input, const std::string& outputName) {
 
  ATH_MSG_INFO("LArCompleteToFlat::shapeFlat, starting");

  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("Shape", "blob");
  spec->extend("ShapeDer", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  
  for (unsigned gain=0;gain<3;++gain) {

    unsigned nSamples=0;
    for (unsigned hs=0;hs<m_hashMax && nSamples==0;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      ILArShape::ShapeRef_t shape= input->Shape(chid,gain);
      nSamples=shape.size();
    }
    if (nSamples==0) {
      ATH_MSG_ERROR( "All input Shapes for gain " << gain << " have 0 samples!" );
      continue;//jump to the next gain
    }

    ATH_MSG_INFO( "Gain " << gain <<": Found " << nSamples << " shape samples in input data" );

    coral::AttributeList* attrList = new coral::AttributeList(*spec);    
    (*attrList)["version"].setValue(0U);
    coral::Blob& shapeBlob=(*attrList)["Shape"].data<coral::Blob>();
    coral::Blob& shapeDerBlob=(*attrList)["ShapeDer"].data<coral::Blob>();
    coral::Blob& toBlob=(*attrList)["TimeOffset"].data<coral::Blob>();

    (*attrList)["nSamples"].setValue(nSamples);
      
    shapeBlob.resize(m_hashMax*sizeof(float)*nSamples);
    shapeDerBlob.resize(m_hashMax*sizeof(float)*nSamples);
    toBlob.resize(m_hashMax*sizeof(float));
    float* pShape=static_cast<float*>(shapeBlob.startingAddress());
    float* pShapeDer=static_cast<float*>(shapeDerBlob.startingAddress());
    float* pTimeOffset=static_cast<float*>(toBlob.startingAddress());
    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      
      ILArShape::ShapeRef_t shape= input->Shape(chid,gain);
      ILArShape::ShapeRef_t shapeDer= input->ShapeDer(chid,gain);
      float timeOffset=input->timeOffset(chid,gain);
      if (shape.size()==0 && gain==2 && m_fakeEMBPSLowGain) { 
	 shape=input->Shape(chid,1);
	 shapeDer=input->ShapeDer(chid,1);
	 timeOffset=input->timeOffset(chid,1);
	 ++nCopiedEMPS;
      }
   
      if (shape.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pShape[hs*nSamples+i]=shape[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << nSamples << ", got " << shape.size() << "."; 
	errIfConnected(chid,gain,"Shape", message.str().c_str());
	for (unsigned i=0;i<nSamples;++i) {
	  pShape[hs*nSamples+i]=0.0;
	}
	++nDefault;
      } 
     
   
      if (shapeDer.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pShapeDer[hs*nSamples+i]=shapeDer[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Number of samples don't match. Expect " << nSamples << ", got " << shapeDer.size() << "."; 
	errIfConnected(chid,gain,"ShapeDer", message.str().c_str());
	for (unsigned i=0;i<nSamples;++i) {
	  pShapeDer[hs*nSamples+i]=0.0;
	}
      }
      pTimeOffset[hs]=timeOffset;
      ++nChannels;

    }//end loop over hash ids
    coll->add(gain,*attrList);
  }//end loop over gains
    
  StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibFlat/SHAPE");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection Shape with key " << outputName );
    return NULL;
  }

  ATH_MSG_INFO( "Converted Shapes to inline storage. Total number of channels=" << nChannels );
   ATH_MSG_INFO( "Number of channels filled with default shape {0,0,0,0} " << nDefault << " (including disconnected)" );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain" <<  nCopiedEMPS );  
  
  return coll;
}



CondAttrListCollection* LArCompleteToFlat::rampFlat(const ILArRamp* input, const std::string& outputName) {

  ATH_MSG_INFO("LArCompleteToFlat::rampFlat, starting");

  unsigned nChannels=0;
  unsigned nCopiedEMPS=0;
  unsigned nDefault=0;

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("RampVec", "blob");
  spec->extend<unsigned>("nPoints");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);


  std::vector<float> defaultRamp={0.0,1.0};

  for (unsigned gain=0;gain<3;++gain) {

    unsigned nPoints=0;
    for (unsigned hs=0;hs<m_hashMax && nPoints==0;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      const ILArRamp::RampRef_t ramp= input->ADC2DAC(chid,gain);
      nPoints=ramp.size();
    }
    if (nPoints==0) {
      ATH_MSG_ERROR( "All input Ramps for gain " << gain << " have 0 points!" );
      continue;//jump to the next gain
    }

    defaultRamp.resize(nPoints,0.0); //fill remaining points if needed
    ATH_MSG_INFO( "Gain " << gain << ": Found a ramp polynom of degree " << nPoints << " in input data" );
    coral::AttributeList* attrList = new coral::AttributeList(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& blobRamp=(*attrList)["RampVec"].data<coral::Blob>();
    (*attrList)["nPoints"].setValue(nPoints);
    blobRamp.resize(m_hashMax*sizeof(float)*nPoints);
    float* pblobRamp=static_cast<float*>(blobRamp.startingAddress());

    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      std::vector<float> rampVec(input->ADC2DAC(chid,gain).asVector());
      if (rampVec.size()==0 && gain==2 && m_fakeEMBPSLowGain && m_cablingSvc->isOnlineConnected(chid)) { 
	rampVec=input->ADC2DAC(chid,1).asVector();
	rampVec[1]*=10.0;
	++nCopiedEMPS;
      }
      
      if (rampVec.size()>=nPoints) {
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=rampVec[i];
	}
      }
      else {
	std::stringstream message;
	message <<"Polynom degree doesn't match. Expect " << nPoints << ", got " << rampVec.size() << "."; 
	errIfConnected(chid,gain,"Ramp", message.str().c_str());
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=defaultRamp[i];
	}
	++nDefault;
      }
      ++nChannels;
    }//end loop over hash ids
    coll->add(gain,*attrList);
  }//end loop over gains
  
  StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibFlat/Ramp");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection for ramp with key " << outputName );
    return NULL;
  }

  ATH_MSG_INFO( "Converted Ramps to inline storage. Total number of channels " << nChannels );
  ATH_MSG_INFO( "Number of channels filled with default ramp {0,1} " << nDefault << " (including disconnected)" );
  if (nCopiedEMPS) 
    ATH_MSG_INFO( "\t Number of low gain EMBPS channels copied from medium gain (applied factor 10)" <<  nCopiedEMPS );  
  

  return coll;
}


CondAttrListCollection* LArCompleteToFlat::DAC2uAFlat(const ILArDAC2uA* input, const std::string& outputName) {
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("DAC2uA", "blob");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  coral::AttributeList* attrList = new coral::AttributeList(*spec);
  (*attrList)["version"].setValue(0U);
  coral::Blob& blob=(*attrList)["DAC2uA"].data<coral::Blob>();
  blob.resize(m_hashMax*sizeof(float));
  float* pblob=static_cast<float*>(blob.startingAddress());
  for (unsigned hs=0;hs<m_hashMax;++hs) {
    const HWIdentifier chid=m_onlineID->channel_Id(hs);
    pblob[hs]=input->DAC2UA(chid);
  }
  coll->add(1,*attrList);
  //delete attrList;//???
  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection with key" << outputName );
    return NULL;
  }
  return coll;
}


CondAttrListCollection* LArCompleteToFlat::uA2MeVFlat(const ILAruA2MeV* input, const std::string& outputName) {
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("uA2MeV", "blob");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  coral::AttributeList* attrList = new coral::AttributeList(*spec);
  (*attrList)["version"].setValue(0U);
  coral::Blob& blob=(*attrList)["uA2MeV"].data<coral::Blob>();
  blob.resize(m_hashMax*sizeof(float));
  float* pblob=static_cast<float*>(blob.startingAddress());
  for (unsigned hs=0;hs<m_hashMax;++hs) {
    const HWIdentifier chid=m_onlineID->channel_Id(hs);
    pblob[hs]=input->UA2MEV(chid);
  }
  coll->add(1,*attrList);
  //delete attrList;//???
  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record CondAttrListCollection with key" << outputName );
    return NULL;
  }
  return coll;
}

AthenaAttributeList* LArCompleteToFlat::DSPThresholdsFlat(const LArDSPThresholdsComplete* input, const std::string& outputName) {
 
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("tQThr", "blob");
  spec->extend("samplesThr", "blob");
  spec->extend("trigSumThr","blob");
  //spec->extend<unsigned>("version");
  spec->extend("Name","string");
  
  coral::AttributeList* attrList = new coral::AttributeList(*spec);    
  //(*attrList)["version"].setValue(0U);
  (*attrList)["Name"].setValue(m_nameOfSet);
  coral::Blob& QBlob=(*attrList)["tQThr"].data<coral::Blob>();
  coral::Blob& SamplesBlob=(*attrList)["samplesThr"].data<coral::Blob>();
  coral::Blob& TrigBlob=(*attrList)["trigSumThr"].data<coral::Blob>();

  QBlob.resize(m_hashMax*sizeof(float));
  SamplesBlob.resize(m_hashMax*sizeof(float));
  TrigBlob.resize(m_hashMax*sizeof(float));
  float* ptQThr=static_cast<float*>(QBlob.startingAddress());
  float* pSamplesThr=static_cast<float*>(SamplesBlob.startingAddress());
  float* pTrigThr=static_cast<float*>(TrigBlob.startingAddress());

  for (unsigned hs=0;hs<m_hashMax;++hs) {
    const HWIdentifier chid=m_onlineID->channel_Id(hs);
    
    ptQThr[hs] = input->tQThr(chid);
    pSamplesThr[hs] = input->samplesThr(chid);
    pTrigThr[hs] = input->trigSumThr(chid);

     ATH_MSG_INFO( "hwid: "<<chid.getString()<<" "<<hs<<" | "<<ptQThr[hs]<<" : "<<pSamplesThr[hs]<<" | "<<pTrigThr[hs] );
  }//end loop over hash ids

  AthenaAttributeList* coll=new AthenaAttributeList(*attrList);
    
  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to record AthenaAttributeList DSPThresholds with key " << outputName );
    return NULL;
  }

  return coll;
}



StatusCode LArCompleteToFlat::stop() {  


  StatusCode sc=detStore()->retrieve(m_onlineID);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to get LArOnlineID" );
    return sc;
  }

  m_hashMax=m_onlineID->channelHashMax();
  

  //CondAttrListCollection* coll;
  
  if (m_uA2MeVInput.size()) {
    const ILAruA2MeV* uA2MeVComplete;
    sc=detStore()->retrieve(uA2MeVComplete,m_uA2MeVInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LAruA2MeV with key " << m_uA2MeVInput );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LAruA2MeV" );
      }
    } else {
      uA2MeVFlat(uA2MeVComplete, "/LAR/ElecCalibFlat/uA2MeV");
    }
  }//end if have m_uA2MeV
    

  if (m_DAC2uAInput.size()) {
    const ILArDAC2uA* DAC2uAComplete;
    sc=detStore()->retrieve(DAC2uAComplete,m_DAC2uAInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArDAC2uA with key " << m_DAC2uAInput );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArDAC2uA" );
      }  
    } else {
      DAC2uAFlat(DAC2uAComplete, "/LAR/ElecCalibFlat/DAC2uA");
      //singleFloatFlat("DAC2uA", DAC2uAComplete, "/LAR/ElecCalibFlat/DAC2uA",1);
    }
  }//end if have m_DAC2uAInput

  if (m_MphysOverMcalInput.size()) {
    const LArMphysOverMcalComplete* MphysOverMcalComplete;
    sc=detStore()->retrieve(MphysOverMcalComplete,m_MphysOverMcalInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArMphysOverMcal with key " << m_MphysOverMcalInput );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArMphysOverMcal" );
      }   
    } else {
      singleFloatFlat("MphysOverMcal", MphysOverMcalComplete, "/LAR/ElecCalibFlat/MphysOverMcal",3,false); //No MphysOverMCal for FCAL
    }
  }//end if have m_MphysOverMcalInput


  if (m_HVScaleCorrInput.size()) {
    const LArHVScaleCorrComplete* HVScaleCorrComplete;
    sc=detStore()->retrieve(HVScaleCorrComplete,m_HVScaleCorrInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArHVScaleCorr with key " << m_HVScaleCorrInput );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArHVScaleCorr" );
      }   
    } else {
      singleFloatFlat("HVScaleCorr", HVScaleCorrComplete, "/LAR/ElecCalibFlat/HVScaleCorr",1);
    }
  }//end if have m_HVScaleCorrInput

  if (m_PedestalInput.size()) {
    const LArPedestalComplete* pedComplete;
    sc=detStore()->retrieve(pedComplete,m_PedestalInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArPedestal" );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArPedestal" );
      }   
    } else {
      pedestalFlat(pedComplete,"/LAR/ElecCalibFlat/Pedestal");

      /*
     CondAttrListCollection* coll=pedestalFlat(pedComplete,"/LAR/ElecCalibFlat/Pedestal");
     LArPedestalFlat* pf=new LArPedestalFlat(coll);
  
     sc=detStore()->record(pf,"PedestalFlat");
     if (sc.isFailure()) {
       ATH_MSG_ERROR( "Failed to record LArPedestalFlat" );
     }
   
     ILArPedestal* iped=pf;
     sc=detStore()->symLink(pf,iped);
     if (sc.isFailure()) {
       ATH_MSG_ERROR( "Failed to symlink LArPedestalFlat" );
     }
   }
      */
    }
  }//end if have m_pedestalInput

  //OFC:
  if (m_OFCInput.size()) {
    const LArOFCComplete* ofcComplete;
    sc=detStore()->retrieve(ofcComplete,m_OFCInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArOFCComplete object" );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArOFCComplete" );
      }
    } else {
      ofcFlat(ofcComplete,"/LAR/ElecCalibFlat/OFC");
      /*  
      CondAttrListCollection* coll=ofcFlat(ofcComplete,"/LAR/ElecCalibFlat/OFC");
      
	LArOFCFlat* of=new LArOFCFlat(coll);
     
	sc=detStore()->record(of,"OFCFlat");
	if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to record LArOFCFlat" );
	}
   
	ILArOFC* iofc=of;
	sc=detStore()->symLink(of,iofc);
	if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to symlink LArOFCFlat" );
	}
      */
    }
  }//end have m_OFCInput

  //Shape:
  if (m_ShapeInput.size()) {
    const LArShapeComplete* shapeComplete;
    sc=detStore()->retrieve(shapeComplete,m_ShapeInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArShapeComplete object" );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArShapeComplete" );
      }   
    } else {
      shapeFlat(shapeComplete,"/LAR/ElecCalibFlat/Shape");
      /*
      CondAttrListCollection* coll=shapeFlat(shapeComplete,"/LAR/ElecCalibFlat/Shape");

      LArShapeFlat* sf=new LArShapeFlat(coll);
     
      sc=detStore()->record(sf,"ShapeFlat");
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to record LArShapeFlat" );
      }
   
      ILArShape* ishape=sf;
      sc=detStore()->symLink(sf,ishape);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to symlink LArShapeFlat" );
      }
      */
    }
  }//end if have m_shapeInput

  //Ramp
  if (m_RampInput.size()) { 
    const LArRampComplete* rampComplete;
    sc=detStore()->retrieve(rampComplete,m_RampInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArRampComplete object" );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArRampComplete" );
      }   
    } else {
      rampFlat(rampComplete,"/LAR/ElecCalibFlat/Ramp");
    }
  }
  
  if(m_DSPThresholdsInput.size()) {
    //DSPThresh:
    const LArDSPThresholdsComplete* DSPTComplete;
    sc=detStore()->retrieve(DSPTComplete,m_DSPThresholdsInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	ATH_MSG_ERROR( "Failed to get LArDSPThresholdsComplete object" );
	return sc;
      } else {
	ATH_MSG_WARNING( "Will not process LArDSPThresholdsComplete" );
      }   
    }
    DSPThresholdsFlat(DSPTComplete,"/LAR/Configuration/DSPThresholdFlat/Templates");
    /*
    AthenaAttributeList *cl=DSPThresholdsFlat(DSPTComplete,"/LAR/Configuration/DSPThresholdFlat/Templates");
    
    LArDSPThresholdsFlat* df;
    if(cl) {
      df=new LArDSPThresholdsFlat(cl);
      
      sc=detStore()->record(df,"DSPThresholdsFlat");
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to record LArDSPThresholdsFlat" );
      }
    } 
    */
  } //end if have m__DSPThresholdsInput
  

  return StatusCode::SUCCESS; 
}





void LArCompleteToFlat::errIfConnected(const HWIdentifier chid, const int gain, const char* objName, const char* message) const{

  if (m_cablingSvc->isOnlineConnected(chid)) {
    if (! (gain==2 && m_onlineID->isEMBPS(chid))) { //No LG Presampler calibration
      ATH_MSG_ERROR( "No valid " << objName << " found for channel "  << m_onlineID->channel_name(chid) << ", gain " << gain << ". ");
      if (message) ATH_MSG_ERROR( message );
      ATH_MSG_ERROR( " Filling with default value." );
    }
  }
  return;
}
