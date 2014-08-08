/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGLABELING_H
#define BTAGGING_BTAGLABELING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BTagging/IBTagLabeling.h"
#include "xAODJet/Jet.h"


#include <string>

class Jet;

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
  class IBTagLabeling;
  class IJetTruthMatching;

  //JBdV
  //class FindLeptonTruth;

  class BTagLabeling : public AthAlgTool, virtual public IBTagLabeling
  {
      public:
          /** Constructors and destructors */
          BTagLabeling(const std::string&,const std::string&,const IInterface*); 
          virtual ~BTagLabeling();


          StatusCode initialize();
          StatusCode finalize();
          StatusCode  BTagLabeling_exec(xAOD::Jet& myJet);

      private:

          ToolHandle< IJetTruthMatching > m_jetTruthMatchTool;        //!< pointer to the Truth matching tool
          //ToolHandle< IJetTruthMatching > m_jetQGPartonTruthMatchTool;        //!< pointer to the QG Truth matching tool
          //ToolHandle< FindLeptonTruth > m_LeptonTruthTool;

  }; // End class

} // End namespace

#endif // BTAGLABELING_H
