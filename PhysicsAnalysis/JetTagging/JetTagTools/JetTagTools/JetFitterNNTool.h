/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

class CalibrationBroker;
  

static const InterfaceID IID_JetFitterNNTool("Analysis::JetFitterNNTool", 1, 0);

  class JetFitterNNTool : public IJetFitterClassifierTool, public AthAlgTool 
  {
    
  public:
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_JetFitterNNTool; };

    JetFitterNNTool(const std::string& name,
                    const std::string& n, const IInterface* p);
    ~JetFitterNNTool();
    
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

    void loadCalibration(const std::string & jetauthor);

    void initializeCalibrationFile();

    std::map<std::string,TTrainedNetwork*> m_NN;

    std::string m_calibrationDirectory;

    std::string m_calibrationSubDirectory;

    ToolHandle<Trk::NeuralNetworkToHistoTool> m_networkToHistoTool;
    
    bool m_useCombinedIPNN;
    
    ToolHandle<CalibrationBroker> m_calibrationTool;

    int m_maximumRegisteredLayers;
    bool m_usePtCorrectedMass;

  };
  
}//end Analysis namespace

#endif
