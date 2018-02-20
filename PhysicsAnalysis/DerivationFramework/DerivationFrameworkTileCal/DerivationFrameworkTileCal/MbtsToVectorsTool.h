///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MbtsToVectorsTool.h 
// Header file for class MBTSToVectors
/////////////////////////////////////////////////////////////////// 
#ifndef DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_MBTSTOVECTORSTOOL_H
#define DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_MBTSTOVECTORSTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"

// PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

// Forward declaration

namespace DerivationFramework {

  class MbtsToVectorsTool: virtual public IAugmentationTool
                     , public AthAlgTool 
  { 
    
    public: 
    
      /// Constructor with parameters: 
      MbtsToVectorsTool( const std::string& type, const std::string& name, const IInterface* parent );
    
      virtual StatusCode addBranches() const;
      virtual StatusCode initialize();

    private:
    
      std::string m_prefix;
      std::string m_cellContainer;
      bool m_saveEtaPhi;

      mutable SG::WriteHandle<std::vector<float> > m_energy;
      mutable SG::WriteHandle<std::vector<float> > m_time;
      mutable SG::WriteHandle<std::vector<float> > m_eta;
      mutable SG::WriteHandle<std::vector<float> > m_phi;
      mutable SG::WriteHandle<std::vector<int> > m_quality;
      mutable SG::WriteHandle<std::vector<int> > m_type;
      mutable SG::WriteHandle<std::vector<int> > m_module;
      mutable SG::WriteHandle<std::vector<int> > m_channel;
  }; 
  
}


#endif //> !DERIVATIONFRAMEWORK_DERIVATIONFRAMEWORKTILECAL_MBTSTOVECTORSTOOL_H
