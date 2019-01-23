/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_Forward.h"

#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_ForwardModuleParameters.h"

#include "SCT_GeoModel/SCT_FwdWheel.h"
#include "SCT_GeoModel/SCT_FwdModule.h"
#include "SCT_GeoModel/SCT_FwdRing.h"
#include "SCT_GeoModel/SCT_FwdRingCooling.h"
#include "SCT_GeoModel/SCT_FwdSupportFrame.h"
#include "SCT_GeoModel/SCT_FwdCoolingPipe.h"
#include "SCT_GeoModel/SCT_FwdPowerTape.h"
#include "SCT_GeoModel/SCT_FwdCylinderServices.h"
#include "SCT_GeoModel/SCT_FwdThermalShieldElement.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

SCT_Forward::SCT_Forward(const std::string & name, int ec)
  : SCT_UniqueComponentFactory(name), 
    m_endcap(ec)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Forward::~SCT_Forward()
{
  for (size_t i = 0; i < m_wheels.size(); i++) delete m_wheels[i];
  for (size_t i = 0; i < m_modules.size(); i++) delete m_modules[i];
}

void 
SCT_Forward::getParameters()
{

  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  const SCT_ForwardModuleParameters * moduleParameters = geometryManager()->forwardModuleParameters();
    
  //m_numRingTypes = parameters->fwdNumRingTypes();
  m_numModuleTypes = moduleParameters->fwdModuleNumTypes();
  m_numWheels    = parameters->fwdNumWheels();
  m_innerRadius  = parameters->fwdInnerRadius();
  m_outerRadius  = parameters->fwdOuterRadius();
  m_zMin         = parameters->fwdZMin();
  m_zMax         = parameters->fwdZMax();
  m_trtGapPos    = parameters->fwdTrtGapPos();
  m_coolingPipeRadius        = parameters->fwdCoolingPipeRadius();
  m_numThermalShieldElements = parameters->fwdNumThermalShieldElements();
  m_cylinderServicesPresent  = parameters->fwdCylinderServicePresent();
  
  // Outer radius of cylinder services is given by inner radius of OTE
  if(m_cylinderServicesPresent) {
    for (int iElement = 0; iElement < m_numThermalShieldElements; iElement++){
      if(parameters->fwdThermalShieldMaterial(iElement) == "sct::FwdOTE") {
        m_outerRadiusCylinderServices = parameters->fwdThermalShieldInnerRadius(iElement);
      }
    }
  }

  // Length of forward envelope
  m_length = m_zMax - m_zMin;  


  // Set numerology
  detectorManager()->numerology().setNumDisks(m_numWheels);


}



const GeoLogVol * 
SCT_Forward::preBuild()
{
  // Create the elements we need for the forward

  // We make all the module types here. There is a outer, middle, truncated middle and inner type module.
  for (int iModuleType = 0; iModuleType < m_numModuleTypes; iModuleType++){
    m_modules.push_back(new SCT_FwdModule("FwdModule"+intToString(iModuleType), iModuleType));
  }

  for (int iWheel = 0; iWheel < m_numWheels; iWheel++){
    // Build Wheels
    std::ostringstream name; name << "Wheel" << iWheel << ((m_endcap > 0) ? "A" : "C");
    //std::cout << getName() << ", iWheel = " << iWheel << ", " <<  name.str() << ", m_endcap = " << m_endcap << std::endl;
    const SCT_FwdWheel * wheel = new SCT_FwdWheel(name.str(), iWheel, m_modules, m_endcap);
    m_wheels.push_back(wheel);
  }


  // Make one end of the Forward tracker
  //  Tube envelope containing the forward
  SCT_MaterialManager materials;
  const GeoTube * forwardEnvelopeShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_length);
  const GeoLogVol * forwardLog = 
    new GeoLogVol(getName(), forwardEnvelopeShape, materials.gasMaterial());
  
  return forwardLog;
}

