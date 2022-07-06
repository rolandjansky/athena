/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/MuonDetectorTool.h"

#include "AmdcDb/AmdcDb.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelKernel/GeoPerfUtils.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonGeoModel/StationSelector.h"
#include "MuonGeoModel/MuonDetectorFactory001.h"
#include "MuonGeoModel/MuonDetectorFactoryLite.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include <fstream>

using namespace MuonGM;

/**
 ** Constructor(s)
 **/
MuonDetectorTool::MuonDetectorTool(const std::string &type, const std::string &name, const IInterface *parent) : 
    GeoModelTool(type, name, parent) {
    declareInterface<IGeoModelTool>(this);
}

StatusCode MuonDetectorTool::initialize() {
    ATH_MSG_INFO("Initializing ...");
    return StatusCode::SUCCESS;
}
MuonDetectorTool::~MuonDetectorTool() { 
    if (m_detector) {
        delete m_detector;
        m_detector = nullptr;
    } 
}

/**
 ** Create the Detector Node corresponding to this tool
 **/
StatusCode MuonDetectorTool::create() {

    std::ofstream geoModelStats;
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;


    MuonGM::MuonDetectorManager *mgr=nullptr;
    if (createFactory(mgr).isFailure())
        return StatusCode::FAILURE;

    if (!m_detector) {
        ATH_CHECK(detStore()->record(mgr,mgr->getName()));

        GeoModelExperiment *theExpt = nullptr;
        ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
        theExpt->addManager(mgr);

        m_manager = mgr;
    }

    if (m_dumpMemoryBreakDown) {
        umem = GeoPerfUtils::getMem();
        ucpu = float(GeoPerfUtils::getCpu() / 100.);
        geoModelStats << "At MuonDetectorTool::everything done     \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = " << umem - mem
                      << " \t Delta T =" << ucpu - cpu << std::endl;
        geoModelStats.close();
    }

    Muon::MuonSectorMapping mapping;
    /// add hash look-up for TGC sectors to the detector store here
    if (m_manager && m_manager->tgcIdHelper()) {
        unsigned int tgcHashMax = m_manager->tgcIdHelper()->detectorElement_hash_max();
        std::vector<int> *sectorHashVec = new std::vector<int>(tgcHashMax, 0);
        for (unsigned int i = 0; i < tgcHashMax; ++i) {
            const MuonGM::TgcReadoutElement *detEl = m_manager->getTgcReadoutElement(i);
            if (detEl)
                (*sectorHashVec)[i] = mapping.getSector(detEl->center().phi());
        }
        if (detStore()->record(sectorHashVec, "TGC_SectorMapping").isFailure()) {
            ATH_MSG_WARNING(" failed to record TGC sector mapping ");
        }
    }
    return StatusCode::SUCCESS;
}

