/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**   
 *   @file SCT_CablingCondAlgFromCoraCool.cxx
 *
 *   @brief Fills an SCT cabling object from the database using CoraCool
 *
 *   @author Shaun Roe
 *   @date 13/11/2008
 */

//package includes
#include "SCT_CablingCondAlgFromCoraCool.h"
#include "SCT_CablingUtilities.h"

//indet includes
#include "InDetIdentifier/SCT_ID.h"

//Athena includes
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

//STL
#include <algorithm>
#include <iostream>
#include <set>
#include <utility>

//Constants at file scope
//Run1: folder names in COMP200 database
static const std::string rodFolderName{"/SCT/DAQ/Configuration/ROD"};
static const std::string rodMurFolderName{"/SCT/DAQ/Configuration/RODMUR"};
static const std::string murFolderName{"/SCT/DAQ/Configuration/MUR"};
static const std::string geoFolderName{"/SCT/DAQ/Configuration/Geog"};
//Run2: folders change name in CONDBR2 database
static const std::string rodFolderName2{"/SCT/DAQ/Config/ROD"};
static const std::string rodMurFolderName2{"/SCT/DAQ/Config/RODMUR"};
static const std::string murFolderName2{"/SCT/DAQ/Config/MUR"};
static const std::string geoFolderName2{"/SCT/DAQ/Config/Geog"};

//the following applied to run1 (pre Sept 2014)
//static const unsigned int possibleSlots[]={6,7,8,10,11,12,14,15,16,18,19,20};// strange numbering of slots for the rods, found in DB.
//...now the 'expanding DAQ' has all slots filled; the TIM sits in slot 13, so...
//static const unsigned int possibleSlots[]={5,6,7,8,9,10,11,12,14,15,16,17,18,19,20,21};
// earlier uploads were then found to have slots 0-11... how to resolve the two?
static const int disabledFibre{255};
static const int defaultLink{128};
// for run1:
//static const int slotsPerCrate(12);//max. 12 slots filled with rods in the crate
//for run 2:
static const int slotsPerCrate{16};
static const int fibresPerMur{12};
static const int fibresPerRod{96};
static const int mursPerRod{8};

//More look up tables
//Disk  harnesses
static const int HO1[]{5,4,3,2,1,0}; //mur on disk C, position 0
static const int HO2[]{12,11,10,9,8,7}; //diskC position 1
static const int HM1[]{6,16,15,14,13,-1}; //diskC position 2 (no 6th position)
static const int HM2[]{22,21,20,19,18,17}; //diskC position 3
static const int HI1[]{28,30,32,29,31,-1}; //diskC position 4
static const int HI2[]{26,27,25,23,24,-1}; //diskC position 5
//
//Disk 9 quadrant reassignment
static const int qc[]{-1,3,2,1,0};
static const int qa[]{-1,2,1,0,3};

//utility functions in file scope
namespace {
  enum SubDetectors{ENDCAP_C=-2, BARREL=0, ENDCAP_A=2};
  // WJM: Reversed ordering for layer 6/(Disk 7) eta 0 on EC C (-2)
  //Possibly inverted modules in disk 6C (counting 0-8 for disks) at eta=1. At eta=0, all modules are inverted
  bool isOdd(int value) {
    return ((value % 2) ==1);
  }
 
  //Endcap Harness fibre-to-position assignments
  int positionNumber(const unsigned int murPosition, const unsigned int indx) {
    int number{-1};
    switch (murPosition){
    case 0: number = HO1[indx]; break; // 0-5
    case 1: number = HO2[indx]; break; // 7-12
    case 2: number = HM1[indx]; break;
    case 3: number = HM2[indx]; break; // 17-22
    case 4: number = HI1[indx]; break;
    case 5: number = HI2[indx]; break;
    }
    return number;
  }
  
  //is the quadrant a special quadrant on disk 9?
  bool specialQuadrant(const unsigned int disk, const unsigned int quadrant) {
    return ((disk==8) and (quadrant==4));
  }
  
  // special quadrant on disk 9 is not really a quadrant, its 4 positions in the middle of each quadrant
  int reassignSpecialQuadrant(const int bec, const unsigned int module) {
    int q{-1};
    if (module>4) return q;
    if (bec==ENDCAP_C) {
      q=qc[module];
    } else if (bec==ENDCAP_A) {
      q=qa[module];
    }
    return q;
  }
  
