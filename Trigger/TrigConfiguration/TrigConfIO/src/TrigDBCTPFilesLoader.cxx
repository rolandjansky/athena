// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#include "./TrigDBHelper.h"
#include "TrigConfIO/TrigDBCTPFilesLoader.h"
#include "TrigConfData/DataStructure.h"
#include <iterator>

TrigConf::TrigDBCTPFilesLoader::TrigDBCTPFilesLoader(const std::string & connection) : 
   TrigDBLoader("TrigDBCTPFilesLoader", connection)
{
   // CTP and MUCTPI hardware files only exist for schema versions >= 3

   { // links to file tables
      auto & q = m_link_queries[3];
      // tables
      q.addToTableList ( "SUPER_MASTER_TABLE", "SMT" );
      q.addToTableList ( "L1_MENU", "L1TM" );
      // bind vars
      q.extendBinding<unsigned int>("smk");
      // conditions
      q.extendCondition("SMT.SMT_ID = :smk AND SMT.SMT_L1_MENU_ID = L1TM.L1TM_ID");
      // attributes
      q.extendOutput<unsigned int>( "L1TM.L1TM_CTP_FILES_ID");
      q.extendOutput<unsigned int>( "L1TM.L1TM_CTP_SMX_ID");
      q.extendOutput<unsigned int>( "L1TM.L1TM_MUCTPI_FILES_ID");
      q.extendOutput<unsigned int>( "L1TM.L1TM_TMC_SIGNALS_ID");
   }

   { // ctp files
      auto & q = m_ctpfiles_queries[3];
      // tables
      q.addToTableList ( "L1_CTP_FILES" );
      // bind vars
      q.extendBinding<unsigned int>("id");
      // conditions
      q.extendCondition("L1CF_ID = :id");
      // attributes
      q.extendOutput<coral::Blob>( "L1CF_LUT" );
      q.extendOutput<coral::Blob>( "L1CF_CAM" );
      q.extendOutput<coral::Blob>( "L1CF_MON_SEL_SLOT7" );
      q.extendOutput<coral::Blob>( "L1CF_MON_SEL_SLOT8" );
      q.extendOutput<coral::Blob>( "L1CF_MON_SEL_SLOT9" );
      q.extendOutput<coral::Blob>( "L1CF_MON_SEL_CTPMON" );
      q.extendOutput<coral::Blob>( "L1CF_MON_DEC_SLOT7" );
      q.extendOutput<coral::Blob>( "L1CF_MON_DEC_SLOT8" );
      q.extendOutput<coral::Blob>( "L1CF_MON_DEC_SLOT9" );
      q.extendOutput<coral::Blob>( "L1CF_MON_DEC_CTPMON" );
      q.extendOutput<coral::Blob>( "L1CF_MON_DMX" );
   }

   {
      // ctp files >= v6
      // in version 6 this field (ctp core switchmatrix) was added
      auto q = m_ctpfiles_queries[3];
      q.extendOutput<coral::Blob>( "L1CF_SMX" );
      m_ctpfiles_queries[6] = q;
   }

   { // ctp smx
      auto & q = m_ctpsmx_queries[3];
      // tables
      q.addToTableList ( "L1_CTP_SMX" );
      // bind vars
      q.extendBinding<unsigned int>("id");
      // conditions
      q.extendCondition("L1SMX_ID = :id");
      // attributes
      q.extendOutput<coral::Blob>( "L1SMX_OUTPUT" );
      q.extendOutput<coral::Blob>( "L1SMX_VHDL_SLOT7" );
      q.extendOutput<coral::Blob>( "L1SMX_VHDL_SLOT8" );
      q.extendOutput<coral::Blob>( "L1SMX_VHDL_SLOT9" );
      q.extendOutput<coral::Blob>( "L1SMX_SVFI_SLOT7" );
      q.extendOutput<coral::Blob>( "L1SMX_SVFI_SLOT8" );
      q.extendOutput<coral::Blob>( "L1SMX_SVFI_SLOT9" );
   }

   { // muctpi
      auto & q = m_muctpi_queries[3];
      // tables
      q.addToTableList ( "L1_MUCTPI_FILES" );
      // bind vars
      q.extendBinding<unsigned int>("id");
      // conditions
      q.extendCondition("L1MF_ID = :id");
      // attributes
      q.extendOutput<coral::Blob>( "L1MF_DATA" );
   }

   { // tmc
      auto & q = m_tmcsig_queries[3];
      // tables
      q.addToTableList ( "L1_TMC_SIGNALS" );
      // bind vars
      q.extendBinding<unsigned int>("id");
      // conditions
      q.extendCondition("L1TMC_ID = :id");
      // attributes
      q.extendOutput<coral::Blob>( "L1TMC_DATA" );
   }
}

