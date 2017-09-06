/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauObjectCollectionMaker.h  $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_TAUOBJECTCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_TAUOBJECTCOLLECTIONMAKER_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief TauObjectCollectionMaker
  *   Makes all systematic variations of taus
  * 
  * 
  **/ 

// system include(s):
#include <memory>
#include <set>
#include <list>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

// CP Tool include(s):
#include "TauAnalysisTools/ITauSmearingTool.h"
#include "TauAnalysisTools/ITauTruthMatchingTool.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{

  class TauObjectCollectionMaker final : public asg::AsgTool {
    public:
      explicit TauObjectCollectionMaker( const std::string& name );
      virtual ~TauObjectCollectionMaker(){}

      // Delete Standard constructors
      TauObjectCollectionMaker(const TauObjectCollectionMaker& rhs) = delete;
      TauObjectCollectionMaker(TauObjectCollectionMaker&& rhs) = delete;
      TauObjectCollectionMaker& operator=(const TauObjectCollectionMaker& rhs) = delete;

      StatusCode initialize();
      StatusCode execute();

      StatusCode printout();

      // return specific Systematic
      inline virtual const std::list<CP::SystematicSet>& specifiedSystematics() const {return m_specifiedSystematics;}
  
      // return all recommendedSystematics
      inline const std::list<CP::SystematicSet>& recommendedSystematics() const {return m_recommendedSystematics;}

    protected:
      // specify Systematic
      virtual void specifiedSystematics( const std::set<std::string>& specifiedSystematics );

    private:
      std::shared_ptr<top::TopConfig> m_config;
      
      std::list<CP::SystematicSet> m_specifiedSystematics;
      std::list<CP::SystematicSet> m_recommendedSystematics;
      
      ToolHandle<TauAnalysisTools::ITauSmearingTool> m_calibrationTool;
      
  };
} // namespace
#endif
