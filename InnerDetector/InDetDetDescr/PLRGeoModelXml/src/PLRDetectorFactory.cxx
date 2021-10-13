/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRGeoModelXml/PLRDetectorFactory.h"

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

#include "PLRGeoModelXml/PLROptions.h"
#include "GeoModelXml/Gmx2Geo.h"
#include "PLRGeoModelXml/PLRGmxInterface.h"
#include "GeoModelXml/GmxInterface.h"

#include "PathResolver/PathResolver.h"

using namespace std;

namespace InDetDDSLHC {

  PLRDetectorFactory::PLRDetectorFactory(InDetDD::AthenaComps *athenaComps,
					     InDetDD::SiCommonItems *commonItems,
					     PLROptions &options) :
    InDetDD::DetectorFactoryBase(athenaComps),
    m_athenaComps(athenaComps),
    m_commonItems(commonItems),
    m_options(&options) {
    //
    //    Create the detector manager... should allow the name to be set
    //
    m_detectorManager = new InDetDD::PixelDetectorManager(detStore(),m_options->detectorName());
  }

  PLRDetectorFactory::~PLRDetectorFactory() {
    // NB the detector manager (m_detectorManager) is stored in the detector store by the Tool and so we don't delete it.
  }

  void PLRDetectorFactory::create(GeoPhysVol *world) {
    ATH_MSG_INFO( "C R E A T E   W O R L D" );
   
    ITk::PLRGmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_moduleTree);
    //    To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
    //    comment out above line and uncomment the following line; also, switch header files above.
    //    GmxInterface gmxInterface;
    
    int flags(0);
    string gmxInput;
    
    if (m_options->gmxFilename().empty()) {
      ATH_MSG_INFO( "gmxFilename not set; getting .gmx from Geometry database Blob not supported"
		     );
    }
    else {
      flags = 0;
      gmxInput = PathResolver::find_file(m_options->gmxFilename(), "DATAPATH");
      if (gmxInput.empty()) { // File not found
	string errMessage("PLRDetectorFactory::create: Unable to find file " + m_options->gmxFilename() +
                                   " with PathResolver; check filename and DATAPATH environment variable");
	throw runtime_error(errMessage);
      }
    }
    
    Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);

    //
    //   Add the tree-top to the detector manager.
    //    
    unsigned int nChildren = world->getNChildVols();
    bool foundVolume = false;

    for (int iChild = nChildren - 1; iChild>=0; --iChild) {
      if( world->getNameOfChildVol(iChild) == "PLR") {
	      // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
	      // the & takes its address.
	      foundVolume  = true;
	      m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
	      break;
      }
    }
  
    if(!foundVolume) ATH_MSG_ERROR("Could not find a logicalVolume named \"PLR\"- this is required to provide the Envelope!");

    doNumerology();

    m_detectorManager->initNeighbours();

  }
  
  InDetDD::PixelDetectorManager * PLRDetectorFactory::getDetectorManager() const {
    return m_detectorManager;
  }

  void PLRDetectorFactory::doNumerology() {
    InDetDD::SiNumerology n;
    
    ATH_MSG_INFO( "\n\nPLR Numerology:\n===============\n\nNumber of parts is " << m_moduleTree.nParts() );
    
  }

}

