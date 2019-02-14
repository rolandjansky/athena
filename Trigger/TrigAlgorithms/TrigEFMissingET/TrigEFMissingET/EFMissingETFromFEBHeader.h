/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_EFMISSINGETFROMFEBHEADER_H
#define TRIGEFMISSINGET_EFMISSINGETFROMFEBHEADER_H

/********************************************************************

NAME:     EFMissingETFromFEBHeader.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Rashid Djilkibaev
CREATED:  July 5, 2006

PURPOSE: Data preparation from FEB Headers (Ex/Ey)

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETBaseTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArReadoutModuleService.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include <vector>

class TrigMissingET;
class TrigEFMissingEtHelper;
class TriggerElement;
class ITrigDataAccess;

/**
  $class EFMissingETFromFEBHeader
  Updates transient helper object with information computed with FEB headers
 **/

class EFMissingETFromFEBHeader : public EFMissingETBaseTool
{
  public:

    EFMissingETFromFEBHeader(const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

    ~EFMissingETFromFEBHeader();

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
    StatusCode addFebEnergyToHelper(TrigEFMissingEtHelper* met);
    StatusCode addLArFebEnergyToHelper(double etamin, double etamax,
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
    StatusCode addFullLArFebEnergyToHelper(TrigEFMissingEtHelper* met, DETID detectorID, bool prepare);
    StatusCode addFullTileFebEnergyToHelper(TrigEFMissingEtHelper* met, DETID detectorID, bool prepare);
    StatusCode addFullTileCellsToHelper(TrigEFMissingEtHelper* met, int sampling, bool prepare);


  private:
    ITrigDataAccess*  m_data;

    bool m_useFullColl;

    std::vector<int>* m_usedfeb;

    LArFebEnergyCollection::const_iterator m_iFebBegin;
    LArFebEnergyCollection::const_iterator m_iFebEnd;
    LArFebEnergyCollection::const_iterator m_Febit;

    // things needed to find the sampling:
    LArCablingLegacyService* m_cablingSvc;
    const LArOnlineID*       m_LArOnlineID;
    const CaloCell_ID*       m_CaloCell_ID;
    LArReadoutModuleService  m_larROModSvc;
};

#endif // TRIGEFMISSINGET_EFMISSINGETFROMFEBHEADER
