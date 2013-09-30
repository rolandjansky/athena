/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JIVEXML_IINDETGEOMODELTOOL_H
#define JIVEXML_IINDETGEOMODELTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace InDetDD{
  class PixelDetectorManager;
  class SCT_DetectorManager;
  class TRT_DetectorManager;
}
class PixelID;
class SCT_ID;
class TRT_ID;

namespace JiveXML{

  static const InterfaceID IID_IInDetGeoModelTool("JiveXML::IInDetGeoModelTool", 1, 0);

  /**
   * @class IInDetGeoModelTool
   * @brief Interface for the InDetGeoModelTool 
   */
  class IInDetGeoModelTool : virtual public IAlgTool{

  public:

    /// Return the interface identifier
    static const InterfaceID& interfaceID();

    /// Provide the Pixel geometry manager
    virtual const InDetDD::PixelDetectorManager* PixelGeoManager() const = 0 ;
    /// Provide the SCT geometry manager
    virtual const InDetDD::SCT_DetectorManager* SCTGeoManager() const = 0 ;
    /// Provide the TRT geometry manager
    virtual const InDetDD::TRT_DetectorManager* TRTGeoManager() const = 0 ;

    /// Provide the Pixel identifier helper
    virtual const PixelID* PixelIDHelper() const = 0 ;
    /// Provide the SCT identifier helper
    virtual const SCT_ID* SCTIDHelper() const = 0 ;
    /// Provide the TRT identifier helper
    virtual const TRT_ID* TRTIDHelper() const = 0 ;

  };
  
  inline const InterfaceID& IInDetGeoModelTool::interfaceID() { return IID_IInDetGeoModelTool; }

} //namespace

#endif
