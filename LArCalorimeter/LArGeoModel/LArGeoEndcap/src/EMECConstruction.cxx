/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// EMECConstruction

// Revision history:

//  LArEMECConstruction:
// 09-May-2001 WGS: Copied from Andrei Soukharev's EMECDetectorConstruction.
// 09-May-2001 WGS: Put in the LAr parent volumes for the endcaps.
// 09-Jan-2002 WGS: The LAr parent volume now comes from the calling routine.
//                  Any shifts in z are handled in the parent volume.
// 02-Dec-2002 J.Toth: wheels' shape and position are corrected;
//                   : support rings in front of sens.volume are introduced,
//                             like in endegeo.age of dice
// 13-Dec-2002 J.Toth: electrode shape is corrected
// 08-Jan-2003 J.Toth: the 11mm section between GTenbars and FrontFace
//                      of WheelSolid is added
//                     new 'average' materials G10FeInner, G10FeOuter are added
//                      for barrettes connecting long.bars and absorbers
// 24-Jan-2003 J.Toth: Code for Emec nonsensitive pieces is revised.
//                     details of emec support structure are added
//                       parameters for radial position of the rings
//                        are still in warm, while those for z positions are
//                        in cold.
//                     missing pieces:guides,feets
//                     Alu cone at Inner surface of Inner wheel is present
//                     new material PermaliE730 for FrontMiddleRing is added
//                       (composition to be checked)
//                     density of glue changed for that of prepreg glue of
//                       absorbers

// 21-Sep-2003        Revised to create EMEC mother volume, and to
// place all EMEC components (wheels,mech structure) within this volume.
//
//             WGS: new class EMECConstruction for GeoModel application created
//  EMECConstruction:
// 06-Oct-2003 J.T. code revised to work in the GeoModel framework
// 06-Oct-2003 J.T. EMEC Mother volume z limits corrected for cold values.
// 06-Oct-2003 J.T. crossing of volume of top indexing ring and stretchers
//                  corrected.
// 21-Jul-2005 C.S. Revised to take into account changes in G4 simulation
// 18-Sep-2006 V.N. Put back options for TB module
// 18-Sep-2006 V.N  Get materials from LarMaterialManager
// May-June 2009 AMS Get most part of geometry constants from LArWheelCalculator
//                   General code cleanup
// Feb 2022 JFB Since the appearance of the GeoUnidentifiedShape, the LArCustomShape
//              class is deprecated. We therefore decouple this package from 
//              GeoSpecialShapes, computing a few constants here which were previously
//              taken from the calculator.  
#include <string>
#include <cmath>
#include <iostream>

#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/StoredPhysVol.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "LArGeoEndcap/EMECConstruction.h"
#include "LArGeoEndcap/EMECSupportConstruction.h"
#include "GeoModelKernel/GeoUnidentifiedShape.h"


LArGeo::EMECConstruction::EMECConstruction(bool is_tb, bool has_inner, bool has_outer)
    : m_fullGeo(true), m_isTB(is_tb), m_hasInnerWheel(has_inner), m_hasOuterWheel(has_outer),
      m_innerWheelVariant("Wheel"), m_outerWheelVariant("Wheel") // "Wheel" (Meaning polycone) or "Cone" or "Slices"
{
/*
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	IMessageSvc* msgSvc;
	StatusCode status = svcLocator->service("MessageSvc", msgSvc);
	if(!status.isFailure()){
		m_msg = new MsgStream(msgSvc, "EMECConstruction");
	} else {
		throw std::runtime_error("EMECConstruction: cannot initialze message service");
	}
	(*m_msg) << MSG::DEBUG << "Parameters:"
	         << "m_fullGeo: " << m_fullGeo << std::endl
	         << "m_isTB: " << m_isTB << std::endl
             << "m_hasInnerWheel: " << m_hasInnerWheel << std::endl
             << "m_hasOuterWheel: " << m_hasOuterWheel
			 << endmsg;
*/
}

LArGeo::EMECConstruction::~EMECConstruction()
{
}

