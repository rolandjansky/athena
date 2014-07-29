C-----------------------------------------------------------------------
      SUBROUTINE PHYFIX(NSTOP,NSTART)
      INTEGER N, NPAD, K 
      DOUBLE PRECISION P, V
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5) 
      SAVE /PYJETS/ 
C New standard event common
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      SAVE /HEPEVT/
      DOUBLE PRECISION PHEP,VHEP

c      IF (NSTOP.EQ.-100) CALL PYHEPC(1)

C NSTOP NSTART : when PHYTIA history ends and event starts.
      NSTOP=0
      NSTART=1
      IF (NHEP.GT.0) THEN
C         PRINT *,'HERWIG/HEPEVT'
         DO I=1,NHEP
            IF(ISTHEP(I).GT.125.OR.(ISTHEP(I).LT.120.AND.
     &           ISTHEP(I).NE.3
     &           .AND.ISTHEP(I).NE.101.AND.ISTHEP(I).NE.102)) THEN
               NSTOP = I-1
               NSTART= I
               GOTO 500
            ENDIF
         ENDDO  
      ELSEIF (N.GT.0) THEN
C         PRINT *,'PYTHIA'
         DO I=1, N
            IF(K(I,1).NE.21) THEN
               NSTOP = I-1
               NSTART= I
               GOTO 500
            ENDIF
         ENDDO
      ELSE 
         PRINT *,'UNKNOWN STRUCTURE'
      ENDIF
      

 500  CONTINUE
      END

C-----------------------------------------------------------------------
      SUBROUTINE TAU_INIMAS
C ----------------------------------------------------------------------
C     INITIALISATION OF MASSES WITH PYTHIA VALUES
C
C     called by : TAUOLA_HEPEVT
C ----------------------------------------------------------------------
      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
C
C(BPK) HERWIG  IDHW AS A CHECK OF THE LINKED GENERATOR
      INTEGER  d_h_nmxhep         ! maximum number of particles
      PARAMETER (d_h_NMXHEP=10000)
      DOUBLE PRECISION AVWGT,EVWGT,GAMWT,TLOUT,WBIGST,WGTMAX,WGTSUM,
     #     WSQSUM
      INTEGER IDHW,IERROR,ISTAT,LWEVT,MAXER,MAXPR,NRN,NUMER,NUMERU,NWGTS
      LOGICAL GENSOF,NOWGT
      COMMON/HWEVNT/AVWGT,EVWGT,GAMWT,TLOUT,WBIGST,WGTMAX,WGTSUM,WSQSUM,
     & IDHW(d_h_NMXHEP),IERROR,ISTAT,LWEVT,MAXER,MAXPR,NOWGT,NRN(2),
     & NUMER,NUMERU,NWGTS,GENSOF
      SAVE /HWEVNT/

      LOGICAL ISHERWIG
      COMMON /LOGHW/ISHERWIG

C IN-COMING / OUT-GOING  FERMION MASSES
C --- tau mass must be the same as in the host program, what-so-ever

      ISHERWIG=.FALSE.
      IF (MAXER.EQ.0) THEN
c         PRINT *,'PYHTIA CALLED'
         AMTAU  = 1.777         ! PYTHIA VALUE
         AMNUTA = 0.010
         AMEL   = 0.00051
         AMNUE  = 0.0
         AMMU   = 0.10566
         AMNUMU = 0.0
*
* MASSES USED IN TAU DECAYS

         AMPIZ  = 0.13498
         AMPI   = 0.13957
         AMRO   = 0.773
         GAMRO  = 0.145
*C    GAMRO  = 0.666
         AMA1   = 1.230
         GAMA1  = 0.400
         AMK    = 0.49360
         AMKZ   = 0.49767
         AMKST  = 0.89610
         GAMKST = 0.05050
      ELSE                      ! HERWIG VALUE
