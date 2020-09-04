/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AtlasFieldMapCondAlg
   @authors RD Schaffer <r.d.schaffer -at- cern.ch>, C Anastopoulos
   @brief create a MagField object for tracking clients

   Purpose: create a MagField map object, filling from a file whose name is taken normally from
   COOL, and save the map in a conditions object for the AtlasFieldCacheCondAlg

**/

// ISF_Services include
#include "MagFieldServices/AtlasFieldMapCondAlg.h"

// Concurrency
#include "GaudiKernel/ConcurrencyFlags.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// TagInfo for special case of turning off toroid or solenoid
#include "EventInfo/TagInfo.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

MagField::AtlasFieldMapCondAlg::AtlasFieldMapCondAlg(const std::string& name, 
                                                   ISvcLocator* pSvcLocator)
    :AthReentrantAlgorithm(name, pSvcLocator){ 
}

MagField::AtlasFieldMapCondAlg::~AtlasFieldMapCondAlg()= default;

StatusCode
MagField::AtlasFieldMapCondAlg::initialize() {

    // CondSvc
    ATH_CHECK( m_condSvc.retrieve() );

    // Read Handle for the map
    ATH_CHECK( m_mapsInputKey.initialize(m_useMapsFromCOOL) );

    // Read Handle for the current
    ATH_CHECK( m_currInputKey.initialize (!m_loadMapOnStart && m_useMapsFromCOOL) );

    // Read Handle for tagInfo
    ATH_CHECK( m_tagInfoKey.initialize() );

    // Output handle for the field map
    ATH_CHECK( m_mapCondObjOutputKey.initialize() );

    // Register write handles for scale factors/cache and the field map
    if (m_condSvc->regHandle(this, m_mapCondObjOutputKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_mapCondObjOutputKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }
    
    ATH_MSG_INFO ( "Initialize: Key " << m_mapCondObjOutputKey.fullKey() << " has been succesfully registered " );
    if (m_useMapsFromCOOL) {
        ATH_MSG_INFO ( "Initialize: Will update the field map from conditions");
    }
    else {
        ATH_MSG_INFO ( "Initialize: Will update the field map from jobOpt file name");
    }
    return StatusCode::SUCCESS;
}

StatusCode
MagField::AtlasFieldMapCondAlg::start() {
    ATH_MSG_DEBUG ( "start: entering  ");

    // Load map on start, we assume that the current context is valid
    if (m_loadMapOnStart) {
        return(execute(Gaudi::Hive::currentContext()));
    }
    
    return StatusCode::SUCCESS;
}

StatusCode
MagField::AtlasFieldMapCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG ( "execute: entering  " << ctx.eventID() );

    // Check if output conditions object with field map object is still valid, if not replace it
    // with new map
    SG::WriteCondHandle<AtlasFieldMapCondObj> mapWriteHandle{m_mapCondObjOutputKey, ctx};
    if (mapWriteHandle.isValid()) {
        ATH_MSG_DEBUG("execute: CondHandle " << mapWriteHandle.fullKey() 
                      << " is still valid. ");
        return StatusCode::SUCCESS;
    }  

    //This will need to be filled before we construct the condition object 
    Cache cache{};

    ATH_CHECK( updateFieldMap(ctx, cache) );

    if (cache.m_fieldMap) {
        ATH_MSG_INFO ( "execute: solenoid zone id  " << cache.m_fieldMap->solenoidZoneId());
    }
    else {
        ATH_MSG_INFO ( "execute: no map read (currents == 0");
    }

    // Save newly created map in conditions object, and record it in the conditions store, with its
    // own range
    auto fieldMapCondObj = std::make_unique<AtlasFieldMapCondObj>();
    // move ownership of the field map to the fieldMapCondObj
    if (cache.m_fieldMap) fieldMapCondObj->setFieldMap(std::move(cache.m_fieldMap));
    if(mapWriteHandle.record(cache.m_mapCondObjOutputRange, std::move(fieldMapCondObj)).isFailure()) {
        ATH_MSG_ERROR("execute: Could not record AtlasFieldMapCondObj object with " 
                      << mapWriteHandle.key() 
                      << " with EventRange " << cache.m_mapCondObjOutputRange
                      << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "execute: recorded AtlasFieldMapCondObj with EventRange " << cache.m_mapCondObjOutputRange );

    return StatusCode::SUCCESS;
}

StatusCode
MagField::AtlasFieldMapCondAlg::updateFieldMap(const EventContext& ctx, Cache& cache) const 
{
    // We get here only for the first creation of the field map, or if the AtlasFieldMapCondObj
    // object, where the map is stored, is no longer valid, i.e. the IOV is out of range

    // Update the map either with the file names from cool or from alg properties, according to m_useMapsFromCOOL

    // Get file names from COOL, or use local ones:
    std::string fullMapFilename;
    std::string soleMapFilename;
    std::string toroMapFilename;

    if (m_useMapsFromCOOL) {

        ATH_MSG_INFO("updateFieldMap: Update map from conditions");
        
        // readin map file name from cool
        SG::ReadCondHandle<CondAttrListCollection> readHandle {m_mapsInputKey, ctx};
        const CondAttrListCollection* attrListColl{*readHandle};
        if (attrListColl == nullptr) {
            ATH_MSG_ERROR("updateFieldMap: Failed to retrieve CondAttributeListCollection with key " << readHandle.key());
            return StatusCode::FAILURE;
        }

        // // handle for COOL field map filenames
        // const DataHandle<CondAttrListCollection> mapHandle;

        // Get the validitiy range
        EventIDRange rangeW;
        if (!readHandle.range(rangeW)) {
            ATH_MSG_FATAL("updateFieldMap: Failed to retrieve validity range for " << readHandle.key());
            return StatusCode::FAILURE;
        }
        cache.m_mapCondObjOutputRange = rangeW;
        ATH_MSG_INFO("updateFieldMap: Update map from conditions: Range of input/output is "
                     << cache.m_mapCondObjOutputRange);

        ATH_MSG_INFO( "updateFieldMap: reading magnetic field map filenames from COOL" );

        for (CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr) {
            const coral::AttributeList &attr = itr->second;
            const std::string &mapType = attr["FieldType"].data<std::string>();
            const std::string &mapFile = attr["MapFileName"].data<std::string>();
            const float soleCur = attr["SolenoidCurrent"].data<float>();
            const float toroCur = attr["ToroidCurrent"].data<float>();
        
            ATH_MSG_INFO("updateFieldMap: found map of type " << mapType << " with soleCur=" << soleCur << " toroCur=" << toroCur << " (path " << mapFile << ")");

            // first 5 letters are reserved (like "file:")
            const std::string mapFile_decoded = mapFile.substr(5);
            if (mapType == "GlobalMap") {
                fullMapFilename = mapFile_decoded;
                cache.m_mapSoleCurrent = soleCur;
                cache.m_mapToroCurrent = toroCur;
            }
            else if (mapType == "SolenoidMap") {
                soleMapFilename = mapFile_decoded;
            }
            else if (mapType == "ToroidMap") {
                toroMapFilename = mapFile_decoded;
            }
            // note: the idea is that the folder contains exactly three maps
            // (if it contains more than 3 maps, then this logic doesn't work perfectly)
            // nominal currents are read from the global map
        }

        if (m_loadMapOnStart) {

            // For loading map on start - online scenario - take the currents from job options
            // And set IOV range to current run number to run number + 1

            cache.m_mapSoleCurrent = m_mapSoleCurrent;
            cache.m_mapToroCurrent = m_mapToroCurrent;

            // Create a range for the current run
            EventIDBase start, stop;
            start.set_run_number(ctx.eventID().run_number());
            start.set_lumi_block(0);
            stop.set_run_number(ctx.eventID().run_number() + 1);
            stop.set_lumi_block(0);
            cache.m_mapCondObjOutputRange = EventIDRange(start,stop);

            ATH_MSG_INFO("updateFieldMap: loadMapOnStart is set, overriding currents from job options - solCur "
                         << cache.m_mapSoleCurrent << ", torCur " << cache.m_mapToroCurrent
                         << " and setting IOV range: " << cache.m_mapCondObjOutputRange);
        }
        else {
            // For normal athena jobs, check the currents in DCS to check if one of the two magnets
            // is OFF so that the correct map can be used.
            // If a field is off, set an IOV validity range to be the current run only.
            // (Note DCS currents have a timestamp-based IOV, so this is not used.)

            //  Note: for the nominal maps from COOL, three maps are available:
            //    - Global with both solenoid and toroid
            //    - Solenoid - just the currents for the solenoid
            //    - Toroid   - just the currents for the toroid

            double soleCurrent;
            double toroCurrent;
            EventIDRange rangeDCS;
            ATH_CHECK( checkCurrentFromConditions(ctx, soleCurrent, toroCurrent, rangeDCS) );

            bool mustCreateIOVRange = false;
            if (soleCurrent < m_soleMinCurrent) {
                cache.m_mapSoleCurrent = 0;
                mustCreateIOVRange = true;
                ATH_MSG_INFO("updateFieldMap: set solenoid current to 0 from DCS");
            }
            if (toroCurrent < m_toroMinCurrent) {
                cache.m_mapToroCurrent = 0;
                mustCreateIOVRange = true;
                ATH_MSG_INFO("updateFieldMap: set toroid current to 0 from DCS");
            }
            if (mustCreateIOVRange) {
                // The currents from DCS are zero for either solenoid or toroid, construct an IOV range for one run
                EventIDBase start, stop;
                // use ctx run number
                start.set_run_number(ctx.eventID().run_number());
                start.set_lumi_block(0);
                stop.set_run_number(ctx.eventID().run_number() + 1);
                stop.set_lumi_block(0);
                cache.m_mapCondObjOutputRange = EventIDRange(start,stop);
                ATH_MSG_INFO("updateFieldMap: map IOV  range " << cache.m_mapCondObjOutputRange);
            }
            else {
                ATH_MSG_INFO("updateFieldMap: currents are OK, will use nominal maps");
            }
        }
    }
    
    else {
        // not m_useMapsFromCOOL - set values from job options
        fullMapFilename = m_fullMapFilename;
        soleMapFilename = m_soleMapFilename;
        toroMapFilename = m_toroMapFilename;
        if (m_mapSoleCurrent < m_soleMinCurrent) {
            cache.m_mapSoleCurrent = 0;
            ATH_MSG_INFO("updateFieldMap: requested solenoid current in JobOpt " << m_mapSoleCurrent << " is below allowed minimum " << m_soleMinCurrent << " setting to 0");
        }
        else {
            cache.m_mapSoleCurrent = m_mapSoleCurrent;
        }
        if (m_mapToroCurrent < m_toroMinCurrent) {
            cache.m_mapToroCurrent = 0;
            ATH_MSG_INFO("updateFieldMap: requested toroid current in JobOpt " << m_mapToroCurrent << " is below allowed minimum " << m_toroMinCurrent << " setting to 0");
        }
        else {
            cache.m_mapToroCurrent = m_mapToroCurrent;
        }
        
        // Create a range for the current run
        EventIDBase start, stop;
        start.set_run_number(ctx.eventID().run_number());
        start.set_lumi_block(0);
        stop.set_run_number(ctx.eventID().run_number() + 1);
        stop.set_lumi_block(0);
        cache.m_mapCondObjOutputRange = EventIDRange(start,stop);

        ATH_MSG_INFO("updateFieldMap: useMapsFromCOOL == false, using default range "
                     << cache.m_mapCondObjOutputRange);
    }

    // We allow to set currents via the TagInfoMgr which adds tags to the TagInfo object - only allowed for offline

    if (m_useMapsFromCOOL) {
    
        // TagInfo object - used to get currents via TagInfoMgr
        SG::ReadHandle<TagInfo> tagInfoH{m_tagInfoKey, ctx}; 
        if (tagInfoH.isValid()) {
            ATH_MSG_INFO("updateFieldMap: tagInfoH " << tagInfoH.fullKey() << " is valid. ");
            int i = 0;
            bool resetCurrentsFromTagInfo = false;
            for ( const auto& tag : tagInfoH->getTags() ) {
                ATH_MSG_DEBUG("updateFieldMap: i, tags: " << i << " " << tag.first << " " << tag.second);
                ++i;
                if (tag.first == "MapSoleCurrent") {
                    cache.m_mapSoleCurrent = std::stof(tag.second);
                    resetCurrentsFromTagInfo = true;
                    ATH_MSG_INFO("updateFieldMap: found MapSoleCurrent in TagInfo, setting the solenoid current "
                                 << cache.m_mapSoleCurrent);
                }
                else 
                    if (tag.first == "MapToroCurrent") {
                        cache.m_mapToroCurrent = std::stof(tag.second);
                        resetCurrentsFromTagInfo = true;
                        ATH_MSG_INFO("updateFieldMap: found MapToroCurrent in TagInfo, setting the toroid current "
                                     << cache.m_mapToroCurrent);
                    }
            }
            if (resetCurrentsFromTagInfo) ATH_MSG_INFO("updateFieldMap: reset currents from TagInfo");
            else                          ATH_MSG_INFO("updateFieldMap: DID NOT reset currents from TagInfo");
        }  
        else {
            ATH_MSG_INFO("updateFieldMap: tagInfoH " << tagInfoH.fullKey() << " is NOT valid. ");
        }
    }

    // Select map file according to the value of the currents which indicate which map is 'on'

    // determine the map to load
    std::string mapFile;
    if ( cache.solenoidOn() && cache.toroidOn() ) mapFile = fullMapFilename;
    else if ( cache.solenoidOn() )                mapFile = soleMapFilename;
    else if ( cache.toroidOn() )                  mapFile = toroMapFilename;
    else {
        // all magnets OFF. no need to read map
        return StatusCode::SUCCESS;
    }
        
    ATH_MSG_INFO ( "updateFieldMap: Set map currents from FieldSvc: solenoid/toroid " 
                   << cache.m_mapSoleCurrent << "," << cache.m_mapToroCurrent);
    ATH_MSG_INFO ( "updateFieldMap: Use map file " << mapFile);

        
    // find the path to the map file
    std::string resolvedMapFile = PathResolver::find_file( mapFile.c_str(), "DATAPATH" );
    if ( resolvedMapFile.empty() ) {
        ATH_MSG_ERROR( "updateFieldMap: Field map file " << mapFile << " not found" );
        return StatusCode::FAILURE;
    }
    // Do checks and extract root file to initialize the map
    if ( resolvedMapFile.find(".root") == std::string::npos ) {
        ATH_MSG_ERROR("updateFieldMap: input file name '" << resolvedMapFile << "' does not end with .root");
        return StatusCode::FAILURE;
    } 
    TFile* rootfile = new TFile(resolvedMapFile.c_str(), "OLD");
    if ( ! rootfile ) {
        ATH_MSG_ERROR("updateFieldMap: failed to open " << resolvedMapFile);
        return StatusCode::FAILURE;
    }
    if ( !rootfile->cd() ) {
        // could not make it current directory
        ATH_MSG_ERROR("updateFieldMap: unable to cd() into the ROOT field map TFile");
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE; 
    }
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");
    if ( tree == nullptr ) {
        // no tree
        ATH_MSG_ERROR("updateFieldMap: TTree 'BFieldMap' does not exist in ROOT field map");
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE;
    }

    // create map
    cache.m_fieldMap = std::make_unique<MagField::AtlasFieldMap>();

    // initialize map
    if (!cache.m_fieldMap->initializeMap( rootfile, cache.m_mapSoleCurrent, cache.m_mapToroCurrent )) {
        // failed to initialize the map
        ATH_MSG_ERROR("updateFieldMap: unable to initialize the map for AtlasFieldMap for file " << resolvedMapFile);
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE;
    }
    
    rootfile->Close();
    delete rootfile;
    

    ATH_MSG_INFO( "updateFieldMap: Initialized the field map from " << resolvedMapFile );

    return StatusCode::SUCCESS;
}


StatusCode
MagField::AtlasFieldMapCondAlg::checkCurrentFromConditions(const EventContext& ctx,
                                                           double& soleCurrent,
                                                           double& toroCurrent,
                                                           EventIDRange& rangeDCS) const
{

    // readin current value
    SG::ReadCondHandle<CondAttrListCollection> readHandle {m_currInputKey, ctx};
    const CondAttrListCollection* attrListColl{*readHandle};
    if (attrListColl == nullptr) {
        ATH_MSG_ERROR("checkCurrentFromConditions: Failed to retrieve CondAttributeListCollection with key " << m_currInputKey.key());
        return StatusCode::FAILURE;
    }


    // Get the validitiy range
    if (!readHandle.range(rangeDCS)) {
        ATH_MSG_FATAL("checkCurrentFromConditions: Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("checkCurrentFromConditions: Range of input currents is " <<  rangeDCS);

    // get magnet currents from DCS
    double solcur{0.};
    double torcur{0.};
    bool gotsol{false};
    bool gottor{false};

    /*
     * due to inconsistencies between CONDBR2 and OFLP200/COMP200 (the former includes channel names
     * in the /EXT/DCS/MAGNETS/SENSORDATA folder, the latter don't), we try to read currents in
     * both ways
     */
    bool hasChanNames{false};
    ATH_MSG_INFO( "checkCurrentFromConditions: Attempt 1 at reading currents from DCS (using channel name)" );
    for ( CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr ) {
        const std::string& name = attrListColl->chanName(itr->first);
        ATH_MSG_INFO( "checkCurrentFromConditions: Trying to read from DCS: [channel name, index, value] "
                      << name << " , " << itr->first << " , " << itr->second["value"].data<float>() );
        if (name.compare("") != 0) {
            hasChanNames = true;
        }
        if ( name.compare("CentralSol_Current") == 0 ) {
            // channel 1 is solenoid current
            solcur = itr->second["value"].data<float>();
            gotsol = true;
        } else if ( name.compare("Toroids_Current") == 0 ) {
            // channel 3 is toroid current
            torcur = itr->second["value"].data<float>();
            gottor = true;
        }
    }
    if ( !hasChanNames ) {
        ATH_MSG_INFO( "checkCurrentFromConditions: Attempt 2 at reading currents from DCS (using channel index)" );
        // in no channel is named, try again using channel index instead
        for ( CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr ) {

            if ( itr->first == 1 ) {
                // channel 1 is solenoid current
                solcur = itr->second["value"].data<float>();
                gotsol = true;
            } else if ( itr->first == 3 ) {
                // channel 3 is toroid current
                torcur = itr->second["value"].data<float>();
                gottor = true;
            }
        }
    }
    if ( !gotsol || !gottor ) {
        if ( !gotsol ) ATH_MSG_ERROR( "checkCurrentFromConditions: Missing solenoid current in DCS information" );
        if ( !gottor ) ATH_MSG_ERROR( "checkCurrentFromConditions: Missing toroid current in DCS information" );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "checkCurrentFromConditions: Currents read from DCS - solenoid " << solcur << " toroid " << torcur );

    // round to zero if close to zero
    if ( solcur < m_soleMinCurrent) {
        solcur = 0.0;
        ATH_MSG_INFO( "checkCurrentFromConditions: Solenoid is off" );
    }
    if ( torcur < m_toroMinCurrent) {
        torcur = 0.0;
        ATH_MSG_INFO( "checkCurrentFromConditions: Toroids are off" );
    }

    soleCurrent = solcur;
    toroCurrent = torcur;

    return StatusCode::SUCCESS;
}
    
