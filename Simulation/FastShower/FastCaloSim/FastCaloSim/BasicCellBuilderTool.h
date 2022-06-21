/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASTCALOSIM_BASICCELLBUILDERTOOL_H
#define FASTCALOSIM_BASICCELLBUILDERTOOL_H

/**
 *  @file   CellBuilderTool.cxx
 *  @brief  Building Cells objects from Atlfast
 *  @author Michael Duehrssen
 */

#include "GaudiKernel/AlgTool.h"

#include <math.h>

#include "CaloInterface/ICaloCellMakerTool.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "FastCaloSim/FastCaloSim_CaloCell_ID.h"
#include "AthenaBaseComps/AthAlgTool.h"

class CaloDetDescrManager;
class CaloCellContainer;
class AtlasDetectorID;
class CellInfoContainer;

class BasicCellBuilderTool: public extends<AthAlgTool, ICaloCellMakerTool>
{
 public:
  BasicCellBuilderTool(
                       const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~BasicCellBuilderTool() = default;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

protected:
  double deta(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  void   minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta, const CellInfoContainer* cont) const;
  double rzmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  double rzent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  double rmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  double rent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  double zmid(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;
  double zent(CaloCell_ID_FCS::CaloSample sample,double eta, const CellInfoContainer* cont) const;

  bool m_isCaloBarrel[CaloCell_ID_FCS::MaxSample];
  bool isCaloBarrel(CaloCell_ID_FCS::CaloSample sample) const {return m_isCaloBarrel[sample];};
};

#endif
