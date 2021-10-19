/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTStrawCondAlg.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

TRTStrawCondAlg::TRTStrawCondAlg(const std::string& name
				 , ISvcLocator* pSvcLocator )
  : ::AthAlgorithm(name,pSvcLocator),
    m_condSvc("CondSvc",name),
    m_strawStatus("TRT_StrawStatusSummaryTool",this)
{ declareProperty("TRTStrawStatusSummaryTool",m_strawStatus);
}
TRTStrawCondAlg::~TRTStrawCondAlg(){}

StatusCode TRTStrawCondAlg::initialize()
{

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Straw status
  ATH_CHECK ( m_strawStatus.retrieve() );

  // Read key
  ATH_CHECK( m_strawReadKey.initialize() );


  // Register write handle
  ATH_CHECK( m_strawWriteKey.initialize() );

  if (m_condSvc->regHandle(this, m_strawWriteKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_strawWriteKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  // Initialize readCondHandle key                                                                                                                           
  ATH_CHECK(m_trtDetEleContKey.initialize());

  // TRT ID helper
  ATH_CHECK(detStore()->retrieve(m_trtId,"TRT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode TRTStrawCondAlg::execute() 
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________

  SG::WriteCondHandle<TRTCond::AliveStraws> writeHandle{m_strawWriteKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");

    return StatusCode::SUCCESS; 
  }



  // ____________ Construct new Write Cond Object  ____________
  std::unique_ptr<TRTCond::AliveStraws> writeCdo{std::make_unique<TRTCond::AliveStraws>()};
  
  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey);
  const InDetDD::TRT_DetElementCollection* elements(trtDetEleHandle->getElements());
  if (not trtDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_FATAL(m_trtDetEleContKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }

  // ____________ Compute number of alive straws for Write Cond object  ____________

  for (std::vector<Identifier>::const_iterator it = m_trtId->straw_layer_begin(); it != m_trtId->straw_layer_end(); ++it ) {

   // Make sure it is a straw_layer id                                                                                                                   
   Identifier strawLayerId = m_trtId->layer_id(*it);
   //Get hash Id                                                                                                                                         
   IdentifierHash hashId = m_trtId->straw_layer_hash(strawLayerId);

   unsigned int nstraws = 0;
   if (trtDetEleHandle.isValid()){
     const InDetDD::TRT_BaseElement *el = elements->getDetectorElement(hashId);
     if( !el ) continue;
     nstraws = el->nStraws();
   }
   else{
     nstraws = m_trtId->straw_max( *it) + 1; // There is a difference of 1 between both methods....
   }
   for (unsigned int i=0; i<nstraws  ;i++) {
      Identifier id = m_trtId->straw_id( *it, i);
      int det = m_trtId->barrel_ec(         id)     ;
      int lay = m_trtId->layer_or_wheel(    id)     ;
      int phi = m_trtId->phi_module(        id)     ;
      bool status               = m_strawStatus->get_status( id );

      if ( status ) {
	ATH_MSG_VERBOSE(" The sector " << det << " " << lay << " " << phi << " has status " << status);
	continue;
      }

      int i_total = findArrayTotalIndex(det, lay);
      int i_wheel = findArrayLocalWheelIndex(det, lay);
       
      writeCdo->update(i_total,i_wheel,phi);

     }
  }

  //__________ Assign range of writeCdo to that of the ReadHandle___________ 
  EventIDRange rangeW;

    SG::ReadCondHandle<StrawStatusContainer> strawReadHandle{m_strawReadKey};
    const StrawStatusContainer* strawContainer{*strawReadHandle};
    if(strawContainer==nullptr) {
        ATH_MSG_ERROR("Null pointer to the straw status container");
        return StatusCode::FAILURE;
    }

    // Get range
    if(!strawReadHandle.range(rangeW)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << strawReadHandle.key());
        return StatusCode::FAILURE;
    }


  // Record  CDO
 if(writeHandle.record(rangeW,std::move(writeCdo)).isFailure()) {
    ATH_MSG_ERROR("Could not record AliveStraws " << writeHandle.key() 
		  << " with EventRange " << rangeW
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}

StatusCode TRTStrawCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

int TRTStrawCondAlg::findArrayTotalIndex(const int det, const int lay) const{
    int arrayindex = 0; // to be reset below
    // NOTE: Below, arrayindex starts at 1
    // because index 0 is filled with TOTAL value.
    if      (det == -1) arrayindex = 1; // barrel side C
    else if (det == -2) {               // endcap side C
      if (lay < 6)      arrayindex = 2; //   wheel A
      else              arrayindex = 3; //   wheel B
    }
    else if (det ==  1) arrayindex = 4; // barrel side A
    else if (det ==  2) {               // endcap side A
      if (lay < 6)      arrayindex = 5; //   wheel A
      else              arrayindex = 6; //   wheel B
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }

int TRTStrawCondAlg::findArrayLocalWheelIndex(const int det, const int lay) const{
    int arrayindex = 9; // to be reset below
    if      (det == -1) {                // barrel side C
      if      (lay == 0) arrayindex = 0; // layer 0
      else if (lay == 1) arrayindex = 1; // layer 1
      else if (lay == 2) arrayindex = 2; // layer 2
    }
    else if (det == -2) {                // endcap side C
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+3;
      }
    }
    else if (det ==  1) {                // barrel side A
      if      (lay == 0) arrayindex = 17; // layer 0
      else if (lay == 1) arrayindex = 18; // layer 1
      else if (lay == 2) arrayindex = 19; // layer 2
    }
    else if (det ==  2) {                // endcap side A
      for (int i=0; i<14; ++i){
        if (lay==i) arrayindex=i+20;
      }
    }
    else        ATH_MSG_WARNING(" detector value is: " << det << ", out of range -2, -1, 1, 2, so THIS IS NOT TRT!!!");
    return arrayindex;
  }