GeoFullPhysVol* LArGeo::EMECConstruction::GetEnvelope(bool bPos)
{
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  StoreGateSvc *detStore;
  if(svcLocator->service("DetectorStore", detStore, false)==StatusCode::FAILURE) {
    throw std::runtime_error("Error in EndcapCryostatConstruction, cannot access DetectorStore");
  }

  IMessageSvc* msgSvc;
  MsgStream *msg;
  StatusCode status = svcLocator->service("MessageSvc", msgSvc);
  if(!status.isFailure()){
    msg = new MsgStream(msgSvc, "EMECConstruction");
  } else {
    throw std::runtime_error("EMECConstruction: cannot access message service");
  }

  // Get GeoModelSvc and RDBAccessSvc

  IRDBAccessSvc* pAccessSvc(0);
  IGeoModelSvc* geoModelSvc(0);
  StatusCode sc = svcLocator->service("RDBAccessSvc",pAccessSvc);
  if(sc != StatusCode::SUCCESS){
    throw std::runtime_error("EMECConstruction: cannot locate RDBAccessSvc!");
  }

  sc = svcLocator->service ("GeoModelSvc",geoModelSvc);
  if(sc != StatusCode::SUCCESS){
    throw std::runtime_error("EMECConstruction: cannot locate GeoModelSvc!");
  }

  DecodeVersionKey larVersionKey(geoModelSvc, "LAr");

  // Flag for building detailed absorber. Default=false
  int multilayered_absorbers = 0;
  IRDBRecordset_ptr larSwitches = pAccessSvc->getRecordsetPtr("LArSwitches", larVersionKey.tag(), larVersionKey.node());
  if(larSwitches->size()!=0){
    if(!(*larSwitches)[0]->isFieldNull("DETAILED_ABSORBER_EC")){
      multilayered_absorbers = (*larSwitches)[0]->getInt("DETAILED_ABSORBER_EC");
    } else if(!(*larSwitches)[0]->isFieldNull("DETAILED_ABSORBER")){
      multilayered_absorbers = (*larSwitches)[0]->getInt("DETAILED_ABSORBER");
    }
  }

  if(multilayered_absorbers > 0){
    (*msg) << MSG::INFO << "multi-layered version of absorbers activated, "
           << "parameter value is " << multilayered_absorbers
           << endmsg;
  }

  const StoredMaterialManager* materialManager = nullptr;
  if(StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) return 0;

  //////////////////////////////////////////////////////////////////
  // Get materials from the manager
  //////////////////////////////////////////////////////////////////

  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if(!LAr) throw std::runtime_error("Error in EMECConstruction, std::LiquidArgon is not found.");

  const GeoMaterial* innerAbsorberMaterial = 0;
  std::string innerAbsorberMaterial_name = "LAr::EMEC_Thickabs";
  if(multilayered_absorbers > 0){
    if(multilayered_absorbers != 2){
// to be replaced with steel - finished by Adam Agocs
      innerAbsorberMaterial_name = "std::Iron";
    } else {
      innerAbsorberMaterial_name = "LAr::EMEC_shell";
    }
  }
  innerAbsorberMaterial = materialManager->getMaterial(innerAbsorberMaterial_name);
  if(!innerAbsorberMaterial){
    throw std::runtime_error(
      (innerAbsorberMaterial_name +
       " is not found for inner absorber in EMECConstruction.").c_str()
    );
  }

  const GeoMaterial* outerAbsorberMaterial = 0;
  std::string outerAbsorberMaterial_name = "LAr::EMEC_Thinabs";
  if(multilayered_absorbers > 0){
    if(multilayered_absorbers != 2){
// to be replaced with steel - finished by Adam Agocs
      outerAbsorberMaterial_name = "std::Iron";
    } else {
      outerAbsorberMaterial_name = "LAr::EMEC_shell";
    }
  }
  outerAbsorberMaterial = materialManager->getMaterial(outerAbsorberMaterial_name);
  if(!outerAbsorberMaterial){
    throw std::runtime_error(
      (outerAbsorberMaterial_name +
       " is not found for outer absorber in EMECConstruction.").c_str()
    );
  }

  const GeoMaterial *Glue = 0;
  const GeoMaterial *Lead = 0;
  if(multilayered_absorbers > 0){
// to be replaced with glue and lead - finished by Adam Agocs
    Glue = materialManager->getMaterial("LAr::Glue");
    if(!Glue) throw std::runtime_error("Error in EMECConstruction, LAr::Glue is not found.");
    Lead = materialManager->getMaterial("std::Lead");
    if(!Lead) throw std::runtime_error("Error in EMECConstruction, LAr::Lead is not found.");
  }

  const GeoMaterial* electrodeMaterial = materialManager->getMaterial("LAr::KaptonC");
  if(!electrodeMaterial) throw std::runtime_error("Error in EMECConstruction, LAr::KaptonC is not found.");

  //////////////////////////////////////////////////////////////////
  // Define geometry
  //////////////////////////////////////////////////////////////////

  // Set up strings for volume names.
  std::string baseName = "LAr::EMEC";

  double phiPosition, phiSize;

  if(m_isTB) {
    phiPosition = Gaudi::Units::halfpi*Gaudi::Units::rad;
    phiSize = M_PI*Gaudi::Units::rad / 8. + 0.065*Gaudi::Units::rad; // half-angle of inner part of module
  }
  else {
    phiPosition = M_PI*Gaudi::Units::rad;
    phiSize = M_PI*Gaudi::Units::rad; // half-angle of a full wheel
  }

  // Define the mother volume for the emec.  Everything
  // else in the emec (wheels,structure, etc.) should be
  // placed inside here.

   //double emecMotherZplan[] = {3641.*Gaudi::Units::mm,4273.*Gaudi::Units::mm};           //warm

  // 21-Jul-2005, C.S. : above line valid in warm, below is in cold.
  // The latter one should apply, othervise SupportMotherVolumes cross
  // the EMEC Mother and/or their position rel to the sensitive LAr
  // become wrong


  IRDBRecordset_ptr cryoPcons =
    pAccessSvc->getRecordsetPtr("CryoPcons", larVersionKey.tag(), larVersionKey.node());
  if(cryoPcons->size() == 0){
    cryoPcons = pAccessSvc->getRecordsetPtr("CryoPcons", "CryoPcons-EMEC-00");
  }

  //double emecMotherZplan[] = {3639.5*Gaudi::Units::mm,3639.5*Gaudi::Units::mm+630.*Gaudi::Units::mm};    //cold (J.T)
  //                                  // Zplane[0]=endg_z0*Gaudi::Units::cm-50*Gaudi::Units::mm
  //                                  // Zplane[1]=Zplane[0]+endg_dzende*Gaudi::Units::cm-2.Gaudi::Units::mm
  //double emecMotherRin[]   = { 279.*Gaudi::Units::mm, 324*Gaudi::Units::mm};	//{  302.*Gaudi::Units::mm,  302.*Gaudi::Units::mm };
  //double emecMotherRout[]  = {(2077.-7)*Gaudi::Units::mm,(2077.-7)*Gaudi::Units::mm};  	// -7 for cold
  //int lastPlaneEmec = (sizeof(emecMotherZplan) / sizeof(double));

  std::string emecMotherName = baseName + "::Mother"; //+ extension;

  GeoTransform *refSystemTransform = 0;
  double zTrans = 0.*Gaudi::Units::mm;
  double zMSTrans = 0.*Gaudi::Units::mm;

  GeoPcon* emecMotherShape = new GeoPcon(phiPosition - phiSize, 2.*phiSize);  //start phi,total phi
  for(unsigned int i = 0; i < cryoPcons->size(); ++ i){
    const IRDBRecord *currentRecord = (*cryoPcons)[i];
    if(currentRecord->getString("PCON") == "EMEC::Mother"){
      if(!refSystemTransform){
        if(m_isTB){
          zTrans = -3700.5*Gaudi::Units::mm;
          zMSTrans = zTrans;
        } else {
          zTrans = currentRecord->getDouble("ZPLANE") - 3639.5*Gaudi::Units::mm;
          zMSTrans = 0.*Gaudi::Units::mm;
        }
        refSystemTransform =  new GeoTransform(GeoTrf::TranslateZ3D(zTrans));
      }
      emecMotherShape->addPlane(currentRecord->getDouble("ZPLANE") + zMSTrans,
                                currentRecord->getDouble("RMIN"),
                                currentRecord->getDouble("RMAX"));
    }
  }

    IRDBRecordset_ptr DB_EmecGeometry =
        pAccessSvc->getRecordsetPtr("EmecGeometry", larVersionKey.tag(), larVersionKey.node());
    if(DB_EmecGeometry->size() == 0){
        DB_EmecGeometry = pAccessSvc->getRecordsetPtr("EmecGeometry", "EmecGeometry-00");
    }
    double zWheelRefPoint = (*DB_EmecGeometry)[0]->getDouble("Z0")*Gaudi::Units::cm;
    double dMechFocaltoWRP=(*DB_EmecGeometry)[0]->getDouble("Z1")*Gaudi::Units::cm;
    double LArTotalThickness = (*DB_EmecGeometry)[0]->getDouble("ETOT") *Gaudi::Units::cm;
    double halfGapBetweenWheels = (*DB_EmecGeometry)[0]->getDouble("DCRACK")*Gaudi::Units::cm;
    double rOuterCutoff = (*DB_EmecGeometry)[0]->getDouble("RLIMIT")*Gaudi::Units::cm;

    IRDBRecordset_ptr DB_EMECmn =
      pAccessSvc->getRecordsetPtr("EmecMagicNumbers", larVersionKey.tag(), larVersionKey.node());
    if(DB_EMECmn->size() == 0)
      DB_EMECmn = pAccessSvc->getRecordsetPtr("EmecMagicNumbers","EMECMagicNumbers-00");
    double reftoactive = (*DB_EMECmn)[0]->getDouble("REFTOACTIVE") *Gaudi::Units::mm;
    double activelength = (*DB_EMECmn)[0]->getDouble("ACTIVELENGTH") *Gaudi::Units::mm;
    double straightstartsection = (*DB_EMECmn)[0]->getDouble("STRAIGHTSTARTSECTION")*Gaudi::Units::mm;
    double wheelThickness=activelength+2*straightstartsection;
    double dWRPtoFrontFace = reftoactive;
    double zWheelFrontFace = dMechFocaltoWRP + dWRPtoFrontFace;
    double zWheelBackFace  = zWheelFrontFace+wheelThickness;
    IRDBRecordset_ptr DB_EMECwp=
      pAccessSvc->getRecordsetPtr("EmecWheelParameters", larVersionKey.tag(),larVersionKey.node());
    if (DB_EMECwp->size()==0) 
      DB_EMECwp=pAccessSvc->getRecordsetPtr("EmecWheelParameters", "EmecWheelParameters","EmecWheelParameters-00");
    int numberOfHalfWavesInner=2*(*DB_EMECwp)[0]->getInt("NACC")+2;
    int numberOfHalfWavesOuter=2*(*DB_EMECwp)[1]->getInt("NACC")+3;// one extra for outer wheel
    double eta_hi = (*DB_EMECwp)[0]->getDouble("ETAINT");
    double eta_mid = (*DB_EMECwp)[0]->getDouble("ETAEXT");
    double eta_low = (*DB_EMECwp)[1]->getDouble("ETAEXT");

    // NEW BLURB 
    double zMid[2];
    double rMin[2][3];
    double rMax[2][3];
    {
      double tanThetaMid   = 2. * std::exp(-eta_mid) / (1. - std::exp(-2.*eta_mid));
      double tanThetaOuter = 2. * std::exp(-eta_low) / (1. - std::exp(-2.*eta_low));
      double tanThetaInner = 2. * std::exp(-eta_hi ) / (1. - std::exp(-2.*eta_hi ));
      double inv_tanThetaOuter = (1. - std::exp(-2.*eta_low)) / (2. * std::exp(-eta_low));
      
      // Inner Wheel Outer Radius
      rMax[0][0] = zWheelFrontFace * tanThetaMid - halfGapBetweenWheels;
      rMax[0][1] = zWheelBackFace  * tanThetaMid - halfGapBetweenWheels;
      // Outer Wheel Outer Radius
      rMax[1][0] = zWheelFrontFace * tanThetaOuter;
      rMax[1][1] = rOuterCutoff;
      rMax[1][2] = rOuterCutoff;
      
      // Note that there is a 3mm gap between the outer surface of the
      // inner wheel and the inner surface of the outer wheel.
      
      // Inner Wheel Inner Radius
      rMin[0][0] = zWheelFrontFace * tanThetaInner;
      rMin[0][1] = zWheelBackFace  * tanThetaInner;
      zMid[0] = 0;
      
      // Outer Wheel Inner Radius
      rMin[1][0] = zWheelFrontFace * tanThetaMid + halfGapBetweenWheels;
      rMin[1][1] = rOuterCutoff * inv_tanThetaOuter * tanThetaMid + halfGapBetweenWheels;
      rMin[1][2] = zWheelBackFace  * tanThetaMid + halfGapBetweenWheels;
      zMid[1] = rOuterCutoff * inv_tanThetaOuter - zWheelFrontFace;
    }

    const GeoLogVol* emecMotherLogical =
        new GeoLogVol(emecMotherName, emecMotherShape, LAr);
    GeoFullPhysVol* emecMotherPhysical = new GeoFullPhysVol(emecMotherLogical);

    if(!m_isTB) baseName += bPos? "::Pos": "::Neg";

    if(m_hasInnerWheel){
        std::string innerName = baseName + "::Inner";
        std::string LArName = innerName + (m_isTB? "Module": "Wheel");

        std::vector<std::string> absorbers;
        std::vector<std::string> electrodes;

        if(m_innerWheelVariant == "Cone"){
            innerName += "Cone";
            absorbers.push_back(innerName + "::Absorber");
            electrodes.push_back(innerName + "::Electrode");
        } else if(m_innerWheelVariant == "Slices"){
            innerName += "Slice";
            char slice = 0; // To silence warnings use character datatype, limited between -128 and 127, sufficient.
            do {
                char buf[4];
                snprintf(buf, 4, "%02d", slice);

		std::string a = innerName + buf + "::Absorber";
		std::string e = innerName + buf + "::Electrode";
                absorbers.push_back(a);
                electrodes.push_back(e);
                slice ++;
            } while(
                numberOfHalfWavesInner > slice
             && slice < 100 // slice number limited by two digits
            );
            if(slice >= 100){
                (*msg) << MSG::ERROR << "too many LArWheel slices, something"
                       << " goes wrong in EMECConstruction" << endmsg;
            }
            innerName += "s";
        } else { // it is a Polycone
            innerName += (m_isTB? "Module": "Wheel");
            absorbers.push_back(innerName + "::Absorber");
            electrodes.push_back(innerName + "::Electrode");
        }
        (*msg) << MSG::INFO << "activating " << innerName << endmsg;
        (*msg) << MSG::DEBUG << absorbers.size() << " absorber, "
                             << electrodes.size() << " electrode shapes created"
                             << endmsg;


        double zBack = wheelThickness;
        GeoPcon* innerShape = new GeoPcon(phiPosition - phiSize, 2.*phiSize);
        innerShape->addPlane(0.*Gaudi::Units::mm, rMin[0][0], rMax[0][0]);
        innerShape->addPlane(zBack              , rMin[0][1], rMax[0][1]);

        GeoLogVol*  innerLogical  = new GeoLogVol (LArName,innerShape, LAr);
        GeoFullPhysVol* fullPV = new GeoFullPhysVol(innerLogical);

        emecMotherPhysical->add(new GeoIdentifierTag(1));
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zWheelRefPoint+dWRPtoFrontFace)));
        emecMotherPhysical->add(fullPV);

        StoredPhysVol *sPhysVol = new StoredPhysVol(fullPV);
        StatusCode status=detStore->record(sPhysVol, bPos? "EMEC_INNER_WHEEL_POS": "EMEC_INNER_WHEEL_NEG");
        if(!status.isSuccess()){
            throw std::runtime_error(bPos? "Cannot store EMEC_INNER_WHEEL_POS": "Cannot store EMEC_INNER_WHEEL_NEG");
        }

        place_custom_solids(
            fullPV, absorbers, electrodes, multilayered_absorbers,
            innerAbsorberMaterial, electrodeMaterial, Glue, Lead
        );
    } // if(m_hasInnerWheel)

    if(m_hasOuterWheel){
        std::string outerName = baseName + "::Outer";
        const std::string LArName = outerName + (m_isTB? "Module": "Wheel");

        std::vector<std::string> absorbers;
        std::vector<std::string> electrodes;

        if(m_outerWheelVariant == "Cone"){
            absorbers.push_back(outerName + "FrontCone::Absorber");
            absorbers.push_back(outerName + "BackCone::Absorber");
            electrodes.push_back(outerName + "FrontCone::Electrode");
            electrodes.push_back(outerName + "BackCone::Electrode");
            outerName += "Cones";
        } else if(m_outerWheelVariant == "Slices"){
            outerName += "Slice";
            char slice = 0; // To silence warnings use character datatype, limited between -128 and 127, sufficient.
            do {
                char buf[4];
                snprintf(buf, 4, "%02d", slice);
		std::string a = outerName + buf + "::Absorber";
		std::string e = outerName + buf + "::Electrode";
                absorbers.push_back(a);
                electrodes.push_back(e);
                slice ++;
            } while( // outer wheel has an extra slice
                numberOfHalfWavesOuter > slice
             && slice < 100 // slice number limited by two digits
            );
            if(slice >= 100){
                (*msg) << MSG::ERROR << "too many LArWheel slices, something"
                       << " goes wrong in EMECConstruction" << endmsg;
            }
            outerName += "s";
        } else { // it is Polycone
            outerName += (m_isTB? "Module": "Wheel");
            absorbers.push_back(outerName + "::Absorber");
            electrodes.push_back(outerName + "::Electrode");
        }
        (*msg) << MSG::INFO << "activating " << outerName << endmsg;
        (*msg) << MSG::DEBUG << absorbers.size() << " absorber, "
                             << electrodes.size() << " electrode shapes created"
                             << endmsg;

        double zBack = wheelThickness;
        GeoPcon *outerShape = new GeoPcon(phiPosition - phiSize, 2.*phiSize);
        outerShape->addPlane(0.*Gaudi::Units::mm, rMin[1][0], rMax[1][0]);
        outerShape->addPlane(zMid[1]            , rMin[1][1], rMax[1][1]);
        outerShape->addPlane(zBack              , rMin[1][2], rMax[1][2]);

        GeoLogVol *outerLogical = new GeoLogVol(LArName, outerShape, LAr);
        GeoFullPhysVol *fullPV = new GeoFullPhysVol(outerLogical);

        emecMotherPhysical->add(new GeoIdentifierTag(1));
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zWheelRefPoint+dWRPtoFrontFace)));
        emecMotherPhysical->add(fullPV);

        StoredPhysVol *sPhysVol = new StoredPhysVol(fullPV);
        StatusCode status = detStore->record(sPhysVol, bPos? "EMEC_OUTER_WHEEL_POS": "EMEC_OUTER_WHEEL_NEG");
        if(!status.isSuccess()){
            throw std::runtime_error(bPos? "Cannot store EMEC_OUTER_WHEEL_POS": "Cannot store EMEC_OUTER_WHEEL_NEG");
        }

        place_custom_solids(
            fullPV, absorbers, electrodes, multilayered_absorbers,
            outerAbsorberMaterial, electrodeMaterial, Glue, Lead
        );

    } // if(m_hasOuterWheel)

