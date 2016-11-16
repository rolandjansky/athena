/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        tau1p3pEleVeto.cxx
// package:     Reconstruction/tauRec
// authors:     Zofia Czyczula
// date:        2006-09-27
//
// 
// This tool veto electrons.
//
// MODIFIED:
// 02-04-2007 - (AK) protection against missing egamma Collection
// 25-03-2008 - (AK for ZC) upgade of the code
// 28-03-1008 - (AK) fix for protection against missing egamma Collection 
//               ERROR->WARNING
// 15/04/2008 - (AK) fixing compilation warning bug #35463
// 03-10-2008 - (ZC) upgarade of the code 
// 16/03/2010 - (AK) use the cell id instead of the pointer 
//-----------------------------------------------------------------------------

////
//TODO: this is cell based and use tracking variables -> can not run on AOD 
//TODO: rename
//

#include <algorithm>
#include <math.h>
#include <sstream>

//#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/IToolSvc.h"
//#include "GaudiKernel/Property.h"

//#include "CaloUtils/CaloCellList.h"
//#include "CaloEvent/CaloCluster.h"
//#include "CaloEvent/CaloCell.h"
#include "CaloUtils/CaloVertexedCell.h"
//#include "AtlasDetDescr/AtlasDetectorID.h"
//#include "CaloIdentifier/CaloID.h"
//#include "CaloIdentifier/CaloCell_ID.h"
//#include "CaloGeoHelpers/CaloSampling.h"

#include "xAODTau/TauJet.h"
#include "xAODJet/Jet.h"
#include "tauRecTools/KineUtils.h"

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TauElectronVetoVariables.h"

