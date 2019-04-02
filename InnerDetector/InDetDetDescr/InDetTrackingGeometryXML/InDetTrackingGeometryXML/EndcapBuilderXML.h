///////////////////////////////////////////////////////////////////
// EndcapBuilderXML.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ENDCAPBUILDERXML_H
#define ENDCAPBUILDERXML_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// STD
#include <vector>
#include <map>
// TRK
#include "TrkDetDescrUtils/BinnedArray.h"

namespace Trk {
  class DiscLayer;
  class Layer;
  class Surface;
  class TrkDetElementBase; 
  class LayerMaterialProperties;
}

namespace InDet {
  class XMLReaderSvc;
  class EndcapLayerTmp;
  class IModuleProvider;

  static const InterfaceID IID_EndcapBuilderXML("EndcapBuilderXML", 1, 0);

  class EndcapBuilderXML : virtual public AthAlgTool {
  public:
    EndcapBuilderXML(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~EndcapBuilderXML();
    
    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_EndcapBuilderXML; }

    // Get number of layers to build
    unsigned int nbOfLayers();

    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // Set Pixel or SCT case
    inline void setPixelCase(bool isPixel) { m_pixelCase = isPixel; }

    // Create disc layers
    void createActiveLayers(unsigned int itmpl, int side, int startLayer, int endLayer, std::vector< Trk::Layer* >& v_layers) const;

  private:
    // create discs/rings layers
    void createActiveRingLayers(unsigned int itmpl, int side, std::vector< Trk::Layer* >& v_layers) const;
    void createActiveDiscLayers(unsigned int itmpl, int side, std::vector< Trk::Layer* >& v_layers) const;

    // create elements
    void createDiscModules(int itmpl, int iring, int side, 
			   std::vector<Trk::TrkDetElementBase*>& cElements, 
			   std::vector<Amg::Vector3D>& centersOnModule) const; 
			   
    Trk::TrkDetElementBase* createDiscDetElement(int itmpl, int iring, int isector, int side, std::vector<Amg::Vector3D>& centersOnModule) const;

    // utilities
    InDet::EndcapLayerTmp *getLayerTmp(unsigned int itmpl) const;
    Trk::BinnedArray<Trk::Surface>* getBinnedArray1D(Trk::BinUtility& BinUtility, 
						     std::vector<Trk::TrkDetElementBase*>& cElements,
						     std::vector<Amg::Vector3D>& centersOnModule) const;

    Trk::BinnedArray<Trk::Surface>* getBinnedArray1D1D(Trk::BinUtility& steerBinUtility, 
						       std::vector<Trk::BinUtility*>& subBinUtility,
						       std::vector<Trk::TrkDetElementBase*>& cElements,
						       std::vector<Amg::Vector3D>& centersOnModule) const;
    
 
    Trk::BinnedArray<Trk::Surface>* getBinnedArray2D(Trk::BinUtility& BinUtility,
						     std::vector<Trk::TrkDetElementBase*>& cElements,
						     std::vector<Amg::Vector3D>& centersOnModule) const;

    void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association
    
    const Trk::LayerMaterialProperties* endcapLayerMaterial(double rMin, double rMax, InDet::EndcapLayerTmp* layerTmp, bool isActive = true) const; //!< helper method to construct endcap material
    
    // properties
    bool  m_pixelCase; // flag for pixel/sct
    ServiceHandle<InDet::XMLReaderSvc>  m_xmlReader;
    ToolHandle<IModuleProvider> m_moduleProvider;

    
    size_t              m_endcapLayerBinsR;               //!< Endcap bins for the material in r
    size_t              m_endcapLayerBinsPhi;             //!< Endcap bins for the material in phi
    bool                m_customMaterial;
    float               m_discEnvelope;

  };
}

#endif //ENDCAPBUILDERXML_H
