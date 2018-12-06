/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/* **********************************************************
*
* Quick and dirty hack to work around fact that there is no RoI
* format version at the moment. Hopefully this is a short-term
* fix which will be suppressed soon and never needed again.
*   Author: Alan Watson <Alan.Watson@cern.ch> 17-5-06
*
********************************************************** */

#include "TrigT1Interfaces/BitOp.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/RoIFormatUpdater.h"
#include "GaudiKernel/MsgStream.h"

using namespace std;

namespace LVL1 {

  unsigned int RoIFormatUpdater::UpdateEMTauRoI(unsigned int RoIWord) {
    // Check ionput is consistent with old format EMTau RoI
    if ((RoIWord&0xf0000000)==0) {
      unsigned int coord = ((RoIWord&0x0fff0000)<<2);
      unsigned int hits  = (RoIWord&0x0000ffff);
      return coord+hits;
    }
    else {
      cout <<  "EMTau RoI type not incorrect. Return RoIWord unmodified" << endl;
      return RoIWord;
    }
  }

  unsigned int RoIFormatUpdater::UpdateJetEnergyRoI(unsigned int RoIWord) {
    // What type of JetEnergy RoI is it?
    unsigned int type = OldJetEnergyType(RoIWord);
    
    if (type == TrigT1CaloDefs::JetRoIWordType) {
      return UpdateJetRoI(RoIWord);
    }
    else if (type == TrigT1CaloDefs::EnergyRoIWordType0) {
      return UpdateEnergyRoI0(RoIWord);
    }
    else if (type == TrigT1CaloDefs::EnergyRoIWordType1) {
      return UpdateEnergyRoI1(RoIWord);
    }
    else if (type == TrigT1CaloDefs::EnergyRoIWordType2) {
      return UpdateEnergyRoI2(RoIWord);
    }
    else if (type == TrigT1CaloDefs::JetEtRoIWordType) {
      return UpdateJetEtRoI(RoIWord);
    }
    else { 
      cout << "JetEnergy RoI type not recognised. Return RoIWord unmodified" << endl;
      return RoIWord;
    }
  }

  unsigned int RoIFormatUpdater::OldJetEnergyType(unsigned int RoIWord)  {
    if ( BitOp::getValue( &RoIWord, 0xf0000000 ) == 1) {
      return TrigT1CaloDefs::JetRoIWordType;
    }
    else if ( BitOp::getValue( &RoIWord, 0xf0000000 ) == 3) {
      int subType = BitOp::getValue( &RoIWord, 0x0c000000 );
      if (subType == 0) {
        return TrigT1CaloDefs::EnergyRoIWordType0;
      }
      else if (subType == 1) {
        return TrigT1CaloDefs::EnergyRoIWordType1;
      }
      else if (subType == 2) {
        return TrigT1CaloDefs::EnergyRoIWordType2;
      }
    }
    else if ( BitOp::getValue( &RoIWord, 0xf0000000 ) == 2) {
      return TrigT1CaloDefs::JetEtRoIWordType;
    }
    return 9999; // not recognised as an old-style JetEnergy RoI
  }
      
  unsigned int RoIFormatUpdater::UpdateJetRoI(unsigned int RoIWord)  {
    unsigned int header = (TrigT1CaloDefs::jetRoIType<<30) +  (TrigT1CaloDefs::jetRoI<<29);
    unsigned int hits  = (RoIWord&0x00000fff);
    unsigned int fwdjet(0);
    if (RoIWord&0x00000f00) fwdjet = 1<<18;
    
    // That was the easy bit. Unfortunately the coordinate format has
    // also changed. This does make sense, but it isn't pretty!
    unsigned int crate = BitOp::getValue( &RoIWord, 0x00200000);
    unsigned int jem   = BitOp::getValue( &RoIWord, 0x001e0000);
    unsigned int row   = BitOp::getValue( &RoIWord, 0x0001c000);
    unsigned int col   = BitOp::getValue( &RoIWord, 0x00003000);
    unsigned int frame = 4*(col>>1) + (row>>1);
    unsigned int rl    = 2*(row%2) + (col%2);
    unsigned int coord = (crate<<9) + (jem<<5) +(frame<<2) + rl;
    coord = (coord<<19);
    
    return header+coord+fwdjet+hits;
  }

  unsigned int RoIFormatUpdater::UpdateJetEtRoI(unsigned int RoIWord)  {
    unsigned int header = (TrigT1CaloDefs::jetRoIType<<30) + (TrigT1CaloDefs::jetEtRoI<<29);
    unsigned int hits  = (RoIWord&0x0000000f);
    return header+hits;
  }

  unsigned int RoIFormatUpdater::UpdateEnergyRoI0(unsigned int RoIWord)  {
    unsigned int header = (TrigT1CaloDefs::energyRoIType<<30) + (TrigT1CaloDefs::energyRoI0<<28);
    unsigned int Ex     = (RoIWord&0x0000ffff);
    return header+Ex;
  }

  unsigned int RoIFormatUpdater::UpdateEnergyRoI1(unsigned int RoIWord)  {
    unsigned int header = (TrigT1CaloDefs::energyRoIType<<30) + (TrigT1CaloDefs::energyRoI1<<28);
    unsigned int hits   = (RoIWord&0x000f0000);
    unsigned int Ey     = (RoIWord&0x0000ffff);
    return header+hits+Ey;
  }

  unsigned int RoIFormatUpdater::UpdateEnergyRoI2(unsigned int RoIWord)  {
    unsigned int header = (TrigT1CaloDefs::energyRoIType<<30) + (TrigT1CaloDefs::energyRoI2<<28);
    unsigned int hits   = (RoIWord&0x00ff0000);
    unsigned int Et     = (RoIWord&0x0000ffff);
    return header+hits+Et;
  }

} // namespace ROIB
