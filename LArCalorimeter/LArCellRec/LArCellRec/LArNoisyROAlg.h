/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARNOISYROALG_H
#define LARNOISYROALG_H



/** 
@class LArNoisyROAlg
@brief Find list of suspicious preamplifiers and Front End Boards from cell collection

 Created September 28, 2009  L. Duflot
 Modified May, 2014 B.Trocme 
 - Remove saturated medium cut
 - Create a new weighted Std algorithm

*/



#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CxxUtils/unordered_map.h"


#include <string>
#include <map>


class CaloCell_ID;
class LArOnlineID;
class LArCablingService;

class LArNoisyROAlg : public AthAlgorithm
{
 public:

  LArNoisyROAlg(const std::string &name,ISvcLocator *pSvcLocator);
  virtual StatusCode initialize();
  virtual StatusCode execute();   
  virtual StatusCode finalize();
  virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

  enum LARFLAGREASON { BADFEBS=0, MEDIUMSATURATEDQ=1, TIGHTSATURATEDQ=2, BADFEBS_W=3} ;

 private:  // classes

  // this class accumulates the number of bad channel in a FEB, per preamp in a FEB
  class FEBEvtStat
  {
  public:
    FEBEvtStat()
    {
      resetCounters();
    }

    void addBadChannel(unsigned int channel)
    {
      m_chanCounter++;
      unsigned int preamp = channel/4;
      m_PAcounters[preamp]++;
    }

    void resetCounters()
    {
      m_chanCounter = 0;
      for ( size_t i = 0; i < 32; i++ ) m_PAcounters[i] = 0;
    }

    unsigned int badChannels() const { return m_chanCounter; }
    const unsigned int* PAcounters() const { return &m_PAcounters[0]; }
  private:
    unsigned int m_chanCounter;
    unsigned int m_PAcounters[32];

  };


  typedef SG::unordered_map<unsigned int, FEBEvtStat> FEBEvtStatMap;
  typedef SG::unordered_map<unsigned int, FEBEvtStat>::iterator FEBEvtStatMapIt;
  typedef SG::unordered_map<unsigned int, FEBEvtStat>::const_iterator FEBEvtStatMapCstIt;



 private:  // methods
  
  // mask default constructor, copy constructor & operator
  LArNoisyROAlg();
  LArNoisyROAlg(const LArNoisyROAlg&);
  LArNoisyROAlg& operator=(const LArNoisyROAlg&);

 private:  // members
  bool knownFEB (unsigned int hid, const std::vector<unsigned int>& v_feb) const;

  std::string m_CaloCellContainerName;
  std::string m_outputKey;
  FEBEvtStatMap m_FEBstats;

  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;
  ToolHandle<LArCablingService> m_cablingService;

  //** Qfactor value above which a channel is considered bad */
  unsigned int m_CellQualityCut;

  //** ignore masked cells ? */
  bool m_ignore_masked_cells;

  //** number of bad channels to declare a preamp noisy */
  unsigned int m_BadChanPerPA;

  //** number of bad channels to declare a FEB noisy */
  unsigned int m_BadChanPerFEB;

  //** min number of bad FEB to put LAr warning in event info */
  unsigned int m_MinBadFEB;

  //** list of FEBs known to be affected by NoiseBursts */
  //** If the number of BAD FEBs (counted with this weight 2) is greater than the m_MinBadFEB, the */
  //** flag BADFEBS_W is set to true */
  //** Example with the (default) value of 5 (strictly greater than), the following combination will lead to a BADFEBS_W */
  //** >=5 (including 1 known bad FEB), >=4 (including 2 known bad FEB), >=3 (including 3 known bad FEB). */
  std::vector<unsigned int> m_knownBadFEBs;

  //** count bad FEB for job */
  SG::unordered_map<unsigned int, unsigned int> m_badFEB_counters;

  //** count bad PA for job */
  std::map<uint64_t, unsigned int> m_badPA_counters;

  //** event counter */
  unsigned int m_event_counter;

  //** Qfactor value above which (>=) a channel is considered with a saturated Qfactor*/
  unsigned int m_SaturatedCellQualityCut;

  //** Count saturated Qfactor cells above this energy cut (absolute value)*/
  float m_SaturatedCellEnergyTightCut;

  //** min number of saturated Qfactor cells to declare an event bad */
  unsigned int m_SaturatedCellTightCut;

  //** Count events with too many saturated Qfactor cells */
  unsigned int m_SaturatedCellTightCutEvents;


};


#endif
