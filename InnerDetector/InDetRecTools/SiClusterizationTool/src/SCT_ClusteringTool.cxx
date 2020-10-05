/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ClusterigTool.cxx
 * Implementation file for the SCT_ClusteringTool
 * @ author Paul J. Bell, 2004; Shaun Roe, 2007; Carl Gwilliam, 2008
 **/

#include "SiClusterizationTool/SCT_ClusteringTool.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT3_RawData.h"
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"
#include "SCT_ReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "SCT_ReadoutGeometry/SCT_ForwardModuleSideDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetPrepRawData/SiWidth.h"
#include "Identifier/IdentifierHash.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "SiClusterizationTool/SCT_ReClustering.h"

#include "GeoPrimitives/GeoPrimitives.h"

#include <algorithm>
#include <cmath>

namespace InDet {
  
  // Functor to enable sorting of RDOs by first strip
  class strip_less_than {
  public:
    bool operator()(const SCT_RDORawData* rdo1, const SCT_RDORawData* rdo2) {
      return rdo1->identify() < rdo2->identify();
    }
  };

  // Are two strips adjacent?
  bool adjacent(unsigned int strip1, unsigned int strip2) {
    return ((1 == (strip2-strip1)) or (1 == (strip1-strip2))); 
  }

  //version for when using rows
  bool adjacent(const unsigned int strip1, const int row1, const unsigned int strip2, const int row2){
    return ((row1==row2) and ((1 == (strip2-strip1)) or (1 == (strip1-strip2)))); 
  }
  
  // Constructor with parameters:
  SCT_ClusteringTool::SCT_ClusteringTool(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent)
  {
  }
  
  StatusCode SCT_ClusteringTool::decodeTimeBins() {
    // Decode time bins from string of form e.g. "01X" to integer bits (-1 means X)
    m_timeBinBits[0] = -1;
    m_timeBinBits[1] = -1;
    m_timeBinBits[2] = -1;

    if (m_timeBinStr.empty()) return StatusCode::SUCCESS;
    if (m_timeBinStr.size() != 3) {
      ATH_MSG_FATAL("Time bin string must only contain 3 bins");
      return StatusCode::FAILURE;
    }
    std::transform(m_timeBinStr.begin(), m_timeBinStr.end(), m_timeBinStr.begin(), ::toupper);

    for (unsigned int i(0); i<m_timeBinStr.size(); ++i) {
      int timeBin(-999);
      if (decodeTimeBin(m_timeBinStr[i], timeBin).isFailure()) return StatusCode::FAILURE;
      m_timeBinBits[i] = timeBin;
    }    

    return StatusCode::SUCCESS;
  }

  StatusCode SCT_ClusteringTool::decodeTimeBin(char timeBin, int& bit) const {
    // Decode individual time char
    if (timeBin == 'X') {bit = -1; return StatusCode::SUCCESS;}
    if (timeBin == '0') {bit =  0; return StatusCode::SUCCESS;}
    if (timeBin == '1') {bit =  1; return StatusCode::SUCCESS;}

    ATH_MSG_FATAL("Invalid time bin string " << timeBin);
    return StatusCode::FAILURE;
  }

  bool SCT_ClusteringTool::testTimeBins(int timeBin) const {
    // Convert the given timebin to a bit set and test each bit
    // if bit is -1 (i.e. X) it always passes, other wise require exact match of 0/1
    // N.B bitset has opposite order to the bit pattern we define

    const std::bitset<3> timePattern(static_cast<unsigned long>(timeBin));
    return testTimeBinsN(timePattern);
  }

  bool SCT_ClusteringTool::testTimeBins01X(int timeBin) const {
    // Convert the given timebin to a bit set and test each bit
    // if bit is -1 (i.e. X) it always passes, otherwise require exact match of 0/1
    // N.B bitset has opposite order to the bit pattern we define

    bool pass(true);
    const std::bitset<3> timePattern(static_cast<unsigned long>(timeBin));
    if (timePattern.test(2) != false) pass=false;
    if (timePattern.test(1) != true) pass=false;
    return pass;
  } 
  
