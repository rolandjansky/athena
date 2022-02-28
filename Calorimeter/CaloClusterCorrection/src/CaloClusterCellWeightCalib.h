/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCLUSTERCELLWEIGHTCALIB_H
#define CALOUTILS_CALOCLUSTERCELLWEIGHTCALIB_H 

#include "GaudiKernel/ToolHandle.h"

#include "CaloInterface/ICellWeightTool.h"

#include "CaloRec/CaloClusterProcessor.h"

class CaloCluster;

class CaloClusterCellWeightCalib : public CaloClusterProcessor 
{  
public:

  /*! @brief Algorithm tool constructor */
  CaloClusterCellWeightCalib(const std::string& type,
			     const std::string& name,
			     const IInterface*  pParent);
  /*! @brief Base tool destructor */
  virtual ~CaloClusterCellWeightCalib() override;

  /*! @brief Tool initialization */
  virtual StatusCode initialize() override;

  /*! @brief Tool execution */
  using CaloClusterProcessor::execute;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloCluster* theCluster) const override;
  
protected:
  /*! @brief Property controlling negative signal handling
   *
   *  Negative cell signals are problematic for the direction calculation
   *  from cells:
   * 
   *  @param Signal    use only E>0 cells for direction
   *  @param AbsSignal (default) use w*|E| for direction 
   *  @param RawSignal do not recalculate direction at all
   *
   *  The property is not case sensitive. The default is AbsSignal, i.e.
   *  following a strategy very similar to the direction calculation 
   *  performed during cluster formation.
   */
  std::string m_directionCalculation;

  /*! @brief Property controlling calibration of noise clusters
   *
   *  Flags calibration of noise (E<0) clusters:
   *
   *  @param true (default) means noise clusters are treated as all others
   *  @param false means noise clusters are not calibrated at all
   */
  bool m_calibNoiseLikeAll;

  /*! @brief Property controlling calibration method for noise clusters
   *
   *  Tags calibration method for noise (E<0) clusters. Only effective if
   *  these clusters are calibrated at all, i.e. @c m_calibNoise is @c true :
   * 
   *   @param true (default) uses same direction correction as all other 
   *               clusters
   *   @param false uses @c AbsSignal calibration for noise clusters.
   */
  std::string m_noiseDirectionCalculation;

  /*! @brief Energy threshold for direction calculation */
  double m_eThreshold;

  /*! @brief Flag to ignore geometrical cell weights in clusters
   *
   *  This flags controls the use of geometrical cell weights in clusters:
   * 
   *  @param false (default) geometrical weights are used
   *  @param true  geometrical weights are ignored
   *
   *  Note that using the geometrical weights requires this tool to run
   *  before the Local Hadronic calibration tools.
   */
  bool m_ignoreGeoWghts;

  /*! @brief Handle for cell weight tool */
  ToolHandle<ICellWeightTool> m_cellWeight;

protected:

  /*! @brief Negative signal handling:  default tag */
  static const std::string m_defName;
  /*! @brief Negative signal handling: positive signal tag */
  static const std::string m_posName;
  /*! @brief Negative signal handling: absolute signal tag */
  static const std::string m_absName;
  /*! @brief Negative signal handling: raw signal tag */
  static const std::string m_rawName; 


protected:

//   /*! @brief Processor type for cluster calibration */
//   typedef StatusCode 
//   (CaloClusterCellWeightCalib::*PROCESSOR)(const xAOD::CaloCluster* pClus) const;

//   /*! @brief Processor implementation for all clusters */
//   StatusCode p_execAll(const xAOD::CaloCluster* pClus) const;
//   /*! @brief Processor implementation for dedicated noise cluster treatment */
//   StatusCode p_execSpc(const xAOD::CaloCluster* pClus) const;

//   /*! @brief Pointer to cluster processor */
//   PROCESSOR p_execute;

  /*! @brief Calculator type for direction calculation */
  typedef StatusCode
  (CaloClusterCellWeightCalib::*CALCULATOR)(xAOD::CaloCluster* pClus) const;

  /*! @brief Calculator implementation for energy only update 
   *
   *  Direction is taken from electromagnetic scale. This is safe for 
   *  all clusters.
   */
  StatusCode f_dirRaw(xAOD::CaloCluster* pClus) const;
  /*! @brief Calculator implementation for energy only update 
   *
   *  Direction is taken from electromagnetic scale. This is safe for 
   *  all clusters. The energy calculation ignores geometrical weights.
   */
  StatusCode f_dirRawNW(xAOD::CaloCluster* pClus) const;
  /*! @brief Calculator implementation for direction from positive signal 
   *
   *  Direction is calculated using E>0 cell signals only.  This may be
   *  highly fluctuating for noise clusters.
   */ 
  StatusCode f_dirPos(xAOD::CaloCluster* pClus) const;
  /*! @brief Calculator implementation for direction from positive signal 
   *
   *  Direction is calculated using E>0 cell signals only.  This may be
   *  highly fluctuating for noise clusters. Geometrical weights
   *  are ignored.
   */ 
  StatusCode f_dirPosNW(xAOD::CaloCluster* pClus) const;
  /*! @brief Calculator implementation for direction from absolute signal
   *
   *  The absolute signal is used to re-calculate the cluster direction.
   *  This is stable for all clusters but changes the direction with 
   *  respect to the fixed scenario somewhat.  
   */
  StatusCode f_dirAbs(xAOD::CaloCluster* pClus) const;
  /*! @brief Calculator implementation for direction from absolute signal
   *
   *  The absolute signal is used to re-calculate the cluster direction.
   *  This is stable for all clusters but changes the direction with 
   *  respect to the fixed scenario somewhat. Geometrical weights are
   *  ignored. 
   */
  StatusCode f_dirAbsNW(xAOD::CaloCluster* pClus) const;

  /*! @brief Pointer to direction calculation implementation */
  CALCULATOR m_calc;

  /*! @brief Pointer to direction calculation for noise clusters */
  CALCULATOR m_calc_noise;

protected:

  /*! @brief Setup for calculation for all or non-noise clusters */
  StatusCode setupAll(MsgStream& report);

  /*! @brief Setup for different calculation for noise clusters */
  StatusCode setupSpc(MsgStream& report);

  /*! @brief Common setup function */
  bool setup(const std::string& name,const std::string& tag,
	     CALCULATOR& calc,std::string& conf,MsgStream& report);

  /*! @brief Helper for non-case sensitive string comparison */
  static bool cmpNoCase(const std::string& a,const std::string& b);
};

/*! @class CaloClusterCellWeightCalib
 *
 *  Tool calculates alternative cluster calibration using an external 
 *  cell weight tool with interface @c ICellWeightTool . It only modifies
 *  the cluster 4-vector from alternative calibration, i.e. it switches
 *  the @c CaloCluster to signal state @c P4SignalState::ALTCALIBRATED
 *  and puts the result of the cell weight calibration into this state.
 *  Previously set alternative calibrations are overwritten, but the
 *  basic (@c P4SignalState::UNCALIBRATED ) and (local hadronic) 
 *  calibrated (@c P4SignalState::CALIBRATED ) are not changed. The cluster
 *  is returned to the signal state it has been received in. Even though 
 *  with this behaviour the tool could run at any time in the cluster
 *  algorithm, it should run right after the cluster formation, i.e. it
 *  should be the first calibration/correction tool, as it is using the
 *  geometrical weights, which are lost in local hadronic calibration. Note
 *  that the tool can be configured to ignore these weights, but this can
 *  lead to unexpected signal calibrations.  
 */
#endif
