/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LowBetaAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

/* class LowBetaAlg

LowBetaAlg is an algoirthm for the identification of Charged Stable 
Massive Particles based on tracking information mainly from the TRT.  
Timing information and energy deposition are used to indentify 
candidate tracks and make measurement of beta and excess ionization 
comparing to relativistic particles. 
 
author Christopher.Marino <Christopher.Marino@cern.ch> 

*/

#ifndef LOWBETAALG_H
#define LOWBETAALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

/////////////////////////////////////////////////////////////////////////////

class AtlasDetectorID;
class Identifier;
class TRT_ID;
class TrtToolBetaLiklihood;
class ITRT_ToT_dEdx;


// Predeclare histogram classes that you use.


namespace InDetDD{ class TRT_DetectorManager; }



namespace Trk {class Track;}

namespace Rec { class TrackParticleContainer; 
                class TrackParticle; }

namespace InDet 
{


  class LowBetaAlg:public AthAlgorithm {
  public:
    LowBetaAlg (const std::string& name, ISvcLocator* pSvcLocator);
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    std::vector<float> ChargedSMPindicators(const Trk::Track& track);
    
    

  protected:
    
    const TRT_ID*              m_trtId;               // TRT ID helper (identifying barrel/wheels and global position)

    const InDetDD::TRT_DetectorManager* m_TRTdetMgr;  // TRT detector manager (to get ID helper)
    
    unsigned int               m_minTRThits;          // Minimum number of TRT hits to give PID.
    float                      m_RcorrZero;           // Inputs for R Correction     
    float                      m_RcorrOne;            // Inputs for R Correction   
    float                      m_RcorrTwo;            // Inputs for R Correction   
    float                      m_TimingOffset;        // timing offset for trailing bit time  
    bool                       m_mcswitch;
 
    //std::string m_tracksName;                          //!< Name of track container in StoreGate
    std::string m_trackParticleCollection;                          //!< Name of track container in StoreGate
    std::string m_InDetLowBetaOutputName;             //!< Name of output container to store results
    
    /** trying to get ahold of the TRT calib DB: */
    ServiceHandle<ITRT_CalDbSvc> m_trtconddbsvc;

    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;

  private: // Functions/variables for using TrtToolsBetaLiklihood, see TrtToolsWrapper.cxx
    
    //////////
    ////////// Member variables needed for TrtToolBetaLiklihood to function properly
    //////////
    
    // The actual TrtToolBetaLiklihood class instance (all the rest here is the wrapper)
    TrtToolBetaLiklihood* m_TrtTool;
    
    ToolHandle<ITRT_ToT_dEdx> m_TRTdEdxTool;
    bool m_TrtToolsSuccess;

    // boolean value that specifies whether TrtToolBetaLiklihood was initialized successfully
    bool m_TrtToolInitSuccess;
    // Track refit container
    //std::string m_RefittedTracksContainerName;
    std::string m_UnslimmedTracksContainerName;
    
    //////////
    ////////// ATHENA function equivalents (call from corresponding function)
    //////////
    StatusCode initializeTrtToolBetaLiklihood();
    int finalizeTrtToolBetaLiklihood();
    
    //////////
    ////////// Conditions Database interaction functions
    //////////
    
    // Function for updating the TRT conditions database entries
    // Adapted from TRT_DriftFunctionTool.cxx
    StatusCode update(IOVSVC_CALLBACK_ARGS);
    
    //////////
    ////////// Wrapper functions for calling TrtToolBetaLiklihood
    //////////
    
    // A wrapper intermediary for the TrtToolBetaLiklihood::TRT_FEbeta function
    // Meant to be called from LowBetaAlg.cxx, via InDet::LowBetaAlg::ChargedSMPindicators
    // Returns a vector of results from TRT_FEbeta where:
    //	vector[0] = LikelihoodBeta
    //	vector[1] = LikelihoodError
    std::vector<float> callTrtToolBetaLiklihood(const Trk::Track& track);
    
    // Gather all of the necessary data that the TRT_FEbeta function takes as inputs
    // Sets all of the input arguments
    StatusCode parseDataForTrtToolBetaLiklihood(const Trk::Track& track,
			std::vector<int>* TRT_bitpattern,std::vector<int>* TRT_bec,std::vector<int>* TRT_strawlayer,std::vector<int>* TRT_layer,
			std::vector<float>* TRT_t0,std::vector<float>* TRT_estDrift,std::vector<float>* TRT_R,std::vector<float>* TRT_R_track,
			std::vector<float>* TrackX,std::vector<float>* TrackY,std::vector<float>* TrackZ,float* RecPt,float* RecEta, std::vector<int>* TRT_isTube);
    
    
    
    
  };

}//end of namespace
#endif // LOWBETAALG_H




