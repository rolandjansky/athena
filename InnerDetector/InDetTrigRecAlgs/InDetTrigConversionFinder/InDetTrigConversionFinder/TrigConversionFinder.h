/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************** 
                          TrigConversionFinder.h  -  Description 
                             ------------------- 
    begin   : 10-11-2005 
    authors : Tatjana Lenz 
    email   : tatjana.lenz@cern.ch 
    trigger changes : Christopher Marino, Iwona Grabowska-Bold
 ***************************************************************************/ 

#ifndef INDETTRIGCONVERSIONFINDER_TRIGCONVERSIONFINDER_H 
#define INDETTRIGCONVERSIONFINDER_TRIGCONVERSIONFINDER_H 
 
#include "GaudiKernel/ToolHandle.h" 

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "VxVertex/VxContainer.h" 

namespace Rec { class TrackParticleContainer; }

namespace InDet
{ 
 
  class IVertexFinder; 
  class TrigConversionFinder : public HLT::FexAlgo 
    { 
	 
    public:   
      TrigConversionFinder(const std::string &name, ISvcLocator *pSvcLocator); 
      ~TrigConversionFinder();
      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);    
      HLT::ErrorCode hltFinalize();

      void resetStatistics(); 
      void analyzeResults(xAOD::VertexContainer*); 
 
    protected: 
      int m_nTracks;
      int m_nVertices;
            
      ToolHandle< IVertexFinder > m_VertexFinderTool;
 
      /** Statistics  */ 
      long m_events_processed;    //!< Number of events processed 
      long m_Gamma_stored;        //!< Number of conversion vertices stored 
      long m_Double_Conversions;  //!< Number of two-track conversions 
      long m_Single_Conversions;  //!< Number of single-track conversions 
      long m_SiSi_Conversions;    //!< Number of Si-Si track conversions 
      long m_SiTrt_Conversions;   //!< Number of Si-TRT track conversions 
      long m_TrtTrt_Conversions;  //!< Number of TRT-TRT track conversions 
      long m_Si_Conversions;      //!< Number of Si single-track conversions 
      long m_Trt_Conversions;     //!< Number of TRT single-track conversions 
      
    }; 
} 
#endif // INDETTRIGCONVERSIONFINDER_TRIGCONVERSIONFINDER_H 
