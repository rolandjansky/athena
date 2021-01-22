/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKTRACK_TRACKINFO_H
#define TRKTRACK_TRACKINFO_H

#include <vector>
#include <iostream>
#include <string>
#include <bitset>

#include "TrkEventPrimitives/ParticleHypothesis.h"

class MsgStream;
class TrackCnv_p4;
class TrackParticleBaseCnv_p1;

namespace Trk
{
    
/**
 * @brief Contains information about the 'fitter' of this track. 
 *  Additionally there is some information about how the e.g. 
 *  fit was configured. Also the information on the properties
 *  of the  track fit is stored.
 * 
 * @author Kirill.Prokofiev@cern.ch
 * @author edward.moyse@cern.ch
 */

class TrackInfo
{
public:

/** 
 * enums to identify who created this track and whic propertis does it have. 
 * N.B If you update this, don't forget to update the dump method!
 */
    enum TrackFitter
    {
/**Track fitter not defined.*/  					
      Unknown			   = 0, 			         

/**tracks produced by iPat*/						
      iPatTrackFitter		   = 1, 			         

/**tracks produced by xKalman*/     				         
      xKalman			   = 2, 			         
  
/**tracks produced by the Kalman Fitter*/       		         
      KalmanFitter		   = 3, 			         
     								        
/**Tracks from Gaussian Sum Filter*/     			         
      GaussianSumFilter 	   = 4, 			         
     
/**Track's from Thijs' global chi^2 fitter*/      		         
      GlobalChi2Fitter  	   = 5, 			         

/**Fast Kalman filter from HLT with simplified material effects*/        
      DistributedKalmanFilter	   = 6, 			         

/**Adpative track fitter with fuzzy hit assignment*/      	         
      DeterministicAnnealingFilter = 7,
      
/** Kalman DNA Fitter*/      
      KalmanDNAFitter	           = 8,		              		

/** Track produced with Muonboy's internal fitter.*/
      MuonboyFitter                = 9,						         

/**maximum number of enums*/     				         
      NumberOfTrackFitters	   = 10				         
    };
     
    enum TrackProperties
    {
    
/** A brem fit was performed on this track*/       
     BremFit		     = 1, 	
     
/** A brem fit was performed on this track and this fit was successful*/
     BremFitSuccessful       = 2,
     
/** A straight track*/      	 
     StraightTrack	     = 3,	 
     
/** A slimmed track*/      
     SlimmedTrack	     = 4,
     
/** A track with a kink or a hard scatter*/                	 
     HardScatterOrKink       = 5,

/** A LowPt track */               	 
     LowPtTrack 	     = 6,

/**maximum number of enums*/        
     NumberOfTrackProperties = 7 
    };

