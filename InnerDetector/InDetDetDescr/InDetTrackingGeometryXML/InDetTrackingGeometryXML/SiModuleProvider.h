/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiModuleProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SIMODULEPROVIDER_H
#define SIMODULEPROVIDER_H

// Interface
#include "InDetTrackingGeometryXML/IModuleProvider.h"

class SiDetectorElementMap;

namespace InDet {

  class SiModuleProvider : virtual public IModuleProvider, public AthAlgTool {
  public:
    SiModuleProvider(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~SiModuleProvider();

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // Fill identifier and idhash
    void setIdentifier(bool isPix, Identifier& id, IdentifierHash& idhash, 
		       int brl_ec, unsigned int ilayer, int iphi, int ieta, int side);

    // Create a new module of type module_type with chips of type chip_type
    Trk::TrkDetElementBase* getDetElement(const Identifier &id, 
					 const IdentifierHash & idHash, 
					 InDet::ModuleTmp* moduleTmp, 
					 const Amg::Vector3D transl,
					 const Amg::Transform3D & transf,
					 const bool isPixel,
					 const bool isBarrel,
					 const bool isOuterMost,
					 const bool debug,
					 bool   useDiscSurface = false,
					 double rmin = 0.,
					 double rmax = 0.,
					 double stereo = 0.);

    // Get module transformation
    Amg::Transform3D getTransform(double R, double dR, double xshift, double zpos, 
				  double tilt, double stereo, double rot, double phi,
				  bool useDiscSurface = false ) const;

    // Get overlap descriptors
    virtual Trk::OverlapDescriptor* getPlanarOverlapDescriptor() const;
    virtual Trk::OverlapDescriptor* getDiscOverlapDescriptor() const;
    virtual Trk::OverlapDescriptor* getDiscOverlapDescriptor(Trk::BinnedArray<Trk::Surface>* binnedArray, std::vector< Trk::BinUtility*>* subBinUtilities) const;

    // Set neighbours
    virtual void setEtaNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev)     const;
    virtual void setPhiNeighbours(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* prev)     const;
    virtual void setFrontAndBackSides(Trk::TrkDetElementBase *elem, Trk::TrkDetElementBase* back) const;

    // map DetElement from surface
    virtual void mapSurfaceDetElement(bool isPixel, const Trk::Surface* surf) const;

  private:
    const PixelID* m_pixIdHelper; 
    const SCT_ID*  m_sctIdHelper;
    SiDetectorElementMap* detElementMap;
  };
}

#endif //SIMODULEPROVIDER_H

