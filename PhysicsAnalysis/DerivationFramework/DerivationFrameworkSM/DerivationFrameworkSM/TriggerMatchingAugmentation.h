/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      ToolHandle< Trig::IMatchingTool > m_tool;
      ToolHandle< Trig::TrigDecisionTool > m_trigDec; 
      std::string m_sgName;
      std::string m_muonContainerName;
      std::string m_electronContainerName;
      std::string m_photonContainerName;
      std::vector< std::string >  m_singletriggerList;
      std::vector< std::string >  m_2mutriggerList;
      std::vector< std::string > m_2etriggerList;
      std::vector< std::string > m_emtriggerList;
      StatusCode matchSingle(const xAOD::IParticleContainer* collection,std::string trigger) const;
      StatusCode matchDi(const xAOD::IParticleContainer* collection1 ,const xAOD::IParticleContainer* collection2 ,std::string trigger) const;


  };
}
 
#endif // DERIVATIONFRAMEWORK_PHOTONVERTEXSELECTIONWRAPPER_H
