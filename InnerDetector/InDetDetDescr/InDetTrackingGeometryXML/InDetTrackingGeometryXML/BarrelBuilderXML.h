///////////////////////////////////////////////////////////////////
// BarrelBuilderXML.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef BARRELBUILDERXML_H
#define BARRELBUILDERXML_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// STD
#include <vector>
// TRK
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetElementBase/TrkDetElementBase.h"

namespace Trk {
  class CylinderLayer;
  class AlpineLayer;
  class Surface;
  class Layer;
  class LayerMaterialProperties;
}

namespace InDet {
  class XMLReaderSvc;
  class BarrelLayerTmp;
  class StaveTmp;
  class ModuleTmp;
  class StaveBuilderXML;
  class IModuleProvider;

  static const InterfaceID IID_BarrelBuilderXML("BarrelBuilderXML", 1, 0);

  class BarrelBuilderXML : virtual public AthAlgTool {
  public:
    BarrelBuilderXML(const std::string& t, const std::string& n, const IInterface* p);
    virtual ~BarrelBuilderXML();
    
    // Interface ID for python stuff to work
    static const InterfaceID& interfaceID() { return IID_BarrelBuilderXML; }
    
    // AlgTool initialize and finalize methods
    StatusCode initialize();
    StatusCode finalize();

    // Set Pixel or SCT case
    void setPixelCase(bool isPixel);

    // Get number of layers to build
    unsigned int nbOfLayers() const;
    
    // Get layer stave type
    bool isAlpineLayer(unsigned int ilayer) const;

    // Create a new layer
    Trk::CylinderLayer *createActiveLayer(unsigned int ilayer, int startLayer, int endLayer) const;

  private:
    InDet::BarrelLayerTmp *getLayerTmp(unsigned int ilayer) const;
    std::vector<InDet::StaveTmp*> getStaveTmp(unsigned int ilayer) const;

    Trk::AlpineLayer *createActiveAlpineLayer(unsigned int layer_index, int startLayer, int endLayer) const;

    Trk::CylinderLayer *createActiveCylinderLayer(unsigned int layer_index, int startLayer, int endLayer) const;
    void createCylinderBarrelModules(unsigned int ilayer, unsigned int istave, InDet::StaveTmp* staveTmp, std::vector<Trk::TrkDetElementBase*>& cElements) const;
    Trk::TrkDetElementBase* CylinderDetElement(unsigned int layerCounter, InDet::StaveTmp *staveTmp, int iphi, int iz) const;

    // JL: Common functions to EndcapBuilder & BarrelBuilder : should be moved to separate class 
    Trk::BinnedArray<Trk::Surface>* getBinnedArray1D1D(Trk::BinUtility& steerBinUtility,
						       std::vector<Trk::BinUtility*>& subBinUtility,  
						       std::vector<const Trk::TrkDetElementBase*>& cElements) const;

    void setPhiNeighbors(std::vector<Trk::TrkDetElementBase*>& pElements, std::vector<Trk::TrkDetElementBase*>& cElements) const;

    void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association
    
    const Trk::LayerMaterialProperties* barrelLayerMaterial(double r, double hz, InDet::StaveTmp* staveTmp, bool isActive = true) const;  //!< helper method to construct barrel material

    inline static bool sortElements (const Trk::TrkDetElementBase* el_min,const Trk::TrkDetElementBase* el_max) { return el_min->center().z()<el_max->center().z(); }

    // properties
    bool  m_pixelCase; // flag for pixel/sct
    ServiceHandle<InDet::XMLReaderSvc>  m_xmlReader;
    ToolHandle<StaveBuilderXML> m_staveBuilder;
    ToolHandle<IModuleProvider> m_moduleProvider;
    
    size_t              m_barrelLayerBinsZ;               //!< Barrel bins for the material in z
    size_t              m_barrelLayerBinsPhi;             //!< Barrel bins for the material in phi
    bool		m_customMaterial;
    bool    m_impMatDescription;
    
  };
}

#endif //BARRELBUILDERXML_H

