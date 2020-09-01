/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMatchingTool.h"

#include "MuonSegment/MuonSegment.h"

#include <cmath>
#include <iostream>

namespace Muon {

  MuonSegmentMatchingTool::MuonSegmentMatchingTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_overlapResolvingTool("Muon::MuonSegmentInOverlapResolvingTool/MuonSegmentInOverlapResolvingTool"),
      m_pairMatchingTool("Muon::MuonSegmentPairMatchingTool/MuonSegmentPairMatchingTool"),
      m_straightLineMatches(0),m_straightLineMatchesGood(0),
      m_overlapMatches(0),m_overlapMatchesGood(0),
      m_curvedMatches(0),m_curvedMatchesGood(0)
  {
    declareInterface<IMuonSegmentMatchingTool>(this);
    declareProperty("UseCosmicsSettings",  m_isCosmics = false, "Pick up settings for cosmics");
    declareProperty("DoOverlapMatch",      m_doOverlapMatch      = true, "Perform matching for segments in a small/large overlap");
    declareProperty("DoStraightLineMatch", m_doStraightLineMatch = true, "Perform matching for segments in regions without field");
    declareProperty("DoCurvedMatch",       m_doCurvedMatch       = true, "Perform matching for segments in a field regions");
    declareProperty("doThetaMatching",     m_thetaMatch  = false, "Pre-matching in theta" );
    declareProperty("doPhiMatching",       m_phiMatch    = false, "Pre-matching in phi" );

    declareProperty("OverlapMatchAngleDPhiCut",m_overlapMatchAngleDPhiCut = 0.15,
		    "Cut on the angular difference between the best phi and the one consistent with the chamber bounds");
    declareProperty("OverlapMatchAngleDYZCut",m_overlapMatchAngleDYZCut = 0.05,
		    "Cut on the angular difference between the best phi and the one consistent with the chamber bounds");
    declareProperty("OverlapMatchPositionCut",m_overlapMatchPositionCut = 100.,
		    "Cut on the distance of recalculated position to the tube edge");
    declareProperty("OverlapMatchPositionResidualCut",m_overlapMatchPositionResidualCut = 30.,
		    "Cut on the segment position residual after recalculation of the paramters");
    declareProperty("OverlapMatchAveragePhiHitPullCut",m_overlapMatchPhiHitPullCut = 20.,
		    "Cut on the average pull of the phi hits with the new segment parameters");

    declareProperty("StraightLineMatchAngleCut",m_straightLineMatchAngleCut = 0.1,
		    "Cut on the angular difference between the extrapolated segment angle and reference");
    declareProperty("StraightLineMatchPositionCut",m_straightLineMatchPositionCut = 200.,
		    "Cut on the distance of extrapolated segment position and reference");
    declareProperty("MaxDistanceBetweenSegments",m_maxDistSegments = 5000.,
		    "If the two segments are further apart than this distance, they are considered to not match");

    declareProperty("OnlySameSectorIfTight",m_onlySameSectorIfTight = true, "Accept only segments that are in the same sector for tight matching");
    declareProperty("TightSegmentMatching",m_useTightCuts = false, "Use tight selection for busy event to suppress combinatorics and improve CPU");

    declareProperty("DumpAngles", m_dumpAngles = false, "Print matching angle information to screen. WARNING: always returns True for suppressNoise");
    declareProperty("DoMatchingCutsBIBM_S", m_matchingbibm_sphisec = 0.015, "Cut on sumDeltaYZ, segments in BI and BM, small phi sec"); 
    declareProperty("DoMatchingCutsBIBO_S", m_matchingbibo_sphisec = 0.015, "Cut on sumDeltaYZ, segments in BI and BO, small phi sec"); 
    declareProperty("DoMatchingCutsBMBO_S", m_matchingbmbo_sphisec = 0.015, "Cut on sumDeltaYZ, segments in BM and BO, small phi sec");
    declareProperty("DoMatchingCutsEIEM_S", m_matchingeiem_sphisec = 0.010*2, "Cut on sumDeltaYZ, segments in EI and EM, small phi sec");  
    declareProperty("DoMatchingCutsEIEO_S", m_matchingeieo_sphisec = 0.020*2, "Cut on sumDeltaYZ, segments in EI and EO, small phi sec"); 
    declareProperty("DoMatchingCutsEMEO_S", m_matchingemeo_sphisec = 0.002, "Cut on sumDeltaYZ, segments in EM and EO, small phi sec");
    declareProperty("DoMatchingCutsBIBM_L", m_matchingbibm_lphisec = 0.005, "Cut on sumDeltaYZ, segments in BI and BM, large phi sec"); 
    declareProperty("DoMatchingCutsBIBO_L", m_matchingbibo_lphisec = 0.015, "Cut on sumDeltaYZ, segments in BI and BO, large phi sec"); 
    declareProperty("DoMatchingCutsBMBO_L", m_matchingbmbo_lphisec = 0.010, "Cut on sumDeltaYZ, segments in BM and BO, large phi sec");
    declareProperty("DoMatchingCutsEIEM_L", m_matchingeiem_lphisec = 0.015*2, "Cut on sumDeltaYZ, segments in EI and EM, large phi sec");  
    declareProperty("DoMatchingCutsEIEO_L", m_matchingeieo_lphisec = 0.025*2, "Cut on sumDeltaYZ, segments in EI and EO, large phi sec"); 
    declareProperty("DoMatchingCutsEMEO_L", m_matchingemeo_lphisec = 0.002, "Cut on sumDeltaYZ, segments in EM and EO, large phi sec");     
    declareProperty("UseEndcapExtrapolationMatching", m_useEndcapExtrapolationMatching = true );
    declareProperty("DrExtrapolationRMS", m_drExtrapRMS = 10 );
    declareProperty("DThetaExtrapolationRMS", m_dthetaExtrapRMS = 0.01*2 );
    declareProperty("DrExtrapolationAlignementOffset", m_drExtrapAlignmentOffset = 50 );
}

