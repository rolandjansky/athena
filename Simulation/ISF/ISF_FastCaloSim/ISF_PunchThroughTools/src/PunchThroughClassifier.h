/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PunchThroughClassifier.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PUNCHTHROUGHTOOLS_SRC_PUNCHTHROUGHCLASSIFIER_H
#define ISF_PUNCHTHROUGHTOOLS_SRC_PUNCHTHROUGHCLASSIFIER_H 1

// ISF includes
#include "ISF_Event/ISFParticle.h"

#include "ISF_FastCaloSimInterfaces/IPunchThroughClassifier.h"

#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"

#include "AthenaBaseComps/AthAlgTool.h"


#include <map>

namespace lwt
{
    class LightweightGraph;
}

namespace ISF {

class PunchThroughClassifier : public extends<AthAlgTool, IPunchThroughClassifier> {
public:

    /** Constructor */
    PunchThroughClassifier(const std::string&,const std::string&,const IInterface*);

    /** Destructor */
    virtual ~PunchThroughClassifier() = default;

    /** AlgTool initialize method */
    virtual StatusCode initialize() override final;
    /** AlgTool finalize method */
    virtual StatusCode finalize() override final;

    /** input variable MinMaxScaler initialize method */
    StatusCode initializeScaler(std::string scalerConfigFile);

    /** neural network initialize method */
    StatusCode initializeNetwork(std::string networkConfigFile);

    /** isotonic regressor calibrator initialize method */
    StatusCode initializeCalibrator(std::string calibratorConfigFile);

    /** interface method to return probability prediction of punch through */
    double computePunchThroughProbability(const ISF::ISFParticle &isfp, const TFCSSimulationState& simulstate) const;

    /** calcalate NN inputs based on isfp and simulstate */
    std::map<std::string, std::map<std::string, double> > computeInputs(const ISF::ISFParticle &isfp, const TFCSSimulationState& simulstate) const;

    /** scale NN inputs using MinMaxScaler */
    std::map<std::string, std::map<std::string, double> > scaleInputs(std::map<std::string, std::map<std::string, double> >& inputs) const;

    /** calibrate NN output using isotonic regressor */
    double calibrateOutput(double& networkOutput) const;

private:
    const char* m_name=nullptr;

    /** NN graph */
    std::unique_ptr<lwt::LightweightGraph> m_graph;

    /** input variable MinMaxScaler members */
    double m_scalerMin;
    double m_scalerMax;
    std::map<std::string, double> m_scalerMinMap;
    std::map<std::string, double> m_scalerMaxMap;

    /** isotonic regressor calibrator members */
    std::string m_calibratorConfigFile;
    double m_calibrationMin;
    double m_calibrationMax;
    std::map<double, double> m_calibrationMap;

    //properties
    std::string m_networkConfigFileName;
    std::string m_scalerConfigFileName;
    std::string m_calibratorConfigFileName;

};
}

#endif //ISF_PUNCHTHROUGHTOOLS_SRC_PUNCHTHROUGHCLASSIFIER_H