  bool SCT_ClusteringTool::testTimeBinsX1X(int timeBin) const {
    // Convert the given timebin to a bit set and test each bit
    // if bit is -1 (i.e. X) it always passes, otherwise require exact match of 0/1
    // N.B bitset has opposite order to the bit pattern we define
    
    bool pass(true);
    const std::bitset<3> timePattern(static_cast<unsigned long>(timeBin));
    if (timePattern.test(1) != true) pass=false;
    return pass;
  }  

  StatusCode SCT_ClusteringTool::initialize() {
    ATH_MSG_INFO("Initialize clustering tool");

    ATH_CHECK(m_clusterMaker.retrieve());

    if (m_checkBadChannels) {
      ATH_MSG_INFO("Clustering has been asked to look at bad channel info");
      ATH_CHECK(m_conditionsTool.retrieve());
    } else {
      m_conditionsTool.disable();
    }
    
    if (m_doFastClustering and not m_lorentzAngleTool.empty()) {
      ATH_CHECK(m_lorentzAngleTool.retrieve());
    } else {
      m_lorentzAngleTool.disable();
    }

    if (decodeTimeBins().isFailure()) return StatusCode::FAILURE;

    /// only one of m_majority01X, m_innermostBarrelX1X and m_innertwoBarrelX1X
    /// can be true - check if this is the case
    /// If none of them is true, m_timeBinStr is used.
    int countTrueSettings(0);
    if (m_majority01X) countTrueSettings++;
    if (m_innermostBarrelX1X) countTrueSettings++;
    if (m_innertwoBarrelX1X) countTrueSettings++;
    if (countTrueSettings!=1) {
      if (!m_timeBinStr.empty()) {
        ATH_MSG_INFO("Timing requirement: m_timeBinStr " << m_timeBinStr << " is used for clustering");
      } else {
        if (countTrueSettings==0) {
          ATH_MSG_INFO("Timing requirement is not used for clustering");
        } else {
          ATH_MSG_FATAL("One and only one of m_majority01X, m_innermostBarrelX1X and m_innertwoBarrelX1X should be set to True!");
          return StatusCode::FAILURE;
        }
      }
    } else {
      ATH_MSG_INFO("Timing requirement: " <<
                   (m_majority01X        ? "m_majority01X"        : "") <<
                   (m_innermostBarrelX1X ? "m_innermostBarrelX1X" : "") <<
                   (m_innertwoBarrelX1X  ? "m_innertwoBarrelX1X"  : "") <<
                   " is true and used for clustering");
    }

    ATH_CHECK(m_SCTDetEleCollKey.initialize());

    return StatusCode::SUCCESS;
  }

  SCT_ClusterCollection* SCT_ClusteringTool::clusterize(const InDetRawDataCollection<SCT_RDORawData>& collection,
                                                        const SCT_ID& idHelper,
                                                        const SCT_ChannelStatusAlg* /*status */,
                                                        const bool /*CTBBadChannels */) const
  {
    ATH_MSG_INFO( "You have invoked the deprecated form of clusterize(...), please use the new interface, of the form  clusterize(InDetRawDataCollection<SCT_RDORawData> & collection,SCT_ID& idHelper)");
    return clusterize(collection, idHelper);
  }
  
  void SCT_ClusteringTool::addStripsToCluster(const Identifier& firstStripId, unsigned int nStrips,
                                              std::vector<Identifier>& clusterVector, const SCT_ID& idHelper) const{
    const unsigned int firstStripNumber(idHelper.strip(firstStripId));
    const unsigned int endStripNumber(firstStripNumber + nStrips); // one-past-the-end
    const Identifier   waferId(idHelper.wafer_id(firstStripId));
    clusterVector.reserve(clusterVector.size() + nStrips);

    for (unsigned int stripNumber(firstStripNumber); stripNumber not_eq endStripNumber; ++stripNumber) {
      const Identifier stripId(idHelper.strip_id(waferId, stripNumber));
      clusterVector.push_back(stripId);
    }
  }
  
