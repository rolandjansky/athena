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

  declareProperty("ForceStop",m_forceStop=true);
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
      if (!withFCAL && m_onlineID->isFCALchannel(chid)) {
	pblob[hs]=1.0; //Fill fcal channels with 1.0. for MPhysOverMcal
      }
      else {
	pblob[hs]=input->get(chid,gain).m_data;
      }

      if (pblob[hs] < 0) {
	errIfConnected(chid,gain,blobName);
      } 
    }
    unsigned coolChan=gain;
    //Special case: Store single-gain constant in channel 1 
    //To avoid AttrList vs AttrListCollection confusion in Athena DB infrastructure
    if (nGain==1) coolChan=1;

    coll->add(coolChan,*attrList);
    //delete attrList;//???
  }

  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << endreq;
    return NULL;
  }
  return coll;
}



CondAttrListCollection* LArCompleteToFlat::pedestalFlat(const ILArPedestal* input, const std::string& outputName) {    
  msg(MSG::INFO)<<"LArCompleteToFlat::pedestalFlat, starting"<<endreq;

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
      pblobPed[hs]=input->pedestal(chid,gain);
      pblobRMS[hs]=input->pedestalRMS(chid,gain);
    }//end loop over hash ids
    collPed->add(gain,*attrList);
  }//end loop over gains
   
  StatusCode sc=detStore()->record(collPed,outputName);//"/LAR/ElecCalibFlat/Pedestal");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection for pedestal with key " << outputName << endreq;
    return NULL;
  }
  msg(MSG::INFO)<<"LArCompleteToFlat::pedestalFlat, returning "<<collPed<<endreq;
  return collPed;
}


CondAttrListCollection* LArCompleteToFlat::ofcFlat(const ILArOFC* input, const std::string& outputName) {
  const unsigned nSamples=5;
 
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("OFCa", "blob");
  spec->extend("OFCb", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  CondAttrListCollection* collOFC=new CondAttrListCollection(true);
  
  for (unsigned gain=0;gain<3;++gain) {
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
      if (ofca.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfca[hs*nSamples+i]=ofca[i];
	}
      }
      else {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfca[hs*nSamples+i]=LArOFCFlat::ERRORCODE;
	}
      } 
     
      LArOFCFlat::OFCRef_t ofcb= input->OFC_b(chid,gain);
      if (ofcb.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfcb[hs*nSamples+i]=ofcb[i];
	}
      }
      else {
	for (unsigned i=0;i<nSamples;++i) {
	  pOfcb[hs*nSamples+i]=LArOFCFlat::ERRORCODE;
	}
      }
      pTimeOffset[hs]=input->timeOffset(chid,gain);

    }//end loop over hash ids
    collOFC->add(gain,*attrList);
  }//end loop over gains
    
  StatusCode sc=detStore()->record(collOFC,outputName);//"/LAR/ElecCalibFlat/OFC");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection OFC with key " << outputName << endreq;
    return NULL;
  }
  return collOFC;
}


CondAttrListCollection* LArCompleteToFlat::shapeFlat(const LArShapeComplete* input, const std::string& outputName) {
  const unsigned nSamples=5;
 
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("Shape", "blob");
  spec->extend("ShapeDer", "blob");
  spec->extend("TimeOffset","blob");
  spec->extend<unsigned>("nSamples");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  
  for (unsigned gain=0;gain<3;++gain) {
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
      if (shape.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pShape[hs*nSamples+i]=shape[i];
	}
      }
      else {
	for (unsigned i=0;i<nSamples;++i) {
	  pShape[hs*nSamples+i]=LArShapeFlat::ERRORCODE;
	}
      } 
     
      ILArShape::ShapeRef_t shapeDer= input->ShapeDer(chid,gain);
      if (shapeDer.size()==nSamples) {
	for (unsigned i=0;i<nSamples;++i) {
	  pShapeDer[hs*nSamples+i]=shapeDer[i];
	}
      }
      else {
	for (unsigned i=0;i<nSamples;++i) {
	  pShapeDer[hs*nSamples+i]=LArShapeFlat::ERRORCODE;
	}
      }
      pTimeOffset[hs]=input->timeOffset(chid,gain);

    }//end loop over hash ids
    coll->add(gain,*attrList);
  }//end loop over gains
    
  StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibFlat/SHAPE");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection Shape with key " << outputName << endreq;
    return NULL;
  }
  return coll;
}



CondAttrListCollection* LArCompleteToFlat::rampFlat(const ILArRamp* input, const std::string& outputName) {
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("RampVec", "blob");
  spec->extend<unsigned>("nPoints");
  spec->extend<unsigned>("version");
  CondAttrListCollection* coll=new CondAttrListCollection(true);
  const unsigned nPoints=2;

  for (unsigned gain=0;gain<3;++gain) {
    coral::AttributeList* attrList = new coral::AttributeList(*spec);
    (*attrList)["version"].setValue(0U);
    coral::Blob& blobRamp=(*attrList)["RampVec"].data<coral::Blob>();
    (*attrList)["nPoints"].setValue(nPoints);
    blobRamp.resize(m_hashMax*sizeof(float)*nPoints);
    float* pblobRamp=static_cast<float*>(blobRamp.startingAddress());

    for (unsigned hs=0;hs<m_hashMax;++hs) {
      const HWIdentifier chid=m_onlineID->channel_Id(hs);
      const ILArRamp::RampRef_t ramp=input->ADC2DAC(chid,gain);
      if (ramp.size()>=nPoints) {
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=ramp[i];
	}
      }
      else {
	errIfConnected(chid,gain,"Ramp");
	//msg(MSG::ERROR) << "Found ramp polynom with degree " << ramp.size() << ". Need at least " 
	//	<< nPoints << ". Filling with with ERRORCODE" << endreq;
	for (size_t i=0;i<nPoints;++i) {
	  pblobRamp[nPoints*hs+i]=LArElecCalib::ERRORCODE;
	}
      }
    }//end loop over hash ids
    coll->add(gain,*attrList);
  }//end loop over gains
     
  StatusCode sc=detStore()->record(coll,outputName);//"/LAR/ElecCalibFlat/Ramp");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection for ramp with key " << outputName << endreq;
    return NULL;
  }
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
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << endreq;
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
    msg(MSG::ERROR) << "Failed to record CondAttrListCollection with key" << outputName << endreq;
    return NULL;
  }
  return coll;
}