GeoVPhysVol * 
SCT_Forward::build(SCT_Identifier id) const
{
  GeoFullPhysVol * forward = new GeoFullPhysVol(m_logVolume);

  for (int iWheel = 0; iWheel < m_numWheels; iWheel++){

    // Build Wheels
    // std::cout << "iWheel = " << iWheel << std::endl;
    //std::ostringstream name; name << "Wheel" << iWheel << ((m_endcap > 0) ? "A" : "C");
    //const SCT_FwdWheel * wheel = new SCT_FwdWheel(name.str(), iWheel, m_modules, m_endcap);
    //m_wheels.push_back(wheel);

    const SCT_FwdWheel * wheel = m_wheels[iWheel];
    std::ostringstream wheelName; wheelName << "Wheel#" << iWheel;
    double zpos = wheel->zPosition() - zCenter();
    //    std::cout << "Adding wheel " << iWheel << ", z = " << m_wheels[iWheel]->zPosition()
    //          << " at " << zpos << ", thickness = " << wheel->thickness() << std::endl;
    forward->add(new GeoNameTag(wheelName.str()));
    forward->add(new GeoIdentifierTag(iWheel));
    GeoAlignableTransform * transform = new GeoAlignableTransform(GeoTrf::TranslateZ3D(zpos));
    forward->add(transform);
    id.setLayerDisk(iWheel);
    GeoVPhysVol * wheelPV = wheel->build(id);
    forward->add(wheelPV);

    // Store the alignable transform
    detectorManager()->addAlignableTransform(2, id.getWaferId(), transform, wheelPV);
  }

  //
  // Place SupportFrame
  //
  SCT_FwdSupportFrame supportFrame("SupportFrame");
  double supportFrameZPos = supportFrame.zPosition() - zCenter();
  forward->add(new GeoTransform(GeoTrf::TranslateZ3D(supportFrameZPos)));
  forward->add(supportFrame.getVolume());

  // Make and Place Cylinder Services

  if(m_cylinderServicesPresent) {

    // New phi-dependent services
    SCT_FwdCylinderServices cylinderServices("CylinderServices",
                                             supportFrame.outerRadius(),
                                             m_outerRadiusCylinderServices,
                                             supportFrame.length());
    forward->add(new GeoTransform(GeoTrf::TranslateZ3D(supportFrameZPos)));
    forward->add(cylinderServices.getVolume());

  } else {

    // Old cylindrical services
    //
    // Make cooling pipes. These extend from the wheel to the TRT Gap.
    //
    {
      // End position of the pipes.
      double endPos = m_trtGapPos;

      // Calculate radius to start placing cooling pipes. This is equal to the
      // outer radius of the support frame + the pipe radius (The pipe radius is to just 
      // give a small gap - it is not really necessary)
      double innerRadiusCooling = supportFrame.outerRadius() + m_coolingPipeRadius; 

      // Inner radius of cylinder representing pipes. Gets incremented for each wheel.
      double rStart = innerRadiusCooling;

      for (int iWheel = 0; iWheel < m_numWheels; iWheel++){
        // Start position of the pipes.
        double startPos = m_wheels[iWheel]->zPosition();
       
        // Assume one cooling circuit per quadrant of each ring. ie 8 pipes per ring.
        int numPipes = 8 * m_wheels[iWheel]->numRings();
    
        // Label Cooling pipe with W# at end of string  
        // std::ostringstream label; label << "CoolingPipeW" << iWheel + 1;       
        //SCT_FwdCoolingPipe * coolingPipe = new SCT_FwdCoolingPipe(label.str(), numPipes, rStart, startPos, endPos);
        SCT_FwdCoolingPipe coolingPipe("OffDiskCoolingPipeW"+intToString(iWheel),
                                       numPipes, rStart, startPos, endPos);  
        //std::cout << "Cooling pipe rmin,rmax: " << coolingPipe.innerRadius() << ", " << coolingPipe.outerRadius()  << std::endl;
      
        // Place the cooling pipes
        double coolingPipeZPos = coolingPipe.zPosition() - zCenter();
        forward->add(new GeoTransform(GeoTrf::TranslateZ3D(coolingPipeZPos)));
        forward->add(coolingPipe.getVolume());

        // Set rStart for next cooling pipe equal to outer radius of this cooling pipe.
        rStart = coolingPipe.outerRadius();
      
      }
    }

    //
    // Make Power Tapes. These extend from the wheel to the TRT Gap.
    //
    {
      
      // End position of the power tapes.
      double endPos = m_trtGapPos;
    
      // Calculate radius to start placing power tapes. This is half way bewteen outer radius
      // of support fram and outer radius of forward envelope.
      // The -1 mm is to avoid a clash with the thermal shield.
      double innerRadiusPowerTapes = 0.5*(supportFrame.outerRadius() + m_outerRadius) - 1*Gaudi::Units::mm;

      // Inner radius of cylinder representing power tapes. Gets incremented for each wheel.
      double rStart = innerRadiusPowerTapes;
    
      for (int iWheel = 0; iWheel < m_numWheels; iWheel++){
      
        // Start position of the power tapes.
        double startPos = m_wheels[iWheel]->zPosition();
      
        // Get total number of modules in wheel
        int numModules = m_wheels[iWheel]->totalModules();

        // Label power tape with W# at end of string  
        //std::ostringstream label; label << "PowerTapeW" << iWheel + 1;       
        //SCT_FwdPowerTape * powerTape = new SCT_FwdPowerTape(label.str(), numModules, rStart, startPos, endPos);
        SCT_FwdPowerTape powerTape("OffDiskPowerTapeW"+intToString(iWheel),
                                   numModules, rStart, startPos, endPos);
        //std::cout << "PowerTape rmin,rmax: " << powerTape.innerRadius() << ", " << powerTape.outerRadius()  << std::endl;


        // Place Power Tapes
        double powerTapeZPos = powerTape.zPosition() - zCenter();
        forward->add(new GeoTransform(GeoTrf::TranslateZ3D(powerTapeZPos)));
        forward->add(powerTape.getVolume());
            
        // Set rStart for next power tape equal to outer radius of this power tape.
        rStart = powerTape.outerRadius();
      } // end loop over wheels
    }
  }

  //
  // Place Thermal Shield Elements
  //
  for (int iElement = 0; iElement < m_numThermalShieldElements; iElement++){
    SCT_FwdThermalShieldElement thermalShieldElement("FwdThermalShieldElement"+intToString(iElement),
                                                     iElement);
    double elementZPos = thermalShieldElement.zPosition() - zCenter();
    forward->add(new GeoTransform(GeoTrf::TranslateZ3D(elementZPos)));
    forward->add(thermalShieldElement.getVolume());
  }

  // Extra Material
  InDetDD::ExtraMaterial xMat(geometryManager()->distortedMatManager());
  xMat.add(forward, "SCTEndcap", zCenter());
  if (m_endcap > 0) {
    xMat.add(forward, "SCTEndcapA", zCenter());
  } else {
    xMat.add(forward, "SCTEndcapC", zCenter());
  }


  return forward;
}