    enum TrackPatternRecoInfo
    {    
/**  Tracks from SiSPSeedFinder */             
     SiSPSeededFinder             = 0,
     
/**  Tracks from SiCTBTracking */                 
     SiCTBTracking                = 1,
     
/**  Tracks with InDetAmbiguitySolver used */            
     InDetAmbiguitySolver         = 2,
     
/**  Tracks with InDetExtensionProcessor used */       
     InDetExtensionProcessor      = 3,
     
/**  Tracks from TRT Seeded Track finder */       
     TRTSeededTrackFinder         = 4,
     
/**  Tracks from MuonBoy */     
     Muonboy                      = 5,
 
/**  Tracks from MuGirl not refitted with iPat */
     MuGirlUnrefitted             = 6,
     
/**  Tracks produced by STACO */
     STACO                        = 7,
        
/**  Tracks produced by StacoLowpt */
     StacoLowPt                   = 8, 
     
/**  Tracks produced by MuTag */	
     MuTag                        = 9,

/**  Tracks produced by Moore */
     MooreToTrackTool             = 10,  
         
/**  TrigInDetTracks produced by TrigIDSCAN, running at trigger LVL2*/
     TrigIDSCAN                   = 11, 

/**  TrigInDetTracks produced by TrigSiTrack, running at trigger LVL2*/
     TrigSiTrack                  = 12, 

/**  TrigInDetTracks produced by TrigTRTxK, running at trigger LVL2*/
     TrigTRTxK                    = 13,

/**  TrigInDetTracks produced by TrigTRTLUT, running at trigger LVL2*/
     TrigTRTLUT                   = 14,
     
/**  Track from FATRAS*/
     Fatras                       = 15,
     
/**  Added because of compilation problems*/     
     iPatLegacyCnv                = 16,
     
/**  Added because of compilation problems*/     
     xKalmanLegacyCnv             = 17,
     
/**  Added because of compilation problems*/     
     SimpleAmbiguityProcessorTool = 18,
  
/**  Added because of compilation problems*/    
     InDetAmbiTrackSelectionTool  = 19,
          
/**  TRT Standalone */	  
     TRTStandalone                = 20,
    
/**  MuidStandalone */     
     MuidStandAlone               = 21,
     
/**  TRTSeededSingleSpTrackFinder */
     TRTSeededSingleSpTrackFinder = 22,
     
/**  Back compatibility with old type of ESD */
     MooreLegacyCnv               = 23,
     
/**  Back compatibility with older ESD versions */     
     MuidComb                     = 24,
     
/**  Moore (MoMu) */ 
     Moore                        = 25,    
     
/**  MuidCombined */     
     MuidCombined	          = 26,
         
/**  MuidVertexAssociator */     
     MuidVertexAssociator         = 27,
         
/**  MuGirl */     
     MuGirl                       = 28,
          
/**  iPatRec */     
     iPatRec                      = 29,
     
/**  MuGirlLowBeta */     
     MuGirlLowBeta                = 30,
     
/**  Fatras Simulation */     
     FatrasSimulation             = 31,     

/** Track which has its hits behind it, e.g. a downward moving cosmic track which through the upper hemisphere of ATLAS will have a beamline perigee pointing 
down, but many/all of its hits will be above. A downward moving cosmic track which is reconstructed across all of ATLAS could also have this flag set (i.e. 
it does not mean that hits are ONLY behind the perigee.
@warning This enum is the subject of some controversy and may be removed in the future. It would be safer to examine 
the track and determine this for yourself (especially as not all Track builders are currently filling this flag). EJWM.
*/
     ReverseOrderedTrack         = 32,

/** Track which (physically) does not intersect the ID or Calo. i.e. not a partially reconstructed IP track, but a cosmic or beam halo track.
@warning This enum is the subject of some controversy and may be removed in the future. It would be safer to examine
the track and determine this for yourself (especially as not all Track builders are currently filling this flag). EJWM.
*/
     MuonNotHittingTileVolume    = 33,
          
/**
 * Entries allowing to distinguish different seed makers
 */     
     SiSpacePointsSeedMaker_Cosmic          = 34,
     SiSpacePointsSeedMaker_HeavyIon        = 35,
     SiSpacePointsSeedMaker_LowMomentum     = 36,
     SiSpacePointsSeedMaker_BeamGas         = 37,
     SiSpacePointsSeedMaker_VeryLowMomentum = 38,
     
/** Muons found by the ID seeded muon recovery */
     MuidMuonRecoveryTool        = 39,

/** Standalone muon that was obtained by refitting a combined muon using the calorimeter information of the combined and the muon measurements */
     MuidStandaloneRefit         = 40,

/** A track in a CaloROI */
     TrackInCaloROI              = 41,    

/**
 * Entries allowing to distinguish different seed makers
 */     
     SiSpacePointsSeedMaker_ForwardTracks       = 42,

/**  L2Star strategies */
     strategyA                                  = 43,
     strategyB                                  = 44,
     strategyC                                  = 45,


/**   for tracks from FTK */
     FTK                                        = 46,

/**  for tracks seeded by the FastTrackFinder */
     FastTrackFinderSeed                        = 47,

/**    for tracks processed by the trigger version of the SiSPSeededFinder */
     SiSPSeededFinderSimple                     = 48,

/**  Large d0 for displaced vertex searches */
     SiSpacePointsSeedMaker_LargeD0             = 49,

/** SLHC ROIConv Track flag */
     SiSpacePointsSeedMaker_ROIConvTracks       = 50,

/** Pseudo-tracking flag */
     Pseudotracking				= 51,

/** Track seeded */
     SiSpacePointsSeedMaker_TrkSeeded           = 52,
     
/**  For track using fitter tool as ambiguity processor tool.
 *   This is used in the ITk fast tracking:
 *   Ambiguity resolution is switched off and SiSPSeededTracks are only refitted
 */
     FastTrackingFitterTool                     = 53,


/**  Maximum number of enums */   
     NumberOfTrackRecoInfo                      = 54
    };
    
    TrackInfo (); //!<needed by POOL. DO NOT USE YOURSELF! 

/** 	           
 * Constructor     
 */	           
    TrackInfo( const TrackFitter fitter, const ParticleHypothesis partHyp);

    TrackInfo( const TrackFitter fitter,
               const ParticleHypothesis partHyp,
               const std::bitset<NumberOfTrackProperties>& properties,
               const std::bitset<NumberOfTrackRecoInfo>& patternRecognition);

    TrackInfo( const TrackInfo& ); //!< copy constructor

    TrackInfo &operator= (const TrackInfo &); //!< assignment operator

    virtual ~TrackInfo (); //!< destructor
    
