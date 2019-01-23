/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_GeoModel/SCT_FwdRing.h"

#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"

#include "SCT_GeoModel/SCT_ForwardParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"

#include "SCT_GeoModel/SCT_FwdModule.h"
#include "SCT_GeoModel/SCT_FwdCoolingBlock.h"

#include "InDetReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <sstream>
#include <cmath>

inline double sqr(double x) {return x*x;}

SCT_FwdRing::SCT_FwdRing(const std::string & name, 
                         //int ringType, 
                         const SCT_FwdModule * module, 
                         //const SCT_FwdRingCooling * cooling,
                         //int stereoSign,
                         //int ringSide)
                         int iWheel,
                         int iRing,
                         int ec)
  : SCT_UniqueComponentFactory(name), 
    //    m_ringType(ringType), 
    // m_ringSide(ringSide), 
    //m_stereoSign(stereoSign), 
    //m_cooling(cooling)
    m_iWheel(iWheel),
    m_iRing(iRing),
    m_endcap(ec),
    m_module(module)
{
  getParameters();
  m_logVolume = preBuild();
}

void 
SCT_FwdRing::getParameters()
{
  
  const SCT_ForwardParameters * parameters = geometryManager()->forwardParameters();
  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();
    
  m_safety       = generalParameters->safety();
  
  m_numModules      = parameters->fwdRingNumModules(m_iRing);
  m_moduleStagger   = parameters->fwdRingModuleStagger(m_iRing);
  m_refStartAngle   = parameters->fwdRingPhiOfRefModule(m_iRing);
  m_refFirstStagger = parameters->fwdRingStaggerOfRefModule(m_iWheel, m_iRing, m_endcap);
  m_ringSide        = parameters->fwdWheelRingSide(m_iWheel, m_iRing, m_endcap);
  m_stereoSign      = parameters->fwdWheelStereoType(m_iWheel);
  m_ringOffset      = parameters->fwdRingDistToDiscCenter(m_iRing);
  m_discSupportThickness =  parameters->fwdDiscSupportThickness();

  m_discRotated     = (parameters->fwdRingUsualRingSide(m_iRing) != m_ringSide);

  m_identifier = m_iRing;
  
  // Set numerology
  detectorManager()->numerology().setNumPhiModulesForDiskRing(m_iWheel,m_iRing,m_numModules);

}

SCT_FwdRing::~SCT_FwdRing()
{
  delete m_coolingBlockHiMain;
  delete m_coolingBlockHiSec;
  delete m_coolingBlockLoMain;
  delete m_coolingBlockLoSec;
}