TrigConf::TrigDBCTPFilesLoader::~TrigDBCTPFilesLoader() = default;

bool
TrigConf::TrigDBCTPFilesLoader::loadHardwareFiles (unsigned int smk,
                                                   TrigConf::L1CTPFiles & ctpfiles,
                                                   uint8_t loadMask,
                                                   const std::string & /*outFileName*/ ) const
{
   unsigned int ctpFilesID{0};
   unsigned int ctpSmxID{0};
   unsigned int muctpiID{0};
   unsigned int tmcSignalsID{0};

   auto session = createDBSession();
   session->transaction().start( /*bool readonly=*/ true);
   const size_t sv = schemaVersion(session.get());
   try {
      QueryDefinition qdef = getQueryDefinition(sv, m_link_queries);
      qdef.setBoundValue<unsigned int>("smk", smk);
      auto q = qdef.createQuery( session.get() );
      auto & cursor = q->execute();
      if ( ! cursor.next() ) {
         TRG_MSG_ERROR("Tried reading L1 CTPFiles, but super master key " << smk << " is not available" );
         throw TrigConf::NoSMKException("TrigDBCTPFilesLoader: super master key " + std::to_string(smk) + " not available");
      }
      const coral::AttributeList& row = cursor.currentRow();
      try {
         ctpFilesID = row["L1TM.L1TM_CTP_FILES_ID"].data<unsigned int>();
      }
      catch(const coral::AttributeException & e) // NULL content
      {}
      try {
         ctpSmxID = row["L1TM.L1TM_CTP_FILES_ID"].data<unsigned int>();
      }
      catch(const coral::AttributeException & e) // NULL content
      {}
      try {
         muctpiID = row["L1TM.L1TM_CTP_FILES_ID"].data<unsigned int>();
      }
      catch(const coral::AttributeException & e) // NULL content
      {}
      try {
         tmcSignalsID = row["L1TM.L1TM_CTP_FILES_ID"].data<unsigned int>();
      }
      catch(const coral::AttributeException & e) // NULL content
      {}

      TRG_MSG_INFO("ID of table L1_CTP_FILES    : " <<  ctpFilesID);
      TRG_MSG_INFO("ID of table L1_CTP_SMX      : " <<  ctpSmxID);
      TRG_MSG_INFO("ID of table L1_MUCTPI_FILES : " <<  muctpiID);
      TRG_MSG_INFO("ID of table L1_TMC_SIGNALS  : " <<  tmcSignalsID);
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading the files table links for super master key " << smk << " a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBCTPFilesLoader: " + std::string(ex.what()));
   }

   if(ctpFilesID>0 && (loadMask & 0x01)!=0 ) {
      QueryDefinition qdef = getQueryDefinition(sv, m_ctpfiles_queries);
      qdef.setBoundValue<unsigned int>("id", ctpFilesID);
      loadCTPFiles(ctpfiles, qdef.createQuery( session.get() ), sv );
   }

   if(ctpSmxID>0 && (loadMask & 0x02)!=0) {
      QueryDefinition qdef = getQueryDefinition(sv, m_ctpsmx_queries);
      qdef.setBoundValue<unsigned int>("id", ctpSmxID);
      loadCTPSMX(ctpfiles, qdef.createQuery( session.get() ), sv );
   }
   
   if(tmcSignalsID>0 && (loadMask & 0x04)!=0) {
      QueryDefinition qdef = getQueryDefinition(sv, m_tmcsig_queries);
      qdef.setBoundValue<unsigned int>("id", tmcSignalsID);
      loadTMC(ctpfiles, qdef.createQuery( session.get() ), sv );
   }
   
   if(muctpiID>0 && (loadMask & 0x08)!=0) {
      QueryDefinition qdef = getQueryDefinition(sv, m_muctpi_queries);
      qdef.setBoundValue<unsigned int>("id", muctpiID);
      loadMUCTPI(ctpfiles, qdef.createQuery( session.get() ), sv );
   }   
   return true;
}


