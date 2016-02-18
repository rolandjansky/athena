/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IModuleProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef IMODULEPROVIDER_H
#define IMODULEPROVIDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// ID
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
  class TrkDetElementBase;
  class OverlapDescriptor;
  class BinUtility;
  class Surface;
  template<typename T> class BinnedArray;
}

namespace InDet {
  class ModuleTmp;

  static const InterfaceID IID_IModuleProvider("IModuleProvider", 1, 0);

  class IModuleProvider : virtual public IAlgTool {
  public:

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_IModuleProvider; }

    // Fill identifier and idhash
    virtual void setIdentifier(bool isPix, Identifier& id, IdentifierHash& idhash, 
			       int brl_ec, unsigned int ilayer, int iphi, int ieta, int side) = 0;

    // Create a new module of type module_type with chips of type chip_type
    virtual Trk::TrkDetElementBase* getDetElement(const Identifier &id, 
						  const IdentifierHash & idHash, 
						  InDet::ModuleTmp* moduleTmp, 
 					          const Amg::Vector3D transl,
						  const Amg::Transform3D & transf,
						  const bool isPixel,
						  const bool isBarrel,
						  const bool isOuterMost,
						  const bool debug,
						  bool useDiscSurface = false,
						  double rmin = 0.,
						  double rmax = 0.,
						  double stereo = 0.) = 0;

    // Get module transformation
    virtual Amg::Transform3D getTransform(double R, double dR, double xshift, double zpos, 
					  double tilt, double stereo, double rot, double phi, bool useDiscSurface = false) const = 0;

    
    // Get overlap descriptors
    virtual Trk::OverlapDescriptor* getPlanarOverlapDescriptor(bool isPixel = true) const = 0;
    virtual Trk::OverlapDescriptor* getDiscOverlapDescriptor(bool isPixel = false) const = 0;
    virtual Trk::OverlapDescriptor* getDiscOverlapDescriptor(bool isPixel, Trk::BinnedArray<Trk::Surface>* binnedArray, std::vector< Trk::BinUtility*>* subBinUtilities) const = 0;

    // Set neighbours
    virtual void setEtaNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev)     const = 0;
    virtual void setPhiNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev)     const = 0;
    virtual void setFrontAndBackSides(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* back) const = 0;

    // map DetElement from surface
    virtual void mapSurfaceDetElement(bool isPixel, const Trk::Surface* surf) const = 0;
  protected:
  };
}

#endif //IMODULEPROVIDER_H

