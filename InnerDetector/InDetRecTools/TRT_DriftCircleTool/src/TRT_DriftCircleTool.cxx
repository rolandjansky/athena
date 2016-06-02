/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class TRT_DriftCircleTool
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
#include "TRT_DriftCircleTool/TRT_DriftCircleTool.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"
#include "InDetRawData/TRT_RDORawData.h"

#include "TRT_DriftFunctionTool/ITRT_DriftFunctionTool.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetIdentifier/TRT_ID.h"
//#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
//#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h"
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "xAODEventInfo/EventInfo.h"
///////////////////////////////////////////////////////////////////
// Constructior
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleTool::TRT_DriftCircleTool(const std::string& t, 
						const std::string& n,
						const IInterface*  p ):
  AthAlgTool(t,n,p),
  m_driftFunctionTool("TRT_DriftFunctionTool"),
  m_ConditionsSummary("InDetTRTConditionsSummaryService",n),
  m_useConditionsStatus(false),
  m_useConditionsHTStatus(false),
  m_useToTCorrection(false),
  m_useHTCorrection(false),
  m_trt_mgr_location("TRT"),
  m_trt_mgr(0),
  m_trtid(0),
  m_coll_pll(0),
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
}

///////////////////////////////////////////////////////////////////
// Destructor  
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleTool::~TRT_DriftCircleTool(){}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize(); 

 

  // Get DriftFunction tool servise
  //
  if ( m_driftFunctionTool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << m_driftFunctionTool.propertyName() << ": Failed to retrieve tool " << m_driftFunctionTool.type() << endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << m_driftFunctionTool.propertyName() << ": Retrieved tool " << m_driftFunctionTool.type() << endreq;
  }

  if(m_useConditionsStatus || m_useConditionsHTStatus){
    if ( m_ConditionsSummary.retrieve().isFailure() ) {
      msg(MSG::FATAL) <<"Failed to retrieve "<< m_ConditionsSummary << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO) << "Retrieved tool " << m_ConditionsSummary << endreq;
    }
  }

  // Get  TRT Detector Manager
  //
  sc = AthAlgTool::detStore()->retrieve(m_trt_mgr, m_trt_mgr_location);
  if (sc.isFailure() || !m_trt_mgr)
  {
    msg(MSG::FATAL) << "Could not find TRT_DetectorManager: "
       	  << m_trt_mgr_location << " !" << endreq;
    return sc;
  }
  // Get TRT ID helper
  sc = detStore()->retrieve(m_trtid,"TRT_ID");
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "Could not retrieve TRT ID helper." );
    return sc;
  }

  return sc;
}

///////////////////////////////////////////////////////////////////
// Finalize
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DriftCircleTool::finalize()
{
   StatusCode sc = AthAlgTool::finalize(); return sc;
}

 
///////////////////////////////////////////////////////////////////
// Test validity gate
///////////////////////////////////////////////////////////////////
bool InDet::TRT_DriftCircleTool::passValidityGate(unsigned int word, float lowGate, float highGate, float t0) const {
    bool foundInterval = false;
    unsigned  mask = 0x02000000;
    int i = 0;
    while ( !foundInterval && (i < 24) ) {
       if (word & mask) {
           float thisTime = ((0.5+i)*3.125)-t0;
           if (thisTime >= lowGate && thisTime <= highGate) foundInterval = true;
       }
       mask >>= 1;
       if (i == 7 || i == 15) mask >>= 1;
       i++;
    }
    if (foundInterval) return true;
    return false;
}

///////////////////////////////////////////////////////////////////
// Trk::TRT_DriftCircles collection production
///////////////////////////////////////////////////////////////////

