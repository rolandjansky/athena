/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */


#ifndef TrigEgammaEmulationToolMT_H
#define TrigEgammaEmulationToolMT_H


#include "TrigEgammaMatchingTool/TrigEgammaMatchingToolMT.h"
#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationInfo.h"
#include "TrigEgammaEmulationToolMT/ITrigEgammaEmulationChain.h"
#include "AsgTools/AsgTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "PATCore/AcceptInfo.h"
#include "PATCore/AcceptData.h"
#include <vector>
#include <map>


#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"
#include "RingerSelectorTools/IAsgRingerSelectorTool.h"

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
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronLHTools{ this, "ElectronLHTools", {}, "Likelihood tools" };
    /* photon selectors */
    ToolHandleArray< IAsgPhotonIsEMSelector > m_photonIsEMTools { this, "PhotonIsEMTools", {} };
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
    bool matchElectron( const TrigCompositeUtils::Decision *dec , Trig::TrigData & ) const;
    bool matchPhoton( const TrigCompositeUtils::Decision *dec , Trig::TrigData & ) const;



};
}


#endif
