/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConditionsData/ITkPixelClusterErrorData.h"

#include "GaudiKernel/ISvcLocator.h"

#include "Identifier/IdentifierHash.h"

#include <fstream>
#include <string>
#include <stdexcept>


namespace ITk
{

void PixelClusterErrorData::Initialize()
{

  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("DetectorStore", m_detStore);
  if(sc.isFailure()){
    throw std::runtime_error("Could not retrieve DetectorStore");
  }
  sc = m_detStore->retrieve(m_pixelID, "PixelID");
  if(sc.isFailure()){
    throw std::runtime_error("Could not retrieve PixelID");
  }

}


std::pair<double,double> PixelClusterErrorData::getDelta(const Identifier* pixelId,
                                                         int sizePhi, double angle,
                                                         int sizeZ, double eta) const
{

  std::vector<double> value = m_constmap.at(*pixelId);
  double period_phi = value[0];
  double period_sinheta = value[1];
  double delta_x_slope = value[2];
  double delta_x_offset = value[3];
  double delta_y_slope = value[5];
  double delta_y_offset = value[6];

  double delta_x = delta_x_slope * fabs(angle - period_phi*(sizePhi-2)) + delta_x_offset;
  double delta_y = delta_y_slope * fabs(sinh(fabs(eta)) - period_sinheta*(sizeZ-2)) + delta_y_offset;

  return std::make_pair(delta_x,delta_y);

}


std::pair<double,double> PixelClusterErrorData::getDeltaError(const Identifier* pixelId) const
{

  std::vector<double> value = m_constmap.at(*pixelId);

  double delta_x_error = value[4];
  double delta_y_error = value[7];

  return std::make_pair(delta_x_error,delta_y_error);

}


// SET METHODS

void PixelClusterErrorData::setDeltaError(const Identifier* pixelId,
					     double period_phi, double period_sinheta,
					     double delta_x_slope, double delta_x_offset, double error_x,
					     double delta_y_slope, double delta_y_offset, double error_y)
{

  std::vector<double> linevalues = {period_phi, period_sinheta,
				    delta_x_slope, delta_x_offset, error_x,
				    delta_y_slope, delta_y_offset, error_y};

  m_constmap[*pixelId] = linevalues;
  return;

}


// save all constants to file
void PixelClusterErrorData::print(const std::string& file) const
{

  std::ofstream outfile(file.c_str());

  for(const auto & x : m_constmap){

    std::vector<double> value = x.second;
    outfile << m_pixelID->wafer_hash(x.first) << " " << value[0] << " " << value[1] << " " << value[2] << " " << value[3] << " " << value[4] << " " << value[5] << " " << value[6] << " " << value[7] << std::endl;

  }

  outfile.close();
}



// Load ITk constants from file
void PixelClusterErrorData::load(const std::string& file){

  std::ifstream infile( file.c_str() );

  if(infile.is_open()){

    //
    // Data in the file is stored in the following columns:
    // waferID_hash : period_phi : period_sinheta : delta_x_slope : delta_x_offset : delta_error_x : delta_y_slope : delta_y_offset : delta_error_y
    //

    int waferID_hash_int;
    double period_phi;
    double period_sinheta;
    double delta_x_slope;
    double delta_x_offset;
    double delta_error_x;
    double delta_y_slope;
    double delta_y_offset;
    double delta_error_y;

    while(!infile.eof()){

      infile >> waferID_hash_int >> period_phi >> period_sinheta >> delta_x_slope >> delta_x_offset >> delta_error_x >> delta_y_slope >> delta_y_offset >> delta_error_y;

      IdentifierHash waferID_hash(waferID_hash_int);
      Identifier pixelId = m_pixelID->wafer_id(waferID_hash);
      setDeltaError(&pixelId,
		    period_phi, period_sinheta,
		    delta_x_slope, delta_x_offset, delta_error_x,
		    delta_y_slope, delta_y_offset, delta_error_y);

    }

    infile.close();

  } else {
    throw std::runtime_error("ITkAnalogueClusteringConstantsFile \"" + file + "\" can not be read. Unable to proceed.");
  }

}

} // namespace ITk