    virtual TrackInfo* clone() const;

//Various set methods

/**
 * Methods setting the properties of track
 */
   void setTrackProperties(const TrackProperties& properties) const;

/**
 * Method setting the pattern recognition algorithm
 */
   void setPatternRecognitionInfo(const TrackPatternRecoInfo& patternReco) const;

/**
 * Method for re-setting a track fitter
 */
   void setTrackFitter(const TrackFitter& fitter) const;
  
/**
 *  Method re-setting the ParticleHypothesis
 */  
   void setParticleHypothesis(const ParticleHypothesis& hypothesis) const;

/**
 * Access methods for track fitter 
 */
   TrackFitter& trackFitter() const;
    
/**
 * Access methods for track properties 
 */   
   bool trackProperties(const TrackProperties& property) const;

/**
 * Access method for pattern recognition algorithm
 */
   bool  patternRecoInfo(const TrackPatternRecoInfo& pattern) const;

/**
 * Method merging pattern recognition and track properties 
 * information from different Track infos.
 */
   void addPatternRecoAndProperties(const TrackInfo&) const;

/**
 * A method adding just pattern recognition info without 
 * adding the actual properties
 */
  void addPatternReco(const TrackInfo&) const;

/** 							          
 * Returns a string with the name of the fitter of this track     
 * (i.e. corresponding to the enum reurned by fitter() )          
 */							          
   std::string dumpInfo() const;
    
   std::string dumpBitsProperties() const;
   
   std::string dumpBitsPatternRecoInfo() const; 
    
/** 							     
 * Returns the particle hypothesis used for Track fitting     
 */							     
   ParticleHypothesis particleHypothesis() const;

   const  std::bitset<NumberOfTrackProperties>&   properties() const;
   const  std::bitset<NumberOfTrackRecoInfo>&     patternRecognition() const;
 
  private:
   friend class ::TrackCnv_p4;
   friend class ::TrackParticleBaseCnv_p1;

/** 									     	 
 * This is an enum, which stores the identity of where the track was created 	 
 */									     	 
    mutable TrackFitter         m_fitter;
   
/**
 * This is an enum, which stores the particle hypothesis (if any)    
 * used for the track fitting					     
 */								      
    mutable ParticleHypothesis  m_particleHypo;   
    
/**						      
 * A bitset providing information on the properties    
 * of the track.				      			      
 */						        
    mutable  std::bitset<NumberOfTrackProperties>     m_properties;

/**						      
 * A bitset providing information on the 
 *  actual pattern recognition algotithm  			      
 */						        
    mutable  std::bitset<NumberOfTrackRecoInfo>     m_patternRecognition;  
          
 };//end of class definitions

/** 
 * Overload of << operator for MsgStream for debug output 
 */ 
  MsgStream& operator << ( MsgStream& sl, const TrackInfo& track);

/** 
 * Overload of << operator for std::ostream for debug output
 */ 
  std::ostream& operator << ( std::ostream& sl, const TrackInfo& track);

//set methods  
  inline void Trk::TrackInfo::setTrackProperties(const TrackProperties& property) const
  {
//   std::cout<<"Set track properties "<<std::endl;
   m_properties.set(property);
   if(property == BremFitSuccessful && m_particleHypo == undefined ) m_particleHypo = electron;
  }

  inline void Trk::TrackInfo::setPatternRecognitionInfo(const TrackPatternRecoInfo& patternReco) const
  {
//   std::cout<<"Setting pattern reco "<<std::endl;
  
   m_patternRecognition.set(patternReco);
  }

  inline void Trk::TrackInfo::setTrackFitter(const TrackFitter& fitter) const
  {
    m_fitter = (fitter < NumberOfTrackFitters)? fitter : Unknown;  
  }

  inline void Trk::TrackInfo::setParticleHypothesis(const ParticleHypothesis& hypothesis) const
  {
   m_particleHypo = hypothesis;
  }

//access methods
  inline Trk::TrackInfo::TrackFitter& Trk::TrackInfo::trackFitter() const
  {
   return m_fitter;
  }

  inline Trk::ParticleHypothesis  Trk::TrackInfo::particleHypothesis() const
  {
   return m_particleHypo;
  }

  inline bool Trk::TrackInfo::trackProperties(const TrackProperties& property) const
  {
   return m_properties.test(property);
  }
  
  inline bool Trk::TrackInfo::patternRecoInfo(const TrackPatternRecoInfo& pattern) const
  {
   return m_patternRecognition.test(pattern);
  }
  
  inline TrackInfo * TrackInfo::clone()const
  {
   return new TrackInfo(*this);
  }
  
  inline std::string TrackInfo::dumpBitsProperties() const
  {
   return m_properties.to_string <char, std::char_traits<char>, std::allocator<char> >();
  }
   
  inline std::string TrackInfo::dumpBitsPatternRecoInfo() const
  {
   return m_patternRecognition.to_string <char, std::char_traits<char>, std::allocator<char> >();
  } 
    

  inline
  const  std::bitset<TrackInfo::NumberOfTrackProperties>&   TrackInfo::properties() const
  {
    return m_properties;
  }
  inline
  const  std::bitset<TrackInfo::NumberOfTrackRecoInfo>&     TrackInfo::patternRecognition() const
  {
    return m_patternRecognition;
  }

}//end of namespace Trk

#endif

