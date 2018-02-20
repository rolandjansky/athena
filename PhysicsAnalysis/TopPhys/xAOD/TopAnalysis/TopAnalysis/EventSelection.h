/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTION_H_
#define EVENTSELECTION_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include "TopEventSelectionTools/ToolLoaderBase.h"
#include "TopCorrections/ScaleFactorRetriever.h"

#include <string>
#include <vector>
#include <memory>

class TH1D;
class TFile;

namespace EL {
  class Worker;
}

namespace top {
  class Event;
  class TopConfig;
  class ParticleLevelEvent;
}

namespace top {
/**
 * @brief Perform the event selection on a top::Event object.
 *
 * Class to perform the event selection and keep track of the number
 * of events passing each cut.  It is also responsible for writing out the
 * 4-vectors in the output file if the user so wished.
 */
class EventSelection final {
public:
    /**
     * @brief Construct using a vector of cuts.
     *
     * The user supplies a simple vector of string that contains the cuts that
     * should be applied.  The tool will try to load each cut (and quit with
     * and error message if it can't).
     *
     * It also sets up a histogram which contains all the cuts on the x-axis
     * and each bin is the number of events passing a cut (in order).
     */
    EventSelection(const std::string& name, const std::vector<std::string>& cutNames, TFile* outputFile, const std::vector<std::unique_ptr<top::ToolLoaderBase>>& toolLoaders,std::shared_ptr<top::TopConfig> config, EL::Worker* wk);

    /**
     * @brief Does not do anything
     */
    virtual ~EventSelection() {}

    /**
     * @brief Move constructor
     */
    EventSelection(EventSelection&& other);

    /**
     * @brief Default constructor is not allowed
     */
    EventSelection() = delete;

    /**
     * @brief Copy is not allowed
     */
    EventSelection(const EventSelection&) = delete;

    /**
     * @brief Assignment is not allowed.
     */
    EventSelection& operator=(const EventSelection&) = delete;
    
