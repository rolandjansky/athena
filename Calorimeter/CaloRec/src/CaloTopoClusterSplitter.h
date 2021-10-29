/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef CALOTOPOCLUSTERSPLITTER_H
#define CALOTOPOCLUSTERSPLITTER_H
/**
 * @class CaloTopoClusterSplitter
 * @version \$Id: CaloTopoClusterSplitter.h,v 1.12 2009-04-18 02:56:16 ssnyder Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 17-May-2004
 * @brief topological cluster splitter.
 *
 * Split clusters based on topological neighboring and energy defined
 * local maxima.  The cells in a cluster are searched for local maxima
 * by means of their energy content. The so found local maxima are
 * used as seeds for a topological clustering as in the
 * CaloTopoClusterMaker. The special case of zero thresholds for
 * neighbors and cells is used such that all cells in the parent
 * cluster will be re-clustered. One further difference to the normal
 * topological clustering is that proto-clusters containing a local
 * maximum can never be merged.  Therefore the number of clusters
 * resulting from the splitting of the parent is determined by the
 * number of local maxima found in the cluster.  If no local maximum
 * was found, the parent cluster will be left un-altered in the
 * cluster container.  If parts of the original cluster are not
 * connected with local maximum containing parts (different cluster
 * algorithm or different neighbor option) all cells of a cluster in
 * this category are included in a rest-cluster and added to the new
 * cluster list. Thus the total energy of all clusters and the number
 * of cells of all clusters should be the same before and after
 * splitting. Cells at the border between 2 split clusters can be
 * shared if the corresponding property is set. Otherwise they are
 * attached to the cluster which provides the neighbor with largest
 * energy content. */

class Identifier; 

#include "CaloIdentifier/CaloCell_ID.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/IdentifierHash.h"

#include "CaloRec/CaloClusterCollectionProcessor.h"

