/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCCABLEASDTOPP_HH
#define TGCCABLEASDTOPP_HH
 
#include <string>
#include "TGCcabling12/TGCCable.h"
#include "TGCcabling12/TGCId.h"
#include "TGCcabling12/TGCDatabaseASDToPP.h" 
#include "TGCcabling12/TGCChannelASDOut.h"
#include "TGCcabling12/TGCChannelPPIn.h"

namespace LVL1TGCCabling12
{
  
class TGCCableASDToPP : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableASDToPP (std::string filename);
  
  virtual ~TGCCableASDToPP (void);

  virtual TGCChannelId* getChannel (const TGCChannelId* channelId, 
				    bool orChannel=false) const;

  void   updateDatabase(std::string&  filename);

private:
  TGCCableASDToPP (void) {}
 
  void initialize(std::string&  filename);
 
  virtual TGCChannelId* getChannelIn (const TGCChannelId* ppin, 
				      bool orChannel=false) const;
  virtual TGCChannelId* getChannelOut (const TGCChannelId* asdout,
				       bool orChannel=false) const;

  void  getUpdateInfo(const std::string filename,
		      const int side, 
		      const int sector, 
		      const std::string blockname,
		      std::vector<std::vector<int> >& info); 

  TGCDatabase* getDatabase(const int side, 
			   const int region, 
			   const int sector, 
			   const int module) const;
  void updateIndividualDatabase(const std::string filename, 
				const int side, 
				const int sector, 
				const std::string blockname, 
				TGCDatabase*& database);

  TGCDatabase* FWDdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* FSDdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* FWTdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* FSTdb[TGCIdBase::MaxSideType][TGCId::NumberOfForwardSector];
  TGCDatabase* FWIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];
  TGCDatabase* FSIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];

  TGCDatabase* EWDdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* ESDdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* EWTdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* ESTdb[TGCIdBase::MaxSideType][TGCId::NumberOfEndcapSector];
  TGCDatabase* EWIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];
  TGCDatabase* ESIdb[TGCIdBase::MaxSideType][TGCId::NumberOfInnerSector];

  // Pointers of common databases are recorded in this array
  TGCDatabase* m_commonDb[TGCIdBase::MaxRegionType][TGCIdBase::MaxModuleType];

  static const int stripForward[];
};
  
} // end of namespace
 
#endif
