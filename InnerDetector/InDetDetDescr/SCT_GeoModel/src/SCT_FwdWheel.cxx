/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdWheel.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

#include "SCT_GeoModel/SCT_FwdModule.h"
#include "SCT_GeoModel/SCT_FwdRing.h"
#include "SCT_GeoModel/SCT_FwdDiscSupport.h"
#include "SCT_GeoModel/SCT_FwdPatchPanel.h"
#include "SCT_GeoModel/SCT_FwdPPConnector.h"
#include "SCT_GeoModel/SCT_FwdPPCooling.h"
#include "SCT_GeoModel/SCT_FwdFSI.h"
#include "SCT_GeoModel/SCT_FwdDiscPowerTape.h"
#include "SCT_GeoModel/SCT_FwdRingCooling.h"
#include "SCT_GeoModel/SCT_FwdOptoHarness.h"
#include "SCT_GeoModel/SCT_FwdDiscFixation.h"

#include "SCT_GeoModel/SCT_FSIHelper.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

#include <iostream>

inline double sqr(double x) {return x*x;}

SCT_FwdWheel::SCT_FwdWheel(const std::string & name,
                           int iWheel,
                           const std::vector<SCT_FwdModule *> & modules,
                           int ec,
                           InDetDD::SCT_DetectorManager* detectorManager,
                           const SCT_GeometryManager* geometryManager,
                           SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_iWheel(iWheel), 
    m_endcap(ec),
    m_pPConnector(0),
    m_pPCooling(0),
    m_discFixation(0),
    m_modules(modules)
{
  getParameters();
  //m_logVolume = 0;
  m_logVolume = preBuild(); 
  
}

SCT_FwdWheel::~SCT_FwdWheel()
{
  delete m_discSupport;
  for (unsigned int iRing = 0; iRing < m_rings.size(); iRing++){
    delete m_rings[iRing];
  }
  for (unsigned int iPPType = 0; iPPType < m_patchPanel.size(); iPPType++) {
    delete m_patchPanel[iPPType];
  }

  if (m_pPConnectorPresent) delete m_pPConnector;
  if (m_pPCoolingPresent)  delete m_pPCooling;
  if (m_discFixationPresent)  delete m_discFixation;
  
  for (unsigned int iFSI = 0; iFSI <  m_fsiType.size(); iFSI++) {
    delete m_fsiType[iFSI];
  }
}

void
SCT_FwdWheel::getParameters()
{
  const SCT_ForwardParameters * parameters = m_geometryManager->forwardParameters();
   
  m_zPosition = parameters->fwdWheelZPosition(m_iWheel);
  m_numRings  = parameters->fwdWheelNumRings(m_iWheel);
  for (int iRing = 0; iRing < m_numRings; iRing++) {
    m_ringType.push_back(parameters->fwdWheelModuleType(m_iWheel,iRing, m_endcap)); 
    //m_ringSide.push_back(parameters->fwdWheelRingSide(m_iWheel,iRing, m_endcap));
    //m_ringStereoType.push_back(parameters->fwdWheelStereoType(m_iWheel));
  }
  
  m_numPatchPanelLocs =  parameters->fwdNumPatchPanelLocs();
  m_numPatchPanelTypes = parameters->fwdNumPatchPanelTypes();

  for (int iPPLoc = 0; iPPLoc < m_numPatchPanelLocs; iPPLoc++) {
    m_patchPanelType.push_back(parameters->fwdPatchPanelType(iPPLoc));
    m_patchPanelLocAngle.push_back(parameters->fwdPatchPanelLocAngle(iPPLoc));
    m_patchPanelRepeatQuadrant.push_back(parameters->fwdPatchPanelRepeatQuadrant(iPPLoc));
  }

  // FIXME: Check and put these in DB or calculate them
  // We have a maximum width of 80.2. Make it 75 for some clearance.
  //m_innerRadius = 267 * Gaudi::Units::mm;
  //m_outerRadius = 590 * Gaudi::Units::mm;
  //m_thickness = 100 * Gaudi::Units::mm;
  // These get swapped later if the wheel is rotated.
  m_thicknessFront = 30 * Gaudi::Units::mm;
  m_thicknessBack  = 45 * Gaudi::Units::mm;

  m_numFSITypes = parameters->fwdFSINumGeomTypes();
  m_fsiVector =  &(parameters->fsiVector(m_iWheel));

  m_numWheels    = parameters->fwdNumWheels();
 
  m_optoHarnessPresent = parameters->fwdOptoHarnessPresent();
  m_pPConnectorPresent = parameters->fwdPPConnectorPresent();
  m_pPCoolingPresent = parameters->fwdPPCoolingPresent();

  m_discFixationPresent = parameters->fwdDiscFixationPresent();
  if(m_discFixationPresent) {
    for (int iLoc = 0; iLoc < parameters->fwdNumCylinderServiceLocs(); iLoc++) {
      // Disc fixations are at same locations at as N2 pipes
      if(parameters->fwdCylinderServiceLocName(iLoc) == "NPipe") {
        m_discFixationLocAngle.push_back(parameters->fwdCylinderServiceLocAngle(iLoc));
      }
    }
  }

  m_safety = m_geometryManager->generalParameters()->safety();

  // Set numerology
  m_detectorManager->numerology().setNumRingsForDisk(m_iWheel,m_numRings);  

}

const GeoLogVol * 
SCT_FwdWheel::preBuild()
{
   
  // Create disc support. 
  m_discSupport = new SCT_FwdDiscSupport("DiscSupport"+intToString(m_iWheel), m_iWheel,
                                         m_detectorManager, m_geometryManager, m_materials);


  // The rings
  for (int iRing = 0; iRing < m_numRings; iRing++){
    std::string ringName = "Ring"+intToString(iRing)+"For"+getName();
    int ringType = m_ringType[iRing];
    m_rings.push_back(new SCT_FwdRing(ringName, m_modules[ringType], m_iWheel, iRing, m_endcap,
                                      m_detectorManager, m_geometryManager, m_materials));
  }


  // Create Patch Panel
  //m_patchPanel = new SCT_FwdPatchPanel("PatchPanel"+intToString(m_iWheel), m_iWheel);
  for (int iPPType = 0; iPPType < m_numPatchPanelTypes; iPPType++) {
    m_patchPanel.push_back(new SCT_FwdPatchPanel("PatchPanel"+intToString(iPPType), iPPType,
                                                 m_detectorManager, m_geometryManager, m_materials));
  }

  // Create Patch Pannel Connector and Cooling, and disc Fixations
  if (m_pPConnectorPresent) {
    m_pPConnector = new SCT_FwdPPConnector("PPConnector", 
                                           m_detectorManager, m_geometryManager, m_materials);
  }
  if (m_pPCoolingPresent) {
    m_pPCooling = new SCT_FwdPPCooling("PPCooling",
                                       m_detectorManager, m_geometryManager, m_materials);
  }
  if (m_discFixationPresent) {
    m_discFixation = new SCT_FwdDiscFixation("DiscFixation",
                                             m_detectorManager, m_geometryManager, m_materials);
  }

  // Calculate total number of modules
  m_totalModules = 0;
  for (int iRing = 0; iRing < m_numRings; iRing++){
    m_totalModules += m_rings[iRing]->numModules();
  }


  // Create the FSI types
  m_fsiType.resize(m_numFSITypes);
  for (unsigned int iFSI = 0; iFSI < m_fsiVector->size(); iFSI++) {
    int type = (*m_fsiVector)[iFSI]->simType();
    if (!m_fsiType[type]) {
      m_fsiType[type] = new SCT_FwdFSI("FSI"+intToString(type), type,
                                       m_detectorManager, m_geometryManager, m_materials);
    }
  }



  // Calculate the extent of the envelope  
  // Use support disc as starting values.
  double maxOuterRadius = m_discSupport->outerRadius();
  double minInnerRadius = m_discSupport->innerRadius();
  double maxModuleThickness = 0.5 * m_discSupport->thickness();

  // Extend min max accounting for rings
  for (int iRing = 0; iRing < m_numRings; iRing++){
    maxOuterRadius = std::max(m_rings[iRing]->outerRadius(), maxOuterRadius); 
    minInnerRadius = std::min(m_rings[iRing]->innerRadius(), minInnerRadius); 
    maxModuleThickness = std::max(maxModuleThickness, m_rings[iRing]->thicknessOuter() +  m_rings[iRing]->ringOffset());
  }
  m_ringMaxRadius = maxOuterRadius;

  // If first or last wheel there is nothing protruding beyond the rings so we reduce the
  // envelope size. Comes to about 20 mm. Note the front becomes the back later for the last wheel.
  if ((m_iWheel == 0) || (m_iWheel == m_numWheels - 1)) {
     m_thicknessFront = maxModuleThickness + 1*Gaudi::Units::mm; // We give plenty of safety as we have the room.
  // But now modified by disc fixations
     if(m_discFixationPresent) {
       m_thicknessFront = std::max(m_thicknessFront,m_discFixation->radius() + m_safety);
     }
  }

  // The outer radius is now defined by the patch panel cooling if present
  if(m_pPCoolingPresent) {
    double ppCoolingOuterRadius = sqrt(sqr(m_patchPanel[2]->outerRadius() + m_pPCooling->deltaR()) + sqr(m_pPCooling->rphi())) + m_safety;
    maxOuterRadius = std::max(ppCoolingOuterRadius, maxOuterRadius);
  }

  // Or maybe by the disc fixations...
  if(m_discFixationPresent) {
    double discFixationOuterRadius = sqrt(sqr(m_ringMaxRadius + m_discFixation->thickness() + m_safety) + sqr(m_discFixation->radius())) + m_safety;
    maxOuterRadius = std::max(discFixationOuterRadius, maxOuterRadius);
  }

  m_rotateWheel = +1; // +1 normal, -1 rotate (ie last wheel)
  if (m_numRings > 0) { // Should always be true
    m_rotateWheel = (m_rings[0]->discRotated()) ? -1 : +1 ;
  }

  // swap thickness front/back (this only happens for the last wheel).
  if (m_rotateWheel < 0) {
    double tmp = m_thicknessFront;
    m_thicknessFront = m_thicknessBack;
    m_thicknessBack  = tmp;
  }
  
  m_thickness =  m_thicknessFront +  m_thicknessBack;

  m_innerRadius = minInnerRadius - m_safety;
  m_outerRadius = maxOuterRadius + m_safety;

  // TODO. Have to account for FSI and patch panels
  //m_thickness   = 2. * maxRingOffset + maxThickness; 
  // m_thickness  = 100 * Gaudi::Units::mm;

  // Make envelope for the wheel
  double envelopeShift = 0.5*(m_thicknessBack - m_thicknessFront);
  const GeoTube * tmpShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  const GeoShape & fwdWheelEnvelopeShape = *tmpShape <<  GeoTrf::Translate3D(0, 0, envelopeShift);

  const GeoLogVol * fwdWheelLog = 
    new GeoLogVol(getName(), &fwdWheelEnvelopeShape, m_materials->gasMaterial());

  
  return fwdWheelLog;

}

GeoVPhysVol * 
SCT_FwdWheel::build(SCT_Identifier id)
{
  GeoFullPhysVol * wheel = new GeoFullPhysVol(m_logVolume);

  // Add discsupport. Its centered so no need for a transform
  wheel->add(m_discSupport->getVolume());

  //
  // Loop over rings and put in rings, cooling and power tapes.
  // 
  double powerTapeZPlusMax = 0.5 * m_discSupport->thickness();
  double powerTapeZMinusMax = -0.5 * m_discSupport->thickness();

  double maxZOfRingsFront = 0;

  for (int iRing = 0; iRing < m_numRings; iRing++){
   
    SCT_FwdRing * ring = m_rings[iRing];

    // Position ring
    double ringZpos = ring->ringSide() * ring->ringOffset(); 
    double ringOuterZ = ring->ringOffset() +  ring->thicknessOuter();
    maxZOfRingsFront = std::max(maxZOfRingsFront, ringOuterZ);

    std::string ringNameTag = "Ring#" + intToString(ring->identifier());
    wheel->add(new GeoNameTag(ringNameTag));
    wheel->add(new GeoIdentifierTag(ring->identifier()));
    wheel->add(new GeoTransform(GeoTrf::Translate3D(0, 0, ringZpos)));    
    id.setEtaModule(ring->identifier());
    wheel->add(ring->build(id));

    // Position cooling
    // Get a pointer to the cooling ring.
    SCT_FwdRingCooling cooling("RingCoolingW"+intToString(m_iWheel)+"R"+intToString(iRing),
                               iRing, m_detectorManager, m_geometryManager, m_materials);
    double coolingZpos = ring->ringSide() * (0.5*(m_discSupport->thickness() + cooling.thickness()));
    wheel->add(new GeoTransform(GeoTrf::TranslateZ3D(coolingZpos)));
    wheel->add(cooling.getVolume());
 
    // Power Tapes
    // Get a pointer to the power tape
    SCT_FwdDiscPowerTape powerTape("PowerTapeW"+intToString(m_iWheel)+
                                   "R"+intToString(iRing), iRing,
                                   m_detectorManager, m_geometryManager, m_materials);

    double powerTapeZpos = ring->ringSide() * (0.5*(m_discSupport->thickness() + powerTape.thickness()) +
                                               cooling.thickness());
    // Make sure we don't overlap with powertape from outer rings
    // We store max extent of power tape for each side (Plus, Minus)
    // This is really only ever an issue for ring2 but we keep it general.
    if (ring->ringSide() > 0) {
      double powerTapeZstart = powerTapeZpos -  0.5 * powerTape.thickness();
      if (powerTapeZstart < powerTapeZPlusMax) {
        powerTapeZpos = powerTapeZPlusMax +  0.5 * powerTape.thickness();
      }
      powerTapeZPlusMax = powerTapeZpos +  0.5 * powerTape.thickness();
    } else {
      double powerTapeZstart = powerTapeZpos +  0.5 * powerTape.thickness();
      if (powerTapeZstart > powerTapeZMinusMax) {
        powerTapeZpos = powerTapeZMinusMax -  0.5 * powerTape.thickness();
      }
      powerTapeZMinusMax = powerTapeZpos -  0.5 * powerTape.thickness();
    }
    if ((std::abs(powerTapeZpos)+0.5*powerTape.thickness()) > (std::abs(ringZpos) -  0.5*ring->thicknessInner())) {
      std::cout << "ERROR:  Power tapes clash with modules!!!" << std::endl; 
    }
    wheel->add(new GeoTransform(GeoTrf::TranslateZ3D(powerTapeZpos)));
    wheel->add(powerTape.getVolume());
  

  } // End Ring loop


  //
  // Put in the patch Panel
  //
  // Put it on opposide side of outer ring (ring 0).
  int patchPanelSide = m_rotateWheel;


  // Get outer position of power tape on that side.
  // In case there are no rings on that side (and hence no powertapes) this will be 1/2 disc support thickness
  double powerTapeZMax = 0;
  if (patchPanelSide > 0) {
    powerTapeZMax =  powerTapeZPlusMax;
  } else {
    powerTapeZMax =  -powerTapeZMinusMax;
  }



  // Loop over patch panel locations
  for (int iPPLoc = 0; iPPLoc < m_numPatchPanelLocs ; iPPLoc++) {

    // The patchpanels generally repeat in the four quadrants.
    int numRepeat = 1;
    if (m_patchPanelRepeatQuadrant[iPPLoc]) numRepeat = 4;
    int ppType = m_patchPanelType[iPPLoc];
    if (ppType >= m_numPatchPanelTypes) std::cout << "ERROR: Patch Panel type number out of range!" << std::endl;
    for (int iRepeat = 0; iRepeat < numRepeat; iRepeat++) {
  
      // Calculate the location.
      double patchPanelAngle = m_patchPanelLocAngle[iPPLoc] + iRepeat * 90*Gaudi::Units::degree;
      double patchPanelZpos =  patchPanelSide * (powerTapeZMax + 0.5*m_patchPanel[ppType]->thickness() + m_safety);
      double patchPanelR = m_patchPanel[ppType]->midRadius();

      // Check for overlap with middle ring.
      if (m_numRings >= 2 && (m_patchPanel[ppType]->innerRadius() <= m_rings[1]->outerRadius())) {
        std::cout << "ERROR: Patch Panel clashes with middle ring" << std::endl;
        std::cout << " PatchPanel inner radius: " << m_patchPanel[ppType]->innerRadius() << std::endl;
        std::cout << " Ring outer radius: " << m_rings[1]->outerRadius() << std::endl;
      }
  
      // Add it to the wheel
      wheel->add(new GeoTransform(GeoTrf::RotateZ3D(patchPanelAngle)*GeoTrf::TranslateX3D(patchPanelR)*GeoTrf::TranslateZ3D(patchPanelZpos)));
      wheel->add(m_patchPanel[ppType]->getVolume());

      // Make and add the connector for PPF0e (type 0)
      // Positioned immediately outside PatchPanel at same angle
      if(ppType == 0 && m_pPConnectorPresent) {
        double ppConnectorZpos =  patchPanelSide * (powerTapeZMax + 0.5*m_pPConnector->thickness() + m_safety);
        double ppConnectorR = m_patchPanel[ppType]->outerRadius() + 0.5*m_pPConnector->deltaR() + m_safety;
        // Check is within wheel
        if (ppConnectorR + 0.5*m_pPConnector->deltaR() >= m_outerRadius) {
          std::cout << "ERROR: Patch Panel Connector clashes outside wheel" << std::endl;
          std::cout << " PatchPanel Connector outer radius: " << ppConnectorR + 0.5*m_pPConnector->deltaR() << std::endl;
          std::cout << " Wheel outer radius: " << m_outerRadius << std::endl;
        }
        // Add it to the wheel
      wheel->add(new GeoTransform(GeoTrf::RotateZ3D(patchPanelAngle)*GeoTrf::TranslateX3D(ppConnectorR)*GeoTrf::TranslateZ3D(ppConnectorZpos)));
      wheel->add(m_pPConnector->getVolume());
      }

      // Make and add the cooling for PPF0c (type 2)
      // Positioned immediately outside PatchPanel at same angle
      if(ppType == 2 && m_pPCoolingPresent) {
        double ppCoolingZpos =  patchPanelSide * (powerTapeZMax + 0.5*m_pPCooling->thickness() + m_safety);
        double ppCoolingR = m_patchPanel[ppType]->outerRadius() + 0.5*m_pPCooling->deltaR() + m_safety;
        // Check is within wheel
        if (ppCoolingR + 0.5*m_pPCooling->deltaR() >= m_outerRadius) {
          std::cout << "ERROR: Patch Panel Cooling clashes outside wheel" << std::endl;
          std::cout << " PatchPanel Cooling outer radius: " << ppCoolingR + 0.5*m_pPCooling->deltaR() << std::endl;
          std::cout << " Wheel outer radius: " << m_outerRadius << std::endl;
        }
        // Add it to the wheel
      wheel->add(new GeoTransform(GeoTrf::RotateZ3D(patchPanelAngle)*GeoTrf::TranslateX3D(ppCoolingR)*GeoTrf::TranslateZ3D(ppCoolingZpos)));
      wheel->add(m_pPCooling->getVolume());
      }
    } 

  } // end loop over patch panel locations


  // Add the optoharness - type depends on number of rings
  // The optoharness is always on the back side (except if the wheel is rotates)
  double optoHarnessZMax = 0.5 * m_discSupport->thickness();
  if (m_optoHarnessPresent) {
    std::string optoharnessName = "OptoHarnessO";
    if(m_numRings > 1) {optoharnessName+="M";}
    if(m_numRings > 2) {optoharnessName+="I";}
    SCT_FwdOptoHarness optoharness(optoharnessName+"W"+intToString(m_iWheel), m_numRings,
                                   m_detectorManager, m_geometryManager, m_materials);
    double optoHarnessZpos = 0.5*m_rotateWheel*(m_discSupport->thickness() + optoharness.thickness());
    wheel->add(new GeoTransform(GeoTrf::TranslateZ3D(optoHarnessZpos)));
    wheel->add(optoharness.getVolume());
    optoHarnessZMax = optoHarnessZpos + 0.5*optoharness.thickness();
  }

  // Loop over FSI locations.
  for (unsigned int iFSI = 0; iFSI < m_fsiVector->size(); iFSI++) {
    int type = (*m_fsiVector)[iFSI]->simType();
    double fsiRadius = (*m_fsiVector)[iFSI]->location().radius();
    double fsiPhi    = (*m_fsiVector)[iFSI]->location().phi();
    int fsiUsualSide = (*m_fsiVector)[iFSI]->location().side();
    int fsiSide =   fsiUsualSide * m_rotateWheel;
    double fsiZpos = fsiSide * m_fsiType[type]->zOffset(); 

    // Check for clashes on front side
    if (fsiUsualSide < 0) {
      double zMin =  std::abs(fsiZpos) -  0.5*m_fsiType[type]->thickness(); 
      if (maxZOfRingsFront > zMin) {
        std::cout << "WARNING: FSI probably clashes with ring" << std::endl;
        std::cout << " maxZOfRingsFront = " << maxZOfRingsFront << std::endl;
        std::cout << " fsiZMin = " << zMin << std::endl;
      }
    } else {   // ... and backside
      // Check radius of fsi. 
      // If outer radii check for overlap with powertape
      // If inner radii check for overlap with optoharness (disksupport if no optoharness)
      double diskMidRadius = 0.5*(m_outerRadius + m_innerRadius);
      double servicesZMax = (fsiRadius > diskMidRadius) ? powerTapeZMax : optoHarnessZMax;
      double zMin =  std::abs(fsiZpos) -  0.5*m_fsiType[type]->thickness(); 
      if (servicesZMax > zMin) { 
        std::cout << "WARNING: FSI probably clashes with disc services" << std::endl;
        std::cout << " servicesZMax = " << servicesZMax << std::endl;
        std::cout << " fsiZMin = " << zMin << std::endl;
        std::cout << " fsiRadius = " << fsiRadius << std::endl;
      }
    }    

    wheel->add(new GeoTransform(GeoTrf::RotateZ3D(fsiPhi)*GeoTrf::TranslateX3D(fsiRadius)*GeoTrf::TranslateZ3D(fsiZpos)));
    wheel->add(m_fsiType[type]->getVolume());    


  } // end loop over FSI.

  // Loop over disc fixation locations
  if(m_discFixationPresent) {
    for (unsigned int iLoc = 0; iLoc < m_discFixationLocAngle.size() ; iLoc++) {
      // The disc fixations repeat in the four quadrants.
      for (int iRepeat = 0; iRepeat < 4; iRepeat++) {
        // Calculate the location.
        double discFixationAngle = m_discFixationLocAngle[iLoc] + iRepeat * 90*Gaudi::Units::degree;
        double discFixationR = m_ringMaxRadius + 0.5*m_discFixation->thickness() + m_safety;
        // Check is within wheel
        if (discFixationR + 0.5*m_discFixation->thickness() >= m_outerRadius) {
          std::cout << "ERROR: Disc Fixation outside wheel" << std::endl;
          std::cout << "Disc fixation outer radius: " << discFixationR + 0.5*m_discFixation->thickness() << std::endl;
          std::cout << " Wheel outer radius: " << m_outerRadius << std::endl;
        }
       // Add it to the wheel
        wheel->add(new GeoTransform(GeoTrf::RotateY3D(90.*Gaudi::Units::degree)*GeoTrf::RotateX3D(discFixationAngle)*GeoTrf::TranslateZ3D(discFixationR)));
        wheel->add(m_discFixation->getVolume());
      }
    }
  }


  
  // Extra Material
  InDetDD::ExtraMaterial xMat(m_geometryManager->distortedMatManager());
  xMat.add(wheel, "SCTDisc");
  xMat.add(wheel, "SCTDisc"+intToString(m_iWheel));
  if (m_endcap > 0) {
    xMat.add(wheel, "SCTDiscA");
    xMat.add(wheel, "SCTDiscA"+intToString(m_iWheel));
  } else {
    xMat.add(wheel, "SCTDiscC");
    xMat.add(wheel, "SCTDiscC"+intToString(m_iWheel));
  }
  return wheel;
  
}

