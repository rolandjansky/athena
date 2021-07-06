/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigT1RPChardware_SectorL_H
#define TrigT1RPChardware_SectorL_H

#include "NOBXS.h"
#include "TrigT1RPChardware/Lvl1Def.h"
#include "MuonCablingTools/BaseObject.h"

class SectorL : public BaseObject {
public:
SectorL(int run, int event, CMAword debug, ubit16 subsys, ubit16 sect); 
~SectorL();
void load(ubit16 padAdd, ubit16 BX, ubit16 RoIAdd, ubit16 pT, ubit16 OPL,
          ubit16 overlapPhi, ubit16 overlapEta, ubit16 RoiAmbiguity, 
	  ubit16 BCIDcounter);
void execute();
ubit16 numberOfBunches(){return m_nBunMax;};
ubit16 sectorAddress(){return m_sector;};
CMAword output(ubit16 i);
CMAword outputToMuCTPI(){return output(m_nBunMax/2);};
// 
private:
int m_run;
int m_event;
ubit16 m_nBunMax;             // Maximum number of Bunch crossings to be cons.
ubit16 m_subsystem;
ubit16 m_sector;
ubit16 m_maxNumPads;
ubit16 m_numOfPads[NOBXS];
ubit16 m_padData;
ubit16 m_sectorInput[NOBXS][10][8];
ubit16 m_numberOfRoIs[NOBXS];
ubit16 m_sectorOut[NOBXS][10];
ubit16 m_pTArray[NOBXS][2][2];
CMAword m_sectorOutput[NOBXS];
void storePT(ubit16 i, ubit16 j, ubit16 k);
};//end-of-class SectorL
#endif
