/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "HGTD_GMX_DetectorTool.h"
#include "HGTD_GmxInterface.h"

#include <HGTD_ReadoutGeometry/HGTD_DetectorManager.h>

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>


HGTD_GMX_DetectorTool::HGTD_GMX_DetectorTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : GeoModelXmlTool(type, name, parent)
{
}


StatusCode HGTD_GMX_DetectorTool::create()
{
    // retrieve the common stuff
    ATH_CHECK(createBaseTool());

    const HGTD_ID *idHelper{};
    ATH_CHECK(detStore()->retrieve(idHelper, "HGTD_ID"));

    GeoModelExperiment *theExpt{};
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

    m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);

    //
    // Check the availability
    //
    std::string node{"InnerDetector"};
    std::string table{"HGTDXDD"};
    if (!isAvailable(node, table)) {
        ATH_MSG_ERROR("No HGTD geometry found. HGTD can not be built.");
        return StatusCode::FAILURE;
    }
    //
    // Create the detector manager
    //
    // The * converts a ConstPVLink to a ref to a GeoVPhysVol
    // The & takes the address of the GeoVPhysVol
    GeoPhysVol *world = &*theExpt->getPhysVol();
    auto *manager = new HGTD_DetectorManager(&*detStore());

    HGTD_GmxInterface gmxInterface(manager, m_commonItems.get());

    // Load the geometry, create the volume,
    // and then find the volume index within the world to allow it to be added
    // last two arguments are the location in the DB to look for the clob
    // (may want to make those configurables)
    int childIndex = createTopVolume(world, gmxInterface, node, table);
    if (childIndex != -1) { //-1 represents an error state from the above method
        manager->addTreeTop(&*world->getChildVol(childIndex));
    } else {
        ATH_MSG_FATAL("Could not find the Top Volume!!!");
        return StatusCode::FAILURE;
    }

    // set the manager
    m_detManager = manager;

    // getName here will return whatever is passed as name to DetectorFactory - make this more explicit?
    ATH_CHECK(detStore()->record(m_detManager, m_detManager->getName()));
    theExpt->addManager(m_detManager);

    return StatusCode::SUCCESS;
}


StatusCode HGTD_GMX_DetectorTool::clear()
{
    // Release manager from the detector store
    SG::DataProxy* _proxy = detStore()->proxy(ClassID_traits< HGTD_DetectorManager >::ID(), m_detManager->getName());
    if (_proxy) {
        _proxy->reset();
        m_detManager = nullptr;
    }

    return StatusCode::SUCCESS;
}


StatusCode HGTD_GMX_DetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE () // Thread unsafe detStore()->regFcn (callback) is used.
{
    // Register call-back for software alignment
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
                            << m_alignmentFolderName << ", Alignment disabled!");
        }
    } else {
        ATH_MSG_INFO("Alignment disabled. No callback registered");
        // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
    }
    return StatusCode::SUCCESS;
}


StatusCode HGTD_GMX_DetectorTool::align(IOVSVC_CALLBACK_ARGS_P( I, keys ))
{
    // The call-back routine, which just calls the real call-back routine from the manager.
    if (!m_detManager) {
        ATH_MSG_WARNING( "Manager does not exist for " << I <<" "<< keys );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "No align method yet implemented for HGTD_DetectorManager. Nothing to do." );

    return StatusCode::SUCCESS;
}
