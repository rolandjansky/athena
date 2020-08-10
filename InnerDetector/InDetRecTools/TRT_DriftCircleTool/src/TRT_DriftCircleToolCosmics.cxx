/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleToolCosmics
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for TRT RDI collection production
///////////////////////////////////////////////////////////////////
// Version 1.0 18/02/2003 I.Gavrilenko
///////////////////////////////////////////////////////////////////

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TRT_DriftCircleTool/TRT_DriftCircleToolCosmics.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetRawData/TRT_RDORawData.h"

#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/ReadHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "StoreGate/ReadCondHandle.h"
///////////////////////////////////////////////////////////////////
// Constructior
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleToolCosmics::TRT_DriftCircleToolCosmics(const std::string& t, 
						const std::string& n,
						const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_driftFunctionTool("TRT_DriftFunctionTool"),
  m_ConditionsSummary("TRT_StrawStatusSummaryTool",this),
  m_useConditionsStatus(false),
  m_trtid(0),
  m_global_offset(0),
  m_useToTCorrection(false),
  m_useHTCorrection(false),
  m_reject_if_first_bit(false),
  m_reject_if_first_bit_argon(true),
  m_min_trailing_edge(11.0*CLHEP::ns),
  m_min_trailing_edge_argon(11.0*CLHEP::ns),
  m_max_drift_time(60*CLHEP::ns),
  m_max_drift_time_argon(60*CLHEP::ns),
  m_out_of_time_supression(false),
  m_out_of_time_supression_argon(false),
  m_validity_gate_suppression(false),
  m_validity_gate_suppression_argon(false),
  m_low_gate(18.0*CLHEP::ns),
  m_low_gate_argon(18.0*CLHEP::ns),
  m_high_gate(38.0*CLHEP::ns),
  m_high_gate_argon(38.0*CLHEP::ns)

{
  declareInterface<ITRT_DriftCircleTool>(this);
  declareProperty("TRTDriftFunctionTool", m_driftFunctionTool);
  declareProperty("ConditionsSummaryTool",m_ConditionsSummary);
  declareProperty("UseConditionsStatus",m_useConditionsStatus);

  declareProperty("UseConditionsHTStatus",m_useConditionsHTStatus);
  declareProperty("useDriftTimeToTCorrection",m_useToTCorrection);
  declareProperty("useDriftTimeHTCorrection",m_useHTCorrection);
  declareProperty("RejectIfFirstBit",m_reject_if_first_bit);
  declareProperty("RejectIfFirstBitArgon",m_reject_if_first_bit_argon);
  declareProperty("MinTrailingEdge",m_min_trailing_edge);
  declareProperty("MinTrailingEdgeArgon",m_min_trailing_edge_argon);
  declareProperty("MaxDriftTime",m_max_drift_time);
  declareProperty("MaxDriftTimeArgon",m_max_drift_time_argon);
  declareProperty("SimpleOutOfTimePileupSupression",m_out_of_time_supression);
  declareProperty("SimpleOutOfTimePileupSupressionArgon",m_out_of_time_supression_argon);
  declareProperty("ValidityGateSuppression",m_validity_gate_suppression);
  declareProperty("ValidityGateSuppressionArgon",m_validity_gate_suppression_argon);
  declareProperty("LowGate",m_low_gate);
  declareProperty("LowGateArgon",m_low_gate_argon);
  declareProperty("HighGate",m_high_gate);
  declareProperty("HighGateArgon",m_high_gate_argon);
  declareProperty("GlobalPhaseOffset",m_global_offset);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleToolCosmics::~TRT_DriftCircleToolCosmics(){}

///////////////////////////////////////////////////////////////////
// Test validity gate
///////////////////////////////////////////////////////////////////
bool InDet::TRT_DriftCircleToolCosmics::passValidityGate(unsigned int word, float lowGate, float highGate, float t0) const
{
  bool foundInterval = false;
  unsigned  mask = 0x02000000;
  int i = 0;
  while ( !foundInterval && (i < 24) ) {
    if (word & mask) {
      float thisTime = ((0.5+i)*3.125)-t0;
      if (thisTime >= lowGate && thisTime <= highGate) foundInterval = true;
    }
    mask >>= 1;
    if (i == 7 || i == 15) 
      mask >>= 1;
    i++;
  }
  if (foundInterval) return true;
  return false;
}


///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleToolCosmics::initialize()
{

  StatusCode sc = AthAlgTool::initialize(); 

  // Get DriftFunction tool
  //
  if ( m_driftFunctionTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(m_driftFunctionTool.propertyName() << ": Failed to retrieve tool " << m_driftFunctionTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO(m_driftFunctionTool.propertyName() << ": Retrieved tool " << m_driftFunctionTool.type());
  }

  // Get TRT ID helper
  sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }

  if(m_useConditionsStatus){
    if ( m_ConditionsSummary.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve "<< m_ConditionsSummary);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Retrieved service " << m_ConditionsSummary);
    }
  }

  // Initialize Read handle key
  ATH_CHECK(m_evtPhaseKey.initialize());

  // Initialize readCondHandle key                                                                                                                         
  ATH_CHECK(m_trtDetEleContKey.initialize());

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleToolCosmics::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); return sc;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircles collection production
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleCollection* InDet::TRT_DriftCircleToolCosmics::convert(int Mode,const InDetRawDataCollection<TRT_RDORawData>* rdo, const EventContext& ctx, const bool /* _getTRTBadChannel */) const
{

  //Initialise a new TRT_DriftCircleCollection
  InDet::TRT_DriftCircleCollection* rio = 0;

  if (!rdo) {
    ATH_MSG_ERROR("empty collection at input");
    return rio;
  }

  SG::ReadHandle<ComTime> theComTime(m_evtPhaseKey, ctx);
  
  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey, ctx);
  const InDetDD::TRT_DetElementCollection* elements(trtDetEleHandle->getElements());
  if (not trtDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_FATAL(m_trtDetEleContKey.fullKey() << " is not available.");
    return rio;
  }
  	 
  float timecor=0.;
  if (theComTime.isValid()) {
    timecor = theComTime->getTime() + m_global_offset;
    ATH_MSG_VERBOSE("Retrieved event phase with name "
		    << theComTime.key() <<" found! Time="<<timecor);
  }else{
    ATH_MSG_VERBOSE("Event phase not found with name "<<theComTime.key()<<"!!!");
    timecor=m_global_offset; // cannot correct drifttime
  }

  DataVector<TRT_RDORawData>::const_iterator r,rb=rdo->begin(),re=rdo->end(); 
  if(rb!=re) { 

   //Get the BaseElement and the rio of the collection
    IdentifierHash IHc                 = rdo      ->identifyHash();
    const InDetDD::TRT_BaseElement* pE = elements->getDetectorElement(IHc);
    rio                                = new InDet::TRT_DriftCircleCollection(IHc);
    rio->setIdentifier(rdo->identify());
    rio->reserve( std::distance(rb, re) );


    // Loop through all RDOs in the collection
    //

    for(r=rb; r!=re; ++r) {

      Identifier   id  = (*r)->identify();
      int   LTbin      = (*r)->driftTimeBin(); 
      bool  isOK       =true;
      double t0         =0.;
      double rawTime    = m_driftFunctionTool->rawTime(LTbin);
      double radius     =0.;
      double driftTime  =0.;
      unsigned int word    = (*r)->getWord(); 

      //
      //Get straw status
      int strawstat=1;

      if(m_useConditionsStatus){
         if((m_ConditionsSummary->getStatus(id) != TRTCond::StrawStatus::Good)
            || (m_ConditionsSummary->getStatusPermanent(id))) {
            strawstat = 0;
         }
         if(!strawstat) continue;
      }

      //correct for phase
      rawTime-=timecor;

      // ToT and HT Corrections            

      bool isArgonStraw=true;
      if (m_useToTCorrection) rawTime -= m_driftFunctionTool->driftTimeToTCorrection((*r)->timeOverThreshold(), id, isArgonStraw);
      if (m_useHTCorrection && (*r)->highLevel())  rawTime += m_driftFunctionTool->driftTimeHTCorrection(id, isArgonStraw);           

      //make tube hit if first bin is high and no later LE appears
      if( LTbin==0 || LTbin==24 ) {
        isOK=false;
      } else {
        radius    = m_driftFunctionTool->driftRadius(rawTime,id,t0,isOK);
        driftTime = rawTime-t0;
      }

      if(!isOK) word &= 0xF7FFFFFF; 
      else word |= 0x08000000;


      //std::vector<Identifier>    dvi // we dont need this                                   ;
      ATH_MSG_VERBOSE( " id " << m_trtid->layer_or_wheel(id)
	  << " " << m_trtid->straw_layer(id)
	  << " " << m_trtid->straw(id)
          << " time bin  " << LTbin
          << " timecor " << timecor
	  << " corrected time " << rawTime );
         
      double error=0;
      if(Mode<2) {
	error = m_driftFunctionTool->errorOfDriftRadius(driftTime,id)         ;
      }

      ATH_MSG_VERBOSE("   hit OK " << isOK << " t0 " << t0
            << " drifttime " << driftTime
	    << " radius " << radius << " error " << error);

      if( !isOK || (error==0.&&Mode<2) ) //Drifttime out of range. Make wirehit
	{
	  ATH_MSG_VERBOSE(" Making wirehit.");
	  radius = 0.;
	  error = 4./sqrt(12.);
	}

      Amg::MatrixX* errmat = new Amg::MatrixX(1,1);                          ;
      (*errmat)(0,0) = error*error;

      Amg::Vector2D loc(radius,0.);

      // if(Mode<1) dvi.push_back(id);  we dont need this 

      InDet::TRT_DriftCircle* tdc = new InDet::TRT_DriftCircle(id,loc,errmat,pE,word);

      if (tdc) {
	     
           if(!m_useConditionsStatus){
                // setting the index (via -> size) has to be done just before the push_back! (for safety) 
                tdc->setHashAndIndex(rio->identifyHash(), rio->size());
                rio->push_back(tdc);

           }else{
             if(strawstat){
                tdc->setHashAndIndex(rio->identifyHash(), rio->size());
                rio->push_back(tdc);
	     } else {
	        ATH_MSG_VERBOSE(" Delete hit on bad channel ");
	        delete tdc;
	     }
           }

      } else{
	ATH_MSG_ERROR("Could not create InDet::TRT_DriftCircle object !");
      }
    }
  }  
  return rio;
}  


