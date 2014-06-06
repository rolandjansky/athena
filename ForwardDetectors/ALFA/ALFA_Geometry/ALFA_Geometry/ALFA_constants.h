/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AlfaConstants_h
#define AlfaConstants_h 1

#define ALFALAYERSCNT	2
#define ALFAPLATESCNT	10
#define ALFAFIBERSCNT	64

#define ODLAYERSCNT	2
#define ODPLATESCNT	3
#define ODFIBERSCNT	15

#define ODSIDESCNT	2
#define RPOTSCNT	8

const int ALFA_Nb_Plates = ALFAPLATESCNT;
const int ALFA_Nb_Layers = ALFALAYERSCNT;
const int ALFA_Nb_Fibers = ALFAFIBERSCNT;
//const double ALFA_stagger[ALFAPLATESCNT] = {0, 0.283, -0.141, 0.141, -0.283, 0.354, -0.071, 0.212, -0.212, 0.071};

const int OD_Nb_Plates = ODPLATESCNT;
const int OD_Nb_Layers = ODLAYERSCNT;
const int OD_Nb_Fibers = ODFIBERSCNT;
//const double OD_stagger[ODPLATESCNT] = {0, -0.167, -0.334};

#define ALFASTATIONHSIZEZ (500*CLHEP::mm)
#define ALFAEDGEBEAMPIPEHLENGTH (0.3*CLHEP::m)
#define DETEDGEDISTANCE (2.0*CLHEP::mm)
#define INACTIVEDETEGDEDISTANCE (42*CLHEP::mm)
#define INNERDETZSPACE (300*CLHEP::mm)
#define ALFA_GVSTHICKNESS (0.001*CLHEP::mm)

#define METROLOGYFILE "rpmetrology.dat"



#endif