class CaloTopoClusterSplitter: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:    
  
  CaloTopoClusterSplitter(const std::string& type, const std::string& name,
			  const IInterface* parent);
  
  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer* theClusters) const override;
  virtual StatusCode initialize() override;

 private: 
  
  const CaloCell_ID* m_calo_id;

  /**
   * @brief type of neighbor relations to use.
   *
   * The CaloIdentifier package defines different types of neighbors
   * for the calorimeter cells. Currently supported neighbor relations
   * for topological clustering are:
   *
   * @li "all2D" for all cells in the same layer (sampling or module)
   *      of one calorimeter subsystem. Note that endcap and barrel
   *      will be unconnected in this case even for the LAREM.
   *
   * @li "all3D" for all cells in the same calorimeter. This means all
   *      the "all2D" neighbors for each cell plus the cells in
   *      adjacent samplings overlapping at least partially in
   *      \f$\eta\f$ and \f$\phi\f$ with the cell. Note that endcap
   *      and barrel will be connected in this case for the LAREM.
   *
   * @li "super3D" for all cells. This means all the "all3D" neighbors
   *      for each cell plus the cells in adjacent samplings from
   *      other subsystems overlapping at least partially in
   *      \f$\eta\f$ and \f$\phi\f$ with the cell. All calorimeters
   *      are connected in this case.
   *
   * The default setting is "super3D".  */
  std::string m_neighborOption;                  
  LArNeighbours::neighbourOption m_nOption;      

  /**
   * @brief if set to true limit the neighbors in HEC IW and FCal2&3.
   *
   * The cells in HEC IW and FCal2&3 get very large in terms of eta
   * and phi.  Since this might pose problems on certain jet
   * algorithms one might need to avoid expansion in eta and phi for
   * those cells. If this property is set to true the 2d neighbors of
   * these cells are not used - only the next sampling neighbors are
   * probed. */
  bool m_restrictHECIWandFCalNeighbors;
                                                 
  /**
   * @brief local maxima need at least this number of neighbors to
   * become seeds
   *
   * each cell above the energy cut having at least this many
   * neighbors in the parent cluster and only neighbors with smaller
   * energy seed a split cluster. */
  int m_nCells;                                 
                                                
  /**
   * @brief local maxima need at least this energy content
   *
   * potential seed cells have to pass this cut on the energy
   * content. */
  float m_minEnergy;                           

  /**
   * @brief share cells at the border between two local maxima
   * 
   * this property needs to be set to true in order to treat cells
   * which would be included in 2 clusters (for more then 2 the 2 with
   * the largest E for the current seed cells are used) as shared
   * cells. Shared cells are first excluded from the clustering and
   * then clustered after all normal cells are clustered. The shared
   * clusters are added to the 2 clusters they neighbor with the
   * weights \f$w_1 = E_1/(E_1+r E_2)\f$ and \f$w_2 = 1-w_1\f$, where
   * \f$E_{1,2}\f$ are the current energies of the 2 neighboring
   * clusters without the shared cells and \f$r=\exp(d_1-d_2)\f$ is
   * the ratio of the expected dependencies on the distances \f$d_i\f$
   * (in units of a typical em shower scale) of each shared cell to
   * the cluster centers. If the property is set to false the border
   * cells are included in the normal clustering and the cluster with
   * the largest E for the current seed cells gets the current border
   * cell. */
  bool m_shareBorderCells;                           

  /**
   * @brief typical em shower scale to use for distance criteria in
   * shared cells
   * 
   * a shared cell is included in both clusters neighboring the cell
   * with weights depending on the cluster energies and the distance
   * of the shared cell to the cluster centroids. The distance is
   * measured in units of this property to roughly describe the
   * exponential slope of the energy density distribution for em
   * showers. The exact choice of this property is not critical but
   * should roughly match the Moliere radius in the LArEM since here
   * the sharing of cells has the biggest use case. */
  float m_emShowerScale;

  /** 
   * @brief vector of names of the calorimeter samplings to consider.
   *
   * The default is to use all EM calorimeter samplings except for:
   * PreSamplerB, EMB1, PreSamplerE, EME1. The first FCal is also
   * considered an EM calorimeter sampling.  The exclusion of the
   * strips is mainly to be able to split converted photon clusters
   * while the exclusion of the hadronic calorimeters is mainly due to
   * the fact that the splitting here does not improve performance.
   * Excluding a sampling from this vector effectively sets the E for
   * each of its cells (for the purpose of local maxima search) to
   * 0. They are still counted in the neighbor criteria but don't make
   * local maxima any more ... */
  std::vector<std::string>  m_samplingNames;         

  /** 
   * @brief actual set of samplings to be used
   *
   * This set is created according to the names given in the property
   * m_samplingNames.  */
  std::set<int> m_validSamplings;                    

  /** 
   * @brief smallest valid sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a quick 
   * lookup if a cell belongs to a valid sampling or not. */
  int m_minSampling;

  /** 
   * @brief largest valid sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a quick 
   * lookup if a cell belongs to a valid sampling or not. */
  int m_maxSampling;

  /** 
   * @brief flag for all samplings - true for used ones, false for excluded ones
   *
   * This vector serves as a quick lookup table to find out if a cell belongs
   * to a sampling that should be used for local maxima. */
  std::vector<bool> m_useSampling;

  /** 
   * @brief vector of names of the secondary calorimeter samplings to
   * consider.
   *
   * Samplings in this list will be considered for local maxima only
   * if no local max in the primary list is overlapping. By default this
   * list is empty  */
  std::vector<std::string>  m_secondarySamplingNames;         

  /** 
   * @brief actual set of secondary samplings to be used
   *
   * This set is created according to the names given in the property
   * m_secondarySamplingNames.  */
  std::set<int> m_validSecondarySamplings;                    

  /** 
   * @brief smallest valid secondary sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a quick 
   * lookup if a cell belongs to a valid secondary sampling or not. */
  int m_minSecondarySampling;

  /** 
   * @brief largest valid secondary sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a quick 
   * lookup if a cell belongs to a valid secondary sampling or not. */
  int m_maxSecondarySampling;

  /** 
   * @brief flag for all secondary samplings - true for used ones,
   * false for excluded ones
   *
   * This vector serves as a quick lookup table to find out if a cell belongs
   * to a sampling that should be used for secondary local maxima. */
  std::vector<bool> m_useSecondarySampling;

  /**
   * @brief if set to true treat cells with a dead OTX which can be
   * predicted by L1 trigger info as good instead of bad cells */
  bool m_treatL1PredictedCellsAsGood;
  
  /**
   * @brief if set to true, splitter only looks at absolute 
   * value of Energy in order to identify potential seed cells */
  bool m_absOpt;


  IdentifierHash m_hashMin;
  IdentifierHash m_hashMax;
};

#endif // CALOTOPOCLUSTERSPLITTER_H