  /**
   * Beware of corner cases: what if all strips are bad? the vector is empty?
   * What if the last strip is bad and contiguous with the next group which is coming? 
   * What if its good and contiguous, but there are also some bad?
   **/
  void SCT_ClusteringTool::addStripsToClusterWithChecks(const Identifier& firstStripId, unsigned int nStrips, std::vector<Identifier>& clusterVector,
                                                        std::vector<std::vector<Identifier> >& idGroups, const SCT_ID& idHelper) const{

    const unsigned int firstStripNumber(idHelper.strip(firstStripId));
    const unsigned int endStripNumber(firstStripNumber + nStrips); // one-past-the-end
    const Identifier   waferId(idHelper.wafer_id(firstStripId));
    clusterVector.reserve(clusterVector.size() + nStrips);

    static const Identifier badId;
    unsigned int nBadStrips(0);
    for (unsigned int stripNumber(firstStripNumber); stripNumber not_eq endStripNumber; ++stripNumber) {
      Identifier stripId(idHelper.strip_id(waferId, stripNumber));
      if (isBad(stripId)) {
        ++nBadStrips;
        stripId = badId;
      }
      clusterVector.push_back(stripId);
    }
    
    // Maybe all the strips are bad, clear the cluster vector
    if (clusterVector.size() == nBadStrips) {
      clusterVector.clear();
      // No need to recluster if vector is empty (same true if empty for other reasons)
      return;
    }

    // Now we have one vector of stripIds, some of which may be 'bad'
    if (nBadStrips != 0) {
      
      clusterVector=recluster(clusterVector, idGroups);
      // After this, the cluster vector is either empty or has the last good cluster
    }
  }

