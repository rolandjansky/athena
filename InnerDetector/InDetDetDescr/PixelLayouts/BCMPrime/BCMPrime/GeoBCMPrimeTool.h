/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @class GeoBCMPrimeTool

  Tool for building BCM' GeoModel
      @author  Jakob Novak <jakob.novak@cern.ch>

*/

#ifndef GEOBCMPRIMETOOL_H
#define GEOBCMPRIMETOOL_H

#include "GeoModelKernel/GeoPhysVol.h"

#include "PixelInterfaces/IGeoBCMPrimeTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"

#include "BCMPrime/BCMPrimeModule.h"

namespace InDet {
  class XMLReaderSvc;
}
class PixelGeoBuilderBasics;

class GeoBCMPrimeTool : virtual public IGeoBCMPrimeTool, public AthAlgTool {

 public:
  GeoBCMPrimeTool(const std::string&,const std::string&,const IInterface*);

  virtual StatusCode initialize() override;
  
  // Register callback function on ConDB object
  StatusCode registerCallback( StoreGateSvc* detStore );

  virtual GeoPhysVol *buildModule(int iLayer, const PixelGeoBuilderBasics *basics) override;
  
  virtual double getTransX() override;
  virtual double getTransY() override;
  virtual double getRotX() override;
  virtual double getRotY() override;
  virtual double getRotZ() override;
  virtual double getTilt() override;
  virtual double getRingRot() override;
  virtual double getRingOffset() override;

private:

  BCMPrimeModule m_module;
  unsigned int m_moduleon;
  
};

#endif
