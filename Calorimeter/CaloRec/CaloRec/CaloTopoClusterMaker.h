/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOTOPOCLUSTERMAKER_H
#define CALOTOPOCLUSTERMAKER_H
/**
 * @class CaloTopoClusterMaker
 * @version \$Id: CaloTopoClusterMaker.h,v 1.24 2009-05-15 12:09:48 artamono Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 23-December-2003
 * @brief topological cluster maker.
 *
 * Make clusters from CaloCells based on topological neighboring and
 * noise motivated thresholds in units of sigma applied to the
 * absolute value of the energy for each cell.  The clusters will have
 * at least one cell with \f$ E > N_{\rm seed}\,\sigma \f$ (or \f$|E|
 * > N_{\rm seed}\,\sigma \f$ if SeedCutsInAbsE is true).  Cells with
 * \f$ |E| > N_{\rm neighbor}\,\sigma \f$ will expand the cluster by
 * means of their neighbors.  All cells in the cluster must fulfill
 * \f$ |E| > N_{\rm cell}\,\sigma \f$.  The actual clustering is
 * performed by expanding around seed cells. The neighboring cells
 * above neighbor threshold (if not used so far) are stored in a list
 * and used in the next expansion step. The algorithm terminates if no
 * more unused neighbor cells are found.  The final clusters are
 * copied to the cluster collection if they pass the \f$E_\perp\f$ (or
 * \f$|E|_\perp\f$ if SeedCutsInAbsE is true) cut.
 *
 * Like all other cluster maker tools this class derives from
 * CaloClusterCollectionProcessor.  */

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloIdentifier/CaloCell_ID.h"
#include "Identifier/IdContext.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/IdentifierHash.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"

class ICalorimeterNoiseTool;
class Identifier; 
class CaloDetDescrManager; 
class CaloDetDescrElement;