C         PRINT *,'HERWIG CALLED'
         ISHERWIG=.TRUE.
         AMTAU  = 1.7771
         AMNUTA = 0.010
         AMEL   = 0.000511
         AMNUE  = 0.0
         AMMU   = 0.10566
         AMNUMU = 0.0
*
* MASSES USED IN TAU DECAYS

         AMPIZ  = 0.13498
         AMPI   = 0.13957
         AMRO   = 0.773
         GAMRO  = 0.145
*C    GAMRO  = 0.666
         AMA1   = 1.230
         GAMA1  = 0.400
         AMK    = 0.49368
         AMKZ   = 0.49767
         AMKST  = 0.89610
         GAMKST = 0.05050
      ENDIF

      RETURN
      END

      SUBROUTINE TAUBRS(IMOD)
C THIS ROUTINE CAN BE CALLED BEFORE ANY TAU+ OR TAU- EVENT IS GENERATED
C IT CAN BE USED TO GENERATE TAU+ AND TAU- SAMPLES OF DIFFERENT
C CONTENTS/BRANCHING RATIOS
      COMMON / TAUKLE / BRA1,BRK0,BRK0B,BRKS
      REAL*4            BRA1,BRK0,BRK0B,BRKS
      COMMON / TAUBRA / GAMPRT(30),JLIST(30),NCHAN

#include "Tauola_i/UserGamprt.inc"      

      IF (IMOD.EQ.1) THEN ! LEP DECAY MODE
C     ==================
      
         DO I=1,22
            JLIST(I) = I
            
            IF(UGUSED.EQ.1) THEN
               IF(I.EQ. 1) GAMPRT(I) = USERGAMPRT(I)  
               IF(I.EQ. 2) GAMPRT(I) = USERGAMPRT(I) 
               IF(I.EQ. 3) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 4) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 5) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 6) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 7) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ. 8) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ. 9) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.10) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.11) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.12) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.13) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.14) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.15) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.16) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.17) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.18) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.19) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.20) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.21) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.22) GAMPRT(I) = USERGAMPRT(I) *0.
            ELSE
               IF(I.EQ. 1) GAMPRT(I) =0.1800 
               IF(I.EQ. 2) GAMPRT(I) =0.1751 
               IF(I.EQ. 3) GAMPRT(I) =0.1110 *0. 
               IF(I.EQ. 4) GAMPRT(I) =0.2515 *0. 
               IF(I.EQ. 5) GAMPRT(I) =0.1790 *0. 
               IF(I.EQ. 6) GAMPRT(I) =0.0071 *0. 
               IF(I.EQ. 7) GAMPRT(I) =0.0134 *0.
               IF(I.EQ. 8) GAMPRT(I) =0.0450 *0.
               IF(I.EQ. 9) GAMPRT(I) =0.0100 *0.
               IF(I.EQ.10) GAMPRT(I) =0.0009 *0.
               IF(I.EQ.11) GAMPRT(I) =0.0004 *0. 
               IF(I.EQ.12) GAMPRT(I) =0.0003 *0. 
               IF(I.EQ.13) GAMPRT(I) =0.0005 *0. 
               IF(I.EQ.14) GAMPRT(I) =0.0015 *0.
               IF(I.EQ.15) GAMPRT(I) =0.0015 *0. 
               IF(I.EQ.16) GAMPRT(I) =0.0015 *0.
               IF(I.EQ.17) GAMPRT(I) =0.0005 *0.
               IF(I.EQ.18) GAMPRT(I) =0.0050 *0.
               IF(I.EQ.19) GAMPRT(I) =0.0055 *0.
               IF(I.EQ.20) GAMPRT(I) =0.0017 *0.
               IF(I.EQ.21) GAMPRT(I) =0.0013 *0.
               IF(I.EQ.22) GAMPRT(I) =0.0010 *0.
            ENDIF
         ENDDO

      ELSEIF (IMOD.EQ.2) THEN ! HAD DECAY MODE
