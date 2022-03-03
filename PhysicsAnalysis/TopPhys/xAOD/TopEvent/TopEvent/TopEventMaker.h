/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ANALYSISTOP_TOPEVENT_TOPEVENTMAKER_H
#define ANALYSISTOP_TOPEVENT_TOPEVENTMAKER_H

// system include(s):
#include <memory>
#include <vector>

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include "TopEvent/Event.h"
#include "TopEvent/SystematicEvent.h"
#include "TopEvent/SystematicEventContainer.h"
#include "TopEvent/RCJet.h"


class RCJet;

namespace top {
  class TopConfig;


  /**
   * @author John Morris <john.morris@cern.ch>
   *
   * @brief TopEventMaker
   *   produce top::Event objects from xAOD::SystematicEvent objects
   **/
  class TopEventMaker final: public asg::AsgTool {
  public:
    explicit TopEventMaker(const std::string& name);
    virtual ~TopEventMaker() {}
    StatusCode initialize();

    // Delete Standard constructors
    TopEventMaker(const TopEventMaker& rhs) = delete;
    TopEventMaker(TopEventMaker&& rhs) = delete;
    TopEventMaker& operator = (const TopEventMaker& rhs) = delete;

    /**
     * @brief Make a top::Event from a xAOD::SystematicEvent
     *
     * A SingleSystEvent contains only access to calibrated/systematic
     * StoreGate keys and vectors telling it which objects in a collection
     * are good
     *
     * You want to do physics on a top::Event
     *
     * @param systEvent Essentially this make an analysis friendly version of
     * the information in systEvent
     *
     * @return A top::Event that you can do analysis on containing the good
     * objects, which you can then perform selection based on event-level info
     * with.  Such as object multiplicity. Fun times are ahead.
     */
    top::Event makeTopEvent(const xAOD::SystematicEvent* currentSystematic);

    /// As top-xaod isn't an asg::AsgTool, it doesn't have access to all the information
    /// Very annoying
    const xAOD::SystematicEventContainer* systematicEvents(const std::string& sgKey) const;
  private:

    void decorateTopEvent(top::Event &event);
    void decorateTopEventSoftMuons(top::Event &event);

    std::shared_ptr<top::TopConfig> m_config;
    std::unique_ptr<RCJet> m_rc;
    std::map<std::string, std::unique_ptr<RCJet> > m_VarRC;
    std::vector<std::string> m_VarRCJetRho;
    std::vector<std::string> m_VarRCJetMassScale;

    static bool s_hasTruthEvent; // one-time check at first event we loop over
  };
}
#endif
