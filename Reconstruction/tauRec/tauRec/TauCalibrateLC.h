/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCALIBRATELC_H
#define TAUREC_TAUCALIBRATELC_H

#include "tauRec/TauToolBase.h"

class TH1;
class TF1;

/**
 * @brief Implementation of tau energy scale (TES) with eta and pile-up correction.
 * 
 *  The energy and eta (direction) correction are done separatly and steered by flags.
 * 
 * @author Margar Simonyan
 * @author Felix Friedrich
 *                                                                              
 */

class TauCalibrateLC : public TauToolBase {
public:

    TauCalibrateLC(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    ~TauCalibrateLC();

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute(TauCandidateData *data);

private:
    std::string tauContainerKey;
    std::string vertexContainerKey;
    std::string calibrationFile; //!< energy calibration file

    static const int nProngBins = 2;

    const TF1 * calibFunc[nProngBins][10]; //maximum 10 eta bins; might not be used on the whole 
    const TH1 * slopeNPVHist[nProngBins];
    const TH1 * etaBinHist;
    const TH1 * etaCorrectionHist;

    unsigned int m_minNTrackAtVertex;
    int    m_nEtaBins;
    double m_averageNPV;

    bool m_doEnergyCorr; //!< switch for energy correction
    bool m_doAxisCorr;   //!< switch for eta correction
    bool m_printMissingContainerINFO;

  double m_clusterCone; //obsolete
};

#endif
