/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


// GeoModel description of LAr EM Barrel calorimeter
// translated from G4 code (G.Parrour) by G.Unal
// translated then from placements to parameterizations
//  by J. Boudreau.
// made consistent with last hardcoded G4 version by G.Unal, July 2005

// creation: August 2003

#include "LArGeoBarrel/BarrelConstruction.h"

#include "LArGeoCode/VDetectorParameters.h"

#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoXF.h"

// volumes used:  Pcon, Tubs, Cons, Box, Trap 
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelUtilities/StoredPhysVol.h"
// For transforms:
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
// For functions:
#include "GeoGenericFunctions/Abs.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "GeoGenericFunctions/Sqrt.h"
#include "GeoGenericFunctions/ATan.h"
#include "GeoGenericFunctions/Rectangular.h"
#include "GeoGenericFunctions/Mod.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/FixedConstant.h"
// For units:
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"


// Some additional objects to store for future reference:
#include "LArReadoutGeometry/GeoStraightAccSection.h"

#include <string>
#include <cmath>
#include <cfloat>

#define BUILD_FRONT_ELECTRONICS
#define BUILD_HIGHETA_ELECTRONICS
#define BUILD_FRONT_G10
#define BUILD_BACK_G10
#define BUILD_ACCORDION_PLATES
#define BUILD_FRONT_STEEL

// #define DEBUGGEO

// GU 29-Jul-2005
//   *Add summing board effective layer
//   *Change material for inner and outer G10 ring -> G10_bar
//   *Put small iron tips in the front G10 pieces before the absorbers
//   *Fix z lenght as in latest hardcoded G4 geometry
//   *if no sagging, set deltay=0 => almost all code sagging/no_sagging becomes the same
//      and much less if(m_A_SAGGING) are needed
//   *small modifications in test beam case to agree with last hardcoded version

// JFB Notes:
// Parameterizing most volumes.
// Wrote the motherboard positioning using function composition.
// Write the cable positioning by inheriting a positioning function.
// All log messages are at the end. They are sent to msg stream.
// Stack logical volume was set smartless in G4, still no solution to that.
// Please review the number of electrodes in the test beam case. Should be
//  one less than number of cells. 

// Redid the structure a little bit.  There are two, full, physical volumes one
// for the positive half-barrel and one for the negative half-barrel but they
// both share the same innards... don't want to duplicate this, do we?

//===================constructor

LArGeo::BarrelConstruction::BarrelConstruction(bool fullGeo)
  :m_parameters(LArGeo::VDetectorParameters::GetInstance()),
   m_A_SAGGING(false),
   m_NVISLIM(-1),
   m_ecamPhysicalPos(NULL),
   m_ecamPhysicalNeg(NULL),
   m_fullGeo(fullGeo)
{
}

//===================destructor

LArGeo::BarrelConstruction::~BarrelConstruction()
{
}

//================== get envelope

GeoFullPhysVol *LArGeo::BarrelConstruction::GetPositiveEnvelope() {
  if (!m_ecamPhysicalPos) MakeEnvelope();
  return m_ecamPhysicalPos;
}

GeoFullPhysVol *LArGeo::BarrelConstruction::GetNegativeEnvelope() {
  if (!m_ecamPhysicalNeg) MakeEnvelope();
  return m_ecamPhysicalNeg;
}