C     ====
         DO I=1,22
            JLIST(I) = I
            
            IF(UGUSED.EQ.1) THEN
               IF(I.EQ. 1) GAMPRT(I) = USERGAMPRT(I) * 0.  
               IF(I.EQ. 2) GAMPRT(I) = USERGAMPRT(I) * 0.  
               IF(I.EQ. 3) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 4) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 5) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 6) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 7) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 8) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 9) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.10) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.11) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.12) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.13) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.14) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.15) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.16) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.17) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.18) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.19) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.20) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.21) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.22) GAMPRT(I) = USERGAMPRT(I)
            ELSE 
               IF(I.EQ. 1) GAMPRT(I) =0.1800 *0.
               IF(I.EQ. 2) GAMPRT(I) =0.1751 *0.
               IF(I.EQ. 3) GAMPRT(I) =0.1110 
               IF(I.EQ. 4) GAMPRT(I) =0.2515 
               IF(I.EQ. 5) GAMPRT(I) =0.1790 
               IF(I.EQ. 6) GAMPRT(I) =0.0071 
               IF(I.EQ. 7) GAMPRT(I) =0.0134
               IF(I.EQ. 8) GAMPRT(I) =0.0450
               IF(I.EQ. 9) GAMPRT(I) =0.0100
               IF(I.EQ.10) GAMPRT(I) =0.0009
               IF(I.EQ.11) GAMPRT(I) =0.0004 
               IF(I.EQ.12) GAMPRT(I) =0.0003 
               IF(I.EQ.13) GAMPRT(I) =0.0005 
               IF(I.EQ.14) GAMPRT(I) =0.0015 
               IF(I.EQ.15) GAMPRT(I) =0.0015 
               IF(I.EQ.16) GAMPRT(I) =0.0015 
               IF(I.EQ.17) GAMPRT(I) =0.0005
               IF(I.EQ.18) GAMPRT(I) =0.0050
               IF(I.EQ.19) GAMPRT(I) =0.0055
               IF(I.EQ.20) GAMPRT(I) =0.0017 
               IF(I.EQ.21) GAMPRT(I) =0.0013 
               IF(I.EQ.22) GAMPRT(I) =0.0010 
            ENDIF
         ENDDO
      ELSEIF (IMOD.EQ.3) THEN   ! !TAU->PI NU DECAY 
