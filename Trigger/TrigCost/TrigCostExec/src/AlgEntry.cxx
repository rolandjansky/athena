/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Local
#include "TrigCostExec/AlgEntry.h"

using namespace std;

//--------------------------------------------------------------------------------------      
Anp::ROBEntry::ROBEntry()
  :nreq(0),
   time(0)
{
}

//--------------------------------------------------------------------------------------      
void Anp::ROBEntry::Clear()
{
  nreq  = 0;
  time  = 0;
  ratio = Ratio();
  robs.clear();
  rids.clear();
}

//--------------------------------------------------------------------------------------      
void Anp::ROBEntry::Print()
{
  cout << "nreq=" << nreq << " time=" << time;

  for(std::map<TrigMonROBData::History, ROB>::const_iterator it = robs.begin(); it != robs.end(); ++it) {
    cout << " hist=" << it->first
	 << " nrob=" << it->second.nrob
	 << " size=" << it->second.size;
  }
  
  cout << endl;
}

//--------------------------------------------------------------------------------------      
void Anp::ROBEntry::AddMonROB(const TrigMonROB &rob)
{
  //
  // Add single ROB request
  //
  nreq++;
  time += rob.getTimer();

  const std::vector<TrigMonROBData> &rvec = rob.getData();
  const std::vector<TrigMonROBSum>   svec = rob.getSum();
  
  double count_nids = 0, count_nids_cache = 0, count_nids_retrv = 0;
  double count_size = 0, count_size_cache = 0, count_size_retrv = 0;

  //
  // Zero out data for ratios after previous Add() call
  //
  ratio.nids = 0;
  ratio.size = 0;

  for(unsigned int i = 0; i < rvec.size(); ++i) {    
    const TrigMonROBData &data = rvec.at(i);
    const unsigned size = data.getROBSize();

    ROB &rob = robs[data.getHistory()];
    
    rob.nrob++;
    rob.size+= size;
    
    count_nids++;
    count_size += data.getROBSize();
    
    rids.push_back(data.getROBId());

    if(data.getHistory() == TrigMonROBData::kRETRIEVED) { 
      count_nids_retrv++;
      count_size_retrv+= size;
    }
    else if(data.getHistory() == TrigMonROBData::kCACHED) { 
      count_nids_cache++;
      count_nids_cache+= size;
    }
  }
  
  for(unsigned int i = 0; i < svec.size(); ++i) {
    const TrigMonROBSum &sum = svec[i];
    
    ROB &rob = robs[sum.getHistory()];
    
    rob.nrob += sum.getNROB();
    rob.size += sum.getSize();

    count_nids += sum.getNROB();
    count_size += sum.getSize();

    if(sum.getHistory() == TrigMonROBData::kRETRIEVED) { 
      count_nids_retrv += sum.getNROB();
      count_size_retrv += sum.getSize();
    }
    else if(sum.getHistory() == TrigMonROBData::kCACHED) { 
      count_nids_cache += sum.getNROB();
      count_size_cache += sum.getSize();
    }
  }

  if(count_nids_retrv > 0.0) {
    ratio.nids       = count_nids_retrv;
    ratio.nids_retrv = count_nids_retrv/count_nids;
    ratio.nids_cache = count_nids_cache/count_nids;
    ratio.nids_other = (count_nids - count_nids_retrv - count_nids_cache)/count_nids;
  }

  if(count_size_retrv > 0.0) {
    ratio.size       = count_size_retrv;
    ratio.size_retrv = count_size_retrv/count_size;
    ratio.size_cache = count_size_cache/count_size;
    ratio.size_other = (count_size - count_size_retrv - count_size_cache)/count_size;
  }
}