void LArGeo::BarrelConstruction::MakeEnvelope()
{
  ISvcLocator *svcLocator = Gaudi::svcLocator();
  IMessageSvc * msgSvc;
  if (svcLocator->service("MessageSvc", msgSvc, true )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in BarrelConstruction, cannot access MessageSvc");
  }
  MsgStream log(msgSvc, "BarrelConstruction"); 
  

  log << MSG::DEBUG;
  
  log  << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         Start of Barrel EM GeoModel definition   +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  log << " " << std::endl;
  log << "Sagging in geometry " << m_A_SAGGING << std::endl;
  log << " " << std::endl;
  log << " " << endmsg;

  if(msgSvc->outputLevel("BarrelConstruction") <= MSG::DEBUG) {
    printParams();
  }

  bool doDetailedAbsorberStraight = false;
  bool doDetailedAbsorberFold = false;

  IGeoModelSvc *geoModel;
  IRDBAccessSvc* rdbAccess;
  if(svcLocator->service ("GeoModelSvc",geoModel) == StatusCode::FAILURE)
     throw std::runtime_error("Error in BarrelConstruction, cannot access GeoModelSvc"); 
  if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
     throw std::runtime_error("Error in BarrelConstruction, cannot access RDBAccessSvc");
  DecodeVersionKey larVersionKey(geoModel, "LAr"); 

  log << MSG::INFO <<  "Getting primary numbers for " << larVersionKey.node() << ", " << larVersionKey.tag() << endmsg;

  IRDBRecordset_ptr switchSet = rdbAccess->getRecordsetPtr("LArSwitches", larVersionKey.tag(), larVersionKey.node());
  if ((*switchSet).size() !=0) {
     const IRDBRecord    *switches   = (*switchSet)[0];
     if (!switches->isFieldNull("DETAILED_ABSORBER")) {
          if (switches->getInt("DETAILED_ABSORBER") !=0) {
            log << MSG::DEBUG  << " DETAILED_ABSORBER is 1 " << endmsg;
            doDetailedAbsorberStraight = true;
            doDetailedAbsorberFold =  true;
          } else {
            log << MSG::DEBUG  << " DETAILED_ABSORBER is 0 " << endmsg;
          }
     } else {
       log << MSG::DEBUG  << " no DETAILED_ABSORBER structure in DB " << endmsg;
     }
  } else {
    log << MSG::WARNING  << " LArSwitches structure not found " << endmsg;
  }

  log << MSG::INFO << "  Makes detailed absorber sandwich  ? " << doDetailedAbsorberStraight << " " << doDetailedAbsorberFold << endmsg;
  log << MSG::INFO << "  Use sagging in geometry  ? " << m_A_SAGGING << endmsg;


  GeoGenfun::Cos  Cos;
  GeoGenfun::Sin  Sin;
  GeoGenfun::Sqrt Sqrt;
  GeoGenfun::ATan ATan;

  double twopi64 = Gaudi::Units::pi/32.;
  double twopi32 = 2.*twopi64;  


  StoreGateSvc *detStore;
  if (svcLocator->service("DetectorStore", detStore, false )==StatusCode::FAILURE) {
    throw std::runtime_error("Error in LArDetectorFactory, cannot access DetectorStore");
  }
  

  const StoredMaterialManager* materialManager = nullptr;
  if (StatusCode::SUCCESS != detStore->retrieve(materialManager, std::string("MATERIALS"))) {
    throw std::runtime_error("Error in BarrelConstruction, stored MaterialManager is not found.");
  }
  
  const GeoMaterial *Iron  = materialManager->getMaterial("std::Iron");
  if (!Iron) throw std::runtime_error("Error in BarrelConstruction, std::Iron is not found.");
  
  const GeoMaterial *LAr  = materialManager->getMaterial("std::LiquidArgon");
  if (!LAr) throw std::runtime_error("Error in BarrelConstruction, std::LiquidArgon is not found.");

  const GeoMaterial *Lead = materialManager->getMaterial("std::Lead");
  if (!Lead) throw std::runtime_error("Error in BarrelConstruction, std::Lead is not found.");
  
  const GeoMaterial *G10_bar  = materialManager->getMaterial("LAr::G10_bar");
  if (!G10_bar) throw std::runtime_error("Error in BarrelConstruction, LAr::G10_bar is not found.");
  
  const GeoMaterial *Moth_elect  = materialManager->getMaterial("LAr::MBoards");
  if (!Moth_elect) throw std::runtime_error("Error in BarrelConstruction, LAr::MBoards is not found.");
  
  const GeoMaterial *Cable_elect  = materialManager->getMaterial("LAr::Cables");
  if (!Cable_elect) throw std::runtime_error("Error in BarrelConstruction, LAr::Cables is not found.");
  
  const GeoMaterial *Thin_abs  = materialManager->getMaterial("LAr::Thinabs");
  if (!Thin_abs) throw std::runtime_error("Error in BarrelConstruction, LAr::Thinabs is not found.");
  
  const GeoMaterial *Thick_abs  = materialManager->getMaterial("LAr::Thickabs");
  if (!Thick_abs) throw std::runtime_error("Error in BarrelConstruction, LAr::Thickabs is not found.");
  
  const GeoMaterial *Kapton_Cu  = materialManager->getMaterial("LAr::KaptonC");
  if (!Kapton_Cu) throw std::runtime_error("Error in BarrelConstruction, LAr::KaptonC is not found.");

  const GeoMaterial *Sumb       = materialManager->getMaterial("LAr::SBoard");
  if (!Sumb) throw std::runtime_error("Error in BarrelConstruction, LAr::SBoard is not found.");

  const GeoMaterial *Glue  = materialManager->getMaterial("LAr::Glue");
  if (!Glue) throw std::runtime_error("Error in BarrelConstruction, LAr::Glue is not found.");

  const GeoElement *Pb = materialManager->getElement("Lead");
  if (!Pb)  throw std::runtime_error("Error in BarrelConstruction,  element lead not found ");

  const GeoElement *Fe = materialManager->getElement("Iron");
  if (!Fe)  throw std::runtime_error("Error in BarrelConstruction,  element Fe not found ");


  double contract = m_parameters->GetValue("LArEMBAbsorberContraction");

  double density_lead = Lead->getDensity()/(contract*contract*contract);
  GeoMaterial* myLead = new GeoMaterial("myLead",density_lead);
  myLead->add(Pb,1.0);
  myLead->lock();

  double density_iron = Iron->getDensity()/(contract*contract*contract);
  GeoMaterial* myIron = new GeoMaterial("myIron",density_iron);
  myIron->add(Fe,1.0);
  myIron->lock();


  
#ifdef DEBUGGEO
   std::cout << "*** BarrelConstruction: List of Materials, density,X0,Lambda " << std::endl;
   std::cout << " Iron       " << Iron->getDensity() << " " 
                               << Iron->getRadLength() << " " 
                               << Iron->getIntLength() << std::endl;
   std::cout << " LAR        " << LAr->getDensity() << " " 
                               << LAr->getRadLength() << " " 
                               << LAr->getIntLength()  << std::endl;
   std::cout << " G10_bar    " << G10_bar->getDensity() << " " 
                               << G10_bar->getRadLength() << " "
                               << G10_bar->getIntLength() << std::endl,
   std::cout << " Moth_elect " << Moth_elect->getDensity() << " " 
                               << Moth_elect->getRadLength() << " "
                               << Moth_elect->getIntLength() << std::endl;
   std::cout << " Cable      " << Cable_elect->getDensity() << " " 
                               << Cable_elect->getRadLength() << " "
                               << Cable_elect->getIntLength() << std::endl;
   std::cout << " Sumb       " << Sumb->getDensity() << " "  
                               << Sumb->getRadLength() << " " 
                               << Sumb->getIntLength() << std::endl;
   std::cout << " Thin_abs   " << Thin_abs->getDensity() << " " 
                               << Thin_abs->getRadLength() << " "
                               << Thin_abs->getIntLength() << std::endl;
   std::cout << " Thick_abs  " << Thick_abs->getDensity() << " " 
                               << Thick_abs->getRadLength() << " "
                               << Thick_abs->getIntLength() << std::endl;
   std::cout << " Kapton_Cu  " << Kapton_Cu->getDensity() << " " 
                               << Kapton_Cu->getRadLength() << " "
                               << Kapton_Cu->getIntLength() << std::endl;
#endif
  std::string baseName = "LAr::EMB::";
  
  
  // -------------------------------------------------------------
  // start Detector geometry
  //--------------------------------------------------------------
  
  double Moth_Z_min = m_parameters->GetValue("LArEMBMotherZmin");
  double Moth_Z_max = m_parameters->GetValue("LArEMBMotherZmax");

  // radius and phi ranges of EM barrel

  double Moth_inner_radius = m_parameters->GetValue("LArEMBMotherRmin");
  double Moth_outer_radius = m_parameters->GetValue("LArEMBMotherRmax");

  double Moth_Phi_Min = 0.;
  double Moth_Phi_Max = m_parameters->GetValue("LArEMBphiMaxBarrel")*Gaudi::Units::deg;

#ifdef DEBUGGEO
  std::cout << " *** Mother volume (Ecam) parameters " << std::endl;
  std::cout << "  Rmin/Rmax " << Moth_inner_radius << " " << Moth_outer_radius << std::endl;
  std::cout << "  Zmin/Zmax " << Moth_Z_min << " " << Moth_Z_max << std::endl;
  std::cout << "  phi1,Dphi (Gaudi::Units::deg)" << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
#endif

  // number of zigs for accordion
  int    Nbrt = (int) ( m_parameters->GetValue("LArEMBnoOFAccZigs") );  // =14

  // Z coordinates for half barrel in 
  double Bar_Z_min = Moth_Z_min;
  double Bar_Z_max = Moth_Z_max;

  // Radius of the first fold
//GU -> to be consistent with hard coded geometry
  double Rhocen1 = m_parameters->GetValue("LArEMBRadiusAtCurvature",0);

  // R and Z coordinates of ETA_cuts.
  double Bar_Eta_cut = (double) (m_parameters->GetValue("LArEMBMaxEtaAcceptance"));
#ifdef DEBUGGEO
  std::cout << " Bar_Eta_cut " << Bar_Eta_cut << std::endl;
#endif

  double Bar_Z_cut,  Bar_Rcmx ;
// z max at low radius
  Bar_Z_cut = (double) (m_parameters->GetValue("LArEMBMotherZmin"))
            + (double)  (m_parameters->GetValue("LArEMBG10FrontDeltaZ"));
// minimal radius at Bar_z_max
  Bar_Rcmx  = (double) (m_parameters->GetValue("LArEMBRminHighZ"));

  double Zp0 = 0.;

  // Half-length of this Half barrel
  double Zhalf = (Bar_Z_max-Bar_Z_min)/2.;
  // Half-length of the half barrel up to Zcut
  double Zhalfc = (Bar_Z_cut-Bar_Z_min)/2.;


  // Accordion shape parameters:
  // rho phi of curvature centers and delta's
  // IN 2D_transverse LOCAL framework of a layer and symetry axis as X_axis
  // delta's are GEOMETRICAL angles of straight parts w.r. the Y_axis )

  double Rhocen[15], Phicen[15], Delta[15], deltay[15], deltax[15], TetaTrans[15];
  for (int idat=0; idat<15; idat++)
    {
      Rhocen[idat] = 
	(double) (m_parameters->GetValue("LArEMBRadiusAtCurvature",idat));
      Phicen[idat] =
	(double) (m_parameters->GetValue("LArEMBPhiAtCurvature",idat));
      Delta[idat]  =
	(double) (m_parameters->GetValue("LArEMBDeltaZigAngle",idat));
      if(idat == 14) Delta[idat]  = (90.0) * Gaudi::Units::deg; 

  // Maximum SAGGING displacement for each of the fifteen folds in Gaudi::Units::mm
  // (should be 0.0, 0.17, 0.30, 0.63, 0.78, 1.06, 1.09, 1.21, 1.07, 1.03, 0.74, 0.61, 0.27, 0.20, 0.0)
//GUtmp sagging amplied by 10
      if (m_A_SAGGING)  {
         deltay[idat] = 
 	   (double) (m_parameters->GetValue("LArEMBSaggingAmplitude",idat));
         deltax[idat] = 
           (double) (m_parameters->GetValue("LArEMBSaggingAmplitude2",idat));
      }
      else {
         deltay[idat]=0.;
         deltax[idat]=0.;
      }
  
  // eta of lead transition
       double etaTrans = (double) (m_parameters->GetValue("LArEMBEtaTrans",idat));
       TetaTrans[idat] = 2.*atan(exp(-etaTrans));

// #ifdef DEBUGGEO
       log << MSG::DEBUG << "idat " << idat << " Rhocen/Phice/Delta/deltay/deltax/etatrans "
	   << Rhocen[idat] << " " << Phicen[idat]*(1./Gaudi::Units::deg) << " "
	   << Delta[idat]*(1./Gaudi::Units::deg) << " " << deltay[idat] << " " << deltax[idat]
	   << " " << etaTrans << endmsg;
// #endif


    }

// parity of accordion waves
  int checkParity=0;   // for case where first absorber wave goes up
  if (Phicen[0]<0.) checkParity=1;  // case where first absorber wave goes down


  int Ncell      = (int) (m_parameters->GetValue("LArEMBnoOFPhysPhiCell"));
  int Nabsorber  = (Ncell==64)  ? Ncell + 1 : Ncell;
  int Nelectrode = Ncell;

  // If the user has specifically limited the number of cells (e.g. for visualization)
  // then only create this number....
  if (m_NVISLIM > 0) Nabsorber  = m_NVISLIM;
  if (m_NVISLIM > 0) Nelectrode = m_NVISLIM;
  if (m_NVISLIM > 0) {
    log << MSG::WARNING;
    log << "================LAr BarrelConstruction===========================" << std::endl;
    log << "===YOU ARE RUNNING WITH A LIMITED SLICE OF BARREL ACCORDEON   ===" << std::endl;
    log << "===THIS OPTION IS FOR VISUALIZATION OR OTHER TESTING, ONLY!!  ===" << std::endl;
    log << "===AND IF THIS IS NOT YOUR INTENTION PLEASE BE SURE TO SET    ===" << std::endl;
    log << "===THE FLAG GeoModelSvc.LArDetectorTool.BarrelCellVisLimit=-1 ===" << std::endl;
    log << "===Remember there are no defaults in Athena. Thank you.       ===" << std::endl;
    log << "=================================================================" << std::endl;
    log << MSG::INFO;
  }


// z of end of G10 ring
  double z1   = m_parameters->GetValue("LArEMBG10FrontDeltaZ")+ Moth_Z_min;
// radius of end of G10 ring
  const double r1    = Moth_inner_radius
    + m_parameters->GetValue("LArEMBInnerElectronics")
    + m_parameters->GetValue("LArEMBG10SupportBarsIn");
// minimum radius at end of volume
  const double r2    =  m_parameters->GetValue("LArEMBRminHighZ");
  const double inv_r2_r1 = 1. / (r2-r1);
  
// max z of Stac (@ r = Rhocen[0] )
  double zmax1_Stac = z1 + (Rhocen[0]-r1)*(Moth_Z_max-z1)*inv_r2_r1;


  // Construct the container volume (Accordion plates, G10 bars, etc...)
  // for the LAr in the Barrel
  // That is the volume ECAM
  // Define the volume into which we'll place all other EMB elements.
  // ==GU 11/06/2003
  //  in test beam case need to have ECAM between -1.555 and size 24.055 degrees
  //  (i.e. a little bit wider than one calorimeter module)
  // (we need to have ECAM volume sligthly wider than STAC to avoid G4 tracking becoming
  //  very confused at the common surface between ECAM and STAC)
  //-----------------ECAM---------------------------------------------------------//
  {                                                                               //
    double Moth_Phi_Min2 = (Ncell == 64) ? -1.555*Gaudi::Units::deg : 0.;                       //
    double Moth_Phi_Max2 = (Ncell == 64) ? 25.61*Gaudi::Units::deg  : 2*M_PI;                   //
                                                                                  //
    double safety_rhocen1 = 0.040*Gaudi::Units::mm;                                             //
    double Zplan[] = {Bar_Z_min-Zp0,Bar_Z_cut-Zp0,Bar_Z_max-Zp0};                 //
    double Riacc[] = {Moth_inner_radius,Moth_inner_radius, Rhocen1-safety_rhocen1};  //
    double Roacc[] = {Moth_outer_radius,Moth_outer_radius,Moth_outer_radius};     //
                                                                                  //
#ifdef DEBUGGEO
     std::cout << " *** Parameters for ECAM Pcon volume " << std::endl;
     std::cout << "  Zplan " << Zplan[0] << " " << Zplan[1] << " " << Zplan[2] << std::endl;
     std::cout << "  Rin   " << Riacc[0] << " " << Riacc[1] << " " << Riacc[2] << std::endl;
     std::cout << "  Rout  " << Roacc[0] << " " << Roacc[1] << " " << Roacc[2] << std::endl;
     std::cout << " PhiMin,Dphi " << Moth_Phi_Min2/Gaudi::Units::deg << " " << Moth_Phi_Max2/Gaudi::Units::deg << std::endl;
#endif
    int ecamArraySize = sizeof(Zplan) / sizeof(double);                           //
    std::string name = baseName + "ECAM";                                         //
    GeoPcon* pCon = new GeoPcon(Moth_Phi_Min2,    // starting phi                 //
				Moth_Phi_Max2);   // moth_phi_max2=Dphi           //
    //                                                                            //
    for (int i=0; i< ecamArraySize; i++)                                          //
      {                                                                           //
	pCon->addPlane(Zplan[i],Riacc[i],Roacc[i]);                               //
      }                                                                           //
    const GeoLogVol* logVol = new GeoLogVol(name,pCon,LAr);                       //
    m_ecamPhysicalPos = new GeoFullPhysVol(logVol);                               //
    m_ecamPhysicalNeg = new GeoFullPhysVol(logVol);                               //
                                                                                  //
    // store FullPhysicalVolumes in the manager                                   //
    //_manager->set_volLink(CaloSubdetNames::EMB_POS,m_ecamPhysicalPos);          //
    //_manager->set_volLink(CaloSubdetNames::EMB_NEG,m_ecamPhysicalNeg);          //
    {
      StoredPhysVol *sPhysVol = new StoredPhysVol(m_ecamPhysicalPos);
      StatusCode status=detStore->record(sPhysVol,"EMB_POS");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store EMB_POS");
    }
    {
      StoredPhysVol *sPhysVol = new StoredPhysVol(m_ecamPhysicalNeg);
      StatusCode status=detStore->record(sPhysVol,"EMB_NEG");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store EMB_NEG");
    }
  }                                                                               //
  //------------------------------------------------------------------------------//

  if(!m_fullGeo) return;

  //
  // Front and Back ELECTRONICS in ECAM
  //
#ifdef BUILD_FRONT_ELECTRONICS

  //   Front Electronics are in a TUBE


  //-----------------TELF---------------------------------------------------------//
  GeoPhysVol *Elnicsf_phys=NULL;
  double Xel1f;
  {
    // WARNING : this "hard_coded" 0.010*Gaudi::Units::mm is a "security" to avoid
    //           fake "overlapping" diagnostics with "DAVID"
    Xel1f = m_parameters->GetValue("LArEMBInnerElectronics");       // 23.*Gaudi::Units::mm
    double DeltaZ = Zhalfc;
    double Zpos = Zhalfc+Bar_Z_min; 
    double Rmini =  Moth_inner_radius + 0.010*Gaudi::Units::mm;
    double Rmaxi = Rmini+Xel1f - 0.010*Gaudi::Units::mm;
    std::string name = baseName + "TELF";
#ifdef DEBUGGEO
    std::cout << " *** parameters for TELF tubs " << std::endl;
    std::cout << " DeltaZ      " << DeltaZ << std::endl;
    std::cout << " Rmin/Rmax   " << Rmini << " " << Rmaxi << std::endl,
    std::cout << " PhiMin,Dphi " << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
    std::cout << " Zpos in ECAM " << Zpos << std::endl;
#endif
    GeoTubs* tubs = new GeoTubs(Rmini,          // rmin
				Rmaxi,          // rmax
				DeltaZ,         // deltaZ/2.
				Moth_Phi_Min,   // phi min
				Moth_Phi_Max);  //  deltaphi
    const GeoLogVol* logVol  = new GeoLogVol(name,tubs,LAr);
    Elnicsf_phys = new GeoPhysVol(logVol);
    m_ecamPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));
    m_ecamPhysicalPos->add(Elnicsf_phys);
    m_ecamPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));
    m_ecamPhysicalNeg->add(Elnicsf_phys);
  }
  //------------------------------------------------------------------------------//


  // GU 28-07-2005
  //------- effective Copper + LAr mixture to accoung for pins+summing boards
  //  (follow mixture described in Pascal Perrodo note
  GeoPhysVol *Sumb_phys=NULL;
  {
    double ThickSum = 10.*Gaudi::Units::mm;    // FIXME should be in geometry database
    double Rmini = Moth_inner_radius+Xel1f-ThickSum;
    double Rmaxi = Moth_inner_radius+Xel1f -0.020*Gaudi::Units::mm;    // safety margin
    double DeltaZ = Zhalfc;
    double Zpos=0.;
    std::string name = baseName + "SUMB";
#ifdef DEBUGGEO
    std::cout << " *** parameters for SUMB tubs " << std::endl;
    std::cout << " DeltaZ      " << DeltaZ << std::endl;
    std::cout << " Rmin/Rmax   " << Rmini << " " << Rmaxi << std::endl,
    std::cout << " PhiMin,Dphi " << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
    std::cout << " Zpos in TELF " << Zpos << std::endl;
#endif

    GeoTubs * tubs = new GeoTubs(Rmini,Rmaxi,DeltaZ,Moth_Phi_Min,Moth_Phi_Max);
    const GeoLogVol* logVol = new GeoLogVol(name,tubs,Sumb);
    Sumb_phys = new GeoPhysVol(logVol);
    Elnicsf_phys->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));
    Elnicsf_phys->add(Sumb_phys);
  }


  // Solids and Logical Volumes for Mother_Boards and cables-------------------//
  {
    double ClearancePS = m_parameters->GetValue("LArEMBMoBoclearfrPS");
    double RhoPosB = Moth_inner_radius + ClearancePS;
    double bdx = .5*(m_parameters->GetValue("LArEMBMoBoTchickness")); // 4.3/2.*Gaudi::Units::mm
    double bdy = .5*(m_parameters->GetValue("LArEMBMoBoHeight"));     // 72.3/2.*Gaudi::Units::mm;
    double bdz = Zhalfc - 0.007*Gaudi::Units::mm;
    
    //------------------------MOTHERBOARDS--------------------------------------------//
    // JFB Make & Place the motherboards inside overall tube                          //
    {                                                                                 //
      double PhiOrig = 0.;                                                            //
      int NoOFboard = (int) m_parameters->GetValue("LArEMBnoOFmothboard");            // 32
      double PhiPos0 = twopi64 + PhiOrig;                                             //
      //                                                                              //
      std::string name = baseName + "MOAC";                                           //
#ifdef DEBUGGEO
      std::cout << " *** parameters for MotherBoard (box)" << std::endl;
      std::cout << "  dx,dy,dz  " << bdx << " " << bdy << " " << bdz << std::endl;
      std::cout << " Radius pos " << RhoPosB << std::endl;
      std::cout << " Phi0,Dphi  " << PhiPos0/Gaudi::Units::deg << " " << twopi32/Gaudi::Units::deg << std::endl;
#endif
      GeoBox               * box    = new GeoBox(bdx,bdy,bdz);                        //
      const GeoLogVol      * logVol = new GeoLogVol(name,box,Moth_elect);             //
      GeoPhysVol           * pV     = new GeoPhysVol(logVol);                         //
      GeoSerialIdentifier  * iD     = new GeoSerialIdentifier(0);                     //
      GeoGenfun::Variable     c;                                                         //
      GeoGenfun::GENFUNCTION  PhiPos = PhiPos0 + twopi32*c;                              //
      GeoXF::TRANSFUNCTION TX =                                                       //
	GeoXF::Pow(GeoTrf::TranslateX3D(1.0),RhoPosB*Cos(PhiPos))*                         //
	GeoXF::Pow(GeoTrf::TranslateY3D(1.0),RhoPosB*Sin(PhiPos))*                         //
	GeoXF::Pow(GeoTrf::RotateZ3D(1.0),PhiPos);                                         //
      GeoSerialTransformer *st = new GeoSerialTransformer(pV, &TX, NoOFboard);        //
      Elnicsf_phys->add(iD);                                                          //
      Elnicsf_phys->add(st);                                                          //
      //                                                                              //
    }                                                                                 //
    //--------------------------------------------------------------------------------//
    
    //** GU:  GeoModel GeoTrap constructor only has the 11 argument
    //        full list and not some possible shorter versions of G4
    //   => add the needed arguments
    // Dz=Dzc,Theta=0,Phi=0,DyDzn=Dy1,DxDyndzn=Dx1 Dxdypdzn=Dx1
    //    Angleydzn=0, DyDzp=Dy2,Dxdnydzp=Dx2,dwdypdzp=Dx2
    //    Angleydzp=0
    //---------------------------------CABLES-----------------------------------------//
    // JFB Place the cables                                                           //
    {                                                                                 //
      //                                                                              //
      double Dzc = Zhalfc - 0.007*Gaudi::Units::mm;                                                 //
      double Dx1 = .5*(m_parameters->GetValue("LArEMBCablethickat0"));                // 1./2.*Gaudi::Units::mm
      double Dx2 = .5*Bar_Eta_cut*(m_parameters->GetValue("LArEMBthickincrfac"));     //
      // Dx2 should be  5.17/2.*Bar_Eta_cut*Gaudi::Units::mm Trapezoid's side linear with Eta       //
      double Dy1 = .5*(m_parameters->GetValue("LArEMBCableEtaheight"));               // 70./2.*Gaudi::Units::mm
      double Dy2 = Dy1;                                                               //
      //                                                                              //
      int NoOFcable = (int) m_parameters->GetValue("LArEMBnoOFcableBundle");          // 64
      double RhoPosC = Moth_inner_radius + ClearancePS;                               //
      //                                                                              //    
      std::string name = baseName + "CAAC";                                           //
#ifdef DEBUGGEO
      std::cout << " *** Parameters for Cable (Trap) " << std::endl;
      std::cout << " Dzc " << Dzc << std::endl;
      std::cout << " Dx1,Dx2 (half thickness at eta=0 and etamax " << Dx1 << " " << Dx2 << std::endl;
      std::cout << " Dy1,Dy2 " << Dy1 << " " << Dy2 << std::endl;
      std::cout << " Radial Position " << RhoPosC << std::endl;
#endif
      //  GeoTrap* trap = new GeoTrap(Dx1,Dx2,Dy1,Dy2,Dzc);                           //
      GeoTrap* trap = new GeoTrap(Dzc,0.,0.,Dy1,Dx1,Dx1,0.,                           //
				  Dy2,Dx2,Dx2,0.);                                    //
      const GeoLogVol* logVol = new GeoLogVol(name,trap,Cable_elect);                 //
      //                                                                              //
      GeoPhysVol           * pV = new GeoPhysVol(logVol);                             //
      GeoSerialIdentifier  * iD   = new GeoSerialIdentifier(0);                       //
      double PhiPos0 = (twopi64 - asin(bdy/RhoPosB))/2.;                              //
#ifdef DEBUGGEO
      std::cout << " PhiPos0 " << PhiPos0 << std::endl;
#endif
      GeoGenfun::Variable I;                                                             //
      //                                                                              //
      // -------------Make a Kronecker Delta Here--------//                           //
      GeoGenfun::Rectangular KDelta;                        //                           //
      KDelta.baseline().setValue(0.0);                   //                           //
      KDelta.height().setValue(1.0);                     //                           //
      KDelta.x0().setValue(-0.5);                        //                           //
      KDelta.x1().setValue(0.5);                         //                           //
      //-------------------------------------------------//                           //
      //                                                                              //
      GeoGenfun::Mod Mod1(1.0),Mod2(2.0),Mod4(4.0);                                      //
      GeoGenfun::GENFUNCTION Int = I - Mod1;                                             //
      GeoGenfun::GENFUNCTION Ccopy    = Int(I + 0.5);                                    //
      GeoGenfun::GENFUNCTION PhiOrig  = 22.5*Gaudi::Units::deg*Int(Ccopy/4);                           //
      GeoGenfun::GENFUNCTION PhiPos1  = PhiPos0 + PhiOrig;                               //
      GeoGenfun::GENFUNCTION PhiPos2  = twopi32 - PhiPos0 + PhiOrig;                     //
      GeoGenfun::GENFUNCTION PhiPos00 =                                                  //
	(KDelta(Mod4(Ccopy)-2) + KDelta(Mod4(Ccopy)-3))*PhiPos2 +                     //
	(1.0-KDelta(Mod4(Ccopy)-2)-KDelta(Mod4(Ccopy)-3))*PhiPos1;                    //
      GeoGenfun::GENFUNCTION PhiPos   = PhiPos00 + Mod2(Ccopy)*twopi32;                  //
      GeoXF::TRANSFUNCTION TX =                                                       //
	GeoXF::Pow(GeoTrf::TranslateX3D(1.0),RhoPosC*Cos(PhiPos))*                         //
	GeoXF::Pow(GeoTrf::TranslateY3D(1.0),RhoPosC*Sin(PhiPos))*                         //
	GeoXF::Pow(GeoTrf::RotateZ3D(1.0),PhiPos);                                         //
      GeoSerialTransformer *st = new GeoSerialTransformer(pV, &TX, NoOFcable);        //
#ifdef DEBUGGEO
      for (int ii=0;ii<NoOFcable;ii++) {
       std::cout << "copy, phi " << ii << " " << PhiPos(ii)/Gaudi::Units::deg << std::endl;
      }
#endif
      Elnicsf_phys->add(iD);                                                          //
      Elnicsf_phys->add(st);                                                          //
      //                                                                              //
    }                                                                                 //
    //--------------------------------------------------------------------------------//
  }
