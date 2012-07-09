/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_GeoModelAthenaComps.h"
#include "InDetCondServices/ISiLorentzAngleSvc.h"

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : InDetDD::AthenaComps("SCT_GeoModel"),
    m_lorentzAngleSvc("",""),
    m_idHelper(0)
{}
 
void 
SCT_GeoModelAthenaComps::setLorentzAngleSvc(const ServiceHandle<ISiLorentzAngleSvc> & lorentzAngleSvc)
{
  m_lorentzAngleSvc = lorentzAngleSvc;
}

void 
SCT_GeoModelAthenaComps::setIdHelper(const SCT_ID* idHelper)
{
  m_idHelper = idHelper;
}

const ServiceHandle<ISiLorentzAngleSvc> & 
SCT_GeoModelAthenaComps::lorentzAngleSvc() const
{
  return m_lorentzAngleSvc;
}

const SCT_ID* 
SCT_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