// ***********************************
//Description of nonsensitive pieces:*
// ***********************************

    if(m_fullGeo) {
        double z0;

        // Volumes for FRONT and BACK support structures
        EMECSupportConstruction::type_t
            FrontSupp = EMECSupportConstruction::Front,
            BackSupp = EMECSupportConstruction::Back;
        if(m_hasInnerWheel && !m_hasOuterWheel){
            FrontSupp = EMECSupportConstruction::FrontInner;
            BackSupp = EMECSupportConstruction::BackInner;
        } else if(m_hasOuterWheel && !m_hasInnerWheel){
            FrontSupp = EMECSupportConstruction::FrontOuter;
            BackSupp = EMECSupportConstruction::BackOuter;
        }

        IRDBRecordset_ptr DMpcons =
            pAccessSvc->getRecordsetPtr("EmecDMPCons", larVersionKey.tag(), larVersionKey.node());
        if(DMpcons->size() == 0){
            DMpcons = pAccessSvc->getRecordsetPtr("EmecDMPCons", "EmecDMPCons-00");
        }

        double front_shift = 0.*Gaudi::Units::mm;
        double back_shift = 0.*Gaudi::Units::mm;
        try {
            for(unsigned int i = 0; i < DMpcons->size(); ++ i){
                std::string object = (*DMpcons)[i]->getString("PCONNAME");
                if(object == "FrontSupportMother"){
                    int zplane = (*DMpcons)[i]->getInt("NZPLANE");
                    if(zplane == 0) front_shift += (*DMpcons)[i]->getDouble("ZPOS")*Gaudi::Units::mm;
                    else if(zplane == 1) front_shift -= (*DMpcons)[i]->getDouble("ZPOS")*Gaudi::Units::mm;
                    else continue;
                } else if(object == "BackSupportMother"){
                    int zplane = (*DMpcons)[i]->getInt("NZPLANE");
                    if(zplane == 0) back_shift -= 0.;//(*DMpcons)[i]->getDouble("ZPOS")*Gaudi::Units::mm;
                    else if(zplane == 1) back_shift += (*DMpcons)[i]->getDouble("ZPOS")*Gaudi::Units::mm;
                    else continue;
                }
            }
            front_shift += reftoactive;
            back_shift += LArTotalThickness - reftoactive;
        }
        catch (...){
            front_shift = -50.*Gaudi::Units::mm; // start of EMEC envelop in the cryo.(length of env=630.)
            back_shift = 580.*Gaudi::Units::mm;
            std::cout << "EMECConstruction: WARNING: cannot get front|back_shift from DB"
                    << std::endl;
        }
//std::cout << "EMECConstruction : " << front_shift << " " << back_shift << std::endl;
        z0 = zWheelRefPoint + front_shift;
        EMECSupportConstruction *fsc = 0;
        if(m_isTB) fsc = new EMECSupportConstruction(FrontSupp, bPos, true, "LAr::EMEC::", Gaudi::Units::halfpi*Gaudi::Units::rad);
        else fsc = new EMECSupportConstruction(FrontSupp, bPos);
        GeoPhysVol* physicalFSM = fsc->GetEnvelope();
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(physicalFSM);
        delete fsc;

        z0 = zWheelRefPoint + back_shift; // end of EMEC envelop in the cryo.
        EMECSupportConstruction *bsc = 0;
        if(m_isTB) bsc = new EMECSupportConstruction(BackSupp, bPos, true, "LAr::EMEC::", Gaudi::Units::halfpi*Gaudi::Units::rad);
        else bsc = new EMECSupportConstruction(BackSupp, bPos);
        GeoPhysVol *physicalBSM = bsc->GetEnvelope();
        GeoTrf::Transform3D rotBSM(GeoTrf::RotateX3D(-M_PI));
        if(m_isTB) rotBSM = GeoTrf::RotateZ3D(M_PI)*rotBSM;
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(0., 0., z0)*rotBSM)));
        emecMotherPhysical->add(physicalBSM);
        delete bsc;

        z0 = zWheelRefPoint + LArTotalThickness * 0.5; //dist. to middle of sens vol. along z  from WRP
        EMECSupportConstruction *osc = 0;
        if(m_isTB) osc = new EMECSupportConstruction(EMECSupportConstruction::Outer, bPos, true, "LAr::EMEC::", Gaudi::Units::halfpi*Gaudi::Units::rad);
        else osc = new EMECSupportConstruction(EMECSupportConstruction::Outer, bPos);
        GeoPhysVol *physicalOSM = osc->GetEnvelope();
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
        emecMotherPhysical->add(physicalOSM);
        delete osc;

        z0 = zWheelRefPoint + LArTotalThickness * 0.5;
        EMECSupportConstruction *isc = 0;
        if(m_isTB) isc = new EMECSupportConstruction(EMECSupportConstruction::Inner, bPos, true, "LAr::EMEC::", Gaudi::Units::halfpi*Gaudi::Units::rad);
        else isc = new EMECSupportConstruction(EMECSupportConstruction::Inner, bPos);
        GeoPhysVol *physicalISM = isc->GetEnvelope();
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
        emecMotherPhysical->add(physicalISM);
        delete isc;

        z0 = zWheelRefPoint + LArTotalThickness * 0.5;
        EMECSupportConstruction *msc = 0;
        if(m_isTB) msc = new EMECSupportConstruction(EMECSupportConstruction::Middle, bPos, true, "LAr::EMEC::", Gaudi::Units::halfpi*Gaudi::Units::rad);
        else msc = new EMECSupportConstruction(EMECSupportConstruction::Middle, bPos);
        GeoPhysVol *physicalMSM = msc->GetEnvelope();
        emecMotherPhysical->add(refSystemTransform);
        emecMotherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
        emecMotherPhysical->add(physicalMSM);
        delete msc;
    }

    return emecMotherPhysical;
}