#endif // BUILD_FRONT_ELECTRONICS

    // add 1.3 Gaudi::Units::mm in z to allow cleareance for absorber with non                    //
    // 0 thickness, at eta=1.475, low r part of the barrel                          //
    // this affects STAC and TELB volumes                                           //
    double clearance = 1.3*Gaudi::Units::mm;     

#ifdef BUILD_HIGHETA_ELECTRONICS

  // back electronics are in an extruded cone
  //
  // ***GU: GeoCons  be very careful, the order of the arguments
  // is NOT the same as in G4Cons routine!!!
  //  G4 order:  Rmin1,Rmax1,Rmin2,Rmax2,DeltaZ,phi0,Dphi
  //  Geo order: Rmin1,Rmin2,Rmax1,Rmax2,DeltaZ,phi0,Dphi
  //---------------------------------HIGHETA ELECTRONICS----------------------------//
  {                                                                                 //
    //                                                                              //
    // GU fix of TELB                                                               //
    double ze1=  zmax1_Stac+clearance;                                              //
    double ze2 = Bar_Z_max;                                                         //
    double safety = 0.05*Gaudi::Units::mm;
    double DeltaZ = 0.5*(ze2-ze1)-safety;                           // 50 micron for safety.
    double Zpos = ze1+DeltaZ+0.5*safety;                                            // 
    double Rmini1 = Rhocen[0] - .030*Gaudi::Units::mm;                                            //
    double Rmaxi1 = Rhocen[0] - .020*Gaudi::Units::mm;                                            //
    double Rmini2 = Rhocen[0] - .030*Gaudi::Units::mm;                                            //
    double Rmaxi2 = Bar_Rcmx - clearance - .070*Gaudi::Units::mm;                                 //
    std::string name = baseName + "TELB";                                           //
#ifdef DEBUGGEO
    std::cout << " *** Parameters for high eta electronics (Cons) " <<std::endl;
    std::cout << " Rmini1,Rmini2,Rmaxi1,Rmaxi2 " << Rmini1 << " " << Rmini2 << " "
       << Rmaxi1 << " " << Rmaxi2 << std::endl,
    std::cout << " DeltaZ " << DeltaZ << std::endl;
    std::cout << " Phi_Min,Dphi " << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
    std::cout << " Zpos " << Zpos << std::endl;
#endif
    GeoCons* cons = new GeoCons(Rmini1,Rmini2,Rmaxi1,Rmaxi2,                        //
				DeltaZ,Moth_Phi_Min,Moth_Phi_Max);                  //
    const GeoLogVol* logVol = new GeoLogVol(name,cons,Cable_elect);                 //
    GeoPhysVol* physVol = new GeoPhysVol(logVol);                                   //
    m_ecamPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));                //
    m_ecamPhysicalPos->add(physVol);                                                //
    m_ecamPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));                //
    m_ecamPhysicalNeg->add(physVol);                                                //
    //                                                                              //
  }                                                                                 //
  //--------------------------------------------------------------------------------//

#endif  // BUILD_HIGHETA_ELECTRONICS


//
// Front and back G10 : TUBES in ECAM
//

#ifdef BUILD_FRONT_G10

  
  //---------------------------------FRONT G10-------------------------------------//
  {                                                                                //
    double Xel1f = m_parameters->GetValue("LArEMBInnerElectronics");               // 23.*Gaudi::Units::mm
    double Xg10f = m_parameters->GetValue("LArEMBG10SupportBarsIn");               // 20.*Gaudi::Units::mm
    double DeltaZ = 0.5* m_parameters->GetValue("LArEMBG10FrontDeltaZ");           //
    double Zpos = DeltaZ+Bar_Z_min;                                                //
    double Rmini = Moth_inner_radius + Xel1f;                                      //
    double Rmaxi = Rmini+Xg10f;                                                    //
    std::string name = baseName + "GTENF";                                         //
#ifdef DEBUGGEO 
    std::cout << " *** parameters for front G10 ring (tubs) " << std::endl;
    std::cout << " Rmini,Rmaxi " << Rmini << " " << Rmaxi << std::endl;
    std::cout << " DeltaZ " << DeltaZ << std::endl;
    std::cout << " phimin,dphi " << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
    std::cout << " Zpos " << Zpos << std::endl;
#endif
    GeoTubs* tubs = new GeoTubs(Rmini,Rmaxi,DeltaZ,Moth_Phi_Min,Moth_Phi_Max);     //
    const GeoLogVol* logVol = new GeoLogVol(name,tubs,G10_bar);                    //
    GeoPhysVol* physVol = new GeoPhysVol(logVol);                                  //
    m_ecamPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));               //
    m_ecamPhysicalPos->add(physVol);                                               //
    m_ecamPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));               //
    m_ecamPhysicalNeg->add(physVol);                                               //

    IRDBRecordset_ptr extraCones = rdbAccess->getRecordsetPtr("LArCones",          
							      larVersionKey.tag(),   
							      larVersionKey.node()); 
    if (extraCones->size() > 0 ) {
      for (unsigned int i=0; i<extraCones->size(); i++)
      {
        std::string conName = (*extraCones)[i]->getString("CONE");
//        std::cout << " cone name " << conName << std::endl;
        if (conName=="ExtraInBar") {
         double extra_dz = 0.5*( (*extraCones)[i]->getDouble("DZ") );
         double extra_rmin1 = (*extraCones)[i]->getDouble("RMIN1");
         double extra_rmin2 = (*extraCones)[i]->getDouble("RMIN2");
         double extra_rmax1 = (*extraCones)[i]->getDouble("RMAX1");
         double extra_rmax2 = (*extraCones)[i]->getDouble("RMAX2");
         double extra_phi0  = (*extraCones)[i]->getDouble("PHI0");
         double extra_dphi  = (*extraCones)[i]->getDouble("DPHI");
         double extra_zpos = (*extraCones)[i]->getDouble("ZPOS");
//         std::cout << " rmin1,rmin2,rmax1,rmax2,dz,phi0,dphi,zpos " << extra_rmin1 << " " << extra_rmin2 << " "
//                 << extra_rmax1 << " " << extra_rmax2 << " " << extra_dz << " " << extra_phi0 << " "
//                 << extra_dphi << " " << extra_zpos << std::endl;
         std::string name = baseName + "ExtraMat1";                                      
         GeoCons* cons = new GeoCons(extra_rmin1,extra_rmin2,extra_rmax1,extra_rmax2,    
                               extra_dz,extra_phi0,extra_dphi);                     
         const GeoLogVol* logVol = new GeoLogVol(name,cons,Lead);                         
         GeoPhysVol* physVol2 = new GeoPhysVol(logVol);                                   
         physVol->add(new GeoTransform(GeoTrf::TranslateZ3D(extra_zpos)));                    
         physVol->add(physVol2);                                                           

        }
      }
    }

  }                                                                                //
  //-------------------------------------------------------------------------------//
#endif // BUILD_FRONT_G10

#ifdef BUILD_BACK_G10

  //---------------------------------BACK G10-------------------------------------//
  {                                                                               //
    double DeltaZ = Zhalf;                                                        //
    double Zpos = Zhalf+Bar_Z_min;                                                //
    double Xtal  = m_parameters->GetValue("LArEMBLArGapTail")+ 0.1*Gaudi::Units::mm;            // 13.*Gaudi::Units::mm
    double Rmini = Rhocen[Nbrt]+Xtal; //                                          //
    // GU to be sure that GTENB does not go outside mother ECAM volume            //
    //  Rmaxi = Rmini+Xg10b;                                                      //
    double   Rmaxi = Moth_outer_radius-0.01*Gaudi::Units::mm;   // 10 microns for more safety.. //
    //                                                                            //
    std::string name = baseName +"GTENB";                                         //
#ifdef DEBUGGEO 
    std::cout << " *** parameters for back G10 ring (tubs) " << std::endl;
    std::cout << " Rmini,Rmaxi " << Rmini << " " << Rmaxi << std::endl;
    std::cout << " DeltaZ  " << DeltaZ << std::endl;
    std::cout << " phimin,dphi " << Moth_Phi_Min/Gaudi::Units::deg << " " << Moth_Phi_Max/Gaudi::Units::deg << std::endl;
    std::cout << " Zpos " << Zpos << std::endl;
#endif

    GeoTubs* tubs = new GeoTubs(Rmini,Rmaxi,DeltaZ,Moth_Phi_Min,Moth_Phi_Max);    //
    const GeoLogVol* logVol = new GeoLogVol(name,tubs,G10_bar);                       //
    GeoPhysVol* physVol = new GeoPhysVol(logVol);                                 //
    m_ecamPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));              //
    m_ecamPhysicalPos->add(physVol);                                              //
    m_ecamPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(Zpos)));              //
    m_ecamPhysicalNeg->add(physVol);                                              //
  }                                                                               //
  //------------------------------------------------------------------------------//
