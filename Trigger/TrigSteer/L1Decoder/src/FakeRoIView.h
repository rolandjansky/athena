/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * Author: Piotr Sarna
 */

#ifndef L1Decoder_FakeRoIView_h
#define L1Decoder_FakeRoIView_h

#include "AthenaBaseComps/AthAlgorithm.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigT1Result/RoIBResult.h"

//#include "AthViews/AthViewAlgorithm.h"
#include "AthViews/View.h"
//#include "EventInfo/EventInfo.h"

namespace AthViews {

  class FakeRoIView
    : public ::AthAlgorithm 
    {
    public:
      FakeRoIView(const std::string& name, ISvcLocator* pSvcLocator);
      virtual StatusCode initialize() override;
      virtual StatusCode start() override;
      virtual StatusCode execute() override;
      virtual StatusCode finalize() override;
      
    private:
      
      /// Default constructor: 
      FakeRoIView();

      struct SingleFakeRoIView {
	double eta;
	double phi;
	uint32_t word;
	std::vector<std::string> passedThresholdIDs;
      };
      
      std::vector<std::vector<SingleFakeRoIView>> parseInputFile();
      std::vector<SingleFakeRoIView> parseInputFileLine(const std::string& line, unsigned lineNumber);
      SingleFakeRoIView parseInputRoI(const std::string& roi, unsigned lineNumber, unsigned roiNumber);
      
      std::vector<TrigConf::TriggerThreshold*> m_emtauThresholds;
      
      SG::WriteHandle< TrigRoiDescriptorCollection > m_trigRoIs;
      SG::WriteHandle< DataVector<LVL1::RecEmTauRoI> > m_recEMTauRoIs;
      SG::WriteHandle< xAOD::TrigCompositeContainer > m_decisions;
      SG::WriteHandle< xAOD::TrigCompositeAuxContainer > m_decisionsAux;

      SG::WriteHandle< std::vector<SG::View*> > m_views;
      std::string  m_viewName;
      //      std::vector< std::string > m_viewNames;
      ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc; //!< access to trigger menu
      
      // can demand objects 
      
      std::string m_inputFilename;
      std::vector<std::vector<FakeRoIView::SingleFakeRoIView>> m_inputData;
      unsigned m_currentRowNumber;
    };
  
}//> end namespace AthViews
#endif
