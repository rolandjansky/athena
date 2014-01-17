/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_EventPhaseAnalysis.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_EVENTPHASEANALYSIS_H
#define INDETTRT_EVENTPHASEANALYSIS_H

// Gaudi includes
//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "InDetIdentifier/TRT_ID.h"
#include "InDetCosmicsEventPhase/InDetCosmicsEventPhaseTool.h" //~~~brett~~~
#include "TRT_ConditionsServices/ITRT_CalDbSvc.h"

#include "InDetTrackSplitterTool/IInDetTrackSplitterTool.h"

#include <string>

#include "TTree.h"

class AtlasDetectorID;
class Identifier;

namespace InDet 
{

  /** @class TRT_EventPhaseAnalysis

  This is for the Doxygen-Documentation.  
  Please delete these lines and fill in information about
  the Algorithm!
  Please precede every member function declaration with a
  short Doxygen comment stating the purpose of this function.
      
  @author  Brett Jackson <brett.david.jackson@cern.ch>
  */  

  class TRT_EventPhaseAnalysis : public AthAlgorithm
    {
    public:

      /** Standard Athena-Algorithm Constructor */
      TRT_EventPhaseAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
      /** Default Destructor */
      ~TRT_EventPhaseAnalysis();

      /** standard Athena-Algorithm method */
      StatusCode          initialize();
      /** standard Athena-Algorithm method */
      StatusCode          beginRun();
      /** standard Athena-Algorithm method */
      StatusCode          execute();
      /** standard Athena-Algorithm method */
      StatusCode          finalize();

    private:

      struct trackData
      {
	Trk::Track* track;
	int& nTRTBarrelHits;
	int& nTRTEndcapHits;
	int& nSCTHits;
	int& nPixelHits; 
	double& EP;
	double& EPFromTE;
	float& tofFitPar0;
	float& tofFitPar1;
	float& tofChi2OverNdf;
	std::vector<float>* timeDiff;
	std::vector<float>* x; 
	std::vector<float>* y;
	std::vector<float>* z; 
	std::vector<float>* dbT0;
      };
	
      
      StatusCode load();
      
      bool analyzeTrack(Trk::Track* track, int& nTRTBarrelHits, int& nTRTEndcapHits, int& nSCTHits, int& nPixelHits, double& EP, double& EPFromTE, 
			float& tofFitPar0, float& tofFitPar1, float& tofChi2OverNdf, std::vector<float>* timeDiff, 
			std::vector<float>* x, std::vector<float>* y, std::vector<float>* z, std::vector<float>* dbT0);
       
      ///////////////////////////////////////////
      /// Helpers, tools, and date vectors
      ///////////////////////////////////////////
      ServiceHandle<ITHistSvc>     m_histSvc;
      ServiceHandle<ITRT_CalDbSvc> m_trtCondDbTool;
      
      ToolHandle<IInDetTrackSplitterTool>            m_tracksplitter;
      ToolHandle<InDet::IInDetCosmicsEventPhaseTool> m_eventPhaseTool;

      const DataVector<Trk::Track>* m_tracksCollection;
      const TRT_ID *m_trtHelper;

      TTree* m_tree;
      std::string m_tracksName;

      int eventCount;

      /////////////////////////////////////
      /// Variables to be stored in tree
      /////////////////////////////////////
      int m_nTrtBarrelHits_trk;
      int m_nTrtBarrelHits_top;
      int m_nTrtBarrelHits_bot;
      
      int m_nTrtEndcapHits_trk;
      int m_nTrtEndcapHits_top;
      int m_nTrtEndcapHits_bot;

      int m_nSctHits_trk;
      int m_nSctHits_top;
      int m_nSctHits_bot;

      int m_nPixelHits_trk;
      int m_nPixelHits_top;
      int m_nPixelHits_bot;

      double m_EP_trk;
      double m_EP_top;
      double m_EP_bot;
      
      double m_EPFromTE_trk;
      double m_EPFromTE_top;
      double m_EPFromTE_bot;

      float m_tofFitPar0_trk;
      float m_tofFitPar0_top;
      float m_tofFitPar0_bot;
      
      float m_tofFitPar1_trk;
      float m_tofFitPar1_top;
      float m_tofFitPar1_bot;
      
      float m_tofChi2OverNdf_trk;
      float m_tofChi2OverNdf_top;
      float m_tofChi2OverNdf_bot;

      double m_trtPhase;
      double m_d0;
      double m_z0;
      double m_phi;
      double m_theta;

      std::vector<float>* m_x_trk;
      std::vector<float>* m_x_top;
      std::vector<float>* m_x_bot;
      
      std::vector<float>* m_y_trk;
      std::vector<float>* m_y_top;
      std::vector<float>* m_y_bot;
      
      std::vector<float>* m_z_trk;
      std::vector<float>* m_z_top;
      std::vector<float>* m_z_bot;
      
      std::vector<float>* m_timeDiff_trk;
      std::vector<float>* m_timeDiff_top;
      std::vector<float>* m_timeDiff_bot;
      
      std::vector<float>* m_dbT0_trk;
      std::vector<float>* m_dbT0_top;
      std::vector<float>* m_dbT0_bot;


    }; 
} // end of namespace

#endif 
