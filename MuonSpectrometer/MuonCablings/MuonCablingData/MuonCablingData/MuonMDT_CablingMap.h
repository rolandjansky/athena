/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MUONMDT_CABLINGMAP_H
#define MUONMDT_CABLING_MUONMDT_CABLINGMAP_H

#include "Identifier/Identifier.h"
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
 **********************************************/

class MdtMezzanineType;
class MdtIdHelper;
class IdentifierHash;

/** typedef to implement the csm mapping to ROD */
/* mapping from hashid to ROD identifier as Subdetector+Rodid */
typedef std::map< IdentifierHash , uint32_t > ChamberToRODMap;

typedef std::map< uint32_t, std::vector<IdentifierHash> > RODToChamberMap;

/** full list of ROB numbers */
typedef std::vector<uint32_t> ListOfROD;

/** typedef to implement the list of mezzanine types */
typedef std::map< uint8_t, std::unique_ptr<MdtMezzanineType>, std::less<uint8_t> > MezzanineTypes;


class MuonMDT_CablingMap : public MdtMapBase<MdtSubdetectorMap> {

 public:
  
struct CablingData{
      CablingData()=default;
     
      int stationIndex{-1};/// Station of the chamber (i.e, BIL,BIS,etc.) 
      int eta{0};          /// Eta of the MDT station
      int phi{0};          /// Phi sector of the MDT station
      int multilayer{-99}; /// Multilayer inside the MDT station
      int layer{-99};      /// Layer inside the multilayer
      int tube{-99};       /// Tube number in the layer

      uint8_t  mrod{0};           /// MROD number
      uint8_t  csm{0};            ///  CSM number
      uint8_t  mezzanine_type{0}; /// Mezzanine type
      uint8_t subdetectorId{0}; /// Subdetector number     
      
      /// the 0xff used to convert the full station)
      uint8_t tdcId{0xFF};          /// Identifier of the corresponding tdc
      uint8_t channelId{0xff};      /// Identifier of the corresponding channel
  


      /// In the Muon spectrometer layouts, before Run 2, no chamber had more than 100 tubes per multilayer. Hence, the 
      /// the tube layer and multilayer information is encoded into a 4 digit long number
      ///           MLTT
      /// where M represents the multilayer number, L the layer number and TT is the tube numbering per each multilayer
      static constexpr int legacy_tube_block_size = 100;
      static  constexpr int legacy_layer_block_size = 10;

      /// Helper constants to extract tube,layer, multilayer information
      int tubes_per_layer{legacy_tube_block_size};
      int layer_block{legacy_layer_block_size};

      /// Equality operator 
      bool operator ==(const CablingData& other) const{
         return stationIndex == other.stationIndex &&
                eta == other. eta &&
                phi == other.phi &&
                multilayer==other.multilayer &&
                layer == other.layer &&
                tube == other.tube &&
                mrod == other.mrod &&
                csm == other.csm &&
                mezzanine_type == other.mezzanine_type &&
                subdetectorId == other.subdetectorId &&
                tdcId == other.tdcId &&
                channelId == other.channelId; 
      }
      bool operator !=(const CablingData& other ) const{
          return ! ((*this) == other);
      }
  };

  MuonMDT_CablingMap();
  ~MuonMDT_CablingMap();


  
  /** Set functions */
  bool setSubdetectorMap(uint8_t subdetectorId,MdtSubdetectorMap* mdtSubdetectorMap, MsgStream &log);

  /** Add a new line describing a mezzanine type */
  bool addMezzanineLine(const int type, const int layer, const int sequence, MsgStream &log);  

  /** Add a new mezzanine */
  /** the indexes multilayer, layer, tube refer to the tube connected to the channelZero */
  bool addMezzanine(const CablingData& cabling_data, MsgStream &log);

  /** Get function */
  MdtSubdetectorMap* getSubdetectorMap(uint8_t subdetectorId) const;

  /** return the ROD id of a given chamber, given station, eta, phi */
  uint32_t getROBId(int station, int eta, int phi, MsgStream &log) const;

  /** return the ROD id of a given chamber, given the hash id */
  uint32_t getROBId(const IdentifierHash stationCode, MsgStream &log) const;

  /** get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration */
  std::vector<uint32_t> getROBId(const std::vector<IdentifierHash>& mdtHashVector, MsgStream &log) const;

 /** return a vector of HashId lists for a  given list of ROD's */
  const std::vector<IdentifierHash> getChamberHashVec(const std::vector< uint32_t> &ROBId_list, MsgStream &log) const;

 /** return a HashId list for a  given ROD */
  const std::vector<IdentifierHash>& getChamberHashVec(const uint32_t ROBI, MsgStream &log) const;

  /** return the ROD id of a given chamber */
  std::vector<uint32_t> getAllROBId() const;

  /** return the offline id given the online id */
  bool getOfflineId(CablingData& cabling_data, MsgStream &log) const;

  /** return the online id given the offline id */
  bool getOnlineId(CablingData& cabling_data, MsgStream &log) const;
  /** converts the cabling data into an identifier. The check valid argument optionally enables the check that the returned identifier is actually well defined within the ranges but is also slow */
  bool convert(const CablingData& cabling_data, Identifier& id, bool check_valid = true )const;
  /** converts the identifier into a cabling data object. Returns false if the Identifier is not Mdt */
  bool convert(const Identifier&id , CablingData& cabling_data) const;
  
 private:

  /** private function to add a chamber to the ROD map */
  bool addChamberToRODMap(int station, int eta, int phi, int subdetectorId, 
			  int rodId, MsgStream &log);

  /** List of mezzanine types, to be initialized from the conditions db */
  MezzanineTypes m_listOfMezzanineTypes;

  /** map returning the RODid for a given chamber ID */
  ChamberToRODMap m_chamberToROD;

 /** map returning a vecotr of Hashid's associated with a given ROD */
  //new sbarnes
   RODToChamberMap m_RODToChamber;
   std::vector<IdentifierHash> m_emptyIdHashVec;

  /** full list of RODs */
  ListOfROD m_listOfROD;

  /** private function to compute a station code for the chamber to ROD map */
  bool getStationCode(int station, int eta, int phi, IdentifierHash& mdtIdHash, MsgStream &log) const;

  /** Pointer to the MdtIdHelper */
  const MdtIdHelper* m_mdtIdHelper{};

  /** assignment and copy constructor operator (hidden) */
  MuonMDT_CablingMap & operator=(const  MuonMDT_CablingMap &right) =delete;
  MuonMDT_CablingMap(const  MuonMDT_CablingMap&) =delete;

};


std::ostream& operator<<(std::ostream& ostr, const MuonMDT_CablingMap::CablingData & obj);

//#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( MuonMDT_CablingMap , 51038731 , 1 )
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<MuonMDT_CablingMap>, 34552845, 0 )

#endif
