/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TriggerMatchingAugmentation.h
///////////////////////////////////////////////////////////////////
// Author: Matthias Saimpert (matthias.saimpert@cern.ch)
//
 
#ifndef DERIVATIONFRAMEWORK_TRIGGERMATCHINGAUGMENTATION_H
#define DERIVATIONFRAMEWORK_TRIGGERMATCHINGAUGMENTATION_H
 
#include <string>
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODBase/IParticle.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TriggerMatchingTool/IMatchingTool.h"

namespace Trig
{
  class IMatchingTool;
  class TrigDecisionTool;
}
 
namespace DerivationFramework {
 
  class TriggerMatchingAugmentation : public AthAlgTool, public IAugmentationTool {
    public:
      TriggerMatchingAugmentation(const std::string& t, const std::string& n, const IInterface* p);
 
      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;
 
    private:

      typedef SG::AuxElement::Decorator<char> decor_t;

      ToolHandle< Trig::IMatchingTool > m_matchTool;
      ToolHandle< Trig::TrigDecisionTool > m_trigDec; 
      std::string m_decorPrefix;
      std::string m_muonContainerName;
      std::string m_electronContainerName;
      std::string m_photonContainerName;
    // Used for property setting
      std::vector< std::string >  m_singleTriggerList = {};
      std::vector< std::string >  m_2mTriggerList = {};
      std::vector< std::string >  m_2eTriggerList = {};
      std::vector< std::string >  m_emTriggerList = {};

    // Used internally
      std::vector< std::pair<const std::string, decor_t> >  m_1eDecorList = {};
      std::vector< std::pair<const std::string, decor_t> >  m_1mDecorList = {};
      std::vector< std::pair<const std::string, decor_t> >  m_1gDecorList = {};

      std::vector< std::pair<const std::string, decor_t> >  m_2eDecorList = {};
      std::vector< std::pair<const std::string, decor_t> >  m_2mDecorList = {};
      std::vector< std::pair<const std::string, decor_t> >  m_emDecorList = {};

    StatusCode matchSingle(const xAOD::IParticleContainer* collection,
			   const std::string& trigger,
			   const decor_t& decor) const;

    StatusCode matchDi(const xAOD::IParticleContainer* collection1,
		       const xAOD::IParticleContainer* collection2,
		       const std::string& trigger,
		       const decor_t& decor) const;


  };
}
 
#endif // DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
