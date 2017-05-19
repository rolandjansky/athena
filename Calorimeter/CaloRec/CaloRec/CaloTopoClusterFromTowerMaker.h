// -*- c++ -*- 
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERMAKER_H
#define CALOREC_CALOTOPOCLUSTERFROMTOWERMAKER_H

#include "AthenaBaseComps/AthAlgTool.h" 

#include "CaloEvent/CaloCellTowerLink.h"

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloProtoCluster.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloTowerContainer.h"

#include <string>
#include <map>

#include <vector>
#include <boost/tuple/tuple.hpp>

class CaloClusterCellLink;
class CaloCell;
class CaloCellClusterWeights;

class CaloTopoClusterFromTowerMaker : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
public:

  /// @brief Tool constructor
  CaloTopoClusterFromTowerMaker(const std::string& type,const std::string& name,const IInterface* pParent);

  /// @brief Tool initialization
  StatusCode initialize();

  /// @brief Tool execution
  StatusCode execute(xAOD::CaloClusterContainer* pClusCont);

  /// @brief Tool finalize
  StatusCode finalize();

private:

  /// @name Tool properties
  /// @{
  std::string m_towerContainerKey;    ///> @brief Key (name) of input tower container
  std::string m_cellTowerLinksKey;    ///> @brief Derived property (cannot be set by user)
  std::string m_cellContainerKey;     ///> @brief Key (name) of input cell container 
  std::string m_clusterContainerKey;  ///> @brief Key of topo-cluster container to be used as source for cells
  std::string m_cellClusterWeightKey; ///> @brief Key for cell-cluster weight look up object
  bool        m_orderByPt;            ///> @brief Orders cluster container by @f$ p_{\text{T}} @f$, default @c true
  double      m_energyThreshold;      ///> @brief Cell energy threshold
  bool        m_applyLCW;             ///> @brief Prepare LCW calibration
  /// @}

  /// @name Processing flags
  /// @{
  /// @brief Energy threshold flag
  bool        m_applyEnergyThreshold;
  /// @brief Use cells from topo-clusters flag
  bool        m_useCellsFromClusters; 
  /// @brief Prepare LCW calibration
  bool        m_prepareLCW;
  /// @}

  /// @name Constants and parameters
  /// @{
  /// @brief Number of cells
  int         m_numberOfCells;
  /// @brief Number of samplings
  int         m_numberOfSamplings;
  /// @brief Default energy threshold
  static double m_energyThresholdDef;
  /// @brief Default container key
  static std::string m_defaultKey;
  /// @}

  /// @brief Access to cell-to-tower links
  const CaloCellTowerLink::Map* m_cellTowerLinksHandle;

  /// @name Cache for cell weights
  /// @{
  /// @brief Data structure tagging a cell as a cluster cell and storing its weight
  typedef boost::tuples::tuple<bool,double> celltag_t;      
  /// @brief Random access lookup of cell tags
  typedef std::vector<celltag_t>            celltagstore_t;
  /// @brief Cell tag store
  celltagstore_t m_cellTagStore;
  /// @brief Previously used cell indices
  std::vector<size_t> m_cellIdxStore;
  /// @}

  /// @brief Pathologies
  ///
  /// Internally used object collects pathologies observed for @c CaloCell objects in execution.
  /// Each pathology is identified by a pre-defined message, and the number of pathologies is produced by
  /// counting the number of occurancies of a given message.
  class CellPathology
  {
  public:
    /// @name Internally used data types
    /// @{
    typedef std::string                   pathology_t;
    typedef int                           count_t;
    typedef std::map<pathology_t,count_t> cache_t;
    /// @}

    /// @brief Constructor
    CellPathology();
    /// @name Set pathologies
    /// @{
    /// @brief Invalid @c CaloCell pointer 
    ///
    /// An invalid @c NULL  pointer was found at a given valid index in the @c CaloCellContainer. 
    ///
    /// @param cellhash hash identifier (index in @c CaloCellContainer) for a given @c CaloCell. 
    /// @param cptr     cell pointer (should be @c NULL in this case)
    void setInvalidPointer(int cellhash,const CaloCell* cptr);
    /// @brief Invalid @c CaloCell hash
    ///
    /// A cell identifier is found to be out of range.
    ///
    /// @param cellhash hash identifier (index in @c CaloCellContainer - here invalid!) for a given @c CaloCell. 
    /// @param cptr     cell pointer (should be @c NULL in this case)
    void setInvalidHash(int cellhash,const CaloCell* cptr);
    /// @brief Invalid sampling identifier for given @c CaloCell
    ///
    /// The sampling identifier associated with a @c CaloCell is not valid (not in the enumerator list).
    ///
    /// @param cellhash hash identifier (index in @c CaloCellContainer - here invalid!) for a given @c CaloCell. 
    /// @param cptr     cell pointer (should be @c NULL in this case)
    void setInvalidSampling(int cellhash,const CaloCell* cptr);
    /// @}

    /// @name Accessors to pathologies stats
    /// @{
    const cache_t& invalidPointer()  const; ///< @brief Occurances of invalid pointers
    const cache_t& invalidHash()     const; ///< @brief Occurances of invalid hash identifier
    const cache_t& invalidSampling() const; ///< @brief Occurances of invalid sampling identifier
    /// @}

  private:

    /// @name Caches
    /// @{
    cache_t _invalidPointer;    ///< @brief Invalid pointer
    cache_t _invalidHash;       ///< @brief Invalid hash
    cache_t _invalidSampling;   ///< @brief Invalid sampling
    /// @}

    /// @name Helpers
    /// @{
    /// @brief Add a pathology to the cache
    ///
    /// @param msg message describing pathology
    /// @param map cache
    void _addMsg(const pathology_t& msg,cache_t& map);
    /// @}
  };

  /// @brief Cluster size tag 
  xAOD::CaloCluster::ClusterSize getClusterSize(const xAOD::CaloTowerContainer& towerCont);

  /// @brief Clean up cell links - remove null pointers
  int cleanupCells(CaloClusterCellLink* clk);

  //  /// @brief Calculate kinemactics
  //  bool calculateKine(xAOD::CaloCluster* pClus);

  /// @brief Book keeping
  void addWarning(const std::string& msg);
  std::map<std::string,unsigned int> m_warning;
  void addMessage(const std::string& msg);
  std::map<std::string,unsigned int> m_message;
  CellPathology m_cellProblems;
  void printProblems(const std::map<std::string,int>& map);

  typedef std::vector<CaloProtoCluster*> protocont_t;

  /// @name Helpers
  /// @{
  /// @brief Build inclusive tower clusters
  ///
  /// Fills all cells into proto-clusters.
  ///
  /// @return @c false in case of problems with data access or inconsistent data structures.
  ///
  /// @param pCellCont reference to non-modifiable @c CaloCellContainer
  /// @param pProtoCont reference to @c CaloProtoCluster container filled on output.
  ///
  bool buildInclClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont);     
  /// @brief Build exclusive tower clusters
  ///
  /// Fills cells above energy threshold into proto-clusters.
  ///
  /// @return @c false in case of problems with data access or inconsistent data structures.
  ///
  /// @param pCellCont reference to non-modifiable @c CaloCellContainer
  /// @param pProtoCont reference to @c CaloProtoCluster container filled on output.
  bool buildExclClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont);     
  //  bool buildFilteredClusters(const CaloCellContainer& pCellCont,protocont_t& pProtoCont); ///< @brief Build tower clusters from topo-cluster cells
  /// @brief Add cells to proto-clusters
  ///
  /// @return @c true if cell successfully added to one (or more) proto-clusters
  ///
  /// @param cptr       pointer ton non-modifiable @c CaloCell object
  /// @param pProtoCont reference to proto-cluster container
  bool addCellToProtoCluster(const CaloCell* cptr,protocont_t& pProtoCont);
  /// @}

  /// @brief Cache
  CaloCellClusterWeights* m_cellClusterWeights;
};

