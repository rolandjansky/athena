/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_BTAGJETPTSCALING_H
#define BTAGGING_BTAGJETPTSCALING_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "BTagging/IBTagJetPtScaling.h"

#include <string>

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis {
  
  class BTagJetPtScaling : public AthAlgTool, virtual public IBTagJetPtScaling {
  public:
    /** Constructors and destructors */
    BTagJetPtScaling(const std::string&,const std::string&,const IInterface*);
    virtual ~BTagJetPtScaling();

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    StatusCode BTagJetPtScaling_exec(xAOD::JetContainer& jets) const override;

  private:
    float m_PtScalingConstPar;
    float m_PtScalingLinearPar;
//    bool  m_doPtScaling;

  }; // End class

} // End namespace

#endif // BTAGJETPTSCALING_H
