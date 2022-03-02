#ifndef TGCBIS78CoincidenceMap_h
#define TGCBIS78CoincidenceMap_h

#include <vector>
#include <map>
#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "TrigT1TGC/TGCNumbering.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/MsgStream.h"

class ITGCTriggerDbTool;

namespace LVL1TGCTrigger {

class BIS78TrigOut;
class TGCArguments;

class TGCBIS78CoincidenceMap {

private:
  TGCBIS78CoincidenceMap();// hide default constructor
  enum {N_dEta=64,N_dPhi=16,N_PT_THRESH=15};// ?
  std::map<TGCRegionType,int> m_NumberOfEtaRaw={{ENDCAP,37},{FORWARD,16}}; // ?
  std::map<TGCRegionType,int> m_NumberOfRoI={{ENDCAP,148},{FORWARD,64}}; // ?
  enum {N_ROI_IN_SSC=8};
  enum {N_Endcap_SSC=19};
  enum {N_EndcapSector=48};

public:


  TGCBIS78CoincidenceMap(TGCArguments* tgcargs,const std::string& version);
  ~TGCBIS78CoincidenceMap();

  int TGCBIS78_pt(const BIS78TrigOut *bis78Out, int /*roi*/) const;

  int getFlagROI(const int roi, const int ssc, const int sec, const int side) const;

  const std::string&  getVersion() const { return m_verName; }

  // copy and assignment operator
  TGCBIS78CoincidenceMap(const TGCBIS78CoincidenceMap& right);
  bool readMap();
  TGCArguments* tgcArgs() { return m_tgcArgs;}
  const TGCArguments* tgcArgs() const { return m_tgcArgs;}

private:
  std::vector<short int> m_CW[N_dEta][N_dPhi];
  
  int m_flagROI[N_ROI_IN_SSC][N_Endcap_SSC][N_EndcapSector];
  // 1 use; 0: not use; -1: not used for Trigger

  std::string m_verName;

  int m_sector;
  TGCRegionType m_region;

  ToolHandle<ITGCTriggerDbTool> m_condDbTool;
  TGCArguments* m_tgcArgs;

  //Declaring private message stream member.
  Athena::MsgStreamMember m_msg;



};





} //end of namespace bracket

#endif // TGCBIS78CoincidenceMap_hh


