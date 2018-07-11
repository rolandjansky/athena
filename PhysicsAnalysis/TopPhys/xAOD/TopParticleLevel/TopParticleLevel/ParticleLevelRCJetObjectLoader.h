/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Load the reclustering tool for the collection of truth jets already selected.
//Create the collection of reclustered jets and save them in the evtStore().
//There is also the pass slection to apply the cut on eta and pt on the reclustered jets.

#ifndef ParticleLevelRCJetObjectLoader_H_
#define ParticleLevelRCJetObjectLoader_H_


#include "TopParticleLevel/ObjectSelectorBase.h"
#include "AsgTools/AsgTool.h"
#include "xAODCore/ShallowCopy.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#include "xAODRootAccess/TActiveStore.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "JetReclustering/JetReclusteringTool.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <set>
#include <list>
#include <vector>


namespace top{
    class ParticleLevelEvent;
    class TopConfig;
}


class ParticleLevelRCJetObjectLoader: public asg::AsgTool {
public:
    
    
    //Default - so root can load based on a name
    ParticleLevelRCJetObjectLoader( const std::shared_ptr<top::TopConfig> & cfg ) ;
   

    //Default - so we can clean up
    ~ParticleLevelRCJetObjectLoader();

    //Run once at the start of the job
    StatusCode initialize();

    //Run for every event (in every systematic) that needs saving
    StatusCode execute(const top::ParticleLevelEvent& plEvent);


    //Clean-up remaining things
    StatusCode finalize();

    bool passSelection(const xAOD::Jet& jet) const;
    
    //Access all of the container names from the Event Saver
     std::string inputContainerName(){return m_InputJetContainer; };
     std::string rcjetContainerName(){return m_OutputJetContainer; };
    
private:

    std::string m_name;
    const std::shared_ptr<top::TopConfig> & m_config;

    bool m_VarRCjets;
    std::string m_VarRCjets_rho;
    std::string m_VarRCjets_mass_scale;

    float m_ptcut;       // in GeV
    float m_etamax;
    float m_trim;
    float m_radius;
    float m_minradius;
    float m_massscale;
    
    std::string m_treeName;

    std::string m_InJetContainerBase;
    std::string m_OutJetContainerBase;
    std::string m_InputJetContainer;
    std::string m_OutputJetContainer;
   
    std::map<std::string,float> mass_scales = {
        {"m_t",172500.},
        {"m_w",80385.},
        {"m_z",91188.},
        {"m_h",125090.}};

    //re-clustered jets
    //  -> need unordered map for systematics
    JetReclusteringTool* m_jetReclusteringTool;
};

#endif
