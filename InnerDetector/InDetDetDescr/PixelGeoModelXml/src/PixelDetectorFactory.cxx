/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModelXml/PixelDetectorFactory.h"

#include <string>
#include <sstream>

#include "StoreGate/StoreGateSvc.h" // For alignment getAlignableFolderType()
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "DetDescrConditions/AlignableTransformContainer.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "ReadoutGeometryBase/SiCommonItems.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/Version.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "PixelGeoModelXml/PixelOptions.h"
#include "GeoModelXml/Gmx2Geo.h"
#include "PixelGeoModelXml/PixelGmxInterface.h"
#include "GeoModelXml/GmxInterface.h"

#include "PathResolver/PathResolver.h"

using namespace std;

namespace InDetDDSLHC {

  PixelDetectorFactory::PixelDetectorFactory(InDetDD::AthenaComps *athenaComps,
					     InDetDD::SiCommonItems *commonItems,
					     PixelOptions &options) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_athenaComps(athenaComps),
    m_commonItems(commonItems),
    m_options(&options) {
    //
    //    Create the detector manager... should allow the name to be set
    //
    m_detectorManager = new InDetDD::PixelDetectorManager(detStore(),m_options->detectorName());

    //TODO - For now this is always assuemd to be present as a default.
    //To be revisited once the ITk alignment scheme is a bit clearer
    m_detectorManager->addFolder("/Indet/Align");
    //
    //   Set Detector Manager pixel version information
    //
    // No database is used at the moment and reasonable defaults are used
    DecodeVersionKey versionKey(geoDbTagSvc(), "Pixel");
    string versionTag = rdbAccessSvc()->getChildTag("Pixel", versionKey.tag(), versionKey.node());
    string versionName = "SLHC";
    string layout = "SLHC";
    string description = "SLHC Geometry";
    int versionMajorNumber = 0;
    int versionMinorNumber = 0;
    int versionPatchNumber = 0;
    InDetDD::Version version(versionTag, versionName, layout, description, versionMajorNumber,
			     versionMinorNumber, versionPatchNumber);
    m_detectorManager->setVersion(version);
  }