  //convert to athena coords; an approx. copy of Bruce Gallop's online code
  std::pair<int, int>
    convertToAthenaCoords(const int bec, const unsigned int disk, const unsigned int quadrant, const int harnessPosition, const unsigned int module) {
    int q{-1};
    int number{-1};
    int phi{-1};
    int eta{-1}, internalPhi{-1};
    const std::pair<int, int> errorValue{std::make_pair(eta, phi)};
    if (specialQuadrant(disk, quadrant)) {
      q=reassignSpecialQuadrant(bec,module);
      if (-1==q) return errorValue;
      number = 6;
    } else {
      number=positionNumber(harnessPosition, module);
      q=quadrant;
    }
    if ((number <0) or (number>32)) {
      return errorValue;
    }
    if (number<13) {
      eta=0;
      internalPhi=q * 13 + number;
    } else if (number < 23) {
      eta=1;
      internalPhi=q * 10 + number -13;
    } else {
      eta=2;
      internalPhi=q * 10 + number -23;
    }
    if (bec<0){ //C
      if (disk==8) { //disk 9 is reversed
        phi = (internalPhi +14) % 52;
      } else if (eta==0) {//outer
        phi = (64-internalPhi) % 52;
      } else {//inner, middle
        phi = (49-internalPhi) % 40;
      }
    } else { //A
      if (disk==8) {//disk 9, reversed
        phi=(64 - internalPhi) % 52;
      } else if (eta==0) {//outer
        phi = (52 + 14 +internalPhi) % 52;
      } else if (eta==1) {//middle
        phi = (10 + 40 +internalPhi) % 40;
      } else {//inners
        phi = (internalPhi + 40 + 11) % 40;
      }
    }
    return std::make_pair(eta, phi);
  }
  //valid numbers for an rx1 or rx0Fibre
  bool
    validLinkNumber(const int link) {
    return ((link>-1) and (link<96)) or (link==defaultLink) or (link==disabledFibre);
  }
}//end of anonymous namespace

// Constructor
SCT_CablingCondAlgFromCoraCool::SCT_CablingCondAlgFromCoraCool(const std::string& name, ISvcLocator* pSvcLocator):
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

