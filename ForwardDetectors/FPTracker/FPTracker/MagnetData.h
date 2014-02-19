/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_MAGNETDATA_H
#define FPTRACKER_MAGNETDATA_H
#include <string>
#include <ostream>
#include <vector>
namespace FPTracker{
  struct MagnetData{

    int nmagnet;
    static const int nside        = 2;
    static const int nmagapergeo  = 4;

    typedef std::vector<std::vector<double> >               ArrayDouble2D;
    typedef std::vector<std::vector<int> >                  ArrayInt2D;
    typedef std::vector<std::vector<std::vector<double> > > ArrayDouble3D;

    ArrayDouble2D magnet_x;
    ArrayDouble2D magnet_y;
    ArrayDouble2D magnet_z;
    ArrayDouble2D magnet_length;
    ArrayDouble2D magnet_strength;
    ArrayDouble2D magnet_xb;

    ArrayDouble3D magnet_aperA;

    ArrayInt2D    magnet_type;
    ArrayInt2D    magnet_aperclass;

    

    float  x0IP;
    float  y0IP;
    float  ax0IP;
    float  ay0IP;

    MagnetData();
    void setNumberOfMagnets(int);
    std::string toString() const;
  };
  
  std::ostream& operator<<(std::ostream&, const MagnetData&);
}
#endif
