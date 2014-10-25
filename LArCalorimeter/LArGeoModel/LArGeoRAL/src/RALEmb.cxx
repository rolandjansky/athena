/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// RALEmb
// Sep 5 JFB

// Fetch parameters from a NOVA database structure.

#include "LArGeoRAL/RALEmb.h"
#include "LArGeoCode/VDetectorParameters.h"

// Athena Service classes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/PhysicalConstants.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdexcept>

class LArGeo::RALEmb::Clockwork {

public:

  Clockwork(){}
  ~Clockwork(){}
  

  IRDBRecordset_ptr barrelGeometry;
  IRDBRecordset_ptr barrelSagging;
  IRDBRecordset_ptr barrelAccordionCables;
  IRDBRecordset_ptr barrelMotherboards;
  IRDBRecordset_ptr barrelLongDiv;
  IRDBRecordset_ptr presamplerGeometry;
  IRDBRecordset_ptr barrelEtaTrans;
  IRDBRecordset_ptr coldContraction;
};


LArGeo::RALEmb::RALEmb():
  c(new Clockwork())
{
  // First, fetch the Athena services.
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap.h


  StatusCode sc;
  IRDBAccessSvc *pAccessSvc;
  sc = svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }



  IGeoModelSvc *geoModel;
  sc = svcLocator->service ("GeoModelSvc",geoModel);
  if (sc != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }

  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();

  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";


  if ( LArVersion.empty()) {
     LArVersion=pAccessSvc->getChildTag("LAr",AtlasVersion,"ATLAS");
  }
  m_oldDB=false; 
  if (LArVersion == "LAr-00" || LArVersion == "LAr-01" ||
      LArVersion == "LAr-Rome-Initial-00" ||
      LArVersion == "LAr-H6-00" ||
      LArVersion == "LAr-Commissioning-00" ||
      LArVersion == "LAr-G3-00") {
     m_oldDB=true;
     std::cout << " in RALEmb: old database tag used, some values are hard coded" << std::endl;
     std::cout << " Non projectivity of lead transition will not be simulated " << std::endl;
   } 
   m_oldSagging=false;
   if (LArVersion == "LAr-00" || LArVersion == "LAr-01" || LArVersion == "LAr-02" ||
      LArVersion == "LAr-Rome-Initial-00" ||
      LArVersion == "LAr-H8-00" ||
      LArVersion == "LAr-H6-00" ||
      LArVersion == "LAr-Commissioning-00" ||
      LArVersion == "LAr-G3-00") {
     m_oldSagging=true;
     std::cout << " in RALEmb: only old sagging values available" << std::endl;
   }
   m_oldContract=false;
   if (LArVersion == "LAr-00" || LArVersion == "LAr-01" || LArVersion == "LAr-02" ||
       LArVersion == "LAr-03" || LArVersion == "LAr-04" ||
       LArVersion == "LAr-Rome-Initial-00" ||
       LArVersion == "LAr-H8-00" ||
       LArVersion == "LAr-H6-00" ||
       LArVersion == "LAr-Commissioning-00" ||
       LArVersion == "LAr-Commissioning-01" ||
       LArVersion == "LAr-G3-00" ||
       LArVersion == "LAr-02-Align-00" ||
       LArVersion == "LAr-02-Align-01" ||
       LArVersion == "LAr-H6-2002-00" ||
       LArVersion == "LAr-H6-2003-00" ||
       LArVersion == "LAr-TBEC-00" ) {
      m_oldContract=true;
   }



  c->barrelGeometry             = pAccessSvc->getRecordsetPtr("BarrelGeometry",detectorKey, detectorNode); 
  c->barrelSagging              = pAccessSvc->getRecordsetPtr("BarrelSagging",detectorKey, detectorNode);
  c->barrelAccordionCables      = pAccessSvc->getRecordsetPtr("BarrelAccordionCables",detectorKey, detectorNode);
  c->barrelMotherboards         = pAccessSvc->getRecordsetPtr("BarrelMotherboards",detectorKey, detectorNode);
  c->barrelLongDiv              = pAccessSvc->getRecordsetPtr("BarrelLongDiv",detectorKey, detectorNode);
  c->presamplerGeometry         = pAccessSvc->getRecordsetPtr("PresamplerGeometry",detectorKey, detectorNode);
  if (!m_oldDB) c->barrelEtaTrans        = pAccessSvc->getRecordsetPtr("BarrelEtaTrans",detectorKey,detectorNode);

  if (!m_oldContract) c->coldContraction  = pAccessSvc->getRecordsetPtr("ColdContraction",detectorKey,detectorNode);
}


