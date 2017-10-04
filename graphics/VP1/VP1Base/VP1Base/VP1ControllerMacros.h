/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CONTROLLERMACROS_H
#define VP1CONTROLLERMACROS_H

#define VP1ACTUAL_STRINGIFY(str) #str
#define VP1STRINGIFY(str) VP1ACTUAL_STRINGIFY(str)
#define VP1SLOTNAME(x) VP1CONTROLLERCLASSNAME::possibleChange_##x
#define VP1SIGNALNAME(x) x##Changed
#ifndef VP1IMPVARNAME
# define VP1IMPVARNAME d
#endif
#define VP1LASTVARNAME(x) VP1IMPVARNAME->last_##x
#define POSSIBLECHANGE_IMP(x) void VP1SLOTNAME(x)() {	\
  if (changed( VP1LASTVARNAME(x) , x())) { \
    if (verbose()&&!initVarsMode()) messageVerbose("Emitting "+QString(VP1STRINGIFY(VP1SIGNALNAME(x)))+"( "+toString(VP1LASTVARNAME(x))+" )"); \
    emit VP1SIGNALNAME(x)(VP1LASTVARNAME(x)); } }

#endif
