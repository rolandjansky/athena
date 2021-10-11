/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCWEIGHTS_H
#define CALOLOCALHADCALIB_GETLCWEIGHTS_H

/**
 * @class GetLCWeights
 * @version \$Id: GetLCWeights.h,v 1.2 2008-12-08 16:57:41 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 3-October-2008
 * @brief Top algorithm to generate Weight histograms for 
 * Local Hadron Calibration 
 *
 * This class is an Algorithm to generate the 2D histograms from
 * single pion simulations to cell-weight the hadronic classified
 * clusters. */

#include "GaudiKernel/HistoDef.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandleKeyArray.h"
#include <vector>
#include <string>

class TProfile2D;
class TFile;
class CaloCell_ID;

class ClusWeight {
 public: 
  unsigned int iClus;
  double weight;
  double eCalibTot;
  ClusWeight * next;
};

class GetLCWeights : public AthAlgorithm
{

 public:

  GetLCWeights(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GetLCWeights();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  void mapinsert(const std::vector<Gaudi::Histo1DDef> & dims);

  void mapparse();

  /**
   * @brief definition of all dimensions used for each sampling
   * 
   * Since samplings can be present or not, the samplings itself are
   * defined via the special first dimension which has to be exactly 1
   * bin with the title of the sampling and the bounds of the
   * CaloSample enum. 
   *
   * The remaining dimensions are:
   *
   * side: negative values for eta < 0 and positive for eta > 0; 1 bin
   * from -1.5 to 1.5 for no side dependency
   *
   * |eta|: the absolute value of eta; normally in ATLAS the bounds
   * are within [0,5], most samplings need smaller ranges and even
   * smaller ones for test beam setups.
   *
   * phi: the phi range; 1 bin from -pi to pi for no phi dependency
   *
   * log10(E_clus): the log10 of the cluster energy; typical range
   * should be within the generated energy range of single pions with
   * about 50% room to the maximum created energy - i.e. use log10(1
   * TeV) as max value if you generated up tp 2 TeV pions
   *
   * log10(rho_cell): the cell energy density (either truth or reco);
   * typical ranges are from -7 to 1
   *
   * weight: the ratio E_true/E_rec (or E_rec/E_true for inversion
   * method; the bounds define the level of truncation for the weights
   * to suppress noise and bad sampling ratios. Typical bounds are 0.5
   * to 3,5, or 10 without inversion method, where 5 and 10 are used
   * for HEC and Tile, respectively. With inversion mewthod the bounds
   * are -2 to 3.
   */
  std::vector<std::vector<Gaudi::Histo1DDef> > m_dimensions;

  /**
   * @brief property to set all dimensions introduced above
   * 
   * Gaudi has only a map<string,Histo1DDef> to use as property. So
   * the syntax for all dimensions introduced above is
   * {'Sampling:Title': ('Title',Low,High,Bins), ... }. */
  std::map<std::string,Gaudi::Histo1DDef> m_dimensionsmap;

  /**
   * @brief Vector of indices in m_dimensions for each sampling
   * 
   * Internal list of indices in m_dimensions object. */
  std::vector<std::vector<int> > m_isampmap;

  /**
   * @brief Vector of vector of actual histograms.
   * 
   * Internal lists of histograms for each possible sampling to fill
   * and save. */
  std::vector<std::vector<TProfile2D *> > m_weight;

  /**
   * @brief Name of the output file to save histograms in.
   * 
   * Use this property to set the name of the output file containing the
   * Weights histograms. */
  std::string m_outputFileName;

  /**
   * @brief Output file to save histograms in.
   * 
   * Internal variable pointing to the output file. */
  std::unique_ptr<TFile> m_outputFile;

  /**
   * @brief Name of the CaloClusterContainer to use. */

  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterCollName;

  /** 
   * @brief vector of calibration hit container names to use. 
   *
   * The containers specified in this property should hold calibration
   * hits inside the calorimeter systems. */
  SG::ReadHandleKeyArray<CaloCalibrationHitContainer> m_CalibrationHitContainerNames;

  /** 
   * @brief flag to switch on/off the use of the inversion method
   *
   * If true the inversion method is used - i.e. the weights are calculated
   * as E_rec/E_true vs. E_true and not as E_true/E_rec vs. E_rec.
   * If the inversion method is used the actual correction histogram has
   * to be created in a final step after all histograms have been added by
   * looping over the possible E_true values, calculating E_rec from the
   * histogram and then filling E_true/E_rec vs. E_rec in the new weighting
   * histogram. */
  bool m_useInversionMethod;

  /** 
   * @brief string to choose different normalization types
   *
   * Available options are "Const": every cluster counts with weight
   * 1; "Lin": Each cluster is weighted with E_calib_clus/E_calib_tot;
   * "Log": Each cluster is weighted with
   * log10(E_calib_clus/E_calib_tot)+2; "NClus": Each cluster is weighted with
   * 1/TotalNumberOfClustersWithE_calib>0.
   */

  std::string m_NormalizationType;

  int m_NormalizationTypeNumber;

  /** 
   * @brief string to choose different classification types
   *
   * Available options are "None": the input MC is expected to be just a single
   * pion MC with one sort of particles;
   * "ParticleID_EM" Gena's ParticleID classification (EM type objects) for the use of QCD MC;
   * "ParticleID_HAD" Gena's ParticleID classification (HAD type objects) for the use of QCD MC;
   */

  std::string m_ClassificationType;
  int m_ClassificationTypeNumber;

};

#endif // CALOLOCALHADCALIB_GETLCWEIGHTS_H
