/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ClusteringTool.h
 *  Header file for SCT_ClusteringTool
 *
 */
#ifndef SiClusterizationTool_SCT_ClusteringTool_H
#define SiClusterizationTool_SCT_ClusteringTool_H

/// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetConditionsSummaryService/IInDetConditionsTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"
#include "SCT_ReadoutGeometry/SCT_ChipUtils.h"

#include "SiClusterizationTool/ISCT_ClusteringTool.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "StoreGate/ReadCondHandleKey.h"

/// Gaudi
#include "GaudiKernel/ToolHandle.h"

/// STL
#include <bitset>
#include <string>
#include <vector>

class SCT_ChannelStatusAlg;
class StatusCode;

namespace InDetDD{
  class SCT_ModuleSideDesign;
}

namespace InDet {
  /**
   * @class SCT_ClusteringTool
   * @brief AlgTool for SCT_Clusterization.
   * Input is from RDOs, assumed to be sorted. They are then scanned 
   * in order and neighbouring RDOs are grouped together.
   */
  class SCT_ClusteringTool: public extends<AthAlgTool, ISCT_ClusteringTool> {
  public:
    /// Normal constructor for an AlgTool; 'properties' are also declared here
    SCT_ClusteringTool(const std::string& type, const std::string& name, const IInterface* parent);
    /// Retrieve the necessary services in initialize                
    StatusCode initialize();
    /// Defaul destructor
    virtual ~SCT_ClusteringTool() = default;

    /// Clusterize method the SCT RDOs. This method is the main one of this class.
    virtual SCT_ClusterCollection*
    clusterize(const InDetRawDataCollection<SCT_RDORawData>& RDOs,
               const SCT_ID& idHelper,
               const InDet::SiDetectorElementStatus *sctDetElementStatus) const;

    /// A new fast method originally implemented for ITk. Can be internally used in clusterize with m_doFastClustering=true.
    virtual SCT_ClusterCollection*
      fastClusterize(const InDetRawDataCollection<SCT_RDORawData>& RDOs,
                     const SCT_ID& idHelper,
                     const InDet::SiDetectorElementStatus *sctDetElementStatus) const;
    
  private:
    typedef std::vector<Identifier> IdVec_t;