//--------------------------------------------------------------------------------------      
void Anp::ROBEntry::AddOneROB(const ROBEntry &rhs)
{
  //
  // Add another entry
  //
  nreq += rhs.nreq;
  time += rhs.time;

  for(std::map<TrigMonROBData::History, ROB>::const_iterator it = rhs.robs.begin(); 
      it != rhs.robs.end(); ++it) {    

    std::pair<std::map<TrigMonROBData::History, ROB>::iterator, bool> pit =
      robs.insert(std::pair<TrigMonROBData::History, ROB>(it->first, it->second));

    if(!pit.second) {
      pit.first->second.nrob += it->second.nrob;
      pit.first->second.size += it->second.size;
    }
  }
}

//--------------------------------------------------------------------------------------      
unsigned Anp::ROBEntry::GetNROB() const
{
  //
  // Return number of ROB ids
  //
  unsigned nrob = 0;

  for(std::map<TrigMonROBData::History, ROB>::const_iterator it = robs.begin();
      it != robs.end(); ++it) {
    nrob += it->second.nrob;
  }

  return nrob;
}

//--------------------------------------------------------------------------------------      
double Anp::ROBEntry::GetSize() const
{
  //
  // Return ROB size in kBytes
  //
  double size = 0;

  for(std::map<TrigMonROBData::History, ROB>::const_iterator it = robs.begin();
      it != robs.end(); ++it) {
    size += it->second.size;
  }

  return 4.0*size/1024.0;
}

//--------------------------------------------------------------------------------------      
unsigned Anp::ROBEntry::GetNROB(TrigMonROBData::History hist) const
{
  std::map<TrigMonROBData::History, ROB>::const_iterator it = robs.find(hist);
  if(it != robs.end()) return it ->second.nrob;
  return 0;
}

//--------------------------------------------------------------------------------------      
double Anp::ROBEntry::GetSize(TrigMonROBData::History hist) const
{
  //
  // Return ROB size in kBytes with given history
  //
  std::map<TrigMonROBData::History, ROB>::const_iterator it = robs.find(hist);
  if(it != robs.end()) return 4.0*double(it->second.size)/1024.0;
  return 0;
}

//--------------------------------------------------------------------------------------      
unsigned Anp::ROBEntry::GetNROBOther() const
{
  //
  // Return number of non-cached or non-retrieved robs
  //
  return GetNROB()-GetNROB(TrigMonROBData::kRETRIEVED)-GetNROB(TrigMonROBData::kCACHED);
}

//--------------------------------------------------------------------------------------      
double Anp::ROBEntry::GetSizeOther() const
{
  //
  // Return number of non-cached or non-retrieved robs
  //
  return GetSize()-GetSize(TrigMonROBData::kRETRIEVED)-GetSize(TrigMonROBData::kCACHED);
}

//--------------------------------------------------------------------------------------      
Anp::AlgEntry::AlgEntry()
  :eta(-1.0e6),
   phi(-1.0e6)
{
}

//--------------------------------------------------------------------------------------      
Anp::AlgEntry::AlgEntry(const TrigMonAlg &alg, float eta_, float phi_)
  :eta      (eta_),
   phi      (phi_),
   alg_start(alg.start()),
   alg_stop (alg.stop()),
   roi      (alg.getRoiIdSet())
{
}

//--------------------------------------------------------------------------------------      
void Anp::AlgEntry::Clear()
{
  rob.Clear();
  roi.clear();

  alg_start = TrigMonTimer(0);
  alg_stop  = TrigMonTimer(0);
}

//--------------------------------------------------------------------------------------      
bool Anp::AlgEntry::FillRoiROB(const TrigMonROB &mon)
{
  //
  // Fill algorithm data
  //  
  if(mon.start().getEncoded() > alg_start.getEncoded() && 
     mon.stop().getEncoded()  < alg_stop.getEncoded()) {
    rob.AddMonROB(mon);
    return true;
  }
  else if(false) {
    cout << "AlgEntry::Fill " << endl
	 << "   start rob, alg: " << mon.start().getEncoded() << ", " << alg_start.getEncoded() << endl
	 << "   stop  rob, alg: " << mon.stop() .getEncoded() << ", " << alg_stop .getEncoded() << endl;
    return false;
  }
  
  return false;
}