LArGeo::RALEmb::~RALEmb()
{
  delete c;
}


double LArGeo::RALEmb::GetValue(const std::string& a_name,
                                const int a0,
                                const int /*a1*/,
                                const int /*a2*/,
                                const int /*a3*/,
                                const int /*a4*/ ) 
{
  
  std::ostringstream A0STR;
  A0STR << "_" << a0;
  const std::string A0 = A0STR.str();

  if ( a_name == "LArEMBnoOFPhysPhiCell"      ) return (*c->barrelGeometry)[0]->getInt("NCELMX");
// GU 7-oct-05  phifirst is stored in radians not in degre
  if ( a_name == "LArEMBAbsPhiFirst"          ) return (*c->barrelGeometry)[0]->getDouble("PHIFIRST");
  if ( a_name == "LArEMBPhiGapWidth"          ) return (*c->barrelGeometry)[0]->getDouble("PHIGAP")*CLHEP::cm;
  if ( a_name == "LArEMBPhiGapAperture"       ) return (*c->barrelGeometry)[0]->getDouble("ALFA")*CLHEP::deg;
  if ( a_name == "LArEMBMotherRmin"           ) return (*c->barrelGeometry)[0]->getDouble("RMIN")*CLHEP::cm;
  if ( a_name == "LArEMBMotherRmax"           ) return (*c->barrelGeometry)[0]->getDouble("RMAX")*CLHEP::cm;
  if ( a_name == "LArEMBFiducialRmax"         ) return (*c->barrelGeometry)[0]->getDouble("ROUT_AC")*CLHEP::cm;
  if ( a_name == "LArEMBMotherZmin"           ) return (*c->barrelGeometry)[0]->getDouble("ZMIN")*CLHEP::cm;
  if ( a_name == "LArEMBMotherZmax"           ) return (*c->barrelGeometry)[0]->getDouble("ZMAX")*CLHEP::cm;
  if ( a_name == "LArEMBnoOFAccZigs"          ) return (*c->barrelGeometry)[0]->getInt("NBRT");
  if ( a_name == "LArEMBnoOFAccSamps"         ) return (*c->barrelGeometry)[0]->getInt("NSTAMX");
  if ( a_name == "LArEMBInnerElectronics"     ) return (*c->barrelGeometry)[0]->getDouble("XEL1")*CLHEP::cm;
  if ( a_name == "LArEMBLArGapTail"           ) return (*c->barrelGeometry)[0]->getDouble("XTAL")*CLHEP::cm;
  if ( a_name == "LArEMBG10SupportBarsIn"     ) return (*c->barrelGeometry)[0]->getDouble("XG10")*CLHEP::cm;
  if ( a_name == "LArEMBG10SupportBarsOut"    ) return (*c->barrelGeometry)[0]->getDouble("XGSB")*CLHEP::cm;
  if ( a_name == "LArEMBZmin"                 ) return (*c->barrelGeometry)[0]->getDouble("ZMIN")*CLHEP::cm;
  if ( a_name == "LArEMBZmax"                 ) return (*c->barrelGeometry)[0]->getDouble("ZMAX")*CLHEP::cm;
  if ( a_name == "LArEMBAccTrackMin"          ) return (*c->barrelGeometry)[0]->getDouble("RHOMIN")*CLHEP::cm;
  if ( a_name == "LArEMBRadiusInnerAccordion" ) return (*c->barrelGeometry)[0]->getDouble("RIN_AC")*CLHEP::cm;
  if ( a_name == "LArEMBRadiusOuterAccordion" ) return (*c->barrelGeometry)[0]->getDouble("ROUT_AC")*CLHEP::cm;
  if ( a_name == "LArEMBMaxEtaAcceptance"     ) return (*c->barrelGeometry)[0]->getDouble("ETACUT");
  if ( a_name == "LArEMBThickEtaAcceptance"   ) return (*c->barrelGeometry)[0]->getDouble("ETACU1");
  if ( a_name == "LArEMBRadiusAtCurvature"    ) return (*c->barrelGeometry)[0]->getDouble("RHOCEN"+A0)*CLHEP::cm;
  if ( a_name == "LArEMBPhiAtCurvature"       ) return (*c->barrelGeometry)[0]->getDouble("PHICEN"+A0)*CLHEP::deg;
  if ( a_name == "LArEMBDeltaZigAngle"        ) return (*c->barrelGeometry)[0]->getDouble("DELTA"+A0)*CLHEP::deg;
  if ( a_name == "LArEMBNeutFiberRadius"      ) return (*c->barrelGeometry)[0]->getDouble("RINT")*CLHEP::cm;
  if ( a_name == "LArEMBLeadTipThickFront"    ) return (*c->barrelGeometry)[0]->getDouble("XTIP_PB")*CLHEP::cm;
  if ( a_name == "LArEMBLeadTipThickEnd"      ) return (*c->barrelGeometry)[0]->getDouble("XTIP_PC")*CLHEP::cm;
  if ( a_name == "LArEMBG10TipThickFront"     ) return (*c->barrelGeometry)[0]->getDouble("XTIP_GT")*CLHEP::cm;
  if ( a_name == "LArEMBG10TipThickEnd"       ) return (*c->barrelGeometry)[0]->getDouble("XTIP_GS")*CLHEP::cm;
  if ( a_name == "LArEMBThinAbsGlue"          ) return (*c->barrelGeometry)[0]->getDouble("TGGL")*CLHEP::cm;
  if ( a_name == "LArEMBThinAbsIron"          ) return (*c->barrelGeometry)[0]->getDouble("TGFE")*CLHEP::cm;
  if ( a_name == "LArEMBThinAbsLead"          ) return (*c->barrelGeometry)[0]->getDouble("TGPB")*CLHEP::cm;
  if ( a_name == "LArEMBThickAbsGlue"         ) return (*c->barrelGeometry)[0]->getDouble("THGL")*CLHEP::cm;
  if ( a_name == "LArEMBThickAbsIron"         ) return (*c->barrelGeometry)[0]->getDouble("THFE")*CLHEP::cm;
  if ( a_name == "LArEMBThickAbsLead"         ) return (*c->barrelGeometry)[0]->getDouble("THPB")*CLHEP::cm;
  if ( a_name == "LArEMBThickElecCopper"      ) return (*c->barrelGeometry)[0]->getDouble("THCU")*CLHEP::cm;
  if ( a_name == "LArEMBThickElecKapton"      ) return (*c->barrelGeometry)[0]->getDouble("THFG")*CLHEP::cm;
  if ( a_name == "LArEMBRminHighZ"            ) {
      if (!m_oldDB) return (*c->barrelGeometry)[0]->getDouble("RMINHIGHZ")*CLHEP::cm;
      else return 154.8*CLHEP::cm;
  }
  if ( a_name == "LArEMBDeltaRS12"            ) {
      if (!m_oldDB) return (*c->barrelGeometry)[0]->getDouble("DELTARS12")*CLHEP::cm;
      else return 0.11*CLHEP::cm;
  }
  if ( a_name == "LArEMBphiMaxBarrel"         ) {
      if (!m_oldDB) return (*c->barrelGeometry)[0]->getDouble("PHIMAXBARREL");
      else  return 360.;
  }
  if ( a_name == "LArEMBG10FrontDeltaZ"       )  {
      if (!m_oldDB) return (*c->barrelGeometry)[0]->getDouble("G10FRONTDELTAZ")*CLHEP::cm;
      else return 309.4*CLHEP::cm;
  }

  //


  // 2) FROM ASAG structure


    if ( a_name == "LArEMBSaggingAmplitude"     ) {
       return (*c->barrelSagging)[0]->getDouble("SAG"+A0)*CLHEP::cm;
    }
    if ( a_name == "LArEMBSaggingAmplitude2"    ) {
       if (!m_oldSagging) return (*c->barrelSagging)[0]->getDouble("SAG"+A0+"_X")*CLHEP::cm;
       else return 0;
    }



  //

  // 3) FROM ACCA ( electronics cables)

  if ( a_name == "LArEMBmasspercentCu"        ) return (*c->barrelAccordionCables)[0]->getDouble("PERCU");
  if ( a_name == "LArEMBmasspercentKap"       ) return (*c->barrelAccordionCables)[0]->getDouble("PERKAP");
  if ( a_name == "LArEMBCablethickat0"        ) return (*c->barrelAccordionCables)[0]->getDouble("THICKIN")*CLHEP::cm;
  if ( a_name == "LArEMBthickincrfac"         ) return (*c->barrelAccordionCables)[0]->getDouble("THICKFAC")*CLHEP::cm;
  if ( a_name == "LArEMBCableEtaheight"       ) return (*c->barrelAccordionCables)[0]->getDouble("HEIGHT")*CLHEP::cm;
  if ( a_name == "LArEMBnoOFcableBundle"      ) return (*c->barrelAccordionCables)[0]->getInt("ACCORCAB");
  if ( a_name == "LArEMBCablclearfrPS"        ) return (*c->barrelAccordionCables)[0]->getDouble("CLEARANCE")*CLHEP::cm;


  // 4) FROM (*C->BARRELMOTHERBOARDS) ( electronics mother_boards)
  if ( a_name == "LArEMBEpoxyVolumicMass"     ) return (*c->barrelMotherboards)[0]->getDouble("DG10")*CLHEP::g/CLHEP::cm3;
  if ( a_name == "LArEMBCuThickness"          ) return (*c->barrelMotherboards)[0]->getDouble("THICU")*CLHEP::cm;
  if ( a_name == "LArEMBG10Thickness"         ) return (*c->barrelMotherboards)[0]->getDouble("THIG10")*CLHEP::cm;
  if ( a_name == "LArEMBMoBoTchickness"       ) return (*c->barrelMotherboards)[0]->getDouble("THIMB")*CLHEP::cm;
  if ( a_name == "LArEMBMoBoHeight"           ) return (*c->barrelMotherboards)[0]->getDouble("HEIGHTMB")*CLHEP::cm;
  if ( a_name == "LArEMBnoOFmothboard"        ) return (*c->barrelMotherboards)[0]->getInt("MOTHBOAR");
  if ( a_name == "LArEMBMoBoclearfrPS"        ) return (*c->barrelMotherboards)[0]->getDouble("CLEARANCE")*CLHEP::cm;

  //

  // 5) FROM ACCO ( Longitudinal Barrel Parameters)
  if ( a_name == "LArEMBfiducialMothZmax"     ) return (*c->barrelLongDiv)[0]->getDouble("ZMAXACT")*CLHEP::cm;
  if ( a_name == "LArEMBfiducialMothZmin"     ) {
     if (!m_oldDB) return (*c->barrelLongDiv)[0]->getDouble("ZMINACT")*CLHEP::cm;
     else return 0.4*CLHEP::cm;
  }

  //

  // 6) FROM COPG ( PreSampler Barrel Parameters)
  if ( a_name == "LArEMBarrelRmin"            ) return (*c->presamplerGeometry)[0]->getDouble("RMIN")*CLHEP::cm;

  // 7) FROM BarrelEtaTrans
  if ( a_name == "LArEMBEtaTrans"             ) {
     if (!m_oldDB) return (*c->barrelEtaTrans)[a0]->getDouble("ETATRANS");
     else return 0.8;
  }

  // 8) FROM ColdContraction
  if (!m_oldContract) {
     if (a_name == "LArEMBAbsorberContraction") return (*c->coldContraction)[0]->getDouble("ABSORBERCONTRACTION");
  }
  else {
     if (a_name == "LArEMBAbsorberContraction") return 0.997;
  }

  // We didn't find a match.  
  std::string errMessage = "RALEmb::GetValue: could not find a match for the key '" + a_name;
  std::cerr << errMessage << std::endl;
  throw std::runtime_error (errMessage.c_str());

  // Unreached.
}
