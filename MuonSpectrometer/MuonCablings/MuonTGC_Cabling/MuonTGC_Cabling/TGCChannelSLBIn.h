/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCHANNELSLBIN_HH
#define MUONTGC_CABLING_TGCCHANNELSLBIN_HH

#include "MuonTGC_Cabling/TGCChannelId.h"

namespace MuonTGC_Cabling
{

class TGCChannelSLBIn : public TGCChannelId
{
public:
  // Constructor & Destructor
  TGCChannelSLBIn(TGCIdBase::SideType side,
		   TGCIdBase::ModuleType module,
		   TGCIdBase::RegionType region,
		   int sector,
		   int id,
		   int channel);

  virtual ~TGCChannelSLBIn(void) {}

  virtual TGCModuleId* getModule(void) const;

  virtual bool isValid(void) const;

// internal structure in 200 channel of SLBIn
public:
  enum CellType {NoCellType=-1,
		 CellTrig=0,CellA=1,CellB=2,CellC=3,CellD=4,
		 MaxCellType};

  static int convertChannelInCell(int channel);
  static CellType convertCellType(int channel);
  static int convertChannelInSLB(TGCIdBase::ModuleType moduleType, 
				 CellType cellType, int channel);
  static int convertChannel(TGCIdBase::ModuleType moduleType, 
			    CellType cellType, int channelInSLB);
  static int getLengthOfCell(CellType cellType);
  static int getOffsetOfCell(CellType cellType);
  static int getLengthOfSLB(TGCIdBase::ModuleType moduleType,
			    CellType cellType);
  static int getAdjacentOfCell(CellType cellType);
  static int getAdjacentOfSLB(TGCIdBase::ModuleType moduleType,
			      CellType cellType);
  
  virtual CellType getCellType(void) const {
    return cellType;
  }
  
  virtual int getChannelInCell(void) const;

  virtual int getChannelInSLB(void) const;

  virtual void setChannel(int channel);

private:
  CellType cellType;
  int channelInCell;
  int channelInSLB;

  static const int lengthCell[];
  static const int offsetCell[];
  static const int lengthWD[];
  static const int lengthSD[];
  static const int lengthWT[];
  static const int lengthST[];
  static const int adjacentCell[];
  static const int adjacentWD[];
  static const int adjacentSD[];
  static const int adjacentWT[];
  static const int adjacentST[];

  TGCChannelSLBIn(void) {}
  
};
  
} // end of namespace
 
#endif
