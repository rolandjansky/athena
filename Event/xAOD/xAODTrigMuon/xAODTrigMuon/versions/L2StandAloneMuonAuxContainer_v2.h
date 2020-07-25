// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonAuxContainer_v2.h 661304 2015-04-18 04:31:23Z mishitsu $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V2_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V2_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for muon SA containers
   ///
   /// This is a fixed implementation of the muon SA auxiliary store.
   ///  Defining every possible property that muon SA can have.
   ///
   /// @author Masaki Ishitsuka <ishitsuka@phys.titech.ac.jp>
   ///
   /// $Revsision$
   /// $Date: 2015-04-18 06:31:23 +0200 (Sat, 18 Apr 2015) $
   ///
   class L2StandAloneMuonAuxContainer_v2 : public AuxContainerBase {

   public:
      /// Default constructor
      L2StandAloneMuonAuxContainer_v2();

   private:
      /// @name 4-momentum variables
      /// @{
      std::vector< float > pt;
      std::vector< float > phi;
      std::vector< float > eta;
      /// @}

      /// @name L2 muon specific variables
      /// @{
      /// Outputs of L2 muon standalone algorithm
      std::vector< uint32_t > roiWord;
      std::vector< int >      sAddress;
      /// Position at MS
      std::vector< float >    rMS;
      std::vector< float >    etaMS;
      std::vector< float >    phiMS;
      std::vector< float >    dirPhiMS;
      std::vector< float >    zMS;
      std::vector< float >    dirZMS;
      std::vector< float >    beta;
      /// Input measurements to be converted to pT by LUT
      std::vector< float >    barrelRadius;
      std::vector< float >    barrelSagitta;
      std::vector< float >    endcapAlpha;
      std::vector< float >    endcapBeta;
      std::vector< float >    endcapRadius;

      /// eta, phi as input to LUT
      std::vector< float >    etaMap;
      std::vector< float >    phiMap;
      std::vector< int >      etaBin;
      std::vector< int >      phiBin;
      /// Flag for RGC and RPC availabilities
      std::vector< int >      isTgcFailure;
      std::vector< int >      isRpcFailure;
      /// Error of pt, eta and phi
      std::vector< float >    deltaPt;
      std::vector< float >    deltaPtParm1;
      std::vector< float >    deltaPtParm2;
      std::vector< float >    deltaPtParm3;
      std::vector< float >    deltaEta;
      std::vector< float >    deltaPhi;

      /// Station fit results (superpoints)
      std::vector< std::vector< float > > superPointR;
      std::vector< std::vector< float > > superPointZ;
      std::vector< std::vector< float > > superPointSlope;
      std::vector< std::vector< float > > superPointIntercept;
      std::vector< std::vector< float > > superPointChi2;

      /// Track positions
      std::vector< std::vector< float > > trackPositionR;
      std::vector< std::vector< float > > trackPositionZ;
      std::vector< std::vector< float > > trackPositionEta;
      std::vector< std::vector< float > > trackPositionPhi;

      /// Detailed information
      std::vector< int >      algoId;
      std::vector< uint32_t > teId;
      std::vector< uint32_t > lvl1Id;
      std::vector< uint32_t > lumiBlock;
      std::vector< uint32_t > muonDetMask;
      std::vector< uint32_t > roiId;
      std::vector< uint32_t > roiSystem;
      std::vector< uint32_t > roiSubsystem;
      std::vector< uint32_t > roiSector;
      std::vector< uint32_t > roiNumber;
      std::vector< uint32_t > roiThreshold;
      std::vector< float >    roiEta;
      std::vector< float >    roiPhi;

      /// pT from different algorithms
      std::vector< float >  tgcPt;
      std::vector< float >  ptBarrelRadius;
      std::vector< float >  ptBarrelSagitta;
      std::vector< float >  ptEndcapAlpha;
      std::vector< float >  ptEndcapBeta;
      std::vector< float >  ptEndcapRadius;
      std::vector< float >  ptCSC;

      /// Road information
      /// Chamber type
      std::vector< std::vector< int > > chamberType1Normal;
      std::vector< std::vector< int > > chamberType1Overlap;
      std::vector< std::vector< int > > chamberType2Normal;
      std::vector< std::vector< int > > chamberType2Overlap;
      /// Slope
      std::vector< std::vector< float > > roadAwNormal;
      std::vector< std::vector< float > > roadAwOverlap;
      /// Intercept
      std::vector< std::vector< float > > roadBwNormal;
      std::vector< std::vector< float > > roadBwOverlap;
      /// Z
      std::vector< std::vector< float > > zMinNormal;
      std::vector< std::vector< float > > zMinOverlap;
      std::vector< std::vector< float > > zMaxNormal;
      std::vector< std::vector< float > > zMaxOverlap;
      /// R
      std::vector< std::vector< float > > rMinNormal;
      std::vector< std::vector< float > > rMinOverlap;
      std::vector< std::vector< float > > rMaxNormal;
      std::vector< std::vector< float > > rMaxOverlap;
      /// Eta
      std::vector< std::vector< float > > etaMinNormal;
      std::vector< std::vector< float > > etaMinOverlap;
      std::vector< std::vector< float > > etaMaxNormal;
      std::vector< std::vector< float > > etaMaxOverlap;

      /// TGC fit results
      std::vector< float >  tgcInnEta;
      std::vector< float >  tgcInnPhi;
      std::vector< float >  tgcInnR;
      std::vector< float >  tgcInnZ;

      std::vector< float >  tgcMid1Eta;
      std::vector< float >  tgcMid1Phi;
      std::vector< float >  tgcMid1R;
      std::vector< float >  tgcMid1Z;

      std::vector< float >  tgcMid2Eta;
      std::vector< float >  tgcMid2Phi;
      std::vector< float >  tgcMid2R;
      std::vector< float >  tgcMid2Z;

      std::vector< float >  tgcInnRhoStd;
      std::vector< long >   tgcInnRhoN;
      std::vector< float >  tgcInnPhiStd;
      std::vector< long >   tgcInnPhiN;

      std::vector< float >  tgcMidRhoChi2;
      std::vector< long >   tgcMidRhoN;
      std::vector< float >  tgcMidPhiChi2;
      std::vector< long >   tgcMidPhiN;

      /// RPC fit results
      std::vector< float >  rpcFitInnPhi;
      std::vector< float >  rpcFitInnSlope;
      std::vector< float >  rpcFitInnOffset;

      std::vector< float >  rpcFitMidPhi;
      std::vector< float >  rpcFitMidSlope;
      std::vector< float >  rpcFitMidOffset;

      std::vector< float >  rpcFitOutPhi;
      std::vector< float >  rpcFitOutSlope;
      std::vector< float >  rpcFitOutOffset;

      /// Size of hit strages
      std::vector< int >      rpcHitsCapacity;
      std::vector< int >      tgcHitsCapacity;
      std::vector< int >      mdtHitsCapacity;
      std::vector< int >      cscHitsCapacity;

      /// RPC pad hits
      std::vector< std::vector< uint32_t > > rpcHitLayer;
      std::vector< std::vector< uint32_t > > rpcHitMeasuresPhi;
      std::vector< std::vector< float > >    rpcHitX;
      std::vector< std::vector< float > >    rpcHitY;
      std::vector< std::vector< float > >    rpcHitZ;
      std::vector< std::vector< float > >    rpcHitTime;
      std::vector< std::vector< float > >    rpcHitDistToEtaReadout;
      std::vector< std::vector< float > >    rpcHitDistToPhiReadout;
      std::vector< std::vector< std::string > > rpcHitStationName;

      /// TGC hits
      std::vector< std::vector< float > >    tgcHitEta;
      std::vector< std::vector< float > >    tgcHitPhi;
      std::vector< std::vector< float > >    tgcHitR;
      std::vector< std::vector< float > >    tgcHitZ;
      std::vector< std::vector< float > >    tgcHitWidth;
      std::vector< std::vector< int > >      tgcHitStationNum;
      std::vector< std::vector< bool > >     tgcHitIsStrip;
      std::vector< std::vector< int > >      tgcHitBCTag;
      std::vector< std::vector< bool > >     tgcHitInRoad;

      /// MDT hits
      std::vector< std::vector< uint32_t > > mdtHitOnlineId;
      std::vector< std::vector< int > >      mdtHitIsOutlier;
      std::vector< std::vector< int > >      mdtHitChamberId;
      std::vector< std::vector< float > >    mdtHitR;
      std::vector< std::vector< float > >    mdtHitZ;
      std::vector< std::vector< float > >    mdtHitPhi;
      std::vector< std::vector< float > >    mdtHitResidual;
      std::vector< std::vector< float > >    mdtHitTime;
      std::vector< std::vector< float > >    mdtHitSpace;
      std::vector< std::vector< float > >    mdtHitSigma;

      /// CSC hits
      std::vector< std::vector< int > >      cscHitIsOutlier;
      std::vector< std::vector< int > >      cscHitChamberId;
      std::vector< std::vector< uint32_t > > cscHitStationName;
      std::vector< std::vector< int > >      cscHitStationEta;
      std::vector< std::vector< int > >      cscHitStationPhi;
      std::vector< std::vector< int > >      cscHitChamberLayer;
      std::vector< std::vector< int > >      cscHitWireLayer;
      std::vector< std::vector< int > >      cscHitMeasuresPhi;
      std::vector< std::vector< int > >      cscHitStrip;
      std::vector< std::vector< float > >    cscHitEta;
      std::vector< std::vector< float > >    cscHitPhi;
      std::vector< std::vector< float > >    cscHitR;
      std::vector< std::vector< float > >    cscHitZ;
      std::vector< std::vector< int > >      cscHitCharge;
      std::vector< std::vector< float > >    cscHitTime;
      std::vector< std::vector< float > >    cscHitResidual;

      // sTGC clusters
      std::vector< std::vector< unsigned int > > stgcClusterLayer;
      std::vector< std::vector< int > >          stgcClusterIsOutlier;
      std::vector< std::vector< int > >          stgcClusterType;
      std::vector< std::vector< float > >        stgcClusterEta;
      std::vector< std::vector< float > >        stgcClusterPhi;
      std::vector< std::vector< float > >        stgcClusterR;
      std::vector< std::vector< float > >        stgcClusterZ;
      std::vector< std::vector< float > >        stgcClusterResidualR;
      std::vector< std::vector< float > >        stgcClusterResidualPhi;
      std::vector< std::vector< int > >          stgcClusterStationEta;
      std::vector< std::vector< int > >          stgcClusterStationPhi;
      std::vector< std::vector< int > >          stgcClusterStationName;

      // MM clusters
      std::vector< std::vector< unsigned int > > mmClusterLayer;
      std::vector< std::vector< int > >          mmClusterIsOutlier;
      std::vector< std::vector< float > >        mmClusterEta;
      std::vector< std::vector< float > >        mmClusterPhi;
      std::vector< std::vector< float > >        mmClusterR;
      std::vector< std::vector< float > >        mmClusterZ;
      std::vector< std::vector< float > >        mmClusterResidualR;
      std::vector< std::vector< float > >        mmClusterResidualPhi;
      std::vector< std::vector< int > >          mmClusterStationEta;
      std::vector< std::vector< int > >          mmClusterStationPhi;
      std::vector< std::vector< int > >          mmClusterStationName;

      /// @}

   }; // class L2StandAloneMuonAuxContainer_v2

} // namespace xAOD

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::L2StandAloneMuonAuxContainer_v2, xAOD::AuxContainerBase );

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V2_H
