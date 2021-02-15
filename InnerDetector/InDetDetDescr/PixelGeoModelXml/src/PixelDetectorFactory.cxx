/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    //
    //   Set Detector Manager pixel version information
    //
    //At some point we may want to decouple also this DB stuff, and make this ITkStrip specific?
    DecodeVersionKey versionKey(geoDbTagSvc(), "Pixel");
    IRDBRecordset_ptr switchSet = rdbAccessSvc()->getRecordsetPtr("PixelSwitches", versionKey.tag(), versionKey.node());
    const IRDBRecord *switches = (*switchSet)[0];
    string layout = "SLHC";
    if (!switches->isFieldNull("LAYOUT")) {
      layout = switches->getString("LAYOUT");
    }
    string description = "Test geometry";
    if (!switches->isFieldNull("DESCRIPTION")) {
      description = switches->getString("DESCRIPTION");
    }
    string versionTag = rdbAccessSvc()->getChildTag("Pixel", versionKey.tag(), versionKey.node());
    string versionName = switches->getString("VERSIONNAME");
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
    PixelGmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_moduleTree);
    //    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
    //    comment out above line and uncomment the following line; also, switch header files above.
    //    GmxInterface gmxInterface;
    
    int flags(0);
    string gmxInput;
    
    if (m_options->gmxFilename() == "") {
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
      if (gmxInput == "") { // File not found
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
      if (world->getNameOfChildVol(iChild) == "ITkPixel") {
	// The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
	// the & takes its address.
	foundVolume  = true;
	m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
	break;
      }
    }
    
    if(!foundVolume) ATH_MSG_ERROR("Could not find a logicalVolume named \"ITkPixel\" - this is required to provide the Envelope!");

    doNumerology();

    m_detectorManager->initNeighbours();

  }
  
  string PixelDetectorFactory::getBlob() {
    DecodeVersionKey versionKey(geoDbTagSvc(), "Pixel");
    std::string versionTag  = versionKey.tag();
    std::string versionNode = versionKey.node();
    ATH_MSG_INFO( "getBlob: versionTag = " << versionTag );
    ATH_MSG_INFO( "getBlob: versionNode = " << versionNode );

    IRDBAccessSvc *accessSvc = m_athenaComps->rdbAccessSvc();
    //   ADA  accessSvc->connect();
    IRDBRecordset_ptr recordSetPixel = accessSvc->getRecordsetPtr("ITKXDD", versionTag, versionNode);
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
    
  }




}