     /**
     * @brief Count the number of initial events
     */
    virtual void countInitial(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const;   
     /**
     * @brief Count the number of events passing GRL
     */
    virtual void countGRL(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const;  
     /**
     * @brief Count the number of events passing Good Calo
     */
    virtual void countGoodCalo(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const;         
    /**
     * @brief Count the number of events passing Primary Vertex
     */
    virtual void countPrimaryVertex(const float mcEventWeight,const float pileupWeight,const float zvtxWeight) const;
    
    /**
     * @brief Apply the selection for each event.
     *
     * This loops through all the configured cuts in order and looks to see if
     * the top::Event passes them or not.  It fills the cutflow histogram if
     * the events passes.
     *
     * @param event This is not modified by the cuts so is const.  In the event
     * loop each event is passed to this function and the cuts are applied and
     * the results recorded.
     * @return If the event passes all cuts, or at least all those before SAVE
     * then true will be returned - which means that this event passes
     * the selection - and false otherwise. The selection flag will be set accordingly.
     * In case of true if SAVE is present then the event will be saved.
     */
    virtual bool apply(const top::Event& event) const;

    /*!
     * @brief Apply the selection for each particle level event.
     *
     * This loops through all the cuts configured for reco (nominal) level to
     * see if the top::ParticleLevelEvent passes them or not. It also fills the
     * particle level cutflow histogram corresponding to the stage up to which
     * the event passes through the event selection.
     *
     * @param plEvent This is the particle level event which is in question. It
     * will not be modified by any of the cuts.
     * @return If the event passes all cuts, or at least all those before SAVE
     * then true will be returned - which means that this event passes
     * the selection - and false otherwise. The selection flag will be set accordingly.
     * In case of true if SAVE is present then the event will be saved.
     */
    virtual bool applyParticleLevel(const top::ParticleLevelEvent& plEvent) const;

    /*!
     * @brief Apply the selection for each upgrade level event.
     *
     * This loops through all the cuts configured for reco (nominal) level to
     * see if the top::ParticleLevelEvent passes them or not. It also fills the
     * upgrade level cutflow histogram corresponding to the stage up to which
     * the event passes through the event selection.
     *
     * @param plEvent This is the upgrade level event which is in question. It
     * will not be modified by any of the cuts.
     * @return If the event passes all cuts, or at least all those before SAVE
     * then true will be returned - which means that this event passes
     * the selection - and false otherwise. The selection flag will be set accordingly.
     * In case of true if SAVE is present then the event will be saved.
     */
    virtual bool applyUpgradeLevel(const top::ParticleLevelEvent& plEvent) const;

    /**
     * @brief Print some yield information.
     *
     * This is not done in the destructor because we might not have a file
     * open at that point in time.
     */
    virtual void finalise() const;

    /**
     * @brief Tells you the name of this event selection.
     *
     * @return The name of this event selection
     */
    const std::string name() const;

    /**
     * @brief Gives you the lists of the Fakes MM configurations associated to this selection.
     *
     * @return The list of configurations
     */
    std::vector<std::string> GetFakesMMConfigs() const;
    
    /**
     * @brief Tells if the event should be saved if it passes this selection
     *
     * @return If one of the cuts is called SAVE, returns true
     *
     */
    bool ToBeSaved() const {return m_toBeSaved;};

private:
    /**
     * @brief Print a cutflow showing the number of events passing each cut.
     */
    virtual void printCuts();

    ///Vector of all the tools that will be used in the analysis.
    std::vector<std::unique_ptr<EventSelectorBase>> m_allCuts;

    ///The cutflow histogram filled by the tool.
    mutable TH1D* m_cutflow;
    mutable TH1D* m_cutflow_Loose;

    ///Cutflow counting MC weights instead of events
    mutable TH1D* m_cutflowMCWeights;
    mutable TH1D* m_cutflowMCWeights_Loose;

    ///Cutflow counting PileUp weights instead of events
    mutable TH1D* m_cutflowPUWeights;
    mutable TH1D* m_cutflowPUWeights_Loose;

    ///Cutflow counting ZVtx weights instead of events
    mutable TH1D* m_cutflowZVtxWeights;
    mutable TH1D* m_cutflowZVtxWeights_Loose;

    ///Cutflow counting MC*Pileup weights instead of events
    mutable TH1D* m_cutflowMCPUWeights;
    mutable TH1D* m_cutflowMCPUWeights_Loose;

    ///Cutflow counting MC*Pileup*ZVtx weights instead of events
    mutable TH1D* m_cutflowMCPUZVtxWeights;
    mutable TH1D* m_cutflowMCPUZVtxWeights_Loose;

    ///Cutflow counting ScaleFactors instead of events
    mutable TH1D* m_cutflowScaleFactors;
    mutable TH1D* m_cutflowScaleFactors_Loose;

    ///Cutflow counting b-tagging scale factors instead of events
    mutable TH1D* m_cutflowBScaleFactors;
    mutable TH1D* m_cutflowBScaleFactors_Loose;

    ///The particle level cutflow histogram filled by the tool.
    mutable TH1D* m_cutflowParticleLevel;
    mutable TH1D* m_cutflowParticleLevelMCWeights;

    ///The upgrade level cutflow histogram filled by the tool.
    mutable TH1D* m_cutflowUpgradeLevel;


    /**
     * @brief Name of this selection (you can have multiple selections run in
     * the same job
     */
    std::string m_name;

    ///To turn on/off confusing messages about mc weights when running on data
    mutable bool m_isMC;

    ///
    std::shared_ptr<TopConfig> m_config;

    ///Scale factors                                                                                                                                                 
    std::unique_ptr<ScaleFactorRetriever> m_sfRetriever;

    // Nominal hash value
    std::size_t m_nominalHashValue;   
    
    // Does this selection contain initial and GRL?
    // If so, what are their positions in the cut flow?
    bool m_containsInitial;
    bool m_containsGRL;
    bool m_containsGoodCalo;
    bool m_containsPrimaryVertex;
    unsigned int m_positionInitial;
    unsigned int m_positionGRL;
    unsigned int m_positionGoodCalo;
    unsigned int m_positionPrimaryVertex;
    
    // If true, event will be saved if it passes this selection
    // If false, event will only be saved if it passes another selection
    // If an event is saved, the flags of all selections (those passed and those not passed) are saved
    bool m_toBeSaved;
};

}

#endif