    /**
     * @name Tool Handles
     */
    //@{
    ToolHandle<IInDetConditionsTool> m_conditionsTool{this, "conditionsTool", "SCT_ConditionsSummaryTool/InDetSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
    ToolHandle<ClusterMakerTool> m_clusterMaker{this, "globalPosAlg", "InDet::ClusterMakerTool"};
    ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool{this, "LorentzAngleTool", "SiLorentzAngleTool/SCTLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};
    //@}

    /**
     * @name Flags to configure SCT_ClusteringTool
     */
    //@{
    IntegerProperty m_errorStrategy{this, "errorStrategy", 1};
    BooleanProperty m_checkBadChannels{this, "checkBadChannels", true};
    StringProperty m_timeBinStr{this, "timeBins", ""};
    BooleanProperty m_innermostBarrelX1X{this, "innermostBarrelX1X", false};
    BooleanProperty m_innertwoBarrelX1X{this, "innertwoBarrelX1X", false};
    BooleanProperty m_majority01X{this, "majority01X", false};
    BooleanProperty m_useRowInformation{this, "useRowInformation", false};
    BooleanProperty m_doFastClustering{this, "doFastClustering", true};
    //@}

    /**
     * @name ReadCondHandleKeys
     */
    //@{
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT. Necessary for alignment"};
    //@}
    /** @brief Optional read handle to get status data to test whether a SCT detector element is good.
     * If set to e.g. SCTDetectorElementStatus the event data will be used instead of the SCT conditions summary tool.
     */
    SG::ReadHandleKey<InDet::SiDetectorElementStatus> m_sctDetElStatus
       {this, "SCTDetElStatus", "", "Key of SiDetectorElementStatus for SCT"};

    /// Time bin bits for timing requirement. Set by decodeTimeBins() in initialize().
    int m_timeBinBits[3]{-1, -1, -1};

    /// Add strips to a cluster vector without checking for bad strips
    static void addStripsToCluster(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector, const SCT_ID& idHelper) ;

    /// Add strips to a cluster vector checking for bad strips
    void addStripsToClusterWithChecks(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector,
				      std::vector<IdVec_t>& idGroups, const SCT_ID& idHelper,
                                      const InDet::SiDetectorElementStatus *det_el_status) const;

    /// Add strips to a cluster vector including row variable for ITk
    void addStripsToClusterInclRows(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector,
                                    std::vector<IdVec_t>& idGroups, const SCT_ID& idHelper,
                                    const InDet::SiDetectorElementStatus *det_el_status) const;

    /**
     * Recluster the current vector, splitting on bad strips, and insert those new groups to the idGroups vector.
     * The cluster vector referenced will be changed by this, as well as the idGroups
     **/
    IdVec_t recluster(IdVec_t& clusterVector, std::vector<IdVec_t>& idGroups) const;

    /// In-class struct to store the centre and width of a cluster
    struct DimensionAndPosition {
      InDetDD::SiLocalPosition centre;
      double width;
      DimensionAndPosition(const InDetDD::SiLocalPosition& c, double w):centre(c), width(w){}
    };
    
    /// Calculate the cluster position and width given the first and last strip numbers for this element
    static DimensionAndPosition clusterDimensions(int firstStrip, int lastStrip, const InDetDD::SiDetectorElement* element,
					   const SCT_ID& idHelper) ;

    /// Calculate the cluster position and width given the first,last strip, and row numbers  for this element for ITk.
    static DimensionAndPosition clusterDimensionsInclRow(int firstStrip, int lastStrip, int row,
                                                  const InDetDD::SiDetectorElement* element, const InDetDD::SCT_ModuleSideDesign* design) ;
  
    /// In-class facade on the 'isGood' method for a strip identifier
    bool isBad(const InDet::SiDetectorElementStatus *sctDetElStatus, const SCT_ID& sctID, const IdentifierHash &waferHash, const Identifier& stripId) const;

    /// Convert time bin string to array of 3 bits
    StatusCode decodeTimeBins();
    /// Convert a single time bin char to an int, bit is modified
    StatusCode decodeTimeBin(char timeBin, int& bit) const;
    /// @name Test the clusters time bin to see if matches pattern
    //@{
    bool       testTimeBins(int timeBin) const;
    static bool       testTimeBins01X(int timeBin) ;
    static bool       testTimeBinsX1X(int timeBin) ;
    bool       testTimeBinsN(const std::bitset<3>& timePattern) const;
    //@}
  };//end of class  

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline bool SCT_ClusteringTool::isBad(const InDet::SiDetectorElementStatus *sctDetElStatus, const SCT_ID& sctID, const IdentifierHash &waferHash, const Identifier& stripId) const {
     if (sctDetElStatus) {
        const int strip_i{sctID.strip(stripId)};
        VALIDATE_STATUS_ARRAY(sctDetElStatus,sctDetElStatus->isCellGood(waferHash.value(), strip_i), m_conditionsTool->isGood(stripId, InDetConditions::SCT_STRIP));
        return !sctDetElStatus->isCellGood(waferHash.value(), strip_i) ;
     }
     else {
        return (not m_conditionsTool->isGood(stripId, InDetConditions::SCT_STRIP));
     }
  }
  
  inline bool SCT_ClusteringTool::testTimeBinsN(const std::bitset<3>& timePattern) const {
    // Convert the given timebin to a bit set and test each bit
    // if bit is -1 (i.e. X) it always passes, other wise require exact match of 0/1
    // N.B bitset has opposite order to the bit pattern we define
    
    if (m_timeBinBits[0] != -1 and timePattern.test(2) != static_cast<bool>(m_timeBinBits[0])) return false;
    if (m_timeBinBits[1] != -1 and timePattern.test(1) != static_cast<bool>(m_timeBinBits[1])) return false;
    if (m_timeBinBits[2] != -1 and timePattern.test(0) != static_cast<bool>(m_timeBinBits[2])) return false;
    return true;
  }

}//end of namespace
#endif // SiClusterizationTool_SCT_ClusteringTool_H
