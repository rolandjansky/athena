/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// a class to fit CSC strips in a plane into clusters                      //
//                                                                         //
// BNL March 26 2003  --- Ketevi A. Assamagan                              //
/////////////////////////////////////////////////////////////////////////////

#include <map>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>

#include "CscAverage.h"

CscAverage::CscAverage() {
  m_msgSvc           =  0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("MessageSvc", m_msgSvc);
  if (sc.isFailure()) std::cout << "CscAverage::Fail to locate Message Service" << std::endl;
}

void CscAverage::cscaverage(double* qstr, int& NStrip, double& thr, double& da, int& ncl, 
		    double* sig, double* zpos, double& noise)
{

  MsgStream mLog(m_msgSvc, "CscAverage");

  /// Find the clusters  and calculate the positions and the errors on the positions
  
  bool cluster_found = false;
  std::map<unsigned int, std::vector<int> > clusters;
  unsigned int NClusters = 1;

  int L = NStrip;

  /// The list of strips above the clustering threshold define the 
  /// cluster - the hole are not treated in the sense that a dead 
  /// strip in the middle of the cluster will mess up the true 
  /// definition of the cluster
  for (int is = 0; is < L; ++is) {

    if (qstr[is] > thr) {
      cluster_found = true;
      clusters[NClusters].push_back(is);
    } else if (cluster_found) {
      cluster_found = false;
      ++NClusters;
    }
    
  }
  
  ncl = (int) clusters.size();

  // Loop on clusters and calculate their position
  for (std::map<unsigned int, std::vector<int> >::const_iterator ic = clusters.begin();
       ic != clusters.end();
       ++ic) {

    /// initialize some parameters
    double sum = 0.;
    double tcharge  = 0.;
    double tcharge2 = 0.;
    int count = 0;
    int maxStrip = -1;
    double maxCharge = 0;
    double maxPos = -1000.0;
    double avg    = maxPos;

    /// clustering by the weight average method
    for (std::vector<int>::const_iterator d = (*ic).second.begin(); d != (*ic).second.end(); ++d) {
        if (qstr[*d] > maxCharge) {
           maxStrip = *d;
           maxCharge = qstr[*d];
        } 
    }

    /// center of gravity method - weighted average
    for (int i = maxStrip-1; i<= maxStrip+1; ++i) {
        if (i>=0 && i<L) {
           double w = qstr[i];
           sum += da*(i+0.5-L/2) * w; 
           tcharge += w;
           count++;
           tcharge2 += count*count;
        }
    } 
    if (tcharge > 0) avg = sum/tcharge;

    /// clustering by just selection the strip with the highest charge
    /// as the position of the cluster
    if (maxStrip >= 0) maxPos = da*(maxStrip+0.5-L/2);

    /// the ratio method for clustering
    /// the constants are from test beam and may need adjustment
    // MS 07-02-2013: remove this section to fix compiler warning:
    // warning: variable 'ratio' set but not used [-Wunused-but-set-variable]
    /*****************************************
    double ratio = avg;
    if ( (maxStrip-1)>=0 && (maxStrip+1) < L) {
       double a = qstr[maxStrip-1];
       double b = qstr[maxStrip];
       double c = qstr[maxStrip+1];
       double r = (c-a)/(a+b+c); // raw value
       if (fabs(r)<0.4)
          ratio = maxPos + da*( 1.29209*r -7.84829*std::pow(r, 5) +25.6318*std::pow(r, 7) );
       else 
          ratio = maxPos + da *( 1.27790*r -20.9485*std::pow(r, 7) );
     } 
    *************************************/
    
     /// now make a selection between the maxPos, the ratio method and the weight average
     /// this selection is made in the CscClustrization algorithm
     *(zpos+(*ic).first-1) = avg;
     if (count > 1 && tcharge > 0) 
        *(sig+(*ic).first-1)  = noise * (da/(tcharge)) * sqrt(2.0*tcharge2) ; //page 204 of Muon TDR! 
     else *(sig+(*ic).first-1)  = da/sqrt(12.0);
     if ( L<92 ) *(sig+(*ic).first-1) = da/sqrt(12.0);
  }

}