inline void CaloTopoClusterFromTowerMaker::CellPathology::_addMsg(const std::string& msg,std::map<std::string,int>& map)
{ if ( map.find(msg) == map.end() ) { map[msg] = 0; } map[msg] += 1; }

inline const std::map<std::string,int>& CaloTopoClusterFromTowerMaker::CellPathology::invalidPointer() const
{ return _invalidPointer; }
inline const std::map<std::string,int>& CaloTopoClusterFromTowerMaker::CellPathology::invalidHash() const
{ return _invalidHash; }
inline const std::map<std::string,int>& CaloTopoClusterFromTowerMaker::CellPathology::invalidSampling() const
{ return _invalidSampling; }

/// @class CaloTopoClusterFromTowerMaker
/// @author Peter Loch <loch@physics.arizona.edu>
///
/// @brief A cluster builder tool forming topo-clusters representing calorimeter tower signals on a regular grid in @f$ (\eta,\phi) @f$ space.
///
/// This tool provides functionality allowing the creation of topo-cluster collections representing towers. The output data objects are 
/// @c xAOD::CaloCluster , collected into @c xAOD::CaloClusterContainer . Several configuration options are supported:
///
/// -# <b>inclusive cell towers</b>
///    All cells are collected into towers, independent of their signal. This is the default behaviour.
/// -# <b>exclusive cell towers</b> 
///    Cells with @f$ E > E_{\rm min} @f$ are collected into towers. This behaviour is turned on by poviding a property @c CellEnergyThreshold setting @f$ E_{\rm min} @f$.
/// -# <b>filtered mode</b>
///    Cells contributing to standard topo-clusters are collected into towers. This behaviour is triggered by providing the @c CaloTopoClusterContainerKey property with a 
///    valid key for a topo-cluster collection in the event store.
///
///  Configuration 2 and 3 are exclusive, with 3 overwriting 2. The output topo-clusters represent calorimeter towers on the EM scale. The can be handed to cluster moment
///  tools (needs EM scale) and, if desired, to a dedicated cluster calibration tool of type @c xAOD::CaloTowerClusterFromTowerCalibrator .  
///
///  To avoid mulitple retrievals of the same weights by searching for cells in (many) topo-clusters, the overall weight of the cell signal is stored in a random access
///  look-up table stored in a @c CaloCellClusterWeights object in the detector store. This object is created by this tool, if needed. If the tool property 
///  @c CellWeightLookupKey is set, this object will be generated, filled, and recorded. 
///      
#endif
