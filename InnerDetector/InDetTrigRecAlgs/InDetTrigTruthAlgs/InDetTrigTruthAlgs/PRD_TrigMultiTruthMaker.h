/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//**************************************************************************
/**
 // Algorithm producing truth info for PrepRawData, keeping all MC particles contributed to a PRD.
 // A. Gaponenko, 2006
 // filename: PRD_TrigMultiTruthMaker.h
 //
 // author: Adapted for the Event Filter: Iwona Grabowska-Bold, June 2006
 //         Iwona.Grabowska@cern.ch
 //
 // Description:  Trigger version of the PRD_MultiTruthMaker
 //               (see original package documentation).
 //
 // -------------------------------
 // ATLAS Collaboration
 */
//***************************************************************************


#ifndef PRD_TRIGMULTITRUTHMAKER_H
#define PRD_TRIGMULTITRUTHMAKER_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrkTruthData/PRD_MultiTruthCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"

//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"

namespace InDet { class IPRD_MultiTruthBuilder; }

namespace InDet {

  class PRD_TrigMultiTruthMaker : public HLT::FexAlgo {
  
  public:
    PRD_TrigMultiTruthMaker(const std::string &name,ISvcLocator *pSvcLocator);

    virtual HLT::ErrorCode hltInitialize();
    virtual HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    virtual HLT::ErrorCode hltFinalize();
  
  
  private:
  
    int m_prdPIXsize;
    int m_prdSCTsize;
    int m_prdTRTsize;

    std::string m_PixelClustersName;
    std::string m_SCTClustersName;
    std::string m_TRTDriftCircleContainerName; 
    std::string m_simDataMapNamePixel;
    std::string m_simDataMapNameSCT;
    std::string m_simDataMapNameTRT;

    std::string m_PRDTruthNamePixel;
    std::string m_PRDTruthNameSCT;
    std::string m_PRDTruthNameTRT;

    ToolHandle<InDet::IPRD_MultiTruthBuilder> m_PRDTruthTool;

    bool m_doTruth; //!< flag which governs whether the truth association is done or not


    //----------------------------------------------------------------
    template<class PRD_Container_Iterator>
    void addPRDCollections(PRD_MultiTruthCollection* prdTruth,
			   PRD_Container_Iterator collections_begin,
			   PRD_Container_Iterator collections_end,
			   const InDetSimDataCollection* simDataMap,
			   bool pixels);
    
    template<class PRD_Collection_Iterator>
    void addPRDRange (PRD_MultiTruthCollection* prdTruth,
		      PRD_Collection_Iterator range_begin,
		      PRD_Collection_Iterator range_end,
		      const InDetSimDataCollection* simDataMap,
		      bool pixels);

  };

} // end of namespace
#endif //PRD_MULTITRUTHMAKER_H