C     ====
         DO I=1,22
            JLIST(I) = I

            IF(UGUSED.EQ.1) THEN
               IF(I.EQ. 1) GAMPRT(I) = USERGAMPRT(I) *0.   
               IF(I.EQ. 2) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 3) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 4) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 5) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 6) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ. 7) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ. 8) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ. 9) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.10) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.11) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.12) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.13) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.14) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.15) GAMPRT(I) = USERGAMPRT(I) *0. 
               IF(I.EQ.16) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.17) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.18) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.19) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.20) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.21) GAMPRT(I) = USERGAMPRT(I) *0.
               IF(I.EQ.22) GAMPRT(I) = USERGAMPRT(I) *0.
            ELSE
               IF(I.EQ. 1) GAMPRT(I) =0.1800 *0.
               IF(I.EQ. 2) GAMPRT(I) =0.1751 *0.
               IF(I.EQ. 3) GAMPRT(I) =0.1110 
               IF(I.EQ. 4) GAMPRT(I) =0.2515 *0. 
               IF(I.EQ. 5) GAMPRT(I) =0.1790 *0.
               IF(I.EQ. 6) GAMPRT(I) =0.0071 *0. 
               IF(I.EQ. 7) GAMPRT(I) =0.0134 *0.
               IF(I.EQ. 8) GAMPRT(I) =0.0450 *0.
               IF(I.EQ. 9) GAMPRT(I) =0.0100 *0.
               IF(I.EQ.10) GAMPRT(I) =0.0009 *0.
               IF(I.EQ.11) GAMPRT(I) =0.0004 *0. 
               IF(I.EQ.12) GAMPRT(I) =0.0003 *0. 
               IF(I.EQ.13) GAMPRT(I) =0.0005 *0. 
               IF(I.EQ.14) GAMPRT(I) =0.0015 *0. 
               IF(I.EQ.15) GAMPRT(I) =0.0015 *0. 
               IF(I.EQ.16) GAMPRT(I) =0.0015 *0. 
               IF(I.EQ.17) GAMPRT(I) =0.0005 *0.
               IF(I.EQ.18) GAMPRT(I) =0.0050 *0.
               IF(I.EQ.19) GAMPRT(I) =0.0055 *0.
               IF(I.EQ.20) GAMPRT(I) =0.0017 *0. 
               IF(I.EQ.21) GAMPRT(I) =0.0013 *0. 
               IF(I.EQ.22) GAMPRT(I) =0.0010 *0. 
            ENDIF
         ENDDO
      ELSEIF (IMOD.EQ.0) THEN ! INCLUSIVE DECAY
         DO I=1,22
            JLIST(I) = I

            IF(UGUSED.EQ.1) THEN
               IF(I.EQ. 1) GAMPRT(I) = USERGAMPRT(I) 
               IF(I.EQ. 2) GAMPRT(I) = USERGAMPRT(I) 
               IF(I.EQ. 3) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 4) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 5) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 6) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 7) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 8) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ. 9) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.10) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.11) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.12) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.13) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.14) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.15) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.16) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.17) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.18) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.19) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.20) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.21) GAMPRT(I) = USERGAMPRT(I)
               IF(I.EQ.22) GAMPRT(I) = USERGAMPRT(I)
            ELSE 
               IF(I.EQ. 1) GAMPRT(I) =0.1800
               IF(I.EQ. 2) GAMPRT(I) =0.1751
               IF(I.EQ. 3) GAMPRT(I) =0.1110 
               IF(I.EQ. 4) GAMPRT(I) =0.2515 
               IF(I.EQ. 5) GAMPRT(I) =0.1790 
               IF(I.EQ. 6) GAMPRT(I) =0.0071 
               IF(I.EQ. 7) GAMPRT(I) =0.0134
               IF(I.EQ. 8) GAMPRT(I) =0.0450
               IF(I.EQ. 9) GAMPRT(I) =0.0100
               IF(I.EQ.10) GAMPRT(I) =0.0009
               IF(I.EQ.11) GAMPRT(I) =0.0004 
               IF(I.EQ.12) GAMPRT(I) =0.0003 
               IF(I.EQ.13) GAMPRT(I) =0.0005 
               IF(I.EQ.14) GAMPRT(I) =0.0015 
               IF(I.EQ.15) GAMPRT(I) =0.0015 
               IF(I.EQ.16) GAMPRT(I) =0.0015 
               IF(I.EQ.17) GAMPRT(I) =0.0005
               IF(I.EQ.18) GAMPRT(I) =0.0050
               IF(I.EQ.19) GAMPRT(I) =0.0055
               IF(I.EQ.20) GAMPRT(I) =0.0017 
               IF(I.EQ.21) GAMPRT(I) =0.0013 
               IF(I.EQ.22) GAMPRT(I) =0.0010 
            ENDIF
         ENDDO

      ENDIF
C     =====
      END


C----------------------------------------------------------------------
       SUBROUTINE TAUBR_PRINT(IMODE)
C TAUOLA PRINTOUT OF THE BRANCHING RATIOS
      COMMON / TAUKLE / BRA1,BRK0,BRK0B,BRKS
      REAL*4            BRA1,BRK0,BRK0B,BRKS
      COMMON / TAUBRA / GAMPRT(30),JLIST(30),NCHAN

      REAL LEPSUM,HADSUM,TPNSUM,TOTSUM
      CHARACTER*15 BRNAME
