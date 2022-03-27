/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//
// algorithm to merge LArHit from different subevents from pileup service to
//   a single event, assuming all subevents are at the same time and neglecting the
//   individual hit times
//  This is aimed to run at first stage for very large luminosity pileup

#include "LArHitFilter.h"
#include "LArSimEvent/LArHit.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/IdentifierHash.h"
#include "LArDigitization/LArHitEMap.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

LArHitFilter::LArHitFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
  , m_larem_id(nullptr)
  , m_larhec_id(nullptr)
  , m_larfcal_id(nullptr)
  , m_SubDetectors("LAr_All")
{
  declareProperty("SubDetectors"   ,m_SubDetectors,"subdetector selection");

  m_ecut[0][0]=0.05;
  m_ecut[0][1]=0.2;
  m_ecut[0][2]=0.1;
  m_ecut[0][3]=0.1;
  m_ecut[1][0]=0.05;
  m_ecut[1][1]=0.1;
  m_ecut[1][2]=0.05;
  m_ecut[1][3]=0.05;
  m_ecut[2][0]=0.05;
  m_ecut[2][1]=0.05;
  m_ecut[2][2]=0.05;
  m_ecut[2][3]=0.05;
  m_ecut[3][0]=0.05;
  m_ecut[3][1]=0.05;
  m_ecut[3][2]=0.05;
  m_ecut[3][3]=0.05;

  return;
}


LArHitFilter::~LArHitFilter()
{
  return;
}


