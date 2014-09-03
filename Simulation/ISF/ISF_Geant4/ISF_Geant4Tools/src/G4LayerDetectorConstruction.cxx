/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4LayerDetectorConstruction.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Fatras includes
#include "ISF_Geant4Tools/G4LayerDetectorConstruction.h"
// Tracking includes
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkGeometry/MaterialProperties.h"
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Geant includes
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"

// G4 material : vacuum setup
G4Material* G4LayerDetectorConstruction::s_g4vacuum = new G4Material("Vacuum",1.,1.01*CLHEP::g/CLHEP::mole,
                                                                     CLHEP::universe_mean_density,
                                                                     kStateGas,0.1*CLHEP::kelvin, 1.e-19*CLHEP::pascal);

G4LayerDetectorConstruction::G4LayerDetectorConstruction()
  :   m_layerMaterial(0),
      m_worldLog(0),
      m_detectorLog(0),
      m_layerLog(0),
      m_worldPhys(0),
      m_detectorPhys(),
      m_layerPhys(0),
      m_momentum(0),
      m_ematProp(0)

{}


G4LayerDetectorConstruction::~G4LayerDetectorConstruction()
{}


void G4LayerDetectorConstruction::updateDetector(const Amg::Vector3D& momentum,
                                                 const Trk::MaterialProperties& emp,
                                                 double pathCorrection)
{

  // update the detector parameters
  m_momentum = (&momentum);
  m_ematProp = (&emp);

  // RE-USE THE MEMORY AS MUCH AS POSSIBLE ----------------------

  // a bit dirty, but the material needs to be deleted
  const G4MaterialTable* mattable = G4Material::GetMaterialTable();
  if (mattable && mattable->size()){
    G4MaterialTable* ncmattable = const_cast<G4MaterialTable*>(mattable);
    ncmattable->clear();
  }

  // now create the material
  if (m_layerMaterial){
    // make Z being an integeter
    int         iZ = int(m_ematProp->averageZ());
    double   diffZ = (m_ematProp->averageZ())-iZ;
    if (diffZ > 0.5) ++iZ;
    // make sure that A scales like Z (for A/Z ratio)
    double ratioZ  = iZ/(m_ematProp->averageZ());
    double     eA  = ratioZ*m_ematProp->averageA();



    m_layerMaterial = new(m_layerMaterial) G4Material("LayerMaterial",
                                                      iZ,
                                                      eA*CLHEP::g/CLHEP::mole,
                                                      m_ematProp->averageRho()*CLHEP::g/CLHEP::mm3);
  } else m_layerMaterial = new G4Material("Silicon", 14., 28.0855*CLHEP::g/CLHEP::mole, 2.330*CLHEP::g/CLHEP::cm3);

  // thickness
  double thickness = m_ematProp ? m_ematProp->thickness()*pathCorrection : 0.25*CLHEP::mm*pathCorrection;

  // MATERIAL output
  std::cout << " [ Material ] " << m_layerMaterial->GetName()  << std::endl;
  std::cout << "              - Z, A       : " << m_layerMaterial->GetZ() << ", " << m_layerMaterial->GetA() << std::endl;
  std::cout <<"               - rho,X0,L0  : " << m_layerMaterial->GetDensity() << ", " << m_layerMaterial->GetRadlen() << ", " << m_layerMaterial->GetNuclearInterLength() << std::endl;
  std::cout <<"               - t/X0, t/L0 : " << thickness/m_layerMaterial->GetRadlen() << ", " << thickness/m_layerMaterial->GetNuclearInterLength() << std::endl;


  // building the curvilinear frame for the mini-detector
  Trk::CurvilinearUVT curvFrame = m_momentum ? Trk::CurvilinearUVT(m_momentum->unit()) : Trk::CurvilinearUVT(Amg::Vector3D(0.5,0.5,0.01));
  const CLHEP::Hep3Vector tempCurvFrameU(curvFrame.curvU().x(),curvFrame.curvU().y(),curvFrame.curvU().z());//Eigen compatibility HACK
  const CLHEP::Hep3Vector tempCurvFrameV(curvFrame.curvV().x(),curvFrame.curvV().y(),curvFrame.curvV().z());//Eigen compatibility HACK
  const CLHEP::Hep3Vector tempCurvFrameT(curvFrame.curvT().x(),curvFrame.curvT().y(),curvFrame.curvT().z());//Eigen compatibility HACK
  CLHEP::HepRotation materialRotation(tempCurvFrameU,tempCurvFrameV,tempCurvFrameT);//Eigen compatibility HACK
  G4ThreeVector materialPosition(0.,0.,0.);

  // (1) WORLD
  // create the world setup, by definition 3 times the thickness
  G4Box* worldBox = new G4Box("WorldBox",15*thickness,15*thickness,15*thickness);
  m_worldLog      = m_worldLog ? new(m_worldLog) G4LogicalVolume(worldBox,s_g4vacuum,"WorldLogical", 0, 0, 0) :
    new G4LogicalVolume(worldBox,s_g4vacuum,"WorldLogical", 0, 0, 0);
  m_worldPhys     = m_worldPhys ? new(m_worldPhys) G4PVPlacement(0,G4ThreeVector(0.,0.,0),"WorldPhysical",m_worldLog,0,false,0) :
    new G4PVPlacement(0,materialPosition,"WorldPhysical",m_worldLog,0,false,0);

  // (2) DETECTOR
  // create the detector setup, by definition 3 times the thickness
  G4Box* detectordBox = new G4Box("WorldBox",10*thickness,10*thickness,10*thickness);
  m_detectorLog      = m_detectorLog ? new(m_detectorLog) G4LogicalVolume(worldBox,s_g4vacuum,"DetectorLogical", 0, 0, 0) :
    new G4LogicalVolume(detectordBox,s_g4vacuum,"DetectordLogical", 0, 0, 0);
  m_detectorPhys     = m_detectorPhys ? new(m_detectorPhys) G4PVPlacement(0,materialPosition,"DetectorPhysical",m_detectorLog,0,false,0) :
    new G4PVPlacement(0,materialPosition,"DetectorPhysical",m_detectorLog,m_worldPhys,false,0);

  // (3) LAYER
  G4Box* layerBox           = new G4Box("MaterialLayer",0.5*thickness,0.5*thickness,0.5*thickness);
  m_layerLog                = m_layerLog ? new(m_layerLog) G4LogicalVolume(layerBox,m_layerMaterial,"MaterialLayerLogical", 0, 0, 0) :
    new G4LogicalVolume(layerBox,m_layerMaterial,"MaterialLayerLogical", 0, 0, 0);
  // -- Placement: inside relatively to the world
  m_layerPhys               = m_layerPhys ? new(m_layerPhys) G4PVPlacement(G4Transform3D(materialRotation,materialPosition),"MaterialLayerPhysical",m_layerLog,m_worldPhys,false,0) :
    new G4PVPlacement(G4Transform3D(materialRotation,materialPosition),"MaterialLayerPhysical",m_layerLog,m_detectorPhys,false,0);
}




G4VPhysicalVolume* G4LayerDetectorConstruction::Construct()
{
  // call the update detector method
  updateDetector(*m_momentum,*m_ematProp);
  return m_worldPhys;
}