const GeoLogVol * 
SCT_FwdRing::preBuild()
{
  //   std::cout << getName() << std::endl;
  //   std::cout << "Wheel, Ring = " << m_iWheel << ", " << m_iRing << std::endl;
  //   std::cout << "m_module->thickness() = " << m_module->thickness() << std::endl;
  //   std::cout << "m_moduleStagger = " << m_moduleStagger << std::endl;
  //   std::cout << "m_refStartAngle = " << m_refStartAngle << std::endl;
  //   std::cout << "m_refFirstStagger = " << m_refFirstStagger << std::endl;
  //   std::cout << "m_ringOffset = " << m_ringOffset << std::endl;

  // Make a ring. This is made of two half rings. They are identical but as
  // we need different identifiers they are made seperately.
  // We will refer to the two halves as inner and outer.
  // Inner will be the one closest to support when disk is on side of support
  // furthest from IP. z of Inner < z of Outer.
  // In later versions need to decide if I should rotate rings that are on IP side of
  // support by 180 around x (or y) axis.

  // Make sure we have even number of modules
  if (m_numModules % 2) std::cout << "SCT_FwdRing: Number of modules in ring must be even." << std::endl;


  // We define here the module with id = 0 as the nearest module to phi = 0.
  // It is at most  1/2 a division from 0, ie Between -0.5*divisionAngle to +0.5*divisionAngle.
  // For old number it was the first module with positive phi.

  // The parameter refStartAngle is the angle of any module.
  // refFirstStagger is whether this is an upper (+1) or lower (-1) module.
  // The stagger and angle of the module with id = 0, is calculated from this.

  double angle =  m_refStartAngle;
  // If disc is rotated then recalculate the angle.
  // It assumed the disc is rotated around the Y axis.  
  // TODO: Check this assumption. 
  if (m_discRotated) angle = Gaudi::Units::pi - angle;
  double divisionAngle = 2*Gaudi::Units::pi / m_numModules;

  // Now we choose module 0 as the first module with -0.5 * divAngle <  phi <=  0.5 * divAngle   
  double moduleCount = angle / divisionAngle;
  int moduleCountInt = static_cast<int>(floor(moduleCount +0.5 -0.0001)); // The -0.0001 allows slightly positive 
  // in case of rounding errors.
  m_startAngle = divisionAngle * (moduleCount - moduleCountInt);
  
  // Determine numbering for -ve endcap.
  // This is for a rotation around Y axis.
  // After rotation we want the first module closest to phi = 0.
  double angleNegEC = Gaudi::Units::pi - m_startAngle; 
  double moduleCountNegEC = angleNegEC / divisionAngle;
  m_moduleZero = static_cast<int>(floor(moduleCountNegEC + 0.5 - 0.0001));
  

  // Determine if it is an upper or lower.
  m_firstStagger = m_refFirstStagger;
  if (moduleCountInt % 2) m_firstStagger = -m_refFirstStagger;

  //   std::cout << "RingType, RingSide, Stereo, rotated = " << m_iRing << " " << m_ringSide << " "
  //          << m_stereoSign << " " << m_discRotated << std::endl;
  //   std::cout << "Ref   Start angle and stagger " << m_refStartAngle/Gaudi::Units::deg << " " << m_refFirstStagger << std::endl;
  //   std::cout << "First Start angle and stagger " << m_startAngle/Gaudi::Units::deg << " " << m_firstStagger << std::endl;
  //   std::cout << "Module zero in -ve endcap " << m_moduleZero << std::endl;


  makeModuleServices();

  // Make envelope for ring
  double moduleClearanceZ = 0.6 * Gaudi::Units::mm; // Arbitrary choice
  double moduleClearanceR = 0.5 * Gaudi::Units::mm; 

  m_innerRadius = m_module->innerRadius() - 0.5*m_module->stereoAngle()*(0.5*m_module->innerWidth()) - moduleClearanceR;
  m_outerRadius = sqrt(sqr(m_module->outerRadius()) + sqr(0.5*m_module->outerWidth())) 
    + 0.5*m_module->stereoAngle()*(0.5*m_module->outerWidth()) + moduleClearanceR;

  // Calculate clearance we have. NB. This is an approximate.
  //std::cout << "Module clearance (radial value does not take into account stereo rotation:" << std::endl;
  //std::cout << " radial: " << moduleClearanceR/Gaudi::Units::mm << " mm" << std::endl;
  //std::cout << " away from disc in z " << moduleClearanceZ/Gaudi::Units::mm << " mm" << std::endl;
  //std::cout << " Lo Module to cooling block: " << -m_moduleStagger-0.5*m_module->thickness() - m_moduleServicesLoOuterZPos << std::endl;
  //std::cout << " Hi Module to cooling block: " << +m_moduleStagger-0.5*m_module->thickness() - m_moduleServicesHiOuterZPos << std::endl;
  //std::cout << " Module to Module: " << m_moduleStagger-m_module->thickness() << std::endl;
  //std::cout << " towards disc in z "   
  //     << std::min(m_moduleStagger-m_module->thickness(), 
  //   std::min(-m_moduleStagger-0.5*m_module->thickness() - m_moduleServicesLoOuterZPos,
  //     +m_moduleStagger-0.5*m_module->thickness() - m_moduleServicesHiOuterZPos)) / Gaudi::Units::mm << " mm" << std::endl;

  m_thicknessOuter = 0.5 * m_module->thickness() + m_moduleStagger + moduleClearanceZ;
  m_thicknessInner = m_maxModuleServicesBaseToRingCenter + 2*m_safety;
  // We have to at least include 1*m_safety as the moduleservices envelope is increased by this amount.
  // m_maxModuleServicesBaseToRingCenter is calculated in makeModuleServices()
    
  m_thickness = m_thicknessOuter + m_thicknessInner;

  // We want the center in z to be at the module mid plane. So we shift the volume.
  double envelopeShift = -m_ringSide * (0.5 * m_thickness - m_thicknessOuter);

  SCT_MaterialManager materials;
  const GeoTube * tmpShape = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * m_thickness);
  const GeoShape & ringEnvelopeShape = (*tmpShape <<  GeoTrf::Translate3D(0, 0, envelopeShift));
  GeoLogVol * ringLog = new GeoLogVol(getName(), &ringEnvelopeShape, materials.gasMaterial());

  //std::cout << "m_innerRadius = " << m_innerRadius << std::endl;
  //std::cout << "m_outerRadius = " << m_outerRadius << std::endl;

  return ringLog;

}



