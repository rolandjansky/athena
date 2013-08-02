/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCMODULESLB_HH
#define MUONTGC_CABLING_TGCMODULESLB_HH
 
#include "MuonTGC_Cabling/TGCModuleId.h"
 
namespace MuonTGC_Cabling
{
  
class TGCModuleSLB : public TGCModuleId
{
public:
  // Constructor & Destructor
  TGCModuleSLB(TGCIdBase::SideType side,
	       TGCIdBase::ModuleType module,
	       TGCIdBase::RegionType region,
	       int sector,
	       int id,
	       int sbLoc = -1,
	       int slbAddr = -1);
  
  virtual ~TGCModuleSLB(void) {}

  virtual bool isValid(void) const;

  // special method for SLB
  int  getSBLoc(void) const { return m_sbLoc; }   
  int  getSlbAddr(void) const { return m_slbAddr; }   

private:
  TGCModuleSLB(void) {}

private:
  int m_sbLoc;
  int m_slbAddr;

};
  
} // end of namespace
 
#endif
