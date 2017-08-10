/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// a class to fit CSC strips in a plane into clusters                      //
//                                                                         //
// BNL March 26 2003  --- Ketevi A. Assamagan                              //
// UCI June   6 2006  --- Michael Schernau                                 //
/////////////////////////////////////////////////////////////////////////////


#include <cassert>
#include <algorithm>
#include <math.h>

#include "CscParabola.h"

CscParabola::CscParabola() {
  m_msgSvc           =  0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", m_msgSvc);
  if (sc.isFailure()) std::cout << "CscParabola::Fail to locate Message Service" << std::endl;
}

void CscParabola::cscParabola(double* qstr, int& NStrip, double& thr, double& da, int& ncl, 
		              double* sig, double* zpos, double&)
{

  MsgStream mLog(m_msgSvc, "CscParabola");

  /// Find the hits and calculate the positions and the errors on the positions
  
  int clusters[96]; // stores the peak channel for each hit.
  ncl = 0; // number of clusters

  /// Look for peaks above the clustering threshold:
  /// 
  /// Dead strip in the middle of the cluster will mess up the 
  /// the cluster, but those clusters don't reconstruct well, anyway
  
  for (int ch = 1; ch < NStrip-1; ch++) { // loop over channels
    if (qstr[ch] > thr) { // there is activity above threshold
    	if ((qstr[ch] > qstr[ch-1]) && (qstr[ch] > qstr[ch+1])) { // this is a peak
           clusters[ncl] = ch; // store the peak channel number
           ncl++;              // count clusters
        }
    }   
  } // next channel
  
  int L = NStrip; // should be 192 or 48, is used for position relative to center of chamber

  if (L<96) { // actually L=48 for the transverse strips.
  	/// These are transverse (phi) hits: charge interpolation not possible
  	/// Add clusters at the edge of the chamber:
  	/// This is only meaningful for transverse strips, since they have no charge interpolation.
  	if ((qstr[0]>qstr[1])&&(qstr[0]>thr)) {
  	  clusters[ncl] = 0; // store the peak channel number
      ncl++;
    }  	
    if ((qstr[L-1]>qstr[L-2])&&(qstr[L-1]>thr)) {
  	  clusters[ncl] = L-1; // store the peak channel number
      ncl++;
    } 
    // Loop over clusters and calculate their position:
    for (int ic = 0; ic < ncl; ic++) {

      int maxStrip = clusters[ic];    // strip with the peak charge
      double maxPos;                  // position of that strip

      /// The strip with the highest charge defines 
      /// the position of the cluster:
      maxPos = da*(maxStrip+0.5-L/2); // position of peak channel relative to center of chamber

   

     /// now offer the maxPos result as the only result:
     *(zpos+ic*3  ) = maxPos;                     // just the peak channel
     *(zpos+ic*3+1) = maxPos;             
     *(zpos+ic*3+2) = maxPos; 
     *(sig+ic)      = da/sqrt(12.0);              // independent of charge.
   } // next cluster
   
 } else { // this is the case for the precision strips
   // Loop over clusters and calculate their position:
   for (int ic = 0; ic < ncl; ic++) {
 
     int maxStrip = clusters[ic];    // strip with the peak charge
     double maxPos;                  // position of that strip
 
     /// The strip with the highest charge defines 
     /// the position of the cluster:
     maxPos = da*(maxStrip+0.5-L/2); // position of peak channel relative to center of chamber
 
     /// The parabola method for position interpolation:
     /// The constants are from test beam and may need adjustment
     double a = qstr[maxStrip-1];    // left   charge
     double b = qstr[maxStrip];      // center charge
     double c = qstr[maxStrip+1];    // right  charge
     double x = 0.5*(a-c)/(a+c-2*b); // peak position of a parabola through the 3 points (-1,a), (0,b), (1,c).
     double p = 3.24610e-01*atan(8.19201*x)+1.36329e-01*x; // corrected pos. in strip width from -0.5 to 0.5
     
 
     /// now offer the maxPos, the parabola method and the center of gravity
     /// this selection is made in the CscClusterization algorithm
     //*(zpos+ic) = maxPos;                     // just the peak channel
     *(zpos+ic) = maxPos + da*p;              // parabola interpolation
     //*(zpos+ic) = maxPos + da* (c-a)/(a+b+c); // uncorrected center of gravity
     *(sig+ic)      = da/5000*75;                 // 75 micron eror, 5mm strip pitch, independent of charge.
      
     // future improvement:
     // check distance to other hits and increase errors for close hits.
      
     /*****************************************
     The error of maxPos alone would be 
     *(sig+ic)      = da/sqrt(12.0);
     *****************************************/
   }   //  next ic
} // if L>96

}

