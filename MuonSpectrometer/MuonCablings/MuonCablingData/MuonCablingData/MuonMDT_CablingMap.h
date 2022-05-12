/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONMDT_CABLING_MUONMDT_CABLINGMAP_H
#define MUONMDT_CABLING_MUONMDT_CABLINGMAP_H

#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/Identifier.h"
#include "MuonCablingData/MdtTdcMap.h"
/**********************************************
 *
 * @brief MDT map data object
 *
 **********************************************/

class MdtMezzanineType;
class MdtIdHelper;
class IdentifierHash;

class MuonMDT_CablingMap {
public:
    /** typedef to implement the list of mezzanine types */
    using MezzanineTypes = std::map<uint8_t, std::unique_ptr<MdtMezzanineType>>;
    using TdcOffSet = std::set<MdtTdcOffSorter, std::less<>>;
    using TdcOnlSet = std::vector<MdtTdcOnlSorter>;
    using OffToOnlMap = std::map<MdtCablingOffData, TdcOffSet>;

    /// The online -> offline conversion needs to treat two cases
    ///   tdcId && channelId == 0xFF:
    ///     ** Decode the station name using the first module in the set with tdcZero() == 0
    ///     ** Ordinary channel decoding
    /// Helper struct below is collection of all the modules & the first module used to decode the stationName
    struct MdtTdcModule {
        TdcOnlSet::const_iterator begin() const { return all_modules.begin(); }
        TdcOnlSet::const_iterator end() const { return all_modules.end(); }
        TdcOnlSet::const_iterator begin() { return all_modules.begin(); }
        TdcOnlSet::const_iterator end() { return all_modules.end(); }

        TdcOnlSet all_modules{};

        MdtTdcOnlSorter zero_module{nullptr};
    };
    using OnlToOffMap = std::map<MdtCablingOnData, MdtTdcModule>;

    /** typedef to implement the csm mapping to ROD */
    /* mapping from hashid to ROD identifier as Subdetector+Rodid */
    using ChamberToRODMap = std::map<IdentifierHash, uint32_t>;
    using RODToChamberMap = std::map<uint32_t, std::vector<IdentifierHash>>;

    using ListOfROD = std::vector<uint32_t>;

    using CablingData = MdtCablingData;
    MuonMDT_CablingMap();
    ~MuonMDT_CablingMap();

    /** Add a new line describing a mezzanine type */
    bool addMezzanineLine(const int type, const int layer, const int sequence, MsgStream& log);

    /** Add a new mezzanine */
    /** the indexes multilayer, layer, tube refer to the tube connected to the channelZero */
    bool addMezzanine(const CablingData& cabling_data, MsgStream& log);

    /** return the offline id given the online id */
    bool getOfflineId(CablingData& cabling_data, MsgStream& log) const;

    /** return the online id given the offline id */
    bool getOnlineId(CablingData& cabling_data, MsgStream& log) const;
    /** converts the cabling data into an identifier. The check valid argument optionally enables the check that the returned identifier is
     * actually well defined within the ranges but is also slow */
    bool convert(const CablingData& cabling_data, Identifier& id, bool check_valid = true) const;
    /** converts the identifier into a cabling data object. Returns false if the Identifier is not Mdt */
    bool convert(const Identifier& id, CablingData& cabling_data) const;

    /** return the ROD id of a given chamber, given the hash id */
    uint32_t getROBId(const IdentifierHash& stationCode, MsgStream& log) const;
    /** get the robs corresponding to a vector of hashIds, copied from Svc before the readCdo migration */
    ListOfROD getROBId(const std::vector<IdentifierHash>& mdtHashVector, MsgStream& log) const;

    /** return a vector of HashId lists for a  given list of ROD's */
    std::vector<IdentifierHash> getChamberHashVec(const std::vector<uint32_t>& ROBId_list, MsgStream& log) const;

    /** return a HashId list for a  given ROD */
    const std::vector<IdentifierHash>& getChamberHashVec(const uint32_t ROBI, MsgStream& log) const;

    /** return the ROD id of a given chamber */
    const ListOfROD& getAllROBId() const;

    /// Returns the map to convert the online -> offline identifiers
    const OnlToOffMap& getOnlineConvMap() const;
    /// Returns hte map to convert the offline -> online identifiers
    const OffToOnlMap& getOfflineConvMap() const;

    bool finalize_init(MsgStream& log);

private:
    /** private function to add a chamber to the ROD map */
    bool addChamberToRODMap(const CablingData& cabling_data, MsgStream& log);

    /// Transforms the identifier to an IdentifierHash
    bool getStationCode(const CablingData& map_data, IdentifierHash& mdtHashId, MsgStream& log) const;

    /** List of mezzanine types, to be initialized from the conditions db */
    MezzanineTypes m_listOfMezzanineTypes;

    /** Pointer to the MdtIdHelper */
    const MdtIdHelper* m_mdtIdHelper{};

    /** assignment and copy constructor operator (hidden) */
    MuonMDT_CablingMap& operator=(const MuonMDT_CablingMap& right) = delete;
    MuonMDT_CablingMap(const MuonMDT_CablingMap&) = delete;

    OffToOnlMap m_toOnlineConv{};
    OnlToOffMap m_toOfflineConv{};
    std::vector<std::unique_ptr<MdtTdcMap>> m_tdcs{};

    /** map returning a vecotr of Hashid's associated with a given ROD */
    // new sbarnes
    RODToChamberMap m_RODToChamber{};
    /** full list of RODs */
    ListOfROD m_listOfROD{};

    /** map returning the RODid for a given chamber ID */
    ChamberToRODMap m_chamberToROD{};
};

//#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(MuonMDT_CablingMap, 51038731, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF(CondCont<MuonMDT_CablingMap>, 34552845, 0)

#endif
