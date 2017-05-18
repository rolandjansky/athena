/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTIONMANAGER_H_
#define EVENTSELECTIONMANAGER_H_

#include <vector>
#include <memory>

#include "TopConfiguration/SelectionConfigurationData.h"
#include "TopAnalysis/EventSelection.h"

class TFile;

namespace EL {
  class Worker;
}

namespace xAOD{
  class SystematicEvent;
}

namespace top {
  class Event;
  class TopConfig;
  class ParticleLevelEvent;
}

namespace top{
/**
 * @brief Maybe you want to run multiple selections (e+jets, mu+jets) on the
 * same input files at the same time.  This class helps.
 *
 * I've heard a rumour that we'll only have a single trigger stream in 2015.
 * I guess this means that it'd be useful to be able to run multiple, orthogonal,
 * selections at the same time on the same input files.  You could imagine
 * running ee, mumu, emu, e+jets and mu+jets.
 */
class EventSelectionManager {
public:
    /**
     * @brief Loads the file and initialises all the relevant tools.
     *
     * @param selectionConfigData Vector of SelectionConfigurationData which
     * contains the name of the event selection and a vector of cuts saved in
     * string format
     * @param outputFile Output file for attaching plots to, etc.
     */
    explicit EventSelectionManager(const std::vector<SelectionConfigurationData>& selectionConfigData, TFile* outputFile, const std::string& toolLoaderNames, std::shared_ptr<top::TopConfig> config,EL::Worker* wk = nullptr);

    /**
     * @brief Does not need to do anything
     */
    virtual ~EventSelectionManager() {}

    /**
     * @brief Move constructor
     */
    EventSelectionManager(EventSelectionManager&& other);

    EventSelectionManager() = delete;
    EventSelectionManager(const EventSelectionManager& rhs) = delete;
    EventSelectionManager& operator=(const EventSelectionManager& rhs) = delete;
    
    /**
     * @brief Count the number of initial events
     */
    virtual void countInitial(const float mcEventWeight,const float pileupWeight,const float zvtxWeight);
    /**
     * @brief Count the number of events passing GRL
     */
    virtual void countGRL(const float mcEventWeight,const float pileupWeight,const float zvtxWeight);  
    /**
     * @brief Count the number of events passing Good Calo
     */
    virtual void countGoodCalo(const float mcEventWeight,const float pileupWeight,const float zvtxWeight); 
    /**
     * @brief Count the number of events passing Primary Vertex
     */
    virtual void countPrimaryVertex(const float mcEventWeight,const float pileupWeight,const float zvtxWeight);

    /**
     * @brief Run through the event selections for each event.
     *
     * For each event loop through all the selections.  If any selection wants
     * to save the event then return true at the end. Otherwise, just the event
     * level cut code is run.
     *
     * At the request of Danilo Enoque Ferreira de Lima it now attaches (to the
     * EventInfo object) a variable for each selection being run with a bool for
     * pass / fail.
     *
     * @param event The event in question
     * @return Returns true if the event passes at least one selection for which
     * SAVE is requested (for saving events to trees).
     */
    virtual bool apply(top::Event& event,const xAOD::SystematicEvent& currentSystematic);

    /*!
     * @brief Execute the event selection using the Particle Level data.
     *
     * This uses the same event selection objects as the reco level (nominal)
     * selection. The function is almost a carbon copy of the apply(top::Event&)
     * variant.
     *
     * @param plEvent The particle level event in question.
     * @return Returns true if the event passes at least one selection for which
     * SAVE is requested (for saving events to trees).
     */
    virtual bool applyParticleLevel(const top::ParticleLevelEvent& plEvent);

    /*!
     * @brief Execute the event selection using the Upgrade Level data.
     *
     * This uses the same event selection objects as the reco level (nominal)
     * selection. The function is almost a carbon copy of the apply(top::Event&)
     * variant.
     *
     * @param upgradeEvent The upgrade level event in question.
     * @return Returns true if the event passes at least one selection for which
     * SAVE is requested (for saving events to trees).
     */
    virtual bool applyUpgradeLevel(const top::ParticleLevelEvent& upgradeEvent);

    /**
     * @brief Call finalise on each tool.
     *
     * Not done in the destructor because these things (might) be copied around
     * in memory by some calls.  This is only called once, just before the
     * destructor at the end of the program.
     */
    virtual void finalise();

    /**
     * @brief To get the mini xAOD really small we need to tell it which
     * branches to save
     *
     * For tiny files we need to tell the code exactly which branches to write-
     * out.  This adds a branch for each selection saying if the event passed
     * or failed that selection.
     *
     * @param extraBranchList A vector of strings that is added to.  A branch
     * for each selection is added (this is stored in event info).
     */
    virtual void addExtraBranches(std::vector<std::string>& extraBranchList);
    
    /**
     * @brief Gives you the lists of the Fakes MM configurations associated to a selection.
     *
     * @return The list of configurations associated to the requested selection
     *
     * @param selection The requested selection.
     */
    std::vector<std::string> GetFakesMMConfigs(std::string selection) const;

private:
    ///A vector of EventSelection objects, for doing fancy things
    std::vector<top::EventSelection> m_selections;
};

}

#endif
