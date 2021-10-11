/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCOUTOFCLUSTER_H
#define CALOLOCALHADCALIB_GETLCOUTOFCLUSTER_H

/**
 * @class GetLCOutOfCluster
 * @version \$Id: GetLCOutOfCluster.h,v 1.2 2008-11-04 14:33:37 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 3-October-2008
 * @brief Top algorithm to generate OutOfCluster histograms for 
 * Local Hadron Calibration 
 *
 * This class is an Algorithm to generate the 2D histograms from
 * single pion simulations to correct for out-of-cluster energy
 * deposits. */


#include "AthenaBaseComps/AthAlgorithm.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/HistoDef.h"
#include <vector>
#include <string>

class TProfile2D;
class TFile;

class GetLCOutOfCluster : public AthAlgorithm
{

 public:

  GetLCOutOfCluster(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GetLCOutOfCluster();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  void mapinsert(const std::vector<Gaudi::Histo1DDef> & dims);

  void mapparse();


  /**
   * @brief definition of all dimensions used for out-of-cluster corrections
   * 
   * The currently defined dimensions are:
   *
   * side: negative values for eta < 0 and positive for eta > 0; 1 bin
   * from -1.5 to 1.5 for no side dependency
   *
   * |eta|: the absolute value of eta; normally in ATLAS the bounds
   * are within [0,5], smaller ones are useful for test beam setups.
   *
   * phi: the phi range; 1 bin from -pi to pi for no phi dependency
   *
   * log10(E_clus^EMnoPSnoGap3): the log10 of the cluster energy on EM
   * scale without pre-samplers and gap scintillators; typical range
   * should be within the generated energy range of single pions with
   * about 50% room to the maximum created energy - i.e. use log10(1
   * TeV) as max value if you generated up tp 2 TeV pions
   *
   * log10(lambda_clus): the log10 of the cluster depth in the
   * calorimeter; typical ranges are from 0 to 4
   *
   * weight: the ratio E_outOfCluster/E_clus^EMnoPSnoGap3/Isolation;
   * the bounds define the level of truncation for the weights
   * to suppress noise and bad sampling ratios. Typical bounds are 0.
   * to 5.
   */
  std::vector<Gaudi::Histo1DDef> m_dimensions;

  /**
   * @brief property to set all dimensions introduced above
   * 
   * Gaudi has only a map<string,Histo1DDef> to use as property. So
   * the syntax for all dimensions introduced above is
   * {'Title': ('Title',Low,High,Bins), ... }. */
  std::map<std::string,Gaudi::Histo1DDef> m_dimensionsmap;

  /**
   * @brief Vector of indices in m_dimensions 
   * 
   * Internal list of indices in m_dimensions object. */
  std::vector<int> m_isampmap;

  /**
   * @brief Vector of actual histograms.
   * 
   * Internal list of histograms to fill and save. */
  std::vector<TProfile2D *> m_ooc;

  /**
   * @brief Name of the output file to save histograms in.
   * 
   * Use this property to set the name of the output file containing the
   * OutOfCluster histograms. */
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
   * @brief vector of names of the calorimeter samplings not to use
   * when applying the out-of-cluster weights.
   *
   * The pre-sampler layers (barrel and endcap) and the
   * gap-scintillators should not receive out-of-cluster weights as
   * they mainly deal with dead-material corrections. */
  std::vector<std::string>  m_invalidSamplingNames;         

  /** 
   * @brief actual set of samplings to be ignored for out-of-cluster weights
   *
   * This set is created according to the names given in the property
   * m_invalidSamplingNames.  */
  std::set<int> m_invalidSamplings;                    

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

#endif // CALOLOCALHADCALIB_GETLCOUTOFCLUSTER_H
