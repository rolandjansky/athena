/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "TRRegionXMLHandler.h"

// package includes
#include "TRTRadiatorParameters.h"
#include "TRTTransitionRadiation.h"

// Athena includes
#include "IdDictDetDescr/IdDictManager.h"

// Gaudi includes
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

// Geant4 includes
#include "G4LogicalVolumeStore.hh"// For logical volume setup

// For XML parsigin
#include "boost/property_tree/xml_parser.hpp"
#include "boost/property_tree/ptree.hpp"

// STL includes
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

TRRegionXMLHandler::TRRegionXMLHandler(TRTTransitionRadiation *tr) :
  m_theProcess(tr),
  m_storeGate(nullptr),
  m_initialLayoutIdDict(false),
  m_msg("TRRegionXMLHandler")
{}

void TRRegionXMLHandler::Process(const std::string& name)
{
  ISvcLocator * svcLocator = Gaudi::svcLocator(); // from Bootstrap

  if (msgLevel(MSG::DEBUG))
    msg(MSG::DEBUG) << "This is TRRegionXMLHandler. Handler called" << endmsg;

  StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
  if( sc.isFailure() ) {
    if (msgLevel(MSG::ERROR))
      msg(MSG::ERROR) << "Unable to locate StoreGate! Stopping!" << endmsg;
    throw std::runtime_error("Unable to locate StoreGate!");
  }
  StoreGateSvc* detStore = nullptr;
  sc = svcLocator->service( "DetectorStore", detStore);
  if( sc.isFailure() ) {
    if (msgLevel(MSG::ERROR))
      msg(MSG::ERROR) << "Unable to locate DetectorStore! Leaving!" << endmsg;
    throw std::runtime_error("Unable to locate DetectorStore!");
  }

  const IdDictManager * idDictMgr = nullptr;
  if (StatusCode::SUCCESS == detStore->retrieve(idDictMgr, "IdDict")) {
    if (idDictMgr) {
      std::string tag = idDictMgr->manager()->tag();
      m_initialLayoutIdDict =
        (tag == "initial_layout" || tag == "destaged_layout");
    }
  } else {
    if (msgLevel(MSG::FATAL))
      msg(MSG::FATAL) << "Could not retrieve geometry layout. TR process is not to be trusted in the following "
                      << endmsg;
    throw std::runtime_error("Could not retrieve geometry layout!");
  }

  // Crack open the XML file
  std::filebuf fb;
  if (!fb.open(name,std::ios::in)){
    msg(MSG::FATAL) << "Could not open file " << name << " bombing out" << endmsg;
    throw std::runtime_error("Could not open file!");
  }
  std::istream is(&fb);
  boost::property_tree::ptree pt;
  read_xml(is, pt);

  for( boost::property_tree::ptree::value_type const& v : pt.get_child("FADS") ) {
    if( v.first == "TRRegionParameters" ) {

      std::string volName=v.second.get<std::string>("<xmlattr>.RadiatorName");
      double foilThickness=v.second.get<double>("<xmlattr>.RadiatorFoilThickness");
      double gasThickness=v.second.get<double>("<xmlattr>.RadiatorGasThickness");
      int regionFlag=v.second.get<int>("<xmlattr>.RadiatorBARRELorENDCAP");
      std::string detectorPart=v.second.get<std::string>("<xmlattr>.DetectorPart");
    
      G4LogicalVolumeStore *g4lvs = G4LogicalVolumeStore::GetInstance();
      unsigned int numberOfVolumes = 0;
      for (const auto log_vol : *g4lvs){
        if (volName == static_cast<const std::string&>(log_vol->GetName())) {
          TRTRadiatorParameters rad( log_vol,
                                     foilThickness,gasThickness,
                                     (BEflag)regionFlag );
          m_theProcess->AddRadiatorParameters(rad);
          ++numberOfVolumes;
        }
      }
    
      if( numberOfVolumes == 0 ) {
        // In case of destaged geometry (no C wheels) any missing volume
        // except the radiators corresponding to the C wheels will cause
        // the code to abort
        if ( m_initialLayoutIdDict != 0 ) {
          if  ( ( volName!="TRT::MainRadiatorC" ) &&
                ( volName!="TRT::ThinRadiatorC" )    ) {
            if (msgLevel(MSG::FATAL))
              msg(MSG::FATAL) << " Volume-name " << volName
                              <<" not found! Geometry layout "
                              << m_initialLayoutIdDict << endmsg;
            std::abort();
          }
        }
      } // Didn't get any volumes
    } // Loop over region parameters objects
  } // Loop over FADS things in the XML file

}
