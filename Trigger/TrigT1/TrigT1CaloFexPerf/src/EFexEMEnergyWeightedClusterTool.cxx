// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMEnergyWeightedClusterTool.cxx
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo (energy weighted clustering)
 */

#include "EFexEMEnergyWeightedClusterTool.h"

LVL1::EFexEMEnergyWeightedClusterTool::EFexEMEnergyWeightedClusterTool(const std::string& type, const std::string& name, const IInterface* parent)
   : AthAlgTool(type, name, parent)
{}


void
LVL1::EFexEMEnergyWeightedClusterTool::findCellsAbove(const CaloConstCellContainer* scells, const float& Thr, std::vector<const CaloCell*>& out) const
{
   out.clear();
   for(auto scell : *scells) {
      if ( scell->et() < Thr )  continue;
      out.push_back(scell);
   }
}


void
LVL1::EFexEMEnergyWeightedClusterTool::findCellsAbove_EMB2_EMEC2(const CaloConstCellContainer* scells, const float& Thr, std::vector<const CaloCell*>& out) const
{
   out.clear();
   for(auto scell : *scells) {
      if ( scell->et() < Thr )  continue;
      if ( scell->caloDDE()->getSampling()==2 || scell->caloDDE()->getSampling()==6 ) {
         out.push_back(scell);
      }
   }
}


void
LVL1::EFexEMEnergyWeightedClusterTool::findCellsAround(const CaloConstCellContainer* scells, const CaloCell* cell, std::vector<const CaloCell*>& out, 
                                                       const float detaSize, const float dphiSize) const
{
   out.clear();
   if ( !cell ) return;
   float etacell = cell->eta();
   float phicell = cell->phi();
   for(auto scell : *scells) {
      if ( std::abs( scell->eta() - etacell) > detaSize ) continue;
      float dphi = std::abs( scell->phi() - phicell);
      dphi = std::abs( M_PI - dphi );
      dphi = std::abs( M_PI - dphi );
      if ( std::abs( dphi ) > dphiSize ) continue;
      out.push_back(scell);
   }
}


void
LVL1::EFexEMEnergyWeightedClusterTool::findCellsAround(const CaloConstCellContainer* scells, const float etacell, const float phicell, 
                                                       std::vector<const CaloCell*>& out, const float detaSize, const float dphiSize) const
{
   out.clear();
   for(auto scell : *scells) {
      if ( std::abs( scell->eta() - etacell) > detaSize ) continue;
      float dphi = std::abs( scell->phi() - phicell);
      dphi = std::abs( M_PI - dphi );
      dphi = std::abs( M_PI - dphi );
      if ( std::abs( dphi ) > dphiSize ) continue;
      out.push_back(scell);
   }
}


void
LVL1::EFexEMEnergyWeightedClusterTool::findTTsAround(const xAOD::TriggerTowerContainer* scells, const float etacell, const float phicell, 
                                                     std::vector<const xAOD::TriggerTower*>& out) const
{
   out.clear();
   for(auto scell : *scells) {
      if ( std::abs( scell->eta() - etacell) > m_detaTT ) continue;
      float dphi = std::abs( scell->phi() - phicell);
      dphi = std::abs( M_PI - dphi );
      dphi = std::abs( M_PI - dphi );
      if ( std::abs( dphi ) > m_dphiTT ) continue;
      out.push_back(scell);
   }
}


bool
LVL1::EFexEMEnergyWeightedClusterTool::isCellEmMaximum(const std::vector<const CaloCell*>& scells, const CaloCell* cell) const
{
   if ( !cell ) return false;
   int samp = cell->caloDDE()->getSampling();
   if ( (samp >= 8) && (samp!=21) ) return false; // include FCAL0 EM
   float cellpt = 1.0001*cell->et(); //make sure you don't get thecell itself
   for(auto scell : scells){ 
      int samp1 = scell->caloDDE()->getSampling();
      if ( ( samp1 >= 8 ) && (samp1!=21) ) continue;
      if ( scell->ID() == cell->ID()  ) continue;
      if ( scell->et() > cellpt ) return false;
   }
   return true;
}


float
LVL1::EFexEMEnergyWeightedClusterTool::sumEmCells(const std::vector<const CaloCell*>& scells) const
{
   float totalSum = 0.0;
   for(auto scell : scells) {
      int samp1 = scell->caloDDE()->getSampling();
      if ( (samp1<8) || (samp1==21) ) totalSum+= scell->energy();
   }
   return totalSum;
}


float
LVL1::EFexEMEnergyWeightedClusterTool::sumEmCells2nd(const std::vector<const CaloCell*>& scells) const
{
   float totalSum = 0.0;
   for(auto scell : scells) {
      if ( (scell->caloDDE()->getSampling()==2) ||(scell->caloDDE()->getSampling()==6) ) {
         totalSum+= scell->energy();
      }
   }
   return totalSum;
}


float
LVL1::EFexEMEnergyWeightedClusterTool::sumHadCells(const std::vector<const CaloCell*>& scells) const
{
   float totalSum = 0.0;
   for(auto scell : scells){
      if ( (scell->caloDDE()->getSampling() <8) || ( scell->caloDDE()->getSampling()>=22) ) continue;
      //totalSum+= (scell->et())*TMath::CosH(scell->eta());
      totalSum+= (scell->energy());
   }
   return totalSum;
}


float
LVL1::EFexEMEnergyWeightedClusterTool::sumHadTTs(const std::vector<const xAOD::TriggerTower*>& scells) const
{
   float totalSum = 0.0;
   for(auto scell : scells){
      if ( std::abs( scell->eta() ) > 1.5 ) continue;
      if ( scell->sampling() == 0 ) continue;
      totalSum+= (scell->pt())*TMath::CosH(scell->eta());
   }
   return totalSum * 1e3; // express in MeV
}


void
LVL1::EFexEMEnergyWeightedClusterTool::findCluster(const std::vector<const CaloCell*>& scells, float &etaCluster, float &phiCluster) const
{
   etaCluster=0.0;
   phiCluster=0.0;
   double etaClusterD=0.0;
   double phiClusterD=0.0;
   double energyCluster=0.0;
   bool cross_phi_bound=false;
   int last_sign=0;
   for(auto scell : scells){
      if ( std::abs( scell->phi() ) < 2.7 ) continue;
      int layer = scell->caloDDE()->getSampling();
      if ( ( layer != 2 ) && ( layer != 6 ) ) continue;
      int cell_sign = ( scell->phi() >=0 ? 1 : -1 );
      if ( ( last_sign!=0 ) && ( last_sign != cell_sign ) ) cross_phi_bound = true;
      last_sign = cell_sign;
   }
   for(auto scell : scells){
      int layer = scell->caloDDE()->getSampling();
      if ( ( layer != 2 ) && ( layer != 6 ) ) continue;
      double scelleta = scell->eta();
      double scellphi = scell->phi();
      double scellet = scell->et();
      etaClusterD+= (scellet * scelleta);
      if (cross_phi_bound && scellphi < 0 ) scellphi += 2 * M_PI;
      phiClusterD+= (scellet * scellphi);
      energyCluster+= (scellet) ;
   }
   if ( energyCluster > 0.1 ) {
      etaClusterD/=energyCluster;
      phiClusterD/=energyCluster;
      etaCluster = (float)etaClusterD;
      phiCluster = (float)phiClusterD;
      if ( phiCluster > M_PI ) phiCluster-=2*M_PI;
   } else {
      etaCluster=-999.0;
      phiCluster=-999.0;
   }
}
