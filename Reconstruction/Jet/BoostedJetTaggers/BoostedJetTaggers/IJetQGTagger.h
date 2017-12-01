// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETQGTAGGER_H
#define IJETQGTAGGER_H

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "JetAnalysisInterfaces/IJetSelectorTool.h"

#include "xAODJet/Jet.h"

namespace CP {

  namespace QGntrackSyst {
    const static SystematicVariation trackefficiency("JET_QG_trackEfficiency");
    const static SystematicVariation trackfakes("JET_QG_trackFakes");
    const static SystematicVariation nchargedtopo("JET_QG_nchargedTopo");
    const static SystematicVariation nchargedexp_up("JET_QG_nchargedExp__1up");
    const static SystematicVariation nchargedme_up("JET_QG_nchargedME__1up");
    const static SystematicVariation nchargedpdf_up("JET_QG_nchargedPDF__1up");
    const static SystematicVariation nchargedexp_down("JET_QG_nchargedExp__1down");
    const static SystematicVariation nchargedme_down("JET_QG_nchargedME__1down");
    const static SystematicVariation nchargedpdf_down("JET_QG_nchargedPDF__1down");
  } //namespace QGntrackSyst

  class IJetQGTagger : public virtual CP::ISystematicsTool, public virtual IJetSelectorTool {

    ASG_TOOL_INTERFACE( CP::IJetQGTagger )

    public:
    enum Classifier {
      OutOfValidRange = 0,
      QuarkJet = 1,
      GluonJet = 2
    };

    virtual ~IJetQGTagger() {}

    virtual Root::TAccept tag(const xAOD::Jet& jet) const = 0;
    virtual Root::TAccept tag(const xAOD::Jet& jet, const xAOD::Vertex* pv) const = 0;

    virtual Classifier classify(const xAOD::Jet& jet) const = 0;
    virtual Classifier classify(Root::TAccept accept) const = 0;

  };

} // namespace CP

#endif /* IJETQGTAGGER_H */
