/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

CaloGeometryFromCaloDDM::CaloGeometryFromCaloDDM()
  : CaloGeometry()
  , AthMessaging("CaloGeometryFromCaloDDM")
{
}

CaloGeometryFromCaloDDM::~CaloGeometryFromCaloDDM()
= default;

bool CaloGeometryFromCaloDDM::LoadGeometryFromCaloDDM(const CaloDetDescrManager* calo_dd_man)
{
  int jentry=0;
  for(CaloDetDescrManager::calo_element_const_iterator calo_iter=calo_dd_man->element_begin();calo_iter<calo_dd_man->element_end();++calo_iter) {
    const CaloDetDescrElement* pcell=*calo_iter;
    addcell(pcell);
  
    if(jentry%25000==0) {
      ATH_MSG_DEBUG(jentry<<" : "<<pcell->getSampling()<<", "<<pcell->identify());
    }
    ++jentry;
  }

  return PostProcessGeometry();
}
bool CaloGeometryFromCaloDDM::LoadFCalChannelMapFromFCalDDM(const FCALDetectorManager* fcal_dd_man){
   this->SetFCal_ChannelMap( fcal_dd_man->getChannelMap() );
   this->calculateFCalRminRmax();
   return this->checkFCalGeometryConsistency();
}
