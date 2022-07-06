/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* file contains the implementation for the AthenaHDFStreamTool class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include "AthenaHDFStreamTool.h"

#include "GaudiKernel/FileIncident.h"

#include "StorageSvc/DbReflex.h"

#include "H5Cpp.h"
#include "H5File.h"
#include "H5Group.h"

static const char* const fmt_oid = "[OID=%08lX%08lX-%016llX]";
static const char* const fmt_aux = "[AUX=%08lX]";

//___________________________________________________________________________
AthenaHDFStreamTool::AthenaHDFStreamTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : AthAlgTool(type, name, parent),
		m_file(nullptr),
		m_group(nullptr),
		m_token(""),
		m_read_data(nullptr),
		m_read_size(0),
		m_read_position(0),
		m_event_iter(0),
		m_isClient(false),
		m_incidentSvc("IncidentSvc", name) {
   declareInterface<IAthenaIPCTool>(this);
}

//___________________________________________________________________________
AthenaHDFStreamTool::~AthenaHDFStreamTool() {
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::initialize() {
   if (!::AthAlgTool::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthAlgTool base class.");
      return(StatusCode::FAILURE);
   }
   // Retrieve IncidentSvc
   if (!m_incidentSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get IncidentSvc");
      return(StatusCode::FAILURE);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::finalize() {
   ATH_MSG_INFO("in finalize()");
   return(::AthAlgTool::finalize());
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::makeServer(int/* num*/, const std::string& /*streamPortSuffix*/) {
   ATH_MSG_ERROR("AthenaHDFStreamTool::makeServer");
   return(StatusCode::FAILURE);
}

//___________________________________________________________________________
bool AthenaHDFStreamTool::isServer() const {
   return(false);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::makeClient(int num, std::string& /*streamPortSuffix*/) {
   ATH_MSG_INFO("AthenaHDFStreamTool::makeClient: " << num);

   if (num > 0) {
      m_file = new H5::H5File( "test.h5", H5F_ACC_TRUNC ); //FIXME, hardcoded filename
      m_group = new H5::Group(m_file->createGroup("data"));
   } else {
      m_file = new H5::H5File( "test.h5", H5F_ACC_RDONLY );
      m_group = new H5::Group(m_file->openGroup("data"));
   }
   m_isClient = true;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
bool AthenaHDFStreamTool::isClient() const {
   return(m_isClient);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::putEvent(long/* eventNumber*/, const void* /* source*/, std::size_t/* nbytes*/, unsigned int/* status*/) const {
   ATH_MSG_ERROR("AthenaHDFStreamTool::putEvent");
   return(StatusCode::FAILURE);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::getLockedEvent(void** target, unsigned int&/* status*/) const {
   ATH_MSG_INFO("AthenaHDFStreamTool::getLockedEvent");
   const std::string dh_entry = "POOLContainer(DataHeader)_entry";
   H5::DataSet dataset = m_group->openDataSet(dh_entry);
   if (m_event_iter + 1 >= dataset.getInMemDataSize()/8) { // End of File
      FileIncident endFileIncident(name(), "EndInputFile", "HDF:test.h5"); //FIXME, hardcoded filename
      m_incidentSvc->fireIncident(endFileIncident);
      ATH_MSG_INFO("AthenaHDFStreamTool::getLockedEvent: no more events = " << m_event_iter);
      return(StatusCode::FAILURE);
   }

   const hsize_t offset[1] = {m_event_iter};
   H5::DataSpace filespace = dataset.getSpace();
   const hsize_t mem_size[1] = {2};
   filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
   H5::DataSpace memspace(1, mem_size);
   long long unsigned int ds_data[2] = {0, 0};
   dataset.read(ds_data, H5::PredType::NATIVE_ULLONG, memspace, filespace);
   std::size_t nbytes = ds_data[1] - ds_data[0];
   m_token = "[DB=00000000-0000-0000-0000-000000000000][CNT=POOLContainer(DataHeader)][CLID=4DDBD295-EFCE-472A-9EC8-15CD35A9EB8D][TECH=00000401]";
   char text[64];
   sprintf(text, fmt_oid, 0ul, nbytes, m_event_iter);
   text[40] = 0;
   m_token += text;

   const int length = m_token.size() + 2; //FIXME: copy token
   *target = new char[length];
   std::memcpy(static_cast<char*>(*target), m_token.c_str(), length - 1);
   target[length - 1] = 0;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::lockEvent(long eventNumber) const {
   ATH_MSG_VERBOSE("AthenaHDFStreamTool::lockEvent: " << eventNumber);
   m_event_iter = eventNumber;
   if (eventNumber == 0) {
      FileIncident beginFileIncident(name(), "BeginInputFile", "HDF:test.h5"); //FIXME, hardcoded filename
      m_incidentSvc->fireIncident(beginFileIncident);
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::putObject(const void* source, std::size_t nbytes, int/* num*/) const {
   if (nbytes == 0 || m_token.empty()) {
      return(StatusCode::SUCCESS);
   }
   ATH_MSG_INFO("AthenaHDFStreamTool::putObject: source = " << source << ", nbytes = " << nbytes);

   if (m_token.find("[CONT=") != std::string::npos) m_token.replace(m_token.find("[CONT="), 6, "[CNT=");
   std::string ds_name = m_token.substr(m_token.find("[CNT=") + 5);
   ds_name = ds_name.substr(0, ds_name.find("]"));
   while (ds_name.find("/") != std::string::npos) { ds_name = ds_name.replace(ds_name.find("/"), 1, "_"); }

   m_token.replace(m_token.find("[TECH="), 15, "[TECH=00000401]");
   std::string className = m_token.substr(m_token.find("[PNAME=") + 7);
   className = className.substr(0, className.find(']'));

   long long unsigned int positionCount = 0;
   if (m_token.find("[CLID=") == std::string::npos) { // Core object
      m_token += "[CLID=" + pool::DbReflex::guid(RootType::ByNameNoQuiet(className)).toString() + "]";
   } else { // Aux Store extension
      char text[64];
      sprintf(text, fmt_aux, nbytes);
      text[15] = 0;
      H5::DataSet dataset = m_group->openDataSet(ds_name);
      const hsize_t offset[1] = {dataset.getInMemDataSize()};
      positionCount = offset[0];
      const hsize_t ds_size[1] = {offset[0] + 15};
      dataset.extend(ds_size);
      H5::DataSpace filespace = dataset.getSpace();
      const hsize_t mem_size[1] = {15};
      filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
      H5::DataSpace memspace(1, mem_size);
      dataset.write(text, H5::PredType::NATIVE_CHAR, memspace, filespace);
   }

// Write Payload data
   if (!m_group->exists(ds_name)) { //if dataset doesn't exist, create it otherwise extend it
      const hsize_t maxdim[1] = {H5S_UNLIMITED};
      const hsize_t ds_size[1] = {nbytes};
      H5::DataSpace filespace(1, ds_size, maxdim);
      H5::DSetCreatPropList ds_prop;
      hsize_t chunkdim[1] = {nbytes};
      if (ds_name.substr(0, 14) == "CollectionTree" || ds_name.substr(0, 18) == "POOLCollectionTree") {
         if (nbytes < 512) {
            chunkdim[0] = 4096;
         } else if (nbytes < 16 * 512) {
            chunkdim[0] = 4 * 4096;
         } else {
            chunkdim[0] = (int(nbytes / 4096) + 1) * 4096;
         }
      }
      ds_prop.setChunk(1, chunkdim);
      char fill_val[1] = {0};
      ds_prop.setFillValue(H5::PredType::NATIVE_CHAR, fill_val);
      H5::DataSet dataset = m_group->createDataSet(ds_name, H5::PredType::NATIVE_CHAR, filespace, ds_prop);
      dataset.write(source, H5::PredType::NATIVE_CHAR);
   } else {
      H5::DataSet dataset = m_group->openDataSet(ds_name);
      const hsize_t offset[1] = {dataset.getInMemDataSize()};
      positionCount = offset[0];
      const hsize_t ds_size[1] = {offset[0] + nbytes};
      dataset.extend(ds_size);
      H5::DataSpace filespace = dataset.getSpace();
      const hsize_t mem_size[1] = {nbytes};
      filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
      H5::DataSpace memspace(1, mem_size);
      dataset.write(source, H5::PredType::NATIVE_CHAR, memspace, filespace);
   }
   if (m_token.find("[OID=") == std::string::npos) { // Core object
      char text[64];
      sprintf(text, fmt_oid, 0ul, nbytes, positionCount);
      text[40] = 0;
      m_token += text;
   } else {
      char text[64];
      std::size_t firstU, firstL;
      long long unsigned int second;
      ::sscanf(m_token.substr(m_token.find("[OID="), 40).c_str(), fmt_oid, &firstU, &firstL, &second);
      if (firstU == 0ul) { //FIXME1
         firstU = firstL; // Keep Core object size
      }
      firstL = positionCount + nbytes - second;
      sprintf(text, fmt_oid, firstU, firstL, second); // FIXME
      text[40] = 0;
      m_token.replace(m_token.find("[OID="), 39, text);
   }

   std::string entry_name = ds_name.substr(ds_name.find("(") + 1);
   entry_name = entry_name.substr(0, entry_name.find(")"));
// For DataHeader, store entry point
   if (entry_name == "DataHeader" || entry_name == "DataHeaderForm") {
      auto dh_entry = ds_name + "_entry";
      if (!m_group->exists(dh_entry)) {
         const hsize_t maxdim[1] = {H5S_UNLIMITED};
         const hsize_t ds_size[1] = {2};
         H5::DataSpace filespace(1, ds_size, maxdim);
         H5::DSetCreatPropList ds_prop;
         const hsize_t chunkdim[1] = {512};
         ds_prop.setChunk(1, chunkdim);
         char fill_val[1] = {0};
         ds_prop.setFillValue(H5::PredType::NATIVE_ULLONG, fill_val);
         H5::DataSet dataset = m_group->createDataSet(dh_entry, H5::PredType::NATIVE_ULLONG, filespace, ds_prop);
         long long unsigned int ds_data[2] = {positionCount, positionCount + nbytes};
         dataset.write(ds_data, H5::PredType::NATIVE_ULLONG);
      } else {
         H5::DataSet dataset = m_group->openDataSet(dh_entry);
         const hsize_t offset[1] = {dataset.getInMemDataSize()/8};//FIXME
         const hsize_t ds_size[1] = {offset[0] + 1};
         dataset.extend(ds_size);
         H5::DataSpace filespace = dataset.getSpace();
         const hsize_t mem_size[1] = {1};
         filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
         H5::DataSpace memspace(1, mem_size);
         long long unsigned int ds_data[1] = {positionCount + nbytes};
         dataset.write(ds_data, H5::PredType::NATIVE_ULLONG, memspace, filespace);
      }
      if (entry_name == "DataHeader") {
         auto dh_form_entry = ds_name + "_form_entry";
         if (!m_group->exists(dh_form_entry)) {
            const hsize_t maxdim[1] = {H5S_UNLIMITED};
            const hsize_t ds_size[1] = {1};
            H5::DataSpace filespace(1, ds_size, maxdim);
            H5::DSetCreatPropList ds_prop;
            const hsize_t chunkdim[1] = {512};
            ds_prop.setChunk(1, chunkdim);
            char fill_val[1] = {0};
            ds_prop.setFillValue(H5::PredType::NATIVE_ULLONG, fill_val);
            H5::DataSet dataset = m_group->createDataSet(dh_form_entry, H5::PredType::NATIVE_ULLONG, filespace, ds_prop);
            long long unsigned int ds_data[1] = {0};
            dataset.write(ds_data, H5::PredType::NATIVE_ULLONG);
         } else {
            H5::DataSet dataset = m_group->openDataSet(dh_form_entry);
            const hsize_t offset[1] = {dataset.getInMemDataSize()/8};//FIXME
            const hsize_t ds_size[1] = {offset[0] + 1};
            dataset.extend(ds_size);
            H5::DataSpace filespace = dataset.getSpace();
            const hsize_t mem_size[1] = {1};
            filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
            H5::DataSpace memspace(1, mem_size);
            auto dh_form_entry_name = ds_name.substr(0, ds_name.find("(")) + "Form(DataHeaderForm)_entry";
            H5::DataSet dh_form_dataset = m_group->openDataSet(dh_form_entry_name);
            long long unsigned int ds_data[1] = {dh_form_dataset.getInMemDataSize()/8 - 1};//FIXME
            dataset.write(ds_data, H5::PredType::NATIVE_ULLONG, memspace, filespace);
         }
      }
      if (entry_name == "DataHeaderForm") {
         auto dh_entry_name = ds_name.substr(0, ds_name.find("(") - 4) + "(DataHeader)_form_entry";
            H5::DataSet dataset = m_group->openDataSet(dh_entry_name);
            const hsize_t offset[1] = {dataset.getInMemDataSize()/8 - 1};//FIXME
            H5::DataSpace filespace = dataset.getSpace();
            const hsize_t mem_size[1] = {1};
            filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
            H5::DataSpace memspace(1, mem_size);
            H5::DataSet dh_form_dataset = m_group->openDataSet(ds_name + "_entry");
            long long unsigned int ds_data[1] = {dh_form_dataset.getInMemDataSize()/8 - 1};//FIXME
            dataset.write(ds_data, H5::PredType::NATIVE_ULLONG, memspace, filespace);
      }
   }
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::getObject(void** target, std::size_t& nbytes, int/* num*/) const {
   if (m_token.empty()) {
      return(StatusCode::SUCCESS);
   }
   ATH_MSG_INFO("AthenaHDFStreamTool::getObject: token  = " << m_token);

   std::string clid_name = m_token.substr(m_token.find("[CLID=") + 6);
   clid_name = clid_name.substr(0, clid_name.find("]"));
   std::string ds_name = m_token.substr(m_token.find("[CNT=") + 5);
   ds_name = ds_name.substr(0, ds_name.find("]"));
   if (ds_name.empty()) {
      return(StatusCode::SUCCESS);
   }
   while (ds_name.find("/") != std::string::npos) { ds_name = ds_name.replace(ds_name.find("/"), 1, "_"); }

   std::string oid_name = m_token.substr(m_token.find("[OID="));
   oid_name = oid_name.substr(0, oid_name.find("]") + 1);
   std::size_t firstU, firstL;
   long long unsigned int second;
   ::sscanf(oid_name.c_str(), fmt_oid, &firstU, &firstL, &second);
   if (m_read_size > m_read_position + 15) { // aux store data already read
      std::size_t aux_size = 0;
      ::sscanf(m_read_data + m_read_position, fmt_aux, &aux_size);
      m_read_position += 15;
      *target = m_read_data + m_read_position;
      nbytes = aux_size;
      m_read_position += nbytes;
      char text[64];
      sprintf(text, fmt_oid, m_read_position, firstL, second); // FIXME
      text[40] = 0;
      m_token.replace(m_token.find("[OID="), 39, text);
      return(StatusCode::SUCCESS);
   } else if (m_read_size > 0) {
      return(StatusCode::FAILURE);
   }

   std::string entry_name = ds_name.substr(ds_name.find("(") + 1);
   entry_name = entry_name.substr(0, entry_name.find(")"));
// For DataHeader, get stored size
   if (entry_name == "DataHeader") {
      if (clid_name == "7BE56CEF-C866-4BEE-9348-A5F34B5F1DAD") { // DataHeaderForm Token is copied from DataHeader, change container name
         ds_name.replace(ds_name.find("(DataHeader)"), 12, "Form(DataHeaderForm)");
         second = m_event_iter; //FIXME, store real DHF id somewhere...
      } else if (clid_name == "00000000-0000-0000-0000-000000000000") { // Return DataHeader Token, for createAddress
         if (firstL > 0) { //FIXME1
            m_token.clear();
         } else {
            m_token.replace(m_token.find("[CLID="), 43, "[CLID=4DDBD295-EFCE-472A-9EC8-15CD35A9EB8D]");
            m_token.replace(m_token.find("[TECH="), 15, "[TECH=00000401]");
         }
         nbytes = m_token.size();
         *target = const_cast<char*>(m_token.c_str());//FIXME
         return(StatusCode::SUCCESS);
      } else {
         m_event_iter = second;
      }

      auto dh_entry = ds_name + "_entry";
      H5::DataSet dataset = m_group->openDataSet(dh_entry);
      if (second + 1 >= dataset.getInMemDataSize()/8) {
         return(StatusCode::FAILURE);
      }
      const hsize_t offset[1] = {second};
      H5::DataSpace filespace = dataset.getSpace();
      const hsize_t mem_size[1] = {2};
      filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
      H5::DataSpace memspace(1, mem_size);
      long long unsigned int ds_data[2] = {0, 0};
      dataset.read(ds_data, H5::PredType::NATIVE_ULLONG, memspace, filespace);
      firstL = ds_data[1] - ds_data[0]; //FIXME1
      second = ds_data[0];
   }

   if (!m_group->exists(ds_name)) {
      return(StatusCode::FAILURE);
   }
   H5::DataSet dataset = m_group->openDataSet(ds_name);
   if (second + firstL > dataset.getInMemDataSize()) {
      return(StatusCode::FAILURE);
   }
   const hsize_t offset[1] = {second};
   H5::DataSpace filespace = dataset.getSpace();
   const hsize_t mem_size[1] = {firstL};
   filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
   H5::DataSpace memspace(1, mem_size);
   m_read_data = new char[firstL];
   m_read_size = firstL;
   dataset.read(m_read_data, H5::PredType::NATIVE_CHAR, memspace, filespace);
   *target = m_read_data;
   if (firstU > 0) {
      ::sscanf(m_read_data + firstL - 15, fmt_aux, &nbytes);
   } else {
      nbytes = firstL;
   }
   m_read_position = nbytes;
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::clearObject(const char** tokenString, int&/* num*/) const {
   std::size_t firstU, firstL;
   long long unsigned int second;
   ::sscanf(m_token.substr(m_token.find("[OID="), 40).c_str(), fmt_oid, &firstU, &firstL, &second);
   std::string ds_name = m_token.substr(m_token.find("[CNT=") + 5);
   ds_name = ds_name.substr(0, ds_name.find("]"));
   while (ds_name.find("/") != std::string::npos) { ds_name = ds_name.replace(ds_name.find("/"), 1, "_"); }

   if (firstU > 0 || ds_name.substr(ds_name.length() - 5, 4) == "Aux.") {
      if (firstU == 0) firstU = firstL;
      char text[64];
      sprintf(text, fmt_aux, firstU);
      text[15] = 0;
      H5::DataSet dataset = m_group->openDataSet(ds_name);
      const hsize_t offset[1] = {dataset.getInMemDataSize()};
      const hsize_t ds_size[1] = {offset[0] + 15};
      dataset.extend(ds_size);
      H5::DataSpace filespace = dataset.getSpace();
      const hsize_t mem_size[1] = {15};
      filespace.selectHyperslab(H5S_SELECT_SET, mem_size, offset);
      H5::DataSpace memspace(1, mem_size);
      dataset.write(text, H5::PredType::NATIVE_CHAR, memspace, filespace);
      firstL += 15;
      firstU = 1ul;
      sprintf(text, fmt_oid, firstU, firstL, second); // FIXME
      text[40] = 0;
      m_token.replace(m_token.find("[OID="), 39, text);
   }
   // Return an empty token string for DataHeaderForm, to indicate HDF5 can't update DataHeader after it was written.
   std::string entry_name = ds_name.substr(ds_name.find("(") + 1);
   entry_name = entry_name.substr(0, entry_name.find(")"));
   if (entry_name == "DataHeaderForm") {
      m_token.clear();
   }
   *tokenString = m_token.c_str();
   return(StatusCode::SUCCESS);
}

//___________________________________________________________________________
StatusCode AthenaHDFStreamTool::lockObject(const char* tokenString, int/* num*/) const {
   m_token = tokenString;
   delete [] m_read_data; m_read_data = nullptr;
   m_read_size = 0;
   m_read_position = 0;
   return(StatusCode::SUCCESS);
}
