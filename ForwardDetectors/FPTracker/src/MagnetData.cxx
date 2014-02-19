/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/MagnetData.h"
#include "FPTracker/DataDumperHelpers.h"
namespace FPTracker{

  MagnetData::MagnetData():
    magnet_x        (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_y        (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_z        (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_length   (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_strength (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_xb       (std::vector< std::vector<double> >(nside, std::vector<double>())),
    magnet_aperA    (std::vector< std::vector<std::vector<double> > >(nside, 
								      std::vector< std::vector<double> >(nmagapergeo, 
													 std::vector<double>()
													  )
								      )
		     ),
    
    magnet_type     (std::vector< std::vector<int> >(nside, std::vector<int>())),
    magnet_aperclass(std::vector< std::vector<int> >(nside, std::vector<int>())){
  
    nmagnet = 0;
   
    x0IP = 0; 
    y0IP = 0;
    ax0IP = 0;
    ay0IP = 0;
  }
    
    
  void MagnetData::setNumberOfMagnets(int nmags){
    for(int is = 0; is<nside; ++is){
      magnet_x        [is].reserve(nmags);
      magnet_y        [is].reserve(nmags);
      magnet_z        [is].reserve(nmags);
      magnet_length   [is].reserve(nmags);
      magnet_strength [is].reserve(nmags);
      magnet_xb       [is].reserve(nmags);
      magnet_type     [is].reserve(nmags);
      magnet_aperclass[is].reserve(nmags);
      
      for(int ig = 0; ig<nmagapergeo; ++ig){magnet_aperA[is][ig].reserve(nmags); }
    }
    nmagnet = nmags;
    
  }

  std::string MagnetData::toString() const{
    std::string total = "\n\n------------ MagnetData ----------\n\n";
    total += "\nmagnet_x:\n";
    total += array2DToString(magnet_x);
    total += "\nmagnet_y:\n";
    total += array2DToString(magnet_y);
    total += "\nmagnet_z:\n";
    total += array2DToString(magnet_z);
    total += "\nmagnet_length:\n";
    total += array2DToString(magnet_length);
    total += "\nmagnet_strength:\n";
    total += array2DToString(magnet_strength);
    total += "\nmagnet_xb:\n";
    total += array2DToString(magnet_xb);
    total += "\nmagnet_type:\n";
    total += array2DToString(magnet_type);
    total += "\nmagnet_aperclass:\n";
    total += array2DToString(magnet_aperclass);
    total += "\nmagnet_aperA:\n";
    total += array3DToString(magnet_aperA);
    return total;

  }
  std::ostream& operator<<(std::ostream& os, const MagnetData& md){
    os<<md.toString();
    return os;
  }
}