  void SCT_ClusteringTool::addStripsToClusterInclRows(const Identifier& firstStripId, unsigned int nStrips, std::vector<Identifier>& clusterVector,
                                                      std::vector<std::vector<Identifier> >& idGroups, const SCT_ID& idHelper) const {

    const unsigned int firstStripNumber(idHelper.strip(firstStripId));
    const unsigned int firstRowNumber(idHelper.row(firstStripId));
    const unsigned int endStripNumber(firstStripNumber + nStrips); // one-past-the-end
    const Identifier   waferId(idHelper.wafer_id(firstStripId));
    clusterVector.reserve(clusterVector.size() + nStrips);
    static const Identifier badId;
    unsigned int nBadStrips(0);
    for (unsigned int stripNumber(firstStripNumber); stripNumber not_eq endStripNumber; ++stripNumber) {
      Identifier stripId(idHelper.strip_id(waferId, firstRowNumber, stripNumber));
      if (isBad(stripId)) {
        ++nBadStrips;
        stripId = badId;
      }
      clusterVector.push_back(stripId);
    }
    
    // Maybe all the strips are bad, clear the cluster vector
    if (clusterVector.size() == nBadStrips) {
      clusterVector.clear();
      // No need to recluster if vector is empty (same true if empty for other reasons)
      return;
    }

    // Now we have one vector of stripIds, some of which may be 'bad'
    if (nBadStrips != 0) {
      clusterVector=recluster(clusterVector, idGroups);
      // After this, the cluster vector is either empty or has the last good cluster
    }
  } 

  
  /**
   * recluster starts with a vector of Ids, some of which may be invalid due to them being bad strips,
   * and a vector of these vectors. We recursively split the original clustervector, adding to idGroups as we go.
   * It should eventually either return an empty cluster or a cluster of all good strips, to be inserted by the caller.
   **/
  SCT_ClusteringTool::IdVec_t SCT_ClusteringTool::recluster(SCT_ClusteringTool::IdVec_t& clusterVector,
                                                            std::vector<SCT_ClusteringTool::IdVec_t>& idGroups) const {

    // Default Identifier constructor gives a sentinel value
    static const Identifier invalidId;
    const unsigned int numberOfBadStrips(std::count(clusterVector.begin(), clusterVector.end(), invalidId));

    // All the strips are good, return the original vector to be put in idGroups by the caller
    if (numberOfBadStrips==0 or clusterVector.empty()) return clusterVector;
    // All the strips are bad, clear the vector and return it
    if (clusterVector.size() == numberOfBadStrips) {
      clusterVector.clear();
      return clusterVector;
    }

    // Pointer to first bad strip
    SCT_ClusteringTool::IdVec_t::iterator pBadId(std::find(clusterVector.begin(), clusterVector.end(), invalidId));
    // Make a new cluster, which could be empty, if the first strip is bad
    SCT_ClusteringTool::IdVec_t subCluster(clusterVector.begin(), pBadId);
    // Remove elements including the badId
    if (pBadId != clusterVector.end()) clusterVector.erase(clusterVector.begin(), ++pBadId);
    if (not subCluster.empty()) idGroups.push_back(subCluster);
    return recluster(clusterVector, idGroups);
  }

  
  SCT_ClusterCollection * 
  SCT_ClusteringTool::clusterize(const InDetRawDataCollection<SCT_RDORawData>& collection,
                                 const SCT_ID& idHelper) const
  {
    ATH_MSG_VERBOSE ("SCT_ClusteringTool::clusterize()");

    if (m_doFastClustering) return fastClusterize(collection, idHelper);

    SCT_ClusterCollection* nullResult(nullptr);
    if (collection.empty()) {
      ATH_MSG_DEBUG("Empty RDO collection");
      return nullResult;
    }

    // Make a copy of the collection for sorting (no need to sort if theres only one RDO)
    std::vector<const SCT_RDORawData*> collectionCopy(collection.begin(), collection.end());
    if (collection.size() not_eq 1) std::sort(collectionCopy.begin(), collectionCopy.end(), strip_less_than());

    // Vector of identifiers in a cluster (most likely is that there is one strip in the cluster)
    IdVec_t currentVector;
    // Vector of clusters to make the cluster collection (most likely equal to collection size)
    std::vector<IdVec_t> idGroups;
    idGroups.reserve(collection.size());
    int n01X(0);
    int n11X(0);

    std::vector<uint16_t> tbinGroups;
    tbinGroups.reserve(collection.size());

    unsigned int previousStrip(0); // Should be ok?
    uint16_t hitsInThirdTimeBin(0);
    int stripCount(0);
    for (const SCT_RDORawData* pRawData: collectionCopy) {
      const Identifier      firstStripId(pRawData->identify());
      const unsigned int    nStrips(pRawData->getGroupSize());
      const int             thisStrip(idHelper.strip(firstStripId));
      const int             BEC(idHelper.barrel_ec(firstStripId));
      const int             layer(idHelper.layer_disk(firstStripId));

      // Flushes the vector every time a non-adjacent strip is found
      if (not adjacent(thisStrip, previousStrip) and not(currentVector.empty())) {
        if (m_majority01X) {
          if (n01X >= n11X) {
            idGroups.push_back(currentVector);
          }
        } else {
          // Add this group to existing groups (and flush)
          idGroups.push_back(currentVector);
        }
        currentVector.clear();
        n01X=0;
        n11X=0;
        tbinGroups.push_back(hitsInThirdTimeBin);
        hitsInThirdTimeBin =0;
        stripCount = 0;
      }

      // Only use clusters with certain time bit patterns if m_timeBinStr set
      bool passTiming(true);
      bool pass01X(true);
      bool passX1X(true);
      const SCT3_RawData* pRawData3(dynamic_cast<const SCT3_RawData*>(pRawData));
      if (!pRawData3) {
        ATH_MSG_ERROR("Casting into SCT3_RawData failed. This is probably caused by use of an old RDO file.");
        return nullptr;
      }
      const int timeBin(pRawData3->getTimeBin());
      std::bitset<3> timePattern(static_cast<unsigned long>(timeBin));
      if (not m_timeBinStr.empty()) passTiming = testTimeBins(timeBin);

      passX1X = testTimeBinsX1X(pRawData3->getTimeBin());
      if (passX1X) pass01X = testTimeBins01X(pRawData3->getTimeBin());
      if (pass01X) n01X++;
      if (passX1X and (not pass01X)) n11X++;
      if (m_innermostBarrelX1X) {
        if ((BEC==0) and (layer==0) and passX1X) passTiming=true;
        else passTiming = pass01X;
      } else if (m_innertwoBarrelX1X) {
        if ((BEC==0) and (layer==0 or layer==1) and passX1X) passTiming=true;
        else passTiming = pass01X;
      }

      // Now we are either (a) pushing more contiguous strips onto an existing vector
      //                or (b) pushing a new set of ids onto an empty vector
      if (passTiming or m_majority01X) {
        if (m_useRowInformation) {
          addStripsToClusterInclRows(firstStripId, nStrips, currentVector, idGroups, idHelper); // Note this takes the current vector only
        } else if (not m_checkBadChannels) {
          addStripsToCluster(firstStripId, nStrips, currentVector, idHelper); // Note this takes the current vector only
        } else {
          addStripsToClusterWithChecks(firstStripId, nStrips, currentVector, idGroups, idHelper); // This one includes the groups of vectors as well
        }
        for (unsigned int iStrip=0; iStrip<nStrips; iStrip++) {
          if (stripCount < 16) hitsInThirdTimeBin |= (timePattern.test(0) << stripCount);
          stripCount++;
        }
      }
      if (not currentVector.empty()) {
        // Gives the last strip number in the cluster
        previousStrip = idHelper.strip(currentVector.back());
      }
    } 
    
    // Still need to add this last vector
    if (not currentVector.empty()) {
      if ((not m_majority01X) or (n01X >= n11X)) {
        idGroups.push_back(currentVector);
        tbinGroups.push_back(hitsInThirdTimeBin);
        hitsInThirdTimeBin=0;
      }
    }

    // Find detector element for these digits
    const Identifier elementID(collection.identify());
    const Identifier waferId{idHelper.wafer_id(elementID)};
    const IdentifierHash waferHash{idHelper.wafer_hash(waferId)};
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* sctDetEle(*sctDetEleHandle);
    if (not sctDetEleHandle.isValid() or sctDetEle==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " is not available.");
      return nullResult;
    }
    const InDetDD::SiDetectorElement* element(sctDetEle->getDetectorElement(waferHash));
    if (!element) {
      ATH_MSG_WARNING("Element not in the element map, ID = "<< elementID);
      return nullResult;
    }

