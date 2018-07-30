// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L2StandAloneMuon_v1.h 661304 2015-04-18 04:31:23Z mishitsu $
#ifndef XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V1_H
#define XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V1_H

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
   class L2StandAloneMuon_v1 : public IParticle {

   public:
      /// Constructor
      L2StandAloneMuon_v1();

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

      /// Get the slope at inner
      float   slopeInner() const;
      /// Set the slope at endcap
      void    setSlopeInner( float value );

      /// Get the intercept at inner
      float   interceptInner() const;
      /// Set the intercept at endcap
      void    setInterceptInner( float value );

      /// Get deltaR
      float   deltaR() const;
      /// Set deltaR
      void    setDeltaR( float value );

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
      uint32_t roiEta() const;
      void     setRoiEta( uint32_t value );

      /// Get and set RoI phi
      uint32_t roiPhi() const;
      void     setRoiPhi( uint32_t value );

      /// Set and get pT from different algorithms
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
      float tgcPt() const;

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
      void setTgcPt( float value );

      void setTgcInn( float eta, float phi, float r, float z ); 
      void setTgcInnF( float rhoStd, long rhoN, float phiStd, long phiN ); 
      void setTgcMid1( float eta, float phi, float r, float z ); 
      void setTgcMid2( float eta, float phi, float r, float z ); 
      void setTgcMidF( float rhoChi2, long rhoN, float phiChi2, long phiN ); 

      /// Get the results of RPC fit
      float rpc1x() const;
      float rpc1y() const;
      float rpc1z() const;

      float rpc2x() const;
      float rpc2y() const;
      float rpc2z() const;

      float rpc3x() const;
      float rpc3y() const;
      float rpc3z() const;

      /// Set the results of RPC fit
      void setRpc1( float x, float y, float z );
      void setRpc2( float x, float y, float z );
      void setRpc3( float x, float y, float z );

      /// Get and set maximum size of storages to be reserved
      int      extensionCapacity() const;
      void     setExtensionCapacity( int value );

      int      robCapacity() const;
      void     setRobCapacity( int value );

      int      csmCapacity() const;
      void     setCsmCapacity( int value );

      int      lvl1EmuCapacity() const;
      void     setLvl1EmuCapacity( int value );

      int      rpcHitsCapacity() const;
      void     setRpcHitsCapacity( int value );

      int      tgcHitsCapacity() const;
      void     setTgcHitsCapacity( int value );

      int      mdtHitsCapacity() const;
      void     setMdtHitsCapacity( int value );

      void     setCapacities( int ext, int rob, int csm, int lvl1, int rpc, int tgc, int mdt );

      /// Get extenstion
      const std::vector< uint32_t >& extension0() const;
      const std::vector< uint32_t >& extension1() const;
      const std::vector< uint32_t >& extension2() const;
      const std::vector< uint32_t >& extension3() const;
      const std::vector< uint32_t >& extension4() const;
      const std::vector< uint32_t >& extension5() const;
      const std::vector< uint32_t >& extension6() const;
      const std::vector< uint32_t >& extension7() const;
      const std::vector< uint32_t >& extension8() const;
      const std::vector< uint32_t >& extension9() const;
      /// Set extenstion
      void setExtension0( uint32_t value );
      void setExtension1( uint32_t value );
      void setExtension2( uint32_t value );
      void setExtension3( uint32_t value );
      void setExtension4( uint32_t value );
      void setExtension5( uint32_t value );
      void setExtension6( uint32_t value );
      void setExtension7( uint32_t value );
      void setExtension8( uint32_t value );
      void setExtension9( uint32_t value );

      /// Level 1 emulation info
      const std::vector< uint32_t >& lvl1Emulation() const;    
      void setEtaPivot(uint32_t ly0, uint32_t ly1 );
      void setEtaLow( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 );
      void setEtaHigh( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 );
      void setPhiPivot (uint32_t ly0, uint32_t ly1 );
      void setPhiLow( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 );
      void setPhiHigh( uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11 );

      /// ROB info
      const std::vector< uint32_t >& robId() const;
      const std::vector< uint32_t >& removedRobId() const;
      void setRequestedRob( uint32_t rob );
      void setRemovedRob( uint32_t rob );

      /// CSM info
      const std::vector< uint32_t >& csmId() const;
      const std::vector< uint32_t >& csmSize() const;
      const std::vector< uint32_t >& csmError() const;
      const std::vector< uint32_t >& removedCsmId() const;
      void setConvertedCsm( uint32_t csm, uint32_t sz );
      void setCsmError( uint32_t err );
      void setRemovedCsm( uint32_t csm );

      /// Get RPC pad hits
      const std::vector< uint32_t >& padHitOnlineId() const;
      const std::vector< uint32_t >& padHitCode() const;
      const std::vector< float >&    padHitX() const;
      const std::vector< float >&    padHitY() const;
      const std::vector< float >&    padHitZ() const;
      const std::vector< float >&    padHitR() const;
      const std::vector< float >&    padHitP() const;
      /// Set RPC pad hits
      void setPadHit(uint32_t onlineId, uint32_t code, float x, float y, float z,
                     float r, float p);

      /// Get TGC hits
      const std::vector< float >&    tgcInnRhoHitPhi() const;
      const std::vector< float >&    tgcInnRhoHitR() const;
      const std::vector< float >&    tgcInnRhoHitZ() const;
      const std::vector< float >&    tgcInnRhoHitWidth() const;
      const std::vector< uint32_t >& tgcInnRhoHitInSeg() const;
      const std::vector< float >&    tgcInnPhiHitPhi() const;
      const std::vector< float >&    tgcInnPhiHitR() const;
      const std::vector< float >&    tgcInnPhiHitZ() const;
      const std::vector< float >&    tgcInnPhiHitWidth() const;
      const std::vector< uint32_t >& tgcInnPhiHitInSeg() const;
      const std::vector< float >&    tgcMidRhoHitPhi() const;
      const std::vector< float >&    tgcMidRhoHitR() const;
      const std::vector< float >&    tgcMidRhoHitZ() const;
      const std::vector< float >&    tgcMidRhoHitWidth() const;
      const std::vector< uint32_t >& tgcMidRhoHitInSeg() const;
      const std::vector< float >&    tgcMidPhiHitPhi() const;
      const std::vector< float >&    tgcMidPhiHitR() const;
      const std::vector< float >&    tgcMidPhiHitZ() const;
      const std::vector< float >&    tgcMidPhiHitWidth() const;
      const std::vector< uint32_t >& tgcMidPhiHitInSeg() const;
      /// Set TGC hits
      void setTgcInnRhoHit( float phi, float r, float z, float width, uint32_t in_seg );
      void setTgcInnPhiHit( float phi, float r, float z, float width, uint32_t in_seg );
      void setTgcMidRhoHit( float phi, float r, float z, float width, uint32_t in_seg );
      void setTgcMidPhiHit( float phi, float r, float z, float width, uint32_t in_seg );

      /// Get the online ID, offline ID, R, Z, redidual, time, space and sigma of each MDT tube
      uint32_t     nMdtHits() const;
      uint32_t     mdtHitOnlineId( unsigned int tube ) const;
      uint32_t     mdtHitOfflineId( unsigned int tube ) const;
      uint32_t     mdtHitChamber( unsigned int tube ) const;
      float        mdtHitR( unsigned int tube ) const;
      float        mdtHitZ( unsigned int tube ) const;
      float        mdtHitResidual( unsigned int tube ) const;
      float        mdtHitTime( unsigned int tube ) const;
      float        mdtHitSpace( unsigned int tube ) const;
      float        mdtHitSigma( unsigned int tube ) const;
      /// Set the properties of each MDT tube
      void         setMdtHit( uint32_t onlineId, uint32_t offlineId, int chamber, float r, float z, 
                              float residual, float time, float space, float sigma );
     
      /// @}

   }; // class L2StandAloneMuon_v1

} // namespace xAOD

/// Operator for printing the contents of a LVL2 muon
std::ostream& operator<< ( std::ostream& out,
                           const xAOD::L2StandAloneMuon_v1& mu );

#endif // XAODTRIGMUON_VERSIONS_L2STANDALONEMUON_V1_H
