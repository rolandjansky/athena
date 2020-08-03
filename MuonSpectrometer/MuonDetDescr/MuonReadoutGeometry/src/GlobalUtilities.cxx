/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 global stuff
 -----------------------------------------
 ***************************************************************************/

#include "MuonReadoutGeometry/GlobalUtilities.h"
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <cmath>

namespace MuonGM 
{

std::string buildString(int i, int ncha)
{
  if (ncha<=0) {
    std::ostringstream ostigg;
    ostigg << i;
    return ostigg.str();
  } else {
    int ij = i;
    std::ostringstream ostigg;
    for (int j=ncha-1; j>=0; j--) {
      ostigg << int(ij/std::pow(10.0,j));
      ij = int(ij%int(std::pow(10.0,j)));
    }
    return ostigg.str();
  }
}
    
    


int strtoint(std::string str, unsigned int istart, unsigned int length)
{
  std::string s(str.substr(istart,length));
  int result = std::stoi(s);
  return result;
}



int stationPhiTGC(std::string stName, int fi, int zi_input, std::string geometry_version) 
{
    // fi and zi_imput are the amdb indices (1, ... 8) and (-8, 8) are their ranges
    std::string stName3 = stName.substr(0,3);
    int stphi = 0;

    int zi = std::abs(zi_input);
    fi -= 1; // start from 0 
    

    int igeometry_ref = 303;
    int igeometry_version = 0;
    if (geometry_version.substr(0,1)=="R")
    {        
        igeometry_version = strtoint(geometry_version, 2, 2)*100+strtoint(geometry_version, 5, 2);
    }
    
    if (stName3 == "T4E")
    {
        switch (fi)
        {
        case 0: stphi = zi;
            break;
        case 1: stphi = zi+3;
            break;
        case 2: stphi = zi+6;
            break;
        case 3: 
	    if (igeometry_version < igeometry_ref) 
            {
		stphi = zi+9-1; //zi are 2 and 3 (counting do not tart by 1)
            }
	    else
		stphi = zi+9; //zi are numbered in order, i.e. 1 and 2 
	    break;	
        case 4: stphi = zi+11;
            break;
        case 5: stphi = zi+14;
            break;
        case 6: stphi = zi+16;
            break;
        case 7: stphi = zi+19;
            break;
        default: stphi = 0;
        }
        // minumum stPhi at phi 0
        stphi = stphi - 1;
        if (stphi <1) stphi = stphi + 21;
    }
    else 
    {
        int nch = 3;
        if ( (stName).substr(2,1) == "E" ) nch=6;
        int fioff = std::abs(zi);
        if (fioff>3 && (stName).substr(2,1) == "F")  fioff = fioff-3;
        // minumum stPhi at phi 0
        if ((stName).substr(2,1) == "F") fioff = fioff -1;
        else fioff = fioff -2;
        stphi = fi*nch + fioff;
        if (stphi<1) {
            if ((stName).substr(2,1) == "F") stphi = stphi + 24;
            else stphi = stphi + 48;
        }
    }

    return stphi;    
}
 
}
