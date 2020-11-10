/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EVENTSAVERBASE_H_
#define EVENTSAVERBASE_H_

#include <vector>
#include <string>
#include <memory>

#include <TClass.h>

class TFile;

namespace top {
  class Event;
  class TreeManager;
  class TopConfig;
  class ParticleLevelEvent;

  /**
   * @brief A base class so users can write their own event saving stuff. Some
   * people, for some reason, prefer flat ntuples to xAODs.
   */
  class EventSaverBase {
  public:
    /**
     * @brief Default ctor/dtor - ROOT wants to use these, don't use fancy ctors
     */
    EventSaverBase() {}
    virtual ~EventSaverBase() {}

    /**
     * @brief Called once at the start of the job after the file has been opened.
     *
     * @param config You may need stuff from the configuration.
     * @param file You probably need the file if you're going to write to it.
     * @param extraBranches So you can write extra branches (only for xAOD).
     */
    virtual void initialize(std::shared_ptr<top::TopConfig> config, TFile* file,
                            const std::vector<std::string>& extraBranches) = 0;

    /**
     * @brief Method to execute any calculations common for
     * reco/particle/parton level.
     */
    virtual void execute() {return;}

    /**
     * @brief I guess you want to save an event sometimes to.
     *  There are multiple ways you might want to do this
     *  All methods below are implemented with a simple return
     *  This means that you do not need to implement them in
     *  your derived class
     *
     * @param event The event in question, in the format used internally.
     */
    virtual void saveEvent(const top::Event& /*event*/) {return;}
    virtual void saveEventToxAOD() {return;}

    /**
     * @breif Run for every event
     *
     * This fills, if requested, the:
     *   MC truth record
     *   PDF info
     *   TopPartons
     */
    virtual void saveTruthEvent() {return;}

    /*!
     * @brief Function to save the particle level event.
     *  This function has a default implementation that does ... nothing! This
     *  is deliberate since doing so does not force developers of custom
     *  EventSaver tools to implement particle level saving.
     * @param plEvent The particle level event whose contents will be saved.
     */
    virtual void saveParticleLevelEvent(const top::ParticleLevelEvent& /* plEvent */) {
      /* Deliberately No-Op */
      return;
    }

    /*!
     * @brief Function to save the upgrade event.
     *  This function has a default implementation that does ... nothing! This
     *  is deliberate since doing so does not force developers of custom
     *  EventSaver tools to implement upgrade saving.
     * @param upgradeEvent The upgrade event whose contents will be saved.
     */
    virtual void saveUpgradeEvent(const top::ParticleLevelEvent& /* plEvent */) {
      /* Deliberately No-Op */
      return;
    }

    /**
     * @brief So you can write something at the end of the job (like metadata).
     */
    virtual void finalize() {return;}

    ClassDef(top::EventSaverBase, 0);
  };
}

#endif