InDet::TRT_DriftCircleCollection* InDet::TRT_DriftCircleTool::convert(int Mode,const InDetRawDataCollection<TRT_RDORawData>* rdo, const bool m_getTRTBadChannel)
{

  //Initialise a new TRT_DriftCircleCollection
  InDet::TRT_DriftCircleCollection* rio = 0;

  if (!rdo) {
    ATH_MSG_ERROR("empty TRT raw data collection");
    return rio;
  }

  float mu = -10;
  const xAOD::EventInfo* m_eventInfo = 0;
      if ( StatusCode::SUCCESS ==  evtStore()->retrieve(m_eventInfo) ){
                mu = (float)           m_eventInfo->averageInteractionsPerCrossing();
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
          const TRT_TB04_RawData* tb_rdo = dynamic_cast<TRT_TB04_RawData*>(*r);
          if(tb_rdo) {
            Identifier   id  = tb_rdo->identify();
            // skip if not first layer
            if(m_trtid->layer_or_wheel(id)) continue;
            // store the trigger pll first time encountered
            m_coll_pll = tb_rdo->getTrigType(); 
          }
       }
    }

    if(msgLvl(MSG::VERBOSE)) msg() << " choose timepll for rdo collection: " << m_coll_pll << endreq;


    // Loop through all RDOs in the collection
    //
    for(r=rb; r!=re; ++r) {

      if (!*r) { 
        ATH_MSG_ERROR(" No RDO pointer! ");
	continue;
      }

      Identifier   id  = (*r)->identify();
      unsigned int m_word = (*r)->getWord(); 
      bool reject_from_neighboring_BC = false;
      bool isArgonStraw = false;

      // LE as in TRT_DriftCircle.h
      unsigned  mask = 0x02000000; 
      bool SawZero = false; 
      int i; 
      for(i=0;i<24;++i) 
      { if      (  (m_word & mask) && SawZero) break; 
         else if ( !(m_word & mask) ) SawZero = true; 
         mask>>=1; 
         if(i==7 || i==15) mask>>=1; 
      } 
      if(i==24) i=0; 
      int tdcvalue = i; 
      int LE  = tdcvalue;

      //TE as in TRT_DriftCircle.h
      int TE = 0;
      mask = 0x00000001;
      SawZero=false;
      for (i = 0; i < 24; ++i)
	{
	  if ( (m_word & mask) && SawZero )
	    break;
	  else if ( !(m_word & mask) )
	    SawZero = true;

	  mask <<= 1;
	  if (i == 7 || i == 15)
	    mask <<= 1;
	}

      if ( 24 == i ) {
        TE = i;
      } else { 
        TE = (23 - i);
      }

      //ToT as in TRT_DriftCircle.h
      double ToT =0.;
      if ( (24 != LE) && (24 != TE) && (0 != TE) && (23 != LE) )
        ToT = (TE - LE + 1) * 3.125;

      unsigned int timepll = 0;

      // Fix hardware bug in testbeam
      if(m_driftFunctionTool->isTestBeamData()) {
        const TRT_TB04_RawData* tb_rdo = dynamic_cast<TRT_TB04_RawData*>(*r);
        if(tb_rdo) timepll = tb_rdo->getTrigType();
        if(m_coll_pll) {
          LE = tdcvalue - timepll/2 + m_coll_pll/2;
        }
        // Increase precision of timing
        LE = 2*LE+(m_coll_pll%2);
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
      
      if (m_useConditionsHTStatus) {
         if (m_ConditionsSummary->getStatusHT(id) == TRTCond::StrawStatus::Argon ||
	     m_ConditionsSummary->getStatusHT(id) == TRTCond::StrawStatus::Dead ) {
            isArgonStraw = true;
         }
      }

      bool  isOK=true;
      double t0=0.;
      double rawTime   = m_driftFunctionTool->rawTime(LE);
      unsigned int word = (*r)->getWord(); 
            
      if (m_useToTCorrection && !isArgonStraw) {
        rawTime -= m_driftFunctionTool->driftTimeToTCorrection((*r)->timeOverThreshold(), id);     
      }  

      if (m_useHTCorrection && !isArgonStraw &&
          ((!m_mask_first_HT_bit &&  (word & 0x04000000)) ||
           (!m_mask_middle_HT_bit && (word & 0x00020000)) ||
           (!m_mask_last_HT_bit &&   (word & 0x00000100)))) {
         rawTime += m_driftFunctionTool->driftTimeHTCorrection(id);           
      }
      
      double radius    = 0.;
      double driftTime = 0;

      //make tube hit if first bin is high and no later LE appears
      if( LE==0 || LE==24 ) {
        isOK=false;
      } else {
        radius    = m_driftFunctionTool->driftRadius(rawTime,id,t0,isOK,word);
        driftTime = rawTime-t0;
      }


      if(!isOK) word &= 0xF7FFFFFF;
      else word |= 0x08000000;

      if (!isArgonStraw) {
         if (m_mask_first_HT_bit) word &= 0xFBFFFFFF;
         if (m_mask_middle_HT_bit) word &= 0xFFFDFFFF;
         if (m_mask_last_HT_bit) word &= 0xFFFFFEFF;
      }
      else {
         if (m_mask_first_HT_bit_argon) word &= 0xFBFFFFFF;
         if (m_mask_middle_HT_bit_argon) word &= 0xFFFDFFFF;
         if (m_mask_last_HT_bit_argon) word &= 0xFFFFFEFF;
      }


      double error=0;

      if(Mode<2) error = m_driftFunctionTool->errorOfDriftRadius(driftTime,id,mu,word);

      if( !isOK || (error==0. && Mode<2) ) //Drifttime out of range. Make wirehit
	{
	  ATH_MSG_VERBOSE(" Making wirehit.");
	  radius = 0.;
	  error = 4./sqrt(12.);
	}

     
     if(!m_getTRTBadChannel){
        // create a drift circle no matter what
        Amg::MatrixX* errmat = new Amg::MatrixX(1,1);
        (*errmat)(0,0) = error*error;
        Amg::Vector2D loc(radius,0.);
        InDet::TRT_DriftCircle* tdc = new InDet::TRT_DriftCircle(id,loc,errmat,pE,word);
        // setting the index (via -> size) has to be done just before the push_back! (for safety) 
        tdc->setHashAndIndex(rio->identifyHash(), rio->size());
        // and push it on the drift circle list
        rio->push_back(tdc);
     }else{
        // check validity range
        reject_from_neighboring_BC = false;
        
        float rawdrifttime =  (0.5+LE)*3.125;
        unsigned int theword = (*r)->getWord();

        if (!isArgonStraw) {
           if (m_out_of_time_supression) {
              // reject if first bit true 
              if ((theword & 0x02000000) && m_reject_if_first_bit) reject_from_neighboring_BC = true;
              // or reject if trailing edge (which is drift time + ToT) is less than min trailing edge
              if ((rawdrifttime + ToT) < m_min_trailing_edge) reject_from_neighboring_BC = true;
              // reject if leading edge is too large
              if (rawdrifttime > m_max_drift_time) reject_from_neighboring_BC = true;
           }
           
           if (m_validity_gate_suppression) {
	      if(!passValidityGate(theword, m_low_gate, m_high_gate, t0)) reject_from_neighboring_BC = true;
           }
        } // is not argon straw
        else { // is argon straw. I have separate loops in case we want to do anything different for argon straws
           if (m_out_of_time_supression_argon) {
              // reject if first bit true 
              if ((theword & 0x02000000) && m_reject_if_first_bit_argon) reject_from_neighboring_BC = true;
              // or reject if trailing edge (which is drift time + ToT) is less than min trailing edge
              if ((rawdrifttime + ToT) < m_min_trailing_edge_argon) reject_from_neighboring_BC = true;
              // reject if leading edge is too large
              if (rawdrifttime > m_max_drift_time_argon) reject_from_neighboring_BC = true;
           }
           
           if (m_validity_gate_suppression_argon) {
	      if(!passValidityGate(theword, m_low_gate_argon, m_high_gate_argon, t0)) reject_from_neighboring_BC = true;
           }
        }

        ATH_MSG_VERBOSE(" Reject from neighboring BC = " << reject_from_neighboring_BC);


        if(m_strawstat && (!reject_from_neighboring_BC)){
	// Only create a drift circle if the channel is not bad and the validity range is OK
          Amg::MatrixX* errmat = new Amg::MatrixX(1,1);
          (*errmat)(0,0) = error*error;
          Amg::Vector2D loc(radius,0.);
          InDet::TRT_DriftCircle* tdc = new InDet::TRT_DriftCircle(id,loc,errmat,pE,word);

          if (tdc) {
             if(msgLvl(MSG::VERBOSE)) msg() << " new hit id "
		  << m_trtid->barrel_ec(id) << " " 
                  << m_trtid->layer_or_wheel(id) << " "
                  << m_trtid->phi_module(id) << " "  
                  << m_trtid->straw_layer(id) << " " 
                  << m_trtid->straw(id) 
                  << " data word " << MSG::hex<<tdc->getWord() <<MSG::dec
                  << " data word raw " << MSG::hex<<(*r)->getWord() <<MSG::dec 
                  << " radius " << radius
		  << " err " << error << endreq;

	     if(msgLvl(MSG::VERBOSE)) msg() << " driftTime "
                  << tdc->rawDriftTime() << " t0 " << t0
                  << " raw time " << (0.5+tdcvalue)*3.125
                  << " ToT " << tdc->timeOverThreshold()  
                  << " OK? " << isOK << " Noise? " 
                                    << tdc->isNoise() << " isArgon? " << isArgonStraw << endreq;

             tdc->setHashAndIndex(rio->identifyHash(), rio->size());
             rio->push_back(tdc);
	  } else {
             ATH_MSG_ERROR("Could not create InDet::TRT_DriftCircle object !");
          }
	} else {

	     ATH_MSG_VERBOSE(" Reject hit out of validity range or in a bad channel");
        }
      }
    }
  }  
  return rio;
}  


