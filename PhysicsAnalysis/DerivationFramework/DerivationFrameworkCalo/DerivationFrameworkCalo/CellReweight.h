// Dear emacs, this is -*- C++ -*-.

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DERIVATIONFRAMEWORK_CELLREWEIGHT_H
#define DERIVATIONFRAMEWORK_CELLREWEIGHT_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloCellContainerFinalizerTool.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/ToolHandle.h"
#include <TH1.h>
#include <TH2.h>

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

  /// return index in m_etaBins vector for a certain t_eta (index goes from 1 to number of bins (i.e. m_nEtaBins-1)
  unsigned int getEtaBin(float t_eta) const;

  /// return number of L1 eta cells per L2 eta cell (deltaEta=0.025)
  unsigned int getEtaSize(int n_eta) const;

  /// creates a map of a cluster with a hottest cell in the centre
  void getClusterMap(int seedEta, int seedPhi, IdentifierHash (*map)[11], const CaloCellContainer* cellCont) const;

  std::string m_oldCellContainerName;
  std::string m_newCellContainerName;
  std::string m_caloRegion;
  std::string m_cellRatioPath;    
  std::string m_cellRatio2DPath;
  std::string m_SGKey_electrons, m_SGKey_photons;
  const CaloCell_ID* m_caloID; //!< Pointer to CaloID helper

  std::map<std::string, TH1F *> m_hratio;
  std::map<std::string, TH2F *> m_hratio2D;
  static const unsigned int m_nEtaBins=16; // number of bin edges. Actual number of bins is 15. And files do not contain weights for bin 2.4-2.6 so they only contain 14 histograms..
  const double m_etaBins[m_nEtaBins] = {0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.3,1.37,1.52,1.6,1.8,2.0,2.2,2.4,2.6};
  std::string m_setaBins[m_nEtaBins] = {"000","020","040","060","080","100","120","130","137","152","160","180","200","220", "240", "260"};

  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool711;
  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool37;
  ToolHandle<CaloClusterCollectionProcessor> m_caloFillRectangularTool55;
  ToolHandle<ICaloConstCellMakerTool> m_caloCellContainerFinalizerTool;

  CaloFillRectangularCluster *m_tool711, *m_tool37, *m_tool55;
  CaloCellContainerFinalizerTool *m_tool2;

  // number of eta, phi bins used for Lr2 reweighting
  static const int m_neta_Lr2=7;
  static const int m_nphi_Lr2=11;
  }; 
}
#endif //DERIVATIONFRAMEWORK_CELLREWEIGHT_H


