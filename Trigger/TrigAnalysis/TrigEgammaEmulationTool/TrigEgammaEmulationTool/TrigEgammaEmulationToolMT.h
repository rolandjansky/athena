/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */


#ifndef TrigEgammaEmulationToolMT_H
#define TrigEgammaEmulationToolMT_H


#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationChain.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationInfo.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include "AsgTools/AsgTool.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "ElectronPhotonSelectorTools/AsgElectronSelectorTool.h"



namespace Trig{
  

class TrigEgammaEmulationToolMT: public asg::AsgTool 
{

  ASG_TOOL_INTERFACE( TrigEgammaEmulationToolMT )
  ASG_TOOL_CLASS0( TrigEgammaEmulationToolMT )

  public:

    TrigEgammaEmulationToolMT(const std::string& myname);

    ~TrigEgammaEmulationToolMT()=default;

    StatusCode initialize();

    asg::AcceptData emulate(const TrigCompositeUtils::Decision *dec, std::string trigger) const;
    

  private:

    /* hypo tools */
    ToolHandleArray<Trig::ITrigEgammaEmulationChain> m_hypoTools{ this, "HypoTools", {}, "Hypo tools" };
    
    /* electron selectors */    
    ToolHandleArray<IAsgElectronLikelihoodTool>   m_egammaElectronDNNTools{ this, "ElectronDNNSelectorTools", {},"DNN tools" };
    ToolHandleArray<IAsgElectronIsEMSelector>     m_egammaElectronCBTools{ this, "ElectronCBSelectorTools", {},"Cut-based tools" };
    ToolHandleArray<IAsgElectronLikelihoodTool>   m_egammaElectronLHTools{ this, "ElectronLHSelectorTools", {},"Likelihood tools" };
    
    /* photon selectors */
    ToolHandleArray< IAsgPhotonIsEMSelector >     m_egammaPhotonCBTools { this, "PhotonCBSelectorTools", {} , "Cut-based tools"};

    /* ringer selectors */
    ToolHandleArray<Ringer::IAsgRingerSelectorTool> m_ringerTools{ this, "RingerTools", {}, "Ringer tools" };


    /*! Trigger decision tool */
    ToolHandle<Trig::TrigDecisionTool> m_trigdec;
    /* Trigger e/g matching tool */
    ToolHandle<TrigEgammaMatchingToolMT> m_matchTool;
    /* Trigger Match list for electrons */ 
    Gaudi::Property< std::vector<std::string> > m_trigElectronList { this, "ElectronTriggerList", {} , "" };
    /* Trigger Match list for photons */ 
    Gaudi::Property< std::vector<std::string> > m_trigPhotonList { this, "PhotonTriggerList", {} , "" };
    /*! Accept */
    asg::AcceptInfo m_accept;


    const ToolHandle<TrigEgammaMatchingToolMT>& match() const {return m_matchTool;}

    bool match( const TrigCompositeUtils::Decision *dec , Trig::TrigData & ) const;
    /*! fill all features from electron support chains */
    bool match_electron( const TrigCompositeUtils::Decision *dec , Trig::TrigData & , std::vector<std::string> ) const;
    /*! fill all features from photon support chains */
    bool match_photon( const TrigCompositeUtils::Decision *dec , Trig::TrigData & , std::vector<std::string> ) const;

};

}


#endif
