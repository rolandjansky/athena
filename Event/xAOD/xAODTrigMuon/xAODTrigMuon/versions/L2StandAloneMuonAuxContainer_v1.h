// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuonAuxContainer_v1.h 613775 2014-08-28 07:56:49Z mishitsu $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V1_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V1_H

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
   /// $Date: 2014-08-28 09:56:49 +0200 (Thu, 28 Aug 2014) $
   ///
   class L2StandAloneMuonAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      L2StandAloneMuonAuxContainer_v1();

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
      /// Slope and intercept at inner
      std::vector< float >    slopeInner;
      std::vector< float >    interceptInner;
      std::vector< float >    deltaR;

      /// eta, phi as input to LUT
      std::vector< float >    etaMap;
      std::vector< float >    phiMap;
      std::vector< int >      etaBin;
      std::vector< int >      phiBin;
      /// Flag for RGC and RPC availabilities
      std::vector< int >      isTgcFailure;
      std::vector< int >      isRpcFailure;
      /// Error of eta and phi
      std::vector< float >    deltaEta;
      std::vector< float >    deltaPhi;

      /// Station fit results (superpoints)
      std::vector< std::vector< float > > superPointR;
      std::vector< std::vector< float > > superPointZ;
      std::vector< std::vector< float > > superPointSlope;
      std::vector< std::vector< float > > superPointIntercept;
      std::vector< std::vector< float > > superPointChi2;

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
      std::vector< uint32_t > roiEta;
      std::vector< uint32_t > roiPhi;

      /// pT from different algorithms
      std::vector< float >  ptBarelRadius;
      std::vector< float >  ptBarelSagitta;
      std::vector< float >  ptEndcapAlpha;
      std::vector< float >  ptEndcapBeta;
      std::vector< float >  ptEndcapRadius;

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
      std::vector< float >  tgcPt;

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
      std::vector< float >  rpc1x;
      std::vector< float >  rpc1y;
      std::vector< float >  rpc1z;

      std::vector< float >  rpc2x;
      std::vector< float >  rpc2y;
      std::vector< float >  rpc2z;

      std::vector< float >  rpc3x;
      std::vector< float >  rpc3y;
      std::vector< float >  rpc3z;

      /// Size of hit strages
      std::vector< int >      extensionCapacity;
      std::vector< int >      robCapacity;
      std::vector< int >      csmCapacity;
      std::vector< int >      lvl1EmuCapacity;
      std::vector< int >      rpcHitsCapacity;
      std::vector< int >      tgcHitsCapacity;
      std::vector< int >      mdtHitsCapacity;

      /// Extensions
      std::vector< std::vector< uint32_t > > extension0;
      std::vector< std::vector< uint32_t > > extension1;
      std::vector< std::vector< uint32_t > > extension2;
      std::vector< std::vector< uint32_t > > extension3;
      std::vector< std::vector< uint32_t > > extension4;
      std::vector< std::vector< uint32_t > > extension5;
      std::vector< std::vector< uint32_t > > extension6;
      std::vector< std::vector< uint32_t > > extension7;
      std::vector< std::vector< uint32_t > > extension8;
      std::vector< std::vector< uint32_t > > extension9;

      /// Level 1 emulation info
      std::vector< std::vector< uint32_t > > lvl1Emulation;    

      /// ROB info
      std::vector< std::vector< uint32_t > > robId;
      std::vector< std::vector< uint32_t > > removedRobId;

      /// CSM info
      std::vector< std::vector< uint32_t > > csmId;
      std::vector< std::vector< uint32_t > > csmSize;
      std::vector< std::vector< uint32_t > > csmError;
      std::vector< std::vector< uint32_t > > removedCsmId;

      /// RPC pad hits
      std::vector< std::vector< uint32_t > > padHitOnlineId;
      std::vector< std::vector< uint32_t > > padHitCode;
      std::vector< std::vector< float > > padHitX;
      std::vector< std::vector< float > > padHitY;
      std::vector< std::vector< float > > padHitZ;
      std::vector< std::vector< float > > padHitR;
      std::vector< std::vector< float > > padHitP;

      /// TGC hits
      std::vector< std::vector< float > >    tgcInnRhoHitPhi;
      std::vector< std::vector< float > >    tgcInnRhoHitR;
      std::vector< std::vector< float > >    tgcInnRhoHitZ;
      std::vector< std::vector< float > >    tgcInnRhoHitWidth;
      std::vector< std::vector< uint32_t > > tgcInnRhoHitInSeg;
      std::vector< std::vector< float > >    tgcInnPhiHitPhi;
      std::vector< std::vector< float > >    tgcInnPhiHitR;
      std::vector< std::vector< float > >    tgcInnPhiHitZ;
      std::vector< std::vector< float > >    tgcInnPhiHitWidth;
      std::vector< std::vector< uint32_t > > tgcInnPhiHitInSeg;
      std::vector< std::vector< float > >    tgcMidRhoHitPhi;
      std::vector< std::vector< float > >    tgcMidRhoHitR;
      std::vector< std::vector< float > >    tgcMidRhoHitZ;
      std::vector< std::vector< float > >    tgcMidRhoHitWidth;
      std::vector< std::vector< uint32_t > > tgcMidRhoHitInSeg;
      std::vector< std::vector< float > >    tgcMidPhiHitPhi;
      std::vector< std::vector< float > >    tgcMidPhiHitR;
      std::vector< std::vector< float > >    tgcMidPhiHitZ;
      std::vector< std::vector< float > >    tgcMidPhiHitWidth;
      std::vector< std::vector< uint32_t > > tgcMidPhiHitInEeg;

      /// MDT hits
      std::vector< std::vector< uint32_t > > mdtHitOnlineId;
      std::vector< std::vector< uint32_t > > mdtHitOfflineId;
      std::vector< std::vector< uint32_t > > mdtHitChamber;
      std::vector< std::vector< float > > mdtHitR;
      std::vector< std::vector< float > > mdtHitZ;
      std::vector< std::vector< float > > mdtHitResidual;
      std::vector< std::vector< float > > mdtHitTime;
      std::vector< std::vector< float > > mdtHitSpace;
      std::vector< std::vector< float > > mdtHitSigma;

      /// @}

   }; // class L2StandAloneMuonAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::L2StandAloneMuonAuxContainer_v1, 1207804085, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::L2StandAloneMuonAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUONAUXCONTAINER_V1_H
