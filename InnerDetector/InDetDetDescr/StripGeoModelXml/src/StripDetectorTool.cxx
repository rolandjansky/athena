/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "StripDetectorTool.h"
#include "StripGmxInterface.h"

#include <SCT_ReadoutGeometry/SCT_DetectorManager.h>
#include <SCT_ReadoutGeometry/SCT_ModuleSideDesign.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelUtilities/DecodeVersionKey.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


namespace ITk
{

StripDetectorTool::StripDetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent)
 : GeoModelXmlTool(type, name, parent)
{
}


StatusCode StripDetectorTool::create()
{
  //retrieve the common stuff
  ATH_CHECK(createBaseTool());
  // Get the detector configuration.
  ATH_CHECK(m_geoModelSvc.retrieve());

  GeoModelExperiment *theExpt;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const SCT_ID *idHelper;
  ATH_CHECK(detStore()->retrieve(idHelper, "SCT_ID"));

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);
 
  //
  // Get the version
  //
  DecodeVersionKey versionKey(&*m_geoModelSvc, "SCT");
  if (versionKey.tag() == "AUTO"){
    ATH_MSG_ERROR("Atlas version tag is AUTO. You must set a version-tag like ATLAS_P2_ITK-00-00-00.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO((versionKey.custom() ? "Building custom " : "Building ")
               << "ITk Strip with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node());
  //
  // Get the Database Access Service and from there the SCT version tag
  //(Do we need this?)
  //
  std::string sctVersionTag = m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());
  ATH_MSG_INFO("ITk Strip Version: " << sctVersionTag);
  //
  // Check if SCT version tag is empty. If so, then the SCT cannot be built.
  // This may or may not be intentional. We just issue an INFO message.
  //
  if (sctVersionTag.empty()) {
    ATH_MSG_INFO("No ITk Strip Version. ITk Strip will not be built.");
    return StatusCode::SUCCESS;
  }
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  ATH_MSG_INFO("C R E A T E   W O R L D");
  GeoPhysVol *world = &*theExpt->getPhysVol();
  m_detManager  = createManager(world);

 //getName here will return whatever is passed as name to DetectorFactory - make this more explicit?
  ATH_CHECK(detStore()->record(m_detManager, m_detManager->getName()));
  theExpt->addManager(m_detManager);

  // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or
  // SCT_DetectorManager
  const InDetDD::SiDetectorManager *siDetManager = m_detManager;
  ATH_CHECK(detStore()->symLink(m_detManager, siDetManager));

  return StatusCode::SUCCESS;
}


StatusCode StripDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_detManager->getName());
  if (proxy) {
    proxy->reset();
    m_detManager = nullptr;
  }
  return StatusCode::SUCCESS;
}


