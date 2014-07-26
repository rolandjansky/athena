
C*********************************************************************

C...FHSETPARA
C...Dummy function, to be removed when FEYNHIGGS is to be linked.

      SUBROUTINE FHSETPARA(IER,SCF,DMT,DMB,DMW,DMZ,DTANB,DMA,DMH,DM3L, 
     &     DM3E,DM3Q,DM3U,DM3D,DM2L,DM2E,DM2Q,DM2U, DM2D,DM1L,DM1E,DM1Q, 
     &     DM1U,DM1D,DMU,ATAU,AT,AB,AMU,AC,AS,AE,AU,AD,
     &     DM1,DM2,DM3,RLT,RLB,QTAU,QT,QB)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      SAVE /PYDAT1/
 
C...Stop program if this routine is ever called.
      WRITE(MSTU(11),5000)
      IF(PYR(0).LT.10D0) STOP

      AB = AB ! dummy line to avoid compiler warnings
      AC = AC ! dummy line to avoid compiler warnings
      AD = AD ! dummy line to avoid compiler warnings
      AE = AE ! dummy line to avoid compiler warnings
      AMU = AMU ! dummy line to avoid compiler warnings
      AS = AS ! dummy line to avoid compiler warnings
      AT = AT ! dummy line to avoid compiler warnings
      AU = AU ! dummy line to avoid compiler warnings
      ATAU = ATAU ! dummy line to avoid compiler warnings
      DM1 = DM1 ! dummy line to avoid compiler warnings
      DM1D = DM1D ! dummy line to avoid compiler warnings
      DM1E = DM1E ! dummy line to avoid compiler warnings
      DM1L = DM1L ! dummy line to avoid compiler warnings
      DM1Q = DM1Q ! dummy line to avoid compiler warnings
      DM1U = DM1U ! dummy line to avoid compiler warnings
      DM2 = DM2 ! dummy line to avoid compiler warnings
      DM2D = DM2D ! dummy line to avoid compiler warnings
      DM2E = DM2E ! dummy line to avoid compiler warnings
      DM2L = DM2L ! dummy line to avoid compiler warnings
      DM2Q = DM2Q ! dummy line to avoid compiler warnings
      DM2U = DM2U ! dummy line to avoid compiler warnings
      DM3 = DM3 ! dummy line to avoid compiler warnings
      DM3D = DM3D ! dummy line to avoid compiler warnings
      DM3E = DM3E ! dummy line to avoid compiler warnings
      DM3L = DM3L ! dummy line to avoid compiler warnings
      DM3Q = DM3Q ! dummy line to avoid compiler warnings
      DM3U = DM3U ! dummy line to avoid compiler warnings
      DMA = DMA ! dummy line to avoid compiler warnings
      DMB = DMB ! dummy line to avoid compiler warnings
      DMH = DMH ! dummy line to avoid compiler warnings
      DMT = DMT ! dummy line to avoid compiler warnings
      DMU = DMU ! dummy line to avoid compiler warnings
      DMW = DMW ! dummy line to avoid compiler warnings
      DMZ = DMZ ! dummy line to avoid compiler warnings
      DTANB = DTANB ! dummy line to avoid compiler warnings
      IER = IER ! dummy line to avoid compiler warnings
      QB = QB ! dummy line to avoid compiler warnings
      QT = QT ! dummy line to avoid compiler warnings
      QTAU = QTAU ! dummy line to avoid compiler warnings
      RLB = RLB ! dummy line to avoid compiler warnings
      RLT = RLT ! dummy line to avoid compiler warnings
      SCF = SCF ! dummy line to avoid compiler warnings

C...Format for error printout.
 5000 FORMAT(1X,'Error: you did not link FEYNHIGGS correctly.'/
     &1X,'Dummy routine FHSETPARA in PYTHIA file called instead.'/
     &1X,'Execution stopped!')
      RETURN
      END
