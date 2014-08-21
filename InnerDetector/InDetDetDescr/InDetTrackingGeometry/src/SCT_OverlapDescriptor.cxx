/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCT_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


// InDet
#include "InDetTrackingGeometry/SCT_OverlapDescriptor.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetIdentifier/SCT_ID.h" 
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkParameters/TrackParameters.h"
// Gaudi
#include "GaudiKernel/PropertyMgr.h"

InDet::SCT_OverlapDescriptor::SCT_OverlapDescriptor(double l1frac,
                                                    double l2frac,
                                                    bool checkLC,
                                                    const Trk::BinnedArray<Trk::Surface>* ba,
                                                    const SCT_ID* sctIdHelper,
                                                    const InDetDD::SiDetectorManager* sctMgr,
                                                    int robustOverlapMode) :
  m_checkLocalCoords(checkLC),
  m_loc1fraction(l1frac),
  m_loc2fraction(l2frac),
  m_binnedSurfaceArray(ba),
  m_sctMgr(sctMgr),
  m_sctIdHelper(sctIdHelper),
  m_robustOverlapMode(robustOverlapMode)
{
  // reserve the maximum number
  m_overlapCells.reserve(7);
}

const Trk::OverlapCell InDet::SCT_OverlapDescriptor::overlapSurface(const Trk::TrackParameters& tp,
                                                                    const Trk::Surface* sfreq,
                                                                    Trk::PropDirection dir) const
{
    
  // the unique break condition
  if (sfreq == 0) return Trk::OverlapCell(0,dir);  
  
  // get the associated Surface to the start parameters - return 0 if not valid
  const Trk::Surface& parameterSurface = tp.associatedSurface();

  // get the detector element 
  const InDetDD::SiDetectorElement* sidetElement 
    = dynamic_cast<const InDetDD::SiDetectorElement*>(parameterSurface.associatedDetectorElement());
          
   // ============== SCT Endcap case ======================================  
  if (sidetElement && sidetElement->isEndcap()) {
    
     if (!(sfreq->bounds().inside(tp.localPosition()))){      
     // --------- debug ---------------
     if ((&parameterSurface) != sfreq){

     // retrieve the backside side Detector Element
     const InDetDD::SiDetectorElement* backSide = sidetElement->otherSide();
     // get the backside detector element Surface
     const Trk::Surface* backSideSurface = (backSide) ? &(backSide->surface(backSide->identify())) : 0;
     // it provides the backside for the case 
     // (1) sfreq == parameterSurface (initial backside search)
        return Trk::OverlapCell(backSideSurface, Trk::anyDirection);         
     }
       // the tp is outside - start the next in eta search
       if (!(sfreq->bounds().insideLoc2(tp.localPosition()))){
       
        double etaSurface  = parameterSurface.center().eta();
        double etaPosition = tp.position().eta();

        // return the surface according to the phi direction
        const InDetDD::SiDetectorElement* nextInEta = etaSurface < etaPosition ? sidetElement->nextInEta() : sidetElement->prevInEta(); 
         
        // get actual surface
        const Trk::Surface* nextInEtaSurface = nextInEta ? &(nextInEta->surface(nextInEta->identify())) : 0;
        return Trk::OverlapCell(nextInEtaSurface, Trk::anyDirection); 
             
        } 

       // outside trapezoid, but inside eta - search for next in phi
        // get next in phi
       double phiSurface  = parameterSurface.center().phi();
       double phiPosition = tp.position().phi();
       // make phi safe
       phiSurface   += M_PI;
       phiPosition  += M_PI;
       // return the surface according to the phi direction
       const InDetDD::SiDetectorElement* nextPhi = (phiPosition > phiSurface) ? sidetElement->nextInPhi() : sidetElement->prevInPhi(); 
       const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(sidetElement->identify())) : 0;
       nextInPhiSurface = (nextInPhiSurface != sfreq) ? nextInPhiSurface : 0;
       return Trk::OverlapCell(nextInPhiSurface, Trk::anyDirection);    
      } 
   }

  // the dynamic cast was successful
  if (sidetElement){
    // retrieve the backside side Detector Element
    const InDetDD::SiDetectorElement* backSide = sidetElement->otherSide();
    // get the backside detector element Surface
    const Trk::Surface* backSideSurface = (backSide) ? &(backSide->surface(backSide->identify())) : 0;
    // this is the tricky part
    // it provides the backside for the case 
    // (1) sfreq == parameterSurface (initial backside search)

    if (sfreq == (&parameterSurface) )
        return Trk::OverlapCell(backSideSurface, Trk::alongMomentum);      

    // if the reference surface is the backside -> do the phi swap
    if (sfreq == backSideSurface && dir == Trk::alongMomentum){
       // get next in phi
       double phiSurface  = parameterSurface.center().phi();
       double phiPosition = tp.position().phi();
       // make phi safe
       phiSurface   += M_PI;
       phiPosition  += M_PI;
       // return the surface according to the phi direction
       const InDetDD::SiDetectorElement* nextPhi = (phiPosition > phiSurface) ? backSide->nextInPhi() : backSide->prevInPhi(); 
       const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(sidetElement->identify())) : 0;
       if (nextInPhiSurface != sfreq)
         return Trk::OverlapCell(nextInPhiSurface, Trk::alongMomentum);
    }
    // (2) sfreq != backside (second backside search)
    if (sfreq != backSideSurface)
        return Trk::OverlapCell(backSideSurface, Trk::anyDirection);

    // break condition for inifinite loop 
    if (backSideSurface == (&parameterSurface)) return Trk::OverlapCell(0, dir);

  } 

  return Trk::OverlapCell(0, dir);
}


