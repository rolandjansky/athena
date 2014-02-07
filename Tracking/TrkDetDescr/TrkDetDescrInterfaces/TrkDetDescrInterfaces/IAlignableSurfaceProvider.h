/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// ================================================
// AlignableSurfaceProvider
// ================================================
//
// IAlignableSurfaceProvider.h
// Interface for IAlignableSurfaceProvider
//
//
// AlgTool to exchange Trk::Surfaces with Trk::AlignableSurfaces and update Trk::AlignableSurfaces for Alignment 

#ifndef ITRKALIGNABLESURFACEPROVIDER_H
#define ITRKALIGNABLESURFACEPROVIDER_H

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"


namespace Trk 
{
  //forward Declarations
  class Surface;

  static const InterfaceID IID_IAlignableSurfaceProvider("Trk::IAlignableSurfaceProvider", 1, 0);

  class IAlignableSurfaceProvider : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

  //creating map with alignable plane surfaces
  virtual void addEntryToMap(const Trk::Surface& )=0;

  //updating alignable plane surfaces in map
  virtual void updateAlignableSurface(Amg::Transform3D&, const Trk::Surface& ) const=0;

  //method to exchange surface for track fitting
  virtual const Trk::Surface& retrieveAlignableSurface(const Trk::Surface& )const =0;

  };

  inline const InterfaceID& Trk::IAlignableSurfaceProvider::interfaceID()
    { 
      return IID_IAlignableSurfaceProvider; 
    }

} // end of namespace

#endif 
