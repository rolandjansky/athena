/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// CPW 17/8/06
// Version using new model of services from A. Tricoli
// 
#include "SCT_GeoModel/SCT_Layer.h"
#include "SCT_GeoModel/SCT_Identifier.h"
#include "SCT_GeoModel/SCT_GeometryManager.h"
#include "SCT_GeoModel/SCT_MaterialManager.h"
#include "SCT_GeoModel/SCT_BarrelParameters.h"
#include "SCT_GeoModel/SCT_GeneralParameters.h"
#include "SCT_GeoModel/SCT_Bracket.h"
#include "SCT_GeoModel/SCT_CoolingEnd.h"
#include "SCT_GeoModel/SCT_Clamp.h"
#include "SCT_GeoModel/SCT_Flange.h"
#include "SCT_GeoModel/SCT_Harness.h"
#include "SCT_GeoModel/SCT_Module.h"
#include "SCT_GeoModel/SCT_Ski.h"
#include "SCT_GeoModel/SCT_SkiAux.h"
#include "SCT_GeoModel/SCT_SkiPowerTape.h"
#include "SCT_GeoModel/SCT_SupportCyl.h"
#include "SCT_GeoModel/SCT_FSIEndJewel.h"
#include "SCT_GeoModel/SCT_FSIScorpion.h"
#include "SCT_GeoModel/SCT_FSIFibreMask.h"

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
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

inline double sqr(double x) {return x * x;}

SCT_Layer::SCT_Layer(const std::string & name,
                     int iLayer,
                     SCT_Module * module,
                     InDetDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_iLayer(iLayer), 
    m_module(module)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Layer::~SCT_Layer()
{
  delete m_bracket;
  delete m_clamp;
  delete m_coolingEnd;
  delete m_flange;
  delete m_harness;
  delete m_ski;
  delete m_skiAux;
  delete m_skiPowerTape;
  delete m_supportCyl;
  if (m_includeFSI) {
    delete m_endJewel;
    delete m_scorpion;
    delete m_fibreMask;
  }
}

void
SCT_Layer::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();
    
  m_safety       = generalParameters->safety();
  m_radius       = parameters->radius(m_iLayer);
  m_skisPerLayer = parameters->skisPerLayer(m_iLayer);
  m_tilt         = parameters->tilt(m_iLayer);
  m_stereoSign   = parameters->layerStereoSign(m_iLayer);
  m_bracketPhiOffset = parameters->layerBracketPhiOffset(m_iLayer);
  m_phiRefModule = parameters->layerPhiOfRefModule(m_iLayer);

  m_outerRadiusOfSupport = parameters->supportCylOuterRadius(m_iLayer);
  m_activeLength = parameters->activeLength();
  m_cylinderLength = parameters->cylinderLength();

  m_includeFSI = parameters->includeFSI();
  if (m_includeFSI) {
    m_nRepeatEndJewel = parameters->fsiEndJewelNRepeat(m_iLayer);
    m_phiEndJewel = parameters->fsiEndJewelPhi(m_iLayer);
    m_zEndJewel = parameters->fsiEndJewelZ(m_iLayer);
    m_nRepeatScorpion = parameters->fsiScorpionNRepeat(m_iLayer);
    m_phiScorpion = parameters->fsiScorpionPhi(m_iLayer);
    m_zScorpion = parameters->fsiScorpionZ(m_iLayer);
  }

  // Set numerology
  m_detectorManager->numerology().setNumPhiModulesForLayer(m_iLayer,m_skisPerLayer);
  m_detectorManager->numerology().setNumEtaModulesForLayer(m_iLayer,parameters->modulesPerSki());

}



