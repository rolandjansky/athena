///////////////////////////////////////////////////////////////////
//
// HGTD_IdDictSvc.cxx                                                        
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
//
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometryXML/HGTD_IdDictSvc.h"
#include <boost/filesystem.hpp>
#include "TMath.h"

HGTD_IdDictSvc::HGTD_IdDictSvc(const std::string& name,ISvcLocator* svc) :
    AthService(name, svc),
    m_dictionaryFileName("HGTDTest.xml"),
    m_doHGTD(false),
    m_createDict(true)
{
    declareProperty("dictionaryFileName",    m_dictionaryFileName);
    declareProperty("doHGTD",                m_doHGTD);
    declareProperty("createXMLDictionary",   m_createDict);
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
    // file.open(filename, ios::app);
    file.open(filename);
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
  // Rows are counted clockwise from the front side of each disk

  std::vector < int > disk_front_modules_per_row = { 8, 10, 12, 14, 16, 18,
                                                    18, 18, 18, 18, 19, 17,
                                                    15, 13, 11,  9,  6,  5,
                                                     4,  3,  2};

  std::vector < int > disk_back_modules_per_row = {  7,  9, 11, 13, 15, 17,
                                                    18, 18, 18, 18, 19, 17,
                                                    15, 13, 10,  8,  6,  5,
                                                      4, 3,  1};

  // loop over front side rows
  for (unsigned int row = 0; row < disk_front_modules_per_row.size(); row++) {
    file << " <region group=\"hgtd\" >" << std::endl;
    file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
    file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
    file << "    <range field=\"hgtd_disk\" values=\"inner_disk outer_disk\" />" << std::endl;
    file << "    <range field=\"hgtd_side\" value=\"front_side\" /> " << std::endl;
    file << "    <range field=\"quadrant\" minvalue=\"0\" maxvalue=\"3\" wraparound=\"TRUE\" />" << std::endl;
    file << "    <range field=\"row\" value=\"" << row << "\" />" << std::endl;
    file << "    <range field=\"module\" minvalue=\"0\" maxvalue=\"" << disk_front_modules_per_row.at(row)-1 << "\" />" << std::endl;
    file << "    <range field=\"phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
    file << "    <range field=\"eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
    file << "  </region>" << std::endl;
  }

  // loop over back side rows
  for (unsigned int row = 0; row < disk_back_modules_per_row.size(); row++) {
    file << " <region group=\"hgtd\" >" << std::endl;
    file << "    <range field=\"part\" value=\"HGTD\" />" << std::endl;
    file << "    <range field=\"hgtd_endcap\" values=\"negative_endcap positive_endcap\" />" << std::endl;
    file << "    <range field=\"hgtd_disk\" values=\"inner_disk outer_disk\" />" << std::endl;
    file << "    <range field=\"hgtd_side\" value=\"back_side\" /> " << std::endl;
    file << "    <range field=\"quadrant\" minvalue=\"0\" maxvalue=\"3\" wraparound=\"TRUE\" />" << std::endl;
    file << "    <range field=\"row\" value=\"" << row << "\" />" << std::endl;
    file << "    <range field=\"module\" minvalue=\"0\" maxvalue=\"" << disk_back_modules_per_row.at(row)-1 << "\" />" << std::endl;
    file << "    <range field=\"phi_index\" minvalue=\"0\" maxvalue=\"14\" />" << std::endl;
    file << "    <range field=\"eta_index\" minvalue=\"0\" maxvalue=\"29\" />" << std::endl;
    file << "  </region>" << std::endl;
  }
  file << std::endl;
}
