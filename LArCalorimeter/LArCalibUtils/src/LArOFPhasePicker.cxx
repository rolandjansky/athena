/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArOFPhasePicker.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "CaloIdentifier/CaloGain.h"


LArOFPhasePicker::LArOFPhasePicker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_inputPhase(nullptr),
  m_onlineID(nullptr),
  m_groupingType(LArConditionsContainerBase::Unknown)
{
  declareProperty("KeyOFC",m_keyOFC="LArOFC_org");
  declareProperty("KeyOFC_new",m_keyOFC_new="LArOFC");
  declareProperty("KeyShape",m_keyShape="LArShape_org");
  declareProperty("KeyShape_new",m_keyShape_new="LArShape");
  declareProperty("KeyPhase",m_keyPhase="");
  declareProperty("GroupingType",m_groupingName="ExtendedSubDetector");
  declareProperty("TimeOffsetCorrection",m_timeOffsetCorr=0);
  declareProperty("DefaultPhase",m_defaultPhase=4);
  declareProperty("doOFC",m_doOFC=true);  
  declareProperty("doShape",m_doShape=true);  
}

LArOFPhasePicker::~LArOFPhasePicker()
{  }

StatusCode LArOFPhasePicker::initialize()
{
  if ( m_groupingName == "Unknown" ) {
     m_groupingType = LArConditionsContainerBase::Unknown ;
  } else if ( m_groupingName == "SingleGroup" ) {
     m_groupingType = LArConditionsContainerBase::SingleGroup ;
  } else if ( m_groupingName == "SubDetector" ) {
     m_groupingType = LArConditionsContainerBase::SubDetectorGrouping ;
  } else if ( m_groupingName == "FeedThrough" ) {
     m_groupingType = LArConditionsContainerBase::FeedThroughGrouping ;
  } else if ( m_groupingName == "ExtendedFeedThrough" ) {
     m_groupingType = LArConditionsContainerBase::ExtendedFTGrouping ;
  } else if ( m_groupingName == "ExtendedSubDetector" ) {
     m_groupingType = LArConditionsContainerBase::ExtendedSubDetGrouping ;
  } else {
     msg(MSG::ERROR)<< "Grouping type " << m_groupingName << " is not foreseen!" << endreq ;
     msg(MSG::ERROR)<< "Only \"Unknown\", \"SingleGroup\", \"SubDetector\", \"FeedThrough\", \"ExtendedFeedThrough\" and \"ExtendedSubDetector\" are allowed" << endreq ;
     return StatusCode::FAILURE ;
  }


  ATH_CHECK(detStore()->retrieve(m_onlineID));

  return StatusCode::SUCCESS;
}

StatusCode LArOFPhasePicker::execute()
{
  return StatusCode::SUCCESS;
}



StatusCode LArOFPhasePicker::stop() {

  ATH_MSG_DEBUG(" In stop() ");

  m_inputPhase=NULL;
  if (m_keyPhase.size()) {
    StatusCode sc=detStore()->retrieve(m_inputPhase,m_keyPhase);
    if (sc.isFailure()) {
      msg(MSG::ERROR)<< "Failed to get input OFC phase with key " << m_keyPhase << endreq;
      msg(MSG::ERROR)<< "Will use default phase !!" << endreq;
      m_inputPhase=NULL;
    }
  }
  else {
    msg(MSG::INFO) << "No StoreGate key for OFC bin given. Will use default phase=" << m_defaultPhase << endreq;
  }


  if (m_doOFC) {
    ATH_CHECK(pickOFC());
  }

  if (m_doShape) {
    ATH_CHECK(pickShape());
  }
  return StatusCode::SUCCESS;
}


StatusCode LArOFPhasePicker::pickOFC() {
  StatusCode sc;
  const LArOFCComplete* inputOFC=NULL;
  sc=detStore()->retrieve(inputOFC,m_keyOFC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to get input OFCs with key " << m_keyOFC << endreq;
    return sc;
  }
  
  LArOFCComplete *larOFCComplete=new LArOFCComplete();
  larOFCComplete->setGroupingType( static_cast<LArConditionsContainerBase::GroupingType>(m_groupingType));
  sc  = larOFCComplete->initialize(); 
  if ( sc.isFailure() ) {
     msg(MSG::ERROR)<< "Could not initialize LArOFCComplete data object - exit!" << endreq ;
     return sc ;
  }
  ATH_MSG_DEBUG("Working on OFC container '"<< m_keyOFC << "' new container will be '" << m_keyOFC_new << "'");


  int count = 0;
  for(unsigned int gain = CaloGain::LARHIGHGAIN; gain < CaloGain::LARNGAIN; ++gain) {
    LArOFCComplete::ConstConditionsMapIterator it=inputOFC->begin(gain);
    LArOFCComplete::ConstConditionsMapIterator it_e=inputOFC->end(gain);
    for(; it!=it_e;++it){
      LArOFCComplete::ConstReference ofc=(*it);
      const HWIdentifier id = it.channelId() ; 
      const int nPhases=ofc.OFC_aSize();
      if (nPhases==0) {
	ATH_MSG_DEBUG("Got empty OFC object for channel " << m_onlineID->channel_name(id) << " (disconnected?)");
	continue;
      }
      count++;
      std::size_t phase=std::min(m_defaultPhase,nPhases-1);

      if(m_inputPhase) {
	 const int p = m_inputPhase->bin(id, gain);
	 if (p>0 && p<nPhases) phase=p;
       }
      ATH_MSG_VERBOSE("OFC picking, gain=" << gain << ", channel "  << m_onlineID->channel_name(id) <<", phase=" << phase);
      ILArOFC::OFCRef_t vOFC_a = ofc.OFC_a(phase);
      ILArOFC::OFCRef_t vOFC_b = ofc.OFC_b(phase);
      const float timeOffset=ofc.timeOffset()+m_timeOffsetCorr;
      //some sanity check on the OFCs
      if ( vOFC_a.size() == 0 || vOFC_b.size() == 0 ) {
	msg(MSG::WARNING) << "OFC not found for gain "<< gain << " channel "  <<  m_onlineID->channel_name(id) << endreq;

      }else if ( vOFC_a.size() != vOFC_b.size() ) {
	msg(MSG::WARNING) << "OFC a (" << vOFC_a.size() << ") and b (" << vOFC_b.size() << ") are not the same size for channel " 
			  <<  m_onlineID->channel_name(id) << endreq;
	msg(MSG::WARNING) << "Will be not exported !!!" << endreq;
      } else { // save in new container
	std::vector<std::vector<float> > OFC_a;
	OFC_a.push_back(vOFC_a.asVector());
	std::vector<std::vector<float> > OFC_b;
	OFC_b.push_back(vOFC_b.asVector());
	larOFCComplete->set(id,(int)gain,OFC_a,OFC_b,timeOffset,25.); //There is no sensible time-bin width for single-phase OFCs 25 seems to make the most sense...
      }
    }
  }

  ATH_MSG_DEBUG(" Selected OFCs for  " << count << " cells");

  sc = detStore()->record(larOFCComplete,  m_keyOFC_new);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to record LArOFCComplete object with key " << m_keyOFC_new << endreq;
    return sc;
  }
  sc = detStore()->symLink(larOFCComplete, (ILArOFC*)larOFCComplete);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to sym-link LArOFCComplete object" << endreq;
    return sc;
  }
  return StatusCode::SUCCESS;
  
}


