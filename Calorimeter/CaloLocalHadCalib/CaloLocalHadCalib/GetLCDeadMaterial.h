/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCDEADMATERIAL_H
#define CALOLOCALHADCALIB_GETLCDEADMATERIAL_H

/**
 * @class GetLCDeadMaterial
 * @version \$Id: GetLCDeadMaterial.h,v 1.1 2009-03-03 17:30:21 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 23-Januar-2009
 * @brief Top algorithm to final processing of dead material tree and producing of dead material constants.
 *
 * It runs sequentially following algorithms: CaloHadDMCoeffFit to fit TProfiles histograms,
 * CaloHadDMCoeffMinim to run minimisation, then it saves constants into pool and text files,
 * and finally it runs toy reconstruction to produce dmreco .vs. dmtruth plots
 *
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>
#include <string>
#include "CxxUtils/checker_macros.h"

class TChain;
class TProfile;
class CaloLocalHadCoeff;


class ATLAS_NOT_THREAD_SAFE GetLCDeadMaterial : public AthAlgorithm {

  public:

    GetLCDeadMaterial(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~GetLCDeadMaterial();
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:

    /**
     * @brief Name of text file with initial parameters for coefficients calculation
     */
    std::string m_HadDMCoeffInputFile;

    /**
     * @brief Name of text file with calculated coefficients
     */
    std::string m_HadDMCoeffOutputFile;

    /**
     * @brief Name of postscript file for fit summary
     */
    std::string m_reportProfiles;

    /**
     * @brief Name of postscript file for minimization summary
     */
    std::string m_reportMinimization;

    /**
     * @brief Name of postscript file for results of toy reconstruction
     */
    std::string m_reportCheck;

    /**
     * @brief Do TProfile fitting 
     */
    bool m_doFit;

    /**
     * @brief Do minimization 
     */
    bool m_doMinimization;

    /**
     * @brief Do pool writing
     */
    bool m_doPool;

    /**
     * @brief Do toy reconstruction for final validation of new dead material constants
     */
    bool m_doCheck;

    /**
     * @brief List of root files to process 
     */
    std::vector<std::string > m_inputRootFiles;

    /**
     * @brief Name of key to write into pool file
     */
    std::string m_key;

    /**
     * @brief H6 combined testbeam flag
     */
    bool m_isTestbeam;

    /**
     * @brief data sample contains single particles only
     */
    bool m_isSingleParticle;

    /**
     * @brief Normalization type for fit procedure
     */
    std::string m_NormalizationTypeForFit;

    /**
     * @brief Normalization type for minimization procedure
     */
    std::string m_NormalizationTypeForMinim;

    /**
     * @brief use particle id information
     */
    std::string m_ClassificationType;

    /**
     * @brief number of maximum events to process in one root files
     */
    int m_MaxEventsPerFile;

};

#endif // CALOLOCALHADCALIB_GETLCDEADMATERIAL_H

