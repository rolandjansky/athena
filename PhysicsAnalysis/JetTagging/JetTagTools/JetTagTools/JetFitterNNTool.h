/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERNNTOOL_C
#define BTAGTOOL_JETFITTERNNTOOL_C

/******************************************************
    @class JetFitterNNTool
    Package : JetTagTools 
    Created : January 2008
    DESCRIPTION:
    This class computes the Neural Network probability for being b-jet, c-jet or uds-jet for 
    a single event. The root MLP network is used for training.
    @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
    @author2 Christian Weiser (christian.weiser AT physik.uni-freiburg.de) (University of Freiburg)
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"
#include "xAODBTagging/BTagging.h"
#include "JetTagCalibration/JetTagCalibCondData.h"
#include <vector>
#include <string>
#include <map>

#include <TString.h>


class TTrainedNetwork;
class TH1;

namespace Trk 
{
  class NeuralNetworkToHistoTool;
}


namespace Analysis {


class IJetFitterTagInfo;


  class JetFitterNNTool : public extends<AthAlgTool, IJetFitterClassifierTool>
  {
  public:

    JetFitterNNTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    virtual ~JetFitterNNTool();

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

    std::unique_ptr<TTrainedNetwork> loadCalibration(const std::string & jetauthor) const;

    std::string m_calibrationDirectory;

    std::string m_calibrationSubDirectory;

    ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;
    
    bool m_useCombinedIPNN;
    
    /** Key of calibration data: */
    SG::ReadCondHandleKey<JetTagCalibCondData> m_readKey{this, "HistosKey", "JetTagCalibHistosKey", "Key of input (derived) JetTag calibration data"};

    int m_maximumRegisteredLayers;
    bool m_usePtCorrectedMass;

  };
  
}//end Analysis namespace

#endif