//
StatusCode
SCT_CablingCondAlgFromCoraCool::initialize() {
  // Check conditions folder names
  if((m_readKeyRod.key()!=rodFolderName) and (m_readKeyRod.key()!=rodFolderName2)) {
    ATH_MSG_FATAL(m_readKeyRod.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }
  if((m_readKeyRodMur.key()!=rodMurFolderName) and (m_readKeyRodMur.key()!=rodMurFolderName2)) {
    ATH_MSG_FATAL(m_readKeyRodMur.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }
  if((m_readKeyMur.key()!=murFolderName) and (m_readKeyMur.key()!=murFolderName2)) {
    ATH_MSG_FATAL(m_readKeyMur.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }
  if((m_readKeyGeo.key()!=geoFolderName) and (m_readKeyGeo.key()!=geoFolderName2)) {
    ATH_MSG_FATAL(m_readKeyGeo.key() << " is incorrect.");
    return StatusCode::FAILURE;
  }

  // SCT_ID
  ATH_CHECK(detStore()->retrieve(m_idHelper, "SCT_ID"));

  // Read Cond Handle
  ATH_CHECK(m_readKeyRod.initialize());
  ATH_CHECK(m_readKeyRodMur.initialize());
  ATH_CHECK(m_readKeyMur.initialize());
  ATH_CHECK(m_readKeyGeo.initialize());

  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

//
StatusCode 
SCT_CablingCondAlgFromCoraCool::finalize() {
  return StatusCode::SUCCESS;
}

//
StatusCode
SCT_CablingCondAlgFromCoraCool::execute(const EventContext& ctx) const {
  // Write Cond Handle
  SG::WriteCondHandle<SCT_CablingData> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Determine the folders are Run2 or Run1
  bool isRun2{m_readKeyRod.key()==rodFolderName2};

  // let's get the ROD AttrLists
  SG::ReadCondHandle<CondAttrListVec> readHandleRod{m_readKeyRod, ctx};
  const CondAttrListVec* pRod{*readHandleRod};
  if (pRod==nullptr) {
    ATH_MSG_FATAL("Could not find ROD configuration data: " << m_readKeyRod.key());
    return StatusCode::FAILURE;
  }
  // build rod-rob map, and store the crate/slot to RobId mapping
  CondAttrListVec::const_iterator rodIt{pRod->begin()};
  CondAttrListVec::const_iterator last_rod{pRod->end()};
  using IntMap = std::map<int, int>;
  IntMap slotMap;
  //there are now 16 crate slots, but they may be numbered (in the DB) as some non-monotonic sequence
  //so here we take whatever is in the database and map it onto numbers 0-15
  {//scope this calculation, only slotMap should emerge
    //map slots onto 0-15
    using S = std::set<int>;
    S slots;
    for (; rodIt != last_rod; ++rodIt) {
      //type of 'slot' changed between COMP200 and CONDBR2:
      if (isRun2) {
        slots.insert(int(rodIt->second["slot"].data<unsigned char>())); //all but 15 inserts will fail, and these 15 should be sorted
      } else {
        slots.insert(int(rodIt->second["slot"].data<short>())); //all but 15 inserts will fail, and these 15 should be sorted
      }
    }
    int counter{0};
    for (S::const_iterator i{slots.begin()};i != slots.end();++i) {
      slotMap[*i]=counter++;
    }
    ATH_MSG_INFO("Number of SCT rod slots inserted: " << counter);
  }
  IntMap crateSlot2RobMap; //local data structure
  bool allInsertsSucceeded{true};
  rodIt=pRod->begin();
  int nrods{0};
  std::set<int> tempRobSet;
  for (; rodIt != last_rod; ++rodIt) {
    const coral::AttributeList& rodAttributes{rodIt->second};
    int rob{rodAttributes["ROB"].data<int>()};
    if (not tempRobSet.insert(rob).second) ATH_MSG_WARNING("Duplicate rob? :" << std::hex << rob << std::dec);
    int crate{isRun2 ? static_cast<int>(rodAttributes["crate"].data<unsigned char>()) : (rodAttributes["crate"].data<int>())};
    int crateSlot{isRun2 ? static_cast<int>(rodAttributes["slot"].data<unsigned char>()) : static_cast<int>(rodAttributes["slot"].data<short>())};
    //see note in header; these may be 0-15, but not necessarily, so we need to map these onto 0-15
    IntMap::const_iterator pSlot{slotMap.find(crateSlot)};
    int slot{(pSlot==slotMap.end()) ? -1 : pSlot->second};
    if (slot==-1) ATH_MSG_ERROR("Failed to find a crate slot in the crate map");
    int rodPosition{(crate * slotsPerCrate) +slot};//generate identifier using crate and slot for use later
    //use the return type of insert which is a pair with the second component(bool) indicating whether the insert was successful
    bool thisInsertSucceeded{crateSlot2RobMap.insert(IntMap::value_type(rodPosition, rob)).second};
    if (not thisInsertSucceeded) {
      ATH_MSG_WARNING("Insert (rodPosition, rob) " << rodPosition << ", " << rob << " failed.");
      ATH_MSG_INFO("map(rod position) is already " << crateSlot2RobMap[rodPosition]);
      ATH_MSG_INFO("crate, slot, slots per crate: " << crate << ", " << slot << ", " << slotsPerCrate);
    }
    allInsertsSucceeded = thisInsertSucceeded and allInsertsSucceeded;
    ++nrods;
  }
  ATH_MSG_INFO(nrods << " rods entered, of which " << tempRobSet.size() << " are unique.");
  
  if (not allInsertsSucceeded) ATH_MSG_WARNING("Some Rod-Rob map inserts failed.");

  /**
   * Get the geography table which gives where the MURs (harnesses, for endcap) really are.
   * In fact for the barrel its obvious, so only extract the endcap ones
   **/
  IntMap geoMurMap;
  SG::ReadCondHandle<CondAttrListVec> readHandleGeo{m_readKeyGeo, ctx};
  const CondAttrListVec* pGeo{*readHandleGeo};
  if (pGeo==nullptr) {
    ATH_MSG_FATAL("Could not find Geog configuration data: " << m_readKeyGeo.key());
    return StatusCode::FAILURE;
  }
  CondAttrListVec::const_iterator geoIt{pGeo->begin()};
  CondAttrListVec::const_iterator last_geo{pGeo->end()};
  for (;geoIt != last_geo;++geoIt) {
    const coral::AttributeList& geoAttributes{geoIt->second};
    int mur{isRun2 ? static_cast<int>(geoAttributes["MUR"].data<unsigned int>()) : (geoAttributes["MUR"].data<int>())};
    int position{isRun2 ? static_cast<int>(geoAttributes["position"].data<short>()) : (geoAttributes["position"].data<int>())};
    if (mur > 10000) geoMurMap[mur]=position; //only for endcap case
  }

  /**
   * get the RODMUR which gives where the individual MURs are; this is an intermediate step, 
   * so make a temporary data structure.
   **/
  IntMap murPositionMap;
  SG::ReadCondHandle<CondAttrListVec> readHandleRodMur{m_readKeyRodMur, ctx};
  const CondAttrListVec* pRodMur{*readHandleRodMur};
  if (pRodMur==nullptr) {
    ATH_MSG_FATAL("Could not find RodMur configuration data: " << m_readKeyRodMur.key());
    return StatusCode::FAILURE;
  }
  CondAttrListVec::const_iterator rodMurIt{pRodMur->begin()};
  CondAttrListVec::const_iterator last_rodMur{pRodMur->end()};
  allInsertsSucceeded = true;
  std::set<int> tempRobSet2;
  for (; rodMurIt!=last_rodMur; ++rodMurIt) {
    const coral::AttributeList& rodMurAttributes{rodMurIt->second};
    int mur{isRun2 ? static_cast<int>(rodMurAttributes["MUR"].data<unsigned int>()) : (rodMurAttributes["MUR"].data<int>())};
    int crate{isRun2 ? static_cast<int>(rodMurAttributes["crate"].data<unsigned char>()) : (rodMurAttributes["crate"].data<int>())};
    int crateSlot{isRun2 ? static_cast<int>(rodMurAttributes["rod"].data<unsigned char>()) : (rodMurAttributes["rod"].data<int>())};//slot is int16, others are int32
    //map slot onto 0-15 range
    IntMap::const_iterator pSlot{slotMap.find(crateSlot)};
    int slot{(pSlot==slotMap.end()) ? -1 : pSlot->second};
    if (slot==-1) ATH_MSG_ERROR("Failed to find a crate slot in the crate map");
    //
    int order{isRun2 ? static_cast<int>(rodMurAttributes["position"].data<unsigned char>()) : (rodMurAttributes["position"].data<int>())};
    int fibreOrder{((((crate * slotsPerCrate) + slot ) * mursPerRod) + order) * fibresPerMur};
    bool thisInsertSucceeded{murPositionMap.insert(IntMap::value_type(mur, fibreOrder)).second};
    if (not thisInsertSucceeded) ATH_MSG_WARNING("Insert (mur, fibre) " << mur << ", " << fibreOrder << " failed.");
    allInsertsSucceeded = thisInsertSucceeded and allInsertsSucceeded;
  }
  if (not allInsertsSucceeded) ATH_MSG_WARNING("Some MUR-position map inserts failed.");

  // let's get the MUR AttrLists
  SG::ReadCondHandle<CondAttrListVec> readHandleMur{m_readKeyMur, ctx};
  const CondAttrListVec* pMur{*readHandleMur};
  if (pMur==nullptr) {
    ATH_MSG_FATAL("Could not find ROD configuration data: " << m_readKeyMur.key());
    return StatusCode::FAILURE;
  }

  // Add dependency
  writeHandle.addDependency(readHandleRod);
  writeHandle.addDependency(readHandleRodMur);
  writeHandle.addDependency(readHandleMur);
  writeHandle.addDependency(readHandleGeo);

  // Construct the output Cond Object and fill it in
  std::unique_ptr<SCT_CablingData> writeCdo{std::make_unique<SCT_CablingData>()};

  //build identifier map
  CondAttrListVec::const_iterator murIt{pMur->begin()};
  CondAttrListVec::const_iterator last_mur{pMur->end()};
  int numEntries{0};
  std::set<int> onlineIdSet, robSet;
  std::set<Identifier> offlineIdSet;
  long long lastSerialNumber{0};
  for (; murIt != last_mur; ++murIt) {
    const coral::AttributeList& murAttributes{murIt->second};
    int mur{isRun2 ? static_cast<int>(murAttributes["MUR"].data<unsigned int>()) : (murAttributes["MUR"].data<int>())};
    bool nullMur{murAttributes["moduleID"].isNull() or murAttributes["module"].isNull()};
    if (9999 == mur or nullMur) continue;
    int fibreInMur{ (isRun2 ? static_cast<int>(murAttributes["module"].data<unsigned char>()) : (murAttributes["module"].data<int>()) ) - 1};
    long long sn{murAttributes["moduleID"].data<long long>()};
    if (lastSerialNumber==sn) { //old version (<2.6) of Coral/Cool doesn't detect a 'null' value, instead it simply repeats the last known value.
      continue;
    }
    lastSerialNumber=sn;
    std::string snString{std::to_string(sn)};
    IntMap::const_iterator pFibre{murPositionMap.find(mur)};
    int fibreOffset{-1};
    if (pFibre==murPositionMap.end()) {
      ATH_MSG_WARNING("Failed to find an MUR position in the cabling");
    } else {
      fibreOffset = pFibre->second;
    }
    int encodedCrateSlot{fibreOffset / (fibresPerMur * mursPerRod)};
    int rob{-1};
    IntMap::const_iterator pCrate{crateSlot2RobMap.find(encodedCrateSlot)};
    if (pCrate == crateSlot2RobMap.end()) {
      ATH_MSG_WARNING("Failed to find a crate slot in the cabling, slot " << encodedCrateSlot);
    } else {
      rob=pCrate->second;
      tempRobSet2.insert(rob);
    }
    
    //cheat to get the geography: use the MUR notation
    int bec{0},eta{0}, layer{0}, phi{0};
    std::pair<int, int> etaPhiPair;
    int harnessPosition{-1};
    if (mur > 10000) { //its an endcap, so has 5 digits in the mur
      layer = (mur/10000) - 1; //first digit is the disk number, 1-9, so subtract 1
      bec = (((mur /1000) % 2)==0) ? ENDCAP_A : ENDCAP_C; //bec is +2 for A if second digit is 0
      int quadrant{(mur/100) % 10}; //3rd digit is quadrant
      //eta = ((mur /10) % 10);  //eta of the harness is the 4th digit
      //int harness = (mur % 10) -1; //last digit is harness, 1 or 2
      harnessPosition=geoMurMap[mur];
      etaPhiPair= convertToAthenaCoords(bec,layer, quadrant,  harnessPosition, fibreInMur);
      eta=etaPhiPair.first;
      phi=etaPhiPair.second;
    } else {
      bec = 0;
      layer= (mur /1000) - 3;//first digit is layer, 3-6
      phi= (mur % 100) - 1; //lower 2 digits are phi check whether this wraps around!
      eta = (((mur / 100) % 10) *2 -1) * (fibreInMur+1);//second digit gives eta sign, 0-> -, 1-> +
    }
    int rxLink[2];
    if (isRun2) {
      rxLink[0]=murAttributes["rx0Fibre"].data<unsigned char>();rxLink[1]= murAttributes["rx1Fibre"].data<unsigned char>();
    } else {
      rxLink[0]=murAttributes["rx0Fibre"].data<int>();rxLink[1]=murAttributes["rx1Fibre"].data<int>();
    }
    if (not (validLinkNumber(rxLink[0]) and validLinkNumber(rxLink[1]))) {
      ATH_MSG_WARNING("Invalid link number in database in one of these db entries: rx0Fibre=" << rxLink[0] << ", rx1Fibre=" << rxLink[1]);
      continue;
    }
    //normal ordering is like rx0=0, rx1=1; i.e. odd links in rx1.
    bool normalOrdering{true};
    if (rxLink[0] != disabledFibre) { //if the first link is not disabled, use that and check its even
      normalOrdering=(rxLink[0] % 2)==0;
    } else { //...otherwise use link1 and check its odd
      normalOrdering=(rxLink[1] % 2)==1;
    }
    
    int possibleLinks[2]{0,0};
    if (normalOrdering) {
      possibleLinks[0]=(rxLink[0]!=disabledFibre) ? rxLink[0] : (rxLink[1]-1);
      possibleLinks[1]=(rxLink[1]!=disabledFibre) ? rxLink[1] : (rxLink[0]+1);
    } else {
      possibleLinks[0]=(rxLink[0]!=disabledFibre) ? rxLink[0] : (rxLink[1]+1);
      possibleLinks[1]=(rxLink[1]!=disabledFibre) ? rxLink[1] : (rxLink[0]-1);
    }
    for (int side{0}; side!=2; ++side) {
      if ((-1==phi) and (-1==eta)) {
        ATH_MSG_WARNING("Invalid eta, phi..skipping insertion to map for module " << snString << " (may be already present in map)");
        continue;
      }
      Identifier offlineId{m_idHelper->wafer_id(bec,layer,phi,eta,side)};
      int link{rxLink[side]};
      if (defaultLink==link) {
        link = (fibreOffset % fibresPerRod) + fibreInMur*2 + side;
      }
      if (disabledFibre==link) {
        int otherLink{rxLink[1-side]};
        if (otherLink==possibleLinks[0]) link=possibleLinks[1];
        if (otherLink==possibleLinks[1]) link=possibleLinks[0];
      }
      bool flippedModule{isOdd(side)!=isOdd(link)};
      if (flippedModule) {
        link = (link==possibleLinks[0]) ? possibleLinks[1] : possibleLinks[0];
      }
      int onlineId{(rob & 0xFFFFFF)|(link << 24)};
      //check uniqueness
      if (not onlineIdSet.insert(onlineId).second) ATH_MSG_WARNING("Insert of online Id : " << onlineId << " failed.");
      if (not offlineIdSet.insert(offlineId).second) {
        ATH_MSG_WARNING("Insert of offline Id : " << offlineId << " failed.");
        ATH_MSG_WARNING(bec << " " << layer << " " << phi << " " << eta << " " << side);
        ATH_MSG_INFO("MUR, position " << mur << ", " << harnessPosition);
      } 
      IdentifierHash offlineIdHash{m_idHelper->wafer_hash(offlineId)};
      insert(offlineIdHash, onlineId, SCT_SerialNumber(sn), writeCdo.get());
      numEntries++;
    }
  }

  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_CablingData " << writeHandle.key() 
                  << " with EventRange " << writeHandle.getRange()
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  const int robLo{*(tempRobSet2.cbegin())};
  const int robHi{*(tempRobSet2.crbegin())};
  ATH_MSG_INFO(numEntries << " entries were made to the identifier map.");
  ATH_MSG_INFO(tempRobSet2.size() << " unique rob ids were used, spanning 0x" << std::hex << robLo << " to 0x" << robHi << std::dec);
  if (tempRobSet.size() != tempRobSet2.size()) {
    ATH_MSG_WARNING("The following existing rods were not inserted : ");
    std::cout << std::hex;
    std::set_difference(tempRobSet.cbegin(), tempRobSet.cend(),
                        tempRobSet2.cbegin(), tempRobSet2.cend(),
                        std::ostream_iterator<int>(std::cout,", "));
    std::cout << std::dec << std::endl;
  }
  tempRobSet.clear();
  return (numEntries==0) ? (StatusCode::FAILURE) : (StatusCode::SUCCESS);
}

//
bool
SCT_CablingCondAlgFromCoraCool::insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData* data) const {
  if (not sn.isWellFormed()) {
    ATH_MSG_FATAL("Serial number is not in correct format");
    return false;
  }
  if (not hash.is_valid()) {
    ATH_MSG_FATAL("Invalid hash: " << hash);
    return false;
  }
  // Check if the pointer of derived conditions object is valid.
  if (data==nullptr) {
    ATH_MSG_FATAL("Pointer of derived conditions object is null");
    return false;
  }
  
  if (not data->setHashForOnlineId(hash, onlineId)) return false;
  if (not data->setOnlineIdForHash(onlineId, hash)) return false;

  bool successfulInsert{data->setHashForSerialNumber(hash, sn)};
  successfulInsert &= data->setSerialNumberForHash(sn, hash);
  // in this form, the data->getHashEntries() will be half the number of hashes
  if (successfulInsert) {
    data->setRod(onlineId.rod()); //move this here so insertion only happens for valid onlineId, hash
  }
  return true;
}
