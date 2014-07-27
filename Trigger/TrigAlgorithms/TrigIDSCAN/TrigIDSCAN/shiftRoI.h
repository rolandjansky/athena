// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         shiftRoI.h  
 **
 **   Description:  shift all the x coordinates of the space points  
 **                 in an RoI  
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Sat Sep 18 00:38:05 CEST 2004
 **   Modified:     
 **                   
 **
 **************************************************************************/ 

#ifndef __SHIFTROI_H
#define __SHIFTROI_H


#include <cmath>
#include <vector>
#include <set>
#include <iostream>

using namespace std;

namespace TrigIdScan {

class point {
  
public:
  
  point(double rt=0, double phit=0, double zt=0, long layt=0) : 
    r(rt), phi(phit), z(zt), layer(layt) {
    x = r*cos(phi);
    y = r*sin(phi);
  } 
  
  point& setxy(double xt, double yt) { 
    x   = xt;   
    y   = yt;
    r   = get_r();
    phi = get_phi();
    return *this;
  }
  
  point& shiftxy(double xt=0, double yt=0) {
    x -= xt;
    y -= yt;
    get_r();
    get_phi();
    return *this;
  }
  
  double get_r() {  return ( r = sqrt( x*x + y*y ) );  }
  
  double get_phi() {
    if ( x==0 ) {
      phi = ( y<0 ? -M_PI : M_PI);
    }
    else {
      if ( x>0 ) phi = atan(y/x);
      else       phi = atan(y/x) + ( y<0 ? -M_PI : M_PI ) ;
    }
    return  phi;
  }
  
public:
  
  double r;
  double phi;
  
  double x;
  double y;
  double z;
  
  long   layer;

};



// class which can be used to define a narrow band based on space points at either end
// and then can store hits from a sample that lie within the narrow band. It is not very 
// fast, since it makes an internal copy of the spacepoints.

class TrackRoad { 

public:

  TrackRoad() : 
    mfit(false), 
    mxgradient(0),    mxoffset(0), 
    mfitxgradient(0), mfitxoffset(0), 
    mzgradient(0),    mzoffset(0), 
    mfitzgradient(0), mfitzoffset(0)
  { } 

  TrackRoad( point* h1, point* h2) : 
    mfit(false), 
    mxgradient(0),    mxoffset(0), 
    mfitxgradient(0), mfitxoffset(0), 
    mzgradient(0),    mzoffset(0), 
    mfitzgradient(0), mfitzoffset(0)
  { 
    double xdiff = h2->x-h1->x;
    double ydiff = h2->y-h1->y;
    double zdiff = h2->z-h1->z;
    
    mhits.push_back(h1);
    mhits.push_back(h2);
    // mhits.insert(h1);
    // mhits.insert(h2);
    
    //    if ( h2->y>0 ) mlayers.insert(h2->layer+1);
    //    else           mlayers.insert(-(h2->layer+1));

    //    if ( h1->y>0 ) mlayers.insert(h1->layer+1);
    //    else           mlayers.insert(-(h1->layer+1));

    if ( ydiff==0 ) return;
    
    mxgradient = xdiff/ydiff;
    mxoffset   = (h2->y*h1->x - h1->y*h2->x)/ydiff;

    mzgradient = zdiff/ydiff;
    mzoffset   = (h2->y*h1->z - h1->y*h2->z)/ydiff;
    
    //    cout << "track() " << xdiff << "  " << mgradient << "  " << moffset << endl;
  }

  double yx(double x) const { return (x-mxoffset)/mxgradient; }
  double yz(double z) const { return (z-mzoffset)/mzgradient; }

  double xgradient()  const  { return mxgradient; } 
  double xoffset()    const  { return mxoffset; } 
 
  double zgradient() const  { return mzgradient; } 
  double zoffset()   const  { return mzoffset; } 
  
  unsigned Nhits()     const  { return mhits.size(); }  
  
  double yxoffset()   const  { 
    if ( mxgradient==0 )  return 0; 
    else return -mxoffset/mxgradient;
  } 

  double yzoffset()  const  { 
    if ( mzgradient==0 )  return 0; 
    else return -mzoffset/mzgradient;
  } 
  
  vector<point*>& hits()   { return mhits; }
  //  set<point*>&    layers() { return mlayers; }

  bool isfit() { return mfit; }

  void fitx();
  void fitz();
  void fit()   { mfit=true; fitx(); fitz(); }

  void unfit() { mfit=false; }

  //  double xvtx()  { if ( !mfit ) fit(); return mfitxoffset; }
  //  double xgrad() { if ( !mfit ) fit(); return mfitxgradient; }

  //  double zvtx()  { if ( !mfit ) fit(); return mfitzoffset; }
  //  double zgrad() { if ( !mfit ) fit(); return mfitzgradient; }

  double xvtx()  const { return mfitxoffset; }
  double xgrad() const { return mfitxgradient; }

  double zvtx()  const { return mfitzoffset; }
  double zgrad() const { return mfitzgradient; }

  //  int layers()  { return mlayers; } 
  //  int Nlayers() { return mlayers.size(); } 

private:

  bool   mfit; 

  double mxgradient;
  double mxoffset;
  
  double mfitxgradient;
  double mfitxoffset;


  double mzgradient;
  double mzoffset;

  double mfitzgradient;
  double mfitzoffset;

  vector<point*>    mhits;
  //  set<int>          mlayers;
};





// class to store loop over all the hits and find the best track 
// road from the combinations  

class TrackShift { 

public:

  TrackShift(vector<point>& p) : mRoad(), mpoints(p) {  } 

  double doShift(double deltar=5); 

  TrackRoad& road() { return mRoad; }

private:

  TrackRoad     mRoad; 
  vector<point> mpoints;

};



}  // namespace TrigIdScan


using TrigIdScan::point;
using TrigIdScan::TrackRoad;

ostream& operator<<(ostream& , const point& );
ostream& operator<<(ostream& , const TrackRoad& );





#endif  /* __SHIFTROI_H */





