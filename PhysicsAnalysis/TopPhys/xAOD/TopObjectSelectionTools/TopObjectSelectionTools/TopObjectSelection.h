/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopObjectSelection.h 810745 2017-09-29 14:03:01Z iconnell $
#ifndef ANALYSISTOP_TOPOBJECTSELECTONTOOLS_TOPOBJECTSELECTION_H
#define ANALYSISTOP_TOPOBJECTSELECTONTOOLS_TOPOBJECTSELECTION_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief TopObjectSelection
  *   Selects top objects according to the ObjectLoaderBase type
  * 
  * $Revision: 810745 $
  * $Date: 2017-09-29 15:03:01 +0100 (Fri, 29 Sep 2017) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <vector>

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Top include(s):
#include "TopObjectSelectionTools/ElectronSelectionBase.h"
#include "TopObjectSelectionTools/MuonSelectionBase.h"
#include "TopObjectSelectionTools/JetSelectionBase.h"
#include "TopObjectSelectionTools/TauSelectionBase.h"
#include "TopObjectSelectionTools/PhotonSelectionBase.h"
#include "TopObjectSelectionTools/OverlapRemovalBase.h"
#include "TopSystematicObjectMaker/ElectronInJetSubtractionCollectionMaker.h"

// boosted-tagging includes
#include "BoostedJetTaggers/SmoothedTopTagger.h"
#include "BoostedJetTaggers/SmoothedWZTagger.h"

// b-tagging
#include "FTagAnalysisInterfaces/IBTaggingSelectionTool.h"
#include "TopEvent/Event.h"

// forward declare
namespace xAOD{
  class SystematicEvent;
}

namespace top {
class TopConfig;

/**
 * @brief Configure the object selection used in the analysis.  The user should
 * be able to configure this, or if they really really want to, then overload it
 * with a class based on TopObjectSelection and load that at run time. Crazy.
 */
class TopObjectSelection : public asg::AsgTool {
public:
    /**
     * @brief Setup the object selection at the start of the run.
     *
     * @param config We need the configuration object because it contains a lot
     * of information about what the user wants (which containers to load for
     * example).  Note that it's not a smart pointer (yet) because of root5
     * restrictions on dynamic class loading.
     * 
     */
    explicit TopObjectSelection( const std::string& name );
    virtual ~TopObjectSelection(){}

    // Delete Standard constructors
    TopObjectSelection(const TopObjectSelection& rhs) = delete;
    TopObjectSelection(TopObjectSelection&& rhs) = delete;
    TopObjectSelection& operator=(const TopObjectSelection& rhs) = delete; 
    
    StatusCode initialize();

    /**
     * @brief Code that runs for every event (note that it runs the selection
     * on all the systematic variations too).
     *
     * @param topEvt The top event containing all the systematic variations.
     */
    StatusCode execute();

    /**
     * @brief Set the code used to select electrons.
     *
     * Note that nullptr means that no selection will be applied (so all
     * electrons will be accepted).
     *
     * @param ptr The code used to perform the electron selection (see
     * TopObjectSelectionTools).
     */
    void electronSelection(ElectronSelectionBase* ptr);

    /**
     * @brief Set the code used to select muons.
     *
     * Note that nullptr means that no selection will be applied (so all
     * muons will be accepted).
     *
     * @param ptr The code used to perform the muon selection (see
     * TopObjectSelectionTools).
     */
    void muonSelection(MuonSelectionBase* ptr);
    
     /**
     * @brief Set the code used to select taus.
     *
     * Note that nullptr means that no selection will be applied (so all
     * muons will be accepted).
     *
     * @param ptr The code used to perform the muon selection (see
     * TopObjectSelectionTools).
     */
    void tauSelection(TauSelectionBase* ptr);   

    /**
     * @brief Set the code used to select jets.
     *
     * Note that nullptr means that no selection will be applied (so all
     * jets will be accepted).
     *
     * @param ptr The code used to perform the jets selection (see
     * TopObjectSelectionTools).
     */
    void jetSelection(JetSelectionBase* ptr);

    /**
     * @brief Set the code used to select photons.
     *
     * Note that nullptr means that no selection will be applied (so all
     * photons will be accepted).
     *
     * @param ptr The code used to perform the photon selection (see
     * TopObjectSelectionTools).
     */
    void photonSelection(PhotonSelectionBase* ptr);
    

    /**
     * @brief Set the code used to select large jets.
     *
     * Note that nullptr means that no selection will be applied (so all
     * large jets will be accepted).
     *
     * @param ptr The code used to perform the large jets selection (see
     * TopObjectSelectionTools).
     */
    void largeJetSelection(JetSelectionBase* ptr);
    
    /**
     * @brief Set the code used to select track jets.
     *
     * Note that nullptr means that no selection will be applied (so all
     * track jets will be accepted).
     *
     * @param ptr The code used to perform the track jets selection (see
     * TopObjectSelectionTools).
     */
    void trackJetSelection(JetSelectionBase* ptr);    

