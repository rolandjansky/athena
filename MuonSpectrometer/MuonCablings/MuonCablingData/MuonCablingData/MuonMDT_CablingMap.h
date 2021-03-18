/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MUONMDT_CABLINGMAP_H
#define MUONMDT_CABLING_MUONMDT_CABLINGMAP_H

#include "MuonCablingData/MdtSubdetectorMap.h"
#include "MuonCablingData/MdtRODMap.h"
#include "MuonCablingData/MdtCsmMap.h"
#include "MuonCablingData/MdtAmtMap.h"

#include "MuonCablingData/MdtMapBase.h"

#include "AthenaKernel/CLASS_DEF.h"

/**********************************************
 *
 * @brief MDT map data object
 *
 * @author Stefano Rosati <Stefano.Rosati@roma1.infn.it>
 *
 **********************************************/

class MdtMezzanineType;
class MdtIdHelper;
class IdentifierHash;

/** typedef to implement the csm mapping to ROD */
/* mapping from hashid to ROD identifier as Subdetector+Rodid */
typedef std::map< IdentifierHash , uint32_t , std::less<int> > ChamberToRODMap;

typedef std::map< uint32_t, std::vector<IdentifierHash> > RODToChamberMap;

/** full list of ROB numbers */
typedef std::vector<uint32_t> ListOfROD;

/** typedef to implement the list of mezzanine types */
typedef std::map< uint8_t, std::unique_ptr<MdtMezzanineType>, std::less<uint8_t> > MezzanineTypes;


class MuonMDT_CablingMap : public MdtMapBase<MdtSubdetectorMap> {

 public:
  
  MuonMDT_CablingMap();
  ~MuonMDT_CablingMap();

  /** cleanup function */
  bool cleanup();
  
  /** Set functions */
  bool setSubdetectorMap(uint8_t subdetectorId,MdtSubdetectorMap* mdtSubdetectorMap);

  /** Add a new line describing a mezzanine type */
  bool addMezzanineLine(const int type, const int layer, const int sequence);  

  /** Add a new mezzanine */
  /** the indexes multilayer, layer, tube refer to the tube connected to the channelZero */
  bool addMezzanine(int mezzanineType, int station, int eta, int phi,
		    int multilayer, int layerZero, int tubeZero,
		    int subdetectorId, int rodId, int csmId,
		    int tdcId, int channelZero);

  /** Get function */
  MdtSubdetectorMap* getSubdetectorMap(uint8_t subdetectorId) const;

  /** return the ROD id of a given chamber, given station, eta, phi */
  uint32_t getROBId(int station, int eta, int phi) const;

  /** return the ROD id of a given chamber, given the hash id */
  uint32_t getROBId(const IdentifierHash stationCode) const;

  /** get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration */
  std::vector<uint32_t> getROBId(const std::vector<IdentifierHash>& mdtHashVector) const;

 /** return a vector of HashId lists for a  given list of ROD's */
  const std::vector<IdentifierHash> getChamberHashVec(const std::vector< uint32_t> &ROBId_list) const;

 /** return a HashId list for a  given ROD */
  const std::vector<IdentifierHash>& getChamberHashVec(const uint32_t ROBId) const;

  /** return the ROD id of a given chamber */
  std::vector<uint32_t> getAllROBId() const;

  /** return the offline id given the online id */
  bool getOfflineId(uint8_t subdetectorId,uint8_t rodId,uint8_t csmId,
		    uint8_t tdcId,uint8_t channelId,
		    int& stationName, int& stationEta, int& stationPhi,
		    int& multiLayer, int& layer, int& tube) const;

  /** return the online id given the offline id */
  bool getOnlineId(int stationName, int stationEta, int stationPhi,
		   int multiLayer, int layer, int tube,
		   uint8_t& subdetectorId, uint8_t& rodId, uint8_t& csmId,
		   uint8_t& tdcId, uint8_t& channelId) const;

 private:

  /** private function to add a chamber to the ROD map */
  bool addChamberToRODMap(int station, int eta, int phi, uint8_t subdetectorId, 
			  uint8_t rodId);

  /** List of mezzanine types, to be initialized from the conditions db */
  std::unique_ptr<MezzanineTypes> m_listOfMezzanineTypes;

  /** map returning the RODid for a given chamber ID */
  std::unique_ptr<ChamberToRODMap> m_chamberToROD;

 /** map returning a vecotr of Hashid's associated with a given ROD */
  //new sbarnes
   std::unique_ptr<RODToChamberMap> m_RODToChamber;
   std::vector<IdentifierHash> m_emptyIdHashVec;

  /** full list of RODs */
  std::unique_ptr<ListOfROD> m_listOfROD;

  /** private function to compute a station code for the chamber to ROD map */
  bool getStationCode(int station, int eta, int phi, IdentifierHash& mdtIdHash) const;

  /** Pointer to the MdtIdHelper */
  const MdtIdHelper* m_mdtIdHelper;

  /** assignment and copy constructor operator (hidden) */
  MuonMDT_CablingMap & operator=(const  MuonMDT_CablingMap &right);
  MuonMDT_CablingMap(const  MuonMDT_CablingMap&);

};

//#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( MuonMDT_CablingMap , 51038731 , 1 )
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<MuonMDT_CablingMap>, 34552845, 0 )

#endif
