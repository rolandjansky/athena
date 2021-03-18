// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/**
 * Name    : EFexEMEnergyWeightedClusterTool.h
 * PACKAGE : Trigger/TrigT1/TrigT1CaloFexPerf
 * AUTHOR  : Denis Oliveira Damazio
 * PURPOSE : emulate the eFex EM algorithm for phase 1 L1Calo (energy weighted clustering)
 */

#ifndef LVL1__EFEXEMENERGYWEIGHTEDCLUSTERTOOL
#define LVL1__EFEXEMENERGYWEIGHTEDCLUSTERTOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

#include <string>
#include <vector>


namespace LVL1
{
   class EFexEMEnergyWeightedClusterTool : public AthAlgTool
   {
   public:
      EFexEMEnergyWeightedClusterTool(const std::string& type, const std::string& name, const IInterface* parent);

      // find the cells at EMB2 and EMEC2 above a given threshold in an event to be used as seeds to build electron/tau candidates
      void findCellsAbove_EMB2_EMEC2(const CaloConstCellContainer*, const float& thr, std::vector<const CaloCell*>& out) const;

      // finds cells around a seed cell. These cells will be part of the cluster
      void findCellsAround(const CaloConstCellContainer*, const CaloCell* cell, std::vector<const CaloCell*>& out, 
                           const float deta, const float dphi) const;

      void findCellsAround(const CaloConstCellContainer*, const float eta, const float phi, std::vector<const CaloCell*>& out,
                           const float deta, const float dphi) const ;


      /** finds TTs around a seed cell. These TTs will be part of the cluster. This helps to cover the part related to TileCall */
      void findTTsAround(const xAOD::TriggerTowerContainer*, const float eta, const float phi, std::vector<const xAOD::TriggerTower*>& out) const ;

      /** checks if a give (seed) cell is the highest in a vector of cells. This is to make sure we have a real local EM maximum */
      bool isCellEmMaximum(const std::vector<const CaloCell*>& scells, const CaloCell* cell) const ;
      /** sum all cells from the vector that are in the EM calorimeter part */
      float sumEmCells(const std::vector<const CaloCell*>& scells) const ;
      /** sum all cells from the vector that are in the EM calorimeter part (only 2nd layer) */
      float sumEmCells2nd(const std::vector<const CaloCell*>& scells) const ;
      /** sum all cells from the vector that are in the HAD calorimeter part */
      float sumHadCells(const std::vector<const CaloCell*>& scells) const ;
      /** sum all TTs from the vector that are in the HAD calorimeter part, but only for |eta|<1.72 (tile region) */
      float sumHadTTs(const std::vector<const xAOD::TriggerTower*>& scells) const ;

      /** detect central cluster position */
      void findCluster(const std::vector<const CaloCell*>& scells, float &etaCluster, float &phiCluster) const;



   private:

      // find the cells above a given threshold in an event to be used as seeds to build electron candidates
      void findCellsAbove(const CaloConstCellContainer*, const float& thr, std::vector<const CaloCell*>& out) const;


   /**
    * member variables
    */
   const float m_detaTT{0.125};  ///< deta for the cluster to TT definition
   const float m_dphiTT{0.15};  ///< dphi for the cluster to TT definition



   // unused code

   /** finds cells around a seed cell whose layer matches the seed. These cells will be part of the cluster */
   //void findCellsAroundLayerMatched(const CaloConstCellContainer*, const CaloCell* cell, std::vector<const CaloCell*>& out, const float deta, const float dphi) const;

   /** finds TTs around a seed cell. These TTs will be part of the cluster. This helps to cover the part related to TileCall */
   // void findTTsAround(const xAOD::TriggerTowerContainer*, const CaloCell* cell, std::vector<const xAOD::TriggerTower*>& out) const ;

   // void findClusterFor(const std::vector<const CaloCell*>& scells, float &etaCluster, float &phiCluster, float &radiusCluster) const;


   };
}

#endif
