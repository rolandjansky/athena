/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Alg_SeedBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAU_ALG_SEEDBUILDER_H
#define PANTAU_ALG_SEEDBUILDER_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

// C++ includes
#include <string>
#include <map>
#include <vector>

// PanTau includes
#include "PanTauAlgs/Tool_InformationStore.h"
#include "PanTauAlgs/Tool_TauConstituentGetter.h"
#include "PanTauAlgs/Tool_TauConstituentSelector.h"
#include "PanTauAlgs/Tool_FeatureExtractor.h"
#include "PanTauAlgs/Tool_DecayModeDeterminator.h"
#include "PanTauAlgs/Tool_FourMomentumCalculator.h"
#include "PanTauAlgs/Tool_DetailsArranger.h"

#include "PanTauEvent/PanTauSeedContainer.h"

class StoreGateSvc;



namespace PanTau 
{

  /** @class Alg_SeedBuilder

      @author  Peter Wienemann <peter.wienemann@cern.ch>
      @author  Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>
      @author  Robindra Prabhu <robindra.prabhu@cern.ch>
      @author  Christian Limbach <limbach@physik.uni-bonn.de>
  */  

  class Alg_SeedBuilder : public AthAlgorithm
    {
    public:

       Alg_SeedBuilder(const std::string& name, ISvcLocator* pSvcLocator);
       ~Alg_SeedBuilder();

       StatusCode          initialize();
       StatusCode          execute();
       StatusCode          finalize();

    private:
        
        mutable MsgStream                                   m_log;
        StoreGateSvc*                                       m_sgSvc;
        
        std::string                                         m_Name_InputAlg;
        
        //Tools used in seed building
        ToolHandle<PanTau::ITool_InformationStore>          m_Tool_InformationStore;
        ToolHandle<PanTau::ITool_TauConstituentGetter>      m_Tool_TauConstituentGetter;
        ToolHandle<PanTau::ITool_TauConstituentSelector>    m_Tool_TauConstituentSelector;
        ToolHandle<PanTau::ITool_FeatureExtractor>          m_Tool_FeatureExtractor;
        
        //Tools used in seed finalizing
        ToolHandle<PanTau::ITool_DecayModeDeterminator>     m_Tool_DecayModeDeterminator;
        ToolHandle<PanTau::ITool_FourMomentumCalculator>    m_Tool_FourMomentumCalculator;
        ToolHandle<PanTau::ITool_DetailsArranger>           m_Tool_DetailsArranger;
        
        std::vector<double>                                 m_Config_PtBins;
        double                                              m_Config_MinPt;
        double                                              m_Config_MaxPt;
        
        void                                                fillDefaultValuesToTau(xAOD::TauJet* tauJet);
        
        
    }; //end class
} // end of namespace

#endif 
