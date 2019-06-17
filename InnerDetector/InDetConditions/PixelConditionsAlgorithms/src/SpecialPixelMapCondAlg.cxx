/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SpecialPixelMapCondAlg.h"
#include "InDetIdentifier/PixelID.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"


SpecialPixelMapCondAlg::SpecialPixelMapCondAlg (const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator),
    m_pixelID(nullptr),
    m_geoModelSvc("GeoModelSvc",name), m_condSvc("CondSvc",name)
{
    declareProperty("GeoModelService", m_geoModelSvc);
}

SpecialPixelMapCondAlg::~SpecialPixelMapCondAlg() {

}

constexpr size_t s_max=4;

StatusCode SpecialPixelMapCondAlg::initialize()
{
    ATH_CHECK(detStore()->retrieve( m_pixelID, "PixelID" ));
    ATH_CHECK( m_condSvc.retrieve() );

    ATH_CHECK(m_geoModelSvc.retrieve());
    if(m_geoModelSvc->geoConfig()!=GeoModel::GEO_RUN1 && m_geoModelSvc->geoConfig()!=GeoModel::GEO_TESTBEAM)m_isIBL=true;
    if(!m_isIBL)m_isIBL = m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN1 && m_pixelID->wafer_hash_max()==2048; // second chance !

    const InDetDD::PixelDetectorManager* pixman=nullptr;
    ATH_CHECK(detStore()->retrieve( pixman, "Pixel" ));
    InDetDD::SiDetectorElementCollection::const_iterator itermin, itermax;
    itermin = pixman->getDetectorElementBegin();
    itermax = pixman->getDetectorElementEnd();
    for(auto &iter=itermin; iter !=itermax; ++iter) {
        const InDetDD::SiDetectorElement* element = *iter;
        if(element ==0) continue;
        Identifier ident = element->identify();
        if(m_pixelID->is_pixel(ident)) { // OK this Element is included
            const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
            if(!design)continue;
            unsigned int mchips = design->numberOfCircuits();
            int mrow =design->rows();
            if(mchips==8||abs(m_pixelID->barrel_ec(ident))==2||(m_pixelID->barrel_ec(ident)==0&&m_pixelID->layer_disk(ident)>0)) {
                mchips *=2; // guess numberOfCircuits()
                mrow /=2; // number of chips in row
            }
            unsigned int i(0);
            if(mrow==336)i = 1;
            else if(mrow==672) i = 2;
            else if(mrow==339) i = 3;
            else if(mrow==678) i = 4;
            unsigned int idsh = m_pixelID->wafer_hash(ident);
            m_chips[idsh]=mchips*10 + i;
            //m_calibobjs.push_back(std::make_pair(ident,mchips));
            // write up the dump calibration here with default value:
        }
    }

    if (m_maskLayers &&  !m_layersToMask.size() && !m_disksToMask.size()) {
        ATH_MSG_DEBUG( "Layer/Disk masking enabled, but no layer/disk specified!" );
        m_maskLayers = false;
    }

    if (!m_maskLayers &&  (m_layersToMask.size() || m_disksToMask.size())) {
        ATH_MSG_DEBUG( "Layer/Disk to mask specified, but masking is disabled!" );
    }

    if (m_maskLayers &&  m_disksToMask.size() && (std::find(m_disksToMask.begin(), m_disksToMask.end(),0)!=m_disksToMask.end())) {
        ATH_MSG_WARNING( "0th Disk not defined (-N to N) - check your setup!" );
    }


    if(m_condAttrListCollectionKeys.size() != m_specialPixelMapKeys.size()) {
        ATH_MSG_FATAL("Inputs do not match outputs, check configuration");
        return StatusCode::FAILURE;
    }
    {   //Limited scope to help avoid errors
        std::array<SG::ReadCondHandleKey<CondAttrListCollection>*, s_max> condarray{{ &m_ConAttrList1, &m_ConAttrList2, &m_ConAttrList3, &m_ConAttrList4 }};
        if(m_condAttrListCollectionKeys.size() > s_max) {
            ATH_MSG_FATAL("Too many collection keys provided, improve code (this was a messy comprimise)");
            return StatusCode::FAILURE;
        }
        for(size_t i=0; i< m_condAttrListCollectionKeys.size(); i++) {
            *(condarray.at(i)) = m_condAttrListCollectionKeys[i];
            ATH_CHECK((*condarray.at(i)).initialize());
        }
    }
    {
        std::array<SG::WriteCondHandleKey<DetectorSpecialPixelMap>*,s_max> outarray{{ &m_pixmap1, &m_pixmap2, &m_pixmap3, &m_pixmap4 }};
        for(size_t i=0; i< m_specialPixelMapKeys.size(); i++) {
            *(outarray.at(i)) = m_specialPixelMapKeys[i];
            if((*outarray.at(i)).initialize().isFailure()){
               ATH_MSG_WARNING("Failed to register " << m_specialPixelMapKeys[i] << ", possibly already exists because of old service. Fix later in migration");
               *(outarray.at(i)) = std::string("");
            }
            else ATH_CHECK(m_condSvc->regHandle(this, *outarray.at(i)));
        }
    }

    if(!m_overlayFolder.key().empty()) ATH_CHECK(m_overlayFolder.initialize());
    if(!m_overlayKey.key().empty()){
       if(m_overlayKey.initialize().isSuccess()){
          ATH_MSG_INFO("Registering " << m_overlayKey.key());
          ATH_CHECK(m_condSvc->regHandle(this, m_overlayKey));
          ATH_MSG_INFO("Registering done" << m_overlayKey.key());
       }else{
          ATH_MSG_WARNING("Failed to register " << m_overlayKey.key() << ", possibly already exists because of old service. Fix later in migration");
          m_overlayKey = std::string("");
       }
    }
    if(!m_maskingOverlayKey.key().empty()) {
       if(m_maskingOverlayKey.initialize().isSuccess()) ATH_CHECK(m_condSvc->regHandle(this, m_maskingOverlayKey));
       else {
          ATH_MSG_WARNING("Failed to register " << m_maskingOverlayKey.key() << ", possibly already exists because of old service. Fix later in migration");
          m_maskingOverlayKey = std::string("");
       }
    }
    if(!m_outputFolder.key().empty()) {
       if(m_outputFolder.initialize().isSuccess()) ATH_CHECK(m_condSvc->regHandle(this, m_outputFolder));
       else { 
          ATH_MSG_WARNING("Failed to register " << m_outputFolder.key() << ", possibly already exists because of old service. Fix later in migration");
          m_outputFolder = std::string("");
       }
    }
    if(!m_outputLongFolder.key().empty()) {
       if(m_outputLongFolder.initialize().isSuccess()) ATH_CHECK(m_condSvc->regHandle(this, m_outputLongFolder));
       else {
          ATH_MSG_WARNING("Failed to register " << m_outputLongFolder.key() << ", possibly already exists because of old service. Fix later in migration");
          m_outputLongFolder = std::string("");
       }
    }
    if(!m_overlayLongFolder.key().empty()){
       if(m_overlayLongFolder.initialize().isSuccess()) ATH_CHECK(m_condSvc->regHandle(this, m_overlayLongFolder));
       else {
         ATH_MSG_WARNING("Failed to register " << m_overlayLongFolder.key() << ", possibly already exists because of old service. Fix later in migration");
         m_overlayLongFolder = std::string("");
       }
    }
    if(!m_overlayLongKey.key().empty()) {
       if(m_overlayLongKey.initialize()) ATH_CHECK(m_condSvc->regHandle(this, m_overlayLongKey));
       else {
         ATH_MSG_WARNING("Failed to register " << m_overlayLongKey.key() << ", possibly already exists because of old service. Fix later in migration");
         m_overlayLongKey = std::string("");
       }
    }
    return StatusCode::SUCCESS;
}

