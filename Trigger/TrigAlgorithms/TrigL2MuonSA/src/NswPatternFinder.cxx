/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "NswPatternFinder.h"
#include "xAODTrigMuon/TrigMuonDefs.h"
#include <cmath>

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::NswPatternFinder::NswPatternFinder(const std::string& type,
						 const std::string& name,
						 const IInterface*  parent):
  AthAlgTool(type,name,parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::NswPatternFinder::findPatterns(const TrigL2MuonSA::MuonRoad& muonRoad,
							std::vector<StgcHitData>&     stgcHits,
							std::vector<MmHitData>&       mmHits,
							TrigL2MuonSA::TrackPattern&   trackPattern) const
{

  ATH_MSG_DEBUG("Start NSW Pattern Finder");

  trackPattern.mmSegment.clear();
  trackPattern.stgcSegment.clear();

  // const int chamber = xAOD::L2MuonParameters::Chamber::EndcapInner; // NSW is located at endcap inner station

  ATH_CHECK( outlierMm(muonRoad, trackPattern, mmHits) );
  ATH_CHECK( outlierStgc(muonRoad, trackPattern, stgcHits) );

  ATH_MSG_DEBUG("nr of MM hits in segment=" << trackPattern.mmSegment.size());
  ATH_MSG_DEBUG("nr of sTGC hits in segment=" << trackPattern.stgcSegment.size());

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::NswPatternFinder::calc_residualR(const double aw,const double bw,const double x,const double y) const
{
  const double ZERO_LIMIT = 1e-4;
  if( std::abs(aw) < ZERO_LIMIT ) return y-bw;
  double ia  = 1/aw;
  double iaq = ia*ia;
  double dz  = x - (y-bw)*ia;
  return dz/sqrt(1.+iaq);
}

double TrigL2MuonSA::NswPatternFinder::calc_dPhi(const double hitphi, const double roadphi) const
{

  double dphi = std::abs(hitphi - roadphi);
  if( dphi > M_PI*2 ) dphi = dphi - M_PI*2;
  if( dphi > M_PI ) dphi = M_PI*2 - dphi;

  return dphi;
}


StatusCode TrigL2MuonSA::NswPatternFinder::outlierMm(const TrigL2MuonSA::MuonRoad& muonRoad, TrigL2MuonSA::TrackPattern& trackPattern, TrigL2MuonSA::MmHits& mmHits) const
{
  TrigL2MuonSA::MmHits mmSegment;
  mmSegment.clear();

  const int chamber = xAOD::L2MuonParameters::Chamber::EndcapInner; // NSW is located at endcap inner station

  const double aw = muonRoad.aw[chamber][0];
  const double bw = muonRoad.bw[chamber][0];
  const double roadphi = muonRoad.phi[chamber][0];

  double z = -1;
  double r = -1;
  double phi = -1;
  double residualR = -1;
  double residualPhi = -1;
  unsigned int i_layer = 999;
  double rWidth = -1;

  for(unsigned int i_hit=0; i_hit<mmHits.size(); i_hit++) {

    z = mmHits[i_hit].z;
    r = mmHits[i_hit].r;
    phi = mmHits[i_hit].phi;
    i_layer = mmHits[i_hit].layerNumber;
    residualR = calc_residualR(aw,bw,z,r);
    residualPhi = calc_dPhi(phi, roadphi);
    mmHits[i_hit].ResidualR = residualR;
    mmHits[i_hit].ResidualPhi = residualPhi;
    rWidth = muonRoad.rWidth[chamber][i_layer];

    ATH_MSG_DEBUG("Mm .../z/r/aw/bw/residualR/residualPhi/rWidth="
		  << z << "/" << r << "/" << aw << "/" << bw << "/" << residualR << "/" << residualPhi << "/" << rWidth);

    if( std::abs(residualR) > rWidth ) { // outside Road
      mmHits[i_hit].isOutlier = 2;
      continue;
    }

    mmSegment.push_back(mmHits[i_hit]);
  }

  trackPattern.mmSegment = mmSegment;

  return StatusCode::SUCCESS;
}

StatusCode TrigL2MuonSA::NswPatternFinder::outlierStgc(const TrigL2MuonSA::MuonRoad& muonRoad, TrigL2MuonSA::TrackPattern& trackPattern, TrigL2MuonSA::StgcHits& stgcHits) const
{
  TrigL2MuonSA::StgcHits stgcSegment;
  stgcSegment.clear();

  const int chamber = xAOD::L2MuonParameters::Chamber::EndcapInner; // NSW is located at endcap inner station

  const double aw = muonRoad.aw[chamber][0];
  const double bw = muonRoad.bw[chamber][0];
  const double roadphi = muonRoad.phi[chamber][0];

  double z = -1;
  double r = -1;
  double phi = -1;
  double residualR = -1;
  double residualPhi = -1;
  unsigned int i_layer = 999;
  double rWidth = -1;
  const double phiWidth = 0.1;
  int channelType = -1;

  for(unsigned int i_hit=0; i_hit<stgcHits.size(); i_hit++) {

    z = stgcHits[i_hit].z;
    r = stgcHits[i_hit].r;
    phi = stgcHits[i_hit].phi;
    i_layer = stgcHits[i_hit].layerNumber;
    channelType = stgcHits[i_hit].channelType;

    residualR = calc_residualR(aw,bw,z,r);
    residualPhi = calc_dPhi(phi, roadphi);

    stgcHits[i_hit].ResidualR = residualR;
    stgcHits[i_hit].ResidualPhi = residualPhi;

    if(channelType==0) { // pad
      rWidth = muonRoad.rWidth[chamber][i_layer];

      ATH_MSG_DEBUG("Stgc pad .../z/r/aw/bw/residualR/residualPhi/rWidth="
		    << z << "/" << r << "/" << aw << "/" << bw << "/" << residualR << "/" << residualPhi << "/" << rWidth);


      if( std::abs(residualPhi) > phiWidth ) { // outside Road
	stgcHits[i_hit].isOutlier = 3;
	continue;
      }
      if( std::abs(residualR) > rWidth ) { // outside Road
	stgcHits[i_hit].isOutlier = 2;
	continue;
      }

    }
    else if(channelType==1) { // strip
      rWidth = muonRoad.rWidth[chamber][i_layer];

      ATH_MSG_DEBUG("Stgc strip .../z/r/aw/bw/residualR/residualPhi/rWidth="
		    << z << "/" << r << "/" << aw << "/" << bw << "/" << residualR << "/" << residualPhi << "/" << rWidth);

      if( std::abs(residualR) > rWidth ) { // outside Road
	stgcHits[i_hit].isOutlier = 2;
	continue;
      }

    }
    else if(channelType==2) { // wire
      ATH_MSG_DEBUG("Stgc wire .../z/r/aw/bw/residualR/residualPhi/rWidth="
		    << z << "/" << r << "/" << aw << "/" << bw << "/" << residualR << "/" << residualPhi << "/" << rWidth);

      if( std::abs(residualPhi) > phiWidth ) { // outside Road
	stgcHits[i_hit].isOutlier = 3;
	continue;
      }

    }


    stgcSegment.push_back(stgcHits[i_hit]);
  }

  trackPattern.stgcSegment = stgcSegment;

  return StatusCode::SUCCESS;
}