const std::vector<Trk::OverlapCellCondition>& InDet::SCT_OverlapDescriptor::overlapCells(const Trk::TrackParameters& tp, 
                                                                                         Trk::PropDirection dir,
                                                                                         const Trk::Surface* startSurface,
                                                                                         const Trk::Surface* endSurface) const
{
 
 // clear the vector from last call
  m_overlapCells.clear();  

  // parameters position / direction
  const Amg::Vector3D& parPosition  = tp.position();  
  const Amg::Vector3D& parDirection = tp.momentum().normalized();

  // this is from where the search starts
  const InDetDD::SiDetectorElement* referenceElement = 0;
  const Trk::Surface* referenceSurface               = 0;
  // this are the check surfaces for fast exit
  const Trk::Surface* startSurfaceOtherSide = 0;
  const Trk::Surface* endSurfaceOtherSide   = 0;

  //robust overlaps
  if(m_robustOverlapMode == 2){

   // (a) a start surface is given ---------------------------------------------------------------------------------
    if (startSurface){
      // the search starts from the startSurface
      referenceSurface = startSurface;
      // get the detector element that comes with the start surface
      const InDetDD::SiDetectorElement* sidetElementStart 
           = dynamic_cast<const InDetDD::SiDetectorElement*>(startSurface->associatedDetectorElement());
      referenceElement = sidetElementStart;
      // go on with the backside
      const InDetDD::SiDetectorElement* backSideStart = sidetElementStart ? sidetElementStart->otherSide() : 0;      
      startSurfaceOtherSide = (backSideStart) ? &(backSideStart->surface(backSideStart->identify())) : 0;
      // fast exit endSurface is backside of start surface
      if (startSurfaceOtherSide == endSurface && endSurface)
          return m_overlapCells;
      // check with momentum directive
      if (startSurfaceOtherSide){
         Amg::Vector3D diffSurfaceCenterStart(startSurfaceOtherSide->center()-startSurface->center());
          // the start backside surface is ahead of the surface w.r.t the momentum,
         // nevertheless: search direction put to anyDirection, because we don'know where the extrapolation would start from
         if (diffSurfaceCenterStart.dot(dir*parDirection) > 0.)
             m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(startSurfaceOtherSide,Trk::anyDirection),2)); //1
      }
    }
    // (b) an end surface is given ---------------------------------------------------------------------------------
    if (endSurface){
      // set the referenceSurface to the endSurface if it is not occupied by the start surface already
      referenceSurface = referenceSurface ? referenceSurface : endSurface;
      // get also the end surface 
      const InDetDD::SiDetectorElement* sidetElementEnd
           = dynamic_cast<const InDetDD::SiDetectorElement*>(endSurface->associatedDetectorElement());
      referenceElement = sidetElementEnd;
      // go on with the backside
      const InDetDD::SiDetectorElement* backSideEnd = sidetElementEnd ? sidetElementEnd->otherSide() : 0;      
      endSurfaceOtherSide = (backSideEnd) ? &(backSideEnd->surface(backSideEnd->identify())) : 0;
      // fast exit startSurface is backside of endSurface (should have been caught by case (a) already)
      if (endSurfaceOtherSide == startSurface && startSurface) 
          return m_overlapCells;
      // check with momentum directive
      if (endSurfaceOtherSide){
          Amg::Vector3D diffSurfaceCenterEnd(endSurface->center()-endSurfaceOtherSide->center());
          // the start backside surface is backwards of the surface w.r.t the momentum
         // nevertheless: search direction put to anyDirection, because we don'know where the extrapolation would start from
          if (diffSurfaceCenterEnd.dot(dir*parDirection) > 0.)
              m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(endSurfaceOtherSide,Trk::anyDirection),2));//1 
         }
      }
    
    
    // (c) neither start nor end surface is given: layer pass-through -------------------------------------------------
    if (!referenceSurface){ 
      // neither start nor endsurface have been given: reference surface comes from track parameters
      referenceSurface = &(tp.associatedSurface());
      // get the detector element that comes with the start surface
      referenceElement = referenceSurface ?
         dynamic_cast<const InDetDD::SiDetectorElement*>(referenceSurface->associatedDetectorElement()) : 0;
      
      // associated Surface not found 
      if (!referenceElement) return m_overlapCells;
      // register the backside if it does not conflict with the direction directive
      const InDetDD::SiDetectorElement* backSide = referenceElement->otherSide();      
      const Trk::Surface* backSideSurface = (backSide) ? &(backSide->surface(backSide->identify())) : 0;
      // take everything / check the backside :
      //  --- register the backside with option 1 : only take when surface per se is hit
      if (backSideSurface) m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(backSideSurface,dir),2));//1         
    }
    
    // return if you have reference surface
    if (!referenceSurface || !referenceElement) return m_overlapCells;
  
    bool isBarrel          = referenceElement->isBarrel();  
    // the extend in one or the other direction
    double halflengthLoc2 = referenceElement->length() * 0.5;
    
    // the surface center & parameter position  
    const Amg::Vector3D& surfCenter = referenceSurface->center();
    
    const InDetDD::SiDetectorElement* nextPhi = referenceElement->nextInPhi();
    const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(nextPhi->identify())) : 0;
    
    if(nextInPhiSurface) registerWithBackside(*nextPhi,dir,startSurface,endSurface);
    
    const InDetDD::SiDetectorElement* prevPhi = referenceElement->prevInPhi();
    const Trk::Surface* prevInPhiSurface = (prevPhi) ? &(prevPhi->surface(prevPhi->identify())) : 0;
    
    if(prevInPhiSurface) registerWithBackside(*prevPhi,dir,startSurface,endSurface);
    
    
    const InDetDD::SiDetectorElement* nextEta = referenceElement->nextInEta();
    const Trk::Surface* nextInEtaSurface = (nextEta) ? &(nextEta->surface(nextEta->identify())) : 0;
    
    if (!nextEta && !isBarrel && m_binnedSurfaceArray && m_sctIdHelper && m_sctMgr) {
      // get the ring number
      unsigned int currentRing = m_sctIdHelper->eta_module(referenceElement->identify());
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
      if (npRingSurface && (unsigned int)m_sctIdHelper->eta_module(npRingSurface->associatedDetectorElementIdentifier()) != currentRing ){
        // get the Detector element and check if the ring changed
        nextEta = dynamic_cast<const InDetDD::SiDetectorElement*>(npRingSurface->associatedDetectorElement());
      }
    }
    
    if(nextInEtaSurface) registerWithBackside(*nextEta,dir,startSurface,endSurface);
    
    
    const InDetDD::SiDetectorElement* prevEta = referenceElement->prevInEta();
    const Trk::Surface* prevInEtaSurface = (prevEta) ? &(prevEta->surface(prevEta->identify())) : 0;
    
    if(prevInEtaSurface) registerWithBackside(*prevEta,dir,startSurface,endSurface);
    
    
     if(nextEta){
    
    const InDetDD::SiDetectorElement* nextPhiEta =  nextEta->nextInPhi();
    
    
    const Trk::Surface* nextInPhiEtaSurface = (nextPhiEta) ? &(nextPhiEta->surface(nextPhiEta->identify())) : 0;
    
    const InDetDD::SiDetectorElement* nextPhiprevEta = nextEta->prevInPhi();
    const Trk::Surface* nextInPhiprevEtaSurface = (nextPhiprevEta) ? &(nextPhiprevEta->surface(nextPhiprevEta->identify())) : 0;
    
    if (nextInPhiEtaSurface) registerWithBackside(*nextPhiEta,dir,startSurface,endSurface);
      
    
     if (nextInPhiprevEtaSurface) registerWithBackside(*nextPhiprevEta,dir,startSurface,endSurface);
       
     }
    
     if(prevEta){
       
       const InDetDD::SiDetectorElement* prevPhinextEta =  prevEta->nextInPhi();
       const Trk::Surface* prevInPhinextEtaSurface = (prevPhinextEta) ? &(prevPhinextEta->surface(prevPhinextEta->identify())) : 0;
       
       const InDetDD::SiDetectorElement* prevPhiEta =  prevEta->prevInPhi();
       const Trk::Surface* prevInPhiEtaSurface = (prevPhiEta) ? &(prevPhiEta->surface(prevPhiEta->identify())) : 0;
       
       if (prevInPhinextEtaSurface) registerWithBackside(*prevPhinextEta,dir,startSurface,endSurface);
      
       
         if (prevInPhiEtaSurface) registerWithBackside(*prevPhiEta,dir,startSurface,endSurface);
      
    
     }  
    
 }

 //standard (non-robust) overlaps
 else{
  // (a) a start surface is given ---------------------------------------------------------------------------------
  if (startSurface){
    // the search starts from the startSurface
    referenceSurface = startSurface;
    // get the detector element that comes with the start surface
    const InDetDD::SiDetectorElement* sidetElementStart 
         = dynamic_cast<const InDetDD::SiDetectorElement*>(startSurface->associatedDetectorElement());
    referenceElement = sidetElementStart;
    // go on with the backside
    const InDetDD::SiDetectorElement* backSideStart = sidetElementStart ? sidetElementStart->otherSide() : 0;      
    startSurfaceOtherSide = (backSideStart) ? &(backSideStart->surface(backSideStart->identify())) : 0;
    // fast exit endSurface is backside of start surface
    if (startSurfaceOtherSide == endSurface && endSurface)
        return m_overlapCells;
    // check with momentum directive
    if (startSurfaceOtherSide){
        Amg::Vector3D diffSurfaceCenterStart(startSurfaceOtherSide->center()-startSurface->center());
        // the start backside surface is ahead of the surface w.r.t the momentum,
       // nevertheless: search direction put to anyDirection, because we don'know where the extrapolation would start from
        if (diffSurfaceCenterStart.dot(dir*parDirection) > 0.)
           m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(startSurfaceOtherSide,Trk::anyDirection),1)); 
    }
  }
  // (b) an end surface is given ---------------------------------------------------------------------------------
  if (endSurface){
    // set the referenceSurface to the endSurface if it is not occupied by the start surface already
    referenceSurface = referenceSurface ? referenceSurface : endSurface;
    // get also the end surface 
    const InDetDD::SiDetectorElement* sidetElementEnd
         = dynamic_cast<const InDetDD::SiDetectorElement*>(endSurface->associatedDetectorElement());
    referenceElement = sidetElementEnd;
    // go on with the backside
    const InDetDD::SiDetectorElement* backSideEnd = sidetElementEnd ? sidetElementEnd->otherSide() : 0;      
    endSurfaceOtherSide = (backSideEnd) ? &(backSideEnd->surface(backSideEnd->identify())) : 0;
    // fast exit startSurface is backside of endSurface (should have been caught by case (a) already)
    if (endSurfaceOtherSide == startSurface && startSurface) 
        return m_overlapCells;
    // check with momentum directive
    if (endSurfaceOtherSide){
        Amg::Vector3D diffSurfaceCenterEnd(endSurface->center()-endSurfaceOtherSide->center());
        // the start backside surface is backwards of the surface w.r.t the momentum
       // nevertheless: search direction put to anyDirection, because we don'know where the extrapolation would start from
        if (diffSurfaceCenterEnd.dot(dir*parDirection) > 0.)
           m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(endSurfaceOtherSide,Trk::anyDirection),1)); 
    }
  }
  
  // (c) neither start nor end surface is given: layer pass-through -------------------------------------------------
  if (!referenceSurface){ 
    // neither start nor endsurface have been given: reference surface comes from track parameters
    referenceSurface = &(tp.associatedSurface());
    // get the detector element that comes with the start surface
    referenceElement = referenceSurface ?
       dynamic_cast<const InDetDD::SiDetectorElement*>(referenceSurface->associatedDetectorElement()) : 0;
    
    // associated Surface not found 
    if (!referenceElement) return m_overlapCells;
    // register the backside if it does not conflict with the direction directive
    const InDetDD::SiDetectorElement* backSide = referenceElement->otherSide();      
    const Trk::Surface* backSideSurface = (backSide) ? &(backSide->surface(backSide->identify())) : 0;
    // take everything / check the backside :
    //  --- register the backside with option 1 : only take when surface per se is hit
    if (backSideSurface) m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(backSideSurface,dir),1));         
  }
  // return if you have reference surface
  if (!referenceSurface || !referenceElement) return m_overlapCells;

  // now do the same as the pixel / sct ---> and always check the overlap  
  // the z-range is eta dependent
  double eta             = tp.position().eta();
  
  // if it is an endSurface call, skip the direction
  double skipDirection = endSurface ? -1. : 1.;
  bool isBarrel          = referenceElement->isBarrel();  
  
  // the extend in one or the other direction
  double halflengthLoc1 = isBarrel ? referenceElement->width() * 0.5 : referenceElement->maxWidth() * 0.5;
  double halflengthLoc2 = referenceElement->length() * 0.5;

  // the surface center & parameter position  
  const Amg::Vector3D& surfCenter = referenceSurface->center();

  // get next in phi ------------------------------------
  double phiSurface  = surfCenter.phi();
  double phiPosition = parPosition.phi();

  // phi jumps over the +/- border either at 0 or +/- pi
  bool phiJump = phiSurface * phiPosition < 0;
        
  // check whether phi Increases or steps over the 2-pi border
  bool phiInc = (phiPosition > phiSurface) || (phiJump && phiSurface > 0.5 * M_PI);
 
  // get the next in phi surface       
  const InDetDD::SiDetectorElement* nextPhi =  phiInc ?
           referenceElement->nextInPhi() : referenceElement->prevInPhi();  
  const Trk::Surface* nextInPhiSurface = (nextPhi) ? &(nextPhi->surface(nextPhi->identify())) : 0;    

  // fast exit the surface next in phi is the endSurface in case a startSurface has been given
  if (startSurface && endSurface 
      && ( (nextInPhiSurface == endSurface) || (nextInPhiSurface == endSurfaceOtherSide) ) )
  return m_overlapCells;

  // the alternative search (for achieving full 100 % efficiency) 
  const InDetDD::SiDetectorElement* prevPhi =  phiInc ?
           referenceElement->prevInPhi() : referenceElement->nextInPhi();   
  const Trk::Surface* prevInPhiSurface = (prevPhi) ? &(prevPhi->surface(prevPhi->identify())) : 0;

  // get next in eta ------------------------------------
  double etaSurface = surfCenter.eta();
  
  // it's not as trivial as that ... re-evaluate this ...
  const InDetDD::SiDetectorElement* nextEta =  0;
  if (isBarrel) nextEta = (eta > etaSurface) ? referenceElement->nextInEta() : referenceElement->prevInEta();  
  // for the endcap : nextEta / prevEta is not always implemented
  if (!nextEta && !isBarrel && m_binnedSurfaceArray && m_sctIdHelper && m_sctMgr) {
      // get the ring number
      unsigned int currentRing = m_sctIdHelper->eta_module(referenceElement->identify());
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
      if (npRingSurface && (unsigned int)m_sctIdHelper->eta_module(npRingSurface->associatedDetectorElementIdentifier()) != currentRing ){
          // get the Detector element and check if the ring changed
          nextEta = dynamic_cast<const InDetDD::SiDetectorElement*>(npRingSurface->associatedDetectorElement());
      }
  }


  const Trk::Surface* nextInEtaSurface = (nextEta) ? &(nextEta->surface(nextEta->identify())) : 0;        
       
  // fast exit the surface next in phi is the endSurface in case a startSurface has been given
  if (startSurface && endSurface 
      && ( (nextInEtaSurface == endSurface) || (nextInEtaSurface == endSurfaceOtherSide) ) )
  return m_overlapCells;

  bool tookPhiOverlap = true;
  bool tookEtaOverlap = true;

  // check in phi for an overlap
  if (nextInPhiSurface) {

      // endcap part needs to be revisited, at the moment: take phi overlap and try it out
      if (isBarrel)
        tookPhiOverlap = checkAndRegisterWithBackside(*nextPhi,
                           				             parPosition,
                           				             parDirection,
                                                     *nextInPhiSurface,
                                                     m_loc1fraction,
                                                     halflengthLoc1,
                                                     skipDirection,
                                                     dir,
                                                     startSurface,
                                                     endSurface);
        else // and register it
          registerWithBackside(*nextPhi,dir,startSurface,endSurface);
   }
   // only for layer crossing, i.e. no start / endSurface 
   if (dir==Trk::anyDirection && prevInPhiSurface){

      // endcap part needs to be revisited, at the moment: take phi overlap and try it out
      if (isBarrel)
        checkAndRegisterWithBackside(*prevPhi,
                           		     parPosition,
                           		     parDirection,
                                     *prevInPhiSurface,
                                     m_loc1fraction,
                                     halflengthLoc1,
                                     skipDirection,
                                     dir,
                                     startSurface,
                                     endSurface);
        else // and register it
          registerWithBackside(*prevPhi,dir,startSurface,endSurface);
   }
   // check for the eta-overlap just like the phi overlap
   if (nextInEtaSurface) {
     // endcap part needs to be revisited
     if (isBarrel)
        tookEtaOverlap = checkAndRegisterWithBackside(*nextEta,
                           				          	  parPosition,
                           				          	  parDirection,
                                                  	  *nextInEtaSurface,
                                                  	  m_loc2fraction,
                                                  	  halflengthLoc2,
                                                  	  skipDirection,
                                                  	  dir,
                                                  	  startSurface,
                                                  	  endSurface);
        else // and register it
          registerWithBackside(*nextEta,dir,startSurface,endSurface);
   }
   // triple hits
   if (tookPhiOverlap && tookEtaOverlap && nextEta){
     
       const InDetDD::SiDetectorElement* nextPhiEta =  phiInc ? nextEta->nextInPhi() : nextEta->prevInPhi();
       const Trk::Surface* nextInPhiEtaSurface = (nextPhiEta) ? &(nextPhiEta->surface(nextPhiEta->identify())) : 0;           
         
       if (nextInPhiEtaSurface){
          // and register it
          registerWithBackside(*nextPhiEta,dir,startSurface,endSurface);
        }  
    } 
 }
    return m_overlapCells;
}