const GeoLogVol * 
SCT_Layer::preBuild()
{
  // Build the components required for the layer.
  // We use the layer number as a string quite a bit
  std::string layerNumStr = intToString(m_iLayer);

  // Build the Flanges
  m_flange     = new SCT_Flange("Flange"+layerNumStr, m_iLayer, m_detectorManager, m_geometryManager, m_materials);
  
  // Build the SupportCyl
  m_supportCyl = new SCT_SupportCyl("SupportCyl"+layerNumStr, m_iLayer, m_cylinderLength,
                                    m_detectorManager, m_geometryManager, m_materials);

  // Build the FSI end jewel, scorpion and fibre mask
  // Mask runs between scorpions and flange in z - must be built after these
  if (m_includeFSI) {
    m_endJewel = new SCT_FSIEndJewel("FSIEndJewel"+layerNumStr, m_detectorManager, m_geometryManager, m_materials);
    m_scorpion = new SCT_FSIScorpion("FSIScorpion"+layerNumStr, m_detectorManager, m_geometryManager, m_materials);
    double length_mask = 0.5*m_cylinderLength - m_flange->length() - m_zScorpion - 0.5*m_scorpion->length();
    m_fibreMask = new SCT_FSIFibreMask("FSIFibreMask"+layerNumStr, m_iLayer, length_mask,
                                       m_detectorManager, m_geometryManager, m_materials);
  }
  else {
    m_endJewel = nullptr;
    m_scorpion = nullptr;
    m_fibreMask = nullptr;
  }

  //
  // Calculations for making active layer components - called ski.
  // This is the modules + doglegs + cooling blocks + coolingpipe 
  // 
  double divisionAngle  = 360. * Gaudi::Units::degree / m_skisPerLayer;

  // We define here the first module(id = 0) as the nearest module to phi = 0 with positive phi.
  // We allow slightly negative in case of rounding errors.
 
  double moduleCount = m_phiRefModule / divisionAngle;
  m_skiPhiStart = divisionAngle * (moduleCount - floor(moduleCount +0.5 -0.0001));
    
  m_skiAuxPhiStart = 0;

  // Make the ski
  // The ski length is now reduced to m_activeLength to make room for the cooling inlet/outlet volumes
  m_ski = new SCT_Ski("Ski"+layerNumStr, m_module, m_stereoSign, m_tilt, m_activeLength,
                      m_detectorManager, m_geometryManager, m_materials);

  //
  // Make SkiAux -  This is the brackets, harness and power tape.
  //
  // Bracket is placed at edge of division. 
  // -tiltSign * (r*divisionAngle/2 - bracket_width/2)
  // Works for both +ve and -ve tilt.
  m_bracket = new SCT_Bracket("Bracket"+layerNumStr, m_detectorManager, m_geometryManager, m_materials);

  m_harness = new SCT_Harness("Harness"+layerNumStr, m_cylinderLength,
                              m_detectorManager, m_geometryManager, m_materials);
  m_skiPowerTape = new SCT_SkiPowerTape("SkiPowerTape"+layerNumStr, m_ski, m_cylinderLength,
                                        m_detectorManager, m_geometryManager, m_materials);

  int tiltSign = (m_tilt < 0) ? -1 : +1;
  
  double bracketOffset   = m_skiPhiStart - tiltSign * m_bracketPhiOffset;
  double powerTapeOffset = bracketOffset - tiltSign * 0.5*divisionAngle; 
  // tiltSign not really necessary in powerTapeOffset calculate
  // - but keeps the bracket and powertape pair associated with the same ski
  
 
  // Make the SkiAux. This is layer dependent.
  m_skiAux = new SCT_SkiAux("SkiAux"+layerNumStr, 
                            m_ski, 
                            m_bracket,
                            m_harness,
                            m_skiPowerTape, 
                            m_outerRadiusOfSupport,
                            bracketOffset, 
                            powerTapeOffset,
                            divisionAngle,
                            m_detectorManager,
                            m_geometryManager,
                            m_materials);

  // Build the clamp: we cannot do this until we have the dimensions of SkiAux
  m_clamp = new SCT_Clamp("Clamp"+layerNumStr, m_iLayer, m_skiAux->outerRadius(),
                          m_detectorManager, m_geometryManager, m_materials);

  // Build the volume representing the cooling inlets, outlet and U-bends.
  // We cannot do this until we have the dimensions of the clamp
  double coolingInnerRadius = m_clamp->outerRadius();
  double clearance = 1*Gaudi::Units::mm;
  double coolingLength = 0.5*m_cylinderLength - 0.5*m_activeLength - clearance;
  m_coolingEnd = new SCT_CoolingEnd("CoolingEnd"+layerNumStr, m_iLayer, coolingInnerRadius, coolingLength,
                                    m_detectorManager, m_geometryManager, m_materials);

  //
  //  Calculate the envelopes.
  //


  //
  // Active Layer Envelope extent
  // Envelope for the active part (ie containing all the skis for the layer)
  //
  double rMinActive, rMaxActive;

  // Returns the min and max radius for the active envelope
  activeEnvelopeExtent(rMinActive, rMaxActive);

  // Add some safety
  rMinActive  -= m_safety;
  rMaxActive  += m_safety;

  // But active layer now includes clamp....
  rMinActive = m_skiAux->outerRadius();

  m_innerRadiusActive = rMinActive;
  m_outerRadiusActive = rMaxActive;

  //
  // Extent Envelope for support cyl, flanges and close-outs. 
  //
  double rMinSupport = std::min(m_supportCyl->innerRadius(),m_flange->innerRadius());
  //
  // Overall Layer Envelope extent
  //
  // Inner radius is inner radius of support.
  m_innerRadius = rMinSupport;
  m_outerRadius = m_outerRadiusActive;

  //
  // Make envelope for layer; length is now same as support cylinder
  //
  double length = m_cylinderLength;
  //  double length = m_layerLength;
  //  if (m_iLayer == 0) {length = m_cylinderLength + m_safety;}
  const GeoTube * layerEnvelopeTube = new GeoTube(m_innerRadius, m_outerRadius, 0.5 * length);

  GeoLogVol * layerLog = new GeoLogVol(getName(), layerEnvelopeTube, m_materials->gasMaterial());


  // Check for overlap.
  if (m_skiAux->outerRadius() > rMinActive) {
    std::cout << "----> WARNING: SCT_Layer: Overlap between active layer and aux layer." << std::endl;
  }

  return layerLog;
}

