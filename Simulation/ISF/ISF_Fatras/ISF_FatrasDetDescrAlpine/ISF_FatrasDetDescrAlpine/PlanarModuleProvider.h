/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarModuleProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PLANARMODULEPROVIDER_H
#define PLANARMODULEPROVIDER_H

// Interface
#include "InDetTrackingGeometryXML/IModuleProvider.h"
// ISF
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"

namespace InDet {
  class XMLReaderSvc;
  class ChipTmp;
  class ModuleTmp;
}

namespace iFatras {
  class PlanarModuleProvider : virtual public InDet::IModuleProvider, public AthAlgTool {
  public:
    PlanarModuleProvider(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~PlanarModuleProvider();

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

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
					  bool useDiscSurface = false,
					  double rmin = 0.,
					  double rmax = 0.,
					  double stereo = 0.);

    // Get module transformation
    Amg::Transform3D getTransform(double xpos, double ypos, double zpos, double tilt, 
				  double stereo, double rot, double phi, double xshift,
				  bool useDiscSurface = false) const;

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
    const AtlasDetectorID* m_atlasIdHelper;
    const PixelID* m_pixIdHelper; 
    const SCT_ID*  m_sctIdHelper;
    // Properties
    ServiceHandle<InDet::XMLReaderSvc>  m_xmlReader;
    IdHashDetElementCollection     *m_pixElementMap, *m_sctElementMap;
    std::string                    m_pixElementMapName, m_sctElementMapName;
  };
}

#endif //PLANARMODULEPROVIDER_H

