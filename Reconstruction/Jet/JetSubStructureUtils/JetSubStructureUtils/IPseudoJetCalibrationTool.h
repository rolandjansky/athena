/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef jetsubstructureutils_ipseudojetcalibrationtool_header
#define jetsubstructureutils_ipseudojetcalibrationtool_header

#include <string>

namespace fastjet{
  class ClusterSequence;
  class PseudoJet;
}
/**
 * @brief interface class for calibrating fastjet::PseudoJets
 *
 */
class IPseudoJetCalibrationTool {

  public:
    virtual ~IPseudoJetCalibrationTool() = default;

    /**
     * @brief Calibrate a fastjet::PseudoJet
     * @param jet input PseudoJet to be calibrated
     * @param R jet radius parameter allows having diferent calibration factors depending on jet radius.
     *        Having R explicitly as an input parameter allows to calibrate also jets for which R cannot
     *        be obtained from the jet, e.g. if they have no associated cluster sequence or are exclusively 
     *        clustered jets.
     */
    virtual void calibratePseudoJet( 
        fastjet::PseudoJet &jet, double R=0 ) const = 0;

    /**
     * @brief calibrate vector of fastjet::Pseudojets
     * @param jetVec PseudoJet vector of jets to be calibrated
     * @param R jet radius parameter allows having diferent calibration factors depending on jet radius.
     *        Having R explicitly as an input parameter allows to calibrate also jets for which R cannot
     *        be obtained from the jet, e.g. if they have no associated cluster sequence or are exclusively 
     *        clustered jets.
     */
    virtual void calibratePseudoJets( 
        std::vector<fastjet::PseudoJet> &jetVec, double R=0 ) const = 0;

};

#endif //> !IPseudoJetCalibrationTool_H
