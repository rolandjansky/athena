#ifndef TGCNSWCoincidenceMap_h
#define TGCNSWCoincidenceMap_h

#include <vector>
#include <map>
#include <string>

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigT1TGC/TGCNumbering.h"

class ITGCTriggerDbTool;

namespace LVL1TGCTrigger {

class NSWTrigOut;
class TGCArguments;

class TGCNSWCoincidenceMap : public AthMessaging {

private:
  TGCNSWCoincidenceMap() = delete;// hide default constructor
  enum {N_dEta=64,N_dPhi=16,N_Dtheta=32,N_PT_THRESH=15};//6bit eta,4bit phi,5bit Dtheta, 4bit pT
  enum ReadCW_Type{EtaPhi_CW=0,EtaDtheta_CW};
  std::map<TGCRegionType,int> m_NumberOfEtaRaw={{ENDCAP,37},{FORWARD,16}};
  std::map<TGCRegionType,int> m_NumberOfRoI={{ENDCAP,148},{FORWARD,64}};

public:


  TGCNSWCoincidenceMap(TGCArguments* tgcargs,const std::string& version,int side,int oct,int mod);
  ~TGCNSWCoincidenceMap() = default;

  bool isForward( int module );

  int TGCNSW_pTcalcu_EtaPhi(const NSWTrigOut *nswOut, int RoI) const;
  int TGCNSW_pTcalcu_EtaDtheta(const NSWTrigOut *nswOut, int RoI) const;

  const std::string& getVersion() const;
  int   getSideId() const;
  int   getOctantId() const;


  // copy and assignment operator
  TGCNSWCoincidenceMap(const TGCNSWCoincidenceMap& right);
  bool readMap(std::string moduleName, ReadCW_Type cw_type);
  bool readShift();
  TGCArguments* tgcArgs() { return m_tgcArgs;}
  const TGCArguments* tgcArgs() const { return m_tgcArgs;}

private:
  std::vector<short int> m_EtaPhi_CW[N_dEta][N_dPhi];
  std::vector<short int> m_EtaDtheta_CW[N_dEta][N_Dtheta];
  std::vector<short int> m_Offset_Eta;
  std::vector<short int> m_Offset_Phi;


  std::string m_verName;
  int m_side;
  int m_octant;
  int m_module;
  int m_sector;
  TGCRegionType m_region;
  bool m_fullCW;



  ToolHandle<ITGCTriggerDbTool> m_condDbTool;
  TGCArguments* m_tgcArgs;
};





} //end of namespace bracket

#endif // TGCNSWCoincidenceMap_hh