StatusCode LArOFPhasePicker::pickShape() 
{
  StatusCode sc;

  const LArShapeComplete* inputShape=NULL;
  sc=detStore()->retrieve(inputShape,m_keyShape);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to get input Shapes with key " << m_keyShape << endreq;
    return sc;
  }
  
  LArShapeComplete *larShapeComplete=new LArShapeComplete();
  larShapeComplete->setGroupingType( static_cast<LArConditionsContainerBase::GroupingType>(m_groupingType));
  sc  = larShapeComplete->initialize(); 
  if ( sc.isFailure() ) {
     msg(MSG::ERROR)<< "Could not initialize LArShapeComplete data object - exit!" << endreq ;
     return sc ;
  }

  ATH_MSG_DEBUG("Working on Shape container '"<< m_keyShape << "' new container will be '" << m_keyShape_new << "'");
  int count = 0;
  for(unsigned int gain = CaloGain::LARHIGHGAIN; gain < CaloGain::LARNGAIN; ++gain) {
    LArShapeComplete::ConstConditionsMapIterator it=inputShape->begin(gain);
    LArShapeComplete::ConstConditionsMapIterator it_e=inputShape->end(gain);
    for(; it!=it_e;++it){
      
      LArShapeComplete::ConstReference shape=(*it);
      const HWIdentifier id = it.channelId() ; 
      const int nPhases=shape.shapeSize();
      if (nPhases==0) {
	ATH_MSG_DEBUG("Got empty Shape object for channel " <<  m_onlineID->channel_name(id) << " (disconnected?)");
	continue;
      }
      count++;

      std::size_t phase=std::min(m_defaultPhase,nPhases-1);
      if(m_inputPhase) {
	 const int p = m_inputPhase->bin(id, gain);
	 if (p>0 && p<nPhases) phase=p;
       }

      ATH_MSG_VERBOSE("Shape picking, gain=" << gain << ", channel "  << m_onlineID->channel_name(id) << ", phase=" << phase);
      ILArShape::ShapeRef_t vShape = shape.shape(phase);
      ILArShape::ShapeRef_t vShapeDer = shape.shapeDer(phase);
      const float timeOffset=shape.timeOffset();
      //some sanity check on the Shapes
      if ( vShape.size() == 0 || vShapeDer.size() == 0 ) {
	msg(MSG::WARNING) << "Shape not found for gain "<< gain << " channel " <<  m_onlineID->channel_name(id) << endreq;
      } else if ( vShape.size() != vShapeDer.size() ) {
	msg(MSG::WARNING) << "Shape a (" << vShape.size() << ") and b (" << vShapeDer.size() << ") are not the same size for channel" 
			  <<  m_onlineID->channel_name(id) << endreq;
	msg(MSG::WARNING) << "Will be not exported !!!" << endreq;
      } else { // save in new container
	std::vector<std::vector<float> > shapeDer;
	std::vector<std::vector<float> > shapeAmpl;
	shapeAmpl.push_back(vShape.asVector());
	shapeDer.push_back(vShapeDer.asVector());	
	larShapeComplete->set(id,(int)gain,shapeAmpl,shapeDer,timeOffset,25.); //There is no sensible time-bin width for single-phase OFCs 25 seems to make the most sense...
      }
    }
  }

  ATH_MSG_DEBUG(" Selected shapes for "  <<  count << " cells");

  sc = detStore()->record(larShapeComplete,  m_keyShape_new);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to record LArShapeComplete object with key " << m_keyShape_new << endreq;
    return sc;
  }
  sc = detStore()->symLink(larShapeComplete, (ILArShape*)larShapeComplete);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to sym-link LArShapeComplete object" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
  
}