class CaloTopoClusterMaker: public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:    
  
  CaloTopoClusterMaker(const std::string& type, const std::string& name,
		       const IInterface* parent);

  StatusCode execute(xAOD::CaloClusterContainer* theClusters);
  StatusCode initialize();

  /** Callback added to handle Data-driven GeoModel initialisation
   */
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

  void getClusterSize();


 private: 
  
 
  inline bool passCellTimeCut(const CaloCell*, float) const;

  const CaloDetDescrManager* m_calo_dd_man; 
  
  const CaloCell_ID* m_calo_id;
  
  /** 
   * @brief vector of names of the cell containers to use as input.
   */
  std::string  m_cellsName; 

  /** 
   * @brief vector of names of the calorimeters to consider.
   *
   * The default is to use all calorimeters (i.e. LAREM, LARHEC,
   * LARFCAL, TILE).  Cells which belong to one of the input cell
   * containers and to one of the calorimeters in this vector are used
   * as input for the cluster maker.  This property is used in order
   * to ignore a certain subsystem (e.g. for LAREM only clusters
   * specify only LAREM in the jobOptions).  */
  std::vector<std::string>  m_caloNames;         

  /** 
   * @brief Flag which subdetectors are to be used.
   *
   * This is initialized according to the names given in the property
   * m_caloNames.  */
  bool m_subcaloUsed[CaloCell_ID::NSUBCALO];

  /** 
   * @brief all cells have to satisfy \f$|E| > N_{\rm cell}\,\sigma\f$
   *
   * This cut determines how much the cluster will extend beyond the
   * last cell passing the neighbor threshold. The smaller this cut is
   * the more cells will be allowed in the tail of the cluster. It
   * should be smaller or equal to the neighbor threshold. If a cell
   * passing this cut is neighbor of two or more cells passing the
   * neighbor cut it will be inserted in the cluster which has the
   * neighbor cell that was asked first for its neighbors. Since the
   * original list of seed cells is ordered in descending order of
   * \f$E/\sigma\f$ (or \f$|E|/\sigma\f$) the distance of the cell (in
   * number of cell generations passing the neighbor cut until this
   * cell will be reached) usually determines in which cluster the
   * cell will end up in. The cell cut should be lower or equal to the
   * neighbor cut.  */
  float m_cellThresholdOnEorAbsEinSigma;            

  /** 
   * @brief cells with \f$|E| > N_{\rm neighbor}\,\sigma\f$ extend the cluster
   *
   * This cut determines how many cells are asked for their neighbors
   * to expand the cluster. The smaller this cut is the more cells
   * will be asked for their neighbors. If a cell passing this cut is
   * neighbor of two other cells passing this cut from different
   * clusters, the two clusters are merged. The neighbor cut should be
   * lower or equal to the seed cut.  */
  float m_neighborThresholdOnEorAbsEinSigma;  

  /** 
   * @brief cells with \f$|E| > N_{\rm seed}\,\sigma\f$ start a cluster
   *
   * This cut determines how many clusters are formed initially.  The
   * smaller this cut is the more clusters will be created.  During
   * the accumulation of cells inside the clusters it can happen that
   * clusters are merged if a cell passing the neighbor threshold
   * would be included in both clusters.  */
  float m_seedThresholdOnEorAbsEinSigma;         

  /** 
   * @brief optional cut on \f$E_\perp\f$ or \f$|E|_\perp\f$ on the
   * cell level
   *
   * This and the following 2 variables define on cell, neighbor and
   * seed level additional thresholds on \f$E_\perp\f$ or
   * \f$|E|_\perp\f$. They are treated like the noise cuts above on
   * each level and used only in case m_usePileUpNoise is set to
   * false. Usually m_usePileUpNoise should be set to true and using
   * these cuts is not recommended.  */
  float m_cellThresholdOnEtorAbsEt;                  

  /** @brief optional cut on \f$E_\perp\f$ or \f$|E|_\perp\f$ on the
      neighbor level */
  float m_neighborThresholdOnEtorAbsEt;              

  /** @brief optional cut on \f$E_\perp\f$ or \f$|E|_\perp\f$ on the
      seed level */
  float m_seedThresholdOnEtorAbsEt;              

  /**                                                                                                             
   * threshold used for timing cut on seed cells. Implemented as |seed_cell_time|<m_seedThresholdOnTAbs. No such cut on neighboring cells.*/
  float m_seedThresholdOnTAbs;



  /**
   * @brief switch to use the CaloNoiseTool 
   *
   * the CaloNoiseTool should be used whenever possible in order to
   * get the electronics noise and the pile-up noise for each cell
   * according to the current conditions. In case this switch is set
   * to false a constant noise value for all cells will be used.  */
  bool m_useNoiseTool;



  /**
   * @brief switch to use the pile-up noise CaloNoiseTool 
   *
   * if usePileUpNoise is set to true the relevant sigma for each cell
   * will be the quadratic sum of the electronics noise at current
   * cell gain and its pile-up noise at the current
   * luminosity. Otherwise it will be just the electronics noise. This
   * switch can be set to true only if m_useNoiseTool is set to true
   * as well.  It is recommended to set usePileUpNoise to true even
   * for situations (like single particle MC) where no pile-up noise
   * is expected since the CaloNoiseTool most likely will be
   * configured in a way that it reflects that situation. If this
   * switch is set to false the additional \f$E_\perp\f$ thresholds
   * declared above will be used.  */
  bool m_usePileUpNoise;
  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

  /**
   * @brief constant noise value in case the CaloNoiseTool is not used
   *
   * this value should only be used for testing purposes. It will be
   * used if the CaloNoiseTool is switched off as the electronics
   * noise for each cell.  */
  float m_noiseSigma;                            

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
   * @brief if set to true limit the neighbors in presampler Barrel and Endcap.
   *
   * The presampler cells add a lot of PileUp in the Hilum
   * samples. With this option set to true the presampler cells do not
   * expand the cluster in the presampler layer.  Only the next
   * sampling is used as valid neighbor source. */
  bool m_restrictPSNeighbors;
                                                 
  /**
   * @brief if set to true seed cuts are on \f$|E|\f$ and \f$|E|_\perp\f$.
   *
   * The seed cuts and the \f$E_\perp\f$ cut on the final clusters
   * before insertion to the CaloClusterContainer will be on absolute
   * energy and absolute transverse energy if this is set to true. If
   * set to false the cuts will be on energy and transverse energy
   * instead.  */
  bool m_seedCutsInAbsE;                         
                                                 
  /**
   * @brief if set to true neighbor cuts are on \f$|E|\f$ and \f$|E|_\perp\f$.
   *
   * The neighbor cuts will be on absolute energy and absolute
   * transverse energy if this is set to true. If set to false the
   * cuts will be on energy and transverse energy instead.  */
  bool m_neighborCutsInAbsE;                         
                                                 
  /**
   * @brief if set to true cell cuts are on \f$|E|\f$ and \f$|E|_\perp\f$.
   *
   * The cell cuts will be on absolute energy and absolute transverse
   * energy if this is set to true. If set to false the cuts will be
   * on energy and transverse energy instead.  */
  bool m_cellCutsInAbsE;                         
                                                 
  /**
   * @brief \f$E_\perp\f$ cut on the final cluster. 
   *
   * The final cluster has to pass this cut (which is on \f$E_\perp\f$
   * or \f$|E|_\perp\f$ of the cluster depending on the above switch)
   * in order to be inserted into the CaloClusterContainer.  */
  float m_clusterEtorAbsEtCut;                   

  /**                                                                                              
   * if set to true, time cut is applied to seed cells, no cut otherwise 
   */
  bool m_seedCutsInT;

  /**                                                                                              
   * if set to true, seed cells failing the time cut are also excluded from cluster at all 
   */
  bool m_cutOOTseed;

                                                 
  /**
   * @brief if set to true use 2-gaussian noise description for 
   * TileCal  */
  bool m_twogaussiannoise;
                                                 
  /**
   * @brief if set to true treat cells with a dead OTX which can be
   * predicted by L1 trigger info as good instead of bad cells */
  bool m_treatL1PredictedCellsAsGood;
                                                 
  /** 
   * @brief vector of names of the calorimeter samplings to consider
   * for seeds.
   *
   * The default is to use all calorimeter samplings. Excluding a
   * sampling from this vector prevents the definition of a seed cell
   * in this sampling. Cells in those samplings are still used and
   * incorporated in the topo clusters (both on the neighbor and the
   * cell level) they can therefore even expand a cluster but not seed
   * one ...*/
  std::vector<std::string>  m_samplingNames;         

  /** 
   * @brief actual set of samplings to be used for seeds
   *
   * This set is created according to the names given in the property
   * m_samplingNames.  */
  std::set<int> m_validSamplings;                    

  /** 
   * @brief smallest valid seed sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a
   * quick lookup if a cell belongs to a valid sampling for seeds or
   * not. */
  int m_minSampling;

  /** 
   * @brief largest valid seed sampling found 
   *
   * This is needed to adjust the range of the vector<bool> for a
   * quick lookup if a cell belongs to a valid sampling for seeds or
   * not. */
  int m_maxSampling;

  /** 
   * @brief flag for all samplings - true for used ones, false for excluded ones
   *
   * This vector serves as a quick lookup table to find out if a cell
   * belongs to a sampling that should be used for seeds. */
  std::vector<bool> m_useSampling;

  /** @brief produces intolerable amount of output if set to true */
  bool m_doALotOfPrintoutInFirstEvent;

  IdentifierHash m_hashMin;
  IdentifierHash m_hashMax;


  /// Cluster size enum. Set based on energy cut jobO
  xAOD::CaloCluster::ClusterSize m_clusterSize; 
};

#endif // CALOTOPOCLUSTERMAKER_HH