bool InDet::SCT_OverlapDescriptor::checkBackside(
                          const Trk::Surface& sf,
                          const Amg::Vector3D& direction,
                          Trk::PropDirection dir,
                          const Trk::Surface* startSurface,
                          const Trk::Surface* endSurface) const
{
    // for pointer comparison
    const Trk::Surface* checkSurface = &sf;
    // the return method here
    bool checkPassed = false;
    // exclude because checkSurface is either start or endSurface 
    if (checkSurface == startSurface || checkSurface == endSurface) return false;
    // do the check on the startSurface
    if (startSurface){
           Amg::Vector3D diffSurfaceCenterStart(checkSurface->center()-startSurface->center());
           checkPassed = diffSurfaceCenterStart.dot(dir*direction) > 0.;
           if (!checkPassed) return false;
    } 
    // do the check on the endSurface 
    if (endSurface){
           Amg::Vector3D diffSurfaceCenterEnd(checkSurface->center()-endSurface->center());
           checkPassed = diffSurfaceCenterEnd.dot(dir*direction) < 0.;
    }          
    return checkPassed;
} 
 
 
bool InDet::SCT_OverlapDescriptor::checkAndRegisterWithBackside(const InDetDD::SiDetectorElement& siDetElement,
                           				  const Amg::Vector3D& position,
                           				  const Amg::Vector3D& direction,
                                          const Trk::Surface& checkSurface,
                                          double fraction,
                                          double halflength,
                                          double flipDirection,
                                          Trk::PropDirection dir,
                                  		  const Trk::Surface* startSurface,
                                          const Trk::Surface* endSurface) const
{
     bool tookPair = false;

     // transform the position and momentum direction into the surface frame 
     Amg::Transform3D invTransformSf(checkSurface.transform().inverse());
     Amg::Vector3D    parPositionSf  = invTransformSf * position;
     Amg::Vector3D    parDirectionSf = invTransformSf.linear() * direction;

     // solve in Surface frame, is solving in local y/z frame
     // eq of line : z = k .y + d
     // -> with py,pz  : pz = k . py + d -> d = pz - k . py
     // -> find pz = 0 : 0  = k . yi + d -> yi = -d/k = py - pz/k

     double py = parPositionSf.y();
     double pz = parPositionSf.z();
     double dy = parDirectionSf.y();
     double dz = parDirectionSf.z();

     // project the direction onto the diff vector  
     bool directionFlag = (dir==Trk::anyDirection) ? true : (dir*dz*pz*flipDirection < 0.);
     double k  = dz/dy;
     double yi = py - pz/k;

     if (directionFlag && (!m_checkLocalCoords || yi*yi < (1.+fraction)*(1.+fraction)*halflength*halflength)){                               
          // took the phi overlap
          tookPair = true;
          // and register it
          registerWithBackside(siDetElement,dir,startSurface,endSurface);            
     }
	return tookPair;
}



