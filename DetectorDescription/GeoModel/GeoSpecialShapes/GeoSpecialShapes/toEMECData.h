/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef _TO_EMEC_DATA_H
#define _TO_EMEC_DATA_H
#include "GeoSpecialShapes/EMECData.h"
class IRDBAccessSvc;
class DecodeVersionKey;
#include <iostream> 
// This code is inline in order to be inoffensive in environments 
// that do not link to the full ATLAS software. Link dependencies
// then only arise when the routine is actually needed. 

inline EMECData toEMECData(IRDBAccessSvc *rdbAccess, const DecodeVersionKey  &larVersionKey) {

  EMECData data;

  // Little lambda function here: retreive recordset and fall back to default version if empty:
  auto getRecordsetPtr=[rdbAccess] (const std::string& node, const std::string& tag, const std::string& tag2node,const std::string & fallbackTag2Node) {

    // try with specified tag/node
    IRDBRecordset_ptr ptr=rdbAccess->getRecordsetPtr(node,tag, tag2node);
    // if failure, try with fallback tag/node
    if (ptr->size()==0) {
      ptr=rdbAccess->getRecordsetPtr(node,fallbackTag2Node);
    }
    return ptr;
  };


  IRDBRecordset_ptr emecGeometry        = getRecordsetPtr("EmecGeometry",        larVersionKey.tag(),larVersionKey.node(),"EmecGeometry-00");
  IRDBRecordset_ptr emecMagicNumbers    = getRecordsetPtr("EmecMagicNumbers",    larVersionKey.tag(), larVersionKey.node(),"EmecMagicNumbers-00");
  IRDBRecordset_ptr emecParams          = getRecordsetPtr("EmecParams",          larVersionKey.tag(), larVersionKey.node(),"EMECParams-00");
  IRDBRecordset_ptr emecWheelParameters = getRecordsetPtr("EmecWheelParameters", larVersionKey.tag(), larVersionKey.node(),"EmecWheelParameters-00");
  IRDBRecordset_ptr emecFan             = getRecordsetPtr("EmecFan",             larVersionKey.tag(), larVersionKey.node(),"EmecFan-00");
  IRDBRecordset_ptr coldContraction     = getRecordsetPtr("ColdContraction",     larVersionKey.tag(), larVersionKey.node(),"ColdContraction-00");
  

  for (size_t i=0;i<emecParams->size();i++) {
    EMECPARAMS emecparams;
    emecparams.PHIROTATION=(*emecParams)[i]->getString("PHIROTATION");
    emecparams.SAGGING=(*emecParams)[i]->getString("SAGGING");
    emecparams.INNERSLANTPARAM=(*emecParams)[i]->getString("INNERSLANTPARAM");
    emecparams.OUTERSLANTPARAM=(*emecParams)[i]->getString("OUTERSLANTPARAM");
    data.emecparams.push_back(emecparams);
  }


  for  (size_t i=0;i<emecWheelParameters->size();i++) {
    EMECWHEELPARAMETERS emecwheelparameters;
    emecwheelparameters.IWHEEL=(*emecWheelParameters)[i]->getInt("IWHEEL");
    emecwheelparameters.NABS=(*emecWheelParameters)[i]->getInt("NABS");
    emecwheelparameters.NACC=(*emecWheelParameters)[i]->getInt("NACC");
    emecwheelparameters.ETAINT=(*emecWheelParameters)[i]->getDouble("ETAINT");
    emecwheelparameters.ETAEXT=(*emecWheelParameters)[i]->getDouble("ETAEXT");
    emecwheelparameters.NWSAMP=(*emecWheelParameters)[i]->getInt("NWSAMP");
    data.emecwheelparameters.push_back(emecwheelparameters);
  }

  for (size_t i=0;i<emecMagicNumbers->size();i++) {
    EMECMAGICNUMBERS emecmagicnumbers;
    emecmagicnumbers.STRAIGHTSTARTSECTION=(*emecMagicNumbers)[i]->getDouble("STRAIGHTSTARTSECTION");
    emecmagicnumbers.FOCALTOREF=(*emecMagicNumbers)[i]->getDouble("FOCALTOREF");
    emecmagicnumbers.REFTOACTIVE=(*emecMagicNumbers)[i]->getDouble("REFTOACTIVE");
    emecmagicnumbers.ACTIVELENGTH=(*emecMagicNumbers)[i]->getDouble("ACTIVELENGTH");
    emecmagicnumbers.REFTOPRESAMPLER=(*emecMagicNumbers)[i]->getDouble("REFTOPRESAMPLER");
    emecmagicnumbers.PRESAMPLERLENGTH=(*emecMagicNumbers)[i]->getDouble("PRESAMPLERLENGTH");
    data.emecmagicnumbers.push_back(emecmagicnumbers);
  }

  for (size_t i=0;i<emecFan->size();i++) {
    EMECFAN emecfan;
    emecfan.LEADTHICKNESSINNER=(*emecFan)[i]->getDouble("LEADTHICKNESSINNER");
    emecfan.LEADTHICKNESSOUTER=(*emecFan)[i]->getDouble("LEADTHICKNESSOUTER");
    emecfan.STEELTHICKNESS=(*emecFan)[i]->getDouble("STEELTHICKNESS");
    emecfan.GLUETHICKNESS=(*emecFan)[i]->getDouble("GLUETHICKNESS");
    emecfan.ELECTRODETOTALTHICKNESS=(*emecFan)[i]->getDouble("ELECTRODETOTALTHICKNESS");
    data.emecfan.push_back(emecfan);
  }


  for (size_t i=0;i<coldContraction->size();i++) {
    COLDCONTRACTION coldcontraction;
    coldcontraction.ABSORBERCONTRACTION=(*coldContraction)[i]->getDouble("ABSORBERCONTRACTION");
    coldcontraction.G10RINGCONTRACTION=(*coldContraction)[i]->getDouble("G10RINGCONTRACTION");
    coldcontraction.MOTHERBOARDCONTRACTION=(*coldContraction)[i]->getDouble("MOTHERBOARDCONTRACTION");
    coldcontraction.CABLECONTRACTION=(*coldContraction)[i]->getDouble("CABLECONTRACTION");
    coldcontraction.COLDWINDOWCONTRACTION=(*coldContraction)[i]->getDouble("COLDWINDOWCONTRACTION");
    coldcontraction.ELECTRODEINVCONTRACTION=(*coldContraction)[i]->getDouble("ELECTRODEINVCONTRACTION");
    data.coldcontraction.push_back(coldcontraction);
  }


  for (size_t i=0;i<emecGeometry->size();i++) {
    EMECGEOMETRY emecgeometry;
    emecgeometry.EFIELD=(*emecGeometry)[i]->getDouble("EFIELD");
    emecgeometry.MLGAP=(*emecGeometry)[i]->getDouble("MLGAP");
    emecgeometry.SABL=(*emecGeometry)[i]->getDouble("SABL");
    emecgeometry.SBAR=(*emecGeometry)[i]->getDouble("SBAR");
    emecgeometry.SBRN=(*emecGeometry)[i]->getDouble("SBRN");
    emecgeometry.SCRB=(*emecGeometry)[i]->getDouble("SCRB");
    emecgeometry.Z1=(*emecGeometry)[i]->getDouble("Z1");
    emecgeometry.Z0=(*emecGeometry)[i]->getDouble("Z0");
    emecgeometry.DCF=(*emecGeometry)[i]->getDouble("DCF");
    emecgeometry.RLIMIT=(*emecGeometry)[i]->getDouble("RLIMIT");
    emecgeometry.EPS=(*emecGeometry)[i]->getDouble("EPS");
    emecgeometry.RHOIN=(*emecGeometry)[i]->getDouble("RHOIN");
    emecgeometry.RCKP=(*emecGeometry)[i]->getDouble("RCKP");
    emecgeometry.ETOT=(*emecGeometry)[i]->getDouble("ETOT");
    emecgeometry.EKAP=(*emecGeometry)[i]->getDouble("EKAP");
    emecgeometry.ECU=(*emecGeometry)[i]->getDouble("ECU");
    emecgeometry.EPREP=(*emecGeometry)[i]->getDouble("EPREP");
    emecgeometry.EINOX=(*emecGeometry)[i]->getDouble("EINOX");
    emecgeometry.NLAYER=(*emecGeometry)[i]->getInt("NLAYER");
    emecgeometry.RMIN=(*emecGeometry)[i]->getDouble("RMIN");
    emecgeometry.RMAX=(*emecGeometry)[i]->getDouble("RMAX");
    emecgeometry.DZENDE=(*emecGeometry)[i]->getDouble("DZENDE");
    emecgeometry.ZORIG=(*emecGeometry)[i]->getDouble("ZORIG");
    emecgeometry.DCRACK=(*emecGeometry)[i]->getDouble("DCRACK");
    emecgeometry.DZPLA=(*emecGeometry)[i]->getDouble("DZPLA");
    emecgeometry.DZDSE=(*emecGeometry)[i]->getDouble("DZDSE");
    emecgeometry.DZDSM=(*emecGeometry)[i]->getDouble("DZDSM");
    emecgeometry.DZDSI=(*emecGeometry)[i]->getDouble("DZDSI");
    emecgeometry.NUREG=(*emecGeometry)[i]->getInt("NUREG");
    emecgeometry.DETA_0=(*emecGeometry)[i]->getDouble("DETA_0");
    emecgeometry.DETA_1=(*emecGeometry)[i]->getDouble("DETA_1");
    emecgeometry.DETA_2=(*emecGeometry)[i]->getDouble("DETA_2");
    emecgeometry.DETA_3=(*emecGeometry)[i]->getDouble("DETA_3");
    emecgeometry.DETA_4=(*emecGeometry)[i]->getDouble("DETA_4");
    emecgeometry.DETA_5=(*emecGeometry)[i]->getDouble("DETA_5");
    emecgeometry.DETA_6=(*emecGeometry)[i]->getDouble("DETA_6");
    emecgeometry.DETA_7=(*emecGeometry)[i]->getDouble("DETA_7");
    emecgeometry.DETA_8=(*emecGeometry)[i]->getDouble("DETA_8");
    emecgeometry.DETA_9=(*emecGeometry)[i]->getDouble("DETA_9");
    emecgeometry.DETA_10=(*emecGeometry)[i]->getDouble("DETA_10");
    emecgeometry.DPHI_0=(*emecGeometry)[i]->getDouble("DPHI_0");
    emecgeometry.DPHI_1=(*emecGeometry)[i]->getDouble("DPHI_1");
    emecgeometry.DPHI_2=(*emecGeometry)[i]->getDouble("DPHI_2");
    emecgeometry.DPHI_3=(*emecGeometry)[i]->getDouble("DPHI_3");
    emecgeometry.DPHI_4=(*emecGeometry)[i]->getDouble("DPHI_4");
    emecgeometry.DPHI_5=(*emecGeometry)[i]->getDouble("DPHI_5");
    emecgeometry.DPHI_6=(*emecGeometry)[i]->getDouble("DPHI_6");
    emecgeometry.DPHI_7=(*emecGeometry)[i]->getDouble("DPHI_7");
    emecgeometry.DPHI_8=(*emecGeometry)[i]->getDouble("DPHI_8");
    emecgeometry.DPHI_9=(*emecGeometry)[i]->getDouble("DPHI_9");
    emecgeometry.DPHI_10=(*emecGeometry)[i]->getDouble("DPHI_10");
    emecgeometry.ETASTR_0=(*emecGeometry)[i]->getDouble("ETASTR_0");
    emecgeometry.ETASTR_1=(*emecGeometry)[i]->getDouble("ETASTR_1");
    emecgeometry.ETASTR_2=(*emecGeometry)[i]->getDouble("ETASTR_2");
    emecgeometry.ETASTR_3=(*emecGeometry)[i]->getDouble("ETASTR_3");
    emecgeometry.ETASTR_4=(*emecGeometry)[i]->getDouble("ETASTR_4");
    emecgeometry.ETASTR_5=(*emecGeometry)[i]->getDouble("ETASTR_5");
    emecgeometry.ETASTR_6=(*emecGeometry)[i]->getDouble("ETASTR_6");
    emecgeometry.EMHIT=(*emecGeometry)[i]->getDouble("EMHIT");
    emecgeometry.EMDIGI=(*emecGeometry)[i]->getDouble("EMDIGI");
    emecgeometry.GAP0=(*emecGeometry)[i]->getDouble("GAP0");
    emecgeometry.ZSHIFT=(*emecGeometry)[i]->getDouble("ZSHIFT");
    data.emecgeometry.push_back(emecgeometry);
  }


  return data;
}


#endif
