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

void PixelITkClusterErrorData::Initialize(){

  return;
}


std::vector< std::tuple<double,double,double> > PixelITkClusterErrorData::getITkAC_angleVec( std::tuple<int,int,int> linekeys ) const{

  if(m_constmap.find(linekeys) == m_constmap.end()){
    // element linekeys does not exist in m_constmap
    // so return an empty vector ready to be filled
    std::vector< std::tuple<double,double,double> > toreturn = {};
    return toreturn;
  }
  else{
    // else return the existing vector
    return m_constmap.at(linekeys);
  }

}


std::tuple<double,double,double> PixelITkClusterErrorData::getITkAC_closestAngle(int xy, int itkregion, int layer, double angle) const{
  
  std::tuple<int,int,int> linekeys = std::make_tuple(xy,itkregion,layer);
  std::vector< std::tuple<double,double,double> > anglevec = getITkAC_angleVec(linekeys);
  
  double angdiff = 999;
  std::tuple<double,double,double> mytuple = std::make_tuple(0,0,0);
  
  // TO DO - write a faster sorting / search algorithm
  for( unsigned int i=0 ; i<anglevec.size() ; i++){
    double angle_from_vec = std::get<0>(anglevec.at(i));
    double anglediff_i = abs(angle_from_vec-angle);
    if( anglediff_i > angdiff ){
      // the break here will only happen if we have found the minimum angdiff,
      // and have passed it
      // (ie. anglediff_i is now growing larger again as we run through the vector)
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      // !!! This requires that the angle bin centres are ordered in the input text file !!!
      // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      break;
    }
    else if( anglediff_i < angdiff ){
      angdiff = anglediff_i;
      mytuple = anglevec.at(i);
    }
  }
  
  return mytuple;
  
}


double PixelITkClusterErrorData::getITkDelta(int xy, int itkregion, int layer, double angle) const{
  std::tuple<double,double,double> mytuple = getITkAC_closestAngle(xy, itkregion, layer, angle); 
  return std::get<1>(mytuple);
}


double PixelITkClusterErrorData::getITkDeltaError(int xy, int itkregion, int layer, double angle) const{
  std::tuple<double,double,double> mytuple = getITkAC_closestAngle(xy, itkregion, layer, angle); 
  return std::get<2>(mytuple);
}



// SET METHODS

void PixelITkClusterErrorData::setITkAngleDeltaError(int xy, int itkregion, int layer, double angle, double delta, double error){

  //Add new angle, delta, error values to existing ones
  std::tuple<int,int,int> linekeys = std::make_tuple(xy,itkregion,layer);
  std::tuple<double,double,double> linevalues = std::make_tuple(angle,delta,error);
  std::vector< std::tuple<double,double,double> > anglevec = getITkAC_angleVec(linekeys);
  anglevec.push_back( linevalues );  
  m_constmap[linekeys] = anglevec;
  return;

}




// save all constants to file
void PixelITkClusterErrorData::Print(std::string file) const {

  std::ofstream* outfile = new std::ofstream(file.c_str()); 

  //TO IMPLEMENT

  outfile->close(); 
  delete outfile; 
}



// Load ITk constants from file
void PixelITkClusterErrorData::Load(std::string file){

  std::ifstream infile( file.c_str() );
  
  if(infile.is_open()){
       
    //
    // Data in the file is stored in the following columns:
    // local_x_or_y : pixel_region : pixel_layer : incident_angle_bin_centre (in Radians, between -pi/2 and pi/2) : delta : delta_error
    //
    // so for example:
    // eta barrel 0 -1.16667 -0.0604302 0.00706271
    // is:
    // local_x_or_y = eta
    // pixel_region = barrel
    // pixel_layer = 0
    // incident_angle_bin_centre = -1.16667
    // delta = -0.0604302
    // delta_error = 0.00706271
    //
    // for vector access of local_x_or_y
    // phi=local_x=0, eta=local_y=1
    // for vector access of pixel_region
    // flat=0, inclined=1, endcap=2
    //


    // The incident angle has been replaced by an integer encoding for
    // 1 = single pixel => no correction
    // 2 = several pixels => non-zero correction
    // Some non-trivial gymnastics is performed to still use the previous implementation but it would benefit from some cleaning
    // TO BE DONE

    
    std::string line;
    std::string var;
    
    int local_x_or_y;
    int pixel_region;
    int pixel_layer;
    double incident_angle_bin_centre;
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
      incident_angle_bin_centre = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
  
      var = line.substr(0,line.find(" "));
      delta = std::stod(var);
      line = line.substr(line.find(" ")+1,line.size());
      
      var = line.substr(0,line.find(" "));
      delta_error = std::stod(var);
        
      setITkAngleDeltaError(local_x_or_y,pixel_region,pixel_layer,
			    incident_angle_bin_centre,delta,delta_error);
      
    }
    
    infile.close();
  
  }else{
    throw std::runtime_error("ITkAnalogueClusteringConstantsFile \"" + file + "\" can not be read. Unable to proceed.");
  }
  
}


}
