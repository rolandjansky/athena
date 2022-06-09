/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMPARAMETRIZATION_CALOGEOMETRYFROMCALODDM_H
#define ISF_FASTCALOSIMPARAMETRIZATION_CALOGEOMETRYFROMCALODDM_H

#include "ISF_FastCaloSimParametrization/CaloGeometry.h"
#include "LArReadoutGeometry/FCALDetectorManager.h"
#include "AthenaBaseComps/AthMessaging.h"

class CaloDetDescrManager;
class FCALDetectorManager;

class CaloGeometryFromCaloDDM : public CaloGeometry, public AthMessaging {
public :
   CaloGeometryFromCaloDDM();
   virtual ~CaloGeometryFromCaloDDM();
   
   bool LoadGeometryFromCaloDDM(const CaloDetDescrManager* calo_dd_man);
   bool LoadFCalChannelMapFromFCalDDM(const FCALDetectorManager* fcal_dd_man);
};

#endif

