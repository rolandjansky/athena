/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMDT_Cabling/MuonMDT_CablingAlg.h"
#include "SGTools/TransientAddress.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "PathResolver/PathResolver.h"
#include <fstream>
#include <string>
#include <stdlib.h>
#include "MuonCondSvc/MdtStringUtils.h"
#include <map>

MuonMDT_CablingAlg::MuonMDT_CablingAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator) {
  
}

StatusCode MuonMDT_CablingAlg::initialize(){

  ATH_MSG_DEBUG( "initialize " << name() );  
  ATH_CHECK(m_readKeyMez.initialize());
  ATH_CHECK(m_readKeyMap.initialize());
  ATH_CHECK(m_writeKey.initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonMDT_CablingAlg::execute(){
  
  ATH_MSG_DEBUG( "execute " << name() );  
  const EventContext& ctx = Gaudi::Hive::currentContext();
  // Write Cond Handle
  SG::WriteCondHandle<MuonMDT_CablingMap> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }
  std::unique_ptr<MuonMDT_CablingMap> writeCdo{std::make_unique<MuonMDT_CablingMap>()};
  
  EventIDRange rangeMez;
  ATH_CHECK(loadMezzanineSchema(ctx,rangeMez,*writeCdo));
  
  EventIDRange rangeMap;
  ATH_CHECK(loadCablingSchema(ctx,rangeMap,*writeCdo));
  

  // Create an intersection of input IOVs
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeMez, rangeMap);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }
  
  if ( writeHandle.record(rangeIntersection, std::move(writeCdo)).isFailure() ) {
    ATH_MSG_FATAL("Could not record MuonMDT_CablingMap " << writeHandle.key() 
		  << " with EventRange " << rangeIntersection << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");
  
  return StatusCode::SUCCESS;
}
StatusCode MuonMDT_CablingAlg::loadCablingSchema(const EventContext& ctx, EventIDRange& iov_range, MuonMDT_CablingMap& cabling_map) const{

  SG::ReadCondHandle<CondAttrListCollection> readHandleMap{ m_readKeyMap, ctx };
  const CondAttrListCollection* readCdoMap{*readHandleMap}; 
  if(!readCdoMap){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  }   
 
  if ( !readHandleMap.range(iov_range) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMap.key());
    return StatusCode::FAILURE;
  }  
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMap.fullKey() << " readCdoMap->size()= " << readCdoMap->size());
  ATH_MSG_INFO("Range of input is " << iov_range);
  
  ATH_MSG_VERBOSE( "Collection CondAttrListCollection CLID "
		   << readCdoMap->clID()  );

  
  // access to Map Schema Table to obtained the Map
  CondAttrListCollection::const_iterator itrMap;
  for (itrMap = readCdoMap->begin(); itrMap != readCdoMap->end(); ++itrMap) {
    const coral::AttributeList& atr=itrMap->second;
    
    CablingData map_data;
    if (!extractStationInfo(atr, map_data)) continue;

    constexpr char delimiter = ',';
    const std::string map=*(static_cast<const std::string*>((atr["Map"]).addressOfData()));
    std::vector<std::string> info_map; 
    MuonCalib::MdtStringUtils::tokenize(map,info_map,delimiter);       
    
    while (extractLayerInfo(info_map,map_data)) {
      /// now this mezzanine can be added to the map:
      ATH_MSG_DEBUG( "Adding new mezzanine "<<map_data );
      cabling_map.addMezzanine(map_data, msgStream());
    }
  } // end of CondAttrListCollection loop
  return StatusCode::SUCCESS;
}