StatusCode MuonDetectorTool::createFactory(MuonGM::MuonDetectorManager * & mgr) const {

    std::ofstream geoModelStats;
    int mem = 0;
    float cpu = 0;
    int umem = 0;
    float ucpu = 0;


    if (m_dumpMemoryBreakDown) {
        geoModelStats.open("MuonGeoModelStatistics_MuonDetectorTool");
        mem = GeoPerfUtils::getMem();
        cpu = float(GeoPerfUtils::getCpu() / 100.);
        geoModelStats << "At MuonDetectorTool::create entry point: \t SZ= " << mem << " Kb \t Time = " << cpu << " seconds " << std::endl;

        umem = GeoPerfUtils::getMem();
        ucpu = float(GeoPerfUtils::getCpu() / 100.);
        geoModelStats << "At MuonDetectorTool::resetting to 0:     \t SZ= " << umem - mem << " Kb \t Time = " << ucpu - cpu << " seconds " << std::endl;
        mem = umem;
        cpu = ucpu;
    }

    ATH_MSG_VERBOSE(" Show properties (user sett.s or default): LayoutName                 "
                    << m_layout << endmsg << " Show properties (user sett.s or default): IncludeCutouts             " << m_includeCutouts << endmsg
                    << " Show properties (user sett.s or default): IncludeCutoutsBog          " << m_includeCutoutsBog << endmsg
                    << " Show properties (user sett.s or default): IncludeCtbBis              " << m_includeCtbBis);

    // Get the detector configuration.
    IGeoModelSvc *geoModel;
    ATH_CHECK(service("GeoModelSvc", geoModel));

    std::string AtlasVersion = geoModel->atlasVersion();
    std::string MuonVersion = geoModel->muonVersionOverride();

    ATH_MSG_INFO("create MuonDetectorTool");
    ATH_MSG_INFO("(from GeoModelSvc)    AtlasVersion = <" << AtlasVersion << ">  MuonVersion = <" << MuonVersion << ">");

    // Unless we are using custom muons, the switch positions are going to
    // come from the database:

    std::string detectorKey = MuonVersion.empty() ? AtlasVersion : MuonVersion;
    std::string detectorNode = MuonVersion.empty() ? "ATLAS" : "MuonSpectrometer";
    ATH_MSG_INFO("Keys for Muon Switches are  (key) " << detectorKey << " (node) " << detectorNode);

    std::string tempLayout = m_layout;
    std::map<std::string, std::string> altAsciiDBMap{};

    GeoModelExperiment *theExpt = nullptr;
    ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
    GeoPhysVol *world = &*theExpt->getPhysVol();

    // Get the detector configuration.
    ServiceHandle<IGeoDbTagSvc> geoDbTag("GeoDbTagSvc",name());
    ATH_CHECK(geoDbTag.retrieve());
    ServiceHandle<IRDBAccessSvc> accessSvc(geoDbTag->getParamSvcName(),name());
    ATH_CHECK(accessSvc.retrieve());
    GeoModelIO::ReadGeoModel* sqliteReader = geoDbTag->getSqliteReader();
    
    if (sqliteReader) {

      ATH_MSG_INFO("New DD Activated; Muon detector description input from SQLITE fie");

      MuonDetectorFactoryLite theFactory(detStore().operator->(),sqliteReader);
      theFactory.setRDBAccess(&*accessSvc);

      theFactory.create(world);  


      mgr=theFactory.getDetectorManager();
      mgr->setMinimalGeoFlag(m_minimalGeoFlag);
      mgr->setGeometryVersion(tempLayout);
      mgr->setCachingFlag(m_cachingFlag);
      mgr->setCachingFlag(m_fillCache_initTime);
      mgr->setMdtDeformationFlag(m_enableMdtDeformations);
      mgr->setMdtAsBuiltParamsFlag(m_enableMdtAsBuiltParameters);
      mgr->setNswAsBuiltParamsFlag(m_enableNswAsBuiltParameters);
      
      if (m_fillCache_initTime) {
	mgr->fillCache();
      } else {
	// cache for RPC / TGC / CSC must be filled once forever
	mgr->fillRpcCache();
	mgr->fillTgcCache();
	mgr->fillCscCache();
      }
    
      return StatusCode::SUCCESS;
    }
    //
    // New DD:  action ends here!!
    // 
    //=====================================================
    // 
    // Old DD: action starts here:  
    //
    
    MuonDetectorFactory001 theFactory(detStore().operator->());
    

    if (MuonVersion == "CUSTOM")
        ATH_MSG_WARNING("Detector Information coming from a custom configuration !!");
    else {

        ATH_MSG_DEBUG("Detector Information coming from the database (job options IGNORED)");

        IRDBRecordset_ptr switchSet = accessSvc->getRecordsetPtr("MuonSwitches", detectorKey, detectorNode);
        if ((*switchSet).size() == 0)
            return StatusCode::FAILURE;
        const IRDBRecord *switches = (*switchSet)[0];

        // m_layout                = switches->getString("LAYOUTNAME");
        tempLayout = switches->getString("LAYOUTNAME");
        //       m_includeInertMaterials = switches->getInt("BUILDINERTMATERIALS");
        //       m_minimalgeo            = switches->getInt("MINIMALGEO");
        if (MuonVersion == "") {
            MuonVersion = accessSvc->getChildTag("MuonSpectrometer", detectorKey, detectorNode);
            ATH_MSG_INFO("(from GeoModelSvc) in AtlasVersion = <" << AtlasVersion << ">  default MuonVersion is <" << MuonVersion << ">");
        }

        ATH_MSG_DEBUG(" m_altAsztFile: " << m_altAsztFile);
        // use ascii file to read in ASZT parameters
        if (m_altAsztFile != "")
            altAsciiDBMap.insert(std::make_pair("ASZT", m_altAsztFile));
        if (m_altCscIntAlinesFile != "")
            altAsciiDBMap.insert(std::make_pair("IACSC", m_altCscIntAlinesFile));
        if (m_altMdtAsBuiltFile != "")
            altAsciiDBMap.insert(std::make_pair("XAMDT", m_altMdtAsBuiltFile));
    }

    //
    // Locate the top level experiment node
    //

    int tempControlCscIntAlines = m_controlCscIntAlines;
    if (!m_useCscIntAlines)
        tempControlCscIntAlines = 0;
    ATH_MSG_INFO("Properties have been set as follows: " << endmsg
                                                         // <<"    LayoutName                     "<< m_layout.substr(0,1) <<endmsg
                                                         << "    LayoutName                     " << tempLayout.substr(0, 1) << endmsg << "    IncludeCutouts                 "
                                                         << m_includeCutouts << endmsg << "    IncludeCutoutsBog              " << m_includeCutoutsBog << endmsg
                                                         << "    IncludeCtbBis                  " << m_includeCtbBis << endmsg << "    ControlAlines                  "
                                                         << m_controlAlines << endmsg << "    MinimalGeoFlag                 " << m_minimalGeoFlag << endmsg
                                                         << "    EnableCscIntAlignment          " << m_useCscIntAlines << endmsg << "    EnableCscIntAlignmentFromGM    "
                                                         << m_useCscIntAlinesFromGM << endmsg << "    FillCacheInitTime              " << m_fillCache_initTime);
    if (m_useCscIntAlines)
        ATH_MSG_INFO("    ControlCscIntAlines            " << tempControlCscIntAlines);
    else
        ATH_MSG_INFO("    ControlCscIntAlines   reset to " << tempControlCscIntAlines);
    ATH_MSG_INFO("    EnableMdtDeformations          " << m_enableMdtDeformations);
    ATH_MSG_INFO("    EnableMdtAsBuiltParameters     " << m_enableMdtAsBuiltParameters);
    ATH_MSG_INFO("    EnableNswAsBuiltParameters     " << m_enableNswAsBuiltParameters);

    if (m_stationSelection > 0) {
        StationSelector::SetSelectionType(m_stationSelection);
        if ((m_selectedStations.size() + m_selectedStEta.size() + m_selectedStPhi.size()) < 1) {
            ATH_MSG_ERROR(" **** Badly set Option " << endmsg << " **** SelectedStations size =" << m_selectedStations.size() << endmsg
                                                    << " **** SelectedStJzz    size =" << m_selectedStations.size() << endmsg
                                                    << " **** SelectedStJff    size =" << m_selectedStations.size() << endmsg << " **** while StationSelection = 1");
            return (StatusCode::FAILURE);
        }
        for (unsigned int i = 0; i < m_selectedStations.size(); i++) {
            ATH_MSG_INFO("          Selected stations      " << m_selectedStations[i]);
        }

        if (m_selectedStEta.size() > 0) {
            for (unsigned int i = 0; i < m_selectedStEta.size(); i++)
                ATH_MSG_INFO("          Selected Jzz locations  " << m_selectedStEta[i]);
        }
        if (m_selectedStPhi.size() > 0) {
            for (unsigned int i = 0; i < m_selectedStPhi.size(); i++)
                ATH_MSG_INFO("          Selected Jff locations  " << m_selectedStPhi[i]);
        }
    }

    if (m_dumpMemoryBreakDown) {
        umem = GeoPerfUtils::getMem();
        ucpu = float(GeoPerfUtils::getCpu() / 100.);
        geoModelStats << "At MuonDetectorTool::material mgr done   \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = " << umem - mem
                      << " \t Delta T =" << ucpu - cpu << std::endl;
        mem = umem;
        cpu = ucpu;
    }

    if (nullptr == m_detector) {
        IRDBAccessSvc *access = nullptr;
        if (m_amdcDb)
            ATH_CHECK(service("AmdcDb", access));
        else
            ATH_CHECK(service("RDBAccessSvc", access));

        bool isAmdcDb = false;
        if (dynamic_cast<AmdcDb *>(access) && m_amdcDb) {
            ATH_MSG_INFO("AmdcDb is used instead of RDBAccessSvc");
            isAmdcDb = true;
            ATH_MSG_INFO("AmdcDb->InitializedSvc() is true");
        }

        theFactory.setDBAtlasVersion(AtlasVersion);
        theFactory.setDBMuonVersion(MuonVersion);
        theFactory.setDBkey(detectorKey);
        theFactory.setDBnode(detectorNode);
        theFactory.setNSWABLineAsciiPath(m_NSWABLineAsciiPath);
        theFactory.setAmdcDb(isAmdcDb);
        theFactory.setLayout(tempLayout);
        theFactory.setCutoutsFlag(m_includeCutouts);
        theFactory.setCutoutsBogFlag(m_includeCutoutsBog);
        theFactory.setCtbBisFlag(m_includeCtbBis);
        theFactory.setControlAlines(m_controlAlines);
        theFactory.setMinimalGeoFlag(m_minimalGeoFlag);
        if (!m_useCscIntAlines)
            tempControlCscIntAlines = 0;
        theFactory.setControlCscIntAlines(tempControlCscIntAlines);
        theFactory.setDumpMemoryBreakDown(m_dumpMemoryBreakDown);
        theFactory.setCachingFlag(m_cachingFlag);
        theFactory.setCacheFillingFlag(m_fillCache_initTime);
        theFactory.setMdtDeformationFlag(m_enableMdtDeformations);
        theFactory.setMdtAsBuiltParaFlag(m_enableMdtAsBuiltParameters);
        theFactory.setNswAsBuiltParaFlag(m_enableNswAsBuiltParameters);
        theFactory.setFineClashFixingFlag(m_enableFineClashFixing);
        theFactory.hasCSC(m_hasCSC);
        theFactory.hasSTgc(m_hasSTgc);
        theFactory.hasMM(m_hasMM);
        if (m_stationSelection > 0)
            theFactory.setSelection(m_selectedStations, m_selectedStEta, m_selectedStPhi);

        theFactory.setRDBAccess(access);
        // theFactory.setUseRDB(1);
        theFactory.setAltAsciiDBMap(altAsciiDBMap);
        theFactory.setDumpAlines(m_dumpAlines);
        theFactory.setDumpCscIntAlines(m_dumpCscIntAlines);
        theFactory.setUseCscIntAlinesFromGM(m_useCscIntAlinesFromGM);
        try {
            //
            // This strange way of casting is to avoid an
            // utterly brain damaged compiler warning.
            //
            theFactory.create(world);
        } catch (const std::bad_alloc &) {
            ATH_MSG_FATAL("Could not create new MuonDetectorNode!");
            return StatusCode::FAILURE;
        }

        if (m_dumpMemoryBreakDown) {
            umem = GeoPerfUtils::getMem();
            ucpu = float(GeoPerfUtils::getCpu() / 100.);
            geoModelStats << "At MuonDetectorTool::factory created     \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = " << umem - mem
                          << " \t Delta T =" << ucpu - cpu << std::endl;
            mem = umem;
            cpu = ucpu;
        }
        // Register the MuonDetectorNode instance with the Transient Detector Store
        MuonGM::MuonDetectorManager *theManager = theFactory.getDetectorManager();
        // Init ABline historical container --- will write there A/B lines from ORACLE / ascii file if any
        theManager->initABlineContainers();
        if ((theManager->initCSCInternalAlignmentMap()).isFailure())
            return StatusCode::FAILURE; // does nothing other then checking the size (map is built while reading data from the primary source)

        if (m_fillCache_initTime) {
            theManager->fillCache();
        } else {
            // cache for RPC / TGC / CSC must be filled once forever
            theManager->fillRpcCache();
            theManager->fillTgcCache();
            theManager->fillCscCache();
        }

        if (m_dumpMemoryBreakDown) {
            umem = GeoPerfUtils::getMem();
            ucpu = float(GeoPerfUtils::getCpu() / 100.);
            geoModelStats << "At MuonDetectorTool::trk cache done      \t SZ= " << umem << " Kb \t Time = " << ucpu << " seconds  ---- \t DeltaM = " << umem - mem
                          << " \t Delta T =" << ucpu - cpu << std::endl;
            mem = umem;
            cpu = ucpu;
        }
    }
    mgr=theFactory.getDetectorManager();
    return StatusCode::SUCCESS;
}

StatusCode MuonDetectorTool::clear() {
    SG::DataProxy *proxy = detStore()->proxy(ClassID_traits<MuonGM::MuonDetectorManager>::ID(), m_manager->getName());
    if (proxy) {
        proxy->reset();
        m_manager = nullptr;
    }
    return StatusCode::SUCCESS;
}