#endif // BUILD_BACK_G10


  GeoPhysVol *stacPhysical=NULL;
  //---------------------------------ACCORDION VOLUME---------------------------//
  //  STAC = Pcon: LAr volume in which the accordion structure is located       //
  //     (the front/back small straight parts are in ECAM not in STAC)           //
  //  in test beam case need to have STAC between -1.055 and size 23.555 degrees
  //  (i.e. a little bit wider than one calorimeter module)
  {                                                                             //

    double Moth_Phi_Min2 = (Ncell == 64) ? -1.055*Gaudi::Units::deg : 0.;                     //
    double Moth_Phi_Max2 = (Ncell == 64) ? 24.61*Gaudi::Units::deg  : 2*M_PI;                 //

    double Zplan1[] = {Bar_Z_min,zmax1_Stac+clearance,Bar_Z_max};                //
    double Riacc1[] = {Rhocen[0],Rhocen[0], Bar_Rcmx-clearance};                //
    double Roacc1[] = {Rhocen[Nbrt],Rhocen[Nbrt],Rhocen[Nbrt]};                 //
    //                                                                          //
    std::string name = baseName + "STAC";                                       //
#ifdef DEBUGGEO
     std::cout << " *** Parameters for STAC Pcon volume " << std::endl;
     std::cout << "  Zplan " << Zplan1[0] << " " << Zplan1[1] << " " << Zplan1[2] << std::endl;
     std::cout << "  Rin   " << Riacc1[0] << " " << Riacc1[1] << " " << Riacc1[2] << std::endl;
     std::cout << "  Rout  " << Roacc1[0] << " " << Roacc1[1] << " " << Roacc1[2] << std::endl;
     std::cout << " PhiMin,Dphi " << Moth_Phi_Min2/Gaudi::Units::deg << " " << Moth_Phi_Max2/Gaudi::Units::deg << std::endl;
     std::cout << " Zpos " << -Zp0 << std::endl;
#endif
    GeoPcon* pCone = new GeoPcon(Moth_Phi_Min2,Moth_Phi_Max2);                  //
    for (int i=0; i<3; i++) pCone->addPlane(Zplan1[i],Riacc1[i],Roacc1[i]);     //
    const GeoLogVol* logVol = new GeoLogVol(name,pCone,LAr);                    //
    stacPhysical = new GeoPhysVol(logVol);                                      //
    m_ecamPhysicalPos->add(new GeoTransform(GeoTrf::TranslateZ3D(-Zp0)));            //
    m_ecamPhysicalPos->add(stacPhysical);                                       //
    m_ecamPhysicalNeg->add(new GeoTransform(GeoTrf::TranslateZ3D(-Zp0)));            //
    m_ecamPhysicalNeg->add(stacPhysical);                                       //
  }                                                                             //
  //----------------------------------------------------------------------------//
  
#ifdef BUILD_ACCORDION_PLATES
  {
    // Construct the straight and the corner parts of the Accordion plates
    
    double Xtip_pb    = m_parameters->GetValue("LArEMBLeadTipThickFront");
    double Xtip_pc    = m_parameters->GetValue("LArEMBLeadTipThickEnd");
    double Xtip_gt    = m_parameters->GetValue("LArEMBG10TipThickFront");
    double Xtip_gs    = m_parameters->GetValue("LArEMBG10TipThickEnd");
    double Thgl       = m_parameters->GetValue("LArEMBThickAbsGlue");
    double Thfe       = m_parameters->GetValue("LArEMBThickAbsIron");
    double Thpb       = m_parameters->GetValue("LArEMBThickAbsLead");
    double Thpb_thin  = m_parameters->GetValue("LArEMBThinAbsLead");
    double Thcu       = m_parameters->GetValue("LArEMBThickElecCopper");
    double Thfg       = m_parameters->GetValue("LArEMBThickElecKapton");
    double Psi        = m_parameters->GetValue("LArEMBPhiGapAperture");   // 360.*Gaudi::Units::deg/1024
    double Contract   = m_parameters->GetValue("LArEMBAbsorberContraction");

    double Thce = (Thpb+Thgl+Thfe)*Contract;
    double Thel = Thcu+Thfg;
   
    double Alfa = Psi;                        // size of one phi cell
   
    double Zmin = Bar_Z_min;
    double Zmax = Bar_Z_max;
// Zcp1 = z max for eta=0.8 at the radius of the middle of the fold
// Zcp2 = z max for eta=1.475 at the radius of the middle of the fold
// Along = lenght of the straight sections
    double Zcp1[15],Zcp2[15],Along[14];

// Rhol = radius at the beginning of the straight section
// Rhoh = radius at the end of the straight section
// Zcp1l = z max for eta=0.8 at the beginning of the straight section
// Zcp1h = z max for eta=0.8 at the end of the straight section
// Zcp2l = z max for eta~=1.475 at the beginning of the straight section
// Zcp2h = z max for eta~=1.475 at the end of the straight section
    double Zcp1l[14],Zcp1h[14],Zcp2l[14],Zcp2h[14];
    double Rhol[14],Rhoh[14];

    double safety_along = 0.007*Gaudi::Units::mm;
 
   
    // Compute centers of curvature coordinates in a local frame.
   
    double Cenx[15]={0}, Ceny[15]={0} ;
    for (int jf=0; jf<(Nbrt+1); jf++)
      {
	Cenx[jf] = Rhocen[jf]*cos(Phicen[jf]); // Phicen[] already in radians
	Ceny[jf] = Rhocen[jf]*sin(Phicen[jf]);
        Zcp1[jf] = Rhocen[jf]/tan(TetaTrans[jf]);
        if (Rhocen[jf] < r2)
           Zcp2[jf] = z1 + (Rhocen[jf]-r1)*inv_r2_r1*(Moth_Z_max-z1);
         else
           Zcp2[jf]=Moth_Z_max;

#ifdef DEBUGGEO
    std::cout << " jf " << jf
        << "Cenx/Ceny " << Cenx[jf] << " " << Ceny[jf] << " "
        << "Zcp1/Zcp2 " << Zcp1[jf] << " " << Zcp2[jf] << std::endl;
#endif

      }

// Compute staight lengths of folds
    double Rint = m_parameters->GetValue("LArEMBNeutFiberRadius");
    for (int jl=0; jl<Nbrt; jl++)
    {
      double Adisc2 = (Cenx[jl+1]-Cenx[jl])*(Cenx[jl+1]-Cenx[jl])+
                      (Ceny[jl+1]-Ceny[jl])*(Ceny[jl+1]-Ceny[jl]);
      double Along2 = Adisc2 - 4.*Rint*Rint; 
      Along[jl] = sqrt(Along2);
#ifdef DEBUGGEO 
      std::cout << "jl " << jl
                << "straight length " << Along[jl] << std::endl;
#endif
      double ddelta = M_PI/2.-Delta[jl];
// different parity depending on direction of first wave
      if (checkParity==0) {
        if (jl%2==1) ddelta=-1.*ddelta;
      } else {
        if (jl%2==0) ddelta=-1.*ddelta;
      }
      double xlong=Along[jl]-2.*safety_along;
      double x2=0.5*(Cenx[jl+1]+Cenx[jl])+0.5*xlong*cos(ddelta);
      double y2=0.5*(Ceny[jl+1]+Ceny[jl])+0.5*xlong*sin(ddelta);
      double x1=0.5*(Cenx[jl+1]+Cenx[jl])-0.5*xlong*cos(ddelta);
      double y1=0.5*(Ceny[jl+1]+Ceny[jl])-0.5*xlong*sin(ddelta);
      Rhol[jl] = sqrt(x1*x1+y1*y1);
      Rhoh[jl] = sqrt(x2*x2+y2*y2);
      Zcp1l[jl] = Rhol[jl]/tan(TetaTrans[jl]);
      Zcp1h[jl] = Rhoh[jl]/tan(TetaTrans[jl+1]);
      if (Rhol[jl] < r2) 
         Zcp2l[jl] = z1 + (Rhol[jl]-r1)*inv_r2_r1*(Moth_Z_max-z1);
       else
         Zcp2l[jl]=Moth_Z_max;
      if (Rhoh[jl] < r2) 
         Zcp2h[jl] = z1 + (Rhoh[jl]-r1)*inv_r2_r1*(Moth_Z_max-z1);
       else    
         Zcp2h[jl]=Moth_Z_max;
#ifdef DEBUGGEO
      std::cout << "x1,y1,x2,y2 " << x1 << " " << y1 << " " << x2 << " "
                << y2 << std::endl;
      std::cout << "  lower/upper radius " << Rhol[jl] << " " << Rhoh[jl]
                << " Z for eta0.8 " << Zcp1l[jl] << " " << Zcp1h[jl]
                << " Z for etamax " << Zcp2l[jl] << " " << Zcp2h[jl] << std::endl;
#endif
  }

   

    double Gama0      = m_parameters->GetValue("LArEMBAbsPhiFirst");
   
    GeoGenfun::Variable icopy;
    GeoGenfun::GENFUNCTION Game = Gama0 + Psi/2 + Alfa*icopy;
    GeoGenfun::GENFUNCTION Gama = Gama0 + Alfa*icopy;
   
    // Creation of the straight absorber parts. Front (TIPB) & Back (TIPC)
    // Creation of the straight electrode parts. Front (TIPK) & Back (TIPL)

    enum FB {FRONT, BACK, TERM};
   
    for (int fb=FRONT; fb!=TERM; fb++) 
      {
       
	int irl = fb==FRONT ? 0 : Nbrt;
       
	double Xtips = Xtip_pb + Xtip_gt;
	double Xtipt = Xtip_pc + Xtip_gs;

	// Absorber (thick, thin)
	{
	  double radius =  fb==FRONT ? Cenx[0] - Xtip_pb/2    : Cenx[Nbrt] + Xtipt/2;
	  double Xhalfb  = fb==FRONT ? Xtip_pb/2 -0.002*Gaudi::Units::mm    : Xtipt/2 - .004*Gaudi::Units::mm;
	  double Zhalfb  = fb==FRONT ? (Bar_Z_cut-Zmin)/2. : (Zmax-Zmin)/2.;
	  double dz01 = (std::min(Zcp1[irl],Zmax)-Zmin)/2.;  // half lenght for thick lead

	  std::string name         = baseName + "FrontBack::Absorber";
	  GeoBox *box              = new GeoBox(Xhalfb,Thce/2,Zhalfb);
	  GeoBox *box2             = new GeoBox(Xhalfb,Thce/2,dz01);
	  const GeoLogVol *logVol  = new GeoLogVol(name,box,Thin_abs);
	  const GeoLogVol *logVol2 = new GeoLogVol(name,box2,Thick_abs);
	  GeoPhysVol* physVol      = new GeoPhysVol(logVol);
	  GeoPhysVol* physVol2     = new GeoPhysVol(logVol2);
	  physVol->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,dz01-Zhalfb)));
	  physVol->add(physVol2);
	  GeoGenfun::GENFUNCTION Xcd = radius*Cos(Gama);
	  GeoGenfun::GENFUNCTION Ycd = radius*Sin(Gama);
	  GeoGenfun::GENFUNCTION Zcd = GeoGenfun::FixedConstant(Zmin+Zhalfb);
	 
	 
	  GeoXF::TRANSFUNCTION TX = 
	    GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcd) *
	    GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycd) *
	    GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcd) * 
	    GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Gama);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol, &TX, Nabsorber);     
	  m_ecamPhysicalPos->add(st);
	  m_ecamPhysicalNeg->add(st);
#ifdef DEBUGGEO
          if (fb==FRONT) std::cout << " *** Front tip Absorber " << std::endl;
          else           std::cout << " *** Back  tip Absorber " << std::endl;
          std::cout << " Thin Abs Box " << Xhalfb << " " << Thce/2. << " " << Zhalfb << std::endl;
          std::cout << " Thick Abs Box " << Xhalfb << " " << Thce/2. << " " << dz01 << std::endl;
          std::cout << " Z position thick in thin " << dz01-Zhalfb << std::endl;
          std::cout << " Radial position " << radius << std::endl;
          std::cout << " Phi0 (Gaudi::Units::deg) " << Gama(0)/Gaudi::Units::deg << std::endl;
          std::cout << " Z position in ECAM " << Zmin+Zhalfb << std::endl;
#endif
	}
	// G10 (only for front part)
	if (fb==FRONT)
	{
	  double Xhalfbg = Xtip_gt/2-0.002*Gaudi::Units::mm;
	  double radiusg = Cenx[0]-Xtip_pb/2. - Xtips/2   ;   
	  double Zhalfbg = (Bar_Z_cut-Zmin)/2.    ;
	  std::string name        = baseName + "FrontBack::G10";
#ifdef DEBUGGEO
          std::cout << " *** Front tip G10 " << std::endl;
          std::cout << " Box parameters " << Xhalfbg << " " << Thce/2. << " " << Zhalfbg << std::endl;
#endif
	  GeoBox *box = new GeoBox(Xhalfbg,Thce/2,Zhalfbg);
	  const GeoLogVol *logVol  = new GeoLogVol(name,box,G10_bar);
	  GeoPhysVol      *physVol = new GeoPhysVol(logVol);

#ifdef BUILD_FRONT_STEEL
// GU 28-07-2005
// add small iron pieces on each side of the G10
          name  = baseName + "FrontBack::Steel";
          double Tgfe = m_parameters->GetValue("LArEMBThinAbsIron");
#ifdef DEBUGGEO
          std::cout << "   Iron Box parameters " << Xhalfbg 
                    << " " << Tgfe/4. << " " << Zhalfbg << std::endl;
#endif
          GeoBox *box2 = new GeoBox(Xhalfbg,Tgfe/4.,Zhalfbg);
          const GeoLogVol *logVol2 = new GeoLogVol(name,box2,Iron);
          GeoPhysVol *physVol2 = new GeoPhysVol(logVol2);
#ifdef DEBUGGEO
          std::cout << "  Position Iron in G10 at y = +- " << 0.5*(+Thce-Tgfe/2.) << std::endl;
#endif
          physVol->add(new GeoTransform(GeoTrf::Translate3D(0.,0.5*(-Thce+Tgfe/2.),0.))); 
          physVol->add(physVol2);
          physVol->add(new GeoTransform(GeoTrf::Translate3D(0.,0.5*(+Thce-Tgfe/2.),0.)));
          physVol->add(physVol2);
#endif   // build_front_steel

// position of G10+Steel inside LAr bath
	  GeoGenfun::GENFUNCTION Xcd = radiusg*Cos(Gama);
	  GeoGenfun::GENFUNCTION Ycd = radiusg*Sin(Gama);
	  GeoGenfun::GENFUNCTION Zcd = GeoGenfun::FixedConstant(Zhalfbg+Zmin);
	  GeoXF::TRANSFUNCTION TX = 
	    GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcd) *
	    GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycd) *
	    GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcd) * 
	    GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Gama);
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol, &TX, Nabsorber);     
	  m_ecamPhysicalPos->add(st);
	  m_ecamPhysicalNeg->add(st);
#ifdef DEBUGGEO
          std::cout << "  Radial position G10 tip " << radiusg << std::endl;
          std::cout << "  Phi0 (Gaudi::Units::deg)" << Gama(0)/Gaudi::Units::deg << std::endl;
          std::cout << "  Zposition in ECAM " << Zmin+Zhalfbg << std::endl;
