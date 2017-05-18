/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TopEventMaker.h 661108 2015-04-17 00:01:16Z morrisj $
#ifndef ANALYSISTOP_TOPEVENT_TOPEVENTMAKER_H
#define ANALYSISTOP_TOPEVENT_TOPEVENTMAKER_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief TopEventMaker
  *   produce top::Event objects from xAOD::SystematicEvent objects
  * 
  * 
  * $Revision: 661108 $
  * $Date: 2015-04-17 02:01:16 +0200 (Fri, 17 Apr 2015) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <vector>

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include "TopEvent/Event.h"
#include "TopEvent/SystematicEvent.h"

namespace top {

  class TopConfig;

  class TopEventMaker final : public asg::AsgTool {
    public:
      explicit TopEventMaker( const std::string& name );
      virtual ~TopEventMaker(){}
      
      // Delete Standard constructors
      TopEventMaker(const TopEventMaker& rhs) = delete;
      TopEventMaker(TopEventMaker&& rhs) = delete;
      TopEventMaker& operator=(const TopEventMaker& rhs) = delete;

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
      top::Event makeTopEvent(const xAOD::SystematicEvent& currentSystematic);
      
      /// As top-xaod isn't an asg::AsgTool, it doesn't have access to all the information
      /// Very annoying
      const xAOD::SystematicEventContainer* systematicEvents(const std::string& sgKey) const;

    private:
      std::shared_ptr<top::TopConfig> m_config;
  };
}
#endif
