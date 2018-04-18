// PACKAGE
#include "GeomACTS/GeoModelLayerBuilder.hpp"
#include "GeomACTS/IdentityHelper.hpp"

// ACTS
#include "ACTS/Tools/ILayerBuilder.hpp"
#include "ACTS/Material/SurfaceMaterialProxy.hpp"
#include "ACTS/Surfaces/CylinderSurface.hpp"
#include "ACTS/Surfaces/DiscSurface.hpp"
#include "ACTS/Layers/GenericApproachDescriptor.hpp"
#include "ACTS/Layers/GenericApproachDescriptor.hpp"
#include "ACTS/Utilities/ApproachDescriptor.hpp"

const Acts::LayerVector
Acts::GeoModelLayerBuilder::negativeLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector nVector;
  mutableThis->buildLayers(nVector, -1);
  return nVector;
}

const Acts::LayerVector
Acts::GeoModelLayerBuilder::centralLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector cVector;
  mutableThis->buildLayers(cVector, 0);
  return cVector;
}

const Acts::LayerVector
Acts::GeoModelLayerBuilder::positiveLayers() const
{
  // @todo Remove this hack once the m_elementStore mess is sorted out
  auto        mutableThis = const_cast<GeoModelLayerBuilder*>(this);
  LayerVector pVector;
  mutableThis->buildLayers(pVector, 1);
  return pVector;

}

std::vector<std::shared_ptr<const Acts::GeoModelDetectorElement>>
Acts::GeoModelLayerBuilder::getDetectorElements() const
{
  auto siDetMng = static_cast<const InDetDD::SiDetectorManager*>(m_cfg.mng);
  
  std::vector<std::shared_ptr<const GeoModelDetectorElement>> elements;

  //if (siDetMng) {
    InDetDD::SiDetectorElementCollection::const_iterator iter;
    for (iter = siDetMng->getDetectorElementBegin();
      iter != siDetMng->getDetectorElementEnd();
      ++iter) {
      const InDetDD::SiDetectorElement* siDetElement = *iter;
      elements.push_back(std::make_shared<const GeoModelDetectorElement>(siDetElement));
    }
  //}
  //else {
    //throw std::domain_error("GeoModelLayerBuilder does not know how "
                            //"to operator on this type of Detector Manager");
  //}

  return elements;
}