StatusCode SpecialPixelMapCondAlg::execute (const EventContext& ctx) const
{
    ATH_MSG_DEBUG("Entering SpecialPixelMapCondAlg");
    const std::array<const SG::ReadCondHandleKey<CondAttrListCollection>*, s_max> condarray{{ &m_ConAttrList1, &m_ConAttrList2, &m_ConAttrList3, &m_ConAttrList4 }};
    const std::array<const SG::WriteCondHandleKey<DetectorSpecialPixelMap>*, s_max> outarray{{ &m_pixmap1, &m_pixmap2, &m_pixmap3, &m_pixmap4 }};
    std::array<bool, s_max> newobject; newobject.fill(true);
    const size_t N = m_condAttrListCollectionKeys.size();//should never be higher than s_max, check in initialize
    std::array<DetectorSpecialPixelMap*, s_max> ptrcache;
    std::array<EventIDRange, s_max> rangearray;
    std::vector<EventIDRange> spmrange;
    ptrcache.fill(nullptr);
    DetectorSpecialPixelMap* &spm = ptrcache[0];

    for (size_t i=0; i<N; i++) {
        if(condarray[i]->key().empty()) continue;
        SG::WriteCondHandle<DetectorSpecialPixelMap> outhandle(*outarray[i], ctx);
        if (outhandle.isValid()) {
          ATH_MSG_DEBUG((*outarray[i]).key() << " already valid");
          newobject[i] = false;       
        }
    }
    if(std::accumulate(newobject.begin(), newobject.begin() + N, 0) == 0){
      ATH_MSG_DEBUG("No new objects needed");
      return StatusCode::SUCCESS;
    }


    for (size_t i=0; i<N; i++) {
        if(condarray[i]->key().empty()) continue;
        ATH_MSG_DEBUG("Attempting to retrieve " << condarray[i]->key() );
        SG::ReadCondHandle<CondAttrListCollection> attrListColl(*condarray[i], ctx);
        if(!attrListColl.range(rangearray[i])) {
           ATH_MSG_ERROR("Failed to retrieve validity range for " << attrListColl.key());
           return StatusCode::FAILURE;
       }
       ATH_CHECK(createFromDetectorStore(attrListColl.retrieve(), true, ptrcache[i]));
       newobject[i] = true;
    }


    if(N>0) spmrange.push_back(rangearray.at(0));
    if(m_mergePixelMaps && newobject[0]) {
        for(size_t i=1; i<N; i++) {
            if(ptrcache[i]==nullptr) continue;
            if(newobject[i] ==false){
              SG::ReadCondHandleKey<DetectorSpecialPixelMap> key(outarray[i]->key());
              SG::ReadCondHandle<DetectorSpecialPixelMap> inhandle(key, ctx);
              *spm += *inhandle.retrieve();
            }else{
              spm->merge(ptrcache[i]);
            }
            spmrange.push_back(rangearray[i]);
        }
    }
    if( !m_overlayFolder.key().empty() &&  !SG::WriteCondHandle<DetectorSpecialPixelMap>(m_overlayKey, ctx).isValid() ) {

        DetectorSpecialPixelMap* overlay =nullptr;
        DetectorSpecialPixelMap* maskoverlay=nullptr;
        SG::ReadCondHandle<CondAttrListCollection> overlayfolder(m_overlayFolder, ctx);
        std::vector<EventIDRange> overlayRanges;
        EventIDRange r;
        if(!overlayfolder.range(r)) {
            ATH_MSG_ERROR("Failed to retrieve validity range for " << overlayfolder.key());
            return StatusCode::FAILURE;
        }
        overlayRanges.push_back(r);
        ATH_CHECK(createFromDetectorStore(overlayfolder.retrieve(), !m_moduleLevelOverlay, overlay ));

        if(m_maskLayers) {
            
            ATH_CHECK(createMaskingOverlay(maskoverlay));
            SG::WriteCondHandle<DetectorSpecialPixelMap> writeh(m_maskingOverlayKey, ctx);
            ATH_MSG_DEBUG("Record condition handle " << m_maskingOverlayKey.key() << " " << r);
            ATH_CHECK(writeh.record(r, maskoverlay));
            ATH_CHECK(registerCondAttrListCollection(maskoverlay, spm, ctx, r));
            overlay->merge(maskoverlay);

        }
        if( !m_overlayLongFolder.key().empty() ) {
            DetectorSpecialPixelMap* overlaylong = nullptr;
            SG::ReadCondHandle<CondAttrListCollection> overlayfolder(m_overlayLongFolder, ctx);
            EventIDRange r;
            if(!overlayfolder.range(r)) {
               ATH_MSG_ERROR("Failed to retrieve validity range for " << overlayfolder.key());
               return StatusCode::FAILURE;
            }
            overlayRanges.push_back(r);
            ATH_CHECK(createFromDetectorStore(overlayfolder.retrieve(), !m_moduleLevelOverlay, overlaylong));
            overlay->merge( overlaylong );
        }



        for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++) {
            if( m_moduleLevelOverlay ) {
                if( (*overlay)[i] != 0 ) {
                    *(*spm)[i] = *(*overlay)[i];
                }
            }
            else {
                unsigned int moduleStatus = (*overlay)[i]->moduleStatus();
                if( moduleStatus != 0 ) {
                    (*spm)[i]->setModuleStatus(moduleStatus);
                }
                if( (*overlay)[i]->hasSpecialChips() ) {
                    for( unsigned int chip = 0; chip < (*overlay)[i]->chipsPerModule(); ++chip ) {
                        unsigned int chipStatus = (*overlay)[i]->chipStatus(chip);
                        if( chipStatus != 0 ) {
                            (*spm)[i]->setChipStatus(chip, chipStatus);
                        }
                    }
                }

                if( (*overlay)[i]->hasSpecialColumnPairs() ) {
                    for( unsigned int chip = 0; chip < (*overlay)[i]->chipsPerModule(); ++chip ) {
                        if( (*overlay)[i]->hasSpecialColumnPairs(chip) ) {
                            for( unsigned int columnPair = 0;
                                    columnPair < (*overlay)[i]->columnsPerChip() / 2; ++columnPair ) {
                                unsigned int columnPairStatus = (*overlay)[i]->columnPairStatus(chip, columnPair);
                                if( columnPairStatus != 0 ) {
                                    (*spm)[i]->setColumnPairStatus(chip, columnPair, columnPairStatus);
                                }
                            }
                        }
                    }
                }

                for(ModuleSpecialPixelMap::const_iterator pixel = (*overlay)[i]->begin();
                        pixel != (*overlay)[i]->end(); ++pixel) {
                    (*spm)[i]->setPixelStatus(pixel->first, pixel->second);
                }
            }
        }

        EventIDRange rangeIntersection;
        if(overlayRanges.size() == 2) rangeIntersection = EventIDRange::intersect(overlayRanges[0], overlayRanges[1]);
        else if (overlayRanges.size() == 1)rangeIntersection = overlayRanges[0];
        else ATH_MSG_ERROR("Logic error line " << __LINE__);
        spmrange.insert(spmrange.end(), overlayRanges.begin(), overlayRanges.end());
        if(overlay){
           SG::WriteCondHandle<DetectorSpecialPixelMap> handleoverlay(m_overlayKey, ctx);
           ATH_MSG_DEBUG("Record condition handle " << m_overlayKey.key() << " " << rangeIntersection);
           ATH_CHECK(handleoverlay.record(rangeIntersection, overlay ));
        }
    }

    for(size_t i=1; i<N; i++) {//Write i>0
       if(newobject[i] ==false) continue;
       SG::WriteCondHandle<DetectorSpecialPixelMap> spmhandle(*outarray[i], ctx);
       ATH_MSG_DEBUG("Record condition handle " << outarray[i]->key() << " " << rangearray[i]);
       ATH_CHECK(spmhandle.record(rangearray[i], ptrcache[i]));
    }
    //write i=0
    if(newobject[0]){
       SG::WriteCondHandle<DetectorSpecialPixelMap> spmhandle(*outarray[0], ctx);
       EventIDRange spmIntersect = spmrange[0];
       for(size_t i = 1; i<spmrange.size(); i++) spmIntersect = EventIDRange::intersect(spmIntersect, spmrange[i]);
       ATH_MSG_DEBUG("Record condition handle " << outarray[0]->key() << " " << spmIntersect);
       ATH_CHECK(spmhandle.record(spmIntersect, spm));
    }

    return StatusCode::SUCCESS;
}


