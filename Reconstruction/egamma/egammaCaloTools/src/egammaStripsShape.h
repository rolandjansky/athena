/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#ifndef EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H
#define EGAMMACALOTOOLS_EGAMMASTRIPSSHAPE_H

/// @class egammaStripsShape
/// @brief   EM cluster shower shape calculations in 1st ECAL sampling 
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///
/// $Revision:$
/// $Date: 2014-02-11 17:40:48 +0100 (Tue, 11 Feb 2014) $
///


class CaloCellContainer;
class CaloDetDescrManager;
class LArEM_ID;

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaStripsShape.h"
#include "egammaInterfaces/Iegammaqweta1c.h"
#include "egammaInterfaces/IegammaEnergyPositionAllSamples.h"

class egammaStripsShape : public AthAlgTool, virtual public IegammaStripsShape {

public:

    /** @brief Default constructor*/
    egammaStripsShape(const std::string& type,
            const std::string& name,
            const IInterface* parent);
    /** @brief Destructor*/
    ~egammaStripsShape();  

    /** @brief AlgTool initialize method.*/
    StatusCode initialize() override;
    /** @brief AlgTool finalize method */
    StatusCode finalize() override;

    /** @brief AlgTool main method */
    virtual StatusCode execute(const xAOD::CaloCluster& cluster, Info& info) const override final;

private:

    /** @brief From the original (eta,phi) position, find the location
      (sampling, barrel/end-cap, granularity) */
    /** @brief set an array of energies,eta,phi in ~40 strips around max*/
    void setArray(const xAOD::CaloCluster& cluster ,CaloSampling::CaloSample sam,
            double eta, double phi,  double deta, double dphi,
            double* enecell, double* etacell, double* gracell,
            int* ncell) const ;
    /** @brief check index of seed in the array*/
    void setIndexSeed(Info& info, double* etacell, double* gracell) const;  
    /** @brief set total width in strips*/
    void setWstot(Info& info, double deta, double* enecell, double* etacell, int* ncell) const;
    /** @brief set fraction of energy in 2nd sampling*/
    void setF2(Info& info, double* enecell,const double eallsamples) const ;
    /** @brief set energy in 3x1 and in 15x3 strips*/
    void setEnergy(Info& info, double* enecell) const;
    /** @brief set asymmetry*/
    void setAsymmetry(Info& info, double* enecell) const;
    /** @brief set width in three strips*/
    void setWs3(Info& info, const xAOD::CaloCluster::CaloSample sam, const xAOD::CaloCluster& cluster, 
            double* enecell, double* etacell,int* ncell) const;
    /** @brief set difference between eta of max and eta of cells*/
    double setDeltaEtaTrackShower(int nstrips,int ieta, double* enecell) const;
    /** @brief set width in 5 strips*/
    void setWidths5(Info& info, double* enecell) const;
    /** @brief set energy of strip with maximum energy*/
    void setEmax(Info& info, double* enecell) const;
    /** @brief set energy of the second local maximum*/
    int setEmax2(Info& info, double* enecell, double* gracell, int* ncell) const;
    /** @brief set energy of strip with minimum energy*/
    void setEmin(int ncsec1,Info& info, double* enecell, double* gracell, int* ncell ) const;
    /** @brief set M.S's valley*/
    void setValley(Info& info, double* enecell) const; 
    /** @brief set fraction of energy outside shower core 
      (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
    void setFside(Info& info, double* enecell, double* gracell, int* ncell) const;
    /** @brief set F1core*/
    void setF1core(Info& info, const xAOD::CaloCluster& cluster) const;

    /** Tool to calculate correction for the eta width modulation in strips */
    ToolHandle<Iegammaqweta1c> m_egammaqweta1c {this,
        "egammaqweta1cTool", "egammaqweta1c/egammaqweta1c"};

    /** @brief tool to calculate sum of energy in all samples */
    ToolHandle<IegammaEnergyPositionAllSamples>  m_egammaEnergyPositionAllSamples {this,
        "egammaEnergyPositionAllSamplesTool", 
        "egammaEnergyPositionAllSamples/egammaEnergyPositionAllSamples"};
    
    // Calo variables
    const CaloDetDescrManager* m_calo_dd;

    // calculate quantities based on information in the strips in a region
    // around the cluster. 
    //
    // Use 2 strips in phi and cover a region of +-1.1875
    // 5 cells in eta based on second sampling granularity ~0.025 in eta.
    //Corresponds to ~19 strips in em barrel)
    //  
    Gaudi::Property<double> m_neta {this, "Neta", 5,
        "Number of eta cell in each sampling in which to calculated shower shapes"};

    Gaudi::Property<double> m_nphi {this, "Nphi", 2.,
        "Number of phi cell in each sampling in which to calculated shower shapes"};

    /** @brief boolean to calculate all variables*/
    Gaudi::Property<bool> m_ExecAllVariables {this, 
        "ExecAllVariables", true, "flag used by trigger"};

    /** @brief boolean to calculate less important variables*/
    Gaudi::Property<bool> m_ExecOtherVariables {this,
        "ExecOtherVariables", true, "Calculate some less important variables"};

};

#endif
