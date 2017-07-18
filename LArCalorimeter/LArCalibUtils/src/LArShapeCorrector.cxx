/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArShapeCorrector.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "CaloIdentifier/CaloGain.h"

LArShapeCorrector::LArShapeCorrector(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_groupingType(LArConditionsContainerBase::Unknown)
{
  declareProperty("KeyShape",m_keyShape="LArShape_org");
  declareProperty("KeyShape_newcorr",m_keyShape_newcorr="LArShape");
  declareProperty("KeyShapeResidual",m_keyShapeResidual="LArResiduals");
  declareProperty("GroupingType",m_groupingName="ExtendedSubDetector");
  declareProperty("ShapePhase",m_phase=0);
}

LArShapeCorrector::~LArShapeCorrector()
{  }

StatusCode LArShapeCorrector::initialize()
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
     msg(MSG::ERROR)<< "Grouping type " << m_groupingName << " is not foreseen!" << endmsg ;
     msg(MSG::ERROR)<< "Only \"Unknown\", \"SingleGroup\", \"SubDetector\", \"FeedThrough\", \"ExtendedFeedThrough\" and \"ExtendedSubDetector\" are allowed" << endmsg ;
     return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArShapeCorrector::execute()
{
  return StatusCode::SUCCESS;
}



StatusCode LArShapeCorrector::stop() {

  ATH_MSG_DEBUG(" In stop() ");

  const LArShapeComplete* inputShape=NULL;
  StatusCode sc=detStore()->retrieve(inputShape,m_keyShape);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to get input Shapes with key " << m_keyShape << endmsg;
    return sc;
  }
    

  const LArShapeComplete* inputShapeResidual=NULL;
  sc=detStore()->retrieve(inputShapeResidual,m_keyShapeResidual);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to get input Shape residuals with key " << m_keyShapeResidual << endmsg;
    return sc;
  }

  LArShapeComplete* larShapeCompleteCorr=new LArShapeComplete();
  larShapeCompleteCorr->setGroupingType( static_cast<LArConditionsContainerBase::GroupingType>(m_groupingType));
  sc  = larShapeCompleteCorr->initialize(); 
  if ( sc.isFailure() ) {
     msg(MSG::ERROR)<< "Could not initialize LArShapeComplete data object - exit!" << endmsg ;
     return sc;
  }

  ATH_MSG_DEBUG("Working on Shape container '"<< m_keyShape << "' new container will be '" << m_keyShape_newcorr << "'");
  int count = 0;
  for(unsigned int gain = CaloGain::LARHIGHGAIN; gain < CaloGain::LARNGAIN; ++gain) {
    LArShapeComplete::ConstConditionsMapIterator it=inputShape->begin(gain);
    LArShapeComplete::ConstConditionsMapIterator it_e=inputShape->end(gain);
    for(; it!=it_e;++it){
      
      LArShapeComplete::ConstReference shape=(*it);
      const HWIdentifier id = it.channelId() ; 
      const int nPhases=shape.shapeSize();
      if (nPhases==0) {
	//ATH_MSG_DEBUG("Got empty Shape object for channel " << id.get_compact() << " (disconnected?)");
	continue;
      }
      count++;

      ILArShape::ShapeRef_t vShape = shape.shape(m_phase);
      ILArShape::ShapeRef_t vShapeDer = shape.shapeDer(m_phase);
      const float timeOffset=shape.timeOffset();
      //some sanity check on the Shapes
      bool doShapeCorr=true;
      if ( vShape.size() == 0 || vShapeDer.size() == 0 ) {
	msg(MSG::WARNING) << "Shape not found for gain "<< gain << " channel 0x"  << std::hex << id.get_compact() << std::dec << endmsg;
	continue;
      }
      if ( vShape.size() != vShapeDer.size() ) {
	msg(MSG::WARNING) << "Shape (" << vShape.size() << ") derivative (" << vShapeDer.size() << ") don't have the same size for channel 0x" 
			  << std::hex << id.get_compact() << std::dec << endmsg;
	continue;
      }

      std::vector<float> theShape;
      std::vector<float> theShapeDer;

      ILArShape::ShapeRef_t vShapeResidual=inputShapeResidual->Shape(id,gain,8); // only one phase, stored in phase #8
      ILArShape::ShapeRef_t vShapeResidualDer=inputShapeResidual->ShapeDer(id,gain,8);
      if ( vShapeResidual.size() == 0 || vShapeResidualDer.size() == 0 ) {
	msg(MSG::WARNING) << "Shape residual not found for gain " << gain 
			  << " channel 0x"  << std::hex << id.get_compact() << std::dec 
			  << ". Will not be applied!" << endmsg;
	doShapeCorr=false;
      }
      if ( vShapeResidual.size() != vShapeResidualDer.size() ) {
	msg(MSG::ERROR) << "Shape residual (" << vShapeResidual.size() << ") and its derivative (" << vShapeResidualDer.size() 
			<< ") don't have the same size for channel 0x" 
			<< std::hex << id.get_compact() << std::dec 
			<< ". Will be not applied!" << endmsg;
	doShapeCorr=false;
      }

      // check if shape and residuals sizes match
      if ( vShape.size() > vShapeResidual.size() ) { //FIXME, allow to apply 5 sample residual on 4 sample shape
	msg(MSG::WARNING) << "Shape residual size does not match the shape size for channel 0x" 
			  << std::hex << id.get_compact() << std::dec 
			  << ". Will be not corrected!" << endmsg;
	doShapeCorr=false;
      }
	 
      if (doShapeCorr) {
	for (unsigned k=0;k<vShape.size();k++) {
	  theShape.push_back( vShape[k] + vShapeResidual[k] );
	  theShapeDer.push_back( vShapeDer[k] + vShapeResidualDer[k] );
	  ATH_MSG_VERBOSE("Shape          channel= " << id.get_compact() << ": " << k << "\t" << vShape[k] << "\t" << vShapeDer[k]);
	  ATH_MSG_VERBOSE("Shape residual channel= " << id.get_compact() << ": " << k << "\t" << vShapeResidual[k] << "\t" << vShapeResidualDer[k]);
	}//end loop over samples
      }
      else { //doShapeCorr==fase
	for (unsigned k=0;k<vShape.size();k++) {
	  theShape.push_back( vShape[k] );
	  theShapeDer.push_back( vShapeDer[k] );
	}
      }//end else

      std::vector<std::vector<float> > shapeAmpl;
      shapeAmpl.push_back(theShape);
      std::vector<std::vector<float> > shapeDer;
      shapeDer.push_back(theShapeDer);
      larShapeCompleteCorr->set(id,(int)gain,shapeAmpl,shapeDer,timeOffset,25.);
      
    }//end loop over cells
  }//end loop over gains

  ATH_MSG_DEBUG(" Selected shapes for "  <<  count << " cells. NChannels=" << larShapeCompleteCorr->chan_size());

  sc = detStore()->record(larShapeCompleteCorr,  m_keyShape_newcorr);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to record LArShapeComplete object with key " << m_keyShape_newcorr << endmsg;
    return sc;
  }
  msg(MSG::INFO) << "Successfully registered LArShapeComplete object with key " << m_keyShape_newcorr << endmsg;

  sc = detStore()->symLink(larShapeCompleteCorr, (ILArShape*)larShapeCompleteCorr);
  if (sc.isFailure()) {
    msg(MSG::ERROR)<< "Failed to sym-link LArShapeComplete object" << endmsg;
    return sc;
  }

  //msg(MSG::INFO) << detStore()->dump() << endmsg;

  return StatusCode::SUCCESS;
  
}

