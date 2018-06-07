///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArNoisyROTool.h 
// Header file for class LArNoisyROTool
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef LARCELLREC_LARNOISYROTOOL_H
#define LARCELLREC_LARNOISYROTOOL_H 1

// STL includes
#include <string>
#include <set>
#include <array>
#include <unordered_map>

// FrameWork includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloInterface/ILArNoisyROTool.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "AthenaKernel/IOVSvcDefs.h"

class LArOnlineID;
class CaloCell_ID;
class LArCablingService;
class LArNoisyROSummary;
class CaloCellContainer;

class LArNoisyROTool: 
  virtual public ILArNoisyROTool,
  public AthAlgTool
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  LArNoisyROTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~LArNoisyROTool(); 

  static const InterfaceID& interfaceID();

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  virtual 
  std::unique_ptr<LArNoisyROSummary> process(const CaloCellContainer*);

 private: 

  /// Default constructor: 
  LArNoisyROTool();

  // Containers

  //enum LARFLAGREASON { BADFEBS=0, MEDIUMSATURATEDQ=1, TIGHTSATURATEDQ=2, BADFEBS_W=3} ;

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

  size_t partitionNumber(const HWIdentifier);


  typedef std::unordered_map<unsigned int, FEBEvtStat> FEBEvtStatMap;
  typedef std::unordered_map<unsigned int, FEBEvtStat>::iterator FEBEvtStatMapIt;
  typedef std::unordered_map<unsigned int, FEBEvtStat>::const_iterator FEBEvtStatMapCstIt;

 private: 
  std::string m_CaloCellContainerName;
  std::string m_outputKey;
  //FEBEvtStatMap m_FEBstats;

  const CaloCell_ID* m_calo_id;
  const LArOnlineID* m_onlineID;
  ToolHandle<LArCablingService> m_cablingService;

  //** Qfactor value above which a channel is considered bad */
  unsigned int m_CellQualityCut;

  //** ignore masked cells ? */
  bool m_ignore_masked_cells;

  //** ignore front inner wheel cells ? */
  bool m_ignore_front_innerwheel_cells;

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
  std::vector<unsigned int> m_knownBadFEBsVec;

  //** Same as above but as set to increase search speed
  std::set<unsigned int> m_knownBadFEBs;

  //** List of FEBs known to be affected by mini Noise Bursts (jobO) */
  std::vector<unsigned int> m_knownMNBFEBsVec;

  //* List of FEBs known to be affected by mini Noise Bursts. Using a set to avoid duplication */
  std::set<HWIdentifier> m_knownMNBFEBs;

  //** count bad FEB for job */
  std::unordered_map<unsigned int, unsigned int> m_badFEB_counters;

  //** count bad PA for job */
  std::map<uint64_t, unsigned int> m_badPA_counters;

  //** event counter */
  unsigned int m_invocation_counter;

  //** Qfactor value above which (>=) a channel is considered with a saturated Qfactor*/
  unsigned int m_SaturatedCellQualityCut;

  //** Count saturated Qfactor cells above this energy cut (absolute value)*/
  float m_SaturatedCellEnergyTightCut;

  //** min number of saturated Qfactor cells to declare an event bad */
  unsigned int m_SaturatedCellTightCut;

  //** Count events with too many saturated Qfactor cells */
  unsigned int m_SaturatedCellTightCutEvents;

  float m_MNBLooseCut;
  float m_MNBTightCut;

  //** jobO flag to turn on a summary printout in finalize
  bool m_printSummary;


  std::array<uint8_t,4> m_partitionMask;

}; 




inline size_t LArNoisyROTool::partitionNumber(const HWIdentifier hwid) {

  int pn=m_onlineID->pos_neg(hwid);
  if (m_onlineID->isEMECchannel(hwid)) {
    if (pn) 
      return 0; //positive EMECA side
    else
      return 3; //negative EMECC side
  }
  if (m_onlineID->isEMBchannel(hwid)) {
    if (pn) 
      return 1; //positive EMBA side
    else
      return 2; //negative EMBC side
  }

  return 4;//Anything else
}




#endif //> !LARCELLREC_LARNOISYROTOOL_H
