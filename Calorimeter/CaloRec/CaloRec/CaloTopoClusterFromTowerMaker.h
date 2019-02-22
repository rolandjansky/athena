// -*- c++ -*- 
/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERMAKER_H
#define CALOREC_CALOTOPOCLUSTERFROMTOWERMAKER_H

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h" 

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloTowerGeometrySvc.h"
#include "CaloRec/CaloProtoCluster.h"

#include "CaloEvent/CaloClusterCellLink.h"
#include "CaloEvent/CaloCellClusterWeights.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include <string>
#include <vector>
#include <bitset>
#include <map>

class CaloCell;
class CaloCellClusterWeights;
class CaloClusterCellLink;

#ifndef _CALOTOPOCLUSTERFROMTOWERMAKER_BITSET_SIZE
#define _CALOTOPOCLUSTERFROMTOWERMAKER_BITSET_SIZE 28
#endif

class CaloTopoClusterFromTowerMaker : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
public:

  ///@brief Tool constructor
  CaloTopoClusterFromTowerMaker(const std::string& type,const std::string& name,const IInterface* pParent);

  ///@name @c AthAlgTool and @c CaloClusterCellProcessor interface implementations
  ///@{
  StatusCode initialize();                                   ///< Setting up the operational mode and corresponding parameters
  StatusCode execute(xAOD::CaloClusterContainer* pClusCont); ///< Execute the tool and fill the @c xAOD::CaloClusterContainer pointed to by @c pClusCont
  StatusCode finalize();                                     ///< Finalize the tool (no action)
  ///@}

private:

  ///@name Internally used types
  ///@{
  typedef std::vector<CaloProtoCluster> protocont_t; ///< Container for @c CaloProtoCluster objects
  typedef std::size_t                   uint_t;      ///< Unsigned integral type
  ///@}

  /// @name Tool properties
  /// @{
  ServiceHandle<CaloTowerGeometrySvc>           m_towerGeometrySvc;         ///< Tower geometry service
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerKey;      ///< Topo-cluster container key
  SG::ReadHandleKey<CaloCellContainer>          m_cellContainerKey;         ///< Calorimeter cell container
  SG::WriteHandleKey<CaloCellClusterWeights>    m_cellClusterWeightKey;     ///< Cell signal weights in clusters key
  bool                                          m_orderByPt;                ///< Orders cluster container by @f$ p_{\text{T}} @f$, default @c true
  bool                                          m_prepareLCW;               ///< Prepare LCW calibration, default is @c false
  bool                                          m_useCellsFromClusters;     ///< Use cells from topo-clusters if @c true, else use all cells, default is @c true
  bool                                          m_applyCellEnergyThreshold; ///< Apply cell energy threshold, default is @c false 
  double                                        m_energyThreshold;          ///< Cell energy threshold, default is set in @c m_energyThresholdDef
  /// @}

  /// @name Constants and parameters
  /// @{
  uint_t             m_numberOfCells;          ///< Number of cells (highest cell index) 
  uint_t             m_numberOfSamplings;      ///< Number of samplings
  uint_t             m_numberOfTowers;         ///< Number of towers
  static double      m_energyThresholdDef;     ///< Default energy threshold
  static std::string m_defaultKey;             ///< Default container key
  /// @}

  ///@name Internally used helpers
  ///@{
  xAOD::CaloCluster::ClusterSize getClusterSize(uint_t etaBins,uint_t phiBins); ///< Returns a cluster size tag from number of eta and phi bins in tower grid
  xAOD::CaloCluster::ClusterSize getClusterSize(uint_t towerBins);              ///< Returns a cluster size tag from number of towers (bins) in tower grid
  int cleanupCells(CaloClusterCellLink* clk,uint_t nclus);                      ///< Checks @c CaloClusterCellLink for consistency
  ///@}

  ///@name Tower builders
  ///
  ///@return @c false in case of problems with data access or inconsistent data structures 
  ///
  ///@param pCellCont reference to non-modifiable @c CaloCellContainer
  ///@param pProtoCont reference to @c CaloProtoCluster container filled on output.
  ///@param clusCont reference to non-modifiable @c xAOD::CaloClusterContainer
  ///@param protoCont reference to modifiable proto-cluster container
  ///@{
  bool buildInclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont);           ///< Inclusive towers
  bool buildExclTowers(const CaloCellContainer& pCellCont,protocont_t& pProtoCont);           ///< Exclusive towers
  bool buildEMTopoTowers(const xAOD::CaloClusterContainer& clusCont,protocont_t& protoCont);  ///< EM topo-towers
  bool buildLCWTopoTowers(const xAOD::CaloClusterContainer& clusCont,protocont_t& protoCont); ///< LCW topo-towers
  ///@}
  /// @brief Adding cells to proto-clusters
  ///
  /// @return @c true if cell successfully added to one (or more) proto-clusters
  ///
  /// @param cptr       pointer ton non-modifiable @c CaloCell object
  /// @param pProtoCont reference to proto-cluster container
  bool addCellToProtoCluster(const CaloCell* cptr,protocont_t& pProtoCont);
  
  ///@name Helpers
  ///@{
  bool        filterProtoCluster(const CaloClusterCellLink& clnk) const;
  ///@}

  ///@brief Excluded sampling
  std::vector<CaloSampling::CaloSample>                       m_excludedSamplings;
  std::vector<std::string>                                    m_excludedSamplingsName;
  std::bitset< _CALOTOPOCLUSTERFROMTOWERMAKER_BITSET_SIZE >   m_exludedSamplingsPattern;
};

