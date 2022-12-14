/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
    
@class   LArTBClusterBuilder
@brief   Build (for TB) a cluster 3x3 around a seed cell in the middle.
    
@author  Mohamed AHARROUCHE
@date    Jul. 10, 2004
     
PACKAGE:  offline/LArCalorimeter/LArClusterRec
                
*/

#ifndef LARTBCLUSTERBUILDER_H
#define LARTBCLUSTERBUILDER_H

#include "LArRecEvent/LArCell.h"
#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloRec/CaloClusterCollectionProcessor.h"

class LArTBClusterBuilder : public AthAlgTool, virtual public CaloClusterCollectionProcessor
{
 public:

  // List of our cells
  typedef  std::list<const CaloCell*> CellList;

  // Constructor
  LArTBClusterBuilder(const std::string& type, const std::string& name, const IInterface* parent);
  
  // Destructor
  virtual ~LArTBClusterBuilder() override;
  
  using CaloClusterCollectionProcessor::execute;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx,
                             xAOD::CaloClusterContainer*) const override;
  
 private:
   // Eta size of the middle and strips layer  
  double m_eta_size,m_deta;

  // Phi size of the middle
  double m_phi_size,m_dphi;

  // Energy threshold
  double m_ethreshold;

  // Number of cluster
  int m_nbcluster;
  
  // Energy & barycentrs per layer 

  std::string m_cellContainerName;
  std::string m_caloRegion;


};


class ene_sup 
{

 public: 
  ene_sup (); 
  bool operator () (const CaloCell* cell1, const CaloCell* cell2);

};

inline ene_sup::ene_sup()
{ }

inline bool  
ene_sup::operator () ( const CaloCell* cell1,const CaloCell* cell2)
{  
  return cell1->et() > cell2->et() ; 
}

#endif
