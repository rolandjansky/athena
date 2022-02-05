/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWUTILS_TILEROI_MAP_H
#define TILERAWUTILS_TILEROI_MAP_H

// #include "TileEvent/TileRoI_ID.h"
#include "TileIdentifier/TileFragHash.h"
#include "CaloIdentifier/TileID_Exception.h" 

#include "AthenaBaseComps/AthAlgTool.h"
#include <string> 
#include <vector> 
#include <map> 

class TileID;
class TileHWID;
class Identifier;
class HWIdentifier;
class TileDetDescrManager;
class TileCablingService;

/**
 * TileCal RoI map to offline Identifier
 * 
 * @author Alexandre Solodkov
 * @version 00-00-01 , 26/08/2002 
 */


class TileRoI_Map : public AthAlgTool { 

 public:
  /** constructor 
  */ 
  TileRoI_Map( const std::string& type, const std::string& name,
        const IInterface* parent ) ; 

  /** define a RoI identifier type, two ints. 
  */ 

  StatusCode initialize(); 

  // For clarity,  use TrigTowerID for TT Identifier .
  typedef Identifier TT_ID; 
  typedef int COLL_ID ; 

  virtual ~TileRoI_Map() ;
  /** AlgTool InterfaceID
  */ 
  static const InterfaceID& interfaceID( ) ; 

  /**
   * return TrigTowerID for various queries. 
   */

  TT_ID  TrigTowerID (const Identifier& id  ) const;

  TT_ID  TrigTowerID (const HWIdentifier& id  ) const;

  /** reserve lookup. given RoI, get CollectionID 
  */ 
  const std::vector<COLL_ID>& CollectionID(const TT_ID& id) const ; 

  /** check validity
  */ 

  bool validId(const TT_ID& tt_id) const ; 

  /** returns Eta-Phi Region for a hashed ID of the Collection. 
   return false for invalid ID. 
  */ 

  bool getEtaPhiRegion(unsigned int hashid, 
                      float& etamin, float& etamax,
                      float& phimin, float& phimax ) const ; 

 private:

  bool addEntry(const HWIdentifier& adc_hwid) ; 

  // a rectangular region in eta phi 
  class EtaPhiRegion
  {
    public:
        EtaPhiRegion() : etamin(999.), etamax(-999.), 
                         phimin(999.), phimax(-999.)
        { } ; 
        
        void set(float eta, float phi){

                etamin = std::min(etamin,eta); 
                etamax = std::max(etamax,eta); 

                phimin = std::min(phimin,phi); 
                phimax = std::max(phimax,phi); 
        }               
        
        float etamin; 
        float etamax; 
        float phimin; 
        float phimax; 
     
  } ; 

  void printEtaPhiRegions(); 

 /** Map for offline ID and online id
 */  
  typedef std::map<Identifier, TT_ID> OFFMAP ; 
  typedef std::map<HWIdentifier, TT_ID> ONLMAP ; 
  typedef std::vector<COLL_ID>  COLL_ID_VEC; 
  typedef std::map<TT_ID, COLL_ID_VEC  > ROIMAP ; 

  typedef std::map<int, EtaPhiRegion  > E_P_MAP ; 

  OFFMAP m_offmap; 
  ONLMAP m_onlmap; 
  ROIMAP m_roimap;

  E_P_MAP m_epmap; 
 
  const TileID*   m_tileID; 
  const TileHWID* m_tileHWID; 
  const TileDetDescrManager* m_tileMgr;
  const TileCablingService* m_cablingSvc; 

  TileFragHash m_idhash; 
  bool m_print; 
};
  

#endif