///@class CaloTopoClusterFromTowerMaker
///
/// @brief A cluster builder tool forming topo-clusters representing calorimeter tower signals on a regular grid in @f$ (\eta,\phi) @f$ space. By default,
///        EM-scale <i>topo-towers</i> are created from cells in topo-clusters.
///
/// This tool fills EM-scale towers and stores them as @c xAOD::CaloCluster. It supports several operational modes, which are
/// controlled by tool properties. It fills a container of type @c xAOD::CaloClusterContainer. The properties controlling its
/// specific behavior are:  
///
/// <table width="90%" align="center" style="border-width:0px;">
/// <tr><td align="center" colspan="4" style="background-color:rgb(245,245,220);color:rgb(165,42,42);"><b>Properties defining tool behavior</b></td></tr>
/// <tr style="color:rgb(131,42,34);">
/// <td align="left" width="25%">Property name</td>
/// <td align="center" width="25%">Property type</td>
/// <td align="center" width="25%">Default value</td>
/// <td align="left" width="25%">Comment</td>
/// </tr> 
/// <tr>
/// <td align="left" valign="top"><tt>OrderClusterByPt</tt></td>
/// <td align="center" valign="top"><tt>bool</tt></td>
/// <td align="center" valign="top"><tt>false</tt></td>
/// <td align="left" valign="top">if @c true, the @c xAOD::CaloClusterContainer is ordered by @f$ p_{\rm T}^{\rm clus} @f$.  See further comments below.</td>
/// </tr>
/// <tr>
/// <td align="left" valign="top"><tt>PrepareLCW</tt></td>
/// <td align="center" valign="top"><tt>bool</tt></td>
/// <td align="center" valign="top"><tt>false</tt></td>
/// <td align="left" valign="top">if @c true, the tool fills a @c CaloCellClusterWeights object and records it into the event store to be used by @c CaloTopoClusterFromTowerCalibrator</td>
/// </tr>
/// <tr>
/// <td align="left" valign="top"><tt>UseCellsFromClusters</tt></td>
/// <td align="center" valign="top"><tt>bool</tt></td>
/// <td align="center" valign="top"><tt>true</tt></td>
/// <td align="left" valign="top">if @c true, only cells from topo-clusters are used to fill the towers (<i>topo-towers</i>); else, @a inclusive @a towers are filled with all cells.</td>
/// </tr>
/// <tr><td align="center" colspan="4" style="background-color:rgb(245,245,220);color:rgb(165,42,42);"><b>Properties setting variables for operational modes</b></td></tr>
/// <tr style="color:rgb(131,42,34)">
/// <td align="left" width="25%">Property name</td>
/// <td align="center" width="25%">Property type</td>
/// <td align="center" width="25%">Default value</td>
/// <td align="left" width="25%">Comment</td>
/// </tr> 
/// <tr>
/// <td align="left" valign="top"><tt>CellEnergyThreshold</tt></td>
/// <td align="center" valign="top"><tt>double</tt></td>
/// <td align="center" valign="top"><tt>m_energyThresholdDef</tt></td>
/// <td align="left" valign="top">cell energy threshold used in exclusive mode only. See further comments below.</td>
/// </tr>
/// <tr>
/// <td align="left" valign="top"><tt>CellContainerKey</tt></td>
/// <td align="center" valign="top"><tt>SG::ReadHandleKey<CaloCellContainer></tt></td>
/// <td align="center" valign="top"><tt>"AllCalo"</tt></td>
/// <td align="left" valign="top">cell container key is needed to pick up @c CaloCellContainer for all operational modes.</td> 
/// </tr>
/// <tr>
/// <td align="left" valign="top"><tt>ClusterContainerKey</tt></td>
/// <td align="center" valign="top"><tt>SG::ReadHandleKey<xAOD::CaloClusterContainer></tt></td>
/// <td align="center" valign="top"><tt>"CaloTopoCluster"</tt></td>
/// <td align="left" valign="top">cluster container key is needed to pick up @c xAOD::CaloClusterContainer for filtered mode (<tt>UseCellsFromCluster = true</tt>)
/// </tr>
/// <tr>
/// <td align="left" valign="top"><tt>CellClusterWeightKey</tt></td>
/// <td align="center" valign="top"><tt>SG::WriteHandleKey<CaloCellClusterWeights></tt></td>
/// <td align="center" valign="top"><tt>&minus;N/A&minus;</tt></td>
/// <td align="left" valign="top">key for @c CaloCellClusterWeights object is needed if <tt>PrepareLCW = true</tt>. Default is empty key. 
/// </tr>
/// </table> 
///                                 
/// The towers can be classified as:
/// -# <b>inclusive cell towers</b>
///    All cells are collected into inclusive towers, independent of their signal. Requires properties <tt>UseCellsFromClusters = false</tt> and <tt>UseCellEnergyThreshold = false</tt>. Only EM
///    towers are possible, as cells not collected into topo-clustersdo not have a calibration applied.
/// -# <b>exclusive cell towers</b> 
///    Cells with @f$ E > E_{\rm min} @f$ are collected into exclusive towers. This behaviour is turned on by  <tt>UseCellsFromClusters = false</tt> and <tt>UseCellEnergyThreshold = true</tt>. A
///    meaningful <tt>CellEnergyThreshold</tt> value needs to be provided in addition.
/// -# <b>filtered mode</b>
///    Cells contributing to standard topo-clusters are collected into topo-towers. This behaviour is triggered by <tt>UseCellsFromClusters = true</tt>. Optionally, LCW calibration can be applied
///    to these towers by setting <tt>PrepareLCW = true</tt> and scheduling a @c CaloTopoClusterFromTowerCalibrator tool after the cluster moment calculators. The values of the <tt>UseEnergyThreshold</tt>
///    and <tt>CellEnergyThreshold</tt> properties are ignored in this mode. A valid event store key needs to be provided in the to pick up the topo-cluster container. 
///
///  Configuration 2 and 3 are exclusive, with 3 overwriting 2. The output topo-clusters represent calorimeter towers on the EM scale. The can be handed to cluster moment
///  tools (needs EM scale) and, if desired, to a dedicated cluster calibration tool of type @c xAOD::CaloTowerClusterFromTowerCalibrator .  
///
///  To avoid multiple retrievals of the same weights by searching for cells in (many) topo-clusters, the overall weight of the cell signal is stored in a random access
///  look-up table stored in a @c CaloCellClusterWeights object in the detector store. This object is created by this tool, if needed. If the tool property 
///  @c CellWeightLookupKey is set, this object will be generated, filled, and recorded. This is essential for calibrated topo-towers!
///
///@note The @c OrderByPt property, which orders the container by descending transverse momentum, is only useful for EM towers. Applyin LCW may lead to a different 
///      order - if a container with LCW towers should be ordered, the corresponding property @c OrderByPt of the @c CaloTopoClusterFromTowerCalibrator tool should
///      be set to @c true. 
///
///@note Many more details on the towers are available on 
///      <a href="https://twiki.cern.ch/twiki/bin/view/AtlasSandboxProtected/CaloTowerPerformance" title="https://twiki.cern.ch/twiki/bin/view/AtlasSandboxProtected/CaloTowerPerformance">this page</a>.
///
/// @author Peter Loch <loch@physics.arizona.edu>
///      
#endif