GeoVPhysVol * 
SCT_Layer::build(SCT_Identifier id)
{
  // We make this a fullPhysVol for alignment code.
  GeoFullPhysVol * layer = new GeoFullPhysVol(m_logVolume);

  double divisionAngle  = 360 * Gaudi::Units::degree / m_skisPerLayer;

  //
  // Active Layer
  //
  // Make envelope for active layer
  //
  const GeoTube * activeLayerEnvelopeShape = new GeoTube(m_innerRadiusActive, m_outerRadiusActive, 0.5 * m_cylinderLength);
  GeoLogVol * activeLayerLog = new GeoLogVol(getName()+"Active", activeLayerEnvelopeShape, m_materials->gasMaterial());
  GeoPhysVol * activeLayer = new GeoPhysVol(activeLayerLog);

  for (int iSki = 0; iSki < m_skisPerLayer; iSki++){
    std::ostringstream name; name << "Ski#" << iSki;
   
    double phi = m_skiPhiStart + iSki * divisionAngle;

    GeoTrf::Vector3D pos(m_radius, 0, 0);
    pos = GeoTrf::RotateZ3D(phi)*pos;
    GeoTrf::Transform3D rot = GeoTrf::RotateZ3D(m_tilt)*GeoTrf::RotateZ3D(phi);
    
    // Because the ski envelope center is not positioned at the rotation axis for the ski we must first
    // apply the inverse of refPointTransform() of the ski.
    GeoTrf::Transform3D trans(GeoTrf::Transform3D(GeoTrf::Translate3D(pos.x(),pos.y(),pos.z())*rot) * m_ski->getRefPointTransform()->getTransform().inverse());

    activeLayer->add(new GeoAlignableTransform(trans));
    activeLayer->add(new GeoNameTag(name.str()));
    activeLayer->add(new GeoIdentifierTag(iSki));
    id.setPhiModule(iSki);
    activeLayer->add(m_ski->build(id));
  }

  // And add the service material 
  double clampZPos = 0.5 * m_cylinderLength - 0.5 *  m_clamp->length();
  activeLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(clampZPos)));
  activeLayer->add(m_clamp->getVolume());
  activeLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(-clampZPos)));
  activeLayer->add(m_clamp->getVolume());

  double coolingZPos = 0.5 * m_cylinderLength - 0.5 *  m_coolingEnd->length();
  activeLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(coolingZPos)));
  activeLayer->add(m_coolingEnd->getVolume());
  activeLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(-coolingZPos)));
  activeLayer->add(m_coolingEnd->getVolume());

  //
  // Aux Layer
  // Envelope for brackets and powertapes.
  //
  const GeoTube * auxLayerEnvelopeShape = new GeoTube(m_skiAux->innerRadius(), m_skiAux->outerRadius(),
                                                      0.5*m_skiAux->length());
  GeoLogVol * auxLayerLog = new GeoLogVol(getName()+"Aux", auxLayerEnvelopeShape, m_materials->gasMaterial());
  GeoPhysVol * auxLayer = new GeoPhysVol(auxLayerLog);

  for (int iSki = 0; iSki < m_skisPerLayer; iSki++){
    //for (int iSki = 0; iSki < 2; iSki++){
    double phi =  m_skiAuxPhiStart + iSki * divisionAngle;
    auxLayer->add(new GeoTransform(GeoTrf::RotateZ3D(phi)));
    auxLayer->add(m_skiAux->getVolume());
  }
 
 
  //
  // Support Layer
  //

  // Envelope for support cylinder, flanges and FSI.
  // Layer0 no longer needs cut-out 
  //
  const GeoTube * supportLayerTube = new GeoTube(m_innerRadius, m_outerRadiusOfSupport, 0.5 * m_cylinderLength); 
  GeoLogVol * supportLayerLog = new GeoLogVol(getName()+"Support", supportLayerTube, 
                                              m_materials->gasMaterial());
  GeoPhysVol * supportLayer = new GeoPhysVol(supportLayerLog);
  
  // Position flanges. One at each end.
  double flangeZPos = 0.5 * m_cylinderLength - 0.5 *  m_flange->length();
  supportLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(flangeZPos)));
  supportLayer->add(m_flange->getVolume());
  supportLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(-flangeZPos)));
  supportLayer->add(m_flange->getVolume());

  // Position supportCyl
  supportLayer->add(m_supportCyl->getVolume());

  if(m_includeFSI) {
    // Position FSI fibre masks
    double fibreMaskZPos = 0.5 * m_cylinderLength - m_flange->length() - 0.5 * m_fibreMask->length();
    supportLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(fibreMaskZPos)));
    supportLayer->add(m_fibreMask->getVolume());
    supportLayer->add(new GeoTransform(GeoTrf::TranslateZ3D(-fibreMaskZPos)));
    supportLayer->add(m_fibreMask->getVolume());

    // Position FSI End jewels
    double jewelRadius = std::sqrt(m_fibreMask->innerRadius()*m_fibreMask->innerRadius() - 0.25 * m_endJewel->rPhiWidth()*m_endJewel->rPhiWidth()) - 0.5 * m_endJewel->radialWidth();
    for ( int i=0; i<m_nRepeatEndJewel; i++) {
      double jewelAngle = m_phiEndJewel + i * 360.*Gaudi::Units::degree/m_nRepeatEndJewel;
      supportLayer->add(new GeoTransform(GeoTrf::RotateZ3D(jewelAngle)*GeoTrf::TranslateX3D(jewelRadius)*GeoTrf::TranslateZ3D(m_zEndJewel)));
      supportLayer->add(m_endJewel->getVolume());
      supportLayer->add(new GeoTransform(GeoTrf::RotateZ3D(jewelAngle)*GeoTrf::TranslateX3D(jewelRadius)*GeoTrf::TranslateZ3D(-m_zEndJewel)));
      supportLayer->add(m_endJewel->getVolume());
    }

    // Position FSI Scorpions
    double scorpionRadius = std::sqrt(m_supportCyl->innerRadius()*m_supportCyl->innerRadius() - 0.25 * m_scorpion->rPhiWidth()*m_scorpion->rPhiWidth()) - 0.5 * m_scorpion->radialWidth();
    for ( int i=0; i<m_nRepeatScorpion; i++) {
      double scorpionAngle = m_phiScorpion + i * 360.*Gaudi::Units::degree/m_nRepeatScorpion;
      supportLayer->add(new GeoTransform(GeoTrf::RotateZ3D(scorpionAngle)*GeoTrf::TranslateX3D(scorpionRadius)*GeoTrf::TranslateZ3D(m_zScorpion)));
      supportLayer->add(m_scorpion->getVolume());
      supportLayer->add(new GeoTransform(GeoTrf::RotateZ3D(scorpionAngle)*GeoTrf::TranslateX3D(scorpionRadius)*GeoTrf::TranslateZ3D(-m_zScorpion)));
      supportLayer->add(m_scorpion->getVolume());
    }
  }

  // Extra Material
  InDetDD::ExtraMaterial xMat(m_geometryManager->distortedMatManager());
  xMat.add(supportLayer, "SCTLayer"+intToString(m_iLayer));


  // Now place all the sub layers into the overall layer.
  layer->add(activeLayer);
  layer->add(auxLayer);
  layer->add(supportLayer);

  return layer;
}