  PixelDetectorFactory::~PixelDetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
  }

  void PixelDetectorFactory::create(GeoPhysVol *world) {
    ATH_MSG_INFO( "C R E A T E   W O R L D" );
   
    ATH_MSG_INFO( m_detectorManager->getVersion().fullDescription() );
    ITk::PixelGmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_moduleTree);
    //    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
    //    comment out above line and uncomment the following line; also, switch header files above.
    //    GmxInterface gmxInterface;
    
    int flags(0);
    string gmxInput;
    
    if (m_options->gmxFilename().empty()) {
      ATH_MSG_INFO( "gmxFilename not set; getting .gmx from Geometry database Blob"
		     );
      flags = 0x1; // Lowest bit ==> string; next bit implies gzip'd but we decided not to gzip
      gmxInput = getBlob();
      string dtdFile = '"' + PathResolver::find_file("geomodel.dtd", "DATAPATH") + '"';
      ATH_MSG_INFO( "dtdFile = " << dtdFile );
      size_t index = gmxInput.find("\"geomodel.dtd\"");
      if (index != string::npos) {
	gmxInput.replace(index, 14, dtdFile);
      }
      else {
	cerr << "Did not find string geomodel.dtd in the gmx input string.\n"; 
      }
    }
    else {
      flags = 0;
      gmxInput = PathResolver::find_file(m_options->gmxFilename(), "DATAPATH");
      if (gmxInput.empty()) { // File not found
	string errMessage("PixelDetectorFactory::create: Unable to find file " + m_options->gmxFilename() +
                                   " with PathResolver; check filename and DATAPATH environment variable");
	throw runtime_error(errMessage);
      }
    }
    
    Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);
    //
    //   Add the tree-top to the detector manager. This also makes it appear as Pixel in VP1.
    //   It is probably the last (most recently added) thing in the world PV so loop from the
    //   back looking for our subdetector name.
    
    //
    unsigned int nChildren = world->getNChildVols();
    bool foundVolume = false;

    for (int iChild = nChildren - 1; iChild>=0; --iChild) {
      //stop if you find a volume for pixel plus PP1...
      if( world->getNameOfChildVol(iChild) == "ITkPixelplusPP1") foundVolume  = true;
      //otherwise, continue looking for a volume just for the pixel
      if (foundVolume  == true || world->getNameOfChildVol(iChild) == "ITkPixel") {
	      // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
	      // the & takes its address.
	      foundVolume  = true;
	      m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
	      break;
      }
    }
  
    if(!foundVolume) ATH_MSG_ERROR("Could not find a logicalVolume named \"ITkPixel\" or \"ITkPixelplusPP1\" - this is required to provide the Envelope!");

    doNumerology();

    m_detectorManager->initNeighbours();

  }
  
  string PixelDetectorFactory::getBlob() {
    DecodeVersionKey versionKey(geoDbTagSvc(), "Pixel");
    const std::string& versionTag  = versionKey.tag();
    const std::string& versionNode = versionKey.node();
    ATH_MSG_INFO( "getBlob: versionTag = " << versionTag );
    ATH_MSG_INFO( "getBlob: versionNode = " << versionNode );

    IRDBAccessSvc *accessSvc = m_athenaComps->rdbAccessSvc();
    //   ADA  accessSvc->connect();
    IRDBRecordset_ptr recordSetPixel = accessSvc->getRecordsetPtr("PIXXDD", versionTag, versionNode);
    if (!recordSetPixel || recordSetPixel->size() == 0) {
      ATH_MSG_FATAL( "getBlob: Unable to obtain Pixel recordSet" );
      throw runtime_error("getBlob: Unable to obtain Pixel recordSet");
    }
    const IRDBRecord *recordPixel =  (*recordSetPixel)[0];
    string pixelString = recordPixel->getString("XMLCLOB");
    //   ADA  accessSvc->disconnect();

    return pixelString;
  }
  
  InDetDD::PixelDetectorManager * PixelDetectorFactory::getDetectorManager() const {
    return m_detectorManager;
  }

  void PixelDetectorFactory::doNumerology() {
    InDetDD::SiNumerology n;
    
    ATH_MSG_INFO( "\n\nPixel Numerology:\n===============\n\nNumber of parts is " << m_moduleTree.nParts() );
    
    bool barrelDone = false;
    for (int b = -1; b <= 1; ++b) {
        if (m_moduleTree.count(b)) {
            msg(MSG::INFO) << "    Found barrel with index " << b << endl;
            n.addBarrel(b);
            if (!barrelDone) {
                n.setNumLayers(m_moduleTree[b].nLayers());
                msg(MSG::INFO) << "        Number of barrel layers = " << n.numLayers() << endl;
                for (LayerDisk::iterator l = m_moduleTree[b].begin(); l != m_moduleTree[b].end(); ++l) {
                    n.setNumEtaModulesForLayer(l->first, l->second.nEtaModules());
                    // All staves within a layer are assumed identical, so we can just look at the first eta
                    n.setNumPhiModulesForLayer(l->first, l->second.begin()->second.nPhiModules());
                    msg(MSG::INFO) << "        layer = " << l->first << " has " << n.numEtaModulesForLayer(l->first) <<
                                     " etaModules each with " <<  n.numPhiModulesForLayer(l->first) << " phi modules" << endl;
                }
                barrelDone = true;
            }
        }

    }
    bool endcapDone = false;

    for (int ec = -2; ec <= 2; ec += 4) {
        if (m_moduleTree.count(ec)) {
            msg(MSG::INFO) << "    Found endcap with index " << ec << endl;
            n.addEndcap(ec);
            if (!endcapDone) {
                n.setNumDiskLayers(m_moduleTree[ec].nLayers());
                msg(MSG::INFO) << "        Number of endcap layers = " << n.numDiskLayers() << endl;
                for (LayerDisk::iterator l = m_moduleTree[ec].begin(); l != m_moduleTree[ec].end(); ++l) {
                    n.setNumDisksForLayer(l->first, l->second.nEtaModules());
                    msg(MSG::INFO) << "        Layer " << l->first << " has " << n.numDisksForLayer(l->first) << " disks" << endl;
                    for (EtaModule::iterator eta = l->second.begin(); eta != l->second.end(); ++eta) {
                        n.setNumPhiModulesForLayerDisk(l->first, eta->first, eta->second.nPhiModules());
                        msg(MSG::DEBUG) << "            Disk " << eta->first << " has " <<
                                           n.numPhiModulesForLayerDisk(l->first, eta->first) << " phi modules" << endl;
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
    msg(MSG::INFO) << "Total number of wafers added is " << totalWafers << endmsg;
    const PixelID *pixelIdHelper = dynamic_cast<const PixelID *> (m_commonItems->getIdHelper());
    msg(MSG::INFO) << "Total number of wafer identifiers is " << pixelIdHelper->wafer_hash_max() << endmsg;

    //    Used in digitization to create one vector big enough to hold all pixels
    n.setMaxNumEtaCells(1);
    for (int d = 0; d < m_detectorManager->numDesigns(); ++d) {
        n.setMaxNumPhiCells(m_detectorManager->getPixelDesign(d)->rows());
        n.setMaxNumEtaCells(m_detectorManager->getPixelDesign(d)->columns());
    }
    msg(MSG::INFO) << "Max. eta cells is " << n.maxNumEtaCells() << endl;
    msg(MSG::INFO) << "Max. phi cells is " << n.maxNumPhiCells() << endl;

    m_detectorManager->numerology() = n;

    msg(MSG::INFO) << "End of numerology\n" << endmsg;

  }

}