GeoVPhysVol * 
SCT_FwdRing::build(SCT_Identifier id) const
{

  // Physical volume for the half ring
  GeoPhysVol * ring = new GeoPhysVol(m_logVolume);
  
  double deltaPhi = 360*Gaudi::Units::degree / m_numModules;
  bool negativeEndCap = (id.getBarrelEC() < 0);
  
  for (int i = 0; i < m_numModules; i++){

    // As used by the identifier
    int idNumber = i;

    // Alternate upper/lower
    int staggerUpperLower = m_firstStagger;
    if (i%2) staggerUpperLower = -m_firstStagger;

    // The negative endcap is rotated and so we have to play some tricks to get the
    // identifier numbering right.

    // In order to get the identifiers going in the direction of
    // increasing phi we have to invert them in the negative endcap.

    // Although the endcaps differ slightly (some upper/lower swaps) we build them in much the same
    // way and change just the numbering.

    // The id number for the detector element
    int idModule = idNumber;
 
    if (negativeEndCap) {
      // identifiers go in the opposite direction for the negative endcap.
      // We renumber so that module number "moduleZero" becomes zero.
      idModule = (m_numModules + m_moduleZero - idNumber) % m_numModules; 
    }

    //std::cout << "Endcap# = " <<id.getBarrelEC() 
    //       << ", idModule = " << idModule 
    //        << ", idModuleSimulation = " <<  idModuleSimulation 
    //       << ", idModule2 = " << (idModuleSimulation & 0xffff)
    //       << ", max    = " << ((idModuleSimulation & 0x00ff0000) >> 16)
    //       << ", moduleZero = " << ((idModuleSimulation & 0xff000000) >> 24)
    //       << std::endl;

    
    // The module is a TRD with length along z-axis. 
    // We need to rotate this so length is along the y-axis
    // This can be achieved with a 90 deg rotation around Y. 
    // This leaves the depth axis point in the -z direction which
    // is correct for modules mounted on the -ve side (side closest to the IP, ringSide  = -1).
    // For modules mounted on the opposite side we
    // rotate 180 around X so that the depth axis is pointing in the same direction as z.
    // Finally we rotate about z by phi and the 0.5*stereo (ie the u-phi or v-phi orientation)

    // It is assumed that the module is centered on the physics center (center of sensor)

    double phi = i * deltaPhi + m_startAngle;

    //std::cout << "Endcap# = " <<id.getBarrelEC() 
    //       << ", idModule = " << idModule 
    //       << ", i = " << i 
    //       << ", phi = " << phi/Gaudi::Units::degree << std::endl;

    GeoTrf::Transform3D rot = GeoTrf::RotateZ3D(phi + 0.5 * m_module->stereoAngle() * m_stereoSign);
    if (m_ringSide > 0) {
      rot = rot*GeoTrf::RotateX3D(180*Gaudi::Units::degree);
    }
    rot = rot*GeoTrf::RotateY3D(90*Gaudi::Units::degree);
    
    double zPos =  staggerUpperLower * m_moduleStagger * m_ringSide;
    GeoTrf::Vector3D xyz(m_module->sensorCenterRadius(), 0, zPos);
    xyz = GeoTrf::RotateZ3D(phi)*xyz;
    GeoTrf::Transform3D modulePos = GeoTrf::Translate3D(xyz.x(),xyz.y(),xyz.z())*rot;

    
    // Add the module
    std::string moduleName = "FwdModuleR" + intToString(m_iRing) + "#" + intToString(idModule);
    ring->add(new GeoNameTag(moduleName));
    ring->add(new GeoIdentifierTag(idModule));
    GeoAlignableTransform * moduleTransform = new GeoAlignableTransform(modulePos);
    ring->add(moduleTransform);
    id.setPhiModule(idModule);
    GeoVPhysVol * modulePV = m_module->build(id);
    ring->add(modulePV);

    // Store alignable transform
    detectorManager()->addAlignableTransform(1, id.getWaferId(), moduleTransform, modulePV);  

    // Add the moduleServices (contains the cooling block)
    // In principle this should also be rotated by the stereo angle (although one
    // would need to be care were the center of rotation is. However this is not
    // really necessary for the services so we do not bother.
    
    double zModuleServices = 0;
    double rModuleServices = 0;
    GeoVPhysVol * moduleServices = 0;
    if (staggerUpperLower > 0){ // Upper 
      zModuleServices =  m_moduleServicesHiZPos * m_ringSide;
      rModuleServices =  m_moduleServicesHiRPos;
      moduleServices  =   m_moduleServicesHi;
    } else { // Lower
      zModuleServices =  m_moduleServicesLoZPos * m_ringSide;
      rModuleServices =  m_moduleServicesLoRPos;
      moduleServices  =  m_moduleServicesLo;
    }
      
    
    ring->add(new GeoTransform(GeoTrf::RotateZ3D(phi)*GeoTrf::Translate3D(rModuleServices, 0, zModuleServices)));
    ring->add(moduleServices);
  
  }

  return ring;
}




