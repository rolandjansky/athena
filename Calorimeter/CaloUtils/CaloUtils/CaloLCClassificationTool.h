/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOLCCLASSIFICATIONTOOL_H
#define CALOUTILS_CALOLCCLASSIFICATIONTOOL_H
/**
 * @class CaloLCClassificationTool
 * @version \$Id: CaloLCClassificationTool.h,v 1.6 2009-01-27 09:09:14 gunal Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 27-September-2005
 * @brief classify clusters according to their probability to 
 *        stem from an em object
 *
 * concrete class implementing a IClusterClassificationTool to
 * classify clusters according to their probability to be caused by an
 * electromagnetic decaying particle or shower.  Clusters with high em
 * probability should not be weighted.  This tool reads its data from
 * pool containing TProfile2D based em-probability data. */

#include "CaloInterface/IClusterClassificationTool.h"
#include "CaloConditions/CaloLocalHadCoeff.h"
#include "CaloConditions/CaloLocalHadDefs.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthAlgTool.h"


class CaloLCClassificationTool : public AthAlgTool, virtual public IClusterClassificationTool
{
 public:

  virtual ~CaloLCClassificationTool();

  virtual
  CaloRecoStatus::StatusIndicator classify(xAOD::CaloCluster* thisCluster)
    const override;
  virtual StatusCode initialize() override;

  CaloLCClassificationTool(const std::string& type, 
			   const std::string& name,
			   const IInterface* parent);

 private:
  
  /**
   * @brief name of the key for em fraction data */
  SG::ReadCondHandleKey<CaloLocalHadCoeff> m_key;

  /**
   * @brief if set to true the spread of EM probabilities in each bin is
   * taken into account for classification otherwise only the mean is
   * used
   *
   * For some regions of phase space the spread in the EM probability
   * values can be quite large. If this property is set to true the EM
   * probability which is maximaly consistent with the current bin is
   * calculated (mean+spread) and in case the property is false just
   * the mean is used to estimate the EM probability. */

  bool m_useSpread;

  /**
   * @brief old data depends on energy density while new data depends on
   * energy density over energy. This bool is needed to switch.
   *
   * Classification histos have energy density or normalized energy density
   * for the x-axis. The old cond data is not normalized which causes problems
   * for neutral pions. The new cond data is and this switch selects
   * the proper normalization for the tool. */

  bool m_useNormalizedEnergyDensity;

  /**
   * @brief cut value on EM probability below which all clusters are
   * classified as hadronic
   *
   * Useful values are around 0.5. Clusters in Bins with an average
   * EM probability below this cut value are tagged as hadronic.
   * Clusters in Bins with an average EM probability above this value are
   * considered electromagnetic and clusters falling in bins with no
   * information are tagged as unknown. */

  double m_maxProbability;

  /**
   * @brief if true the phase space probability based on single pions
   * that the current cluster stems from a pi0 is saved on AOD as a
   * moment. If false the moment goes to ESD only.
   *
   * Classification histos report this quantity. The subsequent
   * calibration functions can retrieve this probability if stored as
   * moment and weight the respective calibration by this (or 1 minus
   * it) to avoid overcorrections. */

  bool m_storeClassificationProbabilityInAOD;
  
  /**
   * @brief interpolate correction coefficients */

  bool m_interpolate;

  /** 
   * @brief vector of names of dimensions in look-up tables to interpolate */
  std::vector<std::string>  m_interpolateDimensionNames;

  /** 
   * @brief actual set of dimension id's to interpolate */
  //std::vector<CaloLocalHadDefs::LocalHadDimensionId> m_interpolateDimensions;
  std::vector<int> m_interpolateDimensions;
  
  /** 
   * @brief is set to true, classify neg. Clusters as well */
  bool m_absOpt;
  
  
  
};

#endif

