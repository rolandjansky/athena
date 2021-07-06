// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IJETQGTAGGER_H
#define IJETQGTAGGER_H

#include "PATInterfaces/CorrectionCode.h"
#include "PATInterfaces/ISystematicsTool.h"

#include "xAODJet/Jet.h"
#include "xAODTracking/Vertex.h"

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
    const static SystematicVariation trackeff("JET_QG_trackeff");
    const static SystematicVariation fake("JET_QG_fake");

  } //namespace QGntrackSyst

  class IJetQGTagger : public virtual CP::ISystematicsTool {

    ASG_TOOL_INTERFACE( CP::IJetQGTagger )

    public:

    virtual ~IJetQGTagger() {}

    virtual StatusCode tag(const xAOD::Jet& jet) const = 0;
    virtual StatusCode tag(const xAOD::Jet& jet, const xAOD::Vertex* pv) const = 0;

    virtual StatusCode sysApplySystematicVariation(const SystematicSet&) = 0;

  };

} // namespace CP

#endif /* IJETQGTAGGER_H */