#endif

	}
	// Electrode
	{
	  double Xhalfbe = fb==FRONT ? Xtips/2 -0.002*Gaudi::Units::mm      : Xtipt/2 - .004*Gaudi::Units::mm;
	  double Zhalfbe = fb==FRONT ? (Bar_Z_cut-Zmin)/2.    : (Zmax - Zmin)/2;
	  double radiuse = fb==FRONT ? Cenx[0] - Xtips/2   : Cenx[Nbrt] + Xtipt/2;
	  std::string name        = baseName + "FrontBack::Electrode";
	  GeoBox *box             = new GeoBox(Xhalfbe,Thel/2,Zhalfbe);
	  const GeoLogVol *logVol = new GeoLogVol(name,box,Kapton_Cu);
	  GeoPhysVol* physVol     = new GeoPhysVol(logVol);
	  GeoGenfun::GENFUNCTION Xcd  = radiuse*Cos(Game);
	  GeoGenfun::GENFUNCTION Ycd  = radiuse*Sin(Game);
	  GeoGenfun::GENFUNCTION Zcd  = GeoGenfun::FixedConstant(Zmin+Zhalfbe);
	 
	  GeoXF::TRANSFUNCTION TX = 
	    GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcd) *
	    GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycd) *
	    GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcd) * 
	    GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Game);
	 
	  GeoSerialTransformer *st = new GeoSerialTransformer(physVol, &TX, Nelectrode);     
	  m_ecamPhysicalPos->add(st);
	  m_ecamPhysicalNeg->add(st);

#ifdef DEBUGGEO
          if (fb==FRONT) std::cout << " *** Front tip electrode "  << std::endl;
          else           std::cout << " *** Back  tip electrode " << std::endl;
          std::cout << " Box " << Xhalfbe << " " << Thel/2. << " " << Zhalfbe << std::endl;
          std::cout << " Radial position " << radiuse << std::endl;
          std::cout << " Phi0 (Gaudi::Units::deg)" << Game(0)/Gaudi::Units::deg << std::endl;
          std::cout << " Z position in ECAM " << Zmin+Zhalfbe << std::endl;
#endif

	 
	}	 
      }


    GeoStraightAccSection *gStraightElectrodes=NULL;
    GeoStraightAccSection *gStraightAbsorbers =NULL;
    //
    // Loop through the straight and corner(Fold) parts of the Accordion plates
    // Repeat each part around Phi, then move to the next, towards outer radii
    //

// GU 09/06/2004 add some safety in z size
    double safety_zlen=0.050*Gaudi::Units::mm;
   
    for(int irl=0; irl<Nbrt; irl++)   // loop over zig-zag in radius
      {
#ifdef DEBUGGEO
        std::cout << " ----- irl = " << irl << std::endl;
#endif
        int iparit;
// different parity depending on direction of first wave
        if (checkParity==0) {       
	 iparit= (1-2*(irl%2));  // +1 for irl=0,2,4,..   -1 for irl=1,3,5,..
        } else {
         iparit=(2*(irl%2)-1);  // -1 for irl=0,2,3...  +1 for irl=1,3,5
        }
       
// length of tubs for folds
        double Zcon1 = Zcp2[irl];
        double Zcon2 = Zcp2[irl+1];
// if the radius at the lower edge of this fold is smaller than
// Bar_rcmx, take the lenght at this radius instead of the radius
// of the fold to avoid going outside the mother volume
        if (irl>0 && Rhoh[irl-1] < Bar_Rcmx) Zcon1=Zcp2h[irl-1];
        if (Rhoh[irl] < Bar_Rcmx) Zcon2=Zcp2h[irl];

        double dz0 = (std::min(Zcon1,Zmax)-Zmin)/2.;     // half lenght in z (fold)
        double dza = (std::min(Zcon2,Zmax)-Zmin)/2.;     // (for last fold)
        double dz01 = (std::min(Zcp1[irl],Zmax)-Zmin)/2.;  // half lenght for thick lead
        double dza1 = (std::min(Zcp1[irl+1],Zmax)-Zmin)/2.;


// special case if Rhocen[il] < Bar_Rcmx < Rhocen[il+1]
// we had to divide the fold in two different pieces to deal with the shape
        int ndivi=1;
        if (Rhocen[irl] < Bar_Rcmx && Rhocen[irl+1] > Bar_Rcmx) {
#ifdef DEBUGGEO
            std::cout << " Divide section in two pieces at r= " << Bar_Rcmx << std::endl;
#endif
            ndivi=2;
        } 

        for (int idivi=0;idivi<ndivi;idivi++) {

            // lenght in z at beginning and end of straigth part
            double bl1,tl1;
            double frac;
            if (ndivi==1) {
              bl1 = (std::min(Zcp2l[irl],Zmax)-Zmin)/2.;
              tl1 = (std::min(Zcp2h[irl],Zmax)-Zmin)/2.;
              frac=1.;
            }
            else {
              if (idivi==0) {
                bl1 = (std::min(Zcp2l[irl],Zmax)-Zmin)/2.;
                tl1 = (Zmax-Zmin)/2.;
                frac=(Bar_Rcmx-Rhol[irl])/(Rhoh[irl]-Rhol[irl]);
              }
              else {
                bl1 = (Zmax-Zmin)/2.;
                tl1 = (Zmax-Zmin)/2.;
                frac=(Rhoh[irl]-Bar_Rcmx)/(Rhoh[irl]-Rhol[irl]);
              }
#ifdef DEBUGGEO
              std::cout << " Division " << idivi << " fraction of straight section " << frac << std::endl;
#endif
            }
//GU 09/06/2004 add small safety in size tl1 and bl1
            tl1=tl1-safety_zlen;
            bl1=bl1-safety_zlen;

//    =================================== Absorbers
	    {
	   
	      // thickness of absorber
	      double Dz = Thce/2.;
	   
	      // For absorbers
	      GeoGenfun::GENFUNCTION x1a = Fx(irl+0., Gama, Cenx, Ceny)
                                           +deltay[irl]*Del1(Gama)
                                           +deltax[irl]*Del2(Gama);
	      GeoGenfun::GENFUNCTION x2a = Fx(irl+1., Gama, Cenx, Ceny)
                                           +deltay[irl+1]*Del1(Gama)
                                           +deltax[irl+1]*Del2(Gama);
	      GeoGenfun::GENFUNCTION y1a = Fy(irl+0., Gama, Cenx, Ceny)
                                           -deltay[irl]*Del2(Gama)
                                           +deltax[irl]*Del1(Gama);
	      GeoGenfun::GENFUNCTION y2a = Fy(irl+1., Gama, Cenx, Ceny)
                                           -deltay[irl+1]*Del2(Gama)
                                           +deltax[irl+1]*Del1(Gama);
	      GeoGenfun::GENFUNCTION dx = x2a - x1a;
	      GeoGenfun::GENFUNCTION dy = y2a - y1a;
	   
	      // Da the two fold centers distance, da straight part length
	   
	      GeoGenfun::GENFUNCTION Da = Sqrt ( dx*dx + dy*dy );
	      GeoGenfun::GENFUNCTION da = Sqrt ( (Da - 2.*Rint)*(Da + 2.*Rint) );
	   
	      // newalpha (slant angle) value of the rotation angle around Z_axis
	      GeoGenfun::GENFUNCTION cosalfa = (da*dx -iparit*2.*Rint*dy)/Da/Da;
	      GeoGenfun::GENFUNCTION sinalfa = (da*dy +iparit*2.*Rint*dx)/Da/Da;
	      GeoGenfun::GENFUNCTION newalpha = ATan2(sinalfa,cosalfa);       
	   
	      GeoGenfun::GENFUNCTION h1 = da/2. * frac  - .007*Gaudi::Units::mm;
	   
	      double Zx0 = (tl1+bl1)/2.;
// thick absorber pieces
// more correct computation with z lenght computed exactly at the
// radius of the end and the beginning of the straight sections
              double Xb1,Xt1;
              if (ndivi==1) {
                Xb1 = (Zcp1l[irl]-Zmin)/2.;
                Xt1 = (Zcp1h[irl]-Zmin)/2.;
              } else {
                double xfrac=(Bar_Rcmx-Rhol[irl])/(Rhoh[irl]-Rhol[irl]);
                if (idivi==0) {
                  Xb1 = (Zcp1l[irl]-Zmin)/2.;
                  Xt1 = (xfrac*Zcp1h[irl]+(1.-xfrac)*Zcp1l[irl]-Zmin)/2.;
                }
                else {
                  Xb1 = (xfrac*Zcp1h[irl]+(1.-xfrac)*Zcp1l[irl]-Zmin)/2.;
                  Xt1 = (Zcp1h[irl]-Zmin)/2.;
                } 
              }
	    // translation in x to include thick absorber into thin absorber
	      double Xtrans = (Xb1+Xt1)/2.-Zx0 + .007*Gaudi::Units::mm;    

            // lengths that remain to be covered with the thin absorber
              double Xt2 = tl1-Xt1;
              double Xb2 = bl1-Xb1;

             // trabslation that would be needed to include think absorber only into overall thin+thick volume
              double Xtrans2 =  Zx0 - (Xb2+Xt2)/2.;
              Xt2 = Xt2 -0.007*Gaudi::Units::mm;
              Xb2 = Xb2 -0.007*Gaudi::Units::mm;
           
	   
	      GeoGenfun::GENFUNCTION alpha = ATan(0.5*(bl1-tl1)/h1);
	      GeoGenfun::GENFUNCTION alpha_t = ATan(0.5*(Xb1-Xt1)/h1);     

            // angle that would be needed for trap do describe only thin absorber
            //     ------------------|---------X---------|
            //     <------------------------------------->  2*tl1
            //                       <--------->   Xt2
            //
            //     <--------------------------------->  2*bl1
            //                    <-------->  Xb2 
            //     ---------------|--------X---------|
            // alpha = (-) angle between X's
            //   tan(alpha) = delta X size / width,   deltaX size = 2*tl1-Xt2-(2*bl1-Xb2),  width = 2.*h1  
	      GeoGenfun::GENFUNCTION alpha_2 = ATan((2.*bl1-Xb2-(2.*tl1-Xt2))/(2.*h1));      
	   
	   
	    // .newalpha is already computed angle wrt z axis
	    // P/2 rotation is to get absorber aligned along local x axis
	    // instead of y, then rotate with angle newalpha
	      GeoGenfun::GENFUNCTION alfrot =  -M_PI/2. - newalpha;

	      GeoGenfun::GENFUNCTION Xcd    = (x1a + x2a)/2. + (2.*idivi-1.)*(1.-frac)*da/2.*cosalfa;
	      GeoGenfun::GENFUNCTION Ycd    = (y1a + y2a)/2. + (2.*idivi-1.)*(1.-frac)*da/2.*sinalfa;
	      GeoGenfun::GENFUNCTION Zcd    = GeoGenfun::FixedConstant(Zmin+(tl1+bl1)/2.+safety_zlen);
	   
	      GeoXF::TRANSFUNCTION TX = 
	        GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcd) *
	        GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycd) *
	        GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcd) * 
	        GeoXF::Pow(GeoTrf::RotateZ3D(1.0),-alfrot)*
	        GeoTrf::RotateY3D(-90*Gaudi::Units::deg);                    

	    // 

	      for (int instance = 0; instance < Nabsorber; instance++) 
	      {
		
		// Define some commonly-used volume names.
		std::string thinName   = baseName + "ThinAbs::Straight";
		std::string thickName  = baseName + "ThickAbs::Straight";

#ifdef DEBUGGEO
                std::cout << " *** thinAbs trap parameters " << std::endl;
                std::cout << "  Thickness " << Dz << std::endl;
                std::cout << "  h1 (rad leng) " << h1(instance) << std::endl;
                std::cout << "  tl1,bl1 " << tl1 << " " << bl1 << std::endl;
                std::cout << "  alpha   " << alpha(instance) << std::endl;
                std::cout << " *** thickAbs trap parameters " << std::endl;
                std::cout << "  Thickness " << Dz << std::endl;
                std::cout << "  h1 (rad leng) " << h1(instance) << std::endl;
                std::cout << "  tl1,bl1 " << Xt1 << " " << Xb1 << std::endl;
                std::cout << "  alpha   " << alpha_t(instance) << std::endl;

                std::cout << " Position absorber x,y,z " << Xcd(instance) << " " << Ycd(instance)
                      << " " << Zcd(instance) << std::endl;
                std::cout << " Rotation along Z " << -alfrot(instance)/deg << std::endl;
                std::cout << " Rotation along Y " << -90 << std::endl;

#endif

		
	        GeoPhysVol* thinPhys = nullptr;
                GeoTrap* thinTrap = new GeoTrap(Dz,0.,0.,h1(instance),tl1,bl1,alpha(instance),
                                                         h1(instance),tl1,bl1,alpha(instance));
                if (!doDetailedAbsorberStraight) {

                  const GeoLogVol* thinLog = new GeoLogVol(thinName,thinTrap,Thin_abs);
                  thinPhys = new GeoPhysVol(thinLog);

                  GeoTrap* thickTrap = new GeoTrap(Dz,0.,0.,h1(instance),Xt1,Xb1,alpha_t(instance),
                                                            h1(instance),Xt1,Xb1,alpha_t(instance));
		  const GeoLogVol* thickLog =  new GeoLogVol(thickName,thickTrap,Thick_abs);
                  GeoPhysVol* thickPhys = new GeoPhysVol(thickLog);
		  // put thick absorber in straight_phys
		  thinPhys->add(new GeoTransform(GeoTrf::TranslateX3D(Xtrans)));
		  thinPhys->add(thickPhys);

#ifdef DEBUGGEO
                  std::cout << " Position thick Abs in Thin at " << Xtrans << std::endl;
#endif
                }

                if (doDetailedAbsorberStraight) {

                   // overall volume => steel
#ifdef DEBUGGEO
                   std::cout << " dz overall absorber volume " << Dz << "  radial length " << h1(instance) <<
                     " lenghts " << tl1 << " " << bl1 << " Angle y axis " << alpha(instance) << std::endl;
#endif
                   const GeoLogVol* thinLog = new GeoLogVol(thinName,thinTrap,myIron);
                   thinPhys = new GeoPhysVol(thinLog);

                   // inside steel put glue, Dz size = Dz size of steel - total thickness of steel/2. covers full eta range

                   double dz_glue = Dz - 0.5*Thfe*Contract;
#ifdef DEBUGGEO
                   std::cout << " dz glue volume in which lead will be put " << dz_glue << std::endl;
#endif
                   GeoTrap* thickTrapGlue = new GeoTrap(dz_glue,0.,0.,h1(instance),tl1,bl1,alpha(instance),
                                                                      h1(instance),tl1,bl1,alpha(instance));
                   std::string thickGlueName = baseName + "ThickAbsGlue::Straight";
                   const GeoLogVol* thickTrapGlueLog = new GeoLogVol(thickGlueName,thickTrapGlue, Glue);
                   GeoPhysVol * thickTrapGluePhys = new GeoPhysVol(thickTrapGlueLog);
                   thinPhys->add(new GeoTransform(GeoTrf::Translate3D(0.,0.,0.)));
                   thinPhys->add(thickTrapGluePhys);

                   // inside glue put lead, Dz size = 0.5*Thpb*Contract, 2 separate volumes for eta<0.8 and eta>0.8
                   double dz_lead_thick=0.5*Thpb*Contract;
#ifdef DEBUGGEO
                   std::cout << " dz thick lead " << dz_lead_thick << " lenghts " << Xt1 << " " << Xb1 << " Angle y axis " << alpha_t(instance) << std::endl;
                   std::cout << " position in overall absorber at X trans " << Xtrans << std::endl;
#endif
                   GeoTrap* thickTrapLead = new GeoTrap(dz_lead_thick,0.,0.,h1(instance),Xt1,Xb1,alpha_t(instance),
                                                                            h1(instance),Xt1,Xb1,alpha_t(instance));
                   std::string thickLeadName= baseName+"ThickAbsLead::Straight";
                   const GeoLogVol* thickTrapLeadLog = new GeoLogVol(thickLeadName,thickTrapLead, myLead);
                   GeoPhysVol * thickTrapLeadPhys = new GeoPhysVol(thickTrapLeadLog);
                   thickTrapGluePhys->add(new GeoTransform(GeoTrf::TranslateX3D(Xtrans)));
                   thickTrapGluePhys->add(thickTrapLeadPhys);

                   double dz_lead_thin = 0.5*Thpb_thin*Contract;
#ifdef DEBUGGEO
                   std::cout << " dz thin lead " << dz_lead_thin << " lenghts " << Xt2 << " " << Xb2 << " Angle y axis " << alpha_2(instance) << std::endl;
                   std::cout << " position in overall absorber at X trans " << Xtrans2 << std::endl;
#endif
                   GeoTrap* thinTrapLead = new GeoTrap(dz_lead_thin,0.,0.,h1(instance),Xt2,Xb2,alpha_2(instance),
                                                                          h1(instance),Xt2,Xb2,alpha_2(instance));
                   std::string thinLeadName = baseName+"ThinAbsLead::Straight";
                   const GeoLogVol* thinTrapLeadLog = new GeoLogVol(thinLeadName,thinTrapLead, myLead);
                   GeoPhysVol * thinTrapLeadPhys = new GeoPhysVol(thinTrapLeadLog);
                   thickTrapGluePhys->add(new GeoTransform(GeoTrf::TranslateX3D(Xtrans2)));
                   thickTrapGluePhys->add(thinTrapLeadPhys);

                    
                }
		
		//------------------------------------------------------------------------
		// JFB.  If there is no sagging we can exit this loop after only one pass..
		// We simply declare the placement recipe, and place the first phys Vol 
		// everywhere.
		//------------------------------------------------------------------------
		if (m_A_SAGGING) {
		  if (!gStraightAbsorbers) gStraightAbsorbers = new GeoStraightAccSection();
		  gStraightAbsorbers->XCent(instance,irl)=TX(instance)(0,3); //dx
		  gStraightAbsorbers->YCent(instance,irl)=TX(instance)(1,3); //dy
		  gStraightAbsorbers->Cosu(instance,irl)    =-(TX(instance)(0,1)); //xy
		  gStraightAbsorbers->Sinu(instance,irl)    = (TX(instance)(0,2)); //xz
		  gStraightAbsorbers->HalfLength(instance,irl) = thinTrap->getDydzn();

		  stacPhysical->add(new GeoTransform(TX(instance)));
                  int icopytot=1000000*idivi+10000*irl+instance;
                  stacPhysical->add(new GeoIdentifierTag(icopytot));
		  stacPhysical->add(thinPhys);
		}
		else {
		  if (!gStraightAbsorbers) gStraightAbsorbers = new GeoStraightAccSection();
		  gStraightAbsorbers->setTransform (irl,TX);
		  gStraightAbsorbers->setHalfLength(irl,thinTrap->getDydzn());
		  GeoSerialTransformer *st = new GeoSerialTransformer(thinPhys, &TX, Nabsorber); 
		  stacPhysical->add(new GeoSerialIdentifier(irl*10000+idivi*1000000));
		  stacPhysical->add(st);
		  break;
		}
	      }    // loop over instances


              if (idivi==0) {
// Folds 
                double phi0_safety=0.;
                if (irl==0) phi0_safety=0.003;
  
// for most section, one fold = fold at the beginning of this section
// for last section, should also to last fold, at the end of the section

                int irl1=irl;
                int irl2 = (irl==Nbrt-1) ? irl+1 : irl;

                for (int jrl=irl1; jrl<=irl2; jrl++) {

// get slant angle for the previous zig-zag
                 int iirl=jrl-1;
                 if (iirl<0) iirl=1;
                 GeoGenfun::GENFUNCTION x0a = Fx(iirl, Gama, Cenx, Ceny)
                                           +deltay[iirl]*Del1(Gama)
                                           +deltax[iirl]*Del2(Gama);
                 GeoGenfun::GENFUNCTION y0a = Fy(iirl, Gama, Cenx, Ceny)
                                           -deltay[iirl]*Del2(Gama)
                                           +deltax[iirl]*Del1(Gama);
                 GeoGenfun::GENFUNCTION dx0 = x1a - x0a;
                 GeoGenfun::GENFUNCTION dy0 = y1a - y0a;
              // Da the two fold centers distance, da straight part length
                 GeoGenfun::GENFUNCTION Da0 = Sqrt ( dx0*dx0 + dy0*dy0 );
                 GeoGenfun::GENFUNCTION da0 = Sqrt ( (Da0 - 2.*Rint)*(Da0 + 2.*Rint) );
              // newalpha (slant angle) value of the rotation angle around Z_axis
                 GeoGenfun::GENFUNCTION cosalfa0 = (da0*dx0 +iparit*2.*Rint*dy0)/Da0/Da0;
                 GeoGenfun::GENFUNCTION sinalfa0 = (da0*dy0 -iparit*2.*Rint*dx0)/Da0/Da0;
                 GeoGenfun::GENFUNCTION alpha_prev = ATan2(sinalfa0,cosalfa0);

#ifdef DEBUGGEO
                if (jrl>0 && jrl<Nbrt) {
                 std::cout << "x1,y1,x0,y0 " << x1a(0) << " " << y1a(0) << " "
                           << x0a(0) << " " << y0a(0) << std::endl;
                 std::cout << " newalpha, alpha_prev " << newalpha(0) << " "
                           << alpha_prev(0) << std::endl;
                }
#endif
                GeoGenfun::Mod Mod2Pi(2*M_PI);
// down folds  (add +M_PI and then rotate by -M_PI to follow same logic as old code)
                 GeoGenfun::GENFUNCTION phi0_dfold_0 = 
                  GeoGenfun::FixedConstant(M_PI/2.+phi0_safety);
                 GeoGenfun::GENFUNCTION dphi_dfold_0 = Mod2Pi(newalpha-phi0_safety - Gama);
                 GeoGenfun::GENFUNCTION phi0_dfold_1 = Mod2Pi(M_PI/2.+ alpha_prev - Gama);
                 GeoGenfun::GENFUNCTION dphi_dfold_1  = Mod2Pi(newalpha-alpha_prev);
                 GeoGenfun::GENFUNCTION phi0_dfold_2 =  Mod2Pi(M_PI/2.+ newalpha - Gama);
                 GeoGenfun::GENFUNCTION dphi_dfold_2 = Mod2Pi(- newalpha + Gama);
// up folds
                 GeoGenfun::GENFUNCTION phi0_ufold_0 =
                  Mod2Pi(M_PI/2.+newalpha-Gama);
                 GeoGenfun::GENFUNCTION dphi_ufold_0 = Mod2Pi(-newalpha+Gama-phi0_safety);
                 GeoGenfun::GENFUNCTION phi0_ufold_1 = Mod2Pi(M_PI/2. + newalpha - Gama);
                 GeoGenfun::GENFUNCTION dphi_ufold_1 =  Mod2Pi(alpha_prev - newalpha);
                 GeoGenfun::GENFUNCTION phi0_ufold_2 = GeoGenfun::FixedConstant(M_PI/2.);
                 GeoGenfun::GENFUNCTION dphi_ufold_2 = Mod2Pi(newalpha-Gama);

                 const GeoGenfun::AbsFunction* phi0_fold=0;
                 const GeoGenfun::AbsFunction* dphi_fold=0;
                 const GeoXF::Function* TXfold=0;

                 std::string thinName;
                 std::string thickName;
                 if (jrl%2==checkParity) {
                    thinName     = baseName + "ThinAbs::CornerDownFold";
                    thickName    = baseName + "ThickAbs::CornerDownFold";
                 } else {
                    thinName     = baseName + "ThinAbs::CornerUpFold";
                    thickName    = baseName + "ThickAbs::CornerUpFold";
                 }

// radius of tubs for folds
                 double Rcmin=Rint-Thce/2.;
                 double Rcmax=Rint+Thce/2.;

// GU 09/06/2004 add some safety in z size
                 double ddz0  = dz0-safety_zlen;
                 double ddz01 = dz01-safety_zlen;
                 if (jrl==Nbrt){
                    ddz0=dza-safety_zlen;
                    ddz01=dza1-safety_zlen;
                 } 


                 GeoGenfun::GENFUNCTION zpos     = GeoGenfun::FixedConstant(Zmin+dz0);
                 double phirot=0;
                 if (jrl%2==checkParity) phirot = -M_PI;
                 GeoXF::TRANSFUNCTION TXfold1=
                   GeoXF::Pow(GeoTrf::TranslateX3D(1.0),x1a) *
                   GeoXF::Pow(GeoTrf::TranslateY3D(1.0),y1a) *
                   GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),zpos) *
                   GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Gama+phirot);
                 GeoXF::TRANSFUNCTION TXfold2 =
                   GeoXF::Pow(GeoTrf::TranslateX3D(1.0),x2a) *
                   GeoXF::Pow(GeoTrf::TranslateY3D(1.0),y2a) *
                   GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),zpos) *
                   GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Gama+phirot);

