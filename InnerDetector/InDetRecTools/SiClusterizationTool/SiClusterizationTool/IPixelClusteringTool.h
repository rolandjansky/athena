/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// I_PixelClusteringTool.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Interface for pixel clustering algorithms, taking input from RDOs
///////////////////////////////////////////////////////////////////

#ifndef SICLUSTERIZATIONTOOL_IPIXELCLUSTERINGTOOL_H
#define SICLUSTERIZATIONTOOL_IPIXELCLUSTERINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDORawData.h"
// forward declare not possible (typedef)
#include "InDetPrepRawData/PixelClusterCollection.h"

class PixelID;

namespace InDetDD
{
  class SiDetectorManager;
}

namespace InDet
{

  static const InterfaceID IID_IPixelClusteringTool("InDet::IPixelClusteringTool", 1, 0);

  class IPixelClusteringTool : virtual public IAlgTool
  {

  public:

    virtual ~IPixelClusteringTool() {};
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;

    static const InterfaceID& interfaceID() { return IID_IPixelClusteringTool; };

    // Clusterize a collection of pixel raw data objects
    virtual PixelClusterCollection* clusterize
    (const InDetRawDataCollection<PixelRDORawData> &RDOs,
     const InDetDD::SiDetectorManager& manager,
     const PixelID& idHelper) const = 0;

  };

}

#endif // SICLUSTERIZATIONTOOL_I_PIXELCLUSTERINGALG_H
