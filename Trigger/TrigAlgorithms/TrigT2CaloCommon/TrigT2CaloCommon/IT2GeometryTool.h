/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     IT2GeometryTool.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 
 AUTHOR:   Denis Oliveira Damazio
           Carlos Osuna Escamilla
 
 PURPOSE:  This Tools provides the information about whether
	   a cell is part of a cluster for different cluster sizes.
*******************************************************************/


#ifndef TRIGT2CALOCOMMON_IT2GEOMETRYTOOL_H
#define TRIGT2CALOCOMMON_IT2GEOMETRYTOOL_H

#include "GaudiKernel/IAlgTool.h"


class IT2GeometryTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IT2GeometryTool, 1, 0);

  /** @brief This prepares internal tables based on the cluster position.
  *   Such tables should be used to determine whether a cells is
  *   inside clusters with different sizes
  * @param[in] nCaloType should be 0 for LAr and 1 for TileCal.
  * @param[in] nCaloSamp should be calorimeter sample.
  * @param[in] energyEta : eta of hottest cell.
  * @param[in] energyPhi : phi of hottest cell.
  */
  virtual int EtaPhiRange(const int nCaloType, const int nCaloSamp,
                  const double& energyEta, const double& energyPhi) = 0;

  /** @brief Finds whether a cell is in a Normal Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See IT2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
  virtual bool CellInNorCluster( const int nCaloType,const int nCaloSamp,
                           const double& etaCell, const double& phiCell) = 0;
  /** @brief Finds whether a cell is in a Narrow Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See IT2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
  virtual bool CellInNarCluster( const int nCaloType,const int nCaloSamp,
                           const double& etaCell, const double& phiCell) = 0;
  /** @brief Finds whether a cell is in a Wide Sized Cluster 
  * @param[in] nCaloType, nCaloSamp : See IT2GeometryTool::EtaPhiRange.
  * @param[in] etaCell : Cell eta position.
  * @param[in] phiCell : Cell phi position.
  * @return cell is or not in this cluster.
  */
  virtual bool CellInWidCluster( const int nCaloType,const int nCaloSamp,
                           const double& etaCell, const double& phiCell) = 0;

  virtual int nStripetaEMnar(const int layer)  const = 0;
  virtual int nStripphiEMnar(const int layer) const = 0;

  virtual int nStripetaEMwid(const int layer) const = 0;
  virtual int nStripphiEMwid(const int layer) const = 0;

  virtual int nStripetaEMnor(const int layer)  const = 0;
  virtual int nStripphiEMnor(const int layer)  const = 0;

  virtual int  nStripetaHADnar(const int layer) const = 0;
  virtual int  nStripphiHADnar(const int layer) const = 0;

  virtual int  nStripetaHADwid(const int layer) const = 0;
  virtual int  nStripphiHADwid(const int layer) const = 0;

  virtual int  nStripetaHADnor(const int layer)  const = 0;
  virtual int  nStripphiHADnor(const int layer)  const = 0;

 protected:
  
  virtual int EtaPhiRangeInt(const int layer, const int& nStripEta,
                const int& nStripPhi,const double& energyEta,
                const double& energyPhi,const int index_size) = 0;
        
};

#endif