// These are offset in z by m_moduleStagger and one is rotated relative to the other by the
// 360/m_numModules.



void 
SCT_FwdRing::makeModuleServices() 
{
  // Make an envelope to contain the two cooling blocks. Not sure if there much to gain by this 
  // rather than just adding the cooling blocks directly to the ring but it may help if we decide
  // to add more things to it later. We call it module services.
  
  // Cooling blocks for the upper Modules
  m_coolingBlockHiMain = new SCT_FwdCoolingBlock("CoolingBlkHiMain",SCT_FwdCoolingBlock::UPPER, SCT_FwdCoolingBlock::MAIN);
  m_coolingBlockHiSec  = new SCT_FwdCoolingBlock("CoolingBlkHiSec", SCT_FwdCoolingBlock::UPPER, SCT_FwdCoolingBlock::SECONDARY);
  // Cooling blocks for the lower Modules
  m_coolingBlockLoMain = new SCT_FwdCoolingBlock("CoolingBlkLoMain",SCT_FwdCoolingBlock::LOWER, SCT_FwdCoolingBlock::MAIN);
  m_coolingBlockLoSec  = new SCT_FwdCoolingBlock("CoolingBlkLoSec", SCT_FwdCoolingBlock::LOWER, SCT_FwdCoolingBlock::SECONDARY);
  
  double coolingBlkMainR = m_module->mainMountPointRadius();
  double coolingBlkSecR  = m_module->endModuleRadius(); // This is the end of the module. Align block with the end.
  double moduleServicesHiWidth = std::max(m_coolingBlockHiMain->rphi(), m_coolingBlockHiSec->rphi());
  double moduleServicesLoWidth = std::max(m_coolingBlockLoMain->rphi(), m_coolingBlockLoSec->rphi());
  double moduleServicesHiLength = std::abs(coolingBlkMainR - coolingBlkSecR) + 0.5 * m_coolingBlockHiMain->deltaR(); 
  double moduleServicesLoLength = std::abs(coolingBlkMainR - coolingBlkSecR) + 0.5 * m_coolingBlockLoMain->deltaR(); 
  double moduleServicesHiThickness = std::max(m_coolingBlockHiMain->thickness(), m_coolingBlockHiSec->thickness());
  double moduleServicesLoThickness = std::max(m_coolingBlockLoMain->thickness(), m_coolingBlockLoSec->thickness());
    
  // Radial position of this services volume. The calculation depends on whether the main cooling block is at the outer or inner radius.
  double moduleOrientation = (coolingBlkMainR > coolingBlkSecR) ? +1 : -1;
  m_moduleServicesHiRPos = coolingBlkMainR + moduleOrientation * (0.5 * m_coolingBlockHiMain->deltaR() - 0.5 * moduleServicesHiLength);
  m_moduleServicesLoRPos = coolingBlkMainR + moduleOrientation * (0.5 * m_coolingBlockLoMain->deltaR() - 0.5 * moduleServicesLoLength);

  // Radial position of the mid point of the secondary cooling block
  double coolingBlkHiSecRMid  = coolingBlkSecR + moduleOrientation * 0.5 * m_coolingBlockHiSec->deltaR();
  double coolingBlkLoSecRMid  = coolingBlkSecR + moduleOrientation * 0.5 * m_coolingBlockLoSec->deltaR();
  
  // z position. Set so the surface closest to the disc support is at a fixed distance relative to the disc support or ring center.
  // The distance between the disc surface and cooling block surface is obtained from the cooling block.
  // We average the number even though they are all the same. 
  double coolingBlockOffsetHi = 0.5 * (m_coolingBlockHiMain->offsetFromDisc() +  m_coolingBlockHiSec->offsetFromDisc());
  double coolingBlockOffsetLo = 0.5 * (m_coolingBlockLoMain->offsetFromDisc() +  m_coolingBlockLoSec->offsetFromDisc());
  double moduleServicesBaseToRingCenterHi = m_ringOffset - 0.5*m_discSupportThickness - coolingBlockOffsetHi;
  double moduleServicesBaseToRingCenterLo = m_ringOffset - 0.5*m_discSupportThickness - coolingBlockOffsetLo;
  m_maxModuleServicesBaseToRingCenter = std::max(moduleServicesBaseToRingCenterHi, moduleServicesBaseToRingCenterLo);
  m_moduleServicesHiZPos =  -(moduleServicesBaseToRingCenterHi - 0.5 * moduleServicesHiThickness);
  m_moduleServicesLoZPos =  -(moduleServicesBaseToRingCenterLo - 0.5 * moduleServicesLoThickness);

  // For checking clearance. Position of outer surface of module service with respect to the ring center.
  //  
  m_moduleServicesHiOuterZPos =  -(moduleServicesBaseToRingCenterHi - moduleServicesHiThickness);
  m_moduleServicesLoOuterZPos =  -(moduleServicesBaseToRingCenterLo - moduleServicesLoThickness);
  
  SCT_MaterialManager materials;
  const GeoBox * moduleServicesHiShape = new GeoBox(0.5*moduleServicesHiLength    + m_safety,  
                                                    0.5*moduleServicesHiWidth     + m_safety, 
                                                    0.5*moduleServicesHiThickness + m_safety);
  const GeoBox * moduleServicesLoShape = new GeoBox(0.5*moduleServicesLoLength    + m_safety,
                                                    0.5*moduleServicesLoWidth     + m_safety, 
                                                    0.5*moduleServicesLoThickness + m_safety);
  const GeoLogVol *  moduleServicesHiLog = new GeoLogVol("ModuleServicesHi", moduleServicesHiShape,  materials.gasMaterial());
  const GeoLogVol *  moduleServicesLoLog = new GeoLogVol("ModuleServicesLo", moduleServicesLoShape,  materials.gasMaterial());
    
  m_moduleServicesHi = new GeoPhysVol(moduleServicesHiLog);
  m_moduleServicesLo = new GeoPhysVol(moduleServicesLoLog);
    
  // Add the cooling blocks
  // Main Upper
  m_moduleServicesHi->add(new GeoTransform(GeoTrf::Translate3D(coolingBlkMainR - m_moduleServicesHiRPos, 0, 0)));
  m_moduleServicesHi->add(m_coolingBlockHiMain->getVolume());
  // Secondary Upper
  m_moduleServicesHi->add(new GeoTransform(GeoTrf::Translate3D(coolingBlkHiSecRMid - m_moduleServicesHiRPos, 0, 0)));
  m_moduleServicesHi->add(m_coolingBlockHiSec->getVolume());
  // Main Lower
  m_moduleServicesLo->add(new GeoTransform(GeoTrf::Translate3D(coolingBlkMainR - m_moduleServicesLoRPos, 0, 0)));
  m_moduleServicesLo->add(m_coolingBlockLoMain->getVolume());
  // Secondary Lower
  m_moduleServicesLo->add(new GeoTransform(GeoTrf::Translate3D(coolingBlkLoSecRMid - m_moduleServicesLoRPos, 0, 0)));
  m_moduleServicesLo->add(m_coolingBlockLoSec->getVolume());
}
