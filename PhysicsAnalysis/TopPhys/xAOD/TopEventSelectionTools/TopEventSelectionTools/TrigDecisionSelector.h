/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGDECISIONSELECTOR_H_
#define TRIGDECISIONSELECTOR_H_

#include "TopEventSelectionTools/EventSelectorBase.h"
#include <memory>


namespace top {
  // Forward declare
  class Event;   
  class TopConfig;
  /**
  * @brief Sometimes you may want to select events that pass a certain trigger,
  * or triggers.
  *
  * This code only accepts the event if it passes at least one of the triggers
  * that it was configured with.  It does an 'OR' of all the trigger decisions.
  *
  * For example:
  *
  * @code
  * TRIGDEC HLT_e24_tight_L1EM20V HLT_e60_medium1
  * @endcode
  *
  * Would select the event if either of those passes.
  */
  class TrigDecisionSelector : public EventSelectorBase {
    public:
      /**
      * @brief A tool to select events that pass a certain trigger (or any
      * trigger in a list).
      *
      * @param name The name of the current event selection - e.g. ee
      * @param config - this will give use a std::vector<std::string> of triggers to check
      */
      TrigDecisionSelector(const std::string& selectorName,std::shared_ptr<top::TopConfig> config);

      /**
      * @brief Select events based on the 'OR' of a few user defined triggers.
      * The thinking is you might want to pass e25i or e30 so "TRIGDEC e25i e30".
      *
      * @param event The event in question.  Not used by the trigger decision
      * tool.  I guess it finds out the information some other, internal, way.
      *
      * @return True if at least one of the triggers in the string list passed.
      */
      virtual bool apply(const top::Event& event) const override;

      ///TRIGDEC because we might have a tool for trigger matching too at some point
      std::string name() const override;

    private:
      ///List of triggers to 'or' together for each event. If any one passes, the event passes
      std::vector<std::string> m_triggers;
  };
}

#endif /* TRIGDECISIONSELECTOR_H_ */
