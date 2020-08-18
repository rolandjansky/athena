/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// AtlasFieldMapCondAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_ATLASFIELDMAPCONDALG_H
#define MAGFIELDSERVICES_ATLASFIELDMAPCONDALG_H 

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"
#include "EventInfo/TagInfo.h"

// #include "MagFieldInterfaces/IMTMagFieldSvc.h"

#include "MagFieldConditions/AtlasFieldMapCondObj.h"

namespace MagField {

    class AtlasFieldMapCondAlg : public AthReentrantAlgorithm 
    {
    public:
        AtlasFieldMapCondAlg(const std::string& name,ISvcLocator* pSvcLocator);
        virtual ~AtlasFieldMapCondAlg();

        StatusCode start() override final;
        StatusCode initialize() override final;
        StatusCode execute(const EventContext& ctx) const override final;

    private:

        /*
         *  Cache of the variables to be updated before we write the conditions object
         */
        struct Cache{
            bool solenoidOn() { return (m_mapSoleCurrent > 0.0); }
            bool toroidOn()   { return (m_mapToroCurrent > 0.0); }
            // current associated with the map
            double m_mapSoleCurrent {7730.};
            double m_mapToroCurrent {20400.};
            // field map names
            std::string m_fullMapFilename{"MagneticFieldMaps/bfieldmap_7730_20400_14m.root"}; // all magnets on
            std::string m_soleMapFilename{"MagneticFieldMaps/bfieldmap_7730_0_14m.root"};     // solenoid on / toroid off
            std::string m_toroMapFilename{"MagneticFieldMaps/bfieldmap_0_20400_14m.root"};    // toroid on / solenoid off
            // field map - pointer and event id range
            std::unique_ptr<MagField::AtlasFieldMap> m_fieldMap;
            
            //"infinite in case we do not update from COOL" 
            EventIDRange m_mapCondObjOutputRange {EventIDRange()}; // default range covers everything (run/event and timestamp)
        }; 

        // get the field map
        StatusCode updateFieldMap(const EventContext& ctx, Cache& cache) const;

        // get DCS currents to decide which field map file to read
        StatusCode checkCurrentFromConditions(const EventContext& ctx,
                                              double& soleCurrent,
                                              double& toroCurrent,
                                              EventIDRange& rangeDCS) const;
        
        /// map file names - if not read from cool
        Gaudi::Property<std::string> m_fullMapFilename {this,
                                                        "FullMapFile", "MagneticFieldMaps/bfieldmap_7730_20400_14m.root",
                                                        "File storing the full magnetic field map"};
        Gaudi::Property<std::string> m_soleMapFilename {this,
                                                        "SoleMapFile", "MagneticFieldMaps/bfieldmap_7730_0_14m.root",
                                                        "File storing the solenoid-only magnetic field map"};
        Gaudi::Property<std::string> m_toroMapFilename {this,
                                                        "ToroMapFile", "MagneticFieldMaps/bfieldmap_0_20400_14m.root",
                                                        "File storing the toroid-only magnetic field map"};
        /// nominal current for the maps
        Gaudi::Property<double>      m_mapSoleCurrent  {this,
                                                        "MapSoleCurrent", 7730., "Nominal solenoid current (A)"};
        Gaudi::Property<double>      m_mapToroCurrent  {this,
                                                        "MapToroCurrent", 20400., "Nominal toroid current (A)"};

        // threshold below which currents are considered zero
        Gaudi::Property<double> m_soleMinCurrent {this, 
                                                  "SoleMinCurrent", 1.0, "Minimum solenoid current (A) for which solenoid is considered ON"};
        Gaudi::Property<double> m_toroMinCurrent {this, 
                                                  "ToroMinCurrent", 1.0, "Minimum toroid current (A) for which toroid is considered ON"};


        // flag to load map on start
        Gaudi::Property<bool> m_loadMapOnStart  {this,
                                                 "LoadMapOnStart", false , "Load the magnetic field map at start"};

        // flag to read magnet map filenames from COOL
        Gaudi::Property<bool> m_useMapsFromCOOL {this,
                                                 "UseMapsFromCOOL", true , "Get magnetic field map filenames from COOL"};

        // COOL folder name containing field maps
        // map input key
        SG::ReadCondHandleKey<CondAttrListCollection> m_mapsInputKey
        {this,
         "COOLMapsFolderName", "/GLOBAL/BField/Maps", "Name of the COOL folder containing field maps"};

        // AtlasFieldMapCondObj - magnet field conditions object containing the map file names
        SG::WriteCondHandleKey<AtlasFieldMapCondObj> m_mapCondObjOutputKey
        {this, 
         "AtlasFieldMapCondObj", "fieldMapCondObj", "Name of key for the Magnetic Field conditions object with the map file names"};

        // COOL folder name containing current information
        // current input key
        SG::ReadCondHandleKey<CondAttrListCollection> m_currInputKey
        {this, 
         "COOLCurrentsFolderName", "/EXT/DCS/MAGNETS/SENSORDATA", "Name of the COOL folder containing magnet currents"};

        ServiceHandle<ICondSvc> m_condSvc { this, 
                                            "CondSvc", "CondSvc", "conditions service" };

        // TagInfo object - used to get currents via TagInfoMgr
        SG::ReadHandleKey<TagInfo> m_tagInfoKey {this,
                                                 "TagInfo", "DetectorStore+ProcessingTags", "tag info key"};

    };
}

#endif //> !MAGFIELDSERVICES_ATLASFIELDMAPCONDALG_H
