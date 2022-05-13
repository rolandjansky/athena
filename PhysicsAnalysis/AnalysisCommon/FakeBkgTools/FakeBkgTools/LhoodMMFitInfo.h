#ifndef Lhood_MM_FitInfo_h
#define Lhood_MM_FitInfo_h

#include <vector>
#include <string>
#include "FakeBkgTools/FakeBkgInternals.h"

class TDirectory;
class TTree;

namespace CP
{
  struct LhoodMMFitInfo{
    
    LhoodMMFitInfo(int nlep = 1);

    std::vector< std::vector < std::vector< FakeBkgTools::Efficiency> > > coeffs_num;
    std::vector < std::vector< FakeBkgTools::Efficiency> > normterms;
    std::vector<std::vector<double> > event_cat;
    std::vector<std::vector<double> > event_sumw2;
    std::vector< std::vector < double > > OSfrac_num;
    std::vector< std::vector < double > >  OSfrac_denom;
    int totEvents;
    std::vector<double> eventCount;
    void add(LhoodMMFitInfo& rhs, int nLepMax);
    void resizeVectors(unsigned nlep);
    void reset();

 };

}

#endif