StatusCode LArHitFilter::initialize()
{
  //
  // ......... make the Sub-detector flag vector
  //

  for (int i=0; i < LArHitEMap::NUMDET ; i++)
    {
      m_SubDetFlag.push_back(false);
    }

  //
  // ......... make the digit container name list
  //

  if ( m_SubDetectors == "LAr_All" )
    {
      m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
      m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::EMENDCAP_INDEX);
      m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
      m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
      m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
    }
  else if ( m_SubDetectors == "LAr_Em" )
    {
      m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
      m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::EMENDCAP_INDEX);
      m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
    }
  else if ( m_SubDetectors == "LAr_EmBarrel" )
    {
      m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
      m_SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
    }
  else if ( m_SubDetectors == "LAr_EmEndCap" )
    {
      m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
      m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
    }
  else if ( m_SubDetectors == "LAr_HEC" )
    {
      m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
      m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;
    }
  else if ( m_SubDetectors == "LAr_Fcal" )
    {
      m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
      m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
    }
  else if (m_SubDetectors == "LAr_EndCap")
    {
      m_CaloType.push_back(LArHitEMap::EMBARREL_INDEX);
      m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::HADENDCAP_INDEX);
      m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;

      m_CaloType.push_back(LArHitEMap::FORWARD_INDEX);
      m_SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
    }
  else
    {
      ATH_MSG_ERROR("Invalid SubDetector properties");
      return(StatusCode::FAILURE);
    }


  //retrieve ID helpers
  const CaloIdManager* caloIdMgr{};
  ATH_CHECK(detStore()->retrieve(caloIdMgr));
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id  = caloIdMgr->getFCAL_ID();

  ATH_CHECK(m_inputEMBHitsKey.initialize());
  ATH_CHECK(m_outputEMBHitsKey.initialize());
  ATH_CHECK(m_inputEMECHitsKey.initialize());
  ATH_CHECK(m_outputEMECHitsKey.initialize());
  ATH_CHECK(m_inputHECHitsKey.initialize());
  ATH_CHECK(m_outputHECHitsKey.initialize());
  ATH_CHECK(m_inputFCALHitsKey.initialize());
  ATH_CHECK(m_outputFCALHitsKey.initialize());

  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::execute(const EventContext& ctx) const
{
  unsigned int nhit_tot=0;
  unsigned int nhit_out=0;

  if(m_SubDetFlag[LArHitEMap::EMBARREL_INDEX]) {
    /// Filter EMB LArHitContainer
    SG::ReadHandle<LArHitContainer> inputEMBHits = SG::makeHandle(m_inputEMBHitsKey, ctx);
    if(!inputEMBHits.isValid()) {
      ATH_MSG_ERROR( "Could not find EMB LArHitsContainer");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found EMB LArHitsContainer");
    SG::WriteHandle<LArHitContainer> outputEMBHits = SG::makeHandle(m_outputEMBHitsKey, ctx);
    ATH_CHECK(outputEMBHits.record(std::make_unique<LArHitContainer>()));
    ATH_CHECK(this->filterContainer(inputEMBHits,outputEMBHits,0));
    nhit_tot += inputEMBHits->size();
    nhit_out += outputEMBHits->size();
  }
  if(m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX]) {
    /// Filter EMEC LArHitContainer
    SG::ReadHandle<LArHitContainer> inputEMECHits = SG::makeHandle(m_inputEMECHitsKey, ctx);
    if(!inputEMECHits.isValid()) {
      ATH_MSG_ERROR( "Could not find EMEC LArHitsContainer");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found EMEC LArHitsContainer");
    SG::WriteHandle<LArHitContainer> outputEMECHits = SG::makeHandle(m_outputEMECHitsKey, ctx);
    ATH_CHECK(outputEMECHits.record(std::make_unique<LArHitContainer>()));
    ATH_CHECK(this->filterContainer(inputEMECHits,outputEMECHits,1));
    nhit_tot += inputEMECHits->size();
    nhit_out += outputEMECHits->size();
  }
  if(m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX]) {
    /// Filter HEC LArHitContainer
    SG::ReadHandle<LArHitContainer> inputHECHits = SG::makeHandle(m_inputHECHitsKey, ctx);
    if(!inputHECHits.isValid()) {
      ATH_MSG_ERROR( "Could not find HEC LArHitsContainer");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found HEC LArHitsContainer");
    SG::WriteHandle<LArHitContainer> outputHECHits = SG::makeHandle(m_outputHECHitsKey, ctx);
    ATH_CHECK(outputHECHits.record(std::make_unique<LArHitContainer>()));
    ATH_CHECK(this->filterContainer(inputHECHits,outputHECHits,1));
    nhit_tot += inputHECHits->size();
    nhit_out += outputHECHits->size();
  }
  if(m_SubDetFlag[LArHitEMap::FORWARD_INDEX]) {
    /// Filter FCAL LArHitContainer
    SG::ReadHandle<LArHitContainer> inputFCALHits = SG::makeHandle(m_inputFCALHitsKey, ctx);
    if(!inputFCALHits.isValid()) {
      ATH_MSG_ERROR( "Could not find FCAL LArHitsContainer");
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG( "Found FCAL LArHitsContainer");
    SG::WriteHandle<LArHitContainer> outputFCALHits = SG::makeHandle(m_outputFCALHitsKey, ctx);
    ATH_CHECK(outputFCALHits.record(std::make_unique<LArHitContainer>()));
    ATH_CHECK(this->filterContainer(inputFCALHits,outputFCALHits,1));
    nhit_tot += inputFCALHits->size();
    nhit_out += outputFCALHits->size();
  }
  ATH_MSG_INFO(" total number of hits found "  << nhit_tot << " " << nhit_out);

  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::filterContainer(SG::ReadHandle<LArHitContainer>& inputContainer, SG::WriteHandle<LArHitContainer>& outputContainer, int ical) const
{
  const unsigned int guess_size = inputContainer->size()/2 + 1;
  std::vector<LArHit*> tmphit;
  tmphit.reserve(guess_size);
  unsigned int nhit_out(0);
  // Loop over cells in this LArHitContainer
  for (const LArHit* f_cell : *inputContainer) {
    double energy = f_cell->energy();
    int ilayer=0;
    Identifier cellId = f_cell->cellID();
    if (ical==0 || ical==1) ilayer = m_larem_id->sampling(cellId);
    if (ical==2 ) ilayer = m_larhec_id->sampling(cellId);
    if (ical==3 ) ilayer = m_larfcal_id->module(cellId);
    if (energy>m_ecut[ical][ilayer]) {
      nhit_out++;
      double time = f_cell->time();
      LArHit* newhit = new LArHit(cellId,energy,time);
      newhit->finalize();
      tmphit.push_back(newhit);
    }
  }              //  loop over  hits

  outputContainer->Clear();
  outputContainer->reserve(nhit_out);
  for (LArHit* hit : tmphit) {
    outputContainer->push_back(hit);
  }
  ATH_MSG_DEBUG("  -- > size after filtering " << outputContainer->size());
  return StatusCode::SUCCESS;
}

StatusCode LArHitFilter::finalize()
{
  ATH_MSG_DEBUG(" LArHitFilter finalize completed successfully");
  return StatusCode::SUCCESS;
}