void
SCT_Layer::activeEnvelopeExtent(double & rmin, double & rmax)
{

  // These are the coordinates of the corners of the ski envelope.
  // x is in the radial direction and x is in the phi direction.

  //GeoTrf::Vector3D c0();
  GeoTrf::Vector3D c1(-(m_ski->env1RefPointVector()->x()) - 0.5*(m_ski->env1Thickness()),
                       -(m_ski->env1RefPointVector()->y()) + 0.5*(m_ski->env1Width()),
                       0.0);
  GeoTrf::Vector3D c2(-(m_ski->env2RefPointVector()->x()) - 0.5*(m_ski->env2Thickness()),
                       -(m_ski->env2RefPointVector()->y()) + 0.5*(m_ski->env2Width()),
                       0.0);
  //GeoTrf::Vector3D c3();
  GeoTrf::Vector3D c4(-(m_ski->env1RefPointVector()->x()) + 0.5*(m_ski->env1Thickness()),
                       -(m_ski->env1RefPointVector()->y()) - 0.5*(m_ski->env1Width()),
  0.0);

  //c0.rotateZ(m_tilt);
  c1 = GeoTrf::RotateZ3D(m_tilt)*c1;
  c2 = GeoTrf::RotateZ3D(m_tilt)*c2;
  //c3.rotateZ(m_tilt);
  c4 = GeoTrf::RotateZ3D(m_tilt)*c4;

  GeoTrf::Vector3D vxmax = c4;
  GeoTrf::Vector3D vxmin;
  if (c1.x() < c2.x()) {
    vxmin = c1;
  }
  else {
    vxmin = c2;
  }

  double xmax = vxmax.x();
  double xmin = vxmin.x();
  double ymax = vxmax.y();
  double ymin = vxmin.y();

  rmax = sqrt(sqr(m_radius + xmax) + sqr(ymax));
  rmin = sqrt(sqr(m_radius + xmin) + sqr(ymin));
}
// *** End of modified lines. ------------------ (07)*********************************

