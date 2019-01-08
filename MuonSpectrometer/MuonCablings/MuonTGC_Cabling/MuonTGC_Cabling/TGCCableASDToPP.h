/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEASDTOPP_HH
#define MUONTGC_CABLING_TGCCABLEASDTOPP_HH
 
#include "MuonCondInterface/ITGCCablingDbTool.h"

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "MuonTGC_Cabling/TGCCable.h"
#include "MuonTGC_Cabling/TGCId.h"

class StatusCode;

namespace MuonTGC_Cabling
{
  
class TGCDatabase;

class TGCCableASDToPP : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableASDToPP (std::string filename);
  
  virtual ~TGCCableASDToPP (void);

  virtual TGCChannelId* getChannel (const TGCChannelId* channelId, 
				    bool orChannel=false) const;

  StatusCode updateDatabase();

private:
  TGCCableASDToPP (void) {}
 
  void initialize(std::string&  filename);
 
  virtual TGCChannelId* getChannelIn (const TGCChannelId* ppin, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* asdout,
				       bool orChannel=false) const;

  StatusCode getUpdateInfo(const int side,  
			   const int sector,  
			   const std::string blockname, 
			   std::vector<std::vector<int> >& info); 

  TGCDatabase* getDatabase(const int side,  
			   const int region,  
			   const int sector,  
			   const int module) const; 

  StatusCode updateIndividualDatabase(const int side,  
				      const int sector,  
				      const std::string blockname,  
				      TGCDatabase*& database);

  TGCDatabase* m_FWDdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* m_FSDdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* m_FWTdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* m_FSTdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* m_FWIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];
  TGCDatabase* m_FSIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];

  TGCDatabase* m_EWDdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* m_ESDdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* m_EWTdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* m_ESTdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* m_EWIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];
  TGCDatabase* m_ESIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];

  static const int s_stripForward[];

  ToolHandle<ITGCCablingDbTool> m_tgcCablingDbTool;
  std::vector<std::string> *m_ASD2PP_DIFF_12;

  /** Pointers of common databases are recorded in this array */
  TGCDatabase* m_commonDb[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];
};
  
} // end of namespace
 
#endif
