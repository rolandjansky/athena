/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         shiftRoI.cxx  
 **
 **   Description:  shift all the y coordinates of the space points  
 **                 in an RoI  
 ** 
 **   Author:       M.Sutton  
 **
 **   Created:      Sat Sep 18 00:38:05 CEST 2004
 **   Modified:     
 **
 **************************************************************************/ 

#include "TrigIDSCAN/shiftRoI.h"

using namespace std;
using namespace TrigIdScan;



void TrackRoad::fitx() { 
    
  double S   = 0;
  double Sx  = 0;
  double Sx2 = 0;
  double Sy  = 0;
  double Sy2 = 0;
  double Sxy = 0;
  
  for ( vector<point*>::const_iterator hitr(mhits.begin()) ; hitr!=mhits.end() ; hitr++ ) {
    S   += 1;
    Sx  += (*hitr)->x;
    Sy  += (*hitr)->y;
    Sx2 += (*hitr)->x*(*hitr)->x;
    Sy2 += (*hitr)->y*(*hitr)->y;
    Sxy += (*hitr)->x*(*hitr)->y;
  }
  
  mfitxoffset   = 0;
  mfitxgradient = 0;
  
  // fitting in x as a function of y (good for cosmics)
  
  double denomy = mhits.size()*Sy2 - Sy*Sy;
  if (denomy>0) {
    mfitxoffset   = (Sx*Sy2 - Sy*Sxy)/denomy;
    mfitxgradient = (Sx*Sy - S*Sxy)/denomy;
  }
  
  //  return xvtx;
}



void TrackRoad::fitz() { 
  
  double S   = 0;
  double Sz  = 0;
  double Sz2 = 0;
  double Sy  = 0;
  double Sy2 = 0;
  double Szy = 0;
  
  for ( vector<point*>::const_iterator hitr(mhits.begin()) ; hitr!=mhits.end() ; hitr++ ) {
    S   += 1;
    Sz  += (*hitr)->z;
    Sy  += (*hitr)->y;
    Sz2 += (*hitr)->z*(*hitr)->z;
    Sy2 += (*hitr)->y*(*hitr)->y;
    Szy += (*hitr)->z*(*hitr)->y;
  }
  
  // fitting in z as a function of y (good for cosmics)
  
  mfitzoffset   = 0;
  mfitzgradient = 0;


  double denomy = mhits.size()*Sy2 - Sy*Sy;
  if (denomy>0) {
    mfitzoffset   = (Sz*Sy2 - Sy*Szy)/denomy;
    mfitzgradient = (Sz*Sy -  S*Szy)/denomy;
  }
  
  //  return zvtx;
}



ostream& operator<<(ostream& s, const point& h) {
  return s << "[ x="  << h.x 
	   << ",\ty=" << h.y 
	   << ",\tz=" << h.z 
	   << ",\tl=" << h.layer 
	   << ",\tr=" << h.r << "\t]";
} 



ostream& operator<<(ostream& s, const TrackRoad& t) {
  return s << "[ xgrad="      << t.xgradient() 
           << "\txoff="    << t.xoffset() 
           << "\tzgrad="      << t.zgradient() 
	   << "\tzoff="    << t.zoffset() 
           << "\tyoff(x)=" << t.yxoffset()
	   << "\tyoff(z)=" << t.yzoffset()
           << "\tNhits="      << t.Nhits() 
	   << "\tfitted xoff=" << t.xvtx()
	   << "\tfitted zoff=" << t.zvtx() << " ]";
} 





/**  TrackShift::doShift() looks at all the combinations of hits 
 **  (longest first) seeing if they are consistent with a track 
 **  before calculating the shift in x that would place the track 
 **  at the origin.
 **/

