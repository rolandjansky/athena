/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_OVERLAPMON_H
#define ANP_OVERLAPMON_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : OverlapMon
 * @Author : Elliot Lipeles
 *
 * @Brief  : Algorithm for computing stream overlaps for online cost data
 *
 **********************************************************************************/

// C/C++
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/ScaleABC.h"

namespace Anp
{
  class OverlapMon: public virtual AlgEvent, public virtual HtmlABC {
  public:

    OverlapMon();
    virtual ~OverlapMon();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();

    void DoConfiguration(const TrigMonConfig &config);

    double GetEffectivePS(const unsigned level, const unsigned counter);
    double TriggerOverlap(const double ps1, const double ps2);

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    void WriteChn(HtmlNode &inode, const unsigned index, const std::string key="");

    void WriteChnWithPS(HtmlNode &inode, const unsigned index, const std::string key="");

    const Registry& GetIndexRegistry() const {return fRegIndex; }

    typedef std::pair< unsigned int , unsigned int >            counter_doublet;
    typedef std::pair< std::string  , std::string  >            string_doublet;
    typedef std::map < counter_doublet , unsigned int >         doublet_map;
    typedef std::map < counter_doublet , double >               doublet_double_map;
    typedef std::map < string_doublet , unsigned int >          str_doublet_map;
    typedef std::map < std::string, std::string >               str2str_map;
    typedef std::map < std::string, std::vector<std::string> >  str2strvec_map;
    typedef std::map < unsigned int, std::string >              int2str_map;
    typedef std::map < std::string, unsigned int >              str2int_map;
    typedef std::map < unsigned int, float >                    int2float_map;

    typedef std::map < std::string , bool >                     str2bool_map;
    typedef std::map < unsigned int, std::set<std::string> >    int2set_map;

  private:

    Anp::Registry               fReg;            // Registry configuration
    Anp::Registry               fRegIndex;       // Registry configuration for index page

    bool                        fIsConfig;       // Is class properly configured?
    Handle<ConfigABC>           fConf;           // Configuration algorithm

    Branch<TrigMonEvent>        fEntry;          // Trigger cost monitoring data for one event    
    TrigMonConfig               fConfig;

    // Counters
    doublet_map                 fCounters[3];     // Map for each level from pair of chain counters 
                                                  // to an event counter   
    doublet_double_map          fScaleCount[3];   // Map for each level from pair of chain counters 
                                                  // to an event counter - this is a double for scaleBG
    int2float_map               fPrescale[3];     // Map from chain counter to chain prescale
    str_doublet_map             fStreamCounts;    // Map of pair of streams to overlap
    str_doublet_map             fStreamChainOvlp; // Map of pair of streams to overlap
    int2set_map                 fLowerChains[3];  // Map to ouput chains

    // Configuration maps
    str2strvec_map              fStream2Chains;    // Stream to vector of chains map
    str2strvec_map              fChain2Streams;    // Chain  to Stream map
    str2strvec_map              fBWGrp2Chains;     // BW group to vector of chains map
    str2strvec_map              fChain2BWGrps;     // Chain  to BW group map
    int2str_map                 fCounter2Chain[3]; // Counter to chain map for each level
    str2int_map                 fChain2Counter[3]; // Chain to count map for each level

    // Active Streams for event
    str2bool_map                fActiveStreams;    // map from stream to a bool which is true if 
                                                   // stream is active for this event
                                                   // stream and BW group are not differentiated here
    void     ResetActiveStreams();
    void     SetActiveStreams(const TrigMonChain &chain);

    // currently hardcoded list of streams to skip in output
    bool     SkipStream(std::string str); 

    // insert string into vector if not already there
    void     InsertUniqueString(std::vector<std::string>& strvec, const std::string& str); 

    // write html pages for group/stream in the grp2chn map
    void AddGroupOverlap(Handle<HtmlNode>  top, std::string       gname, str2strvec_map&   grp2chn);

    // Properties:
    bool                        fDebug;          // Print debugging info
    bool                        fDebugDetails;   // More detailed debuging info
    bool                        fWithPS;         // Compute the overlaps with prescales
    unsigned                    fLevel;

    Handle<ScaleABC>           fScaleBG;        // BG scaling algorithm
  };
}

#endif