double 
SCT_Layer::calcSkiPhiOffset()
{
  // Calculate skiPhiOffset such that active silicon touches edge of division
  // This is what is done in AGE.

  // First calculated for abs(m_tilt). 

  double divisionAngle  = 360 * Gaudi::Units::degree / m_skisPerLayer;

  // double activeHalfWidth =     0.5 * m_skiAux->ski()->module()->activeWidth();
  // double moduleHalfThickness = 0.5 * m_skiAux->ski()->module()->thickness();
  double activeHalfWidth =     0.5 * m_module->activeWidth();
  double moduleHalfThickness = 0.5 * m_module->thickness();

  // rotate top points by tilt xin,yin (-X,+Y) and xout,yout (+X,+Y)
  double ctilt = std::abs(cos(m_tilt));
  double stilt = std::abs(sin(m_tilt));
  double xin  = m_radius
                - ctilt * moduleHalfThickness - stilt * activeHalfWidth;
  double yin  = - stilt * moduleHalfThickness + ctilt * activeHalfWidth;
  double xout = m_radius 
                + ctilt * moduleHalfThickness - stilt * activeHalfWidth;
  double yout =   stilt * moduleHalfThickness + ctilt * activeHalfWidth;
  double alpha_in  = atan(yin/xin);
  double alpha_out = atan(yout/xout);
  double alpha = std::max(alpha_in, alpha_out);
  
  int tiltSign = (m_tilt < 0) ? -1 : 1;
  // As ski is larger than (ie alpha > divisionAngle/2) 
  // skiPhiOffset < 0 for +ve tilt and > 0 for -ve tilt.
  double skiPhiOffset = tiltSign * (0.5 * divisionAngle - alpha);

  return skiPhiOffset;
}
