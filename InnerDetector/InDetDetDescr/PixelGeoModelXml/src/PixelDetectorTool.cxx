/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelXml/PixelDetectorTool.h"
#include "PixelGeoModelXml/PixelGmxInterface.h"

#include <PixelReadoutGeometry/PixelDetectorManager.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelUtilities/DecodeVersionKey.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


namespace ITk
{

PixelDetectorTool::PixelDetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent)
 : GeoModelXmlTool(type, name, parent)
{
}


StatusCode PixelDetectorTool::create()
{
  // retrieve the common stuff
  ATH_CHECK(createBaseTool());

  GeoModelExperiment *theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const PixelID *idHelper = nullptr;
  ATH_CHECK(detStore()->retrieve(idHelper, "PixelID"));

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);

  //
  // Check the availability
  //
  std::string node{"Pixel"};
  std::string table{"PIXXDD"};
  if (!isAvailable(node, table)) {
    ATH_MSG_INFO("Trying new " << m_detectorName.value() << " database location.");
    node = "InnerDetector";
    table = "PixelXDD";
    if (!isAvailable(node, table)) {
      ATH_MSG_ERROR("No ITk Pixel geometry found. ITk Pixel can not be built.");
      return StatusCode::FAILURE;
    }
  }
  //
  // Create the detector manager
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  auto *manager = new InDetDD::PixelDetectorManager(&*detStore(), m_detectorName);
  manager->addFolder(m_alignmentFolderName);

  InDetDD::ITk::PixelGmxInterface gmxInterface(manager, m_commonItems.get(), &m_moduleTree);

  // Load the geometry, create the volume, 
  // and then find the volume index within the world to allow it to be added
  // last two arguments are the location in the DB to look for the clob
  // (may want to make those configurables)
  int childIndex = createTopVolume(world, gmxInterface, node, table);
  if (childIndex != -1) { // -1 represents an error state from the above method
    manager->addTreeTop(&*world->getChildVol(childIndex));
    doNumerology(manager);
    manager->initNeighbours();
  } else {
    ATH_MSG_FATAL("Could not find the Top Volume!!!");
    return StatusCode::FAILURE;
  }

  // set the manager
  m_detManager = manager;

  ATH_CHECK(detStore()->record(m_detManager, m_detManager->getName()));
  theExpt->addManager(m_detManager);

  // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or
  // PixelDetectorManager
  const InDetDD::SiDetectorManager *siDetManager = m_detManager;
  ATH_CHECK(detStore()->symLink(m_detManager, siDetManager));

  return StatusCode::SUCCESS;
}


StatusCode PixelDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_detManager->getName());
  if (proxy) {
    proxy->reset();
    m_detManager = nullptr;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE ()
{
  //
  // Register call-back for software alignment
  //
  if (m_alignable) {
    if (detStore()->contains<AlignableTransformContainer>(m_alignmentFolderName)) {
      ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_alignmentFolderName);
      const DataHandle<AlignableTransformContainer> atc;
      StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, m_alignmentFolderName);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not register callback on AlignableTransformContainer with folder "
                      << m_alignmentFolderName);
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
                      << m_alignmentFolderName << ", Alignment disabled (only if no Run2 scheme is loaded)!");
    }
  } else {
    ATH_MSG_INFO("Alignment disabled. No callback registered");
    // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
  }
  return StatusCode::SUCCESS;
}


StatusCode PixelDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  //
  // The call-back routine, which just calls the real call-back routine from the manager.
  //
  if (!m_detManager) {
    ATH_MSG_WARNING("Manager does not exist");
    return StatusCode::FAILURE;
  }
  if (m_alignable) {
    return m_detManager->align(I, keys);
  } else {
    ATH_MSG_DEBUG("Alignment disabled. No alignments applied");
    return StatusCode::SUCCESS;
  }
}