// first fown fold
                 if (jrl==0 && checkParity==0) {
                    phi0_fold = &(phi0_dfold_0);
                    dphi_fold = &(dphi_dfold_0);
                    TXfold = &(TXfold1);
                 }
// normal fold
                 if (jrl%2==checkParity && jrl >0 && jrl<Nbrt) {
                    phi0_fold = &(phi0_dfold_1);
                    dphi_fold = &(dphi_dfold_1);
                    TXfold = &(TXfold1);
                 }
// last fold if down
                 if (jrl%2 ==checkParity && jrl == Nbrt) {
                    phi0_fold = &(phi0_dfold_2);
                    dphi_fold = &(dphi_dfold_2);
                    TXfold = &(TXfold2);
                 }
// first up fold
                 if (jrl==0 && checkParity==1) {
                    phi0_fold = &(phi0_ufold_0);
                    dphi_fold = &(dphi_ufold_0);
                    TXfold = &(TXfold1);
                 }
// up fold
                 if (jrl%2 ==(1-checkParity) && jrl>0 && jrl < Nbrt) {
                    phi0_fold = &(phi0_ufold_1);
                    dphi_fold = &(dphi_ufold_1);
                    TXfold = &(TXfold1);
                 }
// last fold if up
                 if (jrl%2==(1-checkParity) && jrl==Nbrt) {
                    phi0_fold = &(phi0_ufold_2);
                    dphi_fold = &(dphi_ufold_2);
                    TXfold = &(TXfold2);
                 }

                 if (!phi0_fold || !dphi_fold || !TXfold) {
                   log << MSG::INFO << " LArGeoBarrel::BarrelConstruction  fold not defined..." << endmsg;
                 }
                 else
                 for (int instance = 0; instance < Nabsorber; instance++)
                 {

                  GeoPhysVol* thinPhys = nullptr;

                  if (!doDetailedAbsorberFold) {
                     GeoTubs* thinTubs        = new GeoTubs(Rcmin,Rcmax,ddz0,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
                     GeoTubs* thickTubs       = new GeoTubs(Rcmin,Rcmax,ddz01,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
#ifdef DEBUGGEO
                     std::cout << " Thin Abs Corner  (tubs) " << std::endl;
                     std::cout << " Rmin,Rmax,dZ,phi0,Dphi[rad] " << Rcmin << " " << Rcmax << " "
                       << ddz0 << " " << (*phi0_fold)(instance) << " "
                       <<(*dphi_fold)(instance) << std::endl;
                     std::cout << " Thick Abs Corner (tubs) " << std::endl;
                     std::cout << " Rmin,Rmax,dZ,phi0,Dphi[rad] " << Rcmin << " " << Rcmax << " "
                        << ddz01 << " " << (*phi0_fold)(instance) << " "
                        << (*dphi_fold)(instance) << std::endl;
#endif
                     const GeoLogVol* thinLog  = new GeoLogVol(thinName,thinTubs,Thin_abs);
                     const GeoLogVol* thickLog = new GeoLogVol(thickName,thickTubs,Thick_abs);

                     thinPhys   = new GeoPhysVol(thinLog);
                     GeoPhysVol* thickPhys  = new GeoPhysVol(thickLog);

                     thinPhys->add(new GeoTransform(GeoTrf::TranslateZ3D(ddz01-ddz0)));
                     thinPhys->add(thickPhys);
#ifdef DEBUGGEO
                  std::cout << "  Position Thick fold in Thin  Z = " << ddz01-ddz0 << std::endl;
#endif

                  }

                  if (doDetailedAbsorberFold) {

                     // bigger volume in steel to cover outer edges of absorber
                     GeoTubs* thinTubs        = new GeoTubs(Rcmin,Rcmax,ddz0,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
                     const GeoLogVol* thinLog  = new GeoLogVol(thinName,thinTubs,myIron);
                     thinPhys   = new GeoPhysVol(thinLog);

#ifdef DEBUGGEO
                     std::cout << " overall fold volume rmin,rmax,dz " << Rcmin << " " << Rcmax << " " << ddz0 << std::endl;
#endif

                     // put inside volume with glue
                     GeoTubs* glueTubs = new GeoTubs(Rcmin+0.5*Thfe*Contract,Rcmax-0.5*Thfe*Contract,ddz0,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
                     std::string foldGlueName = baseName+"Glue::Fold";
                     const GeoLogVol* glueTubsLog = new GeoLogVol(foldGlueName,glueTubs,Glue);
                     GeoPhysVol* glueTubsPhys = new GeoPhysVol(glueTubsLog);
                     thinPhys->add(new GeoTransform(GeoTrf::TranslateZ3D(0.)));
                     thinPhys->add(glueTubsPhys); 
#ifdef DEBUGGEO
                     std::cout << "  glue fold volume " << Rcmin+0.5*Thfe*Contract << " " << Rcmax-0.5*Thfe*Contract << " " << ddz0 << std::endl;
#endif

                     // put inside glue, volumes for lead, 2 thickness => 2 volumes

                     // thick lead
                     GeoTubs* thickLeadTubs = new GeoTubs(Rint-0.5*Thpb*Contract,Rint+0.5*Thpb*Contract,ddz01,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
                     std::string foldThickLeadName = baseName+"ThickLead::Fold";
                     const GeoLogVol* thickLeadLog = new GeoLogVol(foldThickLeadName,thickLeadTubs,myLead);
                     GeoPhysVol* thickLeadPhys = new GeoPhysVol(thickLeadLog);
                     glueTubsPhys->add(new GeoTransform(GeoTrf::TranslateZ3D(ddz01-ddz0)));
                     glueTubsPhys->add(thickLeadPhys);
#ifdef DEBUGGEO
                     std::cout << " thick lead volume " << Rint-Thpb*Contract << " " << Rint+Thpb*Contract << " " << ddz01 << std::endl;
                     std::cout << "   put in glue fold volume with z translation " << ddz01-ddz0 << std::endl;
#endif

                     // thin lead
                     GeoTubs* thinLeadTubs = new GeoTubs(Rint-0.5*Thpb_thin*Contract,Rint+0.5*Thpb_thin*Contract,ddz0-ddz01,
                                           (*phi0_fold)(instance),(*dphi_fold)(instance));
                     std::string foldThinLeadName = baseName+"ThinLead::Fold";
                     const GeoLogVol* thinLeadLog = new GeoLogVol(foldThinLeadName,thinLeadTubs,myLead);
                     GeoPhysVol* thinLeadPhys = new GeoPhysVol(thinLeadLog);
                     glueTubsPhys->add(new GeoTransform(GeoTrf::TranslateZ3D(ddz01))); 
                     glueTubsPhys->add(thinLeadPhys);

#ifdef DEBUGGEO
                     std::cout << " thin lead volume " << Rint-Thpb_thin*Contract << " " << Rint+Thpb_thin*Contract << " " << ddz01 << std::endl;
                     std::cout << "   put in glue fold volume with z translation " << ddz01 << std::endl;
#endif


                   
                  }

#ifdef DEBUGGEO
                  if (jrl !=Nbrt) {
                  std::cout << " Position absorber fold x,y,z,rotation/z " << x1a(instance) <<
                     " " << y1a(instance) << " " << zpos(instance) << " " 
                         <<  Gama(instance) << std::endl;
                  } else {
                  std::cout << " Position absorber fold x,y,z,rotation/z " << x2a(instance) <<
                     " " << y2a(instance) << " " << zpos(instance) << " " 
                         <<  Gama(instance) << std::endl;
                  }
#endif
                  if (m_A_SAGGING) {
                      stacPhysical->add(new GeoTransform((*TXfold)(instance)));
                      int icopytot = 10000*jrl+instance;
                      stacPhysical->add(new GeoIdentifierTag(icopytot));
                      stacPhysical->add(thinPhys);
                  }
                  else {
                     GeoSerialTransformer *st = new GeoSerialTransformer(thinPhys, TXfold, Nabsorber);
                     stacPhysical->add(new GeoSerialIdentifier(jrl*10000));
                     stacPhysical->add(st);
                     break;
                  }

                 }    // loop over absorber folds instance
                }     // loop over jrl

              } // idivi==0
	    }   // end absorbers

// ========================  Electrodes:
	    {
	   
	      // thickness of electrode
	      double Dze = Thel/2.;
	   
	      // For electrodes
	      GeoGenfun::GENFUNCTION x1e = Fx(irl+0., Game, Cenx, Ceny)
                                           +deltay[irl]*Del1(Game)
                                           +deltax[irl]*Del2(Game);
	      GeoGenfun::GENFUNCTION x2e = Fx(irl+1., Game, Cenx, Ceny)
                                           +deltay[irl+1]*Del1(Game)
                                           +deltax[irl+1]*Del2(Game);
	      GeoGenfun::GENFUNCTION y1e = Fy(irl+0., Game, Cenx, Ceny) 
                                           -deltay[irl]*Del2(Game)
                                           +deltax[irl]*Del1(Game);
	      GeoGenfun::GENFUNCTION y2e = Fy(irl+1., Game, Cenx, Ceny)
                                           -deltay[irl+1]*Del2(Game)
                                           +deltax[irl+1]*Del1(Game);
	      GeoGenfun::GENFUNCTION dxe = x2e - x1e;
	      GeoGenfun::GENFUNCTION dye = y2e - y1e;
	    // De the two fold centers distance, de straight part length
	      GeoGenfun::GENFUNCTION De = Sqrt ( dxe*dxe + dye*dye );
	      GeoGenfun::GENFUNCTION de = Sqrt ( (De - 2.*Rint)*(De + 2.*Rint) );
	   
	    //newalphe (slant angle) value of the rotation angle around Z_axis
	      GeoGenfun::GENFUNCTION cosalfae = (de*dxe -iparit*2.*Rint*dye)/De/De;
	      GeoGenfun::GENFUNCTION sinalfae = (de*dye +iparit*2.*Rint*dxe)/De/De;
	      GeoGenfun::GENFUNCTION newalphe = ATan2(sinalfae,cosalfae);
	   
	   
	    // newalphae is already computed angle wrt z axis
	    // P/2 rotation is to get absorber aligned along local x axis
	    // instead of y, then rotate with angle newalpha
	      GeoGenfun::GENFUNCTION alfrote = -M_PI/2. - newalphe;
	   
	      GeoGenfun::GENFUNCTION Xcde    = (x1e + x2e)/2.+ (2.*idivi-1.)*(1.-frac)*de/2.*cosalfae;
	      GeoGenfun::GENFUNCTION Ycde    = (y1e + y2e)/2.+ (2.*idivi-1.)*(1.-frac)*de/2.*sinalfae;
	      GeoGenfun::GENFUNCTION Zcde       = GeoGenfun::FixedConstant(Zmin+(tl1+bl1)/2.+safety_zlen);
	   
	   
	      GeoGenfun::GENFUNCTION h1e      = de/2.*frac - .007*Gaudi::Units::mm;
	      GeoGenfun::GENFUNCTION alpha_e  = ATan(0.5*(bl1-tl1)/h1e); 
	   
	   
	      GeoXF::TRANSFUNCTION TXE = 
	        GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcde) *
	        GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycde) *
	        GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcde) * 
	        GeoXF::Pow(GeoTrf::RotateZ3D(1.0),-alfrote)*
	        GeoTrf::RotateY3D(-90*Gaudi::Units::deg);                    
	   
	   
	      for (int instance = 0; instance < Nelectrode; instance++) 
	      {
		std::string name = baseName + "Electrode::Straight";
#ifdef DEBUGGEO
                std::cout << " *** Electrode trap parameters " << std::endl;
                std::cout << "  Thickness " << Dze << std::endl; 
                std::cout << "  h1 (rad leng) " << h1e(instance) << std::endl;
                std::cout << "  tl1,tb1 " << tl1 << " " << bl1 << std::endl;
                std::cout << "  alpha   " << alpha_e(instance) << std::endl;
                std::cout << " Position electrode x,y,z " << Xcde(instance) << " " << Ycde(instance)
                      << " " << Zcde(instance) << std::endl;
                std::cout << " Rotation along Z " << -alfrote(instance)/deg << std::endl;
                std::cout << " Rotation along Y " << -90 << std::endl;

#endif

		GeoTrap* trap = new GeoTrap(Dze,0.,0.,h1e(instance),tl1,bl1,alpha_e(instance),
                                                      h1e(instance),tl1,bl1,alpha_e(instance));
		const GeoLogVol* logVol = new GeoLogVol(name,trap,Kapton_Cu);
		GeoPhysVol* physVol = new GeoPhysVol(logVol);
		//------------------------------------------------------------------------
		// JFB.  If there is no sagging we can exit this loop after only one pass..
		// We simply declare the placement recipe, and place the first phys Vol 
		// everywhere. 
		//------------------------------------------------------------------------
		if (m_A_SAGGING) {
		  if (!gStraightElectrodes) gStraightElectrodes = new GeoStraightAccSection();
		  gStraightElectrodes->XCent(instance,irl)=TXE(instance)(0,3); //dx
		  gStraightElectrodes->YCent(instance,irl)=TXE(instance)(1,3); //dy
		  gStraightElectrodes->Cosu(instance,irl)    =-(TXE(instance)(0,1)); //xy
		  gStraightElectrodes->Sinu(instance,irl)    = (TXE(instance)(0,2)); //xz
		  gStraightElectrodes->HalfLength(instance,irl) = trap->getDydzn();

		  stacPhysical->add(new GeoTransform(TXE(instance)));
                  int icopytot=1000000*idivi+10000*irl+instance;
                  stacPhysical->add(new GeoIdentifierTag(icopytot));
		  stacPhysical->add(physVol);
		}
		else {
		  if (!gStraightElectrodes) gStraightElectrodes = new GeoStraightAccSection();
		  gStraightElectrodes->setTransform (irl,TXE);
		  gStraightElectrodes->setHalfLength(irl,trap->getDydzn());
		  GeoSerialTransformer *st = new GeoSerialTransformer(physVol, &TXE, Nelectrode); 
		  stacPhysical->add(new GeoSerialIdentifier(irl*10000+idivi*1000000));
		  stacPhysical->add(st);
		  break;
		}
	      }   // loop over instances



              if (idivi==0) {
// Folds 
                double phi0_safety=0.;
                if (irl==0) phi0_safety=0.003;
  
// for most section, one fold = fold at the beginning of this section
// for last section, should also to last fold, at the end of the section

                int irl1=irl;
                int irl2 = (irl==Nbrt-1) ? irl+1 : irl;

                for (int jrl=irl1; jrl<=irl2; jrl++) {

// get slant angle for the previous zig-zag
                 int iirl=jrl-1;
                 if (iirl<0) iirl=1;
                 GeoGenfun::GENFUNCTION x0e = Fx(iirl, Game, Cenx, Ceny)
                                           +deltay[iirl]*Del1(Game)
                                           +deltax[iirl]*Del2(Game);
                 GeoGenfun::GENFUNCTION y0e = Fy(iirl, Game, Cenx, Ceny)
                                           -deltay[iirl]*Del2(Game)
                                           +deltax[iirl]*Del1(Game);
                 GeoGenfun::GENFUNCTION dx0 = x1e - x0e;
                 GeoGenfun::GENFUNCTION dy0 = y1e - y0e;
              // Da the two fold centers distance, da straight part length
                 GeoGenfun::GENFUNCTION Da0 = Sqrt ( dx0*dx0 + dy0*dy0 );
                 GeoGenfun::GENFUNCTION da0 = Sqrt ( (Da0 - 2.*Rint)*(Da0 + 2.*Rint) );
              // newalpha (slant angle) value of the rotation angle around Z_axis
                 GeoGenfun::GENFUNCTION cosalfa0 = (da0*dx0 +iparit*2.*Rint*dy0)/Da0/Da0;
                 GeoGenfun::GENFUNCTION sinalfa0 = (da0*dy0 -iparit*2.*Rint*dx0)/Da0/Da0;
                 GeoGenfun::GENFUNCTION alphe_prev = ATan2(sinalfa0,cosalfa0);

#ifdef DEBUGGEO
                if (jrl>0 && jrl<Nbrt) {
                 std::cout << " newalphae, alphae_prev " << newalphe(0) << " "
                           << alphe_prev(0) << std::endl;
                }
#endif

// down folds (add +M_PI and then rotate by -M_PI to follow same logic as old code)
                 GeoGenfun::Mod Mod2Pi(2*M_PI);
                 GeoGenfun::GENFUNCTION phi0_dfold_0 = 
                                       GeoGenfun::FixedConstant(M_PI/2.+phi0_safety);
                 GeoGenfun::GENFUNCTION dphi_dfold_0 = Mod2Pi(newalphe-phi0_safety-Game);
                 GeoGenfun::GENFUNCTION phi0_dfold_1 = Mod2Pi(M_PI/2.+ alphe_prev - Game);
                 GeoGenfun::GENFUNCTION dphi_dfold_1  = Mod2Pi(newalphe-alphe_prev);
                 GeoGenfun::GENFUNCTION phi0_dfold_2 =  Mod2Pi(M_PI/2.+ newalphe - Game);
                 GeoGenfun::GENFUNCTION dphi_dfold_2 = Mod2Pi(- newalphe + Game);  
// up folds
                 GeoGenfun::GENFUNCTION phi0_ufold_0 =
                  Mod2Pi(M_PI/2.+newalphe-Game);
                 GeoGenfun::GENFUNCTION dphi_ufold_0 = Mod2Pi(-newalphe+Game-phi0_safety);
                 GeoGenfun::GENFUNCTION phi0_ufold_1 = Mod2Pi(M_PI/2. + newalphe - Game);
                 GeoGenfun::GENFUNCTION dphi_ufold_1 =  Mod2Pi(alphe_prev - newalphe);
                 GeoGenfun::GENFUNCTION phi0_ufold_2 = GeoGenfun::FixedConstant(M_PI/2.);
                 GeoGenfun::GENFUNCTION dphi_ufold_2 = Mod2Pi(newalphe - Game);

                 const GeoGenfun::AbsFunction* phi0_fold=0;
                 const GeoGenfun::AbsFunction* dphi_fold=0;
                 const GeoXF::Function* TXfold=0;

                 std::string eName;
                 if (jrl%2==checkParity) {
                    eName     = baseName + "Electrode::CornerDownFold";
                 } else {
                    eName     = baseName + "Electrode::CornerUpFold";
                 }

// radius of tubs for folds
                 double Rcmine=Rint-Thel/2.;
                 double Rcmaxe=Rint+Thel/2.;

// GU 09/06/2004 add some safety in z size
                 double ddz0  = dz0-safety_zlen;
                 if (jrl==Nbrt) {
                   ddz0 = dza - safety_zlen;
                 }

                 GeoGenfun::GENFUNCTION zpos     = GeoGenfun::FixedConstant(Zmin+dz0);
                 double phirot = 0;
                 if (jrl%2==checkParity)  phirot = -M_PI;
                 GeoXF::TRANSFUNCTION TXfold1=
                   GeoXF::Pow(GeoTrf::TranslateX3D(1.0),x1e) *
                   GeoXF::Pow(GeoTrf::TranslateY3D(1.0),y1e) *
                   GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),zpos) *
                   GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Game+phirot);
                 GeoXF::TRANSFUNCTION TXfold2 =
                   GeoXF::Pow(GeoTrf::TranslateX3D(1.0),x2e) *
                   GeoXF::Pow(GeoTrf::TranslateY3D(1.0),y2e) *
                   GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),zpos) *
                   GeoXF::Pow(GeoTrf::RotateZ3D(1.0),Game+phirot);

