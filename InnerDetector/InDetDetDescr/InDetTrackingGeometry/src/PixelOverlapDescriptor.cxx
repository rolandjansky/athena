/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelOverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// InDet
#include "InDetTrackingGeometry/PixelOverlapDescriptor.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"

InDet::PixelOverlapDescriptor::PixelOverlapDescriptor(double l1frac, 
                                                      double l2frac, 
                                                      bool checkLC, 
                                                      const Trk::BinnedArray<Trk::Surface>* ba, 
                                                      const PixelID* pixIdHelper,
                                                      const InDetDD::SiDetectorManager* pixMgr,
                                                      int robustOverlapMode) :
  m_checkLocalCoords(checkLC),
  m_loc1fraction(l1frac),
  m_loc2fraction(l2frac),
  m_binnedSurfaceArray(ba),
  m_pixMgr(pixMgr),
  m_pixIdHelper(pixIdHelper),
  m_robustOverlapMode(robustOverlapMode)
{
  // reserve the maximum number
  m_overlapCells.reserve(5);
}


const Trk::OverlapCell InDet::PixelOverlapDescriptor::overlapSurface(const Trk::TrackParameters& tp,
                                                                     const Trk::Surface* sfreq,
                                                                     Trk::PropDirection dir) const
{
  // the unique break condition
  if (sfreq == 0) return Trk::OverlapCell(0,dir);

  const Trk::Surface& parameterSurface = tp.associatedSurface();

  const InDetDD::SiDetectorElement* sidetElement = dynamic_cast<const InDetDD::SiDetectorElement*>(parameterSurface.associatedDetectorElement());

  if (sidetElement){
    double phiSurface  = parameterSurface.center().phi();
    double phiPosition = tp.position().phi();

     // make phi safe
     phiSurface   += M_PI;
     phiPosition  += M_PI;

     const InDetDD::SiDetectorElement* nextPhi = (phiPosition > phiSurface) ? sidetElement->nextInPhi() : sidetElement->prevInPhi();
     const Trk::Surface* phiIncreaseSurface = (nextPhi) ? &(nextPhi->surface(nextPhi->identify())) : 0;
     if (phiIncreaseSurface != sfreq && phiIncreaseSurface != &parameterSurface)
         return Trk::OverlapCell(phiIncreaseSurface,dir);

  }
  return Trk::OverlapCell(0,dir);
}

