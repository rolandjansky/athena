/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_OverlapDescriptor.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// InDet
#include "InDetTrackingGeometry/TRT_OverlapDescriptor.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"
// Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkParameters/TrackParameters.h"

#include "GeoPrimitives/GeoPrimitives.h"

int InDet::TRT_OverlapDescriptor::s_overlapCounter = 0;

const Trk::OverlapCell InDet::TRT_OverlapDescriptor::overlapSurface(const Trk::TrackParameters& tp,
                                                                    const Trk::Surface* sfreq,
                                                                    Trk::PropDirection dir) const
{
  // unique break condition
  if (!sfreq) return Trk::OverlapCell(0,dir);
  
  const Trk::Surface& parameterSurface = tp.associatedSurface();
  
  // don't try to find a next straw if the first one is hit ! 
  if (fabs(tp.parameters()[Trk::driftRadius]) < parameterSurface.bounds().r()) return Trk::OverlapCell(0,dir);

  const InDetDD::TRT_BaseElement* trtBaseElement = dynamic_cast<const InDetDD::TRT_BaseElement*>(parameterSurface.associatedDetectorElement());

  // only search for Barrel section   
  if (trtBaseElement){
    
    Identifier strawID = parameterSurface.associatedDetectorElementIdentifier();
    
    int strawMax = trtBaseElement->nStraws()-1;
    int strawInt = m_trtIdHelper->straw(strawID);
    
    if (strawInt != 0 && strawInt < strawMax && strawMax != -999){
       const Trk::Surface* overlapStraw  = 0;
       // get the straw to the left ...
       Identifier          leftID       =   m_trtIdHelper->straw_id(trtBaseElement->identify(), strawInt-1);
       const Trk::Surface* leftStraw    = (strawInt > 1 ) ? (&(trtBaseElement->surface(leftID))) : 0;
       // get the straw to the right ...
       Identifier          rightID      =   m_trtIdHelper->straw_id(trtBaseElement->identify(), strawInt+1);
       const Trk::Surface* rightStraw   = (strawInt < strawMax ) ? (&(trtBaseElement->surface(rightID))) : 0;
   
       if (leftStraw && rightStraw){

         const Amg::Vector3D& hitPosition = tp.position();
         // get the phi values of all players
         double hitPhi                          = hitPosition.phi();
         double parameterSurfacePhi             = parameterSurface.center().phi();
         double leftSurfacePhi                  = leftStraw->center().phi();
         double rightSurfacePhi                 = rightStraw->center().phi();
         // rescale the phi
         hitPhi              += M_PI;
         parameterSurfacePhi += M_PI;
         leftSurfacePhi      += M_PI;
         rightSurfacePhi     += M_PI;
         // decide in the [0,2Pi) range
         int sign = hitPhi > parameterSurfacePhi ? 1 : -1;
         
         overlapStraw = sign*leftSurfacePhi > sign*rightSurfacePhi ? leftStraw : rightStraw;

       return Trk::OverlapCell(overlapStraw,Trk::alongMomentum);          
 
      }
   } 
 }
 return Trk::OverlapCell(0,dir);
}

const std::vector<Trk::OverlapCellCondition>& InDet::TRT_OverlapDescriptor::overlapCells(const Trk::TrackParameters& tp,
                                                                                         Trk::PropDirection,
                                                                                         const Trk::Surface*,
                                                                                         const Trk::Surface*) const
{
  // clear the vector from last call
  m_overlapCells.clear();
  
  const Trk::Surface& parameterSurface = tp.associatedSurface();  
  
  // don't try to find a next straw if the first one is hit ! 
  if (fabs(tp.parameters()[Trk::driftRadius]) < parameterSurface.bounds().r()) return m_overlapCells;

  const InDetDD::TRT_BaseElement* trtBaseElement = dynamic_cast<const InDetDD::TRT_BaseElement*>(parameterSurface.associatedDetectorElement());

  // only search for Barrel section   
  if (trtBaseElement){
    
    Identifier strawID = parameterSurface.associatedDetectorElementIdentifier();
    
    int strawMax = trtBaseElement->nStraws()-1;
    int strawInt = m_trtIdHelper->straw(strawID);
    
    if (strawInt != 0 && strawInt < strawMax && strawMax != -999){
       // get the straw to the left ...
       Identifier          leftID       =   m_trtIdHelper->straw_id(trtBaseElement->identify(), strawInt-1);
       const Trk::Surface* leftStraw    = (strawInt > 1 ) ? (&(trtBaseElement->surface(leftID))) : 0;
       // get the straw to the right ...
       Identifier          rightID      =   m_trtIdHelper->straw_id(trtBaseElement->identify(), strawInt+1);
       const Trk::Surface* rightStraw   = (strawInt < strawMax ) ? (&(trtBaseElement->surface(rightID))) : 0;
   
       if (leftStraw && rightStraw){

         const Amg::Vector3D& hitPosition = tp.position();
         // get the phi values of all players
         double hitPhi                          = hitPosition.phi();
         double parameterSurfacePhi             = parameterSurface.center().phi();
         double leftSurfacePhi                  = leftStraw->center().phi();
         double rightSurfacePhi                 = rightStraw->center().phi();
         // rescale the phi
         hitPhi              += M_PI;
         parameterSurfacePhi += M_PI;
         leftSurfacePhi      += M_PI;
         rightSurfacePhi     += M_PI;
         // decide in the [0,2Pi) range
         int sign = hitPhi > parameterSurfacePhi ? 1 : -1;
         
         // left first
         if (sign*leftSurfacePhi > sign*rightSurfacePhi){
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(leftStraw,Trk::alongMomentum),0));
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(rightStraw,Trk::alongMomentum),0));
         } else {
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(rightStraw,Trk::alongMomentum),0));         
            m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(leftStraw,Trk::alongMomentum),0));
        }             
      } else if (leftStraw)
          m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(leftStraw,Trk::alongMomentum),0));
      else if (rightStraw)
          m_overlapCells.push_back(std::make_pair(Trk::OverlapCell(rightStraw,Trk::alongMomentum),0));
   } 
 }
  
 return m_overlapCells;
}

