// -*- c++ -*-
#ifndef CALOREC_CALOTOWERGEOMETRYSVC_H
#define CALOREC_CALOTOWERGEOMETRYSVC_H

#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/StoreGateSvc.h"

#include "AthenaBaseComps/AthService.h"

#include "Identifier/IdentifierHash.h"

#include "CaloGeoHelper/CaloPhiRange.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "CaloEvent/CaloCell.h"

#include <string>
#include <vector>
#include <cmath>
#include <array>

#include <boost/tuple/tuple.hpp>

/// @class CaloTowerGeometrySvc
///
/// @brief Tower geometry store and provider
///
/// This service sets up a lookup table storing the geometrical area overlap fraction of a calorimeter cell
/// with towers in a given grid. This lookup table is set up at instantiation of the service. It can only be 
/// defined at that time. The default setup is a @f$ \Delta\eta\times\Delta\phi = 0.1 \times \pi/32 @f$ grid.
/// Any regular grid can be constructed. The grid defintion can be provided as property.
///
/// The cell-to-tower information is stored internally as a (random access) lookup table. For a given cell,
/// the hash index is used to retrieve a list of towers this cell overlaps with, and the overlap paramater
/// (area fraction used as a geometrical weight). This indices and geometrical weights are represented by
/// a list of pairs of @c int and @c double numbers. Each cell can potential overlap with more than one 
/// tower. A more detailed description of towers and the geometrical overlap is available on the
/// <a href="https://twiki.cern.ch/twiki/bin/view/AtlasSandboxProtected/CaloTowerPerformance">calorimeter tower project page</a>.  
///
/// The lookup table is implemented for random access and using the cell hash identifier to retrieve the 
/// requested list of twoer indices and weights. Several retrieval mechanisms are supported (see documentation
/// of the corresponding methods). 
///
/// To map the azimuth of a cell to a tower, @f$ -\pi < \phi < \pi @f$ is used (ATLAS standard). For
/// consistency, all @f$ \phi @f$ values are mapped into this range. 
///
/// The service inherits from @c AthService and thus from the @c Service base class in Gaudi. The managed tower grid
/// is defined by service properties, with the following naming convention:  
/// - pseudorapidity range
///     - number of bins <tt>TowerEtaBins</tt> (default 100)
///     - lower boundary of pseudorapidity range <tt>TowerEtaMin</tt> (default -5.0)
///     - upper boundary of pseudorapidity range <tt>TowerEtaMax</tt> (default  5.0)
/// - azimuth range
///     - number of bins <tt>TowerPhiBins</tt> (default 64)
///     - lower boundary of azimuthal range <tt>TowerPhiMin</tt> (default -&pi;)
///     - upper boundary of azimuthal range <tt>TowerPhiMax</tt> (default &pi;) 
/// 
///  
/// @todo Allow regional grids (varying segmentation as function of @f$ \eta @f$ .
///
class CaloTowerGeometrySvc : public AthService
{
public:

  ///@{
  typedef IdentifierHash::value_t               index_t;          ///< Type for scalar (pseudorapidity,azimuth) index
  typedef boost::tuples::tuple<index_t,index_t> towerindex_t;     ///< Type for composite (tower) index 
  typedef boost::tuples::tuple<index_t,double>  element_t;        ///< Type storing tower index and geometrical weight
  typedef std::vector<element_t>                elementvector_t;  ///< Type for list of elements holding tower index and list of weights
  typedef std::vector<elementvector_t>          elementmap_t;     ///< Type for lists of lists of elements (lookup table type) 
  ///@}

  ///@brief Standard constructor
  ///
  /// Constructs towergrid as defined by properties.
  ///
  ///@param name name of service (implementation of @c Gaudi::Service )
  CaloTowerGeometrySvc(const std::string& name,ISvcLocator* pSvc);
  ///@brief Base class destructor
  virtual ~CaloTowerGeometrySvc() { };

  ///@name Implementation of (Ath)Service interfaces
  ///@{
  virtual StatusCode initialize() override; ///< Initialize service
  virtual StatusCode finalize()   override; ///< Finalize service
  ///@}

