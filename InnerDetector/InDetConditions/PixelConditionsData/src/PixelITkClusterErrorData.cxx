/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelITkClusterErrorData.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "GaudiKernel/ISvcLocator.h"

#include "Identifier/IdentifierHash.h"

#include <fstream>
#include <string>
#include <stdexcept>


namespace PixelCalib{

PixelITkClusterErrorData::PixelITkClusterErrorData():
  m_detStore(nullptr),
  m_pixelID(nullptr)
{
  initialize();
}

void PixelITkClusterErrorData::initialize(){

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("DetectorStore", m_detStore);
  if(sc.isFailure()){
    throw std::runtime_error("Could not retrieve DetectorStore");
  }
  sc = m_detStore->retrieve(m_pixelID, "PixelID");
  if(sc.isFailure()){
    throw std::runtime_error("Could not retrieve PixelID");
  }

  return;
}



std::pair<double,double> PixelITkClusterErrorData::getDelta(const Identifier* pixelId) const{

  std::tuple<double,double,double,double> value = m_constmap.at(*pixelId);
  return std::make_pair(std::get<0>(value),std::get<2>(value));

}


std::pair<double,double> PixelITkClusterErrorData::getDeltaError(const Identifier* pixelId) const{

  std::tuple<double,double,double,double> value = m_constmap.at(*pixelId);
  return std::make_pair(std::get<1>(value),std::get<3>(value));

}



// SET METHODS

void PixelITkClusterErrorData::setDeltaError(const Identifier* pixelId, double delta_x, double error_x, double delta_y, double error_y){

  std::tuple<double,double,double,double> linevalues = std::make_tuple(delta_x,error_x,delta_y,error_y);
  m_constmap[*pixelId] = linevalues;
  return;

}




// save all constants to file
void PixelITkClusterErrorData::print(std::string file) const {

  std::ofstream* outfile = new std::ofstream(file.c_str()); 

  for(auto& x : m_constmap){

    std::tuple<double,double,double,double> value = x.second;
    *outfile << m_pixelID->wafer_hash(x.first) << " " << std::get<0>(value) << " " << std::get<1>(value) << " " << std::get<2>(value) << " " << std::get<3>(value) << std::endl;

  }

  outfile->close(); 
  delete outfile; 
}



// Load ITk constants from file
void PixelITkClusterErrorData::load(std::string file){

  std::ifstream infile( file.c_str() );
  
  if(infile.is_open()){
       
    //
    // Data in the file is stored in the following columns:
    // waferID_hash : delta_x : delta_error_x : delta_y : delta_error_y
    //

    int waferID_hash_int;
    double delta_x;
    double delta_error_x;
    double delta_y;
    double delta_error_y;    

    while(!infile.eof()){

      infile >> waferID_hash_int >> delta_x >> delta_error_x >> delta_y >> delta_error_y;
        
      IdentifierHash waferID_hash(waferID_hash_int);
      Identifier pixelId = m_pixelID->wafer_id(waferID_hash);
      setDeltaError(&pixelId,delta_x,delta_error_x,delta_y,delta_error_y);

    }
    
    infile.close();
  
  }else{
    throw std::runtime_error("ITkAnalogueClusteringConstantsFile \"" + file + "\" can not be read. Unable to proceed.");
  }
  
}


}
