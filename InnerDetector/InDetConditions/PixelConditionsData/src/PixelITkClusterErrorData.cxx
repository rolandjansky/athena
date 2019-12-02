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



double PixelITkClusterErrorData::getITkDelta(int xy, int itkregion, int layer) const{
  std::tuple<int,int,int> linekeys = std::make_tuple(xy,itkregion,layer);
  std::tuple<double,double> mytuple = m_constmap.at(linekeys);
  return std::get<0>(mytuple);
}


double PixelITkClusterErrorData::getITkDeltaError(int xy, int itkregion, int layer) const{
  std::tuple<int,int,int> linekeys = std::make_tuple(xy,itkregion,layer);
  std::tuple<double,double> mytuple = m_constmap.at(linekeys);
  return std::get<1>(mytuple);
}



// SET METHODS

void PixelITkClusterErrorData::setITkDeltaError(int xy, int itkregion, int layer, double delta, double error){

  //Add new angle, delta, error values to existing ones
  std::tuple<int,int,int> linekeys = std::make_tuple(xy,itkregion,layer);
  std::tuple<double,double> linevalues = std::make_tuple(delta,error);
  m_constmap[linekeys] = linevalues;
  return;

}




// save all constants to file
void PixelITkClusterErrorData::print(std::string file) const {

  std::ofstream* outfile = new std::ofstream(file.c_str()); 

  int n_xy = 2; // x + y
  int n_regions = 3; // barrel, inclined, endcap
  int n_layers = 5;

  for(int xy=0; xy<n_xy; xy++){
    for(int region=0; region<n_regions; region++){
      for(int layer=0; layer<n_layers; layer++){

	float delta = getITkDelta(xy,region,layer);
	float delta_err = getITkDeltaError(xy,region,layer);

	std::string xy_string = xy==0 ? "phi" : "eta";
	std::string region_string = "barrel";
	if(region==1) region_string = "inclined";
	else if(region==1) region_string = "endcap";

	*outfile << xy_string << " " << region_string << " " << layer << " " << delta << " " << delta_err <<std::endl;

      }
    }
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
    // local_x_or_y : pixel_region : pixel_layer : delta : delta_error
    //
    // so for example:
    // eta barrel 0 -1.16667 -0.0604302 0.00706271
    // is:
    // local_x_or_y = eta
    // pixel_region = barrel
    // pixel_layer = 0
    // delta = -0.0604302
    // delta_error = 0.00706271
    //
    // for vector access of local_x_or_y
    // phi=local_x=0, eta=local_y=1
    // for vector access of pixel_region
    // flat=0, inclined=1, endcap=2
    //


    
    std::string line;
    std::string var;
    
    int local_x_or_y;
    int pixel_region;
    int pixel_layer;
    double delta;
    double delta_error;
    
    // TO DO - rewrite this with Boost tokenizer
    
    while( std::getline(infile,line) ){
            
      var = line.substr(0,line.find(" "));
      if(var=="phi"){
	// local x
	local_x_or_y = 0;
      }else if(var=="eta"){
	// local y
	local_x_or_y = 1;
      }else{	
	throw std::runtime_error("ITK ANALOGUE CLUSTERING INPUT TEXT FILE IS NOT WHAT I EXPECTED!! DANGER!! : " + line + " (! " + var + " !)");
      }
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      if(var=="barrel"){
	// flat barrel region
	pixel_region = 0;
      }else if(var=="inclined"){
	// inclined barrel region
	pixel_region = 1;
      }else if(var=="endcap"){
	// endcap region
	pixel_region = 2;
      }else{
	throw std::runtime_error("ITK ANALOGUE CLUSTERING INPUT TEXT FILE IS NOT WHAT I EXPECTED!! DANGER!! : " + line + " (! " + var + " !)");
      }
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      pixel_layer = std::stoi(var);
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      delta = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      delta_error = std::stod(var);
        
      setITkDeltaError(local_x_or_y,pixel_region,pixel_layer,
		       delta,delta_error);
      
    }
    
    infile.close();
  
  }else{
    throw std::runtime_error("ITkAnalogueClusteringConstantsFile \"" + file + "\" can not be read. Unable to proceed.");
  }
  
}


}
