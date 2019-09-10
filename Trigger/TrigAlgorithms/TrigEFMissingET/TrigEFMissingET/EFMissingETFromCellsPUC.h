/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMissingETFromCellsPUC_H
#define TRIGEFMISSINGET_EFMissingETFromCellsPUC_H



/********************************************************************

NAME:     EFMissingETFromCellsPUC.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Florian U. Bernlochner, Bob Kowalewski, Kenji Hamano
CREATED:  Nov 28, 2014

PURPOSE:  Pile-up fit

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigMissingEtEvent/TrigMissingET.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>


class TrigMissingET;
class ICaloNoiseTool;



class EFMissingETFromCellsPUC : public EFMissingETBaseTool
{
  public:

    EFMissingETFromCellsPUC(const std::string& type,
                               const std::string& name,
                               const IInterface* parent);

    ~EFMissingETFromCellsPUC();

    virtual StatusCode initialize(); 
    virtual StatusCode finalize();
    virtual StatusCode execute();

    virtual StatusCode execute(xAOD::TrigMissingET *met,
                               TrigEFMissingEtHelper *metHelper,
                               const xAOD::CaloClusterContainer *caloCluster,
                               const xAOD::JetContainer *jets,
                               const xAOD::TrackParticleContainer *track,
                               const xAOD::VertexContainer *vertex,
                               const xAOD::MuonContainer *muon);

    // slow calo data access via RegionSelector
    StatusCode addAllCellsToHelper(TrigEFMissingEtHelper* met);
    StatusCode addLArCellsToHelper(double etamin, double etamax,
                                   double phimin, double phimax,
                                   TrigEFMissingEtHelper* met,
                                   DETID detectorID, int sampling,
                                   bool prepare);
    StatusCode addTileCellsToHelper(double etamin, double etamax,
                                    double phimin, double phimax,
                                    TrigEFMissingEtHelper* met,
                                    DETID detectorID, int sampling,
                                    bool prepare);

    // fast data access via LoadFullCollection
    StatusCode addFullLArCellsToHelper(TrigEFMissingEtHelper* met, DETID detectorID, bool prepare);
    StatusCode addFullTileCellsToHelper(TrigEFMissingEtHelper* met, int sampling, bool prepare);


  private:
    /** configurables */
    float              m_rmsOneSided;       //<! 1-sided cell noise suppression (can be negative)
    float              m_rmsTwoSided;       //<! 2-sided cell noise suppression: disabled if negative
    float              m_maxThreshold;      //<! maximum between the previous ones
    bool               m_doCellNoiseSupp;   //<! switch on/off cell noise suppression
    bool               m_useFullColl;       //<! switch on/off fast access via LoadFullCollection
    bool               m_makeRobustness;    //<! switch on/off robustness checks
    std::vector<float> m_MinCellSNratio;    //<! min cell |energy|/rms for robustness checks
    uint16_t           m_MaxLArQ;
    int                m_MaxTileQ;

    ITrigDataAccess*  m_data;

    // use ToolHandle<ICaloNoiseTool> if extended interface is needed
    ToolHandle<ICalorimeterNoiseTool> m_noiseTool;   //<! noise suppression

    LArCablingService* m_cablingSvc;

    // New way with TrigDataAccess doing BS conversion
    // iterators to LArCells. Note the different type from the above
    LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
    LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
    LArTT_Selector<LArCellCont>::const_iterator m_it;

    TileCellCollection::const_iterator m_itBegin;
    TileCellCollection::const_iterator m_itEnd;
    TileCellCollection::const_iterator m_itt;


    //bool m_saveuncalibrated;
    int  m_methelperposition;
    //xAOD::CaloCluster_v1::State m_clusterstate;

    // Configurables of pile-up fit
    bool   m_subtractpileup;
    //bool   m_use2016algo;       // Use the old (2016) version of the algorithm - included for backwards compatibility
    double m_towerwidthinput;
    double m_etarange;          //the eta range (-m_etarange,+m_etarange) over which towers are defined
    // double m_ptmin; //the tower Et threshold
    double m_aveecluspu;        //a parameter (in MeV) that determines the variance assigned to masked-off regions
    double m_rese;              //the resolution (in units of sqrt(MeV))
    double m_resefloor;         //floor for calo energy resolution (sqrt(Mev))
    double m_nsigma;            //tower ET significance
    double m_varrhoscale;       //adjustment factor for weighting rho errors in fit
    double m_trimfactor;        //Fraction of towers used in calculating the trimmed mean

    //bool m_doLArH11off; //! LAr H11 off or not 
    //bool m_doLArH12off; //! LAr H12 off or not
    //double m_Jetptcut; //! Jet pt cut for LAr H11/H12

    int    m_nphibins;
    int    m_netabins;
    int    m_ntowers;
};

#endif // TRIGEFMISSINGET_EFMissingETFromCellsPUC_H