  ///@name Retrieve towers for cells
  ///@{
  ///@brief Retrieve the list of towers associated with a calorimeter cell referenced by a pointer
  ///
  /// Retrieves the list of tower indices and geometrical weights for a given cell. The values are 
  /// returned in two index-parallel vectors. Previous content in these two vectors is removed if
  /// this method finds towers for the cell. 
  /// 
  /// @return Returns @c StatusCode::SUCCESS if list of towers found, else @s StatusCode::FAILURE.
  ///
  ///@param pCell      pointer to non-modifiable @c CaloCell object.
  ///@param towerIdx   reference to modifiable vector of indices (payload type @c size_t ). 
  ///@para, towerWghts reference to modifiable vector of weights (payload type @c double ).
  StatusCode access(const CaloCell* pCell,std::vector<size_t>& towerIdx,std::vector<double>& towerWghts) const;
  ///@brief Retrieve the list of towers associated with a calorimeter cell referenced its hash identifier
  ///
  /// Retrieves the list of tower indices and geometrical weights for a given cell. The values are 
  /// returned in two index-parallel vectors. Previous content in these two vectors is removed if
  /// this method finds towers for the cell. 
  /// 
  /// @return Returns @c StatusCode::SUCCESS if list of towers found, else @s StatusCode::FAILURE.
  ///
  ///@param cellHash   hash identifier referencing a calorimeter cell.
  ///@param towerIdx   reference to modifiable vector of indices (payload type @c size_t ). 
  ///@para, towerWghts reference to modifiable vector of weights (payload type @c double ).
  StatusCode access(IdentifierHash cellHash,std::vector<size_t>& towerIdx,std::vector<double>& towerWghts) const;
  ///@brief Retrieve the list of towers associated with a calorimeter cell referenced by a pointer
  ///
  /// Retrieves the list of tower indices and geometrical weights for a given cell. The values are 
  /// returned in a vector of (index,weight) pairs.
  /// 
  /// @return Returns a vector of (index,weight) pairs as a @c elementvector_t container. The container is empty
  /// if the cell does not have any overlap with a tower.
  ///
  ///@param pCell      pointer to non-modifiable @c CaloCell object.
  elementvector_t getTowers(const CaloCell* pCell)   const;
  ///@brief Retrieve the list of towers associated with a calorimeter cell referenced by its hash identifier
  ///
  /// Retrieves the list of tower indices and geometrical weights for a given cell. The values are 
  /// returned in a vector of (index,weight) pairs.
  /// 
  /// @return Returns a vector of (index,weight) pairs as a @c elementvector_t container. The container is empty
  /// if the cell does not have any overlap with a tower.
  ///
  ///@param cellHash   hash identifier referencing a calorimeter cell.
  elementvector_t getTowers(IdentifierHash caloHash) const; 
  ///@}

  ///@name Tower index calculators, translaters, manipulators and converters
  ///@{
  index_t etaIndex(const CaloCell* pCell)           const; ///< Get tower @f$ \eta @f$ bin index for a calorimeter cell referenced by a pointer
  index_t etaIndex(IdentifierHash cellHash)         const; ///< Get tower @f$ \eta @f$ bin index for a calorimeter cell referenced by its hash identifier
  index_t etaIndex(double eta)                      const; ///< Get tower @f$ \eta @f$ bin index for a given value of @f$ \eta @f$
  index_t phiIndex(const CaloCell* pCell)           const; ///< Get tower @f$ \phi @f$ bin index for a calorimeter cell referenced by a pointer
  index_t phiIndex(IdentifierHash cellHash)         const; ///< Get tower @f$ \phi @f$ bin index for a calorimeter cell referenced by its hash identifier
  index_t phiIndex(double phi)                      const; ///< Get tower @f$ \phi @f$ bin index for a given value of @f$ \phi @f$
  index_t towerIndex(const CaloCell* pCell)         const; ///< Get global tower index for a calorimeter cell referenced by a pointer
  index_t towerIndex(IdentifierHash cellHash)       const; ///< Get global tower index for a calorimeter cell referenced by its hash identifier
  index_t towerIndex(double eta,double phi)         const; ///< Get global tower index for a pair of @f$ (\eta,\phi) @f$ values
  index_t towerIndex(index_t etaIdx,index_t phiIdx) const; ///< Get global tower index for a pair of @f$ (\eta,\phi) @f$ indices
  index_t towerIndex(const element_t& elm)          const; ///< Get global tower index from payload data
  index_t invalidIndex()                            const; ///< Returns value of invalid index
  bool    isInvalidIndex(index_t idx)               const; ///< Returns @c true if argument is invalid index 
  ///@}

  ///@name Variable generators using tower indices
  ///@{
  double towerEtaLocal(index_t etaIndex)    const; ///< Return pseudorapdity from local index (bin center) 
  double towerPhiLocal(index_t phiIndex)    const; ///< Return azimuth from local index (bin center)
  double towerEtaGlobal(index_t towerIndex) const; ///< Return pseudorapidity from global tower index (bin center)
  double towerPhiGlobal(index_t towerIndex) const; ///< Return azimuth from global tower index (bin center)
  ///@}

  ///@name Helper functions
  ///@{
  double  cellWeight(const element_t& elm)                     const; ///< Retrieve cell signal weight from lookup table entry
  double  cellWeight(IdentifierHash cellHash,index_t towerIdx) const; ///< Retrieve cell signal weight from cell identifier and tower index
  double  cellWeight(const CaloCell* pCell, index_t towerIdx)  const; ///< Retrieve cell signal weight from pointer to cell object and tower index
  ///@}

private:

  ///@name Helpers
  ///@{
  StatusCode f_setupTowerGrid();                                         ///< Internally used function setting up the lookup store
  StatusCode f_setupTowerGridFCal(const CaloDetDescrElement* pCaloDDE);  ///< Internally used function setting up the lookup store for the FCal
  StatusCode f_setupTowerGridProj(const CaloDetDescrElement* pCaloDDE);  ///< Internally used function setting up the lookup store for calorimeters with projective readout
  ///@}

