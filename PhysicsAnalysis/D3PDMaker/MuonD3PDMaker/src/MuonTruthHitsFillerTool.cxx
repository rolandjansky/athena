/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file MuonD3PDMaker/src/MuonTruthHitsFillerTool.cxx
 * @author scott snyder <snyder@bnl.gov>, from code by Niels van Eldik.
 * @date Aug, 2013
 * @brief Fill truth hit information for muons.
 */

#include "MuonTruthHitsFillerTool.h"

#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"

namespace D3PD {
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
MuonTruthHitsFillerTool::MuonTruthHitsFillerTool (const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
  : Base (type, name, parent)
{
  declareProperty("PRD_TruthNames",          m_PRD_TruthNames);
  m_PRD_TruthNames.push_back("CSC_TruthMap");
  m_PRD_TruthNames.push_back("RPC_TruthMap");
  m_PRD_TruthNames.push_back("TGC_TruthMap");
  m_PRD_TruthNames.push_back("MDT_TruthMap");
  m_PRD_TruthNames.push_back("MM_TruthMap");
  m_PRD_TruthNames.push_back("STGC_TruthMap");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode MuonTruthHitsFillerTool::initialize()
{
  CHECK( m_idHelperSvc.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Book variables for this block.
 */
StatusCode MuonTruthHitsFillerTool::book()
{
  typedef Muon::MuonStationIndex MSI;

  CHECK( addVariable ("nprecLayers",     m_nprecLayers )  );
  CHECK( addVariable ("nphiLayers",      m_nphiLayers )  );
  CHECK( addVariable ("ntrigEtaLayers",  m_ntrigEtaLayers )  );

  m_nprecHitsPerChamberLayer.resize(Muon::MuonStationIndex::ChIndexMax+2);
  for( unsigned int i=0; i<MSI::ChIndexMax; ++i) {
    MSI::ChIndex chIndex = (MSI::ChIndex)i;
    CHECK( addVariable ( std::string("nprecHits")+MSI::chName(chIndex),
                         m_nprecHitsPerChamberLayer[i] )  );
  }

  CHECK( addVariable ( std::string("nprecHitsMMS"),
                       m_nprecHitsPerChamberLayer[MSI::ChIndexMax] )  );
  CHECK( addVariable ( std::string("nprecHitsMML"),
                       m_nprecHitsPerChamberLayer[MSI::ChIndexMax+1] )  );

  m_nphiHitsPerChamberLayer.resize(MSI::PhiIndexMax);
  m_ntrigEtaHitsPerChamberLayer.resize(MSI::PhiIndexMax);
  for( unsigned int i=0; i<MSI::PhiIndexMax; ++i) {
    MSI::PhiIndex phiIndex = (MSI::PhiIndex)i;
    CHECK( addVariable ( std::string("nphiHits")+MSI::phiName(phiIndex),
                         m_nphiHitsPerChamberLayer[i] )  );
    CHECK( addVariable ( std::string("ntrigEtaHits")+MSI::phiName(phiIndex),
                         m_ntrigEtaHitsPerChamberLayer[i] )  );
  }

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 */
StatusCode MuonTruthHitsFillerTool::fill (const TrackRecord& p)
{
  CHECK( fillHitCounts (p.GetBarCode()) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 */
StatusCode MuonTruthHitsFillerTool::fill (const TruthParticle& p)
{
  CHECK( fillHitCounts (p.barcode()) );
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 */
StatusCode MuonTruthHitsFillerTool::fill (const xAOD::TruthParticle& p)
{
  CHECK( fillHitCounts (p.barcode()) );
  return StatusCode::SUCCESS;
}


StatusCode MuonTruthHitsFillerTool::fillHitCounts (int barcode)
{
  typedef Muon::MuonStationIndex MSI;

  bool found = false;
  for (const std::string& key : m_PRD_TruthNames) {
    const PRD_MultiTruthCollection* collection = 0;
    if (!evtStore()->retrieve(collection, key).isSuccess()) {
      ATH_MSG_WARNING(  "PRD_MultiTruthCollection " << key << " NOT found");
      continue;
    }

    for (const PRD_MultiTruthCollection::value_type& mc : *collection) {
      // check if gen particle same as input
      if( mc.second->barcode() != barcode ) continue;
      found = true;
      const Identifier& id = mc.first;
      ATH_MSG_VERBOSE("found matching hit " << m_idHelperSvc->toString(id) );
      bool measPhi   = m_idHelperSvc->measuresPhi(id);

      if( m_idHelperSvc->issTgc(id) ) {
        int index = m_idHelperSvc->phiIndex(id);
        if( measPhi ) ++*(m_nphiHitsPerChamberLayer[index]);
        else          ++*(m_ntrigEtaHitsPerChamberLayer[index]);
      }
      else if( m_idHelperSvc->isMM(id) ) {
        int index = m_idHelperSvc->isSmallChamber(id) ? MSI::ChIndexMax : MSI::ChIndexMax + 1;
        ++*(m_nprecHitsPerChamberLayer[index]);
      }
      else if( m_idHelperSvc->isTrigger(id) ) {
        MSI::PhiIndex index = m_idHelperSvc->phiIndex(id);
        if( measPhi ) ++*(m_nphiHitsPerChamberLayer[index]);
        else          ++*(m_ntrigEtaHitsPerChamberLayer[index]);
      }
      else {
        if( measPhi ) {
          Muon::MuonStationIndex::PhiIndex index = m_idHelperSvc->phiIndex(id);
          ++*(m_nphiHitsPerChamberLayer[index]);
        }else{
          Muon::MuonStationIndex::ChIndex chIndex = m_idHelperSvc->chamberIndex(id);
          ++*(m_nprecHitsPerChamberLayer[chIndex]);
        }	    
      }
    }
  }

  if( !found ) {
    ATH_MSG_VERBOSE("No hits associated to muon ");
  }
  else {
    // sum precision hits
    const std::vector<int*>& nprec = m_nprecHitsPerChamberLayer;
    const std::vector<int*>& nphi  = m_nphiHitsPerChamberLayer;
    const std::vector<int*>& ntrig = m_ntrigEtaHitsPerChamberLayer;

    if( *nprec[MSI::BIS] + *nprec[MSI::BIL] > 3 ) ++*m_nprecLayers;
    if( *nprec[MSI::BMS] + *nprec[MSI::BML] > 2 ) ++*m_nprecLayers;
    if( *nprec[MSI::BOS] + *nprec[MSI::BOL] > 2 ) ++*m_nprecLayers;
    if( *nprec[MSI::EIS] + *nprec[MSI::EIL] > 3 ) ++*m_nprecLayers;
    if( *nprec[MSI::EMS] + *nprec[MSI::EML] > 2 ) ++*m_nprecLayers;
    if( *nprec[MSI::EOS] + *nprec[MSI::EOL] > 2 ) ++*m_nprecLayers;
    if( *nprec[MSI::EES] + *nprec[MSI::EEL] > 3 ) ++*m_nprecLayers;
    if( *nprec[MSI::CSS] + *nprec[MSI::CSL] > 2 ) ++*m_nprecLayers;
    if( *nprec[MSI::BEE] > 3 ) ++*m_nprecLayers;
    if( *nprec[MSI::ChIndexMax] + *nprec[MSI::ChIndexMax+1] > 3 ) ++*m_nprecLayers;

    if( *nphi[MSI::BM1] > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::BM2] > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::BO1] > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::T1]  > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::T2]  > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::T3]  > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::T4]  > 0 )  ++*m_nphiLayers;
    if( *nphi[MSI::CSC] > 2 )  ++*m_nphiLayers;
    if( *nphi[MSI::STGC1] + *nphi[MSI::STGC2] > 3 )  ++*m_nphiLayers;

    if( *ntrig[MSI::BM1] > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::BM2] > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::BO1] > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::T1]  > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::T2]  > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::T3]  > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::T4]  > 0 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::CSC] > 2 )  ++*m_ntrigEtaLayers;
    if( *ntrig[MSI::STGC1] + 
        *ntrig[MSI::STGC2] > 3 )  ++*m_ntrigEtaLayers;
    
    ATH_MSG_DEBUG("Muon hits: prec " << *m_nprecLayers <<
                  " phi " << *m_nphiLayers
                  << " trig eta " << *m_ntrigEtaLayers );
  }
  return StatusCode::SUCCESS;
}


} // namespace D3PD