void
Acts::GeoModelLayerBuilder::buildLayers(LayerVector& layersOutput, int type)
{

  std::vector<std::shared_ptr<const GeoModelDetectorElement>> elements = getDetectorElements();


  std::map<std::pair<int, int>, std::vector<const Surface*>> layers;

  for (const auto &element : elements) {
    //const InDetDD::SiDetectorElement* siDetElement = *iter;

    //auto element
      //= std::make_shared<const GeoModelDetectorElement>(siDetElement);

    // std::cout << "ELEMENT: ";
    // std::cout << element->bec() << " ";
    // std::cout << element->layer_disk() << " ";;
    // std::cout << element->eta_module() << " ";
    // std::cout << element->phi_module() << " ";
    // std::cout << "Z = " << element->surface().center().z() << std::endl;

    //IdentityHelper id(element);
    IdentityHelper id = element->identityHelper();

    // wrong subdetector
    //if (m_cfg.subdetector != element->det()) continue;
    // want barrel but not barrel
    if (type == 0 && id.bec() != 0) continue;

    if (type != 0 && id.bec() == 0) continue;
    // want endcap, but wrong side
    if (type != 0 && type * id.bec() < 0) continue;

    m_cfg.elementStore->push_back(element);

    // const PixelID* idhlp = dynamic_cast<const
    // PixelID*>(siDetElement->getIdHelper());
    // int layer_disk = idhlp->layer_disk(siDetElement->identify());
    // int eta_module = idhlp->eta_module(siDetElement->identify());

    // std::cout << "SELECTED: ";
    // std::cout << element->bec() << " ";
    // std::cout << "(" << element->layer_disk() << " " << layer_disk << ") ";
    // std::cout << "(" << element->eta_module() << " " << eta_module << ") ";
    // std::cout << element->phi_module() << " ";
    // std::cout << "Z = " << element->surface().center().z() << std::endl;

    int elementLayer;
    elementLayer = id.layer_disk();

    std::pair<int, int> layerKey(elementLayer, id.bec());

    if (layers.count(layerKey) == 0) {
      // layer not set yet
      layers.insert(std::make_pair(layerKey, std::vector<const Surface*>()));
      // layers.at(elementLayer) = std::vector<const Surface*>();
    }

    // push into correct layer
    layers.at(layerKey).push_back(&element->surface());

    // layer_disk_max appears to be broken, determine empirically
    // int elementLayer = static_cast<inz>(element->layer_disk());
    // nLayers             = std::max(nLayers, elementLayer + 1);

    // sort surface into corresponding layer vector
    // resize layer surface vector if necessary
    // if (layers.size() < elementLayer + 1) {
    // layers.resize(elementLayer + 1);
    //}

    // std::vector<const Surface*>& layerElements
    //= layers.at(elementLayer);
    // layerElements.push_back(&element->surface());
  }

  for (const auto& layerPair : layers) {

    std::unique_ptr<Acts::ApproachDescriptor> approachDescriptor = nullptr;
    std::shared_ptr<const SurfaceMaterialProxy> materialProxy = nullptr;

    std::vector<const Surface*> layerSurfaces = layerPair.second;

    if (type == 0) {  // BARREL
      // layers and extent are determined, build actual layer
      ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {20, 20};
        
      // Copied from layercreator
      //double layerThickness
        //= std::abs(pl.minZ - pl.maxZ) + pl.envZ.first + pl.envZ.second;
  
      double binPosZ   = 0.5 * (pl.minZ + pl.maxZ);
      double envZShift = 0.5 * (-pl.envZ.first + pl.envZ.second);
      double layerZ    = binPosZ + envZShift;
      double layerHalfZ
        = std::abs(pl.maxZ + pl.envZ.second - layerZ);

      auto transform
        = std::make_shared<const Transform3D>(Translation3D(0., 0., -layerZ));
      // set up approach descriptor

      Acts::CylinderSurface* innerBoundary 
        = new Acts::CylinderSurface(transform, pl.minR, layerHalfZ);
      
      Acts::CylinderSurface* outerBoundary 
        = new Acts::CylinderSurface(transform, pl.maxR, layerHalfZ);
      
      Acts::CylinderSurface* centralSurface 
        = new Acts::CylinderSurface(transform, (pl.minR + pl.maxR)/2., layerHalfZ);
      
      // @TODO: needs to be configurable
      size_t binsZ = 100;
      size_t binsPhi = 100;

      Acts::BinUtility materialBinUtil(
          binsPhi, -M_PI, M_PI, Acts::closed, Acts::binPhi);
      materialBinUtil += Acts::BinUtility(
            binsZ, -layerHalfZ, layerHalfZ, Acts::open, Acts::binZ, transform);

      materialProxy
        = std::make_shared<const SurfaceMaterialProxy>(materialBinUtil);

      ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
          "inner=0 / center=1 / outer=2 ) : " << "inner");
      ACTS_VERBOSE("with binning: [" << binsPhi << ", " << binsZ << "]");

      ACTS_VERBOSE("Created ApproachSurfaces for cylinder layer at:");
      ACTS_VERBOSE(" - inner:   R=" << pl.minR);
      ACTS_VERBOSE(" - central: R=" << (pl.minR + pl.maxR)/2.);
      ACTS_VERBOSE(" - outer:   R=" << pl.maxR);



      // set material on inner
      // @TODO: make this configurable somehow
      innerBoundary->setAssociatedMaterial(materialProxy);

      approachDescriptor 
        = std::make_unique<Acts::GenericApproachDescriptor<Acts::Surface>>(
            std::vector<const Acts::Surface*>({innerBoundary, 
                                               centralSurface,
                                               outerBoundary}));

      auto layer = m_cfg.layerCreator->cylinderLayer(layerSurfaces, 
                                                     equidistant, 
                                                     equidistant, 
                                                     pl, 
                                                     transform,
                                                     std::move(approachDescriptor));

      layersOutput.push_back(layer);
    } else {  // ENDCAP
      ProtoLayer pl(layerSurfaces);
      pl.envR    = {0, 0};
      pl.envZ    = {5, 5};


      // copied from layercreator
      double layerZ
        = 0.5 * (pl.minZ - pl.envZ.first + pl.maxZ
            + pl.envZ.second);
      double layerThickness = (pl.maxZ - pl.minZ)
        + pl.envZ.first + pl.envZ.second;

      double layerZInner = layerZ - layerThickness/2.;
      double layerZOuter = layerZ + layerThickness/2.;

      if (layerZInner > layerZOuter) std::swap(layerZInner, layerZOuter);

      auto transformNominal
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZ));
      
      auto transformInner
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZInner));
      
      auto transformOuter
        = std::make_shared<const Transform3D>(Translation3D(0., 0., layerZOuter));

      Acts::DiscSurface* innerBoundary 
        = new Acts::DiscSurface(transformInner, pl.minR, pl.maxR);
      
      Acts::DiscSurface* nominalSurface 
        = new Acts::DiscSurface(transformNominal, pl.minR, pl.maxR);
      
      Acts::DiscSurface* outerBoundary 
        = new Acts::DiscSurface(transformOuter, pl.minR, pl.maxR);

      size_t binsPhi = 100;
      size_t binsR = 50;

      Acts::BinUtility materialBinUtil(
          binsPhi, -M_PI, M_PI, Acts::closed, Acts::binPhi);
      materialBinUtil += Acts::BinUtility(
            binsR, -pl.minR, pl.maxR, Acts::open, Acts::binR, transformNominal);

      ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
          "inner=0 / center=1 / outer=2 ) : " << "inner");
      ACTS_VERBOSE("with binning: [" << binsPhi << ", " << binsR << "]");

      ACTS_VERBOSE("Created ApproachSurfaces for disc layer at:");
      ACTS_VERBOSE(" - inner:   Z=" << layerZInner);
      ACTS_VERBOSE(" - central: Z=" << layerZ);
      ACTS_VERBOSE(" - outer:   Z=" << layerZOuter);
      

      // set material on inner
      // @TODO: make this configurable somehow
      innerBoundary->setAssociatedMaterial(materialProxy);

      approachDescriptor 
        = std::make_unique<Acts::GenericApproachDescriptor<Acts::Surface>>(
            std::vector<const Acts::Surface*>({innerBoundary, 
                                               nominalSurface,
                                               outerBoundary}));

      auto layer = m_cfg.layerCreator->discLayer(layerSurfaces, 
                                                 equidistant, 
                                                 equidistant, 
                                                 pl,
                                                 transformNominal,
                                                 std::move(approachDescriptor));

      layersOutput.push_back(layer);
    }
  }
}
