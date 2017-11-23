// Dear emacs, this is -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_CELLREWEIGHT_H
#define DERIVATIONFRAMEWORK_CELLREWEIGHT_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloCellContainerFinalizerTool.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include <TFile.h>
#include <TH1.h>
#include <TLorentzVector.h>
#include <string>
#include <map>


class CaloFillRectangularCluster;
class CaloCellContainerFinalizerTool;
class ICaloConstCellMakerTool;
class CaloClusterCollectionProcessor;

namespace DerivationFramework {

class CellReweight: public AthAlgTool, public IAugmentationTool { 
 public: 
  CellReweight( const std::string& t, const std::string& n, const IInterface* p );
  typedef  std::list<const CaloCell*> CellList;
  virtual ~CellReweight(); 
  StatusCode  initialize();
  virtual StatusCode  addBranches() const;
  StatusCode  finalize();
  
 private: 
  ///
  void setIndexLr2(std::vector<int> * index,  std::vector<float> *_eta, std::vector<float> *_lr2e, float m_eta) const;
  ///
  void setIndexLr1(std::vector<int> * index, std::vector<int> * neta_index, std::vector<float> *_eta, std::vector<float> *_e, std::vector<float> *_etaLr2, float m_eta) const;
  /// return index i of cell (in vector _eta) with _eta[i] closest to m_eta
  unsigned int locateMid(std::vector<float> * _eta, float m_eta ) const;
  /// return index i of cell (in vector _e) with largest _e[i]
  unsigned int locateMidE(std::vector<float> * _e) const;
  /// return index in eta_bins vector for a certaint t_eta
  unsigned int locateEta(float t_eta) const;
  ///  
  unsigned int getEtaSize(int n_eta) const;

  std::string m_cellContainerName;
  std::string m_newCellName;
  std::string m_caloRegion;
  std::string m_cellRatioPath;
  std::string m_SGKey_electrons, m_SGKey_photons;
  int m_neta_Lr2, m_nphi_Lr2;
  const CaloCell_ID* m_caloID; //!< Pointer to CaloID helper
	  
  std::map<std::string, TH1F *> m_hratio;
  static const unsigned int n_eta_bins=16;
  const double eta_bins[n_eta_bins] = {0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.3,1.37,1.52,1.6,1.8,2.0,2.2,2.4,2.6};
  std::string seta_bins[n_eta_bins] = {"000","020","040","060","080","100","120","130","137","152","160","180","200","220", "240", "260"};

  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool711;
  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool37;
  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool55;
  CaloFillRectangularCluster *m_tool711, *m_tool37, *m_tool55;
  ToolHandle<ICaloConstCellMakerTool> m_caloCellContainerFinalizerTool;
  CaloCellContainerFinalizerTool *m_tool2;

  }; 
}
#endif //DERIVATIONFRAMEWORK_CELLREWEIGHT_H


