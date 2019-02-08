/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMCELLS_H
#define TRIGEFMISSINGET_EFMISSINGETFROMCELLS_H

/********************************************************************

NAME:     EFMissingETFromCells.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev (updated by Diego Casadei)
CREATED:  March 1, 2006     (March 2008)

PURPOSE: Data preparation from cells, uses the
TrigT2CaloCommon/TrigDataAccess.h

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloInterface/ICalorimeterNoiseTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <vector>


class TrigMissingET;
class ICaloNoiseTool;

/**
  $class EFMissingETFromCells
  Updates transient helper object with calorimeter measurement of missing Et
 **/

class EFMissingETFromCells : public EFMissingETBaseTool
{
  public:

    EFMissingETFromCells(const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

    ~EFMissingETFromCells();

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


    // New way with TrigDataAccess doing BS conversion
    // iterators to LArCells. Note the different type from the above
    LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
    LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
    LArTT_Selector<LArCellCont>::const_iterator m_it;

    TileCellCollection::const_iterator m_itBegin;
    TileCellCollection::const_iterator m_itEnd;
    TileCellCollection::const_iterator m_itt;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMCELLS
