/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackInfo.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <sstream>



Trk::TrackInfo::TrackInfo():m_fitter(Unknown), m_particleHypo(undefined)  
{ 
  m_properties.reset();
}    

Trk::TrackInfo::TrackInfo( const TrackFitter fitter, const ParticleHypothesis partHyp)
    :m_fitter(fitter), m_particleHypo(partHyp)
{
 if( m_particleHypo == undefined )
 {							       
  switch( m_fitter ) //switch on fitter of type TrackFitter (ok)
  {  				       
   case GaussianSumFilter:	 //GaussianSumFilter of type TrackFitter (ok, =4)       		  
     m_particleHypo = electron;	        		  
   break;			        		  
  /** The following cases all fall through to Muonboy/GlobalChi2Fitter; is this intentional? 
    * Coverity bug 12476 : the enum types were mixed; I've commented out the original code
    * but the enums should be checked  and made consistent with code here
    * should use C++11 scoped enums to protect against this kind of error (sroe)						       
   		  
   case STACO: // STACO of type TrackPatternRecoInfo (not ok!), = 7 = DeterministicAnnealingFilter     	       
   case MuGirlUnrefitted:	// type TrackPatternRecoInfo (not ok!), = 6 = DistributedKalmanFilter  		  
   case Muonboy:		      // type TrackPatternRecoInfo (not ok!), = 5 = GlobalChi2Fitter   
   **/
   case DeterministicAnnealingFilter:
   case DistributedKalmanFilter:
   case GlobalChi2Fitter:
     m_particleHypo = muon;	        		  
   break;			        		  
   				      	   		  
   default:			        	       
     m_particleHypo = pion;	        		  
  }//end of switch operator				       	       
 }//end of particle hypothesis check			       
 m_properties.reset();
 m_patternRecognition.reset(); 
}//end of main constructor definitions


Trk::TrackInfo::TrackInfo( const TrackFitter fitter,
                           const ParticleHypothesis partHyp,
                           const std::bitset<NumberOfTrackProperties>& properties,
                           const std::bitset<NumberOfTrackRecoInfo>& patternRecognition)
  : TrackInfo (fitter, partHyp)
{
  m_properties = properties;
  m_patternRecognition = patternRecognition;
}

Trk::TrackInfo::TrackInfo (const TrackInfo& rhs)
    :
    m_fitter(rhs.m_fitter),
    m_particleHypo(rhs.m_particleHypo), 
    m_properties(rhs.m_properties), 
    m_patternRecognition(rhs.m_patternRecognition) 
{
    //sroe: removed duplicated code
}

Trk::TrackInfo& 
Trk::TrackInfo::operator= (const TrackInfo& rhs)
{
    if (this!=&rhs){
      //set the fitter to be that of the TrackInfo being copied.
      m_fitter  	         = rhs.m_fitter;	        
      m_particleHypo	     = rhs.m_particleHypo;        
      m_properties	       = rhs.m_properties;   
      m_patternRecognition = rhs.m_patternRecognition;  
    }
    return *this;
}

Trk::TrackInfo::~TrackInfo()
{}

