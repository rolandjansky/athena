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

#include "FastCaloSim/CellInfoContainer.h"
#include "FastCaloSim/FSmap.h"

class CaloCellContainer;
class CaloDetDescrManager;
class CaloCellContainer;
class AtlasDetectorID;

class BasicCellBuilderTool: public extends<AthAlgTool, ICaloCellMakerTool>
{
public:
  //  typedef std::vector< std::vector<cellinfo_vec> > cellinfo_map;
  typedef cellinfo_map::cellinfo cellinfo;
  typedef cellinfo_map::cellinfo_vec cellinfo_vec;

  BasicCellBuilderTool(
                       const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~BasicCellBuilderTool();


  virtual StatusCode initialize() override;

  // update theCellContainer
  virtual StatusCode process (CaloCellContainer* theCellContainer,
                              const EventContext& ctx) const override;

protected:
  void init_all_maps(const CaloDetDescrManager* caloDDM);
  void init_cell(cellinfo_map& map,const CaloDetDescrElement* theDDE);
  void init_volume(cellinfo_map& map);
  void find_phi0(const CaloDetDescrManager* caloDDM);

  //  void init_map(cellinfo_map& map, int layer); //layer 1=EM 3=HAD
  void findCells(const CaloDetDescrManager* caloDDM,
                 cellinfo_vec & cell_vec, double eta_min, double eta_max, double phi_min, double phi_max, int layer);


  void addCell(CaloCellContainer * theCellContainer, int etabin, int phibin, double energy, cellinfo_map& map );

  Identifier m_id;
  double m_phi0_em;
  double m_phi0_had;

  CellInfoContainer m_cellinfoCont;

  double                   m_min_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  double                   m_max_eta_sample[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_rmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zmid_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_rent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]
  FSmap< double , double > m_zent_map[2][CaloCell_ID_FCS::MaxSample]; //[side][calosample]


  double deta(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  void   minmaxeta(CaloCell_ID_FCS::CaloSample sample,double eta,double& mineta,double& maxeta) const;
  double rzmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rzent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double rent(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double zmid(CaloCell_ID_FCS::CaloSample sample,double eta) const;
  double zent(CaloCell_ID_FCS::CaloSample sample,double eta) const;

  const CaloCell_ID*  m_caloCID;

  bool m_isCaloBarrel[CaloCell_ID_FCS::MaxSample];
  bool isCaloBarrel(CaloCell_ID_FCS::CaloSample sample) const {return m_isCaloBarrel[sample];};
};

#endif