unsigned int SpecialPixelMapCondAlg::getChips(const unsigned int index) const {
    unsigned int i =index;
    std::unordered_map<unsigned int, unsigned int> ::const_iterator it= m_chips.find(i);
    if(it !=m_chips.end()) return (*it).second;
    ATH_MSG_ERROR( " Unable to retrieve the number of chips for module idhash "<<index);
    return 0;
}


StatusCode SpecialPixelMapCondAlg::createFromDetectorStore(const CondAttrListCollection *attrListColl,
        bool fillMissing, DetectorSpecialPixelMap*& cacheoutput) const {

    auto spm = std::make_unique< DetectorSpecialPixelMap>();
    spm->resize(m_pixelID->wafer_hash_max(),NULL);



    if( attrListColl == nullptr ) {
        ATH_MSG_ERROR( "Unable to retrieve CondAttrListCollection," << " constructing empty DetectorSpecialPixelMap" );
    }
    else {
        ATH_MSG_DEBUG( "CondAttrListCollection retrieved" );

        if( attrListColl->size() == 0 ) {
            ATH_MSG_INFO( "CondAttrListCollection is empty," << " constructing empty DetectorSpecialPixelMap" );
        }
        else {

            for(auto attrList = attrListColl->begin(); attrList != attrListColl->end(); ++attrList) {

                std::ostringstream attrStr;
                (*attrList).second.toOutputStream(attrStr);
                ATH_MSG_VERBOSE( "ChanNum " << (*attrList).first << " Attribute list " << attrStr.str() );

                CondAttrListCollection::ChanNum chanNum = (*attrList).first;
                CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
                const IOVRange& range = (*iovIt).second;

                if(range.start().isTimestamp()) {
                    ATH_MSG_VERBOSE( "Range timestamp : from " << range.start().timestamp() << " until " << range.stop().timestamp() );
                }
                else {
                    ATH_MSG_VERBOSE( "Range R/E : from " << range.start().run() << " " << range.start().event() << " until " << range.stop().run() << " " << range.stop().event() );
                }

                const unsigned int moduleID = static_cast<unsigned int>((*attrList).second["moduleID"].data<const int>());
                unsigned int idhash;
                if (m_forceNewDBContent) idhash = IdentifierHash(moduleID);
                else if(m_isIBL) {
                    if(m_dummy || IdentifierHash(moduleID)>m_pixelID->wafer_hash_max()) {
                        //	    continue (useful to transport the old DB to new DB with IBL!;
                        int component = static_cast<int>((moduleID & (3 << 25)) / 33554432) * 2 - 2;
                        unsigned int layer = (moduleID & (3 << 23)) / 8388608  ;
                        if(component==0)layer +=1; // shift layer
                        unsigned int phi = (moduleID & (63 << 17)) / 131072 ;
                        int eta = static_cast<int>((moduleID & (15 << 13)) / 8192) - 6 ;
                        Identifier id = m_pixelID->wafer_id( component, layer, phi, eta );
                        idhash =  m_pixelID->wafer_hash(id);
                    }
                    else {
                        idhash = IdentifierHash(moduleID);
                    }
                }
                else {
                    int component = static_cast<int>((moduleID & (3 << 25)) / 33554432) * 2 - 2;
                    unsigned int layer = (moduleID & (3 << 23)) / 8388608 ;
                    unsigned int phi = (moduleID & (63 << 17)) / 131072 ;
                    int eta = static_cast<int>((moduleID & (15 << 13)) / 8192) - 6 ;
                    Identifier id = m_pixelID->wafer_id( component, layer, phi, eta );
                    idhash =  m_pixelID->wafer_hash(id);
                }
                if( idhash < m_pixelID->wafer_hash_max()) {

                    coral::AttributeList::const_iterator attribute = (*attrList).second.begin();
                    ++attribute;
                    if( (*attribute).specification().typeName() == "blob" ) {

                        const coral::Blob& blob = (*attrList).second["SpecialPixelMap"].data<const coral::Blob>();
                        delete (*spm)[idhash]; //May not be necessary?
                        (*spm)[idhash] = new ModuleSpecialPixelMap(blob, getChips(idhash) );
                    }
                    else {

                        const std::string& clob = (*attrList).second["ModuleSpecialPixelMap_Clob"].data<const std::string>();
                        delete (*spm)[idhash];//May not be necessary?
                        (*spm)[idhash] = new ModuleSpecialPixelMap(clob, getChips(idhash) );
                    }
                }
                else {
                    ATH_MSG_FATAL( "Module hashID out of range: " << idhash );
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    if( fillMissing ) {
        for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++) {
            if( (*spm)[i] == 0 ) {
                (*spm)[i] = new ModuleSpecialPixelMap();
                (*spm)[i]->setchipsPerModule(getChips(i));
            }
        }
    }
    cacheoutput = spm.release();

    return StatusCode::SUCCESS;
}


StatusCode SpecialPixelMapCondAlg::createMaskingOverlay(DetectorSpecialPixelMap*& cacheptr) const {

    auto spm = std::make_unique< DetectorSpecialPixelMap >();

    spm->resize(m_pixelID->wafer_hash_max());

    if(m_condAttrListCollectionKeys.size() == 0) {
        ATH_MSG_FATAL( "No database folder specified" );
        return StatusCode::FAILURE;
    }
    else if(m_condAttrListCollectionKeys.size() > 1) {
        ATH_MSG_INFO( "Multiple database folders specified" );
        ATH_MSG_INFO( "Trying to store special pixel map at "
                      << m_condAttrListCollectionKeys[0] );
    }
    int mk(0);
    for(unsigned int i = 0; i < m_pixelID->wafer_hash_max(); i++) {
        Identifier mID( m_pixelID->wafer_id(i) );
        (*spm)[i] = new ModuleSpecialPixelMap();
        (*spm)[i]->setchipsPerModule( getChips(i) );
        if (
            (m_pixelID->barrel_ec(mID) == 0 && (std::find(m_layersToMask.begin(), m_layersToMask.end(), m_pixelID->layer_disk(mID)) != m_layersToMask.end())) ||
            (m_pixelID->barrel_ec(mID) == 2  && (std::find(m_disksToMask.begin(), m_disksToMask.end(), (m_pixelID->layer_disk(mID) + 1)) != m_disksToMask.end())) ||
            (m_pixelID->barrel_ec(mID) == -2 && (std::find(m_disksToMask.begin(), m_disksToMask.end(), -1*(m_pixelID->layer_disk(mID) + 1)) != m_disksToMask.end()))
        ) {
            (*spm)[i]->setModuleStatus(1);
            ATH_MSG_INFO( "List of modules killed "<<mk<<" hash "<<i<<" ["<<m_pixelID->barrel_ec(mID)<<","<< m_pixelID->layer_disk(mID)<<","<<
                          m_pixelID->phi_module(mID)<<","<<m_pixelID->eta_module(mID)<<"]");
            ++mk;
        }
    }

    cacheptr = spm.release();

    return StatusCode::SUCCESS;
}



StatusCode SpecialPixelMapCondAlg::registerCondAttrListCollection(const DetectorSpecialPixelMap* spm,
        const DetectorSpecialPixelMap* referenceMap, const EventContext& ctx, const EventIDRange &r) const {

    coral::AttributeListSpecification* attrSpec = new coral::AttributeListSpecification();//memory leak?

    attrSpec->extend("moduleID", "int");

    if( m_writeBlobs ) {
        attrSpec->extend("SpecialPixelMap", "blob" );
    }
    else {
        attrSpec->extend("ModuleSpecialPixelMap_Clob", "string");
    }

    coral::AttributeListSpecification* attrSpecLong = new coral::AttributeListSpecification();//memory leak?
    attrSpecLong->extend("moduleID", "int");
    attrSpecLong->extend("SpecialPixelMap", "blob" );

    CondAttrListCollection* attrListColl = new CondAttrListCollection(true);
    CondAttrListCollection* attrListCollLong = 0;

    if( m_useDualFolderStructure ) {
        attrListCollLong = new CondAttrListCollection(true);
    }

    CondAttrListCollection::ChanNum chanNum = 0;

    for(DetectorSpecialPixelMap::const_iterator moduleMap = spm->begin(); moduleMap != spm->end(); ++moduleMap) {

        ++chanNum;

        if( m_differentialUpdates && (*moduleMap)->to_string() == (*referenceMap)[moduleMap - spm->begin()]->to_string() ) {
            continue;
        }

        coral::AttributeList attrList(*attrSpec);
        std::ostringstream attrStr;

        Identifier moduleID ( m_pixelID->wafer_id(moduleMap - spm->begin()) );

        unsigned int hashID;
        if(m_isIBL) {
            hashID = m_pixelID->wafer_hash(moduleID);
        }
        else {
            hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
                     ( m_pixelID->layer_disk(moduleID) << 23) +
                     ( m_pixelID->phi_module(moduleID) << 17) +
                     ( (m_pixelID->eta_module(moduleID) + 6) << 13);
        }

        if( m_writeBlobs ) {

            coral::Blob* blob = (*moduleMap)->to_blob();

            attrList["moduleID"].setValue( static_cast<int>( hashID ) );
            attrList["SpecialPixelMap"].setValue(*blob);

            if( msgLvl(MSG::DEBUG) ) {
                attrList.toOutputStream(attrStr);
            }
            ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );

            attrListColl->add(chanNum, attrList);

            delete blob;
        }
        else {

            std::string clob = (*moduleMap)->to_string();
            std::string referenceClob;

            if( m_useDualFolderStructure && m_differentialUpdates ) {
                referenceClob = (*referenceMap)[moduleMap - spm->begin()]->to_string();
            }

            if( m_useDualFolderStructure && clob.size() > 3999 ) {

                coral::AttributeList attrListLong(*attrSpecLong);

                coral::Blob* blob = (*moduleMap)->to_blob();

                attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
                attrListLong["SpecialPixelMap"].setValue(*blob);

                if( msgLvl(MSG::DEBUG) ) {
                    attrListLong.toOutputStream(attrStr);
                }
                ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );

                attrListCollLong->add(chanNum, attrListLong);

                delete blob;

                ModuleSpecialPixelMap moduleMap;
                clob = moduleMap.to_string();
            }
            else if( m_useDualFolderStructure && referenceClob.size() > 3999 ) {

                coral::AttributeList attrListLong(*attrSpecLong);

                ModuleSpecialPixelMap moduleMap;
                coral::Blob* blob = moduleMap.to_blob();

                attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
                attrListLong["SpecialPixelMap"].setValue(*blob);

                if( msgLvl(MSG::DEBUG) ) {
                    attrListLong.toOutputStream(attrStr);
                }
                ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );

                attrListCollLong->add(chanNum, attrListLong);

                delete blob;
            }

            attrList["moduleID"].setValue( static_cast<int>( hashID ) );
            attrList["ModuleSpecialPixelMap_Clob"].setValue( clob );

            if( msgLvl(MSG::DEBUG) ) {
                attrList.toOutputStream(attrStr);
            }
            ATH_MSG_DEBUG( "ChanNum " << chanNum << " Attribute list " << attrStr.str() );

            attrListColl->add(chanNum, attrList);
        }
    }

    if( m_useDualFolderStructure && attrListCollLong->size() == 0 ) {

        coral::AttributeList attrListLong(*attrSpecLong);

        coral::Blob* blob = 0;

        std::string referenceClob;

        if( m_differentialUpdates ) {
            referenceClob = (*referenceMap)[0]->to_string();
        }

        if( m_differentialUpdates && referenceClob.size() > 3999 ) {
            blob = (*referenceMap)[0]->to_blob();
        }
        else {
            ModuleSpecialPixelMap moduleMap;
            blob = moduleMap.to_blob();
        }

        Identifier moduleID ( m_pixelID->wafer_id(0) );

        unsigned int hashID;
        if(m_isIBL) {
            hashID = m_pixelID->wafer_hash(moduleID);
        }
        else {
            hashID = ( ((m_pixelID->barrel_ec(moduleID) + 2) / 2) << 25 ) +
                     ( m_pixelID->layer_disk(moduleID) << 23) +
                     ( m_pixelID->phi_module(moduleID) << 17) +
                     ( (m_pixelID->eta_module(moduleID) + 6) << 13);
        }
        attrListLong["moduleID"].setValue( static_cast<int>( hashID ) );
        attrListLong["SpecialPixelMap"].setValue(*blob);

        attrListCollLong->add(1, attrListLong);

        delete blob;
    }

    {
        SG::WriteCondHandle<CondAttrListCollection> output(m_outputFolder, ctx);
        ATH_MSG_DEBUG("Record condition handle " << m_outputFolder.key() << " " << r);
        ATH_CHECK(output.record(r, attrListColl));
    }
    if( m_useDualFolderStructure ) {
        SG::WriteCondHandle<CondAttrListCollection> outputlong(m_outputLongFolder, ctx);
        ATH_MSG_DEBUG("Record condition handle " << m_outputLongFolder.key() << " " << r);
        ATH_CHECK(outputlong.record(r, attrListCollLong));
    }

    return StatusCode::SUCCESS;
}