  ///@name Access to detector store and other services and stores
  ///@{
  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t; ///< StoreGate handle type
  StoreGateSvc_t m_detectorStore;                     ///< Pointer to detector store
  StoreGateSvc_t& detStore() const;                   ///< Accessor to detector store
  const CaloDetDescrManager* m_caloDDM;               ///< Pointer to calorimeter detector description 
  ///@}

protected:

  ///@name Internal stores
  ///@{
  elementmap_t m_towerLookup;   ///< Cell-to-tower mapping lookup store
  double       m_towerEtaWidth; ///< Width of tower bin in pseudorapidity
  double       m_towerPhiWidth; ///< Width of tower bin in azimuth 
  ///@}
  
  ///@name Properties
  ///@{
  ///@brief Internally stored tower grid descriptors
  size_t m_towerEtaBins; ///< Number of @f$ \eta @f$ bins 
  double m_towerEtaMin;  ///< Lower boundary @f$ \eta_{\rm min} @f$
  double m_towerEtaMax;  ///< Upper boundary @f$ \eta_{\rm max} @f$
  size_t m_towerPhiBins; ///< Number of @f$ \phi @f$ bins 
  double m_towerPhiMin;  ///< Lower boundary @f$ \phi_{\rm min} @f$
  double m_towerPhiMax;  ///< Upper boundary @f$ \phi_{\rm max} @f$
  double m_fcal1Xslice;  ///< Number of x slices for cells in FCal1
  double m_fcal1Yslice;  ///< Number of y slices for cells in FCal1
  double m_fcal2Xslice;  ///< Number of x slices for cells in FCal2
  double m_fcal2Yslice;  ///< Number of y slices for cells in FCal2
  double m_fcal3Xslice;  ///< Number of x slices for cells in FCal3
  double m_fcal3Yslice;  ///< Number of y slices for cells in FCal3
  ///@} 

  ///@name Process flags, helpers and numerical constants
  ///@{
  static index_t m_invalidIndex;                                       ///< Invalid index indicator
  const CaloDetDescrManager* f_caloDDM()                        const; ///< Pointer to calorimeter detector description manager
  const CaloDetDescrElement* f_caloDDE(const CaloCell* pCell)   const; ///< Retrieve calorimeter detector description element for a cell object
  const CaloDetDescrElement* f_caloDDE(IdentifierHash cellHash) const; ///< Retrieve calorimeter detector description element for a given cell hash identifier
  double f_cellEta(IdentifierHash cellHash)                     const; ///< Retrieve calorimeter cell pseudorapidity for a given cell hash identifier
  double f_cellPhi(IdentifierHash cellHash)                     const; ///< Retrieve calorimeter cell azimuth for a given cell hash identifier
  ///@}

  ///@name Stores
  ///@{
  std::array<double,3> m_ndxFCal; ///< Stores number of fragments along x for each FCal module
  std::array<double,3> m_ndyFCal; ///< Stores number of fragments along y for each FCal module
  std::array<double,3> m_wgtFCal; ///< Stores geometrical weights
  ///@}
};

inline CaloTowerGeometrySvc::StoreGateSvc_t& CaloTowerGeometrySvc::detStore() const { return m_detectorStore; }

inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::invalidIndex(index_t idx) const { return m_invalidIndex; } 

inline const CaloDetDescrManager* f_caloDDM()                        const { return m_caloDDM; } 
inline const CaloDetDescrElement* f_caloDDE(const CaloCell* pCell)   const { return pCell->caloDDE(); }
inline const CaloDetDescrElement* f_caloDDE(IdentifierHash cellHash) const { return f_caloDDM()->get_element(cellHash); }

inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(const CaloCell* pCell)   const { return etaIndex(pCell->eta()); }
inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::etaIndex(IdentifierHash cellHash) const { return etaIndex(f_caloDDE(cellHash)->eta()); }

inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::phiIndex(const CaloCell* pCell)   const { return phiIndex(pCell->phi()); }
inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::phiIndex(IdentifierHash cellHash) const { return phiIndex(f_caloDDE(cellHash)->phi()); }

inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::towerIndex(index_t etaIdx,index_t phiIdx) const { 
  return etaIdx < m_towerEtaBins && phiIdx < m_towerPhiBins ? etaIdx+phiIdx*m_towerPhiBins : invalidIndex(); 
} 
inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::towerIndex(double eta,double phi)         const { return towerIndex(etaIndex(eta),phiIndex(phi)); }
inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::towerIndex(const CaloCell* pCell)         const { return towerIndex(pCell->eta(),pCell->phi()); }
inline CaloTowerGeometrySvc::index_t CaloTowerGeometrySvc::towerIndex(const element_t& elm)          const { return elm.get<0>(); }

inline double CaloTowerGeometrySvc::f_cellEta(IdentifierHash cellHash) const { return f_caloDDE(cellHash)->eta(); }
inline double CaloTowerGeometrySvc::f_cellPhi(IdentifierHash cellHash) const { return CaloPhiRange::fix(f_caloDDE(cellHash)->phi()); }

inline double CaloTowerGeometrySvc::cellWeight(const element_t& elm)  const { return elm.get<1>(); }
#endif
