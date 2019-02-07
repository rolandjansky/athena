/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_FwdWheel.h"
#include "SCT_SLHC_GeoModel/SCT_MaterialManager.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeneralParameters.h"
#include "SCT_SLHC_GeoModel/SCT_FwdRing.h"
#include "SCT_SLHC_GeoModel/SCT_FwdDiscSupport.h"

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

namespace InDetDDSLHC {

inline double sqr(double x) {return x*x;}

SCT_FwdWheel::SCT_FwdWheel(const std::string & name, int iWheel)	
  : SCT_UniqueComponentFactory(name), m_iWheel(iWheel){
  getParameters();
  m_logVolume = preBuild(); 
  
}

SCT_FwdWheel::~SCT_FwdWheel(){
  delete m_discSupport;
  for(int iRing=0; iRing<m_numRings; iRing++){
    delete m_rings[iRing];
  }
}

void SCT_FwdWheel::getParameters(){

  const SCT_ForwardParameters* parameters = geometryManager()->forwardParameters();
  const SCT_GeneralParameters * generalParameters = geometryManager()->generalParameters();
  
  m_safety       = generalParameters->safety();
  m_zPosition = parameters->fwdWheelZPosition(m_iWheel);
  m_numRings  = parameters->fwdWheelNumRings(m_iWheel);
  //m_numRingsMax  = parameters->fwdWheelNumRingsMax(m_iWheel);
  for(int iRing=0; iRing<m_numRings; iRing++){
    int ringType = parameters->fwdWheelRingType(m_iWheel, iRing); 
    m_ringOffset.push_back(parameters->fwdRingOffset(ringType));
    m_ringTypes.push_back(ringType);
  }
  m_staggerGap = 0.001* Gaudi::Units::mm;

  // Set numerology
  detectorManager()->numerology().setNumRingsForDisk(m_iWheel,m_numRings);  
}

const GeoLogVol* SCT_FwdWheel::preBuild(){
  // Create disc support. 
  m_discSupport = new SCT_FwdDiscSupport("DiscSupport"+intToString(m_iWheel), m_iWheel);

  // The rings
  for(int iRing=0; iRing<m_numRings; iRing++){
    std::string ringName = "Ring"+intToString(iRing)
      +"For"+getName();
    m_rings.push_back(new SCT_FwdRing(ringName, m_iWheel, 
				      m_ringTypes[iRing], 
				      m_ringOffset[iRing]));
  }
  //Calculate the extent of the envelope  
  //start the support disc
  //m_outerRadius = m_discSupport->outerRadius() + 0.52*Gaudi::Units::cm;//0.01mm safety necessary
  //m_innerRadius = m_discSupport->innerRadius() - 0.52*Gaudi::Units::cm;//0.01mm safety necessary
  m_outerRadius = m_discSupport->outerRadius() + 9*Gaudi::Units::mm;//0.01mm safety necessary
  m_innerRadius = m_discSupport->innerRadius() - 9*Gaudi::Units::mm;//0.01mm safety necessary
  //then comsider rings
  double wheelThickness_neg = -1.0;//negative value! see code below
  double wheelThickness_pos = -1.0;

  for(int iRing=0; iRing<m_numRings; iRing++){

    // Set numerology
    detectorManager()->numerology().setNumPhiModulesForDiskRing(m_iWheel,iRing,m_rings[iRing]->numModules());

    if(m_rings[iRing]->outerRadius() > m_outerRadius || 
       m_rings[iRing]->innerRadius() < m_innerRadius){
      std::cout<<"SCT_FwdWheel::ERROR prolem with radius:"<<std::endl;
      std::cout<<"either m_rings["<<iRing<<"]->outerRadius() "
	       <<m_rings[iRing]->outerRadius()<<" >? m_outerRadius "<<m_outerRadius
	       <<" or m_rings["<<iRing<<"]->innerRadius() "
	       <<m_rings[iRing]->innerRadius()<<" <? m_innerRadius "
	       <<m_innerRadius<<"!!"<<std::endl;
      exit(1);
    }
    //ringside is 1 if the ring meets the beam before support disk
    m_ringSide = (m_ringOffset[iRing]>=0)? 1 : -1;
    //recalculte the wheel thickness: take in acount the ring position, side and thickness
    //the ring offset is measured from the center of disc support to the the center of the ring
    if(m_ringSide == 1){
      //wheel positive thickness
      //--double ringThickness_pos = 0.5*m_rings[iRing]->thickness() + fabs(m_ringOffset[iRing]);
      double ringThickness_pos = m_rings[iRing]->thickness() + fabs(m_ringOffset[iRing]);
      //take account of the stagger
      ringThickness_pos = ringThickness_pos + m_rings[iRing]->thickness() + m_staggerGap;
      wheelThickness_pos = std::max(wheelThickness_pos, ringThickness_pos);
    }else{
      //wheel negative thickness
      double ringThickness_neg = m_rings[iRing]->thickness() + fabs(m_ringOffset[iRing]);
      //take account of the stagger
      ringThickness_neg = ringThickness_neg + m_rings[iRing]->thickness() + m_staggerGap;
      wheelThickness_neg = std::max(wheelThickness_neg, ringThickness_neg);
    }
  }//end of  for(int iRing=0; 
  m_thickness = m_discSupport->thickness();
  //in case there is no ring on one side or the other, consider the thickness of the disc
  if(wheelThickness_neg > 0) m_thickness = m_thickness + wheelThickness_neg;
  if(wheelThickness_pos > 0) m_thickness = m_thickness + wheelThickness_pos;
  //now, if the wheel is not symetric (ie number of ring is not similar on both sides)
  //one should shift the position of the support disc to one or another direction.
  //1- rings on both sides: 
  if(wheelThickness_neg>0 && wheelThickness_pos>0)
    m_discSupportZpos = 0.5*(wheelThickness_neg - wheelThickness_pos);
  //2- rings on the negative side
  else if(wheelThickness_neg > 0) m_discSupportZpos =  0.5*(m_thickness-m_discSupport->thickness());
  //3- ring on the positive side
  else if(wheelThickness_pos > 0) m_discSupportZpos = -0.5*(m_thickness-m_discSupport->thickness());

  m_thickness += m_safety;

  //protection along Z for support disc
  if(fabs(m_discSupportZpos)+0.5*m_discSupport->thickness()>0.5*m_thickness){
    std::cout<<"SCT_FwdWheel.cxx: disc edge outside envelop of wheel "
	     <<m_iWheel<<": half wheel thickness "<<0.5*m_thickness
	     <<", disc_Z "<<m_discSupportZpos
	     <<", halfdiscthickness "<<0.5*m_discSupport->thickness()
	     <<std::endl;
    exit(1);
  }
  // Make envelope for the wheel
  SCT_MaterialManager * materials = geometryManager()->materialManager();

  const GeoTube* fwdWheelEnvelope = new GeoTube(m_innerRadius, m_outerRadius, 0.5*m_thickness);
  const GeoLogVol* fwdWheelLog = new GeoLogVol(getName(), fwdWheelEnvelope, 
					       materials->gasMaterial());
  return fwdWheelLog;
}

GeoVPhysVol* SCT_FwdWheel::build(SCT_Identifier id) const{

  GeoFullPhysVol* wheel = new GeoFullPhysVol(m_logVolume);
  //support disc
  wheel->add(new GeoTransform(GeoTrf::Translate3D(0, 0, m_discSupportZpos)));
  wheel->add(m_discSupport->getVolume());

  //rings
  //apply a stagger to rings to avoid clash in R direction
  double ring_stagger = 0;
  //std::cerr<<"\n--------wheel "<<m_iWheel<<", nb of ring "<<m_numRings<<std::endl;
  for(int iRing=0; iRing<m_numRings; iRing++){
    const SCT_FwdRing* ring = m_rings[iRing];
    //save ring stagger: calculated from even rings
    if(iRing%2 == 0) ring_stagger = ring->thickness();
    /* 
    //protection along Z
    if(fabs(m_ringOffset[iRing]) <= ring->thickness()){
    std::cout<<"SCT_FwdWheel.cxx: ring offset should be > than ring thickness, "
    <<"m_ringOffset["<<iRing<<"] = "<<fabs(m_ringOffset[iRing])
    <<", ring thickness = "<<ring->thickness()
    <<". exit athena!"<<std::endl;
    exit(1);
    }
    */
    double disk_ring_halfthickness = 0.5*m_discSupport->thickness()
      +0.5*ring->thickness();

    double ring_offset = std::abs(m_ringOffset[iRing])+disk_ring_halfthickness;
    //put ring on the left (neg side) or the right (pos side) of the support disk
    ring_offset = m_ringSide*ring_offset;
    //position ring relative to the support disc position
    double ring_z_pos = ring_offset+m_discSupportZpos;
    //apply ring stagger to only odd rings
    if(iRing%2 == 1) 
	ring_z_pos = ring_z_pos + m_ringSide*(ring_stagger + m_staggerGap);

    //protection along Z direction
    double disc_ring_dist = fabs(m_discSupportZpos - ring_z_pos) 
      - disk_ring_halfthickness;
    
    if(fabs(disc_ring_dist)<0){
      std::cout<<"SCT_FwdWheel.cxx: overlap between support "
	       <<"disk in wheel "<<m_iWheel<<" and ring "
	       <<iRing<<": disc_ring_dist "<<disc_ring_dist
	       <<", wheel thickness "<<m_thickness
	       <<", disc thickness "<<m_discSupport->thickness()
	       <<", ring thickness "<<ring->thickness()
	       <<", Z_ring  "<<ring_z_pos
	       <<", Z_disk "<<m_discSupportZpos
	       <<", disk_ring_half_thikness "<<disk_ring_halfthickness
	       <<". exit athena!"<<std::endl;
      exit(1);
    }
    //protection along Z for ring, check if it passes the wheel edge
    if(fabs(ring_z_pos)+0.5*ring->thickness()>0.5*m_thickness){
      std::cout<<"SCT_FwdWheel.cxx: ring "<<iRing
	       <<"'s edge is outside envelop of wheel "
	       <<m_iWheel<<": half wheel thickness "<<0.5*m_thickness
	       <<", ring_Z "<<ring_z_pos
	       <<", halfringthickness "<<0.5*ring->thickness()
	       <<std::endl;
      exit(1);
    }
    //each wheel drops a ring, that's why ring_id (ie eta) is calculated in the following way
    //(ie the second ring from a wheel and the first ring from a following wheel in a region 
    //(SS or LS) have the same eta.
    //int ring_id = iRing + (m_numRingsMax - m_numRings);
    int ring_id = iRing;
    wheel->add(new GeoNameTag("Ring#" + intToString(ring_id)));
    wheel->add(new GeoIdentifierTag(ring_id));
    wheel->add(new GeoTransform(GeoTrf::Translate3D(0, 0, ring_z_pos)));
    //std::cerr<<", ring "<<ring_id;
    id.setEtaModule(ring_id);
    //--id.setEtaModule(iRing);
    //std::cerr<<"ring-eta "<<iRing<<std::endl;
    wheel->add(ring->build(id));
    /*stagger has been introduced. this protection is obsolete!
    //protection along R
    if(iRing>0 && previousRingOffSet==m_ringOffset[iRing])
      if(previousRingOuterRadius > ring->innerRadius()){
	std::cout<<"SCT_FwdWheel.cxx: clash between two rings in wheel "<<m_iWheel
		 <<": RingOuterRadius["<<iRing-1<<"] = "<<previousRingOuterRadius
		 <<"> RingInnerRadius["<<iRing<<"] = "<<ring->innerRadius()
		 <<"!!! exit athena."<<std::endl;
	exit(1);
      }
    */
  }//end of for(int iRing=0; ...)
  return wheel;
}
}
