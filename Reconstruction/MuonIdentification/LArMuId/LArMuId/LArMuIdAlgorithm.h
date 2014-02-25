/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : LArMuIdAlgorithm.h
// Author   : M Ridel
// Created  : Feb 2005
//
// DESCRIPTION
// 
//  
// Properties (JobOption Parameters):
//
//
// BUGS:
//  
// History:
//      
//   
//****************************************************************************
#ifndef  LARMUID_LARMUIDALGORITHM_H
#define  LARMUID_LARMUIDALGORITHM_H

// includes 
#include "GaudiKernel/Algorithm.h" 
#include "StoreGate/StoreGateSvc.h"
#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <string>

class StoreGateSvc;  
class CaloCellContainer;
class CaloClusterContainer; 

class LArMuIdAlgorithm : public Algorithm
{
  public:
  // constructor 
  LArMuIdAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);
  // destructor 
  ~LArMuIdAlgorithm();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  private:

  StoreGateSvc* m_store_gate;
  std::string m_ImpactInCaloCollectionName;
  std::string m_cellContainer;
  std::string m_clustersOutputName;
  const CaloCell_ID* m_calo_helper;
  bool m_tracks;
  float m_dR;
  float m_thr1;
  float m_thr2;
  float m_thr3;
  float m_Deta;
  float m_Dphi;

  int eta2neta(float eta);

  int phi2nphi(float phi);

  void do_clustering(float pix[256][256],int occupied[256][256],int ieta,int iphi,int icluster);

  void get_next_neighbour(int occupied[256][256],int &ieta1, int &iphi1, int ieta, int iphi, int &next_neighbour);

  int m_detSel ;    
};
#endif // LARMUID_TILELOOKFORMUALG_H
