/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERINPUTWRITER_C
#define BTAGTOOL_JETFITTERINPUTWRITER_C

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

  static const InterfaceID IID_JetFitterInputWriter("Analysis::JetFitterInputWriter", 1, 0);

  class JetFitterInputWriter : public IJetFitterClassifierTool, public AthAlgTool
  {

  public:

    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_JetFitterInputWriter; };

    JetFitterInputWriter(const std::string& name,
                         const std::string& n, const IInterface* p);
    ~JetFitterInputWriter();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    StatusCode fillLikelihoodValues(xAOD::BTagging* BTag,
                                    const std::string & jetauthor,
                                    const std::string& inputbasename,
                                    const std::string& outputbasename,
                                    double jetpT,
                                    double jeteta,
                                    double IP3dlike=-5000);

  private:

    bool m_useCombinedIPNN;
    bool m_usePtCorrectedMass;

  };

}//end Analysis namespace

#endif
