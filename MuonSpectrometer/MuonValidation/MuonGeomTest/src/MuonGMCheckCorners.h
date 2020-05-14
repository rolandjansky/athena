/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test MuonGMCheckCorners from digits to pos. in space
 ----------------------------------------------
***************************************************************************/

#ifndef MUONGEOMODEL_MUONGMCHECKCORNERS_H
#define MUONGEOMODEL_MUONGMCHECKCORNERS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"

using namespace MuonGM;

class MuonGMCheckCorners: public AthAlgorithm
{
 public:
    
  MuonGMCheckCorners(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonGMCheckCorners()=default;
    
  StatusCode	initialize();
  StatusCode 	execute(){return StatusCode::SUCCESS;}

 private:

  NTuple::Item<long>  m_statPhi;
  NTuple::Item<long>  m_statML;
  NTuple::Item<long>  m_MLcorner;
  NTuple::Item<float>  m_x;
  NTuple::Item<float>  m_y;
  NTuple::Item<float>  m_z;

  // User setable properties
  int m_check_mdt;
  int m_check_rpc;
  int m_check_rpcrsmap;
  int m_check_mdtrsmap;
  int m_check_tgcrsmap;
  int m_check_tgc;
  int m_check_csc;
  int m_check_parent;
  int m_check_surfaces;
  int m_print_level;
  int m_tgcgood;
  int m_rpcgood;
  int m_mdtgood;    
  int m_station_index;

  // Pointers to the event
  StoreGateSvc*      	        p_EventStore;
  ActiveStoreSvc*      	        p_ActiveStore;

  void checkreadoutcscgeo();
  void checkreadoutrpcgeo();
  void buildRpcRegionSelectorMap();
  void buildMdtRegionSelectorMap();
  void buildTgcRegionSelectorMap();
  void checkreadoutmdtgeo();
  void checkParentStation();
  void checkreadouttgcgeo();
  void showPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc, Identifier id);
  void showMdtPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc);
  void showTgcPointComparison(HepGeom::Point3D<double> gmP, HepGeom::Point3D<double> ddP, const MuonGM::MuonReadoutElement* rpc);
    
};


#endif // MUONGEOMODEL_MUONGMCHECKCORNERS_H
