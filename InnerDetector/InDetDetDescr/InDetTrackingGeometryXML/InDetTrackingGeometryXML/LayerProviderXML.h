///////////////////////////////////////////////////////////////////
// LayerProviderXML.h, (c) ATLAS Detector software
// Replaces SiLayerBuilder for XML geometry
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_LAYERPROVIDERXMLN_H
#define INDETTRACKINGGEOMETRY_LAYERPROVIDERXMLN_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
// Inner detector
#include "TrkDetDescrInterfaces/ILayerProvider.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceCollection.h"
// STD
#include <fstream>
#include <vector>

namespace Trk {
  class Layer;
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class AlpineLayer;
}

namespace InDet {
  class XMLReaderSvc;
  class BarrelBuilderXML;
  class EndcapBuilderXML;
  class IModuleProvider;

  static const InterfaceID IID_LayerProviderXML("LayerProviderXML", 1, 0);

  class LayerProviderXML : virtual public AthAlgTool, virtual public Trk::ILayerProvider {
  public:
    LayerProviderXML(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~LayerProviderXML();

    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_LayerProviderXML; }
   
    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // ILayerBuilder virtual functions to be implemented
    // LayerBuilder interface method - returning the layers at negative side
    const std::vector< const Trk::Layer* > negativeLayers() const; 
    
    // LayerBuilder interface method - returning the central layers
    const std::vector< const Trk::Layer* > centralLayers() const; 
    
    // LayerBuilder interface method - returning the layers at negative side
    const std::vector< const Trk::Layer* > positiveLayers() const; 

    // Name identification
    const std::string& identification() const;

  private:
    void createPixelBarrel(std::vector< const Trk::Layer* >& cylinderLayers) const;
    void createSCTBarrel(std::vector< const Trk::Layer* >& cylinderLayers) const;
    void createPixelEndcap(std::vector< const Trk::Layer* >& discLayers, int side) const;
    void createSCTEndcap(std::vector< const Trk::Layer* >& discLayers, int side) const;
    void alpineToCollections(bool isPixel, Trk::AlpineLayer *layer) const;
    void layerToCollections(bool isPixel, Trk::Layer *layer) const;
    void createMainFileRZ(std::string filename);
    void openDumpFileRZ(std::string filename);
    void closeDumpFileRZ();
    void dumpRZ(std::string title) const;
    void dumpEtaLinesRZ(double zMax, double rMax) const;
    void dumpTubesRZ(double zMax) const; // Set zMax argument to 0 if dumpEtaLinesRZ is called to avoid double definition of zMax

    unsigned int  m_ndumpRZ;
    std::ofstream m_dumpfileRZ;
    std::string m_identification;
    bool m_doPix, m_doSCT;
    int m_startLayer, m_endLayer, m_startEndcap, m_endEndcap;
    ServiceHandle<InDet::XMLReaderSvc>  m_xmlReader;
    ToolHandle<BarrelBuilderXML> m_pixBarrelBuilder;
    ToolHandle<BarrelBuilderXML> m_sctBarrelBuilder;
    ToolHandle<EndcapBuilderXML> m_pixEndcapBuilder;
    ToolHandle<EndcapBuilderXML> m_sctEndcapBuilder;
    ToolHandle<IModuleProvider>  m_moduleProvider;
    mutable SurfaceCollection*   m_surfcoll;
    mutable double               m_SiSurface;
  };
}

#endif // INDETTRACKINGGEOMETRY_LAYERPROVIDERXMLN_H