C I/O
      INTEGER INUT,IOUT
      COMMON / INOUT / INUT,IOUT ! TAUOLA OUT

      CALL TAUBRS(0)
      TOTSUM=0.
      DO I=1,NCHAN
         TOTSUM=TOTSUM+GAMPRT(I)
      ENDDO

      CALL TAUBRS(1)
      LEPSUM=0.
      DO I=1,NCHAN
         LEPSUM=LEPSUM+GAMPRT(I)
      ENDDO

      CALL TAUBRS(2)
      HADSUM=0.
      DO I=1,NCHAN
         HADSUM=HADSUM+GAMPRT(I)
      ENDDO


      CALL TAUBRS(3) !TAU->PI NU DECAY 
      TPNSUM=0.
      DO I=1,NCHAN
         TPNSUM=TPNSUM+GAMPRT(I)
      ENDDO
      

      
      TBRFAC=1.
      TBRFAC2=1.
      IF (IMODE.EQ.0) RETURN ! NO BRANCHINGS
      WRITE (IOUT,9000)
      WRITE (IOUT,9010)
      WRITE (IOUT,9000)
      IF (IMODE.EQ.1) THEN
         TBRFAC=LEPSUM/TOTSUM
         TBRFAC2=TBRFAC*TBRFAC
         BRNAME='LEPTON-LEPTON'
         WRITE (IOUT,9020) BRNAME
         WRITE (IOUT,9030)
         WRITE (IOUT,9040) TBRFAC
         WRITE (IOUT,9050) TBRFAC2
        
      ELSEIF (IMODE.EQ.2) THEN
         TBRFAC=HADSUM/TOTSUM
         TBRFAC2=TBRFAC*TBRFAC
         BRNAME='HADRON-HADRON'
         WRITE (IOUT,9020) BRNAME
         WRITE (IOUT,9030)
         WRITE (IOUT,9040) TBRFAC
         WRITE (IOUT,9050) TBRFAC2

      ELSEIF (IMODE.EQ.3) THEN
         X1=LEPSUM/TOTSUM
         X2=HADSUM/TOTSUM
         TBRFAC2=2*X1*X2
         BRNAME='LEPTON-HADRON'
         WRITE (IOUT,9020) BRNAME
         WRITE (IOUT,9030)
         WRITE (IOUT,9050) TBRFAC2

      ELSEIF (IMODE.EQ.4) THEN
         TBRFAC=TPNSUM/TOTSUM
         TBRFAC2=TBRFAC*TBRFAC
         BRNAME='TAU->PI NU'
         WRITE (IOUT,9020) BRNAME
         WRITE (IOUT,9030)
         WRITE (IOUT,9040) TBRFAC
         WRITE (IOUT,9050) TBRFAC2
      ENDIF
      WRITE (IOUT,9000)
      WRITE (IOUT,9060)
      WRITE (IOUT,9000)
      WRITE (IOUT,9070)
      WRITE (IOUT,9000)

 9000 FORMAT(' ')
 9010 FORMAT(9X,'------< TAUOLA BRANCHING RATIO FOR TAU DECAYS >------')
 9020 FORMAT(10X,'THE TAU DECAYS ARE RESTRICTED TO A:'/' '/
     #10X,'          ',A,'DECAY MODE'/)
 9030 FORMAT(10X,'THE PROCESS CROSS-SECTION MUST BE MULTIPLIED BY:'/)
 9040 FORMAT(10X,' -> A BRANCHING RATIO =',D15.6,' FOR ONE TAU')  
 9050 FORMAT(10X,' -> A BRANCHING RATIO =',D15.6,' FOR TWO TAUS')
 9060 FORMAT(10X,'IN THE HARD PROCESS DECAY PRODUCTS!!!')
 9070 FORMAT(9X,'------> TAUOLA BRANCHING RATIO FOR TAU DECAYS <------')



      RETURN
      END