void PixelDetectorTool::doNumerology(InDetDD::PixelDetectorManager * manager)
{
  ATH_MSG_INFO( "\n\nPixel Numerology:\n===============\n\nNumber of parts is " << m_moduleTree.nParts() );
  InDetDD::SiNumerology n;

  bool barrelDone = false;
  for (int b = -1; b <= 1; ++b) {
      if (m_moduleTree.count(b)) {
          msg(MSG::INFO) << "    Found barrel with index " << b << std::endl;
          n.addBarrel(b);
          if (!barrelDone) {
              n.setNumLayers(m_moduleTree[b].nLayers());
              msg(MSG::INFO) << "        Number of barrel layers = " << n.numLayers() << std::endl;
              for (LayerDisk::iterator l = m_moduleTree[b].begin(); l != m_moduleTree[b].end(); ++l) {
                  n.setNumEtaModulesForLayer(l->first, l->second.nEtaModules());
                  // All staves within a layer are assumed identical, so we can just look at the first eta
                  n.setNumPhiModulesForLayer(l->first, l->second.begin()->second.nPhiModules());
                  msg(MSG::INFO) << "        layer = " << l->first << " has " << n.numEtaModulesForLayer(l->first) <<
                                    " etaModules each with " <<  n.numPhiModulesForLayer(l->first) << " phi modules" << std::endl;
              }
              barrelDone = true;
          }
      }

  }

  bool endcapDone = false;
  for (int ec = -2; ec <= 2; ec += 4) {
      if (m_moduleTree.count(ec)) {
          msg(MSG::INFO) << "    Found endcap with index " << ec << std::endl;
          n.addEndcap(ec);
          if (!endcapDone) {
              n.setNumDiskLayers(m_moduleTree[ec].nLayers());
              msg(MSG::INFO) << "        Number of endcap layers = " << n.numDiskLayers() << std::endl;
              for (LayerDisk::iterator l = m_moduleTree[ec].begin(); l != m_moduleTree[ec].end(); ++l) {
                  n.setNumDisksForLayer(l->first, l->second.nEtaModules());
                  msg(MSG::INFO) << "        Layer " << l->first << " has " << n.numDisksForLayer(l->first) << " disks" << std::endl;
                  for (EtaModule::iterator eta = l->second.begin(); eta != l->second.end(); ++eta) {
                      n.setNumPhiModulesForLayerDisk(l->first, eta->first, eta->second.nPhiModules());
                      msg(MSG::DEBUG) << "            Disk " << eta->first << " has " <<
                                          n.numPhiModulesForLayerDisk(l->first, eta->first) << " phi modules" << std::endl;
                  }
              }
              endcapDone = true;
          }
      }
  }

  msg(MSG::INFO) << endmsg;

  int totalWafers = 0;
  for (BarrelEndcap::iterator bec = m_moduleTree.begin(); bec != m_moduleTree.end(); ++bec) {
      for (LayerDisk::iterator ld = bec->second.begin(); ld != bec->second.end(); ++ld) {
          for (EtaModule::iterator eta = ld->second.begin(); eta != ld->second.end(); ++eta) {
              for (PhiModule::iterator phi = eta->second.begin(); phi != eta->second.end(); ++phi) {
                  for (Side::iterator side =phi->second.begin(); side != phi->second.end(); ++side) {
                      totalWafers++;
                  }
              }
          }
      }
  }
  ATH_MSG_INFO("Total number of wafers added is " << totalWafers);
  const PixelID *pixelIdHelper = dynamic_cast<const PixelID *> (m_commonItems->getIdHelper());
  ATH_MSG_INFO("Total number of wafer identifiers is " << pixelIdHelper->wafer_hash_max());

  //    Used in digitization to create one vector big enough to hold all pixels
  n.setMaxNumEtaCells(1);
  for (int d = 0; d < manager->numDesigns(); ++d) {
    n.setMaxNumPhiCells(manager->getPixelDesign(d)->rows());
    n.setMaxNumEtaCells(manager->getPixelDesign(d)->columns());
  }
  ATH_MSG_INFO("Max. eta cells is " << n.maxNumEtaCells());
  ATH_MSG_INFO("Max. phi cells is " << n.maxNumPhiCells());

  manager->numerology() = n;

  ATH_MSG_INFO("End of numerology\n");
}

} // namespace ITk
