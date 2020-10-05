/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AtlasFieldCacheCondAlg
   @authors RD Schaffer <r.d.schaffer -at- cern.ch>, C Anastopoulos
   @brief create a MagField object for tracking clients

   Purpose: for each event, create a MagField object for tracking clients and store in conditions
   Store

**/

// ISF_Services include
#include "MagFieldServices/AtlasFieldCacheCondAlg.h"

// Concurrency
#include "GaudiKernel/ConcurrencyFlags.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

MagField::AtlasFieldCacheCondAlg::AtlasFieldCacheCondAlg(const std::string& name, 
                                                   ISvcLocator* pSvcLocator)
    :AthReentrantAlgorithm(name, pSvcLocator){ 
}

MagField::AtlasFieldCacheCondAlg::~AtlasFieldCacheCondAlg()= default;

StatusCode
MagField::AtlasFieldCacheCondAlg::initialize() {

    // CondSvc
    ATH_CHECK( m_condSvc.retrieve() );

    // Read Handle for the current
    ATH_CHECK( m_currInputKey.initialize (m_useDCS) );

    // Read handle for the field map cond object
    ATH_CHECK( m_mapCondObjInputKey.initialize() );

    // Output handle for scale factors/cache
    ATH_CHECK( m_condObjOutputKey.initialize() );

    // Register write handle for scale factors/cache
    if (m_condSvc->regHandle(this, m_condObjOutputKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_condObjOutputKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ( "Initialize: Key " << m_condObjOutputKey.fullKey() << " has been succesfully registered " );
    if (m_useDCS) {
        ATH_MSG_INFO ( "Initialize: Will update current from conditions"); // 
    }
    else {
        ATH_MSG_INFO ( "Initialize: Will update current from parameters");
    }

    ATH_MSG_INFO ( "Initialize: useDCS, useSoleCurrent, useToroCurrent. " << (int)m_useDCS << ", "
                   << m_useSoleCurrent << ", " << m_useToroCurrent << " LockMapCurrents " << (int)m_lockMapCurrents);

    
    return StatusCode::SUCCESS;
}

StatusCode
MagField::AtlasFieldCacheCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG ( "execute: entering");

    // Check if output conditions object with field cache object is still valid, if not replace it
    // with new current scale factors
    SG::WriteCondHandle<AtlasFieldCacheCondObj> writeHandle{m_condObjOutputKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("execute: CondHandle " << writeHandle.fullKey() 
                      << " is already valid. " 
                      <<" May happen if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }  

    //This will need to be filled before we construct the condition object 
    Cache cache{};

    if (m_useDCS) {
        ATH_CHECK( updateCurrentFromConditions(ctx, cache) );
    }
    else {
        ATH_CHECK( updateCurrentFromParameters(ctx, cache) );
    }
    
    // Must read map cond object to get previously created map
    SG::ReadCondHandle<AtlasFieldMapCondObj> mapReadHandle{m_mapCondObjInputKey, ctx};
    const AtlasFieldMapCondObj* mapCondObj{*mapReadHandle};
    if (mapCondObj == nullptr) {
        ATH_MSG_ERROR("execute: Could not access conditions map for key: " << m_mapCondObjInputKey.fullKey());
        return StatusCode::FAILURE;
    }

    // simple pointer to the map, to be give to the AtlasFieldCacheCondObj, used for the cache field map access
    const MagField::AtlasFieldMap* fieldMap =  mapCondObj->fieldMap();

    // calculate the scale factors
    if (!m_lockMapCurrents) {
        scaleField(cache, fieldMap);
    }

    // save current scale factor in conditions object
    auto fieldCondObj = std::make_unique<AtlasFieldCacheCondObj>();

    // initialize cond obj with current scale factors and the field svc (needed to setup cache)
    fieldCondObj->initialize(cache.m_solScaleFactor, 
                             cache.m_torScaleFactor, 
                             fieldMap);

    // Record in conditions store the conditions object with scale factors and map pointer for cache
    if(writeHandle.record(cache.m_condObjOutputRange, std::move(fieldCondObj)).isFailure()) {
        ATH_MSG_ERROR("execute: Could not record AtlasFieldCacheCondObj object with " 
                      << writeHandle.key() << " with EventRange " << cache.m_condObjOutputRange
                      << " into Conditions Store");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ( "execute: initialized AtlasFieldCacheCondObj and cache with SFs - sol/tor "
                   << cache.m_solScaleFactor << "/" << cache.m_torScaleFactor
                   << ", EventRange " << cache.m_condObjOutputRange );
    if (fieldMap) {
        ATH_MSG_INFO ( "execute: solenoid zone id  " << fieldMap->solenoidZoneId());
    }
    else {
        ATH_MSG_INFO ( "execute: no map read (currents == 0");
    }

    return StatusCode::SUCCESS;
}

StatusCode
MagField::AtlasFieldCacheCondAlg::updateCurrentFromConditions(const EventContext& ctx, Cache& cache) const
{

    // readin current value
    SG::ReadCondHandle<CondAttrListCollection> readHandle {m_currInputKey, ctx};
    const CondAttrListCollection* attrListColl{*readHandle};
    if (attrListColl == nullptr) {
        ATH_MSG_ERROR("updateCurrentFromConditions: Failed to retrieve CondAttributeListCollection with key " << m_currInputKey.key());
        return StatusCode::FAILURE;
    }


    // Get the validitiy range
    EventIDRange rangeW;
    if (!readHandle.range(rangeW)) {
        ATH_MSG_FATAL("updateCurrentFromConditions: Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }
    cache.m_condObjOutputRange = rangeW;
    ATH_MSG_INFO("UpdateCurrentFromConditions: Range of input/output is " <<  cache.m_condObjOutputRange);

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
    ATH_MSG_INFO( "UpdateCurrentFromConditions: Attempt 1 at reading currents from DCS (using channel name)" );
    for ( CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr ) {
        const std::string& name = attrListColl->chanName(itr->first);
        ATH_MSG_INFO( "UpdateCurrentFromConditions: Trying to read from DCS: [channel name, index, value] "
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
        ATH_MSG_INFO( "UpdateCurrentFromConditions: Attempt 2 at reading currents from DCS (using channel index)" );
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
        if ( !gotsol ) ATH_MSG_ERROR( "UpdateCurrentFromConditions: Missing solenoid current in DCS information" );
        if ( !gottor ) ATH_MSG_ERROR( "UpdateCurrentFromConditions: Missing toroid current in DCS information" );
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "UpdateCurrentFromConditions: Currents read from DCS - solenoid " << solcur << " toroid " << torcur );

    // round to zero if close to zero
    if ( solcur < m_soleMinCurrent) {
        solcur = 0.0;
        ATH_MSG_INFO( "UpdateCurrentFromConditions: Solenoid is off" );
    }
    if ( torcur < m_toroMinCurrent) {
        torcur = 0.0;
        ATH_MSG_INFO( "UpdateCurrentFromConditions: Toroids are off" );
    }

    cache.m_solenoidCurrent = solcur;
    cache.m_toroidCurrent   = torcur;

    return StatusCode::SUCCESS;
}


StatusCode
MagField::AtlasFieldCacheCondAlg::updateCurrentFromParameters(const EventContext& ctx, Cache& cache) const
{

    // take the current values from JobOptions
    double solcur{m_useSoleCurrent};
    double torcur{m_useToroCurrent};
    if ( solcur < m_soleMinCurrent ) {
        solcur = 0.0;
        ATH_MSG_INFO( "updateCurrentFromParameters: Solenoid is off" );
    }
    if ( torcur < m_toroMinCurrent) {
        torcur = 0.0;
        ATH_MSG_INFO( "updateCurrentFromParameters: Toroids are off" );
    }
    cache.m_solenoidCurrent = solcur;
    cache.m_toroidCurrent   = torcur;

    // in case of reading from DB or from FILE, the EventID range is always the current run
    EventIDBase start, stop;
    start.set_run_number(ctx.eventID().run_number());
    start.set_lumi_block(0);
    stop.set_run_number(ctx.eventID().run_number()+1);
    stop.set_lumi_block(0);
    cache.m_condObjOutputRange = EventIDRange(start,stop);

    ATH_MSG_INFO("updateCurrentFromParameters: Update from job options: Range of input/output is " << cache.m_condObjOutputRange);

    return StatusCode::SUCCESS;
}


void
MagField::AtlasFieldCacheCondAlg::scaleField(Cache& cache, const MagField::AtlasFieldMap* fieldMap) const
{
    // Calculate the scale factor for solenoid and toroid
    // For each current, if it is 0, either from conditions or jobOpt, set the corresponding SF to 0

    if ( cache.m_solenoidCurrent > 0.0 ) {
        if ( fieldMap && fieldMap->solenoidCurrent() > 0.0 &&
             std::abs( cache.m_solenoidCurrent/fieldMap->solenoidCurrent() - 1.0 ) > 0.001 ){
            cache.m_solScaleFactor = cache.m_solenoidCurrent/fieldMap->solenoidCurrent(); 
        }
        ATH_MSG_INFO( "scaleField: Solenoid field scale factor " << cache.m_solScaleFactor << ". Desired current and map current: "
                      << cache.m_solenoidCurrent << "," << fieldMap->solenoidCurrent());
    }
    else {
        // No SF set, set it to 0 - current was set to zero either here or for the map, or the map was not read in
        cache.m_solScaleFactor = 0;
        ATH_MSG_INFO( "scaleField: Solenoid field scale factor " << cache.m_solScaleFactor << ". Desired current and map current: "
                      << cache.m_solenoidCurrent << "," << ((fieldMap) ? fieldMap->solenoidCurrent() : 0));
    }
    
    //
    if (cache.m_toroidCurrent > 0.0 ) {
        if ( fieldMap && fieldMap->toroidCurrent() > 0.0 &&
             std::abs(cache.m_toroidCurrent/fieldMap->toroidCurrent() - 1.0 ) > 0.001 ) {
            // scale the field in all zones except for the solenoid zone
            cache.m_torScaleFactor = cache.m_toroidCurrent/fieldMap->toroidCurrent();
        }
        ATH_MSG_INFO( "scaleField: Toroid field scale factor " << cache.m_torScaleFactor << ". Desired current and map current: "
                      << cache.m_toroidCurrent << "," << fieldMap->toroidCurrent());
    }
    else {
        cache.m_torScaleFactor = 0;
        ATH_MSG_INFO( "scaleField: Toroid field scale factor " << cache.m_torScaleFactor << ". Desired current and map current: "
                      << cache.m_toroidCurrent << "," << ((fieldMap) ? fieldMap->toroidCurrent() : 0));
    }
}
