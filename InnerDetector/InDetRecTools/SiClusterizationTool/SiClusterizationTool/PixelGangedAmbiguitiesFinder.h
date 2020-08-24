/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//PixelGangedAmbiguitiesFinder.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Finds pixel clusters with a ganged pixel in common
///////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
// First version 28/01/2004 Tommaso Lari
//
///////////////////////////////////////////////////////////////////

#ifndef SICLUSTERIZATIONTOOL_PIXELGANGEDAMBIGUITIESFINDER_H
#define SICLUSTERIZATIONTOOL_PIXELGANGEDAMBIGUITIESFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

namespace InDetDD {
  class SiDetectorElement;
}


namespace InDet{

  static const InterfaceID IID_PixelGangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder", 1, 0);

  class PixelGangedAmbiguitiesFinder : public AthAlgTool {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    PixelGangedAmbiguitiesFinder(const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);
    ~PixelGangedAmbiguitiesFinder() = default;
  
    static const InterfaceID& interfaceID() { return IID_PixelGangedAmbiguitiesFinder; };

    virtual StatusCode initialize();
  
    // Called by the PixelClusterization algorithms of InDetPrepRawDataFormation
    // A map containing the pairs of Pixel Clusters which shares the same 
    // ganged pixel is created.
    // Inputs are the cluster collection of a module, and the silicon 
    // detector manager, and the map to be filled.
    void execute(PixelClusterCollection* collection,
                 PixelGangedClusterAmbiguities& map) const;  
 
  private:

    typedef InDetDD::SiDetectorElement SiDetectorElement;
    typedef InDetDD::SiLocalPosition SiLocalPosition;

    // If possibile, internally solve ganged pixel ambiguities
    BooleanProperty m_internalSolving{this, "InternalSolving", false, "Standalone resolution of ganged pixel ambiguities"};

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    // Determines if a pixel cell (whose identifier is the first argument) is 
    // a ganged pixel. If this is the case, the last argument assumes the 
    // value of the identifier of the cell it is ganged with. 
    // The second argument is the pixel module the hit belongs to.

    bool isGanged(const Identifier& rdoID,
                  const InDetDD::SiDetectorElement* element,
                  Identifier& gangedID) const;

  };

}

#endif // SICLUSTERIZATIONTOOL_PIXELGANGEDAMBIGUITIESFINDER_H
