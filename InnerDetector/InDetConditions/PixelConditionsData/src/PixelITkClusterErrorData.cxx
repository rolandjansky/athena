/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/PixelITkClusterErrorData.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <string>
#include <stdexcept>


namespace PixelCalib{

// Load defaults - real values to be loaded from database

void PixelITkClusterErrorData::initialize(){

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
    *outfile << x.first << " " << std::get<0>(value) << " " << std::get<1>(value) << " " << std::get<2>(value) << " " << std::get<3>(value) << std::endl;

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
    // pixelID : delta_x : delta_error_x : delta_y : delta_error_y
    //
    
    std::string line;
    std::string var;

    std::string pixelId_str;
    double delta_x;
    double delta_error_x;
    double delta_y;
    double delta_error_y;    
    
    // TO DO - rewrite this with Boost tokenizer
    
    while( std::getline(infile,line) ){
            
      var = line.substr(0,line.find(" "));
      pixelId_str = var;
      line = line.substr(line.find(" ")+1,line.size());

      var = line.substr(0,line.find(" "));
      delta_x = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      delta_error_x = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());

      var = line.substr(0,line.find(" "));
      delta_y = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      delta_error_y = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
        
      Identifier pixelId;
      pixelId.set(pixelId_str);      

      setDeltaError(&pixelId,delta_x,delta_error_x,delta_y,delta_error_y);

    }
    
    infile.close();
  
  }else{
    throw std::runtime_error("ITkAnalogueClusteringConstantsFile \"" + file + "\" can not be read. Unable to proceed.");
  }
  
}


}