AthenaAttributeList* LArCompleteToFlat::DSPThresholdsFlat(const LArDSPThresholdsComplete* input, const std::string& outputName) {
 
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("tQThr", "blob");
  spec->extend("samplesThr", "blob");
  spec->extend("trigSumThr","blob");
  spec->extend<unsigned>("version");
  
  coral::AttributeList* attrList = new coral::AttributeList(*spec);    
  (*attrList)["version"].setValue(0U);
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

     msg(MSG::INFO) << "hwid: "<<chid.getString()<<" "<<hs<<" | "<<ptQThr[hs]<<" : "<<pSamplesThr[hs]<<" | "<<pTrigThr[hs] << endreq;
  }//end loop over hash ids

  AthenaAttributeList* coll=new AthenaAttributeList(*attrList);
    
  StatusCode sc=detStore()->record(coll,outputName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to record AthenaAttributeList DSPThresholds with key " << outputName << endreq;
    return NULL;
  }

  return coll;
}



StatusCode LArCompleteToFlat::stop() {  


  StatusCode sc=detStore()->retrieve(m_onlineID);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to get LArOnlineID" << endreq;
    return sc;
  }

  m_hashMax=m_onlineID->channelHashMax();
  

  //CondAttrListCollection* coll;
  
  if (m_uA2MeVInput.size()) {
    const ILAruA2MeV* uA2MeVComplete;
    sc=detStore()->retrieve(uA2MeVComplete,m_uA2MeVInput);
    if (sc.isFailure()) {
      if(m_forceStop) { 
	msg(MSG::ERROR) << "Failed to get LAruA2MeV with key " << m_uA2MeVInput << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LAruA2MeV" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArDAC2uA with key " << m_DAC2uAInput << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArDAC2uA" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArMphysOverMcal with key " << m_MphysOverMcalInput << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArMphysOverMcal" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArHVScaleCorr with key " << m_HVScaleCorrInput << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArHVScaleCorr" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArPedestal" << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArPedestal" << endreq;
      }   
    } else {
      pedestalFlat(pedComplete,"/LAR/ElecCalibFlat/Pedestal");

      /*
     CondAttrListCollection* coll=pedestalFlat(pedComplete,"/LAR/ElecCalibFlat/Pedestal");
     LArPedestalFlat* pf=new LArPedestalFlat(coll);
  
     sc=detStore()->record(pf,"PedestalFlat");
     if (sc.isFailure()) {
       msg(MSG::ERROR) << "Failed to record LArPedestalFlat" << endreq;
     }
   
     ILArPedestal* iped=pf;
     sc=detStore()->symLink(pf,iped);
     if (sc.isFailure()) {
       msg(MSG::ERROR) << "Failed to symlink LArPedestalFlat" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArOFCComplete object" << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArOFCComplete" << endreq;
      }
    } else {
      ofcFlat(ofcComplete,"/LAR/ElecCalibFlat/OFC");
      /*  
      CondAttrListCollection* coll=ofcFlat(ofcComplete,"/LAR/ElecCalibFlat/OFC");
      
	LArOFCFlat* of=new LArOFCFlat(coll);
     
	sc=detStore()->record(of,"OFCFlat");
	if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to record LArOFCFlat" << endreq;
	}
   
	ILArOFC* iofc=of;
	sc=detStore()->symLink(of,iofc);
	if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to symlink LArOFCFlat" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArShapeComplete object" << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArShapeComplete" << endreq;
      }   
    } else {
      shapeFlat(shapeComplete,"/LAR/ElecCalibFlat/Shape");
      /*
      CondAttrListCollection* coll=shapeFlat(shapeComplete,"/LAR/ElecCalibFlat/Shape");

      LArShapeFlat* sf=new LArShapeFlat(coll);
     
      sc=detStore()->record(sf,"ShapeFlat");
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to record LArShapeFlat" << endreq;
      }
   
      ILArShape* ishape=sf;
      sc=detStore()->symLink(sf,ishape);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to symlink LArShapeFlat" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArRampComplete object" << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArRampComplete" << endreq;
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
	msg(MSG::ERROR) << "Failed to get LArDSPThresholdsComplete object" << endreq;
	return sc;
      } else {
	msg(MSG::WARNING) << "Will not process LArDSPThresholdsComplete" << endreq;
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
	msg(MSG::ERROR) << "Failed to record LArDSPThresholdsFlat" << endreq;
      }
    } 
    */
  } //end if have m__DSPThresholdsInput
  

  return StatusCode::SUCCESS; 
}





void LArCompleteToFlat::errIfConnected(const HWIdentifier chid, const int gain, const char* objName) const{

  if (m_cablingSvc->isOnlineConnected(chid)) {
    msg(MSG::ERROR) << "No valid " << objName << " found for channel "  << m_onlineID->channel_name(chid) 
  		    << ", gain " << gain << ". Filling with ERRORCODE." << endreq;
  }
  return;
}
