/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

// $Id: MissingETObjectCollectionMaker.h 683191 2015-07-15 16:05:25Z morrisj $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MISSINGETOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_MISSINGETOBJECTCOLLECTIONMAKER_H

/**
 * @author John Morris <john.morris@cern.ch>
 *
 * @brief MissingETObjectCollectionMaker
 *   Makes all systematic variations of Missing ET
 *
 * $Revision: 683191 $
 * $Date: 2015-07-15 17:05:25 +0100 (Wed, 15 Jul 2015) $
 *
 **/

// system include(s):
#include <memory>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"

namespace xAOD {
  class SystematicEvent;
}

namespace top {
  class TopConfig;
}

namespace top {
  class MissingETObjectCollectionMaker final: public asg::AsgTool {
  public:
    explicit MissingETObjectCollectionMaker(const std::string& name);
    virtual ~MissingETObjectCollectionMaker() {}

    // Delete Standard constructors
    MissingETObjectCollectionMaker(const MissingETObjectCollectionMaker& rhs) = delete;
    MissingETObjectCollectionMaker(MissingETObjectCollectionMaker&& rhs) = delete;
    MissingETObjectCollectionMaker& operator = (const MissingETObjectCollectionMaker& rhs) = delete;

    StatusCode initialize();
    StatusCode recalculateMET(bool);

    // return specific Systematic
    inline virtual const std::list<CP::SystematicSet>& specifiedSystematics() const {return m_specifiedSystematics;}

    // return all recommendedSystematics
    inline const std::list<CP::SystematicSet>& recommendedSystematics() const {return m_recommendedSystematics;}
  protected:
    // specify Systematic
    virtual void specifiedSystematics(const std::set<std::string>& specifiedSystematics);
  private:
    StatusCode recalculateEventMET(xAOD::SystematicEvent* event,
                                   const xAOD::MissingETContainer* met_core,
                                   const xAOD::MissingETAssociationMap* met_map,
                                   bool forceUseLooseObjects=false,
                                   std::string outputContainerSuffix="");

    std::shared_ptr<top::TopConfig> m_config;

    std::list<CP::SystematicSet> m_specifiedSystematics;
    std::list<CP::SystematicSet> m_recommendedSystematics;

    std::string m_MET_core;
    std::string m_MET_map;

    ToolHandle<IMETMaker> m_met_maker;
    ToolHandle<IMETSystematicsTool> m_met_systematics;
  };
}

#endif