double TrackShift::doShift(double deltar) {

  //  cout << "TrackShift::doShift()" << endl;

  mRoad = TrackRoad();

  // static const unsigned int MinNPoints = 6;  // finish is we find track with >6 hits
  // get as a parameter
  //  static const double  yRoadWidth = 10;       // hits must be within yRoadWidth mm of expected position
  double deltarsq = deltar*deltar;

  // don't want too many hits!!
  // if ( mpoints.size()>500 ) return 0;
  
  vector<TrackRoad> t;  // track road candidates

  // look up table of ptrs to hits by layer
  // use layers 1..7 and -1..-7 for hits in top 
  // and bottom of detector respectively, don't
  // have any hits in layer h[0], use offset 
  // pointer so can access h[-7] to h[7] etc
  // so instead of the usual (and best!!) layer=0..6 
  // convention, here, we use layer=1..7 and 
  // layer=-1..-7 so we can deal with "top" and 
  // "bottom" layers seperately
  vector<point*>    _h[15];  
  vector<point*>*    h = _h+7;  
                 
  // set up the lookup table of pointers to the hits - avoids actually
  // copying the hits, since this sets up the data base about 20% quicker
  // use references everywhere else in code, so execution of the rest of 
  // the code shows no improvement.

  //  for ( vector<point>::iterator hitr(mpoints.begin()) ; hitr!=mpoints.end() && ihit<100 ; hitr++ ) { 
  for ( unsigned i=0 ; i<mpoints.size() ; i++ ) { 
    //  only look in the barrel
    if ( mpoints[i].layer<0 || mpoints[i].layer>6 ) continue;

    // top or bottom of barrel?    
    if ( mpoints[i].y>0 )  h[ (mpoints[i].layer+1)].push_back(&(mpoints[i]));
    else                   h[-(mpoints[i].layer+1)].push_back(&(mpoints[i]));
  }



  // TrackRoad* iptr = NULL;
  unsigned hmax = 0;
  // loop down all possible lengths from full length (14 layers)
  // to minimum length (2 layers) - single layer tracks are not 
  // considered
  for ( unsigned len=14 ; len>1 ; len-- ) { 

    //    cout << "len=" << len << endl;

    // if this length is les than the number of hit layers
    // for the longest track so far then give up  
    if ( len<hmax ) break;
    // if ( len<hmax ) cout << "giving up!!" << endl;
 
    // all possible top layers
    //    for ( int tlayer=7 ; tlayer-len>=-7 ; tlayer--) { 
    for ( int _tlayer=14 ; _tlayer-- ; ) { 
      
      // calculate bottom layer for this length track
      // int _blayer = tlayer-len+1;

      int tlayer = _tlayer-6;
      int blayer =  tlayer-len+1;

      if ( tlayer<=0 ) tlayer--;
      if ( blayer<=0 ) blayer--;

      // no layer=0 in this scheme
      if ( blayer<-7 ) break;

      //      cout << "len=" << len;
      //      cout << "\tb=" << blayer << " ( " << h[blayer].size() << " )" 
      //	   << "\tt=" << tlayer << " ( " << h[tlayer].size() << " )" << endl;

      // all possible hits in top layer
      for ( unsigned itop=0 ; itop<h[tlayer].size() ; itop++ ) { 
	
	// all posible hits in bottom layer
	for ( unsigned ibot=0 ; ibot<h[blayer].size() ; ibot++ ) { 
	  
	  // create the track road candidate for this particular 
	  // top and bottom pair of hits
	  TrackRoad tr( h[tlayer][itop], h[blayer][ibot]);

	  //	  cout << "\tnew road\n\t" << *h[tlayer][itop] << "\n\t" << *h[blayer][ibot] << endl;

	  // all layers in between
	  for ( int layer=blayer+1 ; layer<tlayer ; layer++ ) { 
	    if ( layer==0 ) continue; 

	    // find the single closest hit per layer
	    double diff = 0;
	    point* closest = NULL;

	    // all hits in layer
	    for ( unsigned i=0 ; i<h[layer].size() ; i++ ) { 

	      point* mp = h[layer][i];
	      
	      double x = mp->x;
	      double y = mp->y;
	      double z = mp->z;
	        
	      double xdiff = 2.5*(x - ( tr.xgradient()*y + tr.xoffset() )) ;
	      double zdiff = 0.08*(z - ( tr.zgradient()*y + tr.zoffset() )) ;
	      
	      double delr = xdiff*xdiff+zdiff*zdiff;

	      if ( closest==NULL || delr<diff ) { 
		closest = mp;
		diff    = delr;
		
		//		if ( diff<deltarsq ) 
		//	cout << "\t" << *mp 
		//	     << "\tdx=" << xdiff 
		//	     << "\tdz=" << zdiff 
		//	     << "\tdr=" << sqrt(delr) << "  " << xdiff/zdiff << endl;
		
	      }
	      
	    } // finished all hits in layer

	    // found a closest hit in this layer?
	    if ( closest!=NULL ) { 
	      if ( diff<deltarsq )	tr.hits().push_back(closest);
	    }	    

	  } // finished all intermediate layers

	  // add the track
	  // t.push_back(tr);  

	  //	  cout << "len=" << len << "\t" << tr << "\tN=" << tr.Nhits() << endl;

	  // get track road with most hits 
	  if ( tr.Nhits()>hmax ) {
	    t.push_back(tr);
	    hmax = tr.Nhits();
	    //    cout << "TrackShift::doShift() temp max = " << hmax
	    //	       << "\t(track "   << t.size()-1 << ")" << endl;
	  }

	  else if ( hmax==tr.Nhits() ) {
	    // if they have the same number should really
	    //   get the one with the smallest chi2, but I cheat and 
	    //   get the most vertical one
	    // note that the last track in the t vector is always the best road so far
 
	    if ( fabs(tr.xgradient())<fabs(t.back().xgradient()) ) {
	      // cout << "max grad=" << t.back().xgradient()
	      //      << "\tgrad="   << tr.xgradient() << endl; 
	      t.push_back(tr);
	      hmax = tr.Nhits();
	      //  cout << "TrackShift::doShift() temp max = " << hmax
	      //       << "\t(track "   << t.size()-1 << ") [most vertical]" << endl;
	    }
	  } 

	} // bottom layer hits
      } // top layer hits
      
    } // top layer
  } // track length

  // not found any track candidates, return shift=0  
  if ( hmax==0 ) return 0;

  // assign the "highest occupancy" simple track road 
  mRoad = t.back();

  // fit the offset and gradient of this track road
  mRoad.fit();
  //  cout << "best road\n" << mRoad << endl;

  return mRoad.xvtx();
}