void
TrigConf::TrigDBCTPFilesLoader::loadCTPFiles(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query, size_t schemaVersion) const
{
   TRG_MSG_INFO("Loading data from table L1_CTP_FILES.");
   try
   {
      auto & cursor = query->execute();
      cursor.next();
      const coral::AttributeList& row = cursor.currentRow();
      {
         ctpfiles.set_Ctpcore_LUT(
            loadDBFieldIntoVector(row, "L1CF_LUT", L1CTPFiles::CTPCORE_LUT_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpcore_CAM(
            loadDBFieldIntoVector(row, "L1CF_CAM", L1CTPFiles::CTPCORE_CAM_SIZE)
         );
      }
      if(schemaVersion>=6) {
         ctpfiles.set_Ctpcore_SMX(
            loadDBFieldIntoVector(row, "L1CF_SMX", L1CTPFiles::CTPCORE_SMX_SIZE)
         );
      }

      {
         ctpfiles.set_Ctpin_MonSelector_Slot7(
            loadDBFieldIntoVector(row, "L1CF_MON_SEL_SLOT7", L1CTPFiles::CTPIN_MONSEL_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpin_MonSelector_Slot8(
            loadDBFieldIntoVector(row, "L1CF_MON_SEL_SLOT8", L1CTPFiles::CTPIN_MONSEL_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpin_MonSelector_Slot9(
            loadDBFieldIntoVector(row, "L1CF_MON_SEL_SLOT9", L1CTPFiles::CTPIN_MONSEL_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpin_MonDecoder_Slot7(
            loadDBFieldIntoVector(row, "L1CF_MON_DEC_SLOT7", L1CTPFiles::CTPIN_MONDEC_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpin_MonDecoder_Slot8(
            loadDBFieldIntoVector(row, "L1CF_MON_DEC_SLOT8", L1CTPFiles::CTPIN_MONDEC_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpin_MonDecoder_Slot9(
            loadDBFieldIntoVector(row, "L1CF_MON_DEC_SLOT9", L1CTPFiles::CTPIN_MONDEC_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpmon_MonSelector(
            loadDBFieldIntoVector(row, "L1CF_MON_SEL_CTPMON", L1CTPFiles::CTPMON_SELECTOR_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpmon_MonDecoder(
            loadDBFieldIntoVector(row, "L1CF_MON_DEC_CTPMON", L1CTPFiles::CTPMON_DECODER_SIZE)
         );
      }
      {
         ctpfiles.set_Ctpmon_DMX(
            loadDBFieldIntoVector(row, "L1CF_MON_DMX", L1CTPFiles::CTPMON_DMX_SIZE)
         );
      }
      ctpfiles.set_HasCompleteCtpData(true);
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading the L1CTPFiles a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBCTPFilesLoader: " + std::string(ex.what()));
   }
}

void
TrigConf::TrigDBCTPFilesLoader::loadCTPSMX(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query, size_t) const {
   TRG_MSG_INFO("Loading data from table L1_CTP_SMX");
   try
   {
      auto & cursor = query->execute();
      cursor.next();
      const coral::AttributeList& row = cursor.currentRow();
      
      ctpfiles.set_Smx_Output( loadDBFieldIntoString(row, "L1SMX_OUTPUT") );
      ctpfiles.set_Smx_Vhdl_Slot7( loadDBFieldIntoString(row, "L1SMX_VHDL_SLOT7") );
      ctpfiles.set_Smx_Vhdl_Slot8( loadDBFieldIntoString(row, "L1SMX_VHDL_SLOT8") );
      ctpfiles.set_Smx_Vhdl_Slot9( loadDBFieldIntoString(row, "L1SMX_VHDL_SLOT9") );
      ctpfiles.set_Smx_Svfi_Slot7( loadDBFieldIntoString(row, "L1SMX_SVFI_SLOT7") );
      ctpfiles.set_Smx_Svfi_Slot8( loadDBFieldIntoString(row, "L1SMX_SVFI_SLOT8") );
      ctpfiles.set_Smx_Svfi_Slot9( loadDBFieldIntoString(row, "L1SMX_SVFI_SLOT9") );
      ctpfiles.set_HasCompleteSmxData(true);      
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading the L1CTPFiles a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBCTPFilesLoader: " + std::string(ex.what()));
   }
}

void
TrigConf::TrigDBCTPFilesLoader::loadMUCTPI(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query, size_t) const {
   TRG_MSG_INFO("Loading data from table L1_MUCTPI_FILES");

   bool incomplete = false;
   boost::property_tree::ptree pt;
   try
   {
      auto & cursor = query->execute();
      cursor.next();
      const coral::AttributeList& row = cursor.currentRow();
      const coral::Blob & blob = row["L1MF_DATA"].data<coral::Blob>();
      blobToPtree(blob, pt);
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading the L1CTPFiles a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBCTPFilesLoader: " + std::string(ex.what()));
   }
   DataStructure ds("L1_MUCTPI_FILES", std::move(pt));
   std::vector<std::string> keys = ds.getKeys();
   for(auto k : {L1CTPFiles::RoiMaskA, L1CTPFiles::RoiMaskC, L1CTPFiles::PtLutBarrel, L1CTPFiles::PtLutEndcap}) {
      if( auto dv = ds.getList_optional(L1CTPFiles::s_keyMap.at(k)) ) {
         keys.erase( std::find(keys.begin(), keys.end(), L1CTPFiles::s_keyMap.at(k)) );
         std::vector<uint32_t> v;
         v.reserve(200);
         for( auto x : *dv ) {
            v.push_back(std::stoul(x.getValue<std::string>(), nullptr, 0));
         }
         ctpfiles.set_Muctpi(k, std::move(v));
      } else {
         incomplete = true;
      }
   }
   for(const std::string & k : keys) {
      if(ds.isNull(k)) {
         TRG_MSG_INFO("Attribute " << k << " has null-content");
         incomplete = true;
         continue;
      }
      auto sopt = ds.getAttribute_optional<std::string>(k);
      if(k.compare(0, 6, "pt_lut") == 0) {
         auto dv = ds.getList(k);
         std::vector<uint32_t> v;
         v.reserve(200);
         for( auto x : dv ) {
            v.push_back(std::stoul(x.getValue<std::string>(), nullptr, 0));
         }
         ctpfiles.set_Muctpi_Extra_Ptlut(k, std::move(v));
      } else if(k=="multiplicities_nbits") {
         auto dv = ds.getList(k);
         std::vector<uint32_t> v;
         for( auto x : dv ) {
            v.push_back(std::stoul(x.getValue<std::string>(), nullptr, 0));
         }
         ctpfiles.set_Muctpi_Nbits(std::move(v));
      }
   }
   ctpfiles.set_HasCompleteMuctpiData(!incomplete);
}

void
TrigConf::TrigDBCTPFilesLoader::loadTMC(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query, size_t) const {
   TRG_MSG_INFO("Loading data from table L1_TMC_SIGNALS");
   boost::property_tree::ptree pt;
   try
   {
      auto & cursor = query->execute();
      cursor.next();
      const coral::AttributeList& row = cursor.currentRow();
      const coral::Blob & blob = row["L1TMC_DATA"].data<coral::Blob>();
      blobToPtree(blob, pt);
   }
   catch(coral::QueryException & ex) {
      TRG_MSG_ERROR("When reading the L1CTPFiles a coral::QueryException was caught ( " << ex.what() <<" )" );
      throw TrigConf::QueryException("TrigDBCTPFilesLoader: " + std::string(ex.what()));
   }

   DataStructure ds("L1_TMC", std::move(pt));

   // should always be the correct type
   if( auto ft = ds.getAttribute<std::string>("filetype"); ft != "tmcresult" ) {
      throw TrigConf::ParsingException("TrigDBCTPFilesLoader::loadTMC: json structure of unexpected file type found. Expected 'tmcresult', but found " + ft);
   }

   // read the ctpcore inputs
   if (auto dv = ds.getObject_optional("CTPCORE.TriggerInputs"))
   {
      std::vector<TrigConf::L1CTPFiles::CTPCoreInput> ctpcoreInputs;
      for (const std::string &k : dv->getKeys())
      {
         const TrigConf::DataStructure &inp = dv->getObject(k);
         const std::string &inputType = inp["type"];
         TrigConf::L1CTPFiles::CTPCoreInput::InputType inpEnum = TrigConf::L1CTPFiles::CTPCoreInput::NONE;
         if (inputType == "PIT")
         {
            inpEnum = TrigConf::L1CTPFiles::CTPCoreInput::PIT;
         }
         else if (inputType == "DIR")
         {
            inpEnum = TrigConf::L1CTPFiles::CTPCoreInput::DIR;
         }
         ctpcoreInputs.push_back(
            TrigConf::L1CTPFiles::CTPCoreInput(
               inp.getAttribute<size_t>("number"), inp.getAttribute<std::string>("name"),
               inp.getAttribute<size_t>("bit"), inp.getAttribute<size_t>("phase"),
               inpEnum));
      }
      TRG_MSG_INFO("Loading ctpcore inputs " << ctpcoreInputs.size());
      ctpfiles.set_Tmc_CtpcoreInputs(std::move(ctpcoreInputs));
   }

   // read the ctpin map
   {
      std::map<std::string, size_t> ctpin;
      for (size_t slot : {7, 8, 9})
      {
         for (size_t conn : {0, 1, 2, 3})
         {
            std::string path = "CTPINs.SLOT" + std::to_string(slot) + ".Monitoring.Cables.CON" + std::to_string(conn);
            if (auto dv = ds.getObject_optional(path))
            {
               if (auto ov = dv->getList_optional("outputs"))
               {
                  for (const DataStructure &output : *ov)
                  {
                     size_t number = output.getAttribute<size_t>("number");
                     unsigned int mapping = number + 64 * conn + 256 * (slot - 7);
                     ctpin[output.getAttribute<std::string>("TriggerCounter")] = mapping;
                  }
               }
            }
         }
      }
      TRG_MSG_INFO("Loading ctpin counters " << ctpin.size());
      ctpfiles.set_Tmc_CtpinCounters(std::move(ctpin));
   }

   // read the ctpmon map
   if( auto dv = ds.getObject_optional("ctpmonMap") ) {
      std::map<std::string, size_t> ctpmon;
      for(const std::string & k : dv->getKeys()) {
         ctpmon[k] = dv->getAttribute<size_t>(k);
      }
      ctpfiles.set_Tmc_CtpmonCounters(std::move(ctpmon));
      TRG_MSG_INFO("Loading ctp mon counters " << ctpmon.size());
   }

   ctpfiles.set_Tmc_Data(std::move(ds));

   ctpfiles.set_HasCompleteTmcData(true);
}

std::vector<uint32_t>
TrigConf::TrigDBCTPFilesLoader::loadDBFieldIntoVector(const coral::AttributeList& row, const std::string& field, size_t size) const {
   std::vector<uint32_t> vec;
   if(size>0) {
      vec.reserve(size);
   }
   try {
      TRG_MSG_INFO("Loading " << field << " of size (#words) " << size);
      const coral::Blob& blob = row[field].data<coral::Blob>();
      boost::iostreams::stream<boost::iostreams::array_source> stream(
         static_cast<const char*> (blob.startingAddress()), 
         blob.size()
      );
      std::string word{""};
      while(stream >> word) {
         vec.push_back(std::stoul(word,nullptr, 0));
      }
   }
   catch(const coral::AttributeException & e) {} // NULL content
   // check the size if a positive size was requested
   if(size > 0 && size != vec.size()) {
      TRG_MSG_ERROR("File content from DB of size " << vec.size() << ", but expect " << size);
      throw std::runtime_error( "CTPFilesLoader: file of unexpected size" );
   }

   return vec;
}

std::string
TrigConf::TrigDBCTPFilesLoader::loadDBFieldIntoString(const coral::AttributeList& row, const std::string& field) const {
   std::string result;
   try {
      const coral::Blob& blob = row[field].data<coral::Blob>();
      boost::iostreams::stream<boost::iostreams::array_source> stream(
         static_cast<const char*> (blob.startingAddress()), 
         blob.size()
      );
      result = std::string (
         std::istreambuf_iterator<char>(stream.rdbuf()),
         std::istreambuf_iterator<char>()
      );
      TRG_MSG_INFO("Loading " << field << " of size " << result.size());
   }
   catch(const coral::AttributeException & e) {} // NULL content
   if(result.empty()) {
      TRG_MSG_ERROR("Field " << field << " in DB is empty");
      throw std::runtime_error( "CTPFilesLoader: field " + field + " in DB is empty" );
   }
   return result;
}
