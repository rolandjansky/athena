/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 
#ifndef InDet_GeoBCMPrimeTool_H
#define InDet_GeoBCMPrimeTool_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include "GeoModelKernel/GeoPhysVol.h"

class PixelGeoBuilderBasics;

static const InterfaceID IID_IGeoBCMPrimeTool( "InDet::IGeoBCMPrimeTool", 1, 0 );

class IGeoBCMPrimeTool : public virtual IAlgTool {
 public:
  
  /// Retrieve Interface ID
   static const InterfaceID& interfaceID( ) { return IID_IGeoBCMPrimeTool; }
   
   virtual GeoPhysVol* buildModule( int , const PixelGeoBuilderBasics* ) = 0;

   virtual double getTransX() = 0;
   virtual double getTransY() = 0;
   virtual double getRotX() = 0;
   virtual double getRotY() = 0;
   virtual double getRotZ() = 0;
   virtual double getTilt() = 0;
   virtual double getRingRot() = 0;
   virtual double getRingOffset() = 0;

};


#endif
