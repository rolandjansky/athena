/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_RATEBAND_H
#define ANP_RATEBAND_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : RateBand
 * @Author : Josh Kunkle
 *
 * @Brief  : Class for computing group bandwith
 * 
 **********************************************************************************/

#include <vector>
#include <string>
#include <set>

// Local
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/HtmlNode.h"

#include "TrigCostRate/MakeNodes.h"


namespace Anp
{
  class Registry;
  class RateABC;

  class RateBand: public virtual Base {
  public:

    RateBand();
    virtual ~RateBand();
    
    void ConfigReg(const Registry &reg);

    void SetRateAlg(RateABC &rate);
   
    void Finalize(Registry &reg);

    void AddConfiguredGroup(Handle<RateGroup> group) {fOlapGroups.push_back(group); }

    std::map<std::string, std::vector<Handle<RateChain> > >         GetRateGroups() {return fbwOlaps; } 
    std::vector<std::pair<std::string, std::string> >               GetBwCorr()     { return fbwCorr; }
    std::vector<std::pair<std::vector<std::string>, std::string > > GetBwOlaps()    { return fOlapCalc; }
    
  private :

    typedef std::map<std::string, std::vector<Handle<RateChain> > > GroupMap;
  
  private:

    void MakeOverlaps(int order, GroupMap & OlapGroups,  
		      std::vector<Handle<RateChain> > groupChains = std::vector<Handle<RateChain> >(),
                      const std::string & name = "",
		      int index = 0);

    void Tokenize(const std::string &name, std::set<std::string> &) const;
  
    int FindOrder(const std::string &name) const;

    double GetCorrFrac(const std::string &name1, const std::string &name2) const;

  private:
   
    Registry                                            fReg;
    
    bool                                                fDebug;
    std::string                                         fOlapKey;    // key to identify bandwidth groups
    std::vector<Handle<RateGroup> >                     fGroups;     // groups from RateAlg
    std::vector<Handle<RateGroup> >                     fOlapGroups; // configured groups, passed from RateAlg
    GroupMap                                            fbwOlaps;    // created group names, chains
    std::vector<Handle<RateGroup> >                     fUnions;     // unions from RateAlg
    std::vector< std::pair<std::string,std::string> >   fbwCorr;     // corrected rates
    std::vector<std::pair<std::vector<std::string>, std::string> > fOlapCalc; //overlap rates
  };
}

#endif
