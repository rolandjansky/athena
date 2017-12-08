/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/ToolFactory.h"
#include "TRT_DriftCircleTool/TRT_DriftCircleToolCosmics.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetRawData/TRT_RDORawData.h"

#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "CommissionEvent/ComTime.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "StoreGate/ReadHandle.h"
///////////////////////////////////////////////////////////////////
// Constructior
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleToolCosmics::TRT_DriftCircleToolCosmics(const std::string& t, 
						const std::string& n,
						const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_driftFunctionTool("TRT_DriftFunctionTool"),
  m_ConditionsSummary("InDetTRTConditionsSummaryService",n),
  m_useConditionsStatus(false),
  m_trt_mgr_location("TRT"),
  m_trt_mgr(0),
  m_trtid(0),
  m_coll_pll(0),
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
  m_high_gate_argon(38.0*CLHEP::ns),
  m_mask_first_HT_bit(false),
  m_mask_first_HT_bit_argon(false),
  m_mask_middle_HT_bit(false),
  m_mask_middle_HT_bit_argon(false),
  m_mask_last_HT_bit(false),
  m_mask_last_HT_bit_argon(false)

{
  declareInterface<ITRT_DriftCircleTool>(this);
  declareProperty("TrtDescrManageLocation",m_trt_mgr_location);
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
  declareProperty("MaskFirstHTBit",m_mask_first_HT_bit);
  declareProperty("MaskFirstHTBitArgon",m_mask_first_HT_bit_argon);
  declareProperty("MaskMiddleHTBit",m_mask_middle_HT_bit);
  declareProperty("MaskMiddleHTBitArgon",m_mask_middle_HT_bit_argon);
  declareProperty("MaskLastHTBit",m_mask_last_HT_bit);
  declareProperty("MaskLastHTBitArgon",m_mask_last_HT_bit_argon);
  declareProperty("GlobalPhaseOffset",m_global_offset);
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleToolCosmics::~TRT_DriftCircleToolCosmics(){}

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


  // Get  TRT Detector Manager
  //
  sc = AthAlgTool::detStore()->retrieve(m_trt_mgr, m_trt_mgr_location);
  if (sc.isFailure() || !m_trt_mgr)
  {
    ATH_MSG_FATAL("Could not find TRT_DetectorManager: "
		  << m_trt_mgr_location << " !");
    return sc;
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
  ATH_CHECK(m_comTimeName.initialize());
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
// Trk::TRT_DriftCircles collection production
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleCollection* InDet::TRT_DriftCircleToolCosmics::convert(int Mode,const InDetRawDataCollection<TRT_RDORawData>* rdo, const bool /* m_getTRTBadChannel */)
{

  //Initialise a new TRT_DriftCircleCollection
  InDet::TRT_DriftCircleCollection* rio = 0;

  if (!rdo) {
    ATH_MSG_ERROR("empty collection at input");
    return rio;
  }

  SG::ReadHandle<ComTime> theComTime(m_comTimeName);
  

  	 
  double timecor=0.;
  if (theComTime.isValid()) {
    timecor = theComTime->getTime() + m_global_offset;
    ATH_MSG_VERBOSE("Retrieved ComTime object with name "
		    << m_comTimeName<<" found! Time="<<timecor);
  }else{
    ATH_MSG_VERBOSE("ComTime object not found with name "<<m_comTimeName<<"!!!");
    timecor=m_global_offset; // cannot correct drifttime
    //return rio;
  }

  DataVector<TRT_RDORawData>::const_iterator r,rb=rdo->begin(),re=rdo->end(); 
  if(rb!=re) { 

   //Get the BaseElement and the rio of the collection
    IdentifierHash IHc                 = rdo      ->identifyHash();
    const InDetDD::TRT_BaseElement* pE = m_trt_mgr->getElement(IHc);
    rio                                = new InDet::TRT_DriftCircleCollection(IHc);
    rio->setIdentifier(rdo->identify());
    rio->reserve( std::distance(rb, re) );

    //In case of real CTB data: 
    if(m_driftFunctionTool->isTestBeamData()) {
      //   perform initial loop to find the trigger pll in first layer
       for(r=rb; r!=re; ++r) {
          // skip if rdo is not testbeam or cosmic flavor
          const TRT_TB04_RawData* tb_rdo = dynamic_cast<const TRT_TB04_RawData*>(*r);
          if(tb_rdo) {
            Identifier   id  = tb_rdo->identify();
            // skip if not first layer
            if(m_trtid->layer_or_wheel(id)) continue;
            // store the trigger pll first time encountered
            m_coll_pll = tb_rdo->getTrigType(); 
          }
       }
    }

    ATH_MSG_VERBOSE( " choose timepll for rdo collection: " << m_coll_pll);

    // Loop through all RDOs in the collection
    //
    for(r=rb; r!=re; ++r) {

      if (!*r) { 
        ATH_MSG_ERROR(" No RDO pointer! ");
	continue;
      }

      Identifier   id  = (*r)->identify();
      int          tdcvalue  = (*r)->driftTimeBin();
      int          newtdcvalue  = tdcvalue;
      unsigned int timepll = 0;

      // Fix hardware bug in testbeam
      if(m_driftFunctionTool->isTestBeamData()) {
        const TRT_TB04_RawData* tb_rdo = dynamic_cast<const TRT_TB04_RawData*>(*r);
        if(tb_rdo) timepll = tb_rdo->getTrigType();
        if(m_coll_pll) {
          newtdcvalue = tdcvalue - timepll/2 + m_coll_pll/2;
        }
        // Increase precision of timing
        newtdcvalue=2*newtdcvalue+(m_coll_pll%2);
      }

      //
      //Get straw status
      int m_strawstat=1;

      if(m_useConditionsStatus){
         if((m_ConditionsSummary->getStatus(id) != TRTCond::StrawStatus::Good)
            || (m_ConditionsSummary->getStatusPermanent(id))) {
            m_strawstat = 0;
         }
      }


      
      bool  isOK=true;
      double t0=0.;
      double driftTime=0.;
      double radius=0.;
      double rawTime   = m_driftFunctionTool->rawTime(newtdcvalue);


      //correct for phase
      rawTime-=timecor;
      //make tube hit if first bin is high and no later LE appears
      if( newtdcvalue==0 || newtdcvalue==24 ) {
        isOK=false;
      } else {
        radius    = m_driftFunctionTool->driftRadius(rawTime,id,t0,isOK);
        driftTime = rawTime-t0;
      }

      unsigned int word = (*r)->getWord(); 
      if(!isOK) word &= 0xF7FFFFFF; 
      else word |= 0x08000000;

      //std::vector<Identifier>    dvi // we dont need this                                   ;
      ATH_MSG_VERBOSE( " id " << m_trtid->layer_or_wheel(id)
	  << " " << m_trtid->straw_layer(id)
	  << " " << m_trtid->straw(id)
          << " new time bin  " << newtdcvalue << " old bin " << (*r)->driftTimeBin()
          << " timecor " << timecor
	  << " old timepll " << timepll  << " new time " << rawTime );
         

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
             if(m_strawstat){
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


