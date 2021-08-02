/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArSamplesFilterParams
   @brief storage of a collection of digits for one cell
*/

#ifndef LArSamples_FilterParams_H
#define LArSamples_FilterParams_H

#include <vector>
#include <map>
#include <cfloat>
#include "LArCafJobs/CaloId.h"
#include "LArSamplesMon/Data.h"
#include "CaloIdentifier/CaloGain.h"


namespace LArSamples {
  
  typedef std::pair<int, int> UIntRange;
  class History;
  class Data;
  class CellInfo;
  
  struct ChannelSpec {
    ChannelSpec(CaloId cal, unsigned int ft, unsigned int s, unsigned int c) :
        calo(cal), feedThrough(ft), slot(s), channel(c) { }
    bool match(const CellInfo& info) const;
    CaloId calo;
    int feedThrough, slot, channel;
  };
  
#ifndef __GCCXML__
  struct FuncCut {
    FuncCut(const DataFuncSet& f = DataFuncSet(), const DataFuncArgs& a = DataFuncArgs()) 
      : func(f), args(a), min(-DBL_MAX), max(DBL_MAX) { }
    bool pass(const Data& data) const;
    bool gotMin() const { return min > -DBL_MAX/2; }
    bool gotMax() const { return max < +DBL_MAX/2; }
    void setEqual(double val) { min = val; max = val - 1; }
    DataFuncSet func;
    DataFuncArgs args;
    double min, max;
  };
#endif

  class FilterParams  {
  
   public:
   
    FilterParams() : 
      m_status(0xffffffff), m_requireSat(0),
      m_requireConvergence(false), m_channelSel(0), m_indexParity(0) { }

    FilterParams(const FilterParams& other);

    bool set(const char* selection);

    void addRunRange(unsigned int run1, unsigned int run2) { m_runs.push_back(UIntRange(run1, run2)); }
    void addEventRange(unsigned int event1, unsigned int event2) { m_events.push_back(UIntRange(event1, event2)); }
    void addLBRange(unsigned int lb1, unsigned int lb2) { m_lbs.push_back(UIntRange(lb1, lb2)); }
    void addHashIdRange(unsigned int hashId1, unsigned int hashId2) { m_hashIds.push_back(UIntRange(hashId1, hashId2)); }
    
    void addRun(unsigned int run) { m_runs.push_back(UIntRange(run, run + 1)); }
    void addEvent(unsigned int event) { m_events.push_back(UIntRange(event, event + 1)); }
    void addLB(unsigned int lb) { m_lbs.push_back(UIntRange(lb, lb + 1)); }
    void addHashId(unsigned int hashId) { m_hashIds.push_back(UIntRange(hashId, hashId + 1)); }
    void addLayer(unsigned int layer) { m_layers.push_back(layer); }
    void addGain(CaloGain::CaloGain gain) { m_gains.push_back(gain); }

    void addRunEventFromData(const Data& data);
    
    void addChannel(CaloId calo, int feedthrough, int slot, int channel);
    void addChannels(const char* fileName); // bad channels list in usual format: 1 per line, | barrel_ec | pos_neg | ft | slot | channel | pb | 
    void setChannelVeto(short veto = -1) { m_channelSel = veto; }  // apply channelList as Veto (true) or positive filter (false)

    void addCalo(CaloId calo) { m_calos.push_back(calo); }
    void addPartition(PartitionId part) { m_partitions.push_back(part); }
    void setStatus(unsigned int status) { m_status = status; }
    
    void setRequireConvergence(bool conv = true) { m_requireConvergence = conv; }
    void setRequireSaturation  (bool flag = true) { m_requireSat = m_requireSat | (flag ? 1 : 0); }
    void setRequireNoSaturation(bool flag = true) { m_requireSat = m_requireSat | (flag ? 12 : 0); }
    void setIndexParity(short parity) { m_indexParity = parity; }

    //bool pass(unsigned int hashId, CaloId calo, 
    //          unsigned int run, unsigned int event, unsigned int status, 
    //          double cellEnergy, double adcMax, double sampleMax) const;
    //bool passEvent(unsigned int run, unsigned int event, unsigned int status, double cellEnergy, double adcMax, double sampleMax) const;
    //bool passCell(unsigned int hashId, CaloId calo) const;
  
    bool pass(unsigned int hashId, const History& history, unsigned int k) const;
    bool passHash(unsigned int hashId) const;
    bool passCell(const CellInfo& info) const;
    bool passEvent(const Data& data) const;
  
    static const short m_saturation = 3800;
    
    static bool splitToken(const TString& token, TString& var, TString& op, TString& arg);
    TString processToken(const TString& var, const TString& op, const TString& arg, const TString& oring = "");

  private:
     
    std::vector<UIntRange> m_runs, m_events, m_hashIds, m_lbs;
    std::vector<CaloId> m_calos;
    std::vector<PartitionId> m_partitions;
    std::vector<ChannelSpec> m_channels;
    std::vector<int> m_layers;
    std::vector<CaloGain::CaloGain> m_gains;
    unsigned int m_status, m_requireSat;
    
#ifndef __GCCXML__
    std::map<TString, FuncCut> m_cuts;
#endif    
    
    bool m_requireConvergence;
    int m_channelSel; // +1 for selection, -1 for veto;
    short m_indexParity;
  };
}
#endif
