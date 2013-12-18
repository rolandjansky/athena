/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBTRACKSUMMARY_H
#define MUONCALIBTRACKSUMMARY_H

#include "MuonCalibIdentifier/MuonFixedId.h"
#include "MuonCalibExtraUtils/MuonFixedIdManipulator.h"
#include <string>
#include <vector>
#include <set>

namespace MuonCalib {

  struct MuonCalibTrackSummary {

    struct ChamberHitSummary {
      struct Projection {
	Projection() : nhits(0),nholes(0),noutliers(0),ndeltas(0) {}
	int nhits;
	int nholes;
	int noutliers;
	int ndeltas;
      };

      ChamberHitSummary( MuonFixedId chID ) : chId(chID) {}


      MuonFixedId chId;
      bool isMdt() const { return chId.is_mdt(); }

      int nhits() const     { return first.nhits + second.nhits; }
      int nholes() const    { return first.nholes + second.nholes; }
      int noutliers() const { return first.noutliers + second.noutliers; }
      int ndeltas() const   { return first.ndeltas + second.ndeltas; }

      int netaHits() const { return isMdt() ? nhits() : first.nhits; }
      int nphiHits() const { return isMdt() ? 0       : second.nhits; }

      int nMdtHitsMl1() const { return isMdt() ? first.nhits  : 0; }
      int nMdtHitsMl2() const { return isMdt() ? second.nhits : 0; }

      // before using make sure isMdt == true!
      Projection& mdtMl1() const { return first; }
      Projection& mdtMl2() const { return second; }

      // before using make sure isMdt == false!
      Projection& etaProjection() const { return first; }
      Projection& phiProjection() const { return second; }
      

      private:
      
      mutable Projection first;  //<! eta projections for cluster chambers, first multi layer for mdt chambers
      mutable Projection second; //<! phi projections for cluster chambers, first multi layer for mdt chambers

    };

    MuonCalibTrackSummary() : nhits(0),npixel(0),nsct(0),ntrtBarrel(0),ntrtEndcap(0),
			      nhitsUpperHemisphere(0),nhitsLowerHemisphere(0),nscatters(0),nholes(0),noutliers(0),
			      npseudo(0),ntubeHits(0),nmdtHits(0),nmdtHitsBelowADCCut(0),
			      nrpcPhiHits(0),nrpcEtaHits(0),
			      ntgcPhiHits(0),ntgcEtaHits(0),
			      ncscPhiHits(0),ncscEtaHits(0),
			      hasEndcapA(false),hasEndcapC(false),hasBarrel(false),
			      hasEndcapLayersWithTGC(false),hasBarrelLayersWithRPC(false),
			      isTrack(false) {}


    unsigned int nhits;
    unsigned int npixel;
    unsigned int nsct;
    unsigned int ntrtBarrel;
    unsigned int ntrtEndcap;
    unsigned int nhitsUpperHemisphere;
    unsigned int nhitsLowerHemisphere;    
    unsigned int nscatters; 
    unsigned int nholes;
    unsigned int noutliers;
    unsigned int npseudo;
    unsigned int ntubeHits;
    unsigned int nmdtHits;
    unsigned int nmdtHitsBelowADCCut;
    unsigned int nrpcPhiHits;
    unsigned int nrpcEtaHits;
    unsigned int ntgcPhiHits;
    unsigned int ntgcEtaHits;
    unsigned int ncscPhiHits;
    unsigned int ncscEtaHits;

    bool hasEndcapA;
    bool hasEndcapC;
    bool hasBarrel;

    bool hasEndcapLayersWithTGC;
    bool hasBarrelLayersWithRPC;
    
    bool isTrack;

    /** access to number of chambers per technology on the track */
    unsigned int nmdtCh() const;
    unsigned int nrpcCh( bool phi ) const;
    unsigned int ntgcCh( bool phi ) const;
    unsigned int ncscCh( bool phi ) const;

    unsigned int nrpcEtaPhiCh() const;
    unsigned int ntgcEtaPhiCh() const;
    unsigned int ncscEtaPhiCh() const;

    unsigned int ntrt() const { return ntrtBarrel + ntrtEndcap; }

    /** access to number of eta/phi trigger hits */
    unsigned int nphiTrigHits() const { return nrpcPhiHits + ntgcPhiHits; }
    unsigned int netaTrigHits() const { return nrpcEtaHits + ntgcEtaHits; }

    /** access to number of MDT hits per ml (ml=1,2) */
    unsigned int nmdtHitsPerMl( int ml ) const;

    bool hasEndcap() const { return hasEndcapA || hasEndcapC; }
    bool hasMuon() const { return nmdtHits + nrpcEtaHits + nrpcPhiHits + ntgcEtaHits + ntgcPhiHits + ncscEtaHits + ncscPhiHits > 0; }
    bool hasId() const { return npixel + nsct + ntrt() > 0; }

    std::vector<ChamberHitSummary> chamberHitSummary;
    
    /** set of phi layers on the track*/
    std::set<MuonFixedIdManipulator::PhiStationIndex> phiStationLayers;

    /** set of precision layers on the track */
    std::set<MuonFixedIdManipulator::StationIndex>    precisionStationLayers;

    /** set of phi layers which also have eta hits on the track*/
    std::set<MuonFixedIdManipulator::PhiStationIndex> phiEtaStationLayers;

    /** set of precision layers with (MDT: two multi layers, CSC: at least 2 eta and phi hits) on the track */ 
    std::set<MuonFixedIdManipulator::StationIndex>    goodPrecisionStationLayers; 

    /** dump counts to string */
    std::string dump() const;
  };
}

#endif
