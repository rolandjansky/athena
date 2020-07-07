/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCDEADMATERIALTREE_H
#define CALOLOCALHADCALIB_GETLCDEADMATERIALTREE_H

/**
 * @class GetLCDeadMaterialTreeTree
 * @version \$Id: GetLCDeadMaterialTree.h,v 1.2 2009-05-18 20:31:52 pospelov Exp $
 * @author Gennady Pospelov <guennadi.pospelov@cern.ch>
 * @date 23-Januar-2009
 * @brief Top algorithm to generate special dead material tree for 
 * Local Hadron Calibration 
 *
 * This class is an Algorithm to generate special dead material tree from ESD
 * for latter calculation of dead material correction coefficients 
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include <vector>
#include <string>
#include "CxxUtils/checker_macros.h"

class TTree;
class TFile;
class CaloDetDescrManager;
class CaloCell_ID;
class CaloLocalHadCoeff;
class CaloHadDMCoeffData;

class ATLAS_NOT_THREAD_SAFE GetLCDeadMaterialTree : public AthAlgorithm {

  public:

    GetLCDeadMaterialTree(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~GetLCDeadMaterialTree();
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:
    /**
     * @brief Name of text file with initial parameters for coefficients calculation
     *
     * File contains binning info for cluster's emfrac, log10ener, log10lambda and eta 
     * to recover energy losses in specific dead material zones */
    std::string m_HadDMCoeffInitFile;

    /**
     * @brief Output tree
     *
     * Output tree to fill and save. */
    TTree *m_outputTree;

    /**
     * @brief Name of the output file to save tree in.
     *
     * Use this property to set the name of the output file containing the
     * DM data tree. */
    std::string m_outputFileName;

    /**
     * @brief Output file to save tree in.
     *
     * Internal variable pointing to the output file. */
    TFile * m_outputFile;

    /**
     * @brief Name of the uncalibrated CaloClusterContainer to use. */
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterCollName;

    /**
     * @brief Name of the calibrated CaloClusterContainer to use. */
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterCollNameCalib;

    /**
     * @brief Collection of dead material correction coeffitients */
    CaloLocalHadCoeff *m_HadDMCoeff;

    /**
     * @brief data to save into the tree */
    CaloHadDMCoeffData *m_data;

    /**
     * @brief save additional cluster info from calibrated collections */
    bool m_doSaveCalibClusInfo;

    /**
     * @brief to work on testbeam data */
    bool m_isTestbeam;        // H6 testbeam flag

    double m_energyMin;
    //double m_energyMax;

    typedef std::pair<std::string,xAOD::CaloCluster::MomentType> moment_name_pair;
    typedef std::vector<moment_name_pair> moment_name_vector;
    std::vector<xAOD::CaloCluster::MomentType> m_momentForDMArea;

    const CaloDetDescrManager* m_calo_dd_man; 
    const CaloCell_ID* m_calo_id;
};

#endif // CALOLOCALHADCALIB_GETLCDEADMATERIALTREE_H