    /**
     * @brief Set the code used to perform the overlap removal.  At the moment
     * the overlap removal is performed after all the electron, muon, jet etc
     * selections are performed.
     *
     * @param ptr The overlap removal code to be used.
     */
    void overlapRemovalPostSelection(OverlapRemovalBase* ptr);
    void overlapRemovalPostSelectionTrackJets(OverlapRemovalBase* ptr);

    /**
     * @brief Print details of this object selection to wherever the user asks
     * for.  Useful for checking cuts in the log files I hope!
     *
     * @param os Where you would like the output printing to, presumably std::cout?
     */
    virtual void print(std::ostream& os) const;
    
    /**
     * @brief or prehaps you'd like the AsgTool print function
     */
    virtual void print() const;

private:
    void applySelectionPreOverlapRemoval();
    void applySelectionPreOverlapRemovalPhotons();
    void applySelectionPreOverlapRemovalElectrons();
    void applySelectionPreOverlapRemovalMuons();
    void applySelectionPreOverlapRemovalTaus();
    void applySelectionPreOverlapRemovalJets();
    void applySelectionPreOverlapRemovalLargeRJets();
    void applySelectionPreOverlapRemovalTrackJets();
    
    virtual StatusCode applyOverlapRemoval();  
    virtual StatusCode applyOverlapRemoval(const bool isLoose,const std::string& sgKey);
    virtual StatusCode applyOverlapRemoval(xAOD::SystematicEvent* currentSystematic);
  
    void applyTightSelectionPostOverlapRemoval( const xAOD::IParticleContainer* xaod , std::vector<unsigned int>& indices );
    void trackJetOverlapRemoval(const  xAOD::IParticleContainer* xaod_el, 
                                const  xAOD::IParticleContainer* xaod_mu,
                                const  xAOD::IParticleContainer* xaod_tjet,
                                std::vector<unsigned int>& goodElectrons,
                                std::vector<unsigned int>& goodMuons,
                                std::vector<unsigned int>& goodTrackJets);  

    void decorateMuonsPostOverlapRemoval(const xAOD::MuonContainer* xaod_mu,
					 const xAOD::JetContainer* xaod_jet,
					 std::vector<unsigned int>& goodMuons,
					 std::vector<unsigned int>& goodJets);
  
    /**
     * @brief Pointer to the configuration object so we can check which objects
     * were requested in the config file.
     */
    std::shared_ptr<top::TopConfig> m_config;

    ///Electron selection code - can load user defined classes
    std::unique_ptr<top::ElectronSelectionBase> m_electronSelection;

    ///Muon selection code - can load user defined classes
    std::unique_ptr<top::MuonSelectionBase> m_muonSelection;

    ///Tau selection code - can load user defined classes
    std::unique_ptr<top::TauSelectionBase> m_tauSelection;

    ///Jet selection code - can load user defined classes
    std::unique_ptr<top::JetSelectionBase> m_jetSelection;

    ///Photon selection code - can load user defined classes                                                                                                         
    std::unique_ptr<top::PhotonSelectionBase> m_photonSelection;


    ///Large-R jet selection code - can load user defined classes
    std::unique_ptr<top::JetSelectionBase> m_largeJetSelection;
    
    ///Track jet selection code - can load user defined classes
    std::unique_ptr<top::JetSelectionBase> m_trackJetSelection;    

    ///Overlap removal that runs after all object selection
    std::unique_ptr<top::OverlapRemovalBase> m_overlapRemovalToolPostSelection;
    
    //Electron In Jet Subtraction
    std::unique_ptr<top::ElectronInJetSubtractionCollectionMaker> m_electronInJetSubtractor;
    
    // Pass selection strings
    const std::string m_passPreORSelection;
    const std::string m_passPreORSelectionLoose;
    // the following two are used to give failing JVT jets a lower priority in the OR
    const std::string m_ORToolDecoration;
    const std::string m_ORToolDecorationLoose;
    
    // Do we need to request loose lepton selection
    bool m_doLooseCuts;
    
    // do decorate the jets with the b-tagging flags
    std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool>> m_btagSelTools;
    std::unordered_map<std::string, ToolHandle<IBTaggingSelectionTool>> m_trkjet_btagSelTools;
    
    // do decorate the large-R jets with the boosted-tagging flags
    ToolHandle<SmoothedTopTagger> m_topTag50;
    ToolHandle<SmoothedTopTagger> m_topTag80;
    ToolHandle<SmoothedWZTagger>  m_WTag50;
    ToolHandle<SmoothedWZTagger>  m_WTag80;
    ToolHandle<SmoothedWZTagger>  m_ZTag50;
    ToolHandle<SmoothedWZTagger>  m_ZTag80;
};
}
#endif
