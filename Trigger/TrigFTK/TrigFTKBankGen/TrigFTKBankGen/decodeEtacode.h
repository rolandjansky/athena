/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGFTKBANKGEN_DECODEETACODE_H
#define TRIGFTKBANKGEN_DECODEETACODE_H

// decode "etacode", see TrigFTKSim/FTKHit.h

#define SECTION_INT(a) (a%1000 < 20 ? 0 : (a%10))
#define ETA_CODE_INT(a) (a%1000)
#define PHI_MOD_INT(a) (a/1000)
#define ETA_MOD_INT(a) (a%1000 < 20 ? a%1000 : (a%1000)/20-1)
#define ASIDE_INT(a) (a%1000 < 20 ? 0 : ((ETA_CODE_INT(a))/10)%2 == 0) //POSEC
#define CSIDE_INT(a) (a%1000 < 20 ? 0 : ((ETA_CODE_INT(a))/10)%2)      //NEGEC


#endif
