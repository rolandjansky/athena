/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file SCT_ClusteringTool.h
 *  Header file for SCT_ClusteringToo
 *
 */
#ifndef SiClusterizationTool_SCT_ClusteringTool_H
#define SiClusterizationTool_SCT_ClusteringTool_H

//STL
#include <vector>
#include <string>
//Gaudi
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
//Athena
#include "Identifier/Identifier.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"
#include "SiClusterizationTool/ClusterMakerTool.h"

class SCT_ID;
class SCT_ChannelStatusAlg;
class StatusCode;
class IInDetConditionsSvc;

namespace InDetDD{
  class SCT_ModuleSideDesign;
}

namespace InDet {
  /**  AlgTool for SCT_Clusterization.
   * Input is from RDOs, assumed to be sorted. They are then scanned 
   * in order and neighbouring RDOs are grouped together.
   */
  class SCT_ClusteringTool: public AthAlgTool, virtual public ISCT_ClusteringTool{
  public:
    ///Normal constructor for an AlgTool; 'properties' are also declared here
    SCT_ClusteringTool(const std::string &type, const std::string &name, const IInterface *parent);    
    ///Retrieve the necessary services in initialize                
    StatusCode initialize();
    virtual ~SCT_ClusteringTool() {};

    /// Clusterize the SCT RDOs... deprecated form passes explicit channel status object
    virtual SCT_ClusterCollection *
    clusterize( const InDetRawDataCollection<SCT_RDORawData> & RDOs,
                const InDetDD::SiDetectorManager& manager,
                const SCT_ID& idHelper,
                const SCT_ChannelStatusAlg* status,
                const bool CTBBadChannels) const;
    
    /// Clusterize the SCT RDOs...
    virtual SCT_ClusterCollection *
    clusterize( const InDetRawDataCollection<SCT_RDORawData> & RDOs,
                const InDetDD::SiDetectorManager& manager,
                const SCT_ID& idHelper) const;
    
  private:
    //    mutable MsgStream                         m_log;
    int                                       m_errorStrategy;
    bool                                      m_checkBadChannels;
    //ServiceHandle<SCT_ConditionsSummarySvc> m_conditionsSvc;
    ServiceHandle<IInDetConditionsSvc>        m_conditionsSvc;
    ToolHandle< ClusterMakerTool >            m_clusterMaker;
    typedef std::vector<Identifier>           IdVec_t;
    std::string                               m_timeBinStr;
    int                                       m_timeBinBits[3];     
    bool                                      m_innermostBarrelX1X;
    bool                                      m_innertwoBarrelX1X;
    bool                                      m_majority01X;
    bool                                      m_useRowInformation;
    
    ///Add strips to a cluster vector without checking for bad strips
    void  addStripsToCluster(const Identifier & firstStripId, const unsigned int nStrips,IdVec_t & clusterVector,const SCT_ID& idHelper) const;
                                  
    ///Add strips to a cluster vector checking for bad strips
    void addStripsToClusterWithChecks(const Identifier & firstStripId, const unsigned int nStrips, IdVec_t & clusterVector, 
				      std::vector<IdVec_t > & idGroups, const SCT_ID& idHelper) const;

    void addStripsToClusterInclRows(const Identifier & firstStripId, const unsigned int nStrips, IdVec_t & clusterVector,std::vector<IdVec_t > & idGroups, const SCT_ID& idHelper) const;

    /** Recluster the current vector, splitting on bad strips, and insert those new groups to the idGroups vector.
     * The cluster vector referenced will be changed by this, as well as the idGroups
     **/
    IdVec_t recluster(IdVec_t & clusterVector, std::vector<IdVec_t > & idGroups) const;

    /// In-class struct to store the centre and width of a cluster                                
    struct DimensionAndPosition{
      InDetDD::SiLocalPosition centre;
      double width;
      DimensionAndPosition(const InDetDD::SiLocalPosition c, const double w):centre(c), width(w){}
    };
    
    ///Calculate the cluster position and width given the first and last strip numbers for this element
    DimensionAndPosition clusterDimensions(const int firstStrip, const int lastStrip, const InDetDD::SiDetectorElement* element,
					   const SCT_ID& idHelper) const;  

    DimensionAndPosition clusterDimensionsInclRow(const int firstStrip, const int lastStrip, const int row, const InDetDD::SiDetectorElement* element, const InDetDD::SCT_ModuleSideDesign* design) const;
  
    /// In-class facade on the 'isGood' method for a strip identifier
    bool isBad(const Identifier & stripId) const;   

    // Convert time bin string to array of 3 bits
    StatusCode decodeTimeBins();
    // Convert a single time bin char to an int
    StatusCode decodeTimeBin(const char& timeBin, int& bit);    
    // Test the clusters time bin to see if matches pattern
    bool       testTimeBins(int timeBin) const;
    bool       testTimeBins01X(int timeBin) const;
    bool       testTimeBinsX1X(int timeBin) const;

  };//end of class  
}//end of namespace
#endif // SiClusterizationTool_SCT_ClusteringTool_H