    const InDetDD::SCT_ModuleSideDesign* design;
    if (idHelper.is_barrel(elementID)) {
      design = (static_cast<const InDetDD::SCT_BarrelModuleSideDesign*>(&element->design()));
    } else {
      design = (static_cast<const InDetDD::SCT_ForwardModuleSideDesign*>(&element->design()));
    }

    IdentifierHash idHash(collection.identifyHash());
    SCT_ClusterCollection* clusterCollection = new SCT_ClusterCollection(idHash);
    clusterCollection->setIdentifier(elementID);
    clusterCollection->reserve(idGroups.size());

    int clusterNumber(0);
    // All strips are assumed to be the same width.
    std::vector<uint16_t>::iterator tbinIter(tbinGroups.begin());
    
    /// If clusters have been split due to bad strips, would require a whole lot
    /// of new logic to recalculate hitsInThirdTimeBin word - instead, just find
    /// when this is the case here, and set hitsInThirdTimeBin to zero later on
    const bool badStripInClusterOnThisModuleSide = (idGroups.size() != tbinGroups.size());

    for (const IdVec_t& stripGroup: idGroups) {
      const int nStrips(stripGroup.size());
      if (nStrips == 0) continue;
      //
      const InDetDD::SiLocalPosition dummyPos(1, 0);
      DimensionAndPosition clusterDim(dummyPos, 1.0);//just initialize with arbitrary values, will be set properly in the next two lines...
      if (m_useRowInformation) clusterDim = clusterDimensionsInclRow(idHelper.strip(stripGroup.front()), idHelper.strip(stripGroup.back()), idHelper.row(stripGroup.front()), element, design);
      else clusterDim = clusterDimensions(idHelper.strip(stripGroup.front()), idHelper.strip(stripGroup.back()), element, idHelper);
      const Amg::Vector2D localPos(clusterDim.centre.xPhi(), clusterDim.centre.xEta());
      // Since clusterId is arbitary (it only needs to be unique) just use ID of first strip
      //const Identifier clusterId = element->identifierOfPosition(clusterDim.centre);
      const Identifier clusterId(stripGroup.front());
      if (!clusterId.is_valid()) ATH_MSG_VERBOSE(clusterId << " is invalid.");
      //
      // Find length of strip at centre
      const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(clusterDim.centre));
      const double stripLength(std::abs(ends.first.xEta()-ends.second.xEta()));
      //
      // Now make a SiCluster
      clusterNumber++;
      const SiWidth siWidth(Amg::Vector2D(nStrips, 1), Amg::Vector2D(clusterDim.width, stripLength));
      
