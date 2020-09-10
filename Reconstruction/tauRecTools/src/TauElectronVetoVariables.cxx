/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include <algorithm>
#include <cmath>
#include <sstream>

#include "GaudiKernel/SystemOfUnits.h"
#include "CaloUtils/CaloVertexedCell.h"

#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "tauRecTools/KineUtils.h"
#include "tauRecTools/HelperFunctions.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TauElectronVetoVariables.h"

using Gaudi::Units::GeV;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------
TauElectronVetoVariables::TauElectronVetoVariables(const std::string &name) :
TauRecToolBase(name) {
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------
TauElectronVetoVariables::~TauElectronVetoVariables() { }

//-------------------------------------------------------------------------
// Initializer
//-------------------------------------------------------------------------
StatusCode TauElectronVetoVariables::initialize()
{
  ATH_CHECK( m_caloExtensionTool.retrieve() );

  if (!m_ParticleCacheKey.key().empty()) {ATH_CHECK(m_ParticleCacheKey.initialize());}
  else {m_useOldCalo = true;}

  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Execution
//-------------------------------------------------------------------------
StatusCode TauElectronVetoVariables::execute(xAOD::TauJet& pTau) const
{
    if (pTau.nTracks() < 1) {
        return StatusCode::SUCCESS;
    }

    ATH_MSG_DEBUG("in execute()");

    float detPhiTrk = 0.;
    float detEtaTrk = 0.;
    float clEtaTrk = 0.;
    float distEtaTrk = 0.;

    float energy_3phi[101] = {0.0};
    float eta[101] = {0.0};
    int max1 = 0;
    int max2 = 0;
    int max = 0;
    int n = 0;
    float Emax1 = 0.;
    float Emax2 = 0.;
    float etamaxcut = 0.158;
    float phimaxcut = 0.1;
    float signum_eta = 0.;

    signum_eta = pTau.track(0)->eta() / std::abs(pTau.track(0)->eta());

    float sumETCellsLAr = 0.;
    float eta0cut = 0.075;
    float eta1cut = 0.0475;
    float eta2cut = 0.075;
    float eta3cut = 1.5;
    float phi0cut = 0.3;
    float phi1cut = 0.3;
    float phi2cut = 0.075;
    float phi3cut = 0.075;
    float etareg = 0.;
    float etacase1 = 1.8;
    float etagran1 = 0.00315;
    float etagran2 = 0.00415;
    float sumETCellsHad1 = 0.;
    float etahadcut = 0.2;
    float phihadcut = 0.2;

    const CaloCell *pCell;

    int trackIndex = -1;

    //---------------------------------------------------------------------
    // Calculate eta, phi impact point of leading track at calorimeter layers EM 0,1,2,3
    //---------------------------------------------------------------------
    Trk::TrackParametersIdHelper parsIdHelper;
        
    const int numOfsampEM = 4;
    double eta_extrapol[4];
    double phi_extrapol[4];

    for (int i = 0; i < numOfsampEM; ++i) {
      eta_extrapol[i] = -11111.;
      phi_extrapol[i] = -11111.;
    }

    /*get the CaloExtension object*/
    const Trk::CaloExtension * caloExtension = nullptr;
    std::unique_ptr<Trk::CaloExtension> uniqueExtension ;
    const xAOD::TrackParticle *orgTrack = pTau.track(0)->track();
    trackIndex = orgTrack->index();

    if (m_useOldCalo) {
      /* If CaloExtensionBuilder is unavailable, use the calo extension tool */
      ATH_MSG_VERBOSE("Using the CaloExtensionTool");
      uniqueExtension = m_caloExtensionTool->caloExtension(*orgTrack);
      caloExtension = uniqueExtension.get();
    } else {
      /*get the CaloExtension object*/
      ATH_MSG_VERBOSE("Using the CaloExtensionBuilder Cache");
      SG::ReadHandle<CaloExtensionCollection>  particleCache {m_ParticleCacheKey};
      caloExtension = (*particleCache)[trackIndex];
      ATH_MSG_VERBOSE("Getting element " << trackIndex << " from the particleCache");
      if( not caloExtension ){
        ATH_MSG_VERBOSE("Cache does not contain a calo extension -> Calculating with the a CaloExtensionTool" );
        uniqueExtension = m_caloExtensionTool->caloExtension(*orgTrack);
        caloExtension = uniqueExtension.get();
      }
    }

    const std::vector<const Trk::CurvilinearParameters*>& clParametersVector = caloExtension->caloLayerIntersections();
    
    if( not caloExtension || clParametersVector.empty() ){
      ATH_MSG_WARNING("extrapolation of leading track to calo surfaces failed  : caloLayerIntersection is empty" );
      return StatusCode::SUCCESS;
    }

    // loop over calo layers
    for( const Trk::CurvilinearParameters * cur : clParametersVector ){
      
      // only use entry layer
      if( !parsIdHelper.isEntryToVolume(cur->cIdentifier()) ) continue;
      
      CaloSampling::CaloSample sample = parsIdHelper.caloSample(cur->cIdentifier());
      int index = -1;
      if( sample == CaloSampling::PreSamplerE || sample == CaloSampling::PreSamplerB ) index = 0;
      else if( sample == CaloSampling::EME1 || sample == CaloSampling::EMB1 )          index = 1;
      else if( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 )          index = 2;
      else if( sample == CaloSampling::EME3 || sample == CaloSampling::EMB3 )          index = 3;
      if( index < 0 ) continue;
      eta_extrapol[index] = cur->position().eta();
      phi_extrapol[index] = cur->position().phi();
    }

    for (int i = 0; i < numOfsampEM; ++i) {
      if ( eta_extrapol[i] < -11110. || phi_extrapol[i] < -11110. )
      {
        ATH_MSG_DEBUG("extrapolation of leading track to calo surfaces failed for sampling : " << i );
        return StatusCode::SUCCESS;
      }
    }

    const xAOD::Jet* pJetSeed = pTau.jet();
    if (!pJetSeed) {
      ATH_MSG_ERROR("tau does not have jet seed for electron veto cell variable calculation");
      return StatusCode::FAILURE;
    }

    // Loop through jets, get links to clusters
    std::vector<const xAOD::CaloCluster*> clusterList;
    ATH_CHECK(tauRecTools::GetJetClusterList(pJetSeed, clusterList, m_incShowerSubtr));

    std::bitset<200000> cellSeen;

    for (auto cluster : clusterList){

      CaloClusterCellLink::const_iterator pCellIter  = cluster->getCellLinks()->begin();
      CaloClusterCellLink::const_iterator pCellIterE = cluster->getCellLinks()->end();

      for (; pCellIter != pCellIterE; pCellIter++) {

	double cellEta, cellPhi, cellET;

        pCell = *pCellIter;
	    if (cellSeen.test(pCell->caloDDE()->calo_hash())) continue;
	    else cellSeen.set(pCell->caloDDE()->calo_hash());

        if (m_doVertexCorrection && pTau.vertexLink()) {
          CaloVertexedCell vxCell (*pCell, pTau.vertex()->position());
          cellPhi = vxCell.phi();
          cellEta = vxCell.eta();
          cellET = vxCell.et();
        }
        else {
          cellPhi = pCell->phi();
          cellEta = pCell->eta();
          cellET = pCell->et();
        }
	
        int sampling = pCell->caloDDE()->getSampling();
        if (sampling == 4) sampling = 0;
        if (sampling == 5) sampling = 1;
        if (sampling == 6) sampling = 2;
        if (sampling == 7) sampling = 3;
        if (sampling == 18) sampling = 12;
        if (sampling == 8) sampling = 12;
        if (sampling == 15) sampling = 12;
        if (sampling == 19) sampling = 13;
        if (sampling == 16) sampling = 13;
        if (sampling == 20) sampling = 14;
        if (sampling == 17) sampling = 14;
	
        int i = 2;
        if (sampling < 4) i = sampling;
        if (sampling == 12 || sampling == 13 || sampling == 14) i = 3;

        detPhiTrk = Tau1P3PKineUtils::deltaPhi( cellPhi, phi_extrapol[i] );
	    detEtaTrk = std::abs( cellEta - eta_extrapol[i] );
	    clEtaTrk = eta_extrapol[i];
	    distEtaTrk = cellEta - eta_extrapol[i];

        if ((sampling == 0 && detEtaTrk < eta0cut && detPhiTrk < phi0cut) ||
                (sampling == 1 && detEtaTrk < eta1cut && detPhiTrk < phi1cut) ||
                (sampling == 2 && detEtaTrk < eta2cut && detPhiTrk < phi2cut) ||
                (sampling == 3 && detEtaTrk < eta3cut && detPhiTrk < phi3cut)) {

            sumETCellsLAr += cellET;
        }

        if (sampling == 12 && detEtaTrk < etahadcut && detPhiTrk < phihadcut) sumETCellsHad1 += cellET;

        if (std::abs(cellEta) > 0.8 && std::abs(cellEta) <= 1.2 && (sampling == 13 || sampling == 14) && detEtaTrk < etahadcut && detPhiTrk < phihadcut) {
            sumETCellsHad1 += cellET;
        }

        if (std::abs(pTau.track(0)->eta()) <= 1.7) {
            if (sampling == 1 && detEtaTrk < etamaxcut && detPhiTrk <= phimaxcut) {
                if ((std::abs(cellEta) < 1.37 || std::abs(cellEta) > 1.52) && std::abs(cellEta) < 1.85) {
                    if (std::abs(clEtaTrk) <= etacase1 && std::abs(cellEta) <= etacase1) {
                        n = 50 + int(distEtaTrk / etagran1);
                    }
                    if (std::abs(clEtaTrk) <= etacase1 && std::abs(cellEta) > etacase1) {
                        n = 50 + int(signum_eta * ((etacase1 - std::abs(clEtaTrk)) / etagran1 + (-etacase1 + std::abs(cellEta)) / etagran2));
                    }
                    energy_3phi[n] = energy_3phi[n] + cellET / GeV;
                    eta[n] = signum_eta * (clEtaTrk - cellEta);
                } else {
                    energy_3phi[n] = 0;
                    eta[n] = 0;
                }
            }
        } else {
            energy_3phi[n] = 0;
            eta[n] = 0;
        }

        if (std::abs(cellEta) <= etacase1) {
            etareg = 0.00315;
        } else {
            etareg = 0.00415;
        }
        
      } //end cell loop

    }// end jet constituent loop

    for (int m1 = 0; m1 < 101; m1++) {
        if ((energy_3phi[m1] > Emax1)) {
            Emax1 = energy_3phi[m1];
            max1 = m1;
        }
    }

    for (int m2 = 1; m2 < 100; m2++) {
        if (m2 == max1) continue;

        if ((energy_3phi[m2] > Emax2) && (energy_3phi[m2] > energy_3phi[m2 - 1]) && (energy_3phi[m2] > energy_3phi[m2 + 1])) {
            Emax2 = energy_3phi[m2];
            max2 = m2;
        }
    }

    if (std::abs(eta[max1]) >= etareg) {
        max = max1;
    } else {
        max = max2;
    }

    float TRTratio = -9999.0;
    uint8_t TRTHTHits;
    uint8_t TRTHTOutliers;
    uint8_t TRTHits;
    uint8_t TRTOutliers;

    const xAOD::TrackParticle* leadTrack = pTau.track(0)->track();
      
    if ( !leadTrack->summaryValue( TRTHits, xAOD::SummaryType::numberOfTRTHits ) )
    {
      ATH_MSG_DEBUG("retrieval of track summary value failed. Not filling electron veto variables for this one prong candidate");
      return StatusCode::SUCCESS;
    }
    if ( !leadTrack->summaryValue( TRTHTHits, xAOD::SummaryType::numberOfTRTHighThresholdHits ) )
    {
      ATH_MSG_DEBUG("retrieval of track summary value failed. Not filling electron veto variables for this one prong candidate");
      return StatusCode::SUCCESS;
    }
    if ( !leadTrack->summaryValue( TRTOutliers, xAOD::SummaryType::numberOfTRTOutliers ) )
    {
      ATH_MSG_DEBUG("retrieval of track summary value failed. Not filling electron veto variables for this one prong candidate");
      return StatusCode::SUCCESS;
    }
    if ( !leadTrack->summaryValue( TRTHTOutliers, xAOD::SummaryType::numberOfTRTHighThresholdOutliers ) )
    {
      ATH_MSG_DEBUG("retrieval of track summary value failed. Not filling electron veto variables for this one prong candidate");
      return StatusCode::SUCCESS;
    }
      

    if (TRTHits + TRTOutliers != 0) {
	  TRTratio = float( TRTHTHits + TRTHTOutliers) / float( TRTHits + TRTOutliers );
    } 
    else {
      TRTratio = 0.0;
    }

    pTau.setDetail(xAOD::TauJetParameters::TRT_NHT_OVER_NLT , TRTratio );
    pTau.setDetail(xAOD::TauJetParameters::secMaxStripEt , energy_3phi[max] );
    pTau.setDetail(xAOD::TauJetParameters::hadLeakEt , static_cast<float>( sumETCellsHad1 / ( pTau.track(0)->pt() ) ) );
    pTau.setDetail(xAOD::TauJetParameters::sumEMCellEtOverLeadTrkPt , static_cast<float>( ( sumETCellsLAr / ( pTau.track(0)->pt() ) ) ) );


    return StatusCode::SUCCESS;
}

#endif