bool MuonMDT_CablingAlg::extractStationInfo(const coral::AttributeList& atr, CablingData& map_data ) const {

    map_data.eta=*(static_cast<const int*>((atr["Eta"]).addressOfData()));   
    map_data.phi=*(static_cast<const int*>((atr["Phi"]).addressOfData()));
    map_data.mrod=*(static_cast<const int*>((atr["MROD"]).addressOfData()));
    map_data.csm=*(static_cast<const int*>((atr["CSM"]).addressOfData()));
    map_data.channelId=*(static_cast<const int*>((atr["Chan"]).addressOfData()));
    map_data.mezzanine_type=*(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
    
    const std::string chamber_name=*(static_cast<const std::string*>((atr["Chamber_Name"]).addressOfData()));
    const std::string subdetector_id=*(static_cast<const std::string*>((atr["SubDet_Id"]).addressOfData()));
   
  
    // convert the string name to index
    std::string stationNameString = chamber_name.substr(0,3);
    // fix for the BOE chambers, which in the offline are treated as BOL                                                                            
    if (stationNameString == "BOE") {
        stationNameString = "BOL";
    }
    map_data.stationIndex = m_idHelperSvc->mdtIdHelper().stationNameIndex(stationNameString);
    ATH_MSG_VERBOSE( "station name: " << stationNameString << " index: " << map_data.stationIndex  );
   // convert the subdetector id to integer
   map_data.subdetectorId = atoi(subdetector_id.c_str());
   const std::string tube_attr{"TubesPerLayer"};

   const int* layer_size = atr.exists(tube_attr) ? static_cast<const int*>((atr[tube_attr]).addressOfData()) : nullptr;
   if(layer_size){
     static std::atomic<bool> sInfoPrinted{false};
     if (!sInfoPrinted){
          sInfoPrinted = true;
          ATH_MSG_INFO("Additional information found in the database concerning the number of tubes per layer");
     }
      map_data.tubes_per_layer = (*layer_size);
      ATH_MSG_VERBOSE("Over write the tubes per layer to "<<map_data.tubes_per_layer);
   }
   ATH_MSG_DEBUG( "Data load is: /n" <<
                     "Chamber_Name = " << chamber_name << " eta= " <<  map_data.eta << "   Phi= " << map_data.phi 
		     << " sub_id = " <<subdetector_id << "  mrod = " << map_data.mrod << " csm = " << map_data.csm 
		     << "  chan= " << map_data.channelId << " mezzanine_type= " << map_data.mezzanine_type<< " FINISHED HERE " );


   if (map_data.stationIndex<0) {
      static std::atomic<bool> stWarningPrinted {false};
      if (!stWarningPrinted) {
        ATH_MSG_WARNING("Found stationIndex="<< map_data.stationIndex<<" which is not reasonable, maybe related to ATLASRECTS-5961, continuing...");
        stWarningPrinted = true;    
     }
   }   
   return  map_data.stationIndex >= 0;
}
bool MuonMDT_CablingAlg::extractLayerInfo(std::vector<std::string>& info_map, CablingData& map_data ) const{    
    ATH_MSG_VERBOSE( " parsing of the map"  );
    /// The map data is endcoded in repitive blocks of
    ///   <tdcId>,<channelId>,<tube_numbering>,<tdcId>,<channelId>,<tube_numbering>,...
    constexpr unsigned int coding_block_size = 3; 
    const unsigned int n = info_map.size() >= coding_block_size? coding_block_size: info_map.size();
    bool decoded_full_block{false};
    for(unsigned int i=0; i<n;++i){
      ATH_MSG_VERBOSE( i << "..."<< info_map[i] );
      int info = MuonCalib::MdtStringUtils::atoi(info_map[i]);
      // this is a tdcid
      if (i==0) {
	      map_data.tdcId = info;	
      }
      // this is a channel Id
      else if (i==1) {
	      map_data.channelId = info;
      }
      // this is a tube id that must be unpacked
      else if (i==2) {
        /// unpack the tube Id
        map_data.tube = info % map_data.tubes_per_layer;
	      map_data.layer = ((info-map_data.tube)/map_data.tubes_per_layer)%map_data.layer_block;
	      map_data.multilayer = (((info-map_data.tube)/map_data.tubes_per_layer)- map_data.layer) / map_data.layer_block ;
        decoded_full_block = true;
      }
  }

  // the stationIndex is later on passed to the MdtIdHelper, thus, it must be a reasonable station name, i.e. not < 0  
	if (decoded_full_block) info_map.erase(info_map.begin(),info_map.begin()+coding_block_size);
  return decoded_full_block;

}



StatusCode MuonMDT_CablingAlg::loadMezzanineSchema(const EventContext& ctx, EventIDRange& iov_range, MuonMDT_CablingMap& cabling_map) const{
  /// Read Cond Handle
  
  SG::ReadCondHandle<CondAttrListCollection> readHandleMez{ m_readKeyMez, ctx };
  const CondAttrListCollection* readCdoMez{*readHandleMez}; 
  if ( !readHandleMez.range(iov_range) ) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandleMez.key());
    return StatusCode::FAILURE;
  } 
  if(!readCdoMez){
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE; 
  }   
   
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandleMez.fullKey() << " readCdoMez->size()= " << readCdoMez->size());
  ATH_MSG_INFO("Range of input is " << iov_range);

  CondAttrListCollection::const_iterator itrMez;
  for (itrMez = readCdoMez->begin(); itrMez != readCdoMez->end(); ++itrMez) {
    const coral::AttributeList& atr=itrMez->second;
    int sequence{0}, layer{0}, mezzanine_type{0};

    mezzanine_type=*(static_cast<const int*>((atr["Mezzanine_Type"]).addressOfData()));
    layer=*(static_cast<const int*>((atr["Layer"]).addressOfData()));
    sequence=*(static_cast<const int*>((atr["Sequence"]).addressOfData()));    
    ATH_MSG_VERBOSE( "Sequence load is " << sequence << " for the mezzanine type =  "<< mezzanine_type<< " for the layer  number  = " <<layer  );        
    
    // here add the mezzanine type to the cabling class
    if (!cabling_map.addMezzanineLine(mezzanine_type, layer, sequence, msgStream())) {
      ATH_MSG_ERROR( "Could not add the mezzanine sequence to the map " );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_VERBOSE( "Sequence added successfully to the map"  );
    }    
  }
  return StatusCode::SUCCESS;
}
