/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


class CaloCellContainer;
class CaloDetDescrManager;
class LArEM_ID;

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "egammaInterfaces/IegammaStripsShape.h"

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
    virtual StatusCode execute(const xAOD::CaloCluster& cluster,
                               const CaloDetDescrManager& cmgr,
                               Info& info) const override final;

  private:

    /** @brief From the original (eta,phi) position, find the location
      (sampling, barrel/end-cap, granularity) */
    /** @brief set an array of energies,eta,phi in ~40 strips around max*/
    void setArray(const xAOD::CaloCluster& cluster,
                  const CaloDetDescrManager& cmgr,
                  CaloSampling::CaloSample sam, double eta, double phi,
                  double deta, double dphi, double* enecell, double* etacell,
                  double* gracell, int* ncell) const;
    /** @brief check index of seed in the array*/
    void setIndexSeed(Info& info, const double* etacell, const double* gracell) const;  
    /** @brief set total width in strips*/
    void setWstot(Info& info, double deta, const double* enecell, const double* etacell, const int* ncell) const;
    /** @brief set fraction of energy in 2nd sampling*/
    void setF2(Info& info, const double* enecell,const double eallsamples) const ;
    /** @brief set energy in 3x1 and in 15x3 strips*/
    void setEnergy(Info& info, const double* enecell) const;
    /** @brief set asymmetry*/
    void setAsymmetry(Info& info, const double* enecell) const;
    /** @brief set width in three strips*/
    void setWs3(Info& info, const xAOD::CaloCluster::CaloSample sam, const xAOD::CaloCluster& cluster, 
            const double* enecell, const double* etacell,const int* ncell) const;
    /** @brief set difference between eta of max and eta of cells*/
    double setDeltaEtaTrackShower(int nstrips,int ieta, const double* enecell) const;
    /** @brief set width in 5 strips*/
    void setWidths5(Info& info, const double* enecell) const;
    /** @brief set energy of strip with maximum energy*/
    void setEmax(Info& info, const double* enecell) const;
    /** @brief set energy of the second local maximum*/
    int setEmax2(Info& info, const double* enecell, const double* gracell, const int* ncell) const;
    /** @brief set energy of strip with minimum energy*/
    void setEmin(int ncsec1,Info& info, const double* enecell, const double* gracell, const int* ncell ) const;
    /** @brief set M.S's valley*/
    void setValley(Info& info, double* enecell) const; 
    /** @brief set fraction of energy outside shower core 
      (E(+/-3strips)-E(+/-1strips))/ E(+/-1strips) */
    void setFside(Info& info, const double* enecell, const double* gracell, const int* ncell) const;
    /** @brief set F1core*/
    void setF1core(Info& info, const xAOD::CaloCluster& cluster) const;

    // calculate quantities based on information in the strips in a region
    // around the cluster. 
    /** @brief boolean to calculate all variables*/
    Gaudi::Property<bool> m_ExecAllVariables {this, 
        "ExecAllVariables", true, "flag used by trigger"};
};

#endif
