/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/* ****************************************************************************
 Filename    : CaloDmNeighbours.h
 Author      : guennadi.pospelov@cern.ch
 Created     : April, 2007
 DESCRIPTION : Initialize and provide access to neighbours accross
               the calorimeter systems for dead material calibration hits.
**************************************************************************** */

#ifndef CALOCALIBHITREC_CALODMNEIGHBOURS_H
#define CALOCALIBHITREC_CALODMNEIGHBOURS_H

#include "StoreGate/StoreGateSvc.h"
#include "CxxUtils/CachedPointer.h"

#include <string>

class Identifier;
class IdentifierHash;
class ExpandedIdentifier;

class CaloDetDescrManager; 
class CaloDM_ID;
class CaloCell_ID;
class LArFCAL_ID;
class LArHEC_ID;
class AtlasDetectorID;

class CaloDmRegion;
class CaloDmDescrManager;

class CaloDmNeighbours {
  public:
    enum field_indexes {
      kDM_SUBDET=1, kDM_TYPE=2, kDM_SAMPL=3, kDM_REGION=4, kDM_ETA=5, kDM_PHI=6,
      kLAR_SIDE=2, kLAR_SAMPL=3, kLAR_REGION=4, kLAR_ETA=5, kLAR_PHI=6,
      kTILE_SECTION=1, kTILE_SIDE=2, kTILE_PHI = 3, kTILE_ETA = 4, kTILE_SAMPL=5,
      kFCAL_SIDE=2, kFCAL_ETA=4, kFCAL_PHI=5
    };

    CaloDmNeighbours(const CaloDmDescrManager *dmMgr);
    ~CaloDmNeighbours();

    int initialize(std::string filename);

    int getNeighbours_DmHitsForCaloCell(const Identifier &cell_id,  std::vector<IdentifierHash>& neighbourList) const;
    int getNeighbours_CaloCellsForDmHit(const IdentifierHash &dmHashID,  std::vector<IdentifierHash>& neighbourList) const;

  private:
    StatusCode load_neighbours(const std::string& DmNeighboursFileName);
    StatusCode unfold_neighbours(std::string &MyDm, std::string &MyCalo);
    int get_indexes(ExpandedIdentifier &idExp, int &indxSide, int &indxEta, int &indxPhi);
    int get_id(ExpandedIdentifier &idExp, Identifier &id, IdentifierHash &hash_id);

    int generate_neighbour_file();
    int make_CaloSample2DmRegion_map();

    const CaloDetDescrManager* getMgr() const;

    std::string m_DmNeighboursFileName;

    StoreGateSvc* m_detStore;
    CxxUtils::CachedPointer<const CaloDetDescrManager> m_caloDetDescrManager; 
    const CaloDmDescrManager *m_caloDmDescrManager;
    const CaloDM_ID*    m_caloDM_ID;
    const CaloCell_ID*    m_caloCell_ID;
    const LArFCAL_ID*   m_larFcal_ID;
    const LArHEC_ID*    m_larHec_ID;
    const AtlasDetectorID* m_id_helper;

    std::vector<std::vector<std::vector<const CaloDmRegion * > > > m_CaloSample2DmRegion;
    std::vector<std::vector<IdentifierHash > > m_DmNeighbourLArVector;
    std::vector<std::vector<IdentifierHash > > m_DmNeighbourTileVector;
};

#endif
