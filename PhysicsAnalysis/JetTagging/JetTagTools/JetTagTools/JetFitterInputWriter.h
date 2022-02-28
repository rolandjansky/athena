/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERINPUTWRITER_H
#define BTAGTOOL_JETFITTERINPUTWRITER_H

/******************************************************
    @class JetFitterInputWriter
    Package : JetTagTools
    Created : January 2008
    DESCRIPTION: write inputs to xAOD
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"
#include "xAODBTagging/BTagging.h"


// #include <TString.h>

class TTrainedNetwork;
class TH1;

namespace Trk
{
  class NeuralNetworkToHistoTool;
}


namespace Analysis {


  class IJetFitterTagInfo;

  class JetFitterInputWriter : public extends<AthAlgTool, IJetFitterClassifierTool>
  {
  public:
    using base_class::base_class;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    virtual
    StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
                                    const std::string & jetauthor,
                                    const std::string& inputbasename,
                                    const std::string& outputbasename,
                                    float jetpT,
                                    float jeteta,
                                    float IP3dlike=-5000) const override;

  private:
    Gaudi::Property<bool> m_useCombinedIPNN
    { this, "useCombinedIPNN", true, "" };
    Gaudi::Property<bool> m_usePtCorrectedMass
    { this, "usePtCorrectedMass", false, "" };

  };

}//end Analysis namespace

#endif