const std::vector<Trk::OverlapCellCondition>& InDet::PixelOverlapDescriptor::overlapCells(const Trk::TrackParameters& tp,
                                                                                          Trk::PropDirection dir,
                                                                                          const Trk::Surface* startSurface,
                                                                                          const Trk::Surface* endSurface) const
{
  // clear the vector from last call
  m_overlapCells.clear();

  // the direction flip :
  //  - while you search the next in phi along momentum (i.e. with start surface)
  //  - you search the previous in phi opposite momentum (i.e. with end surface)
  double skipDirection = endSurface ? -1. : 1. ;

  // the associated surface to the start parameters of the search
  const Trk::Surface& parameterSurface = tp.associatedSurface();

  // get the according detector element
  const InDetDD::SiDetectorElement* sidetElement 
    = dynamic_cast<const InDetDD::SiDetectorElement*>(parameterSurface.associatedDetectorElement());

  // return empty cell vector
  if (!sidetElement) return m_overlapCells;

  // the extend in one or the other direction
  double halflengthLoc1 = sidetElement->width()*0.5;
  double halflengthLoc2 = sidetElement->length()*0.5;

  // the z-range is eta dependent
  double eta    = tp.position().eta();
  bool isBarrel = sidetElement->isBarrel();

  // the surface center & parameter position
  const Amg::Vector3D& surfCenter   = parameterSurface.center();
  const Amg::Vector3D& parPosition  = tp.position();
  const Amg::Vector3D  parDirection = tp.momentum().normalized();

  // get next in phi and make 2-pi-safe
  double phiSurface  = surfCenter.phi();
  double phiPosition = parPosition.phi();
  // phi jumps over the +/- border either at 0 or +/- pi
  bool phiJump = phiSurface * phiPosition < 0;

  // check whether phi Increases or steps over the 2-pi border
  bool phiInc = (phiPosition > phiSurface) || (phiJump && phiSurface > 0.5 * M_PI);


  // robust overlap search also deals with rings in the case of Pixel upgrade design
  if (m_robustOverlapMode == 2){

    Trk::PropDirection searchDirection = Trk::anyDirection;

    const InDetDD::SiDetectorElement* nextPhi =  sidetElement->nextInPhi();

    const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(nextPhi->identify())) : 0;


    const InDetDD::SiDetectorElement* prevPhi =  sidetElement->prevInPhi();
    const Trk::Surface* prevInPhiSurface = (prevPhi) ? &(prevPhi->surface(prevPhi->identify())) : 0;


    const InDetDD::SiDetectorElement* nextEta = sidetElement->nextInEta();

    // get the next eta surface
    if (!nextEta && !isBarrel && m_binnedSurfaceArray && m_pixIdHelper && m_pixMgr) {
    
      unsigned int currentRing = m_pixIdHelper->eta_module(sidetElement->identify());
      // two cases:
      //   (a) eta increases and we are not yet at the lowest disk
      //   (b) eta descreases and we are not yet at the highest disk
      int signR = parPosition.perp() > surfCenter.perp() ? 1 : -1 ;
      // take the step out / in of the  current module to the next / previous ring
      double momentumPhi = parDirection.phi();
      Amg::Vector3D radialStep(cos(momentumPhi),sin(momentumPhi),0.);
      Amg::Vector3D probePosition(parPosition+signR*halflengthLoc2*radialStep);
      // get the new surface from the array
      const Trk::Surface* npRingSurface = m_binnedSurfaceArray->object(probePosition);
      if (npRingSurface && (unsigned int)m_pixIdHelper->eta_module(npRingSurface->associatedDetectorElementIdentifier()) != currentRing){
        const InDetDD::SiDetectorElement* testEta = dynamic_cast<const InDetDD::SiDetectorElement*>(npRingSurface->associatedDetectorElement());
        if (testEta)
            if(testEta->isPixel()) nextEta = testEta;
      }
    }

    const Trk::Surface* nextInEtaSurface = (nextEta) ? &(nextEta->surface(nextEta->identify())) : 0;
   
    const InDetDD::SiDetectorElement* prevEta = sidetElement->prevInEta();
   
    const Trk::Surface* prevInEtaSurface = (prevEta) ? &(prevEta->surface(prevEta->identify())) : 0;
   
   
    if (nextInPhiSurface){  m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInPhiSurface,searchDirection),2));}
   
    if (prevInPhiSurface){  m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(prevInPhiSurface,searchDirection),2));}

    if (nextInEtaSurface && startSurface != nextInEtaSurface && endSurface != nextInEtaSurface )
        m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInEtaSurface,searchDirection),2));
       
    if (prevInEtaSurface && startSurface != prevInEtaSurface && endSurface != prevInEtaSurface )
        m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(prevInEtaSurface,searchDirection),2));

    // the next in eta section : check for phi overlaps
    if (nextEta){
      
      const InDetDD::SiDetectorElement* nextPhiEta =  nextEta->nextInPhi();
      const Trk::Surface* nextInPhiEtaSurface = (nextPhiEta) ? &(nextPhiEta->surface(nextPhiEta->identify())) : 0;
      const InDetDD::SiDetectorElement* nextPhiprevEta = nextEta->prevInPhi();
      const Trk::Surface* nextInPhiprevEtaSurface = (nextPhiprevEta) ? &(nextPhiprevEta->surface(nextPhiprevEta->identify())) : 0;
      
      if (nextInPhiEtaSurface && startSurface != nextInPhiEtaSurface && endSurface != nextInPhiEtaSurface)
         m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInPhiEtaSurface,searchDirection),2));
      
      if (nextInPhiprevEtaSurface && startSurface != nextInPhiprevEtaSurface && endSurface != nextInPhiprevEtaSurface)
        m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInPhiprevEtaSurface,searchDirection),2));
    }

    // the previous in eta section : check for phi overlaps
    if (prevEta){
    
      const InDetDD::SiDetectorElement* prevPhinextEta =  prevEta->nextInPhi();
      const Trk::Surface* prevInPhinextEtaSurface = (prevPhinextEta) ? &(prevPhinextEta->surface(prevPhinextEta->identify())) : 0;
      const InDetDD::SiDetectorElement* prevPhiEta =  prevEta->prevInPhi();
      const Trk::Surface* prevInPhiEtaSurface = (prevPhiEta) ? &(prevPhiEta->surface(prevPhiEta->identify())) : 0;
      
      if (prevInPhinextEtaSurface && startSurface != prevInPhinextEtaSurface && endSurface != prevInPhinextEtaSurface)
          m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(prevInPhinextEtaSurface,searchDirection),2));
      
      if (prevInPhiEtaSurface && startSurface != prevInPhiEtaSurface && endSurface != prevInPhiEtaSurface)
          m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(prevInPhiEtaSurface,searchDirection),2));
    }
  //standard (non-robust) overlaps
  } else {
    // get the next in phi surface
    const InDetDD::SiDetectorElement* nextPhi =  phiInc ?
             sidetElement->nextInPhi() : sidetElement->prevInPhi();
    // next surface in phi
    const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(nextPhi->identify())) : 0;
    // the alternative search (for achieving full 100 % efficiency)
    const InDetDD::SiDetectorElement* prevPhi =  phiInc ?
             sidetElement->prevInPhi() : sidetElement->nextInPhi();
    const Trk::Surface* prevInPhiSurface = (prevPhi) ? &(prevPhi->surface(prevPhi->identify())) : 0;
    
    // for the triple hits
    bool tookPhiOverlap = false;
    bool tookEtaOverlap = false;
    bool directionFlag  = true;
    
    Trk::PropDirection searchDirection = (startSurface || endSurface) ? dir : Trk::anyDirection;
    
    // check in phi for an overlap, never take the startSurface
    if (nextInPhiSurface
        && startSurface != nextInPhiSurface
        && endSurface != nextInPhiSurface ) {
    
          // transform the position and momentum direction into the surface frame
          Amg::Transform3D invTransformSf(nextInPhiSurface->transform().inverse());
          Amg::Vector3D parDirectionSf =  invTransformSf.linear()*parDirection;
          Amg::Vector3D parPositionSf  =  invTransformSf*parPosition;
    
          // solve in Surface frame, is solving in local x/z frame
          // eq of line : z = k .x + d
          // -> with px,pz  : pz = k . px + d -> d = pz - k . px
          // -> find pz = 0 : 0  = k . xi + d -> xi = -d/k = px - pz/k    
          double px = parPositionSf.x();
          double pz = parPositionSf.z();
          double dx = parDirectionSf.x();
          double dz = parDirectionSf.z();
    
          double k  = dz/dx;
          double xi = px - pz/k;
    
          // project the direction onto the diff vector
          directionFlag =  (dir == Trk::anyDirection) ? true : (dir*dz*pz*skipDirection < 0.);
          // check the opposite direction in case that a start surface is given
          if (directionFlag && (!m_checkLocalCoords || xi*xi < (1.+m_loc1fraction)*(1.+m_loc1fraction)*halflengthLoc1*halflengthLoc1)){
            // took the phi overlap
            tookPhiOverlap = true;
            // registering overlap condition 2 : try in any case, since decision has been taken by fraction search
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInPhiSurface,searchDirection),2));
          }
     } // end of nextInPhiSurface
    // the restrictive, alternative search : only perform it for anyDirection
    if (dir == Trk::anyDirection
        && prevInPhiSurface
        && startSurface != prevInPhiSurface
        && endSurface != prevInPhiSurface){
    
          // transform the position and momentum direction into the surface frame
            Amg::Transform3D invTransformSf(prevInPhiSurface->transform().inverse());
            Amg::Vector3D parDirectionSf =  invTransformSf.linear()*parDirection;
            Amg::Vector3D parPositionSf  =  invTransformSf*parPosition;
    
          // solve in Surface frame, is solving in local x/z frame
          // eq of line : z = k .x + d
          // -> with px,pz  : pz = k . px + d -> d = pz - k . px
          // -> find pz = 0 : 0  = k . xi + d -> xi = -d/k = px - pz/k
    
          double px = parPositionSf.x();
          double pz = parPositionSf.z();
          double dx = parDirectionSf.x();
          double dz = parDirectionSf.z();
    
          double k  = dz/dx;
          double xi = px - pz/k;
    
          // check the opposite direction in case that a start surface is given
          if (!m_checkLocalCoords || xi*xi < (1.+m_loc1fraction)*(1.+m_loc1fraction)*halflengthLoc1*halflengthLoc1){
            // registering overlap condition 2 : try in any case, since decision has been taken by fraction search
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(prevInPhiSurface,dir),2));
          }
    } // end of privInPhiSurface


  // next in eta does only make sense for pixel modules in the barrel
  if (isBarrel){

    // eta from surface
    double etaSurface = surfCenter.eta();

    // eta is safe, it is constantly increasing/decreasing
    bool etaInc  =  (eta > etaSurface);

    // get the nextEta detector element
    const InDetDD::SiDetectorElement* nextEta =  etaInc  ?
        sidetElement->nextInEta() : sidetElement->prevInEta();

    // get the next eta surface
    const Trk::Surface* nextInEtaSurface = (nextEta) ? &(nextEta->surface(nextEta->identify())) : 0;

    // check for the eta-overlap just like the phi overlap, never take the start surface
    if (nextInEtaSurface &&
        startSurface != nextInEtaSurface &&
        endSurface   != nextInEtaSurface ) {

        // transform the position and momentum direction into the surface frame
        Amg::Transform3D invTransformSf(nextInEtaSurface->transform().inverse());
        Amg::Vector3D parDirectionSf =  invTransformSf.linear()*parDirection;
        Amg::Vector3D parPositionSf  =  invTransformSf*parPosition;

        // solve in Surface frame, is solving in local y/z frame
        // eq of line : z = k .y + d
        // -> with py,pz  : pz = k . py + d -> d = pz - k . py
        // -> find pz = 0 : 0  = k . yi + d -> yi = -d/k = py - pz/k

        double py = parPositionSf.y();
        double pz = parPositionSf.z();
        double dy = parDirectionSf.y();
        double dz = parDirectionSf.z();

        double k  = dz/dy;
        double yi = py - pz/k;

        // project the direction onto the diff vector
        directionFlag = (dir==Trk::anyDirection) ? true : (dir*dz*pz*skipDirection < 0.);

        if (directionFlag && (!m_checkLocalCoords || yi*yi < (1.+m_loc2fraction)*(1.+m_loc2fraction)*halflengthLoc2*halflengthLoc2)){
          // took the phi overlap
          tookEtaOverlap = true;
          // registering overlap condition 2 : try in any case, since decision has been taken by fraction search
          m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInEtaSurface,searchDirection),2));
       }
     } // end of nextInEtaSurface 
     // triple hits
     if (tookPhiOverlap && tookEtaOverlap){
       // next / prev phi
       const InDetDD::SiDetectorElement* nextPhiEta =  phiInc ? nextEta->nextInPhi() : nextEta->prevInPhi();
       const Trk::Surface* nextInPhiEtaSurface = (nextPhiEta) ? &(nextPhiEta->surface(nextPhiEta->identify())) : 0;
       // registering overlap condition 2 : decision taken by the fraction search
       if (nextInPhiEtaSurface)
           m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(nextInPhiEtaSurface,searchDirection),2));
     } // end of triple hits
   } // end of isBarrel
  } //end of standard search
  
  return m_overlapCells;
}



