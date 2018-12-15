/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "DBM_Services.h"

#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTube.h"

GeoVPhysVol* DBM_PP0::Build()  
{
  double pp0_Rmin = m_gmt_mgr->DBMPP0RIn();
  double pp0_Rmax = m_gmt_mgr->DBMPP0ROut();
  double pp0_thick = m_gmt_mgr->DBMPP0Thick();

  // materials

  const GeoTube* pp0Ring = new GeoTube(pp0_Rmin, pp0_Rmax, pp0_thick/2.);
  const GeoMaterial* dbmPP0_mat = m_mat_mgr->getMaterialForVolume("pix::DBMPP0", pp0Ring->volume());
  const GeoLogVol* pp0Log = new GeoLogVol("dbmPP0Log", pp0Ring, dbmPP0_mat);
  GeoPhysVol* pp0Phys = new GeoPhysVol(pp0Log);

  return pp0Phys;
}