using CLHEP::GeV;

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------
TauElectronVetoVariables::TauElectronVetoVariables(const std::string &name) :
TauRecToolBase(name),
m_doCellCorrection(false), //FF: don't do cell correction by default
m_caloExtensionTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool")
{
    declareProperty("CellCorrection", m_doCellCorrection);
    declareProperty("ParticleCaloExtensionTool",   m_caloExtensionTool );
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------
TauElectronVetoVariables::~TauElectronVetoVariables() { }

//-------------------------------------------------------------------------
// Finalizer
//-------------------------------------------------------------------------
StatusCode TauElectronVetoVariables::finalize()
{
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Initializer
//-------------------------------------------------------------------------
StatusCode TauElectronVetoVariables::initialize()
{
    if (m_caloExtensionTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Cannot find tool named <" << m_caloExtensionTool << ">");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}
StatusCode TauElectronVetoVariables::eventInitialize()
{
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
// Execution
//-------------------------------------------------------------------------
StatusCode TauElectronVetoVariables::execute(xAOD::TauJet& pTau)
{

    if (pTau.nTracks() < 1) {
        return StatusCode::SUCCESS;
    }

    ATH_MSG_VERBOSE(name() << " in execute() ...");

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

    signum_eta = pTau.track(0)->eta() / fabs(pTau.track(0)->eta());

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

    //use tau vertex to correct cell position
    bool applyCellCorrection = false;
    if (m_doCellCorrection && pTau.vertexLink()) {
       applyCellCorrection = true;
    }

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

    // get the extrapolation into the calo
    const Trk::CaloExtension* caloExtension = 0;
    if( !m_caloExtensionTool->caloExtension(*pTau.track(0)->track(),caloExtension) || caloExtension->caloLayerIntersections().empty() ){
      ATH_MSG_WARNING("extrapolation of leading track to calo surfaces failed  " );
      return StatusCode::SUCCESS;
    }

    // loop over calo layers
    for( auto cur = caloExtension->caloLayerIntersections().begin(); cur != caloExtension->caloLayerIntersections().end() ; ++cur ){
      
      // only use entry layer
      if( !parsIdHelper.isEntryToVolume((*cur)->cIdentifier()) ) continue;
      
      CaloSampling::CaloSample sample = parsIdHelper.caloSample((*cur)->cIdentifier());
      int index = -1;
      if( sample == CaloSampling::PreSamplerE || sample == CaloSampling::PreSamplerB ) index = 0;
      else if( sample == CaloSampling::EME1 || sample == CaloSampling::EMB1 )          index = 1;
      else if( sample == CaloSampling::EME2 || sample == CaloSampling::EMB2 )          index = 2;
      else if( sample == CaloSampling::EME3 || sample == CaloSampling::EMB3 )          index = 3;
      if( index < 0 ) continue;
      eta_extrapol[index] = (*cur)->position().eta();
      phi_extrapol[index] = (*cur)->position().phi();
    }

    for (int i = 0; i < numOfsampEM; ++i) {
      if ( eta_extrapol[i] < -11110. || phi_extrapol[i] < -11110. )
	{
	  ATH_MSG_DEBUG("extrapolation of leading track to calo surfaces failed for sampling : " << i );
	  return StatusCode::SUCCESS;
	}
    }

    const xAOD::Jet* pJetSeed = (*pTau.jetLink());
    if (!pJetSeed) {
      ATH_MSG_WARNING("tau does not have jet seed for electron veto cell variable calculation");
      return StatusCode::SUCCESS;
    }

    xAOD::JetConstituentVector::const_iterator cItr = pJetSeed->getConstituents().begin();
    xAOD::JetConstituentVector::const_iterator cItrE = pJetSeed->getConstituents().end();

    std::bitset<200000> cellSeen;

    for (; cItr != cItrE; ++cItr) {
      
      const xAOD::CaloCluster* cluster = dynamic_cast<const xAOD::CaloCluster*>( (*cItr)->rawConstituent() ); 
      
      CaloClusterCellLink::const_iterator pCellIter  = cluster->getCellLinks()->begin();
      CaloClusterCellLink::const_iterator pCellIterE = cluster->getCellLinks()->end();
     

      double cellPhi;
      double cellEta;
      double cellET;
    for (; pCellIter != pCellIterE; pCellIter++) {

        pCell = *pCellIter;
	
	if (cellSeen.test(pCell->caloDDE()->calo_hash())) {
	  //already encountered this cell
	  continue;
	}
	else {
	  //New cell
	  cellSeen.set(pCell->caloDDE()->calo_hash());
	}


        if (applyCellCorrection) {
          //ATH_MSG_INFO( "before cell correction: phi= " << cell->phi() << ", eta= " << cell->eta()<< ", energy= " << cell->energy() << ", et= " <<cell->et() );
          CaloVertexedCell vxCell (*pCell, (*pTau.vertexLink())->position());
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
	detEtaTrk = std::fabs( cellEta - eta_extrapol[i] );
	clEtaTrk = eta_extrapol[i];
	distEtaTrk = cellEta - eta_extrapol[i];

        if ((sampling == 0 && detEtaTrk < eta0cut && detPhiTrk < phi0cut) ||
                (sampling == 1 && detEtaTrk < eta1cut && detPhiTrk < phi1cut) ||
                (sampling == 2 && detEtaTrk < eta2cut && detPhiTrk < phi2cut) ||
                (sampling == 3 && detEtaTrk < eta3cut && detPhiTrk < phi3cut)) {

            sumETCellsLAr += cellET;
        }

        if (sampling == 12 && detEtaTrk < etahadcut && detPhiTrk < phihadcut) sumETCellsHad1 += cellET;

        if (fabs(cellEta) > 0.8 && fabs(cellEta) <= 1.2 && (sampling == 13 || sampling == 14) && detEtaTrk < etahadcut && detPhiTrk < phihadcut) {
            sumETCellsHad1 += cellET;
        }

        if (fabs(pTau.track(0)->eta()) <= 1.7) {
            if (sampling == 1 && detEtaTrk < etamaxcut && detPhiTrk <= phimaxcut) {
                if ((fabs(cellEta) < 1.37 || fabs(cellEta) > 1.52) && fabs(cellEta) < 1.85) {
                    if (fabs(clEtaTrk) <= etacase1 && fabs(cellEta) <= etacase1) {
                        n = 50 + int(distEtaTrk / etagran1);
                    }
                    if (fabs(clEtaTrk) <= etacase1 && fabs(cellEta) > etacase1) {
                        n = 50 + int(signum_eta * ((etacase1 - fabs(clEtaTrk)) / etagran1 + (-etacase1 + fabs(cellEta)) / etagran2));
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

        if (fabs(cellEta) <= etacase1) {
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

    if (fabs(eta[max1]) >= etareg) {
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
        } else {
            TRTratio = 0.0;
        }
    // }

    pTau.setDetail(xAOD::TauJetParameters::TRT_NHT_OVER_NLT , TRTratio );
    pTau.setDetail(xAOD::TauJetParameters::secMaxStripEt , energy_3phi[max] );
    pTau.setDetail(xAOD::TauJetParameters::hadLeakEt , static_cast<float>( sumETCellsHad1 / ( pTau.track(0)->pt() ) ) );
    pTau.setDetail(xAOD::TauJetParameters::sumEMCellEtOverLeadTrkPt , static_cast<float>( ( sumETCellsLAr / ( pTau.track(0)->pt() ) ) ) );


    return StatusCode::SUCCESS;
}




#endif
