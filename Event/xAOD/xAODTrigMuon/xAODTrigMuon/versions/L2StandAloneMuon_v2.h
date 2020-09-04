// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuon_v2.h 661304 2015-04-18 04:31:23Z mishitsu $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V2_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V2_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <iosfwd>

// xAOD include(s):
#include "xAODBase/IParticle.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

   /// Class describing standalone muons reconstructed in the LVL2 trigger
   ///
   /// This is a simple description of the LVL2 standalone muons, which need
   /// much fewer properties than EF or offline muons.
   ///
   /// @author Masaki Ishitsuka <ishitsuka@phys.titech.ac.jp>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 661304 $
   /// $Date: 2015-04-18 06:31:23 +0200 (Sat, 18 Apr 2015) $
   ///
   class L2StandAloneMuon_v2 : public IParticle {

   public:
      /// Constructor
      L2StandAloneMuon_v2();

      /// @name Functions implementing the xAOD::IParticle interface
      /// @{

      /// The transverse momentum (\f$p_T\f$) of the particle
      virtual double           pt() const;
      /// The pseudorapidity (\f$\eta\f$) of the particle
      virtual double           eta() const;
      /// The azimuthal angle (\f$\phi\f$) of the particle
      virtual double           phi() const;
      /// The invariant mass of the particle
      virtual double           m() const;
      /// The total energy of the particle
      virtual double           e() const;
      /// The true rapidity (y) of the particle
      virtual double           rapidity() const;

      /// Definition of the 4-momentum type
      typedef TLorentzVector FourMom_t;

      /// The full 4-momentum of the particle
      virtual FourMom_t        p4() const;

      /// Base 4 Momentum type for trig muons
      typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

      ///  The full 4-momentum of the particle : internal trig muon type.
      GenVecFourMom_t genvecP4() const; 

      /// The type of the object as a simple enumeration
      virtual Type::ObjectType type() const;

      /// @}

      /// @name 4-momentum setter functions
      /// @{

      /// Set the transverse momentum (\f$p_T\f$) of the muon
      void setPt( float pt );
      /// Set the pseudorapidity (\f$\eta\f$) of the muon
      void setEta( float eta );
      /// Set the azimuthal angle (\f$\phi\f$) of the muon
      void setPhi( float phi );

      /// @}

      /// @name Properties specific to the LVL2 muon reconstruction
      /// @{

      /// Get the RoI ID of the seeding LVL1 muon
      uint32_t roiWord() const;
      /// Set the RoI ID of the seeding LVL1 muon
      void     setRoIWord( uint32_t value );

      /// Get the station address of the muon
      int      sAddress() const;
      /// Set the station address of the muon
      void     setSAddress( int value );

      /// Get the eta at muon spectrometer
      float   etaMS() const;
      /// Set the eta at muon spectrometer
      void    setEtaMS( float value );

      /// Get the phi at muon spectrometer
      float   phiMS() const;
      /// Set the phi at muon spectrometer
      void    setPhiMS( float value );

      /// Get tan phi at muon spectrometer
      float   dirPhiMS() const;
      /// Set tan phi at muon spectrometer
      void    setDirPhiMS( float value );

      /// Get the R at muon spectrometer
      float   rMS() const;
      /// Set the R at muon spectrometer
      void    setRMS( float value );

      /// Get the Z at muon spectrometer
      float   zMS() const;
      /// Set the Z at muon spectrometer
      void    setZMS( float value );

      /// Get dRdZ at muon spectrometer
      float   dirZMS() const;
      /// Set dRdZ at muon spectrometer
      void    setDirZMS( float value );

      /// Get beta
      float   beta() const;
      /// Set beta
      void    setBeta( float value );

      /// Get the fitted radius of the muon in the barrel
      float   barrelRadius() const;
      /// Set the fitted radius of the muon in the barrel
      void    setBarrelRadius( float value );

      /// Get the fitted sagitta of the muon in the barrel
      float   barrelSagitta() const;
      /// Set the fitted sagitta of the muon in the barrel
      void    setBarrelSagitta( float value );

      /// Get the fitted \f$\alpha\f$ value in the endcap
      float   endcapAlpha() const;
      /// Set the fitted \f$\alpha\f$ value in the endcap
      void    setEndcapAlpha( float value );

      /// Get the fitted \f$\beta\f$ value in the endcap
      float   endcapBeta() const;
      /// Set the fitted \f$\beta\f$ value in the endcap
      void    setEndcapBeta( float value );

      /// Get the fitted radius of the muon in the endcap
      float   endcapRadius() const;
      /// Set the fitted radius of the muon in the endcap
      void    setEndcapRadius( float value );

      /// Get eta used to refer pT LUT
      float   etaMap() const;
      /// Set eta used to refer pT LUT
      void    setEtaMap( float value );

      /// Get phi used to refer pT LUT
      float   phiMap() const;
      /// Set phi used to refer pT LUT
      void    setPhiMap( float value );

      /// Get eta bin of pT LUT
      int     etaBin() const;
      /// Set eta bin of pT LUT
      void    setEtaBin( int value );

      /// Get phi bin of pT LUT
      int     phiBin() const;
      /// Set phi bin of pT LUT
      void    setPhiBin( int value );

      /// Get flag to see if TGC is properly read
      int     isTgcFailure() const;
      /// Set flag to record if TGC is properly read
      void    setIsTgcFailure( int value );

      /// Get flag to see if RPC is properly read
      int     isRpcFailure() const;
      /// Set flag to record if RPC is properly read
      void    setIsRpcFailure( int value );

      /// Get error of pT
      float   deltaPt() const;
      /// Set error of pT
      void    setDeltaPt( float value );

      /// Get and set parameter sets for delta pT calculation
      float   deltaPtParm1() const;
      void    setDeltaPtParm1( float value );

      float   deltaPtParm2() const;
      void    setDeltaPtParm2( float value );

      float   deltaPtParm3() const;
      void    setDeltaPtParm3( float value );
      
      /// Get error of eta
      float   deltaEta() const;
      /// Set error of eta
      void    setDeltaEta( float value );

      /// Get error of phi
      float   deltaPhi() const;
      /// Set error of phi
      void    setDeltaPhi( float value );

      /// Get the measured radious of the muon in one particular super point
      float superPointR( int chamber ) const;
      /// Get the measured Z position of the muon in one particular super point
      float superPointZ( int chamber ) const;
      /// Get the measured slope of the muon in one particular super point
      float superPointSlope( int chamber ) const;
      /// Get the measured intercept of the muon in one particular super point
      float superPointIntercept( int chamber ) const;
      /// Get the chi2 of the fit in one particular super point
      float superPointChi2( int chamber ) const;
      /// Set the properties of one particular super point measurement
      void  setSuperPoint( int chamber, float r, float z, float slope, float intercept=0., float chi2=0. );

      /// Number of track positions stored
      uint32_t nTrackPositions() const;
      /// Get R postion of track
      float    trackPositionR( unsigned int n ) const;
      /// Get Z postion of track
      float    trackPositionZ( unsigned int n ) const;
      /// Get eta postion of track
      float    trackPositionEta( unsigned int n ) const;
      /// Get phi postion of track
      float    trackPositionPhi( unsigned int n ) const;
      /// Set position of muon track
      void     setTrackPosition( float r, float z, float eta, float phi );

      /// Get and set L2 muon algorithm ID (defined above)
      int      algoId() const;
      void     setAlgoId( int value);
      
      /// Get and set input TE ID
      uint32_t teId() const;
      void     setTeId( uint32_t value);

      /// Get and set extended level-1 ID
      uint32_t lvl1Id() const;
      void     setLvl1Id( uint32_t value );

      /// Get and set lumi block
      uint32_t lumiBlock() const;
      void     setLumiBlock( uint32_t value );

      /// Get and set detector mask
      uint32_t muonDetMask() const;
      void     setMuonDetMask( uint32_t value );

      /// Get and set RoI ID
      uint32_t roiId() const;
      void     setRoiId( uint32_t value );

      /// Get and set RoI system ID
      uint32_t roiSystem() const;
      void     setRoiSystem(uint32_t value);

      /// Get and set RoI subsystem ID
      uint32_t roiSubsystem() const;
      void     setRoiSubsystem(uint32_t value);

      /// Get and set RoI sector ID
      uint32_t roiSector() const;
      void     setRoiSector( uint32_t value );

      /// Get and set RoI number
      uint32_t roiNumber() const;
      void     setRoiNumber( uint32_t value );

      /// Get and set RoI threshold number
      uint32_t roiThreshold() const;
      void     setRoiThreshold( uint32_t value );

      /// Get and set RoI eta
      float    roiEta() const;
      void     setRoiEta( float value );

      /// Get and set RoI phi
      float    roiPhi() const;
      void     setRoiPhi( float value );

      /// Set and get pT from different algorithms
      float tgcPt() const;
      void  setTgcPt( float value );

      float ptBarrelRadius() const;
      void  setPtBarrelRadius( float value );

      float ptBarrelSagitta() const;
      void  setPtBarrelSagitta( float value );

      float ptEndcapAlpha() const;
      void  setPtEndcapAlpha( float value );

      float ptEndcapBeta() const;
      void  setPtEndcapBeta( float value );

      float ptEndcapRadius() const;
      void  setPtEndcapRadius( float value );

      float ptCSC() const;
      void  setPtCSC( float value );

      /// Get the muon road information
      /// Chamber type
      int chamberType1( int station, int sector ) const;
      int chamberType2( int station, int sector ) const;
      /// Slope
      float roadAw( int station, int sector ) const;
      /// Intercept
      float roadBw( int station, int sector ) const;
      /// Z
      float zMin( int station, int sector ) const;
      float zMax( int station, int sector ) const;
      /// R
      float rMin( int station, int sector ) const;
      float rMax( int station, int sector ) const;
      /// Eta
      float etaMin( int station, int sector ) const;
      float etaMax( int station, int sector ) const;

      /// Set the muon road information
      void  setChamberType1( int station, int sector, int chamberType);
      void  setChamberType2( int station, int sector, int chamberType);
      void  setRoad( int station, int sector, float aw, float bw );
      void  setRegionZ( int station, int sector, float min, float max );
      void  setRegionR( int station, int sector, float min, float max );
      void  setRegionEta( int station, int sector, float min, float max );

      /// Get the results of TGC fit
      float tgcInnEta() const;
      float tgcInnPhi() const;
      float tgcInnR() const;
      float tgcInnZ() const;

      float tgcInnRhoStd() const;
      long  tgcInnRhoN() const;
      float tgcInnPhiStd() const;
      long  tgcInnPhiN() const;
 
      float tgcMid1Eta() const;
      float tgcMid1Phi() const;
      float tgcMid1R() const;
      float tgcMid1Z() const;

      float tgcMid2Eta() const;
      float tgcMid2Phi() const;
      float tgcMid2R() const;
      float tgcMid2Z() const;

      float tgcMidRhoChi2() const;
      long  tgcMidRhoN() const;
      float tgcMidPhiChi2() const;
      long  tgcMidPhiN() const;

      /// Set the results of TGC fit
      void setTgcInn( float eta, float phi, float r, float z ); 
      void setTgcInnF( float rhoStd, long rhoN, float phiStd, long phiN ); 
      void setTgcMid1( float eta, float phi, float r, float z ); 
      void setTgcMid2( float eta, float phi, float r, float z ); 
      void setTgcMidF( float rhoChi2, long rhoN, float phiChi2, long phiN ); 

      /// Get the results of RPC fit
      float rpcFitInnPhi() const;
      float rpcFitInnSlope() const;
      float rpcFitInnOffset() const;

      float rpcFitMidPhi() const;
      float rpcFitMidSlope() const;
      float rpcFitMidOffset() const;

      float rpcFitOutPhi() const;
      float rpcFitOutSlope() const;
      float rpcFitOutOffset() const;

      /// Set the results of RPC fit
      void setRpcFitInn( float phi, float slope, float offset ); 
      void setRpcFitMid( float phi, float slope, float offset ); 
      void setRpcFitOut( float phi, float slope, float offset ); 

      /// Get and set maximum size of storages to be reserved
      int      rpcHitsCapacity() const;
      void     setRpcHitsCapacity( int value );

      int      tgcHitsCapacity() const;
      void     setTgcHitsCapacity( int value );

      int      mdtHitsCapacity() const;
      void     setMdtHitsCapacity( int value );

      int      cscHitsCapacity() const;
      void     setCscHitsCapacity( int value );

      void     setStgcClustersCapacity( const int value );
      void     setMmClustersCapacity( const int value );

      void     setCapacities( int rpc, int tgc, int mdt, int csc );

      /// Get RPC hits
      const std::vector< uint32_t >& rpcHitLayer() const;
      const std::vector< uint32_t >& rpcHitMeasuresPhi() const;
      const std::vector< float >&    rpcHitX() const;
      const std::vector< float >&    rpcHitY() const;
      const std::vector< float >&    rpcHitZ() const;
      const std::vector< float >&    rpcHitTime() const;
      const std::vector< float >&    rpcHitDistToEtaReadout() const;
      const std::vector< float >&    rpcHitDistToPhiReadout() const;
      const std::vector< std::string >& rpcHitStationName() const;
      /// Set RPC hits
      void setRpcHit(uint32_t layer,  uint32_t measuresPhi, float x, float y, float z,
                     float time, float distEta, float distPhi, std::string stationName );

      /// Get TGC hits
      const std::vector< float >& tgcHitEta() const;
      const std::vector< float >& tgcHitPhi() const;
      const std::vector< float >& tgcHitR() const;
      const std::vector< float >& tgcHitZ() const;
      const std::vector< float >& tgcHitWidth() const;
      const std::vector< int >&   tgcHitStationNum() const;
      const std::vector< bool >&  tgcHitIsStrip() const;
      const std::vector< int >&   tgcHitBCTag() const;
      const std::vector< bool >&  tgcHitInRoad() const;
      /// Set TGC hits
      void setTgcHit(float eta, float phi, float r, float z, float width,
                     int stationNum, bool isStrip, int bcTag, bool inRoad);

      /// Get the online ID, offline ID, R, Z, redidual, time, space and sigma of each MDT tube
      uint32_t     nMdtHits() const;
      uint32_t     mdtHitOnlineId( unsigned int tube ) const;
      int          mdtHitOfflineId( unsigned int tube ) const;
      int          mdtHitIsOutlier( unsigned int tube ) const;
      int          mdtHitChamber( unsigned int tube ) const;
      float        mdtHitR( unsigned int tube ) const;
      float        mdtHitZ( unsigned int tube ) const;
      float        mdtHitPhi( unsigned int tube ) const;
      float        mdtHitResidual( unsigned int tube ) const;
      float        mdtHitTime( unsigned int tube ) const;
      float        mdtHitSpace( unsigned int tube ) const;
      float        mdtHitSigma( unsigned int tube ) const;
      /// Set the properties of each MDT tube
      void         setMdtHit( uint32_t onlineId, int isOutier, int chamber, float r, float z, float phi,
                              float residual, float time, float space, float sigma );
     
      /// Get CSC hits
      uint32_t     nCscHits() const;
      int          cscHitIsOutlier( unsigned int tube ) const;
      int          cscHitChamber( unsigned int tube ) const;
      uint32_t     cscHitStationName( unsigned int tube ) const;
      int          cscHitStationEta( unsigned int tube ) const;
      int          cscHitStationPhi( unsigned int tube ) const;
      int          cscHitChamberLayer( unsigned int tube ) const;
      int          cscHitWireLayer( unsigned int tube ) const;
      int          cscHitMeasuresPhi( unsigned int tube ) const;
      int          cscHitStrip( unsigned int tube ) const;
      float        cscHitEta( unsigned int tube ) const;
      float        cscHitPhi( unsigned int tube ) const;
      float        cscHitR( unsigned int tube ) const;
      float        cscHitZ( unsigned int tube ) const;
      int          cscHitCharge( unsigned int tube ) const;
      float        cscHitTime( unsigned int tube ) const;
      float        cscHitResidual( unsigned int tube ) const;
      /// Set the properties of each CSC tube
      void         setCscHit( int isOutlier, int chamber, uint32_t stationName,
                              int stationEta, int stationPhi, int chamberLayer, int wireLayer, int measuresPhi, int strip,
                              float eta, float phi, float r, float z, int charge, float time, float residual );

      /// Get sTGC clusters
      const std::vector< unsigned int >& stgcClusterLayer() const;
      const std::vector< int >&          stgcClusterIsOutlier() const;
      const std::vector< int >&          stgcClusterType() const;
      const std::vector< float >&        stgcClusterEta() const;
      const std::vector< float >&        stgcClusterPhi() const;
      const std::vector< float >&        stgcClusterR() const;
      const std::vector< float >&        stgcClusterZ() const;
      const std::vector< float >&        stgcClusterResidualR() const;
      const std::vector< float >&        stgcClusterResidualPhi() const;
      const std::vector< int >&          stgcClusterStationEta() const;
      const std::vector< int >&          stgcClusterStationPhi() const;
      const std::vector< int >&          stgcClusterStationName() const;
      /// Set sTGC hits
      void setStgcCluster(const unsigned int layer, const int isOutlier, const int type,
			  const float eta, const float phi, const float r, const float z,
			  const float residualR, const float residualPhi,
			  const int stationEta, const int stationPhi, const int stationName );

      /// Get MM clusters
      const std::vector< unsigned int >& mmClusterLayer() const;
      const std::vector< int >&          mmClusterIsOutlier() const;
      const std::vector< float >&        mmClusterEta() const;
      const std::vector< float >&        mmClusterPhi() const;
      const std::vector< float >&        mmClusterR() const;
      const std::vector< float >&        mmClusterZ() const;
      const std::vector< float >&        mmClusterResidualR() const;
      const std::vector< float >&        mmClusterResidualPhi() const;
      const std::vector< int >&          mmClusterStationEta() const;
      const std::vector< int >&          mmClusterStationPhi() const;
      const std::vector< int >&          mmClusterStationName() const;
      /// Set MM hits
      void setMmCluster(const unsigned int layer, const int isOutlier,
			const float eta, const float phi, const float r, const float z,
			const float residualR, const float residualPhi,
			const int stationEta, const int stationPhi, const int stationName );

      /// @}

   }; // class L2StandAloneMuon_v2

} // namespace xAOD

/// Operator for printing the contents of a LVL2 muon
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2StandAloneMuon_v2& mu );

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V2_H
