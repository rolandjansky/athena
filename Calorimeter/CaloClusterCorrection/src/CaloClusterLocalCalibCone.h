/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOCLUSTERCORRECTION_CALOCLUSTERLOCALCALIBCONE_H
#define CALOCLUSTERCORRECTION_CALOCLUSTERLOCALCALIBCONE_H
/**
 * @class CaloClusterLocalCalibCone
 * @version \$Id: CaloClusterLocalCalibCone.h,v 1.4 2009-04-22 17:26:21 ssnyder Exp $
 * @author Sven Menke <menke@mppmu.mpg.de>
 * @date 23-November-2006
 * @brief cone method for local hadron calibration
 *
 * concrete class implementing a CaloClusterCollectionProcessor to
 * form cones from all clusters in a cluster collection and weight the
 * cells in the clusters according to cone and cell
 * properties. Basically the energy of the cone and the eta and energy
 * density of the cell are used to look up the weights. This tool
 * assumes that classification of the clusters is already done. This
 * tool reads its data from a root file containing TProfile2D
 * histograms. */

#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

class TProfile2D;
class TFile;
class CaloCell_ID;
class CaloDetDescrManager;
class ICalorimeterNoiseTool;

class CaloClusterLocalCalibCone : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{

 public:
  
  enum ConeSchema {Angle,DeltaR};

  // Constructor
  CaloClusterLocalCalibCone(const std::string& type,
                           const std::string& name,
                           const IInterface* parent);

  virtual ~CaloClusterLocalCalibCone() override;

  virtual StatusCode initialize() override;

  using CaloClusterCollectionProcessor::execute; // xxx temporary
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer*  clusColl) const override;

 private:

  CaloClusterLocalCalibCone();

  StatusCode initDataFromFile(const std::string& hadWeightFileName);

  /**
   * @brief name of the schema to use for making the cones ("Angle" or "DeltaR")
   *
   * The schema to use for cone making. If the schema is Angle the
   * distance criterion is the angular distance in radians of the cone
   * to the cluster in case of DeltaR it is the distance in eta-phi
   * space. */
  std::string m_coneSchemaName;

  /**
   * @brief actual schema enum
   *
   * The enum corresponding to the used schema. */
  ConeSchema m_coneSchema;

  /**
   * @brief max distance to cone axis allowed for a cluster to be included 
   *
   * Cones are made starting with the leading cluster in E_T. Clusters
   * with distances from the cone below this value are adding their
   * energy to the cone but the cone direction remains that of the
   * leading cluster. */
  double m_coneDistance;

  /**
   * @brief required reco status for the clusters in order to be calibrated
   *
   * Only clusters matching this reco status will be used in the calibration
   * procedure. */
  int m_recoStatus;

  /**
   * @brief name of the file containing the hadronic cell weights
   *
   * The H1-style hadronic cell weights are stored as function of
   * cone energy and the cell energy density for each eta region
   * (cell eta) and calorimeter sampling of the cell.  The weights
   * are stored as histograms in this file. */
  std::string m_hadWeightFileName;

  /**
   * @brief vector with n+1 elements defining the n eta bins 
   *
   * The i-th eta bin covers the region m_etaBins[i] <= |eta| <
   * m_etaBins[i+1].  The definition given here has to match the one
   * used in the data file containing the histograms. */
  std::vector<double>  m_etaBins;

  /**
   * @brief vector with nSample (=24) elements defining the calo index 
   *
   * The i-th number corresponds to the i-th sampling as defined in
   * CaloCell_ID::CaloSample. An entry of -1 means no data is defined
   * for this sampling. Different samplings could point to the same
   * index.  The definition given here has to match the one used in
   * the data file containing the histograms. */
  std::vector<int>  m_caloIndices;

  /**
   * @brief vector with nSample (=24) elements defining the sampling index 
   *
   * The i-th number corresponds to the i-th sampling as defined in
   * CaloCell_ID::CaloSample. An entry of -1 means no data is defined
   * for this sampling. Different samplings could point to the same
   * index.  The definition given here has to match the one used in
   * the data file containing the histograms. */
  std::vector<int>  m_samplingIndices;

  /**
   * @brief minimal signal/elec_noise ratio for a cell to be weighted
   *
   * Only cells with |energy| above this value times the RMS of the electronics
   * noise are considered in weighting. */
  double  m_signalOverNoiseCut;


  /**
   * @brief update also sampling variables */
  bool m_updateSamplingVars;

  /**
   * @brief pointer to the TFile containing the TProfile2D histograms
   *
   * The file is opened in the initialization phase. */

  TFile *m_hadWeightFile;

  /**
   * @brief vector of vector of pointers to TProfile2D histograms
   *
   * This object contains the actual data used for calibration. The
   * number of samplings and eta bins define the size of the
   * vectors. */

  std::vector<std::vector<TProfile2D *> > m_data;

  ICalorimeterNoiseTool * m_noiseTool;
};

#endif
