/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// algorithm to merge LArHit from different subevents from pileup service to
//   a single event, assuming all subevents are at the same time and neglecting the
//   individual hit times
//  This is aimed to run at first stage for very large luminosity pileup

#include "LArDigitization/LArHitFilter.h"
#include "LArSimEvent/LArHit.h"
#include "CaloIdentifier/LArID.h"
#include "Identifier/IdentifierHash.h"
#include "LArDigitization/LArHitEMap.h"

LArHitFilter::LArHitFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_larem_id(nullptr)
  , m_larhec_id(nullptr)
  , m_larfcal_id(nullptr)
  , m_SubDetectors("LAr_All")
  , m_inputEMBHits("StoreGateSvc+LArHitEMBOLD")
  , m_outputEMBHits("StoreGateSvc+LArHitEMB")
  , m_inputEMECHits("StoreGateSvc+LArHitEMECOLD")
  , m_outputEMECHits("StoreGateSvc+LArHitEMEC")
  , m_inputHECHits("StoreGateSvc+LArHitHECOLD")
  , m_outputHECHits("StoreGateSvc+LArHitHEC")
  , m_inputFCALHits("StoreGateSvc+LArHitFCALOLD")
  , m_outputFCALHits("StoreGateSvc+LArHitFCAL")
{
  declareProperty("SubDetectors"   ,m_SubDetectors,"subdetector selection");
  declareProperty("EMBHitsInput"   , m_inputEMBHits);
  declareProperty("EMBHitsOutput"  , m_outputEMBHits);
  declareProperty("EMECHitsInput"  , m_inputEMECHits);
  declareProperty("EMECHitsOutput" , m_outputEMECHits);
  declareProperty("HECHitsInput"   , m_inputHECHits);
  declareProperty("HECHitsOutput"  , m_outputHECHits);
  declareProperty("FCALHitsInput"  , m_inputFCALHits);
  declareProperty("FCALHitsOutput" , m_outputFCALHits);

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
  const DataHandle<CaloIdManager> caloIdMgr;
  StatusCode sc = detStore()->retrieve(caloIdMgr);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Unable to retrieve CaloIdManager from DetectoreStore");
    return StatusCode::FAILURE;
  }
  m_larem_id   = caloIdMgr->getEM_ID();
  m_larhec_id  = caloIdMgr->getHEC_ID();
  m_larfcal_id  = caloIdMgr->getFCAL_ID();


  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::execute()
{
  unsigned int nhit_tot=0;
  unsigned int nhit_out=0;

  if(m_SubDetFlag[LArHitEMap::EMBARREL_INDEX])
    {
      /// Filter EMB LArHitContainer
      if(!m_inputEMBHits.isValid())
        {
          ATH_MSG_ERROR( "Could not find EMB LArHitsContainer");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG( "Found EMB LArHitsContainer");
      if (!m_outputEMBHits.isValid()) m_outputEMBHits = CxxUtils::make_unique<LArHitContainer>();
      ATH_CHECK(this->filterContainer(m_inputEMBHits,m_outputEMBHits,0));
      nhit_tot+= m_inputEMBHits->size();
      nhit_out+= m_outputEMBHits->size();
    }
  if(m_SubDetFlag[LArHitEMap::EMENDCAP_INDEX])
    {
      /// Filter EMEC LArHitContainer
      if(!m_inputEMECHits.isValid())
        {
          ATH_MSG_ERROR( "Could not find EMEC LArHitsContainer");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG( "Found EMEC LArHitsContainer");
      if (!m_outputEMECHits.isValid()) m_outputEMECHits = CxxUtils::make_unique<LArHitContainer>();
      ATH_CHECK(this->filterContainer(m_inputEMECHits,m_outputEMECHits,1));
      nhit_tot+= m_inputEMECHits->size();
      nhit_out+= m_outputEMECHits->size();
    }
  if(m_SubDetFlag[LArHitEMap::HADENDCAP_INDEX])
    {
      /// Filter HEC LArHitContainer
      if(!m_inputHECHits.isValid())
        {
          ATH_MSG_ERROR( "Could not find HEC LArHitsContainer");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG( "Found HEC LArHitsContainer");
      if (!m_outputHECHits.isValid()) m_outputHECHits = CxxUtils::make_unique<LArHitContainer>();
      ATH_CHECK(this->filterContainer(m_inputHECHits,m_outputHECHits,1));
      nhit_tot+= m_inputHECHits->size();
      nhit_out+= m_outputHECHits->size();
    }
  if(m_SubDetFlag[LArHitEMap::FORWARD_INDEX])
    {
      /// Filter FCAL LArHitContainer
      if(!m_inputFCALHits.isValid())
        {
          ATH_MSG_ERROR( "Could not find FCAL LArHitsContainer");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG( "Found FCAL LArHitsContainer");
      if (!m_outputFCALHits.isValid()) m_outputFCALHits = CxxUtils::make_unique<LArHitContainer>();
      ATH_CHECK(this->filterContainer(m_inputFCALHits,m_outputFCALHits,1));
      nhit_tot+= m_inputFCALHits->size();
      nhit_out+= m_outputFCALHits->size();
    }
  ATH_MSG_INFO(" total number of hits found "  << nhit_tot << " " << nhit_out);

  return StatusCode::SUCCESS;

}

StatusCode LArHitFilter::filterContainer(SG::ReadHandle<LArHitContainer>& inputContainer, SG::WriteHandle<LArHitContainer>& outputContainer, int ical)
{
  // Loop over cells in this LArHitContainer
  LArHitContainer::const_iterator f_cell=inputContainer->begin();
  LArHitContainer::const_iterator l_cell=inputContainer->end();

  const unsigned int guess_size = inputContainer->size()/2 + 1;
  std::vector<LArHit*> tmphit;
  tmphit.reserve(guess_size);
  unsigned int nhit_out(0);
  while (f_cell != l_cell) {
    double energy = (*f_cell)->energy();
    //nhit_tot++;
    int ilayer=0;
    Identifier cellId = (*f_cell)->cellID();
    if (ical==0 || ical==1) ilayer = m_larem_id->sampling(cellId);
    if (ical==2 ) ilayer = m_larhec_id->sampling(cellId);
    if (ical==3 ) ilayer = m_larfcal_id->module(cellId);
    if (energy>m_ecut[ical][ilayer]) {
      nhit_out++;
      double time = (*f_cell)->time();
      LArHit* newhit = new LArHit(cellId,energy,time);
      newhit->finalize();
      tmphit.push_back(newhit);
    }
    ++f_cell;
  }              //  loop over  hits

  outputContainer->Clear();
  outputContainer->reserve(nhit_out);

  f_cell=tmphit.begin();
  l_cell=tmphit.end();
  while (f_cell != l_cell) {
    LArHit* hit = (*f_cell);
    outputContainer->push_back(hit);
    ++f_cell;
  }
  ATH_MSG_DEBUG("  -- > size after filtering " << outputContainer->size());
  return StatusCode::SUCCESS;
}

StatusCode LArHitFilter::finalize()
{
  ATH_MSG_DEBUG(" LArHitFilter finalize completed successfully");
  return StatusCode::SUCCESS;
}
