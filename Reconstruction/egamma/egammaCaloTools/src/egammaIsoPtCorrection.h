/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMACALOTOOLS_EGAMMAISOPTCORRECTION_H
#define EGAMMACALOTOOLS_EGAMMAISOPTCORRECTION_H
/********************************************************************
NAME:     egammaIsoPtCorrection.h 
PACKAGE:  offline/Reconstruction/egamma/egammaCaloTools
CREATED:  Nov 2010
AUTHOR:   T. Cuhadar Donszelmann
********************************************************************/

// INCLUDE HEADER FILES:
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IegammaIsoPtCorrection.h"
#include "xAODEgamma/EgammaFwd.h"
#include <utility>
#include <vector>

class egammaIsoPtCorrection : public AthAlgTool, virtual public IegammaIsoPtCorrection {

 public:

  /** @brief Default constructor*/
  egammaIsoPtCorrection(const std::string& type,
	    const std::string& name,
	    const IInterface* parent);

  /** @brief Destructor*/
  ~egammaIsoPtCorrection();  

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();

 
  /** @brief Calculate the correction  for etcone */
  double CalcPtCorrection(const xAOD::Egamma* eg, double dR, std::string str="" );


 private:

  /** @brief correction type */
  std::string m_type;

  /** @brief range of eta bins for e-ID*/
  std::vector<double> m_EtaBins;

  /** @brief Correction parameters for electrons */
  std::vector<double>  m_ElectronPtCorrection15;
  std::vector<double>  m_ElectronPtCorrection20;
  std::vector<double>  m_ElectronPtCorrection25;
  std::vector<double>  m_ElectronPtCorrection30;
  std::vector<double>  m_ElectronPtCorrection35;
  std::vector<double>  m_ElectronPtCorrection40;
  std::vector<double>  m_ElectronPtCorrectiontp40;

  std::vector<double>  m_ElectronOffset15;
  std::vector<double>  m_ElectronOffset20;
  std::vector<double>  m_ElectronOffset25;
  std::vector<double>  m_ElectronOffset30;
  std::vector<double>  m_ElectronOffset35;
  std::vector<double>  m_ElectronOffset40;
  std::vector<double>  m_ElectronOffsettp40;


  /** @brief Correction parameters for converted photons */
  std::vector<double>  m_PhotonConvertedPtCorrection15;
  std::vector<double>  m_PhotonConvertedPtCorrection20;
  std::vector<double>  m_PhotonConvertedPtCorrection25;
  std::vector<double>  m_PhotonConvertedPtCorrection30;
  std::vector<double>  m_PhotonConvertedPtCorrection35;
  std::vector<double>  m_PhotonConvertedPtCorrection40;
  std::vector<double>  m_PhotonConvertedPtCorrectiontp40;

  std::vector<double>  m_PhotonConvertedOffset15;
  std::vector<double>  m_PhotonConvertedOffset20;
  std::vector<double>  m_PhotonConvertedOffset25;
  std::vector<double>  m_PhotonConvertedOffset30;
  std::vector<double>  m_PhotonConvertedOffset35;
  std::vector<double>  m_PhotonConvertedOffset40;
  std::vector<double>  m_PhotonConvertedOffsettp40;

  /** @brief Correction parameters for unconverted photons */
  std::vector<double>  m_PhotonUnconvertedPtCorrection15;
  std::vector<double>  m_PhotonUnconvertedPtCorrection20;
  std::vector<double>  m_PhotonUnconvertedPtCorrection25;
  std::vector<double>  m_PhotonUnconvertedPtCorrection30;
  std::vector<double>  m_PhotonUnconvertedPtCorrection35;
  std::vector<double>  m_PhotonUnconvertedPtCorrection40;
  std::vector<double>  m_PhotonUnconvertedPtCorrectiontp40;

  std::vector<double>  m_PhotonUnconvertedOffset15;
  std::vector<double>  m_PhotonUnconvertedOffset20;
  std::vector<double>  m_PhotonUnconvertedOffset25;
  std::vector<double>  m_PhotonUnconvertedOffset30;
  std::vector<double>  m_PhotonUnconvertedOffset35;
  std::vector<double>  m_PhotonUnconvertedOffset40;
  std::vector<double>  m_PhotonUnconvertedOffsettp40;

  /** @brief Get eta bin */
  int GetEtaBin(double eta_w) const ;
  /** @brief Get pt correction for photon candidates */
  void getPhotonConvertedCorrection(int dR, int indx, std::pair<double,double> &) const;
  /** @brief Get pt correction for photon candidates */
  void getPhotonUnconvertedCorrection(int dR, int indx, std::pair<double,double>&) const;
  /** @brief Get pt correction for electron candidates */
  void getElectronCorrection(int dR, int indxi, std::pair<double,double>&) const;


};
#endif