std::string Trk::TrackInfo::dumpInfo() const
{
    std::stringstream authorInfo;
    authorInfo<<"The Fitter of this track is: ";
    
    switch ( m_fitter )
    {
     case Trk::TrackInfo::Unknown: authorInfo<<"Unknown";    
     break; 		     
        
     case Trk::TrackInfo::iPatTrackFitter: authorInfo<<"iPatTrackFitter";   
     break;		        
     
     case Trk::TrackInfo::xKalman: authorInfo<<"xKalman";   
     break;
      		         
     case Trk::TrackInfo::KalmanFitter: authorInfo<<"KalmanFitter";
     break;  		   		    
     				   		   
     case Trk::TrackInfo::GaussianSumFilter: authorInfo<<"GaussianSumFilter"; 
     break;
      		         
     case Trk::TrackInfo::GlobalChi2Fitter: authorInfo<<"GlobalChi2Fitter";   	   		    
     break;

     case Trk::TrackInfo::DistributedKalmanFilter: authorInfo<<"DistributedKalmanFilter";	   		    
     break;
         
     case Trk::TrackInfo::DeterministicAnnealingFilter: authorInfo<<"DeterministicAnnealingFilter"; 											    
     break;

     case Trk::TrackInfo::MuonboyFitter: authorInfo<<"MuonboyFitter";                                                                
     break;     

    default: authorInfo<<"Fitter not recognized " << m_fitter;
    }
   
//adding properties, but checking first 
// that they are there at all   
    if(m_properties.any())
    { 
     authorInfo<< (m_properties.test(Trk::TrackInfo::BremFit) ? " + BremFit": "");
     authorInfo<< (m_properties.test(Trk::TrackInfo::BremFitSuccessful) ? " + BremFitSuccess": "");
     authorInfo<< (m_properties.test(Trk::TrackInfo::StraightTrack) ? " + StraightTrack": "");
     authorInfo<< (m_properties.test(Trk::TrackInfo::SlimmedTrack) ? " + SlimmedTrack": "");
     authorInfo<< (m_properties.test(Trk::TrackInfo::HardScatterOrKink) ? " + HardScatterOrKink": "");
     authorInfo<< (m_properties.test(Trk::TrackInfo::LowPtTrack) ? " + LowPtTrack": "");
    }//end of adding track properties
    
//adding a pattern recognition structure
    if(m_patternRecognition.any())
    {
//     std::cout<<"calling the pattern reco dump"<<std::endl;
     authorInfo<<"; The Pattern Recognition Agorithms used: ";
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSPSeededFinder)	     	      ? "SiSPSeedFinder; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiCTBTracking)	     	      ? "SiCTBTracking; ": "");
//      std::cout<<"3d line pattern reco dump"<<std::endl;
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::InDetAmbiguitySolver)   	      ? "InDetAmbiguitySolver; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::InDetExtensionProcessor)	      ? "InDetExtensionProcessor; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TRTSeededTrackFinder)   	      ? "TRTSeededTrackFinder; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::Muonboy)		     	      ? "Muonboy; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuGirlUnrefitted)       	      ? "MuGirlUnrefitted; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::STACO)		     	      ? "STACO; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::StacoLowPt)	     	      ? "StacoLowPt; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuTag)		     	      ? "MuTag; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MooreToTrackTool)       	      ? "MooreToTrackTool; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TrigIDSCAN)	     	      ? "TrigIDSCAN; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TrigSiTrack)	     	      ? "TrigSiTrack; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TrigTRTxK) 	     	      ? "TrigTRTxK; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TrigTRTLUT)	     	      ? "TrigTRTLUT; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::Fatras)		     	      ? "Fatras; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::iPatLegacyCnv)	     	      ? "iPatLegacyCnv; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::xKalmanLegacyCnv)	     	      ? "xKalmanLegacyCnv; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SimpleAmbiguityProcessorTool)    ? "SimpleAmbiguityProcessorTool; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::InDetAmbiTrackSelectionTool)     ? "InDetAmbiTrackSelectionTool; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TRTStandalone)                   ? "TRTStandalone; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidStandAlone)		      ? "MuidStandAlone; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TRTSeededSingleSpTrackFinder)    ? "TRTSeededSingleSpTrackFinder; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MooreLegacyCnv )		      ? "MooreLegacyCnv; ": "");     
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidComb)   		      ? "MuidComb; ": "");    
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::Moore)		              ? "Moore; ": "");		   
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidCombined)		      ? "MuidCombined; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidVertexAssociator)	      ? "MuidVertexAssociator; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuGirl)			      ? "MuGirl; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::iPatRec)			      ? "iPatRec; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuGirlLowBeta)		      ? "MuGirlLowBeta; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::FatrasSimulation)                ? "FatrasSimulation; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuonNotHittingTileVolume)        ? "MuonNotHittingTileVolume; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::ReverseOrderedTrack)             ? "ReverseOrderedTrack; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_Cosmic)  		       ? "SiSpacePointsSeedMaker_Cosmic; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_HeavyIon)  		       ? "SiSpacePointsSeedMaker_HeavyIon; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_LowMomentum)  	       ? "SiSpacePointsSeedMaker_LowMomentum; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_BeamGas)	               ? "SiSpacePointsSeedMaker_BeamGas; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_VeryLowMomentum)  	       ? "SiSpacePointsSeedMaker_VeryLowMomentum; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_ROIConvTracks)                 ? "SiSpacePointsSeedMaker_ROIConvTracks; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_LargeD0)  	       ? "SiSpacePointsSeedMaker_LargeD0; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::Pseudotracking		)  	       ? "Pseudotracking; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidMuonRecoveryTool)  	       ? "MuidMuonRecoveryTool; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::MuidStandaloneRefit)  	       ? "MuidStandaloneRefit; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::TrackInCaloROI)  	               ? "TrackInCaloROI; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSpacePointsSeedMaker_ForwardTracks)  	        ? "SiSpacePointsSeedMaker_ForwardTracks; ": ""); 
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::strategyA)  	       ? "strategyA; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::strategyB)  	       ? "strategyB; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::strategyC)  	       ? "strategyC; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::FTK)  	       ? "FTK; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::FastTrackFinderSeed)  	       ? "FastTrackFinderSeed; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::SiSPSeededFinderSimple)  	               ? "SiSPSeededFinderSimple; ": "");
     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::FastTrackingFitterTool)  	               ? "FastTrackingFitterTool; ": "");
     
     
//     authorInfo<< (m_patternRecognition.test(Trk::TrackInfo::NumberOfTrackRecoInfo)   ? "NumberOfTrackRecoInfo; ": "");
 //    std::cout<<"end of the pattern reco dump"<<std::endl;
    }//end of adding pattern recognition properties
    return authorInfo.str();
}//end of dump method

std::ostream& Trk::operator << ( std::ostream& sl, const Trk::TrackInfo& info)
{
    sl <<info.dumpInfo()<<std::endl;
    return sl;
}

MsgStream& Trk::operator << ( MsgStream& sl, const Trk::TrackInfo& info)
{
    sl <<info.dumpInfo()<<endmsg;
    return sl;
}


//new mathod, allowing to merge properties and pattern reco info
//from different TrackInfo's

void Trk::TrackInfo::addPatternRecoAndProperties(const TrackInfo& rhs) const
{
//merging information by means of bitwise "or"
  addPatternReco(rhs);
  m_properties = m_properties | rhs.m_properties;
}//end addPatternRecoaAndProperties method

void Trk::TrackInfo::addPatternReco(const TrackInfo& rhs) const
{
//merging information by means of bitwise "or"
  m_patternRecognition = m_patternRecognition | rhs.m_patternRecognition;
}//end addPatternReco method
