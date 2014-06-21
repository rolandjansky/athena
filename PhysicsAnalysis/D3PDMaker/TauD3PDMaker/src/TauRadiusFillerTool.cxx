/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCellsDetailsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date December, 2009
 * @brief Block filler tool for tau track details.
 */


#include "TauRadiusFillerTool.h"
#include "tauEvent/TauJet.h"
#include "tauEvent/TauJetContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
#include "CaloEvent/CaloCellLink.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "JetEvent/Jet.h"
#include "FourMomUtils/P4Helpers.h"
#include "EventKernel/SignalStateHelper.h"	
#include "AthenaKernel/errorcheck.h"
#include "AnalysisUtils/AnalysisMisc.h"
#include "Particle/TrackParticle.h"


namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
TauRadiusFillerTool::TauRadiusFillerTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : BlockFillerTool<Analysis::TauJet> (type, name, parent),
    m_emrademscale(0), m_centfracemscale(0), m_isofracemscale(0), m_trkavgdist(0)
{
}


/**
 * @brief Book variables for this block.
 */
StatusCode TauRadiusFillerTool::book()
{
    CHECK( addVariable ("calcVars_EMRadEMScale",  m_emrademscale )  );
    CHECK( addVariable ("calcVars_centfracEMScale",  m_centfracemscale )  );
    CHECK( addVariable ("calcVars_isofracEMScale",  m_isofracemscale )  );
    CHECK( addVariable ("calcVars_trkAvgDistEMScale",  m_trkavgdist )  );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode TauRadiusFillerTool::fill (const Analysis::TauJet& p)
{
  // find the eta/phi of the jet seed at EM Scale
  const Jet* jetSeed = p.jet();

  //  std::cout << "DUGAN eta, phi are " << jetSeed->eta() << " " << jetSeed->phi() << std::endl;

  SignalStateHelper sigstateH( jetSeed , P4SignalState::State(0));


  // std::cout << "DUGAN EM eta, phi are " << jetSeed->eta() << " " << jetSeed->phi() << std::endl;

  P4EEtaPhiM P4Seed( 1., jetSeed->eta(), jetSeed->phi(), 0. );
  // now setup loop over cells
  NavigationToken<CaloCell> nt;
  std::vector<const CaloCell*> Cells;
  p.fillToken( nt ); 
  NavigationToken<CaloCell>::const_iterator firstcell = nt.begin();
  NavigationToken<CaloCell>::const_iterator lastcell = nt.end();
  double EMRadius = 0.;
  double sumEMCellET = 0.;
  double ET1Sum = 0;
  double ET4Sum = 0;
  double ET12Sum = 0;       
  double EMET1Sum = 0;
  double EMET4Sum = 0;   
  double sumCellE = 0.;
  const CaloCell *cell;
  double cellEta, cellPhi, cellEnergy, cellET;
  for ( ; firstcell != lastcell; firstcell++ ) {
    cell = *firstcell;

    cellEta = cell->eta();
    cellPhi = cell->phi();
    cellEnergy = cell->energy();
    cellET = cell->et();
    CaloSampling::CaloSample calo = CaloSamplingHelper::getSampling( *cell );
    double dR = P4Helpers::deltaR( P4Seed, cellEta, cellPhi );	               
    if ( dR < 0.4 ) {
      sumCellE += cellEnergy;
      ET4Sum += cellET;
    }
    if ( dR < 0.1 ) ET1Sum += cellET;
    if ( 0.1 < dR && dR < 0.2 ) ET12Sum += cellET;

    if ( ( calo == CaloSampling::PreSamplerB ) ||
	 ( calo == CaloSampling::PreSamplerE ) ||
	 ( calo == CaloSampling::EMB1 ) ||
	 ( calo == CaloSampling::EME1 ) ||
	 ( calo == CaloSampling::EMB2 ) ||
	 ( calo == CaloSampling::EME2 )) {
      if ( dR < 0.1 ) EMET1Sum += cellET;
      if ( dR < 0.4 ) EMET4Sum += cellET;
      EMRadius += dR*cellET;
      sumEMCellET += cellET;


    }

  } // end cell loop 


  if(fabs(sumEMCellET) >  0.000001){
    *m_emrademscale = EMRadius / sumEMCellET;
  }
  else
    {
      *m_emrademscale = -1.0;
    }
  //
  if(fabs(ET4Sum) >  0.000001){
    *m_centfracemscale = ET1Sum/ET4Sum;
    *m_isofracemscale = ET12Sum/ET4Sum;
      }
  else
    {
      *m_centfracemscale = -1.0;
      *m_isofracemscale = -1.0;
    }
  // Now loop over tau tracks to calculate the track radius from the same
  // axis in the jet
   unsigned int numTrack = p.numTrack();
   double trkRadius = 0.;
   double sumPtTracks = 0.;  
   double ptTrack = 0.;
   double dRTrack = 0.;
   for(unsigned int i=0; i < numTrack; i++)
     {
       const Rec::TrackParticle* track = p.track(i);
       if (!track){
	 return StatusCode::SUCCESS;       
       }

       ptTrack = track->pt();
       sumPtTracks += ptTrack;
       dRTrack = (track->hlv()).deltaR(P4Seed.hlv());
       trkRadius += dRTrack*ptTrack;
     }
   if(fabs(sumPtTracks) >  0.000001){
     *m_trkavgdist = trkRadius / sumPtTracks;
   }
   else
     {
       *m_trkavgdist = -1.0;
     }

  return StatusCode::SUCCESS;
} // end fill method

} // namespace D3PD