      SCT_Cluster* cluster = (m_clusterMaker) ? (m_clusterMaker->sctCluster(clusterId, localPos, stripGroup, siWidth, element, m_errorStrategy))
        : (new SCT_Cluster(clusterId, localPos, stripGroup, siWidth, element, 0));
      cluster->setHashAndIndex(clusterCollection->identifyHash(), clusterCollection->size());
      if (tbinIter != tbinGroups.end()) {
        cluster->setHitsInThirdTimeBin(*tbinIter);
        ++tbinIter;
      }
      if (badStripInClusterOnThisModuleSide) cluster->setHitsInThirdTimeBin(0); /// clusters had been split - recalculating HitsInThirdTimeBin too difficult to be worthwhile for this rare corner case..

      clusterCollection->push_back(cluster);
    }

    return clusterCollection;
  }

  SCT_ClusterCollection* SCT_ClusteringTool::fastClusterize(const InDetRawDataCollection<SCT_RDORawData>& collection,
                                                            const SCT_ID& idHelper) const
  {
    if (collection.empty()) return nullptr;

    std::vector<const SCT_RDORawData*> collectionCopy(collection.begin(), collection.end());

    if (collectionCopy.size() > 1) std::sort(collectionCopy.begin(), collectionCopy.end(), strip_less_than());

    IdVec_t currentVector;
    currentVector.reserve(100);

    std::vector<IdVec_t> idGroups;
    idGroups.reserve(collectionCopy.size());

    std::vector<uint16_t> tbinGroups;
    tbinGroups.reserve(collectionCopy.size());

    unsigned int previousStrip = 0; // Should be ok?
    uint16_t hitsInThirdTimeBin = 0;
    int stripCount  =  0;
    int previousRow = -1;
    int thisRow     = -1;

    const Identifier badId;

    for (const SCT_RDORawData* pRawData: collectionCopy) {
      Identifier firstStripId = pRawData->identify();
      Identifier waferId = idHelper.wafer_id(firstStripId);
      unsigned int nStrips = pRawData->getGroupSize();
      int thisStrip = idHelper.strip(firstStripId);

      if (m_useRowInformation) thisRow = idHelper.row(firstStripId);

      // Flushes the vector every time a non-adjacent strip is found
      //
      if (not currentVector.empty() and
          ((m_useRowInformation and !adjacent(thisStrip, thisRow, previousStrip, previousRow)) or 
           (not m_useRowInformation and !adjacent(thisStrip, previousStrip)))) {

        // Add this group to existing groups (and flush)
        //
        idGroups.push_back(currentVector);
        currentVector.clear();

        tbinGroups.push_back(hitsInThirdTimeBin);
        hitsInThirdTimeBin = 0;
        stripCount         = 0;
      }

      // Only use clusters with certain time bit patterns if m_timeBinStr set

      const SCT3_RawData* pRawData3 = dynamic_cast<const SCT3_RawData*>(pRawData);
      //sroe: coverity 31562
      if (!pRawData3) {
        ATH_MSG_ERROR("Casting into SCT3_RawData failed. This is probably caused by use of an old RDO file.");
        return nullptr;
      }

      int timeBin = pRawData3->getTimeBin();
      std::bitset<3> timePattern(static_cast<unsigned long>(timeBin));

      bool passTiming = true;

      if (!m_timeBinStr.empty()) passTiming = testTimeBinsN(timePattern);

      // Now we are either (a) pushing more contiguous strips onto an existing vector
      //                or (b) pushing a new set of ids onto an empty vector
      //
      if (passTiming) {
        unsigned int nBadStrips(0);
        for (unsigned int sn=thisStrip; sn!=thisStrip+nStrips; ++sn) {
          Identifier stripId = m_useRowInformation ? idHelper.strip_id(waferId,thisRow,sn) : idHelper.strip_id(waferId,sn);
          if (m_conditionsTool->isGood(stripId, InDetConditions::SCT_STRIP)) {
            currentVector.push_back(stripId);
          } else {
            currentVector.push_back(badId);
            ++nBadStrips;
          }
          if (stripCount < 16) {
            hitsInThirdTimeBin = hitsInThirdTimeBin | (timePattern.test(0) << stripCount);
          }
          ++stripCount;
        }

        if (currentVector.size() == nBadStrips) {
          currentVector.clear();
        } else if (nBadStrips) {
          currentVector=recluster(currentVector, idGroups);
        }
      }

      if (not currentVector.empty()) {
        // Gives the last strip number in the cluster
        //
        previousStrip = idHelper.strip(currentVector.back());
        if (m_useRowInformation) previousRow = idHelper.row(currentVector.back());
      }
    }

    // Still need to add this last vector
    //
    if (not currentVector.empty()) {
      idGroups.push_back(currentVector);
      tbinGroups.push_back(hitsInThirdTimeBin);
      hitsInThirdTimeBin=0;
    }

    // Find detector element for these digits
    //
    IdentifierHash idHash = collection.identifyHash();
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
    const InDetDD::SiDetectorElementCollection* sctDetEle(*sctDetEleHandle);
    if (not sctDetEleHandle.isValid() or sctDetEle==nullptr) {
      ATH_MSG_FATAL(m_SCTDetEleCollKey.fullKey() << " is not available.");
      return nullptr;
    }
    const InDetDD::SiDetectorElement* element(sctDetEle->getDetectorElement(idHash));
    if (!element) {
      ATH_MSG_WARNING("Element not in the element map, hash = " << idHash);
      return nullptr;
    }

    const InDetDD::SCT_ModuleSideDesign* design = (dynamic_cast<const InDetDD::SCT_ModuleSideDesign*>(&element->design()));
    if(design==nullptr) return nullptr;

    SCT_ClusterCollection* clusterCollection = new SCT_ClusterCollection(idHash);
    Identifier elementID = collection.identify();
    clusterCollection->setIdentifier(elementID);
    clusterCollection->reserve(idGroups.size());

    int clusterNumber = 0;

    // All strips are assumed to be the same width.
    //
    std::vector<IdVec_t>::iterator pGroup = idGroups.begin();
    std::vector<IdVec_t>::iterator lastGroup = idGroups.end();
    std::vector<uint16_t>::iterator tbinIter = tbinGroups.begin();

    // If clusters have been split due to bad strips, would require a whole lot
    // of new logic to recalculate hitsInThirdTimeBin word - instead, just find
    // when this is the case here, and set hitsInThirdTimeBin to zero later on
    //
    double iphipitch  = 1./element->phiPitch();
    double shift = m_lorentzAngleTool->getLorentzShift(idHash);
    double stripPitch = design->stripPitch();
    bool badStripInClusterOnThisModuleSide = (idGroups.size() != tbinGroups.size());
    bool rotate = (element->design().shape() == InDetDD::Trapezoid || element->design().shape() == InDetDD::Annulus);
    double stripL = 0.;
    double COV11 = 0.;

    if (not rotate) {
      stripL = design->etaPitch();
      COV11 = stripL*stripL*(1./12.);
    }

    for (; pGroup!=lastGroup; ++pGroup) {
      int nStrips = pGroup->size();
      double dnStrips = static_cast<double>(nStrips);
      Identifier clusterId = pGroup->front();

      int firstStrip = idHelper.strip(clusterId);
      double width = stripPitch;
      InDetDD::SiLocalPosition centre;
      if (m_useRowInformation) {
        int row = idHelper.row(clusterId);
        centre = element->rawLocalPositionOfCell(design->strip1Dim(firstStrip, row));
        if (nStrips > 1) {
          InDetDD::SiLocalPosition lastStripPos(element->rawLocalPositionOfCell(design->strip1Dim(firstStrip+nStrips-1, row)));
          centre = (centre+lastStripPos)*.5;
          width *=dnStrips;
        }
      } else {
        DimensionAndPosition clusterDim = clusterDimensions(idHelper.strip(pGroup->front()), idHelper.strip(pGroup->back()), element, idHelper);
        centre = clusterDim.centre;
        width  = clusterDim.width;
      }

      Amg::Vector2D localPos{centre.xPhi(), centre.xEta()};
      Amg::Vector2D locpos{centre.xPhi()+shift, centre.xEta()};

      // Now make a SiCluster
      //
      double x = 0.;
      // single strip - resolution close to pitch/sqrt(12)
      // two-strip hits: better resolution, approx. 40% lower
      // lines taken from ClusterMakerTool::sctCluster
      if (nStrips == 1) {
        x = 1.05*width;
      } else {
        if (nStrips == 2) {
          x = 0.27*width;
        } else {
          x = width;
        }
      }

      double V[4] = {x*x*(1./12.), 0., 0., COV11};

      if (rotate) {
        // Find length of strip at centre
        //
        std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(centre));
        stripL = std::abs(ends.first.xEta()-ends.second.xEta());

        double w       = element->phiPitch(localPos)*iphipitch;
        double sn      = element->sinStereoLocal(localPos);
        double sn2     = sn*sn;
        double cs2     = 1.-sn2;
        double v0      = V[0]*w*w;
        double v1      = stripL*stripL*(1./12.);
        V[0]           = cs2*v0+sn2*v1;
        V[1]  =  V[2]  = sn*sqrt(cs2)*(v0-v1);
        V[3]           = sn2*v0+cs2*v1;
      }

      Amg::MatrixX* errorMatrix(new Amg::MatrixX(2,2));
      *errorMatrix<<V[0],V[1],V[2],V[3];

      SiWidth siWidth{Amg::Vector2D(dnStrips,1.), Amg::Vector2D(width,stripL)};

      SCT_Cluster* cluster = new SCT_Cluster{clusterId, locpos, *pGroup , siWidth, element, errorMatrix};

      cluster->setHashAndIndex(idHash, clusterNumber);

      if (tbinIter != tbinGroups.end()) {
        cluster->setHitsInThirdTimeBin(*tbinIter);
        ++tbinIter;
      }

      // clusters had been split - recalculating HitsInThirdTimeBin too difficult to be worthwhile for this rare corner case..
      //
      if (badStripInClusterOnThisModuleSide) cluster->setHitsInThirdTimeBin(0);

      clusterCollection->push_back(cluster);
      clusterNumber++;
    }
    return clusterCollection;
  }

  SCT_ClusteringTool::DimensionAndPosition 
  SCT_ClusteringTool::clusterDimensions(int firstStrip, int lastStrip,
                                        const InDetDD::SiDetectorElement* pElement,
                                        const SCT_ID& /*idHelper*/) const {  //since a range check on strip numbers was removed, idHelper is no longer needed here
    const int                      nStrips(lastStrip - firstStrip + 1); 
    // Consider strips before and after (in case nStrips=1), both are guaranteed 
    // to return sensible results, even for end strips
    const InDetDD::SiCellId        cell1(firstStrip - 1);
    const InDetDD::SiCellId        cell2(lastStrip + 1);
    const InDetDD::SiLocalPosition firstStripPos(pElement->rawLocalPositionOfCell(cell1));
    const InDetDD::SiLocalPosition lastStripPos(pElement->rawLocalPositionOfCell(cell2));
    const double                   width((static_cast<double>(nStrips)/static_cast<double>(nStrips+1))*( lastStripPos.xPhi()-firstStripPos.xPhi()));
    const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)*0.5);
    return SCT_ClusteringTool::DimensionAndPosition(centre, width);
  }

  SCT_ClusteringTool::DimensionAndPosition 
  SCT_ClusteringTool::clusterDimensionsInclRow(int firstStrip, int lastStrip, int row,
                                               const InDetDD::SiDetectorElement* pElement, const InDetDD::SCT_ModuleSideDesign* design) const{
    const int                      nStrips(lastStrip - firstStrip + 1); 
    const int firstStrip1D = design->strip1Dim(firstStrip, row);
    const int lastStrip1D = design->strip1Dim(lastStrip, row);
    const double stripPitch = design->stripPitch();
    const InDetDD::SiCellId        cell1(firstStrip1D); 
    const InDetDD::SiCellId        cell2(lastStrip1D);  
    const InDetDD::SiLocalPosition firstStripPos(pElement->rawLocalPositionOfCell(cell1));
    const InDetDD::SiLocalPosition lastStripPos(pElement->rawLocalPositionOfCell(cell2));
    const double                   width(nStrips*stripPitch);
    const InDetDD::SiLocalPosition centre((firstStripPos+lastStripPos)*0.5);
    return SCT_ClusteringTool::DimensionAndPosition(centre, width);
  } 
}
