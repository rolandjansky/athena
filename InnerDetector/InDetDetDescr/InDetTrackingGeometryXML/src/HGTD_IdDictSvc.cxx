///////////////////////////////////////////////////////////////////
//
// HGTD_IdDictSvc.cxx                                                        
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
//
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometryXML/HGTD_IdDictSvc.h"

#include <boost/filesystem.hpp>
#include "TMath.h"

using namespace std;

HGTD_IdDictSvc::HGTD_IdDictSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name, svc),
  m_dictionaryFileName("HGTDTest.xml"),
  m_doHGTD(false),
  m_createDict(true),
  m_geomVersion(1) {
    declareProperty("dictionaryFileName",    m_dictionaryFileName);
    declareProperty("doHGTD",                m_doHGTD);
    declareProperty("createXMLDictionary",   m_createDict);
    declareProperty("GeoVersion",   m_geomVersion );
}

HGTD_IdDictSvc::~HGTD_IdDictSvc()
{
}

// Query the interface
StatusCode HGTD_IdDictSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_HGTD_IdDictSvc == riid )
    *ppvInterface = (HGTD_IdDictSvc *) this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode HGTD_IdDictSvc::initialize()
{
    // Dump XML dictionary
    if(m_createDict) {
        writeDictionary(m_dictionaryFileName);    
    }

    return StatusCode::SUCCESS;
}

StatusCode HGTD_IdDictSvc::finalize()
{
  return StatusCode::SUCCESS;
}

void HGTD_IdDictSvc::writeDictionary(std::string filename)
{
    std::ofstream file;
    // look for the directory
    std::size_t found = filename.find("/");
    if (found!=std::string::npos) {
        std::string folder = filename.substr(0, found);
        boost::filesystem::path dir(folder);
        if(!(boost::filesystem::exists(dir))) {
            ATH_MSG_INFO("Directory " << folder << " doesn't exist... Creating it!");
            if (boost::filesystem::create_directory(dir))
                ATH_MSG_INFO("Directory " << folder << " created!");
        }
    }
    openDictFile(file,filename);
    if (m_doHGTD) {
        writeHgtdDict(file);
    }
    closeDictFile(file);
}

void HGTD_IdDictSvc::openDictFile(std::ofstream& file,std::string filename) const
{
    file.open(filename, ios::app);
    ATH_MSG_DEBUG("HGTD_IdDictSvc::openDictFile ");
}

void HGTD_IdDictSvc::closeDictFile(std::ofstream& file) const
{
  if (m_doHGTD)
    file << "</IdDictionary>" << std::endl;
  file.close();
}