  StatusCode MuonSegmentMatchingTool::initialize()
  {
    ATH_CHECK(AthAlgTool::initialize());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_pairMatchingTool.retrieve());
    ATH_CHECK(m_overlapResolvingTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode MuonSegmentMatchingTool::finalize()
  {
    double goodOverlapMatchFraction = m_overlapMatches != 0 ? (double)m_overlapMatchesGood/((double)m_overlapMatches) : 0.;
    ATH_MSG_INFO("Overlap matches:      total " << std::setw(7) << m_overlapMatches 
		 << " good " << std::setw(7) << m_overlapMatchesGood << " fraction " << goodOverlapMatchFraction);

    double goodStraightLineMatchFraction = m_straightLineMatches != 0 ? (double)m_straightLineMatchesGood/((double)m_straightLineMatches) : 0.;
    ATH_MSG_INFO("StraightLine matches: total " << std::setw(7) << m_straightLineMatches 
		 << " good " << std::setw(7) << m_straightLineMatchesGood << " fraction " << goodStraightLineMatchFraction);

    double goodCurvedMatchFraction = m_curvedMatches != 0 ? (double)m_curvedMatchesGood/((double)m_curvedMatches) : 0.;
    ATH_MSG_INFO("Curved matches:       total " << std::setw(7) << m_curvedMatches 
		 << " good " << std::setw(7) << m_curvedMatchesGood << " fraction " << goodCurvedMatchFraction);


    if( AthAlgTool::finalize().isFailure() ) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }


  bool MuonSegmentMatchingTool::match( const MuonSegment& seg1, const MuonSegment& seg2 ) const {
    
    ATH_MSG_VERBOSE(" match: segments " << std::endl
			   << m_printer->print(seg1) << std::endl << m_printer->print(seg2));
   
    // get identifiers 
    Identifier chid1 = m_edmHelperSvc->chamberId(seg1);
    Identifier chid2 = m_edmHelperSvc->chamberId(seg2);
    if( chid1 == chid2 ) return false;

    MuonStationIndex::StIndex stIndex1 = m_idHelperSvc->stationIndex(chid1);
    MuonStationIndex::StIndex stIndex2 = m_idHelperSvc->stationIndex(chid2);

    if( isSLMatch(chid1,chid2) ){ 
      if( !m_idHelperSvc->isMdt(chid1) || !m_idHelperSvc->isMdt(chid2)) return false;
      // if there is a stereo angle match using overlap matching tool

      if( stIndex1 == stIndex2 ){
	
	if( hasStereoAngle(chid1,chid2) ){
	  if( !m_doOverlapMatch ) return true;
	  int eta1 = m_idHelperSvc->mdtIdHelper().stationEta(chid1);
	  int eta2 =  m_idHelperSvc->mdtIdHelper().stationEta(chid2); 
	  int phi1 = m_idHelperSvc->sector(chid1);
	  int phi2 =  m_idHelperSvc->sector(chid2);
	  
	  // require that the two segments are close in eta AND are in adjecent sectors
	  if( (eta1==eta2||eta1==eta2-1||eta1==eta2+1) && 
	      (phi1==(phi2+1)      ||  phi1==(phi2-1) ||
	       (phi1==1&&phi2==16) || (phi1==16&&phi2==1) ) ) {
	    return overlapMatch(seg1,seg2);
	  }else return false;
	}
      }
      if( !m_doStraightLineMatch ) return true;
      
      if( stIndex1 == stIndex2 ) return false;

      return straightLineMatch(seg1,seg2);
    }
    
    // if we get here perform a curved matching
    if( !m_doCurvedMatch ) return true;
    if( stIndex1 == stIndex2 ) return false;
    
    return curvedMatch(seg1,seg2);

  }


  bool MuonSegmentMatchingTool::straightLineMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const {
    ++m_straightLineMatches;

    ATH_MSG_VERBOSE(" straight line match ");
    
    // Suppress cavern background noise and combinatorics using both theta and phi matching
    if ( m_thetaMatch )
      if( !suppressNoise(seg1, seg2, m_useTightCuts ) ) {
	return false;
      }

    // Suppress cavern background noise and combinatorics using phi matching
    if ( m_phiMatch ){
      if( !suppressNoisePhi(seg1, seg2, m_useTightCuts ) ) return false;
    }
    
    ++m_straightLineMatchesGood;
    return true;
  }

  bool MuonSegmentMatchingTool::curvedMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const {
    ++m_curvedMatches;

    ATH_MSG_VERBOSE(" curved match ");

    // Suppress cavern background noise and combinatorics using both theta and phi matching
    if ( m_thetaMatch )
      if( !suppressNoise(seg1, seg2, m_useTightCuts ) ) {
	return false;
      }

    // Suppress cavern background noise and combinatorics using phi matching
    if ( m_phiMatch ){
      if( !suppressNoisePhi(seg1, seg2, m_useTightCuts ) ) return false;
    }

    ++m_curvedMatchesGood;
    return true;
  }

  bool MuonSegmentMatchingTool::overlapMatch( const MuonSegment& seg1, const MuonSegment& seg2 ) const {
    ++m_overlapMatches;
    
    ATH_MSG_VERBOSE(" overlap match ");

    Identifier chid = m_edmHelperSvc->chamberId(seg1);

    // check the distance between the two segments
    float segDist = (seg1.globalPosition() - seg2.globalPosition()).mag();
    ATH_MSG_VERBOSE(" Distance between segments " << segDist );
    if(m_isCosmics && segDist > m_minDistSegmentsCosmics) {
      ATH_MSG_DEBUG(" Too far appart, accepting ");
      return true;
    }
    if (segDist > m_maxDistSegments) return false;

    if( !m_idHelperSvc->isMdt(chid) ) {
      ATH_MSG_DEBUG(" not a mdt segment " << m_idHelperSvc->toString(chid));
      return true;
    }

    IMuonSegmentInOverlapResolvingTool::SegmentMatchResult result = m_overlapResolvingTool->matchResult(seg1,seg2);
    
    ATH_MSG_VERBOSE(result.toString());

    if( !result.goodMatch() ){
      ATH_MSG_DEBUG(" bad match ");
      return false;
    }
    
    result.phiResult = m_overlapResolvingTool->bestPhiMatch(seg1,seg2);
    if( fabs(result.angularDifferencePhi) > m_overlapMatchAngleDPhiCut || fabs(result.phiResult.deltaYZ) > m_overlapMatchAngleDYZCut ) {
      ATH_MSG_DEBUG(" failed angle cut: diff phi  " << result.angularDifferencePhi
			   << "  deltaYZ " << result.phiResult.deltaYZ);
      return false;
    }

    if( fabs(result.averagePhiHitPullSegment1) > m_overlapMatchPhiHitPullCut || fabs(result.averagePhiHitPullSegment2) > m_overlapMatchPhiHitPullCut ) {
      ATH_MSG_DEBUG(" failed phi hit pull cut: seg1 " << result.averagePhiHitPullSegment1
			   << "  seg2 " << result.averagePhiHitPullSegment2);
      return false;
    }

    if( !result.segmentResult1.inBounds(m_overlapMatchPositionCut) || !result.segmentResult2.inBounds(m_overlapMatchPositionCut) ){
      ATH_MSG_DEBUG(" failed position cut ");
      return false;
    }

    if( fabs(result.segmentResult1.positionResidual) > m_overlapMatchPositionResidualCut ||
	fabs(result.segmentResult2.positionResidual) > m_overlapMatchPositionResidualCut ){
      ATH_MSG_DEBUG(" failed position residual cut: seg1 " << result.segmentResult1.positionResidual
			   << "  seg2 " << result.segmentResult2.positionResidual);
      return false;
    }

    ++m_overlapMatchesGood;
    return true;
  }

  bool MuonSegmentMatchingTool::isSLMatch( const Identifier& chid1, const Identifier& chid2 ) const {
    
    // check whether there is field
    if(!m_toroidOn) return true;

    MuonStationIndex::StIndex stIndex1 = m_idHelperSvc->stationIndex(chid1);
    MuonStationIndex::StIndex stIndex2 = m_idHelperSvc->stationIndex(chid2);
    
    // check whether segments in same station
    if( stIndex1 == stIndex2 ) return true;

    // check whether segments in endcap EM/EO region
    if( (stIndex1==MuonStationIndex::EO && stIndex2==MuonStationIndex::EM) ||
	(stIndex1==MuonStationIndex::EM && stIndex2==MuonStationIndex::EO) ) return true;
    
    // all other cases should be treated with curvature
    return false;
  }

  bool MuonSegmentMatchingTool::hasStereoAngle( const Identifier& id1, const Identifier& id2 ) const {
   
    // check whether same phi, else stereo angle (need correction for cosmic up-down tracks)
    int phi1 = m_idHelperSvc->mdtIdHelper().stationPhi(id1);
    int phi2 =  m_idHelperSvc->mdtIdHelper().stationPhi(id2);
    
    if( phi1!=phi2) return true; 
    
    // check whether there is a small/large overlap 
    bool isSmallChamber1 = m_idHelperSvc->isSmallChamber(id1);
    bool isSmallChamber2 = m_idHelperSvc->isSmallChamber(id2);

    return isSmallChamber1 != isSmallChamber2;
  }


  // DOMINIQUE:
  // Check that segment pairs is compatible with being produce within the calorimeter volume
  // Keep it loose to retain long live particles decaying in calo
  bool MuonSegmentMatchingTool::suppressNoise( const MuonSegment& seg1, const MuonSegment& seg2, const bool& useTightCuts ) const {

    // calculate matching variables
    IMuonSegmentPairMatchingTool::SegmentMatchResult result = m_pairMatchingTool->matchResult(seg1,seg2);

    MuonStationIndex::StIndex station_a = m_idHelperSvc->stationIndex( result.chid_a );
    MuonStationIndex::StIndex station_b = m_idHelperSvc->stationIndex( result.chid_b );

    bool isEndcap_a   = m_idHelperSvc->isEndcap( result.chid_a );
    bool isCSC_a      = m_idHelperSvc->isCsc( result.chid_a );
    bool isBEE_a      = station_a == MuonStationIndex::BE;

    bool isEndcap_b   = m_idHelperSvc->isEndcap( result.chid_b );
    bool isCSC_b      = m_idHelperSvc->isCsc( result.chid_b );
    bool isBEE_b      = station_a == MuonStationIndex::BE;

    
    if ( m_dumpAngles ) {

      std::cout << "SegmentPositionChange "
	      << " " << m_idHelperSvc->chamberNameString(result.chid_a)
	      << " " << m_idHelperSvc->chamberNameString(result.chid_b)
	      << " " << result.phiSector_a
	      << " " << result.phiSector_b
	      << " " << result.deltaTheta_a
	      << " " << result.deltaTheta_b
	      << " " << result.deltaTheta
	      << " " << result.angleAC
	      << " " << result.angleBC
	      << " " << result.angleAB
	      << std::endl;
//      return true; // to get the maximum statistics and not be hindered by current cuts
    }    
  
    ATH_MSG_VERBOSE( "matching " << m_idHelperSvc->chamberNameString(result.chid_a)
		     << " " << m_idHelperSvc->chamberNameString(result.chid_b)
		     << " phis " << result.phiSector_a
		     << " " << result.phiSector_b
		     << " thetas " << result.deltaTheta_a
		     << " " << result.deltaTheta_b
		     << " thetaSum " << result.deltaTheta 
		     << " tight cuts " << useTightCuts );
    
    // The difference between the segment direction in the inner and outer stations ~< 60 degrees in all cases
    if ( result.angleAB > 1.0 ) return false;

      
    // First: loose selection
    if ( !useTightCuts ) {
      if ( isCSC_a || isCSC_b ) {
         ATH_MSG_VERBOSE( " check CSC result ");
        if ( result.phiSector_a != result.phiSector_b ) {
	  return false;
        }
	if( (isCSC_a && !isEndcap_b) || (isCSC_b && !isEndcap_a) ) return false; 
 	return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
      }
      // BEE
      else if ( isBEE_a || isBEE_b ) {
         ATH_MSG_VERBOSE( " check BEE result ");
	if ( result.deltaTheta > 0.300 ) {
	  return false;
	} else {
	  return true;
	}
      }
      // Barrel/endcap overlap  
      else if ( isEndcap_a != isEndcap_b ) {
        ATH_MSG_VERBOSE( " check B-E result ");
	if ( result.deltaTheta > 0.300 ) {
	  return false;
	} else {
	  return true;
	}
      }
      // Phi-sector overlap  
      else if ( result.phiSector_a != result.phiSector_b ) {
        ATH_MSG_VERBOSE( " check phiSector result ");
	if ( result.deltaTheta > 0.300 ) {
          ATH_MSG_VERBOSE( " check phiSector reject ");
	  return false;
	} else {
	  return true;
	}
      }
      // Barrel inner to middle station 
      else if ( station_a == MuonStationIndex::BI &&
		station_b == MuonStationIndex::BM ) {
        ATH_MSG_VERBOSE( " check BI BM result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingbibm_sphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingbibm_lphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
      }
      // Barrel inner to outer station 
      else if ( station_a == MuonStationIndex::BI &&
		station_b == MuonStationIndex::BO ) {
        ATH_MSG_VERBOSE( " check BI BO result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingbibo_sphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingbibo_lphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
      }
      
      // Barrel middle to outer station 
      else if ( station_a == MuonStationIndex::BM && station_b == MuonStationIndex::BO ) {
        ATH_MSG_VERBOSE( " check BM BO result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingbmbo_sphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingbmbo_lphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
      }
      // Endcap inner to middle station 
      else if ( station_a == MuonStationIndex::EI &&
		(station_b == MuonStationIndex::EM ) ) {
        ATH_MSG_VERBOSE( " check EI EM result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingeiem_sphisec ) {
            ATH_MSG_VERBOSE( " reject EI EM S result ");
	    return false;
	  } else {
	    return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingeiem_lphisec ) {
            ATH_MSG_VERBOSE( " reject EI EM L result ");
	    return false;
	  } else {
	    return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	  }
	}
      }
     // Endcap inner to outer station 
      else if ( station_a == MuonStationIndex::EI &&
		(station_b == MuonStationIndex::EO) ) {
        ATH_MSG_VERBOSE( " check EI EO result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingeieo_sphisec ) {
            ATH_MSG_VERBOSE( " reject EI EO S result ");
	    return false;
	  } else {
	    return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingeieo_lphisec ) {
            ATH_MSG_VERBOSE( " reject EI EO L result ");
	    return false;
	  } else {
	    return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	  }
	}
      }
      // Endcap middle to outer station 
      else if ( station_a == MuonStationIndex::EM && station_b == MuonStationIndex::EO ) {
	// 5 mrad
        ATH_MSG_VERBOSE( " check EM EO result ");
	if ( result.phiSector_a%2==0) {
	  if ( result.deltaTheta > 6.67*m_matchingemeo_sphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
	else if ( result.phiSector_a%2==1) {
	  if ( result.deltaTheta > 6.67*m_matchingemeo_lphisec ) {
	    return false;
	  } else {
	    return true;
	  }
	}
      }
      
      return true;
    }
    
    ATH_MSG_VERBOSE( " check further matching result ");

    // Second: tight selection if only if requested
    if( m_onlySameSectorIfTight && result.phiSector_a != result.phiSector_b ){
      ATH_MSG_VERBOSE(" rejection pair as in different sector and using tight cuts");
      return false;
    }
    if ( isCSC_a || isCSC_b ) {
      if ( result.phiSector_a != result.phiSector_b ) {
          return false;
      } else {
        if ( result.deltaTheta > 0.100 ) {
          return false;
        } else {
	  if( (isCSC_a && !isEndcap_b) || (isCSC_b && !isEndcap_a) ) return false; 
	  return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
        }
      }

      
    }
    // BEE
    else if ( isBEE_a || isBEE_b ) {
      if ( result.deltaTheta > 0.200 ) {
	return false;
      } else {
	return true;
      }
    }
    // Looser cut for segment in two different phi sectors
    else if ( result.phiSector_a != result.phiSector_b ) {
      if ( result.deltaTheta > 0.150 ) {
	return false;
      } 
      return true;
    }
    // Barrel/endcap overlap  
    else if ( isEndcap_a != isEndcap_b ) {
      if ( result.deltaTheta > 0.150 ) {
	return false;
      } else {
	return true;
      }
    }
    // Phi-sector overlap  
    else if ( result.phiSector_a != result.phiSector_b ) {
      unsigned nChambers_a = m_edmHelperSvc->chamberIds(seg1).size();
      unsigned nChambers_b = m_edmHelperSvc->chamberIds(seg2).size();
      if ( nChambers_a < 2 && nChambers_b < 2 ) {
	return false;
      }
      else if ( result.deltaTheta > 0.150 ) {
	return false;
      } else {
	return true;
      }
    }
    // Barrel inner to middle station 
    else if ( station_a == MuonStationIndex::BI &&
	      station_b == MuonStationIndex::BM ) {
      if ( result.phiSector_a%2==0 ){
	if ( result.deltaTheta > m_matchingbibm_sphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
      else if ( result.phiSector_a%2==1 ){
	if ( result.deltaTheta > m_matchingbibm_lphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
    }
    // Barrel inner to outer station 
    else if ( station_a == MuonStationIndex::BI &&
	      station_b == MuonStationIndex::BO ) {
      if ( result.phiSector_a%2==0 ){
	if ( result.deltaTheta > m_matchingbibo_sphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
      else if ( result.phiSector_a%2==1 ){
	if ( result.deltaTheta > m_matchingbibo_lphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
    }
    // Barrel middle to outer station 
    else if ( station_a == MuonStationIndex::BM && station_b == MuonStationIndex::BO ) {
      if( result.phiSector_a%2==0){
        if ( result.deltaTheta > m_matchingbmbo_sphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
      else if( result.phiSector_a%2==1){
        if ( result.deltaTheta > m_matchingbmbo_lphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
    }
    // Endcap inner to middle station 
    else if ( (station_a == MuonStationIndex::EI||station_a == MuonStationIndex::BI) && station_b == MuonStationIndex::EM ) {
      if( result.phiSector_a%2==0){
	if ( result.deltaTheta > m_matchingeiem_sphisec ) {
	  return false;
	} else {
	  return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	}
      }
      else if( result.phiSector_a%2==1){
	if ( result.deltaTheta > m_matchingeiem_lphisec ) {
	  return false;
	} else {
	  return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	}
      }
    }
    // Endcap inner to outer station 
    else if ( station_a == MuonStationIndex::EI &&
	      (station_b == MuonStationIndex::EO) ) {
      if( result.phiSector_a%2==0){
	if ( result.deltaTheta > m_matchingeieo_sphisec ) {
	  return false;
	} else {
	  return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	}
      }
      else if( result.phiSector_a%2==1){
	if ( result.deltaTheta > m_matchingeieo_lphisec ) {
	  return false;
	} else {
	  return endcapExtrapolationMatch(seg1,seg2,useTightCuts);
	}
      }
    }
    // Endcap middle to outer station 
    else if ( station_a == MuonStationIndex::EM && station_b == MuonStationIndex::EO ) {
      if( result.phiSector_a%2==0){
	if ( result.deltaTheta > m_matchingemeo_sphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
      else if( result.phiSector_a%2==1){
	if ( result.deltaTheta > m_matchingemeo_lphisec ) {
	  return false;
	} else {
	  return true;
	}
      }
    }
    
    return true;
  } // if m_useLocalAngles

  bool MuonSegmentMatchingTool::suppressNoisePhi( const MuonSegment& seg1, const MuonSegment& seg2, const bool& useTightCuts ) const {
    // calculate matching variables
    IMuonSegmentPairMatchingTool::SegmentMatchResult result = m_pairMatchingTool->matchResult(seg1,seg2);
    
    MuonStationIndex::StIndex station_a = m_idHelperSvc->stationIndex( result.chid_a );
    MuonStationIndex::StIndex station_b = m_idHelperSvc->stationIndex( result.chid_b );
       
    bool isEndcap_a   = m_idHelperSvc->isEndcap( result.chid_a );
       
    bool isEndcap_b   = m_idHelperSvc->isEndcap( result.chid_b );
       
    if ( m_dumpAngles ) {
      std::cout << "SegmentPositionChange Phi"
		<< " " << m_idHelperSvc->chamberNameString(result.chid_a)
		<< " " << m_idHelperSvc->chamberNameString(result.chid_b)
		<< " deltaPhipos " << result.deltaPhipos
		<< " deltaPhidir " << result.deltaPhidir
		<< " phiposerr_a " << result.phiposerr_a
		<< " phiposerr_b " << result.phiposerr_b
		<< " phidirerr_a " << result.phidirerr_a
		<< " phidirerr_b " << result.phidirerr_b
		<< " shorttube_a " << result.shorttube_a
		<< " shorttube_b " << result.shorttube_b
		<< std::endl;
//      return true; // to get the maximum statistics and not be hindered by current cuts
    }    
  
    //Keep segments only if they are in the same or adjacent phi sectors
    if (result.phiSector_a != result.phiSector_b && ((result.phiSector_a!=16 && result.phiSector_b!=1) && (result.phiSector_a!=1 && result.phiSector_b!=16)) && (result.phiSector_a != (result.phiSector_b+1) && result.phiSector_a != (result.phiSector_b-1))) return false;      
    
    // First: loose selection
    if ( !useTightCuts ) {
      //cuts on second coordinate
      if(result.phiSector_a==result.phiSector_b){
	if(result.phiposerr_a < 10001.000 && result.phiposerr_b < 10001.000){

	  if( !isEndcap_a && !isEndcap_b){
	    if( result.deltaPhipos > 0.1 ){
	      return false;
	    }
	    else {
	      return true;
	    }
	  }
	  if( isEndcap_a && isEndcap_b){
	    //small sectors
	    if( result.phiSector_a%2==0 ) {
	      if( result.deltaPhipos > 0.1 ){
		return false;
	      }
	      else {
		return true;
	      }	      
	    }
	    //large sectors
	    if (result.phiSector_a%2==1) {
	      if( result.deltaPhipos > 0.2 ){
		return false;
	      }
	      else {
		return true;
	      }	           
	    }
	  }
	}
// 	if(result.phidirerr_a < 10001.000 && result.phidirerr_b < 10001.000){
// 	  if( !isEndcap_a && !isEndcap_b){
// 	    if( result.deltaPhidir > 0.2 ){
// 	      return false;
// 	    }
// 	    else {
// 	      return true;
// 	    }
// 	  }
// 	  if( isEndcap_a && isEndcap_b){
// 	    return true;
// 	  }
// 	}
      }

      if(result.phiSector_a!=result.phiSector_b){
	if(result.phiposerr_a < 10001.000 && result.phiposerr_b < 10001.000){
	  if( !isEndcap_a && !isEndcap_b){
	    if( result.deltaPhipos > 0.1 ){
	      return false;
	    }
	    else {
	      return true;
	    }
	  }
	  if( isEndcap_a && isEndcap_b){
	    if( result.deltaPhipos > 0.2 ){
	      return false;
	    }
	    else {
	      return true;
	    }
	  }
	}
// 	if(result.phidirerr_a < 10001.000 && result.phidirerr_b < 10001.000){
// 	  if( !isEndcap_a && !isEndcap_b){
// 	    if( result.deltaPhidir > 0.2 ){ 
// 	      return false;
// 	    }
// 	    else {
// 	      return true;
// 	    }
// 	  }
// 	  if( isEndcap_a && isEndcap_b){
// 	    return true;
// 	  }
// 	}
      }
       
      //cuts on distance to tube edge
      //only if in a different phi sector
      if(result.phiSector_a!=result.phiSector_b){
	//measured inner segment
	if(result.phiposerr_a < 10001.000){	 
	  if ( station_a == MuonStationIndex::BM &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_a > 800){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EM ) {
// MM or STGC have result.shorttube = 99999.
	    if(result.shorttube_a > 3500 && result.shorttube_a != 99999.){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EO ) {
	    if(result.shorttube_a > 3500 && result.shorttube_a != 99999.){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EM &&
	       station_b == MuonStationIndex::EO ) {
	    if(result.shorttube_a > 800){
	      return false;
	    } else {
	      return true;
	    }
	  }
	}
	//measured outer segment
	if(result.phiposerr_b < 10001.000){	 
	  if ( station_a == MuonStationIndex::BI &&
	       station_b == MuonStationIndex::BM ) {
	    if(result.shorttube_b > 800){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::BI &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_b > 800){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::BM &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_b > 800){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EM ) {
	    if(result.shorttube_b > 1400){
	      return false;
	    } else {
	      return true;
	    }
	  }
	}
      } 
      return true;
    }
    
    
    // Second: tight selection if only if requested
    //cuts on second coordinate
    if(result.phiSector_a==result.phiSector_b){
      if(result.phiposerr_a < 10001.000 && result.phiposerr_b < 10001.000){
	if( !isEndcap_a && !isEndcap_b){
	  if( result.deltaPhipos > 0.1 ){
	    return false;
	  }
	  else {
	    return true;
	  }
	}
	if( isEndcap_a && isEndcap_b){
	  //small sectors
	  if( result.phiSector_a%2==0 ) {
	    if( result.deltaPhipos > 0.08 ){
	      return false;
	    }
	    else {
	      return true;
	    }	      
	  }
	  //large sectors
	  if (result.phiSector_a%2==1) {
	    if( result.deltaPhipos > 0.1 ){
	      return false;
	    }
	    else {
	      return true;
	    }	           
	  }
	}
      }
//       if(result.phidirerr_a < 10001.000 && result.phidirerr_b < 10001.000){
// 	if( !isEndcap_a && !isEndcap_b){
// 	  if( result.deltaPhidir > 0.1 ){
// 	    return false;
// 	  }
// 	  else {
// 	    return true;
// 	  }
// 	}
// 	if( isEndcap_a && isEndcap_b){
// 	  return true;
// 	}
//       }
    }

    if(result.phiSector_a!=result.phiSector_b){
      if(result.phiposerr_a < 10001.000 && result.phiposerr_b < 10001.000){
	if( !isEndcap_a && !isEndcap_b){
	  if( result.deltaPhipos > 0.05 ){
	    return false;
	  }
	  else {
	    return true;
	  }
	}
	if( isEndcap_a && isEndcap_b){
	  if( result.deltaPhipos > 0.1 ){
	    return false;
	  }
	  else {
	    return true;
	  }
	}
      }
//       if(result.phidirerr_a < 10001.000 && result.phidirerr_b < 10001.000){
// 	if( !isEndcap_a && !isEndcap_b){
// 	  if( result.deltaPhidir > 0.1 ){
// 	    return false;
// 	  }
// 	  else {
// 	    return true;
// 	  }
// 	}
// 	if( isEndcap_a && isEndcap_b){
// 	  return true;
// 	}
//       }
      //cuts on distance to tube edge
      //only if in a different phi sector
      if(result.phiSector_a!=result.phiSector_b){
	//measured inner segment
	if(result.phiposerr_a < 10001.000){	 
	  if ( station_a == MuonStationIndex::BM &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_a > 600){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EM ) {
	    if(result.shorttube_a > 3500 && result.shorttube_a != 99999.){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EO ) {
	    if(result.shorttube_a > 3500 && result.shorttube_a != 99999.){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EM &&
	       station_b == MuonStationIndex::EO ) {
	    if(result.shorttube_a > 500){
	      return false;
	    } else {
	      return true;
	    }
	  }
	}
	//measured outer segment
	if(result.phiposerr_b < 10001.000){	 
	  if ( station_a == MuonStationIndex::BI &&
	       station_b == MuonStationIndex::BM ) {
	    if(result.shorttube_b > 600){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::BI &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_b > 700){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::BM &&
	       station_b == MuonStationIndex::BO ) {
	    if(result.shorttube_b > 700){
	      return false;
	    } else {
	      return true;
	    }
	  }
	  if ( station_a == MuonStationIndex::EI &&
	       station_b == MuonStationIndex::EM ) {
	    if(result.shorttube_b > 700){
	      return false;
	    } else {
	      return true;
	    }
	  }
	}
      }
    }    
    return true;
  } // if m_useLocalAngles



  bool MuonSegmentMatchingTool::endcapExtrapolationMatch( const MuonSegment& seg1, const MuonSegment& seg2, 
							  bool useTightCuts ) const {

    if( !m_useEndcapExtrapolationMatching ) return true;

    Identifier chid1 = m_edmHelperSvc->chamberId(seg1);
    Identifier chid2 = m_edmHelperSvc->chamberId(seg2);
    if( chid1 == chid2 ) return false;

    MuonStationIndex::StIndex stIndex1 = m_idHelperSvc->stationIndex(chid1);
    MuonStationIndex::StIndex stIndex2 = m_idHelperSvc->stationIndex(chid2);
    if( stIndex1 == stIndex2 ) return false;

    const MuonSegment* segInner = 0;
    if( stIndex1 == MuonStationIndex::EI ) segInner = &seg1;
    if( stIndex2 == MuonStationIndex::EI ) segInner = &seg2;

    const MuonSegment* segOuter = 0;
    if( stIndex1 == MuonStationIndex::EM || stIndex1 == MuonStationIndex::EO  ) segOuter = &seg1;
    if( stIndex2 == MuonStationIndex::EM || stIndex2 == MuonStationIndex::EO  ) segOuter = &seg2;

    if( !segInner || !segOuter ) {
      return false;
    }
    const Amg::Vector3D& pos1 = segOuter->globalPosition();
    const Amg::Vector3D& dir1 = segOuter->globalDirection();
    const Amg::Vector3D& pos2 = segInner->globalPosition();
    const Amg::Vector3D& dir2 = segInner->globalDirection();

    // extrapolate the first segment to the inner layer
    double r_expected = 0;
    double theta_expected = 0;
    double rhoInv = 0;
    simpleEndcapExtrapolate( pos1.x(), pos1.y(), pos1.z(), dir1.theta(), pos2.z(),
			     r_expected, theta_expected, rhoInv );
  
    if( rhoInv < 0 ) rhoInv *= -1.;
    double dr = pos2.perp() - r_expected;
    double dtheta = dir2.theta() - theta_expected;

    double drCut = m_drExtrapRMS + 5e6*rhoInv;
    if( useTightCuts ) drCut *= 2;
    else               drCut *= 4;

    if( (stIndex1 == MuonStationIndex::EM && stIndex2 == MuonStationIndex::BI) || (stIndex1 == MuonStationIndex::BI && stIndex2 == MuonStationIndex::EM) ){
      drCut += 3*m_drExtrapAlignmentOffset;
    }else{
      drCut += m_drExtrapAlignmentOffset;
    }

    double dthetaCut = m_dthetaExtrapRMS + 1.5e3*rhoInv;
    if( useTightCuts ) dthetaCut *= 2;
    else               dthetaCut *= 4;

    ATH_MSG_VERBOSE(" simple match " << m_printer->print(seg1) << std::endl
		    << "              " << m_printer->print(seg2) << std::endl
		    << " dr " << dr << " cut " << drCut << " dtheta " << dtheta << " cut " << dthetaCut << " rhoInv " << 1e6*rhoInv);

    if( fabs(dr) > drCut )         return false;
    if( fabs(dtheta) > dthetaCut ) return false;
    return true;
  }


  void MuonSegmentMatchingTool::simpleEndcapExtrapolate(double x_segment, double y_segment, double z_segment, double theta_segment, 
							double z_extrapolated,  double& r_expected, 
							double& theta_expected, double& rhoInv ) const {
    //
    // Endcap extrapolation with a parabolic track model
    //
    //     In the region z = z start - z end of the Toroid one has:
    //        r_expected  = z tan(theta) + (z  - z start)*(z  - z start)/ rho
    //
    //        outside this region No field
    //  
    //     here z start = 6 000 z end = 10 000
    //     theta = direction at the vertex rho is the effective curvature
    //
    double z_start =  7000.;
    double z_end   = 12000.;

    if(z_extrapolated<0) z_start = - z_start;
    if(z_extrapolated<0)   z_end = - z_end;

    double r_segment = sqrt(x_segment*x_segment+y_segment*y_segment);

    if(fabs(z_extrapolated) >  fabs(z_segment)) {
      ATH_MSG_WARNING(" extrapolate outwards is not implemented for z " << z_extrapolated);
      r_expected = 0;
      theta_expected = 0.;
      return;
    }
    if(fabs(z_segment) < fabs(z_end) )  {
      ATH_MSG_WARNING(" segment before end of Toroid: SL extrapolation is used implemented " << z_segment);
    }

    // use SL extrapolation to go to the z_end of the Toroid

    double r_end = r_segment + (z_end-z_segment)*tan(theta_segment);

    //  std::cout << "z end " << z_end << " r_end " << r_end << std::endl; 
    //  std::cout << "r_segment " << r_segment  << " z _segment " << z_segment << std::endl; 

    double zgeo = (z_end-z_start)*(z_end-z_start) -2*z_end*(z_end-z_start);
    rhoInv = (r_end - z_end*tan(theta_segment)) / zgeo;
    double tantheta = tan(theta_segment) - 2*(z_end - z_start)*rhoInv;
    r_expected = z_extrapolated*tantheta + (z_extrapolated-z_start)*(z_extrapolated-z_start)*rhoInv;
    //
    // increase radius by 30% wrt straight line 
    //
    double r_SL = r_segment + (z_extrapolated-z_segment)*tan(theta_segment);
    r_expected = r_expected + 0.3*(r_expected-r_SL);
    theta_expected = atan(tantheta+2*(z_extrapolated-z_start)*rhoInv);

    if(tan(theta_segment)<0&&theta_expected<0) theta_expected += 2*acos(0.);
    if(tan(theta_segment)>0&&theta_expected<0) theta_expected = - theta_expected;
  }



}

