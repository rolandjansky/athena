/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
Modified: 30.04.2005

History:
=================================================
30.04.2005 -- Added Subroutine JNDUMP

*/

#ifndef f2cFortran
#define f2cFortran
#endif
#include "cfortran.h"


/*
 1) Feed-forward network (JN):                                        C
   ERRJN, GAUSJN, GJN, GPJN, JNCHOP, JNCOGR, JNCGBE, JNDELT, JNDUMP,  C
   JNERR, JNFEED, JNHEAD, JNHEIG, JNHESS, JNINDX, JNINIT, JNLINS,     C
   JNREAD, JNROLD, JNSATM, JNSCGR, JNSEFI, JNSEPA, JNSTAT, JNTEST,    C
   JNTRAL, JNTRED, JNTQLI                                             C
*/

#define MAXI 1000;
#define MAXO 1000;

typedef struct
{
  int MSTJN[ 40 ];
  float PARJN[ 40 ];
  int MSTJM[ 20 ];
  float PARJM[ 20 ];
  float OIN[ 1000 ];
  float OUT[ 1000 ];
  int MXNDJM;
} JNDAT1_DEF;

#define JNDAT1 COMMON_BLOCK(JNDAT1,jndat1)
COMMON_BLOCK_DEF(JNDAT1_DEF,JNDAT1);

//COMMON /JNDAT2/ TINV(10),IGFN(10),ETAL(10),WIDL(10),SATM(10)

typedef struct
{
  float TINV[ 10 ];
  int IGFN[ 10 ];
  float ETAL[ 10 ];
  float WIDL[ 10 ];
  float SATM[ 10 ];
} JNDAT2_DEF;

#define JNDAT2 COMMON_BLOCK(JNDAT2,jndat2)
COMMON_BLOCK_DEF(JNDAT2_DEF,JNDAT2);


typedef struct
{
  float O [ 2000 ]; // Current value of node I in the network, does not include the input units
  float A [ 2000 ]; // Current value of the summed input alpha_i to node I
  float D [ 2000 ]; // Current value of delta_i at node I
  float T [ 2000 ]; // Current value of threshold thetha_i at node I
  float DT [ 2000 ]; // Current value of the update delta(thetha_i) for the threshold at node I
  float W [ 150000 ]; // Current value for the weight with index IW
  float DW [ 150000 ]; // Current value of the update for weight with index IW
  int NSELF [ 150000 ]; // Switches for updating weight with index IW ( 0 = do not update, 1 = update )
  int NTSELF [ 2000 ]; // Switches for updating the threshold for node I --------- " -----------------
  float G [ 152000 ]; // Temporary weight and threshold vectors used in CG, SCG and QP
  float ODW [ 150000 ]; // Stores old weight gradient in CG, SCG, QP and Rprop
  float ODT [ 2000 ];  // stores old threshold gradient in CG, SCG, QP and Rprop
  float ETAV [ 152000 ]; // Individual learning rates used in Rprop
} JNINT1_DEF;

#define JNINT1 COMMON_BLOCK(JNINT1,jnint1)
COMMON_BLOCK_DEF(JNINT1_DEF,JNINT1);

/*
  INTEGER FUNCTION JNINDX(IL,I,J)
*/

PROTOCCALLSFFUN3(INT,JNINDX,jnindx,INT,INT,INT)
#define JNINDX(IL,I,J) CCALLSFFUN3(JNINDX,jnindx,INT,INT,INT,IL,I,J)


/*
  REAL FUNCTION GAUSJN(IDUM)
*/

PROTOCCALLSFFUN1(FLOAT,GAUSJN,gausjn,INT)
#define GAUSJN(IDUM) CCALLSFFUN1(GAUSJN,gausjn,INT,IDUM)

/*
  REAL FUNCTION RJN(IDUM)
*/

PROTOCCALLSFFUN1(FLOAT,RJN,rjn,INT)
#define RJN(IDUM) CCALLSFFUN1(RJN,rjn,INT,IDUM)
  
/*
  SUBROUTINE JNINIT
*/

PROTOCCALLSFSUB0(JNINIT,jninit)
#define JNINIT() CCALLSFSUB0(JNINIT,jninit)

/*
  SUBROUTINE JNTRAL
*/

PROTOCCALLSFSUB0(JNTRAL,jntral)
#define JNTRAL() CCALLSFSUB0(JNTRAL,jntral)

/*
  SUBROUTINE JNTEST
*/

PROTOCCALLSFSUB0(JNTEST,jntest)
#define JNTEST() CCALLSFSUB0(JNTEST,jntest)

/*
SUBROUTINE JNDUMP(NF)
*/
PROTOCCALLSFSUB1(JNDUMP,jndump,INT)
#define JNDUMP(NF) CCALLSFSUB1(JNDUMP,jndump,INT,NF);


/*
SUBROUTINE JNREAD(NF)
*/
PROTOCCALLSFSUB1(JNREAD,jnread,INT)
#define JNREAD(NF) CCALLSFSUB1(JNREAD,jnread,INT,NF);

/*
SUBROUTINE JNSTAT(IS)
*/
PROTOCCALLSFSUB1(JNSTAT,jnstat,INT)
#define JNSTAT(IS) CCALLSFSUB1(JNSTAT,jnstat,INT,IS);

/*				      
SUBROUTINE JNSEFI(ILA,I1,I2,J1,J2,NO)
*/
PROTOCCALLSFSUB6(JNSEFI,jnsefi,INT,INT,INT,INT,INT,INT)
#define JNSEFI(ILA,I1,I2,J1,J2,NO) CCALLSFSUB6(JNSEFI,jnsefi,INT,INT,INT,INT,INT,INT,ILA,I1,I2,J1,J2,NO);

/*
SUBROUTINE REWIND(INT,INT)
*/
PROTOCCALLSFSUB1(REWIND,rewind,INT)
#define REWIND(UNIT) CCALLSFSUB1(REWIND,rewind,INT,UNIT);