// first fown fold
                 if (jrl==0 && checkParity==0) {
                    phi0_fold = &(phi0_dfold_0);
                    dphi_fold = &(dphi_dfold_0);
                    TXfold = &(TXfold1);
                 }
// normal fold
                 if (jrl%2==checkParity && jrl >0 && jrl<Nbrt) {
                    phi0_fold = &(phi0_dfold_1);
                    dphi_fold = &(dphi_dfold_1);
                    TXfold = &(TXfold1);
                 }
// last fold if down
                 if (jrl%2 ==checkParity && jrl == Nbrt) {
                    phi0_fold = &(phi0_dfold_2);
                    dphi_fold = &(dphi_dfold_2);
                    TXfold = &(TXfold2);
                 }
// first up fold
                 if (jrl==0 && checkParity==1 ) {
                    phi0_fold = &(phi0_ufold_0);
                    dphi_fold = &(dphi_ufold_0);
                    TXfold = &(TXfold1);

                 }
// up fold
                 if (jrl%2 ==(1-checkParity) && jrl>0 && jrl < Nbrt) {
                    phi0_fold = &(phi0_ufold_1);
                    dphi_fold = &(dphi_ufold_1);
                    TXfold = &(TXfold1);
                 }
// last fold if up
                 if (jrl%2==(1-checkParity) && jrl==Nbrt) {
                    phi0_fold = &(phi0_ufold_2);
                    dphi_fold = &(dphi_ufold_2);
                    TXfold = &(TXfold2);
                 }

                 if (!phi0_fold || !dphi_fold || !TXfold) {
                   log << MSG::INFO << " LArGeoBarrel::BarrelConstruction  fold not defined..." << endmsg;
                 }
                 else
                 for (int instance = 0; instance < Nelectrode; instance++)
                 {

                  GeoTubs* foldeTubs       = new GeoTubs(Rcmine,Rcmaxe,ddz0,
                                         (*phi0_fold)(instance),(*dphi_fold)(instance));
#ifdef DEBUGGEO
                  std::cout << " Electrode Corner  (tubs) " << std::endl;
                  std::cout << " Rmin,Rmax,dZ,phi0,Dphi[rad] " << Rcmine << " " << Rcmaxe << " "
                    << ddz0 << " " << (*phi0_fold)(instance) << " "
                    <<(*dphi_fold)(instance) << std::endl;
#endif
                  const GeoLogVol* foldeLog  = new GeoLogVol(eName,foldeTubs,Kapton_Cu);

                  GeoPhysVol* foldePhys   = new GeoPhysVol(foldeLog);

#ifdef DEBUGGEO
                  if (jrl!=Nbrt) {
                  std::cout << " Position electrode fold x,y,z,rotation/z " << x1e(instance) <<
                     " " << y1e(instance) << " " << zpos(instance) << " " 
                         << Game(instance) << std::endl;
                  } else {
                  std::cout << " Position electrode fold x,y,z,rotation/z " << x2e(instance) <<
                     " " << y2e(instance) << " " << zpos(instance) << " " 
                         << Game(instance) << std::endl;
                  }
#endif
                  if (m_A_SAGGING) {
                      stacPhysical->add(new GeoTransform((*TXfold)(instance)));
                      int icopytot = 10000*jrl+instance;
                      stacPhysical->add(new GeoIdentifierTag(icopytot));
                      stacPhysical->add(foldePhys);
                  }
                  else {
                     GeoSerialTransformer *st = new GeoSerialTransformer(foldePhys, TXfold, Nelectrode);
                     stacPhysical->add(new GeoSerialIdentifier(jrl*10000));
                     stacPhysical->add(st);
                     break;
                  }

                 }    // loop over electrode folds instance
                }     // loop over jrl

              } // idivi==0
	    }    // end electrodes
        }        // end loop over ndivi
      }         //  end loop over irl (zig-zag)
    {
      StatusCode status = detStore->record(gStraightElectrodes,"STRAIGHTELECTRODES");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store STRAIGHTELECTRODES structure");
    }
    {
      StatusCode status = detStore->record(gStraightAbsorbers,"STRAIGHTABSORBERS");
      if(!status.isSuccess()) throw std::runtime_error ("Cannot store STRAIGHTABSORBERS structure");
    }
  }

