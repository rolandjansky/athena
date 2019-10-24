/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEGAMMAMONITORING_TRIGEGAMMAMONITORALGORITHM_H
#define TRIGEGAMMAMONITORING_TRIGEGAMMAMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoring/Monitored.h"

#include "TrigEgammaAnalysisTools/ITrigEgammaAnalysisBaseTool.h"
#include "TrigEgammaAnalysisTools/IMonitoringGroupLookup.h"

class TrigEgammaMonitorAlgorithm : public extends<AthMonitorAlgorithm,IMonitoringGroupLookup> {
    public:
        TrigEgammaMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~TrigEgammaMonitorAlgorithm();
        virtual StatusCode initialize() override;
        virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

        // IMonitoringGroupLookup implementation
        ToolHandle<GenericMonitoringTool> findGroup( const std::string& name ) const override;

    private:
        ///Name of the trigger items to be monitored.
        //std::vector<std::string> m_trigItems;
        // Using the new way to declare JO properties: Gaudi::Property<int> m_myProperty {this,"MyProperty",0};
        ToolHandleArray< ITrigEgammaAnalysisBaseTool > m_tools {this,"Tools",{}}; ///< Array of Egamma Monitoring Tools
        /*! Is monitoring trigger chain specific default True */
        bool m_isChainSpecific;

};
#endif