void HGTD_IdDictSvc::writeHgtdDict(std::ofstream& file)
{
  // For now, hardcode number of rows and modules per row - later, possibly take them from a DB
  //  2-ring implementation
  std::vector < int > layers_0_2_phi_modules_per_eta = { 18, 19, 19, 19, 19, 18, 17, 15, 14, 13, 12, 11,  9,  8, 5, 2,
                                                         19, 18, 18, 19, 19, 19, 19, 18, 17, 15, 14, 13, 12, 11, 9, 8, 5, 2,
                                                         19, 18, 18, 19, 19, 19, 19, 18, 17, 15, 14, 13, 12, 11, 9, 8, 5, 2,
                                                         19, 18, 18, 19, 19, 19, 19, 18, 17, 15, 14, 13, 12, 11, 9, 8, 5, 2,
                                                         19, 18
                                                        };

  std::vector < int > layers_1_3_phi_modules_per_eta = { 18, 18, 19, 19, 18, 17, 16, 15, 13, 12, 11, 10,  9,  7, 5, 1,
                                                         18, 18, 18, 18, 19, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 5, 1,
                                                         18, 18, 18, 18, 19, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 5, 1,
                                                         18, 18, 18, 18, 19, 19, 18, 17, 16, 15, 13, 12, 11, 10, 9, 7, 5, 1,
                                                         18, 18
                                                        };

  // 3-ring implementation
  // Rows are counted clockwise from the front side of each disk
  //  ANTI clockwise rows from default 3-ring layout ( Jan 31, 2021 )
  //  https://cernbox.cern.ch/index.php/s/PPXEWSBnBjwI7UU
  unsigned int numRows = 21;
  std::vector < int > disk_0_front = { 18, 18, 17, 16, 13, 13, 11,
                                       1, 3, 4, 6, 6, 8, 8, 
                                       10, 11, 15, 17, 19, 18, 18 };   
  std::vector < int > disk_0_back = {  18, 18, 18, 15, 14, 13, 11, 
                                        1, 3, 5, 5, 7,  7, 9, 
                                        9, 12, 15, 17, 19, 18, 18 }; 
  //  mirrored and flipped
  std::vector < int > disk_1_front, disk_1_back;
  for ( unsigned int r = 0; r < numRows; r++ ) 
  {
    disk_1_front.push_back( disk_0_back[ numRows - 1 - r ] );
    disk_1_back.push_back( disk_0_front[ numRows - 1 - r ] );
  }
  std::array< std::vector< int >, 4 > modules_per_row_allLayers =  { disk_0_front, disk_0_back, 
                                                                     disk_1_front, disk_1_back };
  if ( m_geomVersion == 0 ) // 2-ring 
  {
    // loop over eta bins for layer 0
    for (unsigned int eta = 0; eta < layers_0_2_phi_modules_per_eta.size(); eta++) {
      file << "  <region group=\"hgtd\" >" << std::endl;
      file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
      file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"hgtd_layer\" value=\"0\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_module\" minvalue=\"0\" maxvalue=\"" << layers_0_2_phi_modules_per_eta.at(eta)-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_module\" value=\"" << eta << "\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
      file << "  </region>" << std::endl;
    }

    // loop over eta bins for layer 1
    for (unsigned int eta = 0; eta < layers_1_3_phi_modules_per_eta.size(); eta++) {
      file << "  <region group=\"hgtd\" >" << std::endl;
      file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
      file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"hgtd_layer\" value=\"1\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_module\" minvalue=\"0\" maxvalue=\"" << layers_1_3_phi_modules_per_eta.at(eta)-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_module\" value=\"" << eta << "\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
      file << "  </region>" << std::endl;
    }

    // loop over eta bins for layer 2
    for (unsigned int eta = 0; eta < layers_0_2_phi_modules_per_eta.size(); eta++) {
      file << "  <region group=\"hgtd\" >" << std::endl;
      file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
      file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"hgtd_layer\" value=\"2\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_module\" minvalue=\"0\" maxvalue=\"" << layers_0_2_phi_modules_per_eta.at(eta)-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_module\" value=\"" << eta << "\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
      file << "  </region>" << std::endl;
    }

    // loop over eta bins for layer 3
    for (unsigned int eta = 0; eta < layers_1_3_phi_modules_per_eta.size(); eta++) {
      file << "  <region group=\"hgtd\" >" << std::endl;
      file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
      file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
      file << "    <range field=\"hgtd_layer\" value=\"3\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_module\" minvalue=\"0\" maxvalue=\"" << layers_1_3_phi_modules_per_eta.at(eta)-1 << "\" wraparound=\"TRUE\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_module\" value=\"" << eta << "\" />" << std::endl;
      file << "    <range field=\"hgtd_phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
      file << "    <range field=\"hgtd_eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
      file << "  </region>" << std::endl;
    }
  } 
  //  3-ring
  else {
    // following the naming of range-fields from 2-ring even though they seem illed.
    // loop over endCaps 
    for ( int ec = 0; ec < 2; ec++ ) {
      std::string ecStr = ec > 0 ?  "value=\"positive_endcap" : "value=\"negative_endcap";
                              
      // loop over  layers
      for ( int layer = 0; layer < 4; layer++ ) {
	// loop over quadrants
	for ( int qdr = 0; qdr < 4; qdr++ ) {
	  std::vector<int> modules_per_row = modules_per_row_allLayers[ layer ];
	  // loop over numRows per quadrant 
	  for (unsigned int row = 0; row < modules_per_row.size(); row++) {
	    file << "  <region group=\"hgtd\" >" << std::endl;
	    file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
	    file << "    <range field=\"hgtd_endcap\" " << ecStr << "\" />" << std::endl;
	    file << "    <range field=\"hgtd_layer\" value=\"" << layer << "\" />" << std::endl;

	    int row_idfr = 21*qdr + row + 1;
	    file << "    <range field=\"hgtd_phi_module\" value=\"" << row_idfr << "\" />" << std::endl;
	    
	    if ( modules_per_row.at(row) > 1 )  
	      file << "    <range field=\"hgtd_eta_module\" minvalue=\"1\" maxvalue=\"" 
		   << modules_per_row.at(row) << "\" />" << std::endl;
	    else 
	      file << "    <range field=\"hgtd_eta_module\" value=\"" << modules_per_row.at(row) << "\" />" << std::endl;
	    
	    // following the strange specification from 2-ring
	    file << "    <range field=\"hgtd_phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
	    file << "    <range field=\"hgtd_eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
	    file << "  </region>" << std::endl;
	  } 
	}
      }
    }     
  }
  file << std::endl;
}
