/* we are in GIT -- we are in GIT ----
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_CONSTANTS_H
#define AFP_CONSTANTS_H 1

//#define USE_TDLBARS 1

#define DETXSIDE (-1)
#define LHCXOFFSET (-97.0*CLHEP::mm)

#define SIDCNT 4
#define SID_NOMINALSLOPE (14*CLHEP::deg)
#define SID_NOMINALSPACING (9.0*CLHEP::mm)
#define SID_DISTANCETOFLOOR (0.4*CLHEP::mm)
#define SID_ZDISTANCEINRPOT (20.0*CLHEP::mm)
#define SID_CORRZOFFSET (5.0*CLHEP::mm)
#define SID_SENSORXDIM (18.8*CLHEP::mm)
#define SID_SENSORYDIM (20.2*CLHEP::mm)
#define SID_SENSORTHICKNESS (0.230*CLHEP::mm)
#define SID_MAINPLATEXDIM (75.6*CLHEP::mm) 
#define SID_MAINPLATEYDIM (80.0*CLHEP::mm)
#define SID_PLATETHICKNESS (1.0*CLHEP::mm)
#define SID_VACUUMSENSORTHICKNESS (0.0001*CLHEP::mm)
#define SID_GLOBALVACUUMSENSORID 11
#define SID_GLOBALVACUUMSENSORZOFFSET (275*CLHEP::mm)
#define SID_DEATH_EDGE (1.9*CLHEP::mm)
#define SID_LOWER_EDGE (0.1*CLHEP::mm)

#define TD_TRAINSCNT 4
#define TD_COLUMNSCNT 2
#define TD_NOMINALSLOPE (48*CLHEP::deg)
#define TD_FIRSTBARVERTLENGTH (150.0*CLHEP::mm)
#define TD_FIRSTBARHORZLENGTH (50.0*CLHEP::mm)
#define TD_SLIMBARBORDERWIDTH (0.001*CLHEP::mm)
#define TD_SLIMCUT (0.01*CLHEP::mm)
#define TD_SENSORTHICKNESS (1.0*CLHEP::mm)
#define TD_DISTANCETOFLOOR (0.4*CLHEP::mm)
#define TD_ZDISTANCEINRPOT (10.0*CLHEP::mm)
#define TD_VERTXGAP (0.03*CLHEP::mm)
#define TD_VERTZGAP (0.13*CLHEP::mm)
#define TD_HORZYGAP (0.13*CLHEP::mm)

#define INNERSTATZDISTANCE (205.5*CLHEP::m) // avarage of A and C -205.824 m and 205.217 m
#define OUTERSTATZDISTANCE (217.6*CLHEP::m) // avarage of A and C -217.909 m and 217.302 m
#define STATIONSHIFTINYAXIS (0.0*CLHEP::mm)
#define RPFLOORDISTANCE (1.5*CLHEP::mm)//(-0.6*CLHEP::mm)

#define HBPTHINWINDOWTHICKNESS (0.5*CLHEP::mm) // design window thickness; maximum is 3 mm (wall thickness)
#define HBPWINDOWCUTYSIGN (1) // y axis sign, where thin window should have cutted corner

#endif //AFP_CONSTANTS_H
