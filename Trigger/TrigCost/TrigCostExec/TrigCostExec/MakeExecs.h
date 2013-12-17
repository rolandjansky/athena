/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MAKEEXECS_H
#define ANP_MAKEEXECS_H

/**********************************************************************************
 * @Package: TrigCostExec
 * @Class  : MakeExecs
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Algorithm to for making trigger cost summary:
 *  - summary per algorithm: cpu time, ROB and ROS requests
 *  - summary per chain: cpu time, ROB and ROS requests
 *  - summary per sub-detector: cpu time of requesting algorithms, ROB and ROS requests
 *  - summary per ROS: cpu time of requesting algorithms, ROB and ROS requests
 *
 * Algorithm creates and executes ExecClient objects:
 *  - one instance for entire run
 *  - one instance per lumi block
 * 
 **********************************************************************************/

// C/C++
#include <map>
#include <string>

// Local
#include "TrigCostBase/AlgEvent.h"
#include "TrigCostBase/ConfigABC.h"
#include "TrigCostBase/HtmlABC.h"
#include "TrigCostBase/LumiABC.h"
#include "TrigCostBase/NtupleSvc.h"

// Local
#include "TrigCostExec/ExecBase.h"
#include "TrigCostExec/ExecData.h"

namespace Anp
{
  class MakeExecs: public virtual AlgEvent, public virtual HtmlABC {
  public:
    
    MakeExecs();
    virtual ~MakeExecs();
    
    void Config(const Registry &reg);

    void Signal(const Registry &reg, const std::string &signal);

    void Save(TDirectory *dir);

    bool Init();

    void Exec();

    void Done();

    void Write(HtmlNode &inode, HtmlMenu &hmenu);

    const Registry& GetIndexRegistry() const { return fRegIndex; }

    void Run(const TrigMonConfig &event);
    void Run(const TrigMonEvent &event); 

    const Exec::Event& GetEvent() const { return fEvent; }
    
    const std::vector<Ptr<Exec::AlgCall> > GetAlgCalls() const;

  public:

    struct LBData {
      
    LBData() :run(0), lb(0), L1Count(0), L2Count(0),lb_time(0.0) {}
      
      void ProcessAlg(const Exec::AlgCall &alg);
      
      void FinishEvent(const Exec::Event &event);
      
      unsigned    run;
      unsigned    lb;      
      unsigned    L1Count;
      unsigned    L2Count;
      double      lb_time;
      std::string lb_start;

      std::vector<Handle<ExecClient> > clients;
      Handle<ExecClient>               count;
      Exec::Summary                    summary;
    };

  private:
    
    bool PassLevel(const TrigMonEvent &event);
    void Read(const TrigMonEvent &event);
    void Fill();
    
    LBData* GetLB(unsigned run, unsigned lb);

    std::vector<Handle<ExecClient> > BookClients(const std::string &regkey,
						 const std::string &suffix,
						 const std::string &option) const;
    
    Handle<ExecClient> BookClient(const Registry &reg) const;

    Ptr<Exec::ROBConf> GetConfROB(unsigned robid);
    
    Ptr<TrigConfSeq>   GetConfSeq(unsigned index) const;
    Ptr<TrigConfAlg>   GetConfAlg(unsigned index, unsigned alg_pos) const;
    Ptr<TrigConfChain> GetConfChn(unsigned level, unsigned counter) const;        

    Exec::ROILoad &GetROILoad(const unsigned roi_id);

    std::string level() const;
    std::string ConvertUTC(const uint32_t sec, const uint32_t nsec) const;
    
    bool WriteFullHtml(unsigned lb);
    
    Handle<ExecClient> SelectLBClient(const std::vector<Handle<ExecClient> > &clients,
				      const ExecClient& parent) const;

  private:
    
    //
    // Sequence and algorithm configuration pointers
    //
    struct SeqConf {
      
      Ptr<TrigConfSeq>               conf_seq;
      std::vector<Ptr<TrigConfAlg> > conf_alg;
    };   

    typedef std::map<uint32_t, std::vector<Ptr<Exec::AlgCall> > > AlgMap;   
    typedef std::vector<Handle<ExecClient> >  ClientVec;
    typedef std::map<unsigned, LBData>        ClientMap;
    typedef std::map<unsigned, Exec::ROILoad> ROIMap;
    
  private:

    // Properties:
    std::string                        fDirName;
    std::string                        fRootFileName;

    bool                               fDebug;
    bool                               fNewMenu;      // Signal to write details of LB when there is a menu change
    bool                               fCostEvtPassL2;// Write cost events only if they pass the L2
    unsigned                           fLevel;
    unsigned                           fNLBFullHtml;  // Number of LBs with full html summary
    unsigned                           fNLBSummHtml;  // Number of LBs with full html summary

    // Variables:
    TDirectory                        *fDir;          // Directory pointer

    Anp::Registry                      fReg;          // Registry configuration
    Anp::Registry                      fRegIndex;     // Registry configuration for index page
    
    Branch<TrigMonEvent>               fEntry;        // Trigger data
    Handle<ConfigABC>                  fConf;         // Configuration algorithm
    Handle<LumiABC>                    fLumi;         // Luminosity (normalization) algorithm

    // Cost summary:
    bool                               fIsConfig;
    
    Exec::Event                        fEvent;
    Exec::Summary                      fSummary;

    AlgMap                             fAlgCall;
    ROIMap                             fROILoad;

    // Configuration:
    std::vector<SeqConf>               fConfSeq;
    std::vector<Ptr<TrigConfChain> >   fConfChainL2;
    std::vector<Ptr<TrigConfChain> >   fConfChainEF;    

    // Clients:
    ClientVec                          fClientVec;   // Clients executed on every event
    ClientMap                          fClientMap;   // Clients executed on lumi block
    ClientVec                          fSurveyVec;   // Clients executed for summary step
  };

  //
  // Inlined functions
  //
  inline Ptr<TrigConfChain> MakeExecs::GetConfChn(unsigned level, unsigned counter) const {
    //
    // Lookup pointer to trigger chain configuration
    //
    if     (level == 2 && counter < fConfChainL2.size()) return fConfChainL2.at(counter);
    else if(level == 3 && counter < fConfChainEF.size()) return fConfChainEF.at(counter);
    
    return Ptr<TrigConfChain>();
  }

  inline Ptr<TrigConfSeq> MakeExecs::GetConfSeq(unsigned index) const {
    //
    // Lookup pointer to trigger sequence configuration
    // 
    if(index < fConfSeq.size()) { 
      return fConfSeq.at(index).conf_seq;
    }
    
    return Ptr<TrigConfSeq>();
  }

  inline Ptr<TrigConfAlg> MakeExecs::GetConfAlg(unsigned index, unsigned alg_pos) const {
    //
    // Lookup pointer to trigger algorithm configuration
    // 
    if(index < fConfSeq.size()) { 
      const SeqConf &seq = fConfSeq.at(index);
      
      if(alg_pos < seq.conf_alg.size()) {
	return seq.conf_alg.at(alg_pos);
      }
    }

    return Ptr<TrigConfAlg>();
  }

  inline Exec::ROILoad &MakeExecs::GetROILoad(const unsigned roi_id) {

    ROIMap::iterator iter = fROILoad.find(roi_id);
    if(iter != fROILoad.end()){
      return iter->second;
    }

    iter=fROILoad.insert(ROIMap::value_type(roi_id,Exec::ROILoad())).first;

    return iter->second;
  }
}

#endif