void LArGeo::EMECConstruction::setFullGeo(bool flag)
{
  m_fullGeo = flag;
}

// Place the custom accordion volumes into the liquid argon
void LArGeo::EMECConstruction::place_custom_solids(
    GeoFullPhysVol *fullPV,
    std::vector<std::string> &absorbers,
    std::vector<std::string> &electrodes,
    int multilayered_absorbers,
    const GeoMaterial *Absorber, const GeoMaterial *Electrode,
    const GeoMaterial *Glue, const GeoMaterial *Lead
)
{

  for(auto name: absorbers){
    GeoLogVol* log_volume = new GeoLogVol(name, new GeoUnidentifiedShape("LArCustomShape", name) , Absorber);
    GeoPhysVol* phys_volume = new GeoPhysVol(log_volume);
    fullPV->add(new GeoIdentifierTag(1));
    fullPV->add(new GeoTransform(GeoTrf::Transform3D::Identity()));
    fullPV->add(phys_volume);
    if(multilayered_absorbers > 0){
      GeoPhysVol* glue_phys = phys_volume;
      std::string lead_name = name;
      size_t repl = lead_name.find("Absorber");
      if(std::string::npos != repl) lead_name.replace(repl, 8, "Lead");
      else throw std::runtime_error(lead_name + ": cannot find 'Absorber'");
      if(multilayered_absorbers != 2){
	std::string glue_name = name;
	glue_name.replace(repl, 8, "Glue");
	GeoLogVol* glue_log = new GeoLogVol(glue_name, new GeoUnidentifiedShape("LArCustomShape",glue_name), Glue);
	glue_phys = new GeoPhysVol(glue_log);
	phys_volume->add(new GeoIdentifierTag(1));
	phys_volume->add(new GeoTransform(GeoTrf::Transform3D::Identity()));
	phys_volume->add(glue_phys);
      }
      GeoLogVol *lead_log = new GeoLogVol(lead_name, new GeoUnidentifiedShape("LArCustomShape",lead_name), Lead);
      GeoPhysVol *lead_phys  = new GeoPhysVol(lead_log);
      glue_phys->add(new GeoIdentifierTag(1));
      glue_phys->add(new GeoTransform(GeoTrf::Transform3D::Identity()));
      glue_phys->add(lead_phys);
    }
  }

    for(auto name: electrodes){
      GeoLogVol* log_volume = new GeoLogVol(name, new GeoUnidentifiedShape("LArCustomShape",name), Electrode);
      GeoPhysVol* phys_volume = new GeoPhysVol(log_volume);
      fullPV->add(new GeoIdentifierTag(1));
      fullPV->add(new GeoTransform(GeoTrf::Transform3D::Identity()));
      fullPV->add(phys_volume);
    }
}