void InDet::SCT_OverlapDescriptor::registerWithBackside(const InDetDD::SiDetectorElement& siDetElement,
                                          				Trk::PropDirection dir,
                                  		  				const Trk::Surface* startSurface,
                                          				const Trk::Surface* endSurface) const
{
   


    // get the detector element from this surface
    const Trk::Surface* detSurface = &(siDetElement.surface(siDetElement.identify()));
    if (dir == Trk::anyDirection || (detSurface != startSurface && detSurface != endSurface) )
        m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(detSurface,Trk::anyDirection),2));
    // register the backside with condition 1 : try only if the surface is hit 
    const InDetDD::SiDetectorElement* backSide = siDetElement.otherSide();      
    const Trk::Surface* backSideSurface = (backSide) ? &(backSide->surface(backSide->identify())) : 0;
    if (backSideSurface != startSurface && backSideSurface != endSurface)
        m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(backSideSurface,Trk::anyDirection),2));
}

                          
                          
/*
bool InDet::SCT_OverlapDescriptor::checkOverlapAndFraction(
                            const Trk::Surface& sf,
                            const Trk::GlobalPosition& position,
                            const Trk::GlobalDirection& direction,
                            Trk::PropDirection dir,
                            double fraction,
                            double halflenght,
                            bool  skipDirection)  const
{
   // the surface transform to bring the position / direction into the surface frame
   HepGeom::Transform3D invTransformSf(sf.transform().inverse());
   HepGeom::Vector3D<double> parDirectionSf = invTransformSf * direction;
   HepGeom::Point3D<double>  parPositionSf   =  invTransformSf * position;
 
   // solve in Surface frame, is solving in local x/z frame
   // eq of line : z = k .x + d
   // -> with px,pz  : pz = k . px + d -> d = pz - k . px
   // -> find pz = 0 : 0  = k . xi + d -> xi = -d/k = px - pz/k
   
   double px = parPositionSf.x();
   double pz = parPositionSf.z();
   double dx = parDirectionSf.x();
   double dz = parDirectionSf.z();
    
   // project the direction onto the diff vector
   double flipSide = skipDirection ? 1. : -1.; 
   bool directionFlag = (dir == Trk::anyDirection) ? true : (double(dir)*flipSide*dz*pz < 0.);
      
   // solve the line equation      
   double k  = dz/dx;
   double xi = px - pz/k; 
    
   // check for fraction and direction
   return ( directionFlag && xi*xi < (1.+fraction)*(1.+fraction)*halflenght*halflenght );   
    
}
*/

