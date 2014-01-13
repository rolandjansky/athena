/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_SIMLCOMB_H
#define ANP_SIMLCOMB_H

/**********************************************************************************
 * @Package: TrigCostRate
 * @Class  : SimlABC
 * @Author : Rustem Ospanov
 *
 * @Brief  : Concrete implementation of algorithm for simulating combined triggers
 * 
 **********************************************************************************/

// Trigger
#include "TrigMonitoringEvent/TrigMonConfig.h"
#include "TrigMonitoringEvent/TrigMonEvent.h"

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/Handle.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostRate/SimlABC.h"
#include "TrigCostRate/RateChain.h"

//C++
#include <list>

namespace Anp
{

  class SimulChain {

    public:
    SimulChain(unsigned ChId, std::vector<unsigned> &TEs);
    SimulChain(unsigned ChId, std::vector<unsigned> &TEs, std::vector<unsigned char> &rois);

    ~SimulChain(){};

    unsigned ChainId;
    std::vector<unsigned> TE_ids;
    std::vector<unsigned char> roi_id; 
    std::vector<bool> TE_found;
    bool xeteChain;
 
    bool isPassed();
    int isRequired(unsigned TE);
    bool compareRoi(std::vector<unsigned char> roi);
  };
    

  class CombTrig: public Base {
  public:

    CombTrig();
    ~CombTrig() {}
    
    void Clear();
//    void setDebug(bool dbug){fCombDebug = dbug;}

    bool Config(const std::vector<TrigConfChain> &ivec);

    bool SetLower(Anp::Handle<Anp::CombTrig> trig);
    bool SetConfg(const TrigConfChain &conf);
    bool SetTEs(const std::vector<TrigConfChain> &cvec);

    bool Pass(const TrigMonEvent& event, unsigned int ievent,
	      TrigMonL1Item::Decision     decisionBitLV1, // Decision bit used for LV1 rates
	      TrigMonChain::Decision      decisionBitHLT  // Decision bit used for HLT rates
	      );

    const CombTrig&      GetLower() const;
    const TrigConfChain& GetConfg() const { return fConfg; }

    uint16_t     GetEncodedId() const { return fConfg.getEncodedId(); }
    uint16_t     GetCounter()   const { return fConfg.getCounter(); }
    uint16_t     GetLevel()     const { return fLevel; }

    void resetTEcount() {fChcount = 0;}
    void resetChcount() {fTEcount = 0;}

  private:
    
    typedef Anp::Handle<Anp::CombTrig> TrigHandle;
    typedef std::vector<Anp::Handle<Anp::CombTrig> > TrigVec;

  private:

    std::vector<TrigConfChain>  fConfgVec; // Input chains
    TrigConfChain               fConfg;    // My configuration
    Anp::Handle<Anp::CombTrig>  fLower;    // My input lower level trigger
    uint16_t                    fLevel;    // Trigger level (keep for debugging)
    
    unsigned int                fIEvent;   // External current event number
    bool                        fPass;     // Passed this event?

    std::vector<SimulChain>	fRequiredTEs;
    bool			fCombDebug;
    int fTEcount;
    int fChcount;

  };


  class SimlComb: public virtual AlgEvent, public virtual SimlABC {
  public:
    
    SimlComb();
    virtual ~SimlComb();
   
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    bool Init();

    void Exec();

    void Done();
    
    const std::vector<TrigMonChain>&  GetBits() const { return fBits; }
    const std::vector<TrigConfChain>& GetConf() const { return fSimulCh; }
    
  private:   

    typedef std::vector<Anp::Handle<Anp::CombTrig> > TrigVec;
    typedef std::vector<uint16_t> IdVec;

  private:

    void Set(const TrigMonConfig &config);

    void Run(const TrigMonEvent &event);
    
    
    Anp::Handle<Anp::CombTrig> Make(std::vector<std::vector<std::string> >& combination);

  private:

    Registry                     fReg;        // Registry copy
    Branch<TrigMonEvent>         fEntry;      // Trigger data
    Handle<ConfigABC>            fConf;       // Configuration algorithm handle

    bool                         fDebug;      // Debugging flag

    // Variables:
    std::vector<TrigConfChain>   fConfgCh;    // Configured triggers
    std::vector<TrigConfChain>   fSimulCh;    // Simulated triggers
    
    bool                         fIsConfig;   // Set trigger configuration?
    unsigned                     fCounter[4]; // Current trigger counter, one per level (0 not used)
    unsigned                     fIEvent;     // Internal current event number
    
    TrigVec                      fTrig;       // Combined triggers
    std::vector<TrigMonChain>    fBits;       // Compute passed bits

    TrigMonL1Item::Decision     fDecisionBitLV1; // Decision bit used for LV1 rates
    TrigMonChain::Decision      fDecisionBitHLT; // Decision bit used for HLT rates

    std::vector<std::vector< std::vector<std::string> > > ParseXML(const std::string &fname);
  };

  //
  // Inlined member functions
  //
  inline bool SimulChain::isPassed(){ 

    for(unsigned i = 0; i < TE_found.size(); ++i){
      if(!TE_found[i]) return false;
    }
    return true;
  } 
  inline SimulChain::SimulChain(unsigned ChId, std::vector<unsigned> &TEs){
    ChainId = ChId;
    TE_ids = TEs;
    for(unsigned i = 0; i < TEs.size(); ++i){
      TE_found.push_back(0);
    }
    roi_id.clear();
  }
  inline SimulChain::SimulChain(unsigned ChId, std::vector<unsigned> &TEs, std::vector<unsigned char> &rois){
    ChainId = ChId;
    TE_ids = TEs;
    roi_id = rois;
    for(unsigned i = 0; i < TEs.size(); ++i){
      TE_found.push_back(false);
    }
  }

  inline int SimulChain::isRequired(unsigned TE){
    for(unsigned i = 0; i < TE_ids.size(); ++i){
      if(TE_ids[i] == TE) return i;
    }
    return 999;
  }
  inline bool SimulChain::compareRoi(std::vector<unsigned char> roi){

    if(roi_id.size() == 0){
      roi_id=roi;
      return true;
    }
    if(roi.size() == 1 && roi[0] == roi_id[0]) return true;
    if(roi.size() == 2){
      if(roi[0] == roi_id[0] && roi[1] == roi_id[1]) return true;
      if(roi[0] == roi_id[1] && roi[1] == roi_id[0]) return true;
    }
    return false; 
  }

}
#endif
