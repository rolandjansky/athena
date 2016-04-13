 
C*********************************************************************
 
C...SSMSSM
C...Dummy function, to be removed when ISAJET (ISASUSY) is to be linked.

      SUBROUTINE SSMSSM(RDUM1,RDUM2,RDUM3,RDUM4,RDUM5,RDUM6,RDUM7,
     &RDUM8,RDUM9,RDUM10,RDUM11,RDUM12,RDUM13,RDUM14,RDUM15,RDUM16,
     &RDUM17,RDUM18,RDUM19,RDUM20,RDUM21,RDUM22,RDUM23,RDUM24,RDUM25,
     &IDUM1,IDUM2)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      REAL RDUM1,RDUM2,RDUM3,RDUM4,RDUM5,RDUM6,RDUM7,RDUM8,RDUM9,
     &RDUM10,RDUM11,RDUM12,RDUM13,RDUM14,RDUM15,RDUM16,RDUM17,RDUM18,
     &RDUM19,RDUM20,RDUM21,RDUM22,RDUM23,RDUM24,RDUM25
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      SAVE /PYDAT1/
 
C...Stop program if this routine is ever called.
      WRITE(MSTU(11),5000)
      IF(PYR(0).LT.10D0) STOP
 
      IDUM1 = IDUM1 ! dummy line to avoid compiler warnings
      IDUM2 = IDUM2 ! dummy line to avoid compiler warnings 
      RDUM1 = RDUM1 ! dummy line to avoid compiler warnings
      RDUM2 = RDUM2 ! dummy line to avoid compiler warnings
      RDUM3 = RDUM3 ! dummy line to avoid compiler warnings
      RDUM4 = RDUM4 ! dummy line to avoid compiler warnings
      RDUM5 = RDUM5 ! dummy line to avoid compiler warnings
      RDUM6 = RDUM6 ! dummy line to avoid compiler warnings
      RDUM7 = RDUM7 ! dummy line to avoid compiler warnings
      RDUM8 = RDUM8 ! dummy line to avoid compiler warnings
      RDUM9 = RDUM9 ! dummy line to avoid compiler warnings
      RDUM10 = RDUM10 ! dummy line to avoid compiler warnings
      RDUM11 = RDUM11 ! dummy line to avoid compiler warnings
      RDUM12 = RDUM12 ! dummy line to avoid compiler warnings
      RDUM13 = RDUM13 ! dummy line to avoid compiler warnings
      RDUM14 = RDUM14 ! dummy line to avoid compiler warnings
      RDUM15 = RDUM15 ! dummy line to avoid compiler warnings
      RDUM16 = RDUM16 ! dummy line to avoid compiler warnings
      RDUM17 = RDUM17 ! dummy line to avoid compiler warnings
      RDUM18 = RDUM18 ! dummy line to avoid compiler warnings
      RDUM19 = RDUM19 ! dummy line to avoid compiler warnings
      RDUM20 = RDUM20 ! dummy line to avoid compiler warnings
      RDUM21 = RDUM21 ! dummy line to avoid compiler warnings
      RDUM22 = RDUM22 ! dummy line to avoid compiler warnings
      RDUM23 = RDUM23 ! dummy line to avoid compiler warnings
      RDUM24 = RDUM24 ! dummy line to avoid compiler warnings
      RDUM25 = RDUM25 ! dummy line to avoid compiler warnings


C...Format for error printout.
 5000 FORMAT(1X,'Error: you did not link ISAJET correctly.'/
     &1X,'Dummy routine SSMSSM in PYTHIA file called instead.'/
     &1X,'Execution stopped!')
      RETURN
      END
