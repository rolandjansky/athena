/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOLOCALHADCALIB_GETLCCLASSIFICATION_H
#define CALOLOCALHADCALIB_GETLCCLASSIFICATION_H

/**
 * @class GetLCClassification
 * @version \$Id: GetLCClassification.h,v 1.1.1.1 2008-11-04 08:56:11 menke Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 3-October-2008
 * @brief Top algorithm to generate classification histograms for 
 * Local Hadron Calibration 
 *
 * This class is an Algorithm to generate the 2D histograms from
 * single pion simulations to classify as hadronic or electromagnetic.
 * The output histograms need a combination step to calculate the
 * actual probabilty distributions. */

class StoreGateSvc;

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/HistoDef.h"
#include <vector>
#include <string>

class TH2F;
class TFile;

class GetLCClassification : public AthAlgorithm
{

 public:

  GetLCClassification(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GetLCClassification();
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  void mapinsert(const std::vector<Gaudi::Histo1DDef> & dims);

  void mapparse();

  /**
   * @brief definition of all dimensions used for classification
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
   * log10(E_clus): the log10 of the cluster energy; typical range
   * should be within the generated energy range of single pions with
   * about 50% room to the maximum created energy - i.e. use log10(1
   * TeV) as max value if you generated up tp 2 TeV pions
   *
   * log10(<rho_cell>) - log10(E_clus): the log10 of the cell energy
   * density moment normalized to the cluster energy; typical ranges
   * are from -9 to -4
   *
   * log10(lambda_clus): the log10 of the cluster depth in the
   * calorimeter; typical ranges are from 0 to 4
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
  std::vector<TH2F *> m_hclus;

  /**
   * @brief Name of the output file to save histograms in.
   * 
   * Use this property to set the name of the output file containing the
   * classification histograms. */
  std::string m_outputFileName;

  /**
   * @brief Output file to save histograms in.
   * 
   * Internal variable pointing to the output file. */
  TFile * m_outputFile;

  /**
   * @brief Name of the CaloClusterContainer to use. */

  std::string m_clusterCollName;

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

#endif // CALOLOCALHADCALIB_GETLCCLASSIFICATION_H