StatusCode StripDetectorTool::registerCallback()
{
  //
  //  Register call-back for software alignment
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


StatusCode StripDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys)){
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

InDetDD::SCT_DetectorManager * StripDetectorTool::createManager(GeoPhysVol * theWorld){

  InDetDD::SCT_DetectorManager * theManager = new InDetDD::SCT_DetectorManager(&*detStore(), m_detectorName);
  //in the factory we would set the version here and write it to the mgr... is it really necessary???
  //Leave it out for now until we find we need it...

  theManager->addFolder(m_alignmentFolderName);

  InDetDD::ITk::StripGmxInterface gmxInterface(theManager, m_commonItems.get(), &m_waferTree);

  //Load the geometry, create the volume, 
  //and then find the volume index within the world to allow it to be added
  //last two arguments are the location in the DB to look for the clob
  //(may want to make those configurables)
  int childIndex = createTopVolume(theWorld,gmxInterface, "SCT", "ITKXDD");
  if(childIndex != -1){ //-1 represents an error state from the above method
   theManager->addTreeTop(&*theWorld->getChildVol(childIndex));
   doNumerology(theManager);
   theManager->initNeighbours();
  }
  else ATH_MSG_FATAL("Could not find the Top Volume!!!");
 
  return theManager;

}

void StripDetectorTool::doNumerology(InDetDD::SCT_DetectorManager * manager)
{
  ATH_MSG_INFO("\n\nSCT Numerology:\n===============\n\nNumber of parts is " << m_waferTree.nParts() << "\n");
  InDetDD::SiNumerology n;

  bool barrelDone = false;
  for (int b = -1; b <= 1; ++b) {
    if (m_waferTree.count(b)) {
      msg(MSG::INFO) << "  Found barrel with index " << b << std::endl;
      n.addBarrel(b);
      if (!barrelDone) {
        n.setNumLayers(m_waferTree[b].nLayers());
        msg(MSG::INFO) << "    Number of barrel layers = " << n.numLayers() << std::endl;
        for (LayerDisk::iterator l = m_waferTree[b].begin(); l != m_waferTree[b].end(); ++l) {
          n.setNumEtaModulesForLayer(l->first, l->second.nEtaModules());
          // All staves within a layer are assumed identical, so we can just look at the first eta
          n.setNumPhiModulesForLayer(l->first, l->second.begin()->second.nPhiModules());
          msg(MSG::INFO) << "    layer = " << l->first << " has " << n.numEtaModulesForLayer(l->first)
                        << " etaModules each with " <<  n.numPhiModulesForLayer(l->first) << " phi modules" << std::endl;
        }
        barrelDone = true;
      }
    }

  }

  bool endcapDone = false;
  for (int ec = -2; ec <= 2; ec += 4) {
    if (m_waferTree.count(ec)) {
      msg(MSG::INFO) << "  Found endcap with index " << ec << std::endl;
      n.addEndcap(ec);
      if (!endcapDone) {
        n.setNumDisks(m_waferTree[ec].nLayers());
        msg(MSG::INFO) << "    Number of endcap wheels = " << n.numDisks() << std::endl;
        for (LayerDisk::iterator l = m_waferTree[ec].begin(); l != m_waferTree[ec].end(); ++l) {
          n.setNumRingsForDisk(l->first, l->second.nEtaModules());
          msg(MSG::INFO) << "    Wheel " << l->first << " has " << n.numRingsForDisk(l->first) << " rings" << std::endl;
          for (EtaModule::iterator eta = l->second.begin(); eta != l->second.end(); ++eta) {
            n.setNumPhiModulesForDiskRing(l->first, eta->first, eta->second.nPhiModules());
            msg(MSG::INFO) << "      Ring " << eta->first << " has "
                           << n.numPhiModulesForDiskRing(l->first, eta->first) << " phi modules" << std::endl;
          }
        }
        endcapDone = true;
      }
    }
  }

  msg(MSG::INFO) << endmsg;

  int totalWafers = 0;
  for (BarrelEndcap::iterator bec = m_waferTree.begin(); bec != m_waferTree.end(); ++bec) {
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
  const SCT_ID *sctIdHelper = dynamic_cast<const SCT_ID *> (m_commonItems->getIdHelper());
  ATH_MSG_INFO("Total number of wafer identifiers is " << sctIdHelper->wafer_hash_max());
  //
  // Used in digitization to create one vector big enough to hold all strips, whichever detector is in consideration.
  // Anyway they are common to pixels and strips! Pixels dominate the EtaCell count (which traditionally the SCT does not set)
  //
  n.setMaxNumEtaCells(1);
  for (int d = 0; d < manager->numDesigns(); ++d) {
    n.setMaxNumPhiCells(manager->getSCT_Design(d)->cells());
  }
  ATH_MSG_INFO("Max. eta cells is " << n.maxNumEtaCells());
  ATH_MSG_INFO("Max. phi cells is " << n.maxNumPhiCells());
  ATH_MSG_INFO("Max. no. strips is " << n.maxNumStrips());

  manager->numerology() = n;

  ATH_MSG_INFO("End of numerology\n");

  //
  //  Alignment preparation
  //
  if (m_alignable) {
    ATH_MSG_INFO("Set up alignment directories");
    const std::string topFolder(m_alignmentFolderName);
    const std::string barrelBase("/SCTB");
    const std::string endcapBase("/SCTE");
    std::string baseName("");

    // Register the keys and the level corresponding to the key
    // and whether it expects a global or local shift.
    // level 0: sensor, level 1: module, level 2, layer/disc, level 3: whole barrel/enccap
    //Before the type was determined from content; would be better to just set it explcitly if we only expect one
    //Format for ITk strip. Set it to "static_run1" for the moment, since this is what exisits in the conditions
    //but this should be revisited in future
    InDetDD::AlignFolderType alignFolderType = InDetDD::static_run1 ;
    manager->addAlignFolderType(alignFolderType);

    switch (alignFolderType) {
    case InDetDD::static_run1:
      manager->addChannel(topFolder + "/ID", 3, InDetDD::global);
      manager->addChannel(topFolder + "/SCT",2, InDetDD::global);
      for (BarrelEndcap::iterator bec = m_waferTree.begin(); bec != m_waferTree.end(); ++bec) {
        switch (bec->first) {
        case -2:
          baseName = topFolder + endcapBase + "C";
        break;
        case 0:
          baseName = topFolder + barrelBase;
        break;
        case 2:
          baseName = topFolder + endcapBase + "A";
        break;
        default:
          ATH_MSG_FATAL("Unknown SCT part with bec-ID " << bec->first << " encountered.");
          throw std::runtime_error("Unknown ITkStrip part for alignment.");
        }
        for (LayerDisk::iterator ld = bec->second.begin(); ld != bec->second.end(); ++ld) {
          std::ostringstream layer;
          layer << ld->first + 1;
          manager->addChannel(baseName + layer.str(), 1, InDetDD::local);
        }
      }
    break;
    // To be added: case InDetDD::timedependent_run2:, see SCT_GeoModel
    default:
      ATH_MSG_FATAL("Alignment requested for unknown alignment folder type in StripDetectorFactory.");
      throw std::runtime_error("Wrong alignment folder type for StripDetectorFactory in StripGeoModelXml.");
    }
  }

  return;
}


} // namespace ITk