#endif  //  BUILD_ACCORDION_PLATES
  
  log << MSG::DEBUG << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "+         END   of Barrel EM GeoModel definition   +" << std::endl;
  log << "+                                                  +" << std::endl;
  log << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  
  
}

// Generic Function Versions

GeoGenfun::FunctionNoop LArGeo::BarrelConstruction::Fx(double r, GeoGenfun::GENFUNCTION G, const double Cenx[], const double Ceny[] ) const
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
  GeoGenfun::GENFUNCTION result =  (Cos(G)*(Cenx[i]+Cenx[j])/2-Sin(G)*(Ceny[i]+Ceny[j])/2) ;
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::BarrelConstruction::Fy(double r, GeoGenfun::GENFUNCTION G, const double Cenx[], const double Ceny[] ) const
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
  GeoGenfun::GENFUNCTION result = (Sin(G)*(Cenx[i]+Cenx[j])/2+Cos(G)*(Ceny[i]+Ceny[j])/2) ;
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::BarrelConstruction::Del1(GeoGenfun::GENFUNCTION G) const
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  GeoGenfun::GENFUNCTION result = (Cos(  G ) * Sin( G ) );
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::BarrelConstruction::Del2(GeoGenfun::GENFUNCTION G) const
{
  GeoGenfun::Cos Cos;
  GeoGenfun::GENFUNCTION result = (Cos(  G ) * Cos( G ) );
  return GeoGenfun::FunctionNoop(&result);
}


GeoGenfun::FunctionNoop LArGeo::BarrelConstruction::ATan2(GeoGenfun::GENFUNCTION y, GeoGenfun::GENFUNCTION x) const {

  // Manufacture a Theta Function:
  GeoGenfun::Rectangular Theta;
  Theta.x0().setValue(0.0);
  Theta.x1().setValue(DBL_MAX);
  Theta.baseline().setValue(0.0);
  Theta.height().setValue(1.0);

  // Manufacture an ATan function:
  GeoGenfun::ATan ATan;
  

  // Manufacture a Mod function, putting this on the range (0-2PI)
  GeoGenfun::Mod Mod2Pi(2*M_PI);

  // Now take ATan if x is positive 
  
  GeoGenfun::GENFUNCTION result = Theta(x)*ATan(y/x) + Theta(-x)*(Mod2Pi(ATan(y/x)+M_PI)); 
  return GeoGenfun::FunctionNoop(&result);

}


void LArGeo::BarrelConstruction::printParams()
{
 std::cout << "******** Print out of LArBarrel Accordion parameters " << std::endl;
  std::cout << "Absorber parameters: " <<std::endl;
  std::cout << "Glue for ThinAbs        " << 
       m_parameters->GetValue("LArEMBThinAbsGlue") << std::endl;
  std::cout << "Iron for ThinAbs        " << 
       m_parameters->GetValue("LArEMBThinAbsIron") << std::endl;
  std::cout << "Lead for ThinAbs        " << 
        m_parameters->GetValue("LArEMBThinAbsLead") << std::endl;
  std::cout << "Glue for ThickAbs       " << 
        m_parameters->GetValue("LArEMBThickAbsGlue") << std::endl;
  std::cout << "Iron for ThickAbs       " << 
       m_parameters->GetValue("LArEMBThickAbsIron") << std::endl;
  std::cout << "Lead for ThickAbs       " 
        << m_parameters->GetValue("LArEMBThickAbsLead") << std::endl;

  std::cout << "Electrode parameters: " << std::endl;
  std::cout << "Copper thickness        "  <<
        m_parameters->GetValue("LArEMBThickElecCopper") << std::endl;
  std::cout << "Kapton thickness        " <<  
         m_parameters->GetValue("LArEMBThickElecKapton") << std::endl;

  std::cout << "Accordion geometry description " << std::endl;
  std::cout << "phi first absorber      " << 
     m_parameters->GetValue("LArEMBAbsPhiFirst") << std::endl;
  std::cout << "zmin of absorber        " << 
     m_parameters->GetValue("LArEMBMotherZmin") << std::endl;
  std::cout << "zmax of absorber        " << 
    m_parameters->GetValue("LArEMBMotherZmax") << std::endl;
  std::cout << "Max eta                 " << 
    m_parameters->GetValue("LArEMBMaxEtaAcceptance") << std::endl;
  std::cout << "Nominal eta lead change " << 
    m_parameters->GetValue("LArEMBThickEtaAcceptance") << std::endl;
  std::cout << "Number of abs in phi    " << 
    m_parameters->GetValue("LArEMBnoOFPhysPhiCell") << std::endl;
  std::cout << "Dphi between absorbers  "  << 
    m_parameters->GetValue("LArEMBPhiGapAperture") << std::endl;
  std::cout << "Rmin Mother             " << 
    m_parameters->GetValue("LArEMBMotherRmin") << std::endl;
  std::cout << "Rmax Mother             " << 
     m_parameters->GetValue("LArEMBMotherRmax") << std::endl;
  std::cout << "Rmin at zmax            "  <<
       m_parameters->GetValue("LArEMBRminHighZ") << std::endl;
  std::cout << "Phimax barrel           " << 
       m_parameters->GetValue("LArEMBphiMaxBarrel") << std::endl;
  std::cout << "Number of zigs          " <<  
     m_parameters->GetValue("LArEMBnoOFAccZigs") << std::endl;
  std::cout << "Fold Gaudi::Units::rad of curvature   " << 
     m_parameters->GetValue("LArEMBNeutFiberRadius") << std::endl;
  for (int i=0;i<15;i++) {
    std::cout << "Fold " << i << " radius " <<  
      m_parameters->GetValue("LArEMBRadiusAtCurvature",i) << std::endl;
  }
  for (int i=0;i<15;i++) {
    std::cout << "Fold " << i << " phi0   " <<  
      m_parameters->GetValue("LArEMBPhiAtCurvature",i) << std::endl;
  }
  for (int i=0;i<15;i++) {
    std::cout << "Fold " << i << " eta trans   " <<  
      m_parameters->GetValue("LArEMBEtaTrans",i) << std::endl;
  }
  for (int i=0;i<14;i++) {
    std::cout << "zig " << i << " angle   " <<  
      m_parameters->GetValue("LArEMBDeltaZigAngle",i) << std::endl;
  }

  std::cout << "Fiducial active region definition " << std::endl;
  std::cout << "Zmin                    " << 
       m_parameters->GetValue("LArEMBfiducialMothZmin") << std::endl;
  std::cout << "Zmax                    " << 
     m_parameters->GetValue("LArEMBfiducialMothZmax") << std::endl;
  std::cout << "Rmin                    " <<
     m_parameters->GetValue("LArEMBRadiusInnerAccordion") << std::endl;
  std::cout << "Rmax                    "  <<  
    m_parameters->GetValue("LArEMBFiducialRmax") << std::endl;
  std::cout << "Inactive DR S1 to S2    " << 
    m_parameters->GetValue("LArEMBDeltaRS12") << std::endl;

  std::cout << "Description of matter from PS to strip " << std::endl;
  std::cout << "DeltaR for LAR+Cable+MotherBoard volume " << 
       m_parameters->GetValue("LArEMBInnerElectronics") << std::endl;
  std::cout << "Number of cable volumes " << 
      m_parameters->GetValue("LArEMBnoOFcableBundle") << std::endl;
  std::cout << "Cable Cu fraction       "  <<
      m_parameters->GetValue("LArEMBmasspercentCu") << std::endl;
  std::cout << "Cable Kapton fraction   " <<
       m_parameters->GetValue("LArEMBmasspercentKap") << std::endl;
  std::cout << "Cable thickness@eta=0   " <<
       m_parameters->GetValue("LArEMBCablethickat0") << std::endl;
  std::cout << "Cable thick increase/eta " << 
     m_parameters->GetValue("LArEMBthickincrfac") << std::endl;
  std::cout << "Cable width              " << 
      m_parameters->GetValue("LArEMBCableEtaheight") << std::endl;
  std::cout << "DR of cable from PS      " << 
      m_parameters->GetValue("LArEMBCablclearfrPS") << std::endl;
  std::cout << "Number of motherboard    " << 
          m_parameters->GetValue("LArEMBnoOFmothboard") << std::endl;
  std::cout << "MotherBoard Cu thickness " << 
        m_parameters->GetValue("LArEMBCuThickness") << std::endl;
  std::cout << "MotherBoard G10 thickness " << 
     m_parameters->GetValue("LArEMBG10Thickness") << std::endl;
  std::cout << "MotherBoard thickness    " << 
     m_parameters->GetValue("LArEMBMoBoTchickness") << std::endl;
 std::cout << "MotherBoard width         " << 
     m_parameters->GetValue("LArEMBMoBoHeight") << std::endl;
  std::cout << "MotherBoard DR from PS   " << 
     m_parameters->GetValue("LArEMBMoBoclearfrPS") << std::endl;
  std::cout << "G10 inner ring thickness " << 
    m_parameters->GetValue("LArEMBG10SupportBarsIn") << std::endl;
  std::cout << "Dz G10 inner ring        " << 
    m_parameters->GetValue("LArEMBG10FrontDeltaZ") << std::endl;
  std::cout << "G10 front tip            " << 
    m_parameters->GetValue("LArEMBG10TipThickFront") << std::endl;
  std::cout << "Absorber front tip       " << 
    m_parameters->GetValue("LArEMBLeadTipThickFront") << std::endl;
  std::cout << "Description of matter after accordion " << std::endl;
  std::cout << "G10 outer ring thickness " << 
     m_parameters->GetValue("LArEMBG10SupportBarsOut") << std::endl,
  std::cout << "Absorber outer tip       " << 
    m_parameters->GetValue("LArEMBLeadTipThickEnd") << std::endl;
  std::cout << "G10 outer tip            " <<
     m_parameters->GetValue("LArEMBG10TipThickEnd") << std::endl;
  std::cout << "total outer tip DeltaR   " << 
    m_parameters->GetValue("LArEMBLArGapTail") << std::endl;
}
