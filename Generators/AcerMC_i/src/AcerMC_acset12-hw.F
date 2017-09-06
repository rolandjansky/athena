
c     Switching routines that enables the choice of top pair decays
c     for AcerMC 1.x generator as done in the fortran setup
c     Adapted for Herwig 6.5 by Borut Paul Kersevan (February 2003)
      SUBROUTINE ACERMC_ACSET12_HW(IAMOD)

#include "HERWIG65.INC"
#include "GeneratorFortranCommon/heprup.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "AcerMC_i/AcerMC_acset.inc"

      INTEGER IAMOD,IACPROC,ISEM,IDBOS,IWP,JJ
      DOUBLE PRECISION VLBRAT_T,VLBRAT_W,VLBRAT,ACBRAT,RWP,BRTMP
      LOGICAL NOIMP
C W/Z/H BR ARRAY FROM HERWIG
      DOUBLE PRECISION BRMODE(12,3)
      DATA BRMODE/
     & 0.321D0,0.321D0,0.000D0,0.017D0,0.017D0,0.108D0,0.108D0,0.108D0,
     & 4*0.0D0,0.321D0,0.321D0,0.000D0,0.017D0,0.017D0,0.108D0,0.108D0,
     & 0.108D0,4*0.0D0,0.154D0,0.120D0,0.154D0,0.120D0,0.152D0,0.000D0,
     & 0.033D0,0.033D0,0.033D0,0.067D0,0.067D0,0.067D0/

      DOUBLE PRECISION HWRGEN
      EXTERNAL HWRGEN

      IACPROC=LPRUP(NPRUP)-600
      NOIMP=(IACPROC.EQ.1.OR.IACPROC.EQ.2.OR.IACPROC.EQ.4.OR.
     #     IACPROC.EQ.7.OR.IACPROC.EQ.8.OR.IACPROC.EQ.9.OR.
     #     IACPROC.EQ.10.OR.IACPROC.EQ.92.OR.IACPROC.EQ.93) ! ONLY RELEVANT CHANS
      IF (.NOT.NOIMP) THEN 
         ACSET12=0
         RETURN
      ENDIF

      ISEM=ACSET12

      IF (IAMOD.EQ.-1) THEN ! INIT

C 'SEMI-LEPTONIC' DECAYS FOR TOP PAIRS - BR
         
         IDBOS=198              ! W DECAY
            
         IF (ISEM.GE.1) THEN
            VLBRAT_T=(1.D0)**2  ! FORCE ONLY T-B DECAYS BR=1 IN HERWIG
            VLBRAT_W=0.D0
            DO JJ=1,5           ! HADRONIC W BR
             VLBRAT_W=VLBRAT_W+BRMODE(JJ,IDBOS-197)
            ENDDO
            IF (ISEM.EQ.1) THEN ! ELECTRON BR
             VLBRAT_W=VLBRAT_W*BRMODE(6,IDBOS-197)
            ELSEIF (ISEM.EQ.2) THEN ! MUON BR
             VLBRAT_W=VLBRAT_W*BRMODE(7,IDBOS-197)
            ELSEIF (ISEM.EQ.3) THEN ! TAU BR
             VLBRAT_W=VLBRAT_W*BRMODE(8,IDBOS-197)
            ELSEIF (ISEM.EQ.4) THEN ! E+MU BR
             VLBRAT_W=VLBRAT_W*(BRMODE(6,IDBOS-197)+BRMODE(7,IDBOS-197))
            ELSEIF (ISEM.EQ.5) THEN ! SL DECAY MODE FOR WTT -> W^(+/-) E/MU^(+/-) JJ BB 
               IF (IACPROC.NE.4) THEN
               WRITE(*,*) 
     #'        ----<             STOPPED BY ACERMC                >----'
               WRITE(*,*) 
     #'        ----<  ACSET12=5 WORKS ONLY FOR ONLY FOR PROC=4    >----'
               STOP
               ENDIF
               VLBRAT_W=0.5D0*VLBRAT_W*
     #              (BRMODE(6,IDBOS-197)+BRMODE(7,IDBOS-197))
            ENDIF
            VLBRAT=VLBRAT_T*VLBRAT_W
            ACBRAT=2.D0*VLBRAT  ! SEMI-LEPTONICS ALLOW 2 OPTIONS 1-2+2-1
            WRITE(*,*) ' '
            WRITE(*,*)
     #'        ----<          ACERMC  1.3  & HERWIG 6.5           >----'
            WRITE(*,*) ' '
            WRITE(*,*)
     #'             --<  BRANCHING MODE FOR TOP -> W -> ACTIVE >--'
            WRITE(*,*) ' '
            WRITE(*,24) ISEM,ACBRAT
 24         FORMAT (
     #'               THE ACERMC ACSET12 SWITCH IS SET TO ',I2/
     #'               THE PROCESS CROSS-SECTION SHOULD BE '/
     #'               MULTIPLIED BY THE BRANCHING RATIO =',D15.6/
     #'               IF NOT ALREADY DONE AT GENERATION TIME'/)  

            WRITE(*,*)
     #'             -->  BRANCHING MODE FOR TOP -> W -> ACTIVE <--'
            WRITE(*,*) ' '
            WRITE(*,*)
     #'        ---->          ACERMC  1.3  & HERWIG 6.5           <----'
            WRITE(*,*) ' '
         
         ENDIF
      
      ELSEIF (IAMOD.EQ.0) THEN ! SWAP THE BR-S
               
C 'SEMI-LEPTONIC' DECAYS FOR TOP PAIRS - SWITCH
C  ALLOWED W+/- DECAY MODES
         IF(ISEM.EQ.0) RETURN   ! <---- BUGFIX THANKS TO BJARTE MOHN

C FORCE ONLY T-B DECAYS
         RWP=HWRGEN(9)
         IF (ISEM.EQ.5) THEN ! SL DECAY MODE FOR WTT -> W^(+/-) E/MU^(+/-) JJ BB  
            IWP=-24*INT(SIGN(1,IDUP(6)))
            IF (IWP.EQ.-24) RWP=1.D0
            IF (IWP.EQ.24) RWP=-1.D0
         ENDIF

         IF (RWP.GT.0.5D0) THEN
            BRTMP=0.D0
            CALL ACMODK(6,BRTMP,100,12,-11,5,0,0) ! SWITCH OFF LEPTONIC DECAYS
            CALL ACMODK(6,BRTMP,100,14,-13,5,0,0)
            CALL ACMODK(6,BRTMP,100,16,-15,5,0,0)
            BRTMP=0.5D0
            CALL ACMODK(6,BRTMP,100,2,-1,5,0,0)
            CALL ACMODK(6,BRTMP,100,4,-3,5,0,0)
            IF (ISEM.EQ.1) THEN
               BRTMP=1.D0
               CALL ACMODK(-6,BRTMP,100,-12,11,-5,0,0)
            ELSEIF (ISEM.EQ.2) THEN
               BRTMP=1.D0
               CALL ACMODK(-6,BRTMP,100,-14,13,-5,0,0)
            ELSEIF (ISEM.EQ.3) THEN
               BRTMP=1.D0
               CALL ACMODK(-6,BRTMP,100,-16,15,-5,0,0)            
            ELSEIF (ISEM.EQ.4) THEN
               BRTMP=0.D0
               CALL ACMODK(-6,BRTMP,100,-2,1,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-4,3,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-16,15,-5,0,0)
               BRTMP=0.5D0
               CALL ACMODK(-6,BRTMP,100,-12,11,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-14,13,-5,0,0)
            ELSEIF (ISEM.EQ.5) THEN
               BRTMP=0.D0
               CALL ACMODK(-6,BRTMP,100,-2,1,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-4,3,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-16,15,-5,0,0)
               BRTMP=0.5D0
               CALL ACMODK(-6,BRTMP,100,-12,11,-5,0,0)
               CALL ACMODK(-6,BRTMP,100,-14,13,-5,0,0)
            ENDIF
         ELSE
            BRTMP=0.D0
            CALL ACMODK(-6,BRTMP,100,-12,11,-5,0,0) ! SWITCH OFF LEPTONIC DECAYS
            CALL ACMODK(-6,BRTMP,100,-14,13,-5,0,0)
            CALL ACMODK(-6,BRTMP,100,-16,15,-5,0,0)
            BRTMP=0.5D0
            CALL ACMODK(-6,BRTMP,100,-2,1,-5,0,0)
            CALL ACMODK(-6,BRTMP,100,-4,3,-5,0,0)
            IF (ISEM.EQ.1) THEN
               BRTMP=1.D0
               CALL ACMODK(6,BRTMP,100,12,-11,5,0,0)
            ELSEIF (ISEM.EQ.2) THEN
               BRTMP=1.D0
               CALL ACMODK(6,BRTMP,100,14,-13,5,0,0)
            ELSEIF (ISEM.EQ.3) THEN
               BRTMP=1.D0
               CALL ACMODK(6,BRTMP,100,16,-15,5,0,0)
            ELSEIF (ISEM.EQ.4) THEN
               BRTMP=0.D0
               CALL ACMODK(6,BRTMP,100,2,-1,5,0,0)
               CALL ACMODK(6,BRTMP,100,4,-3,5,0,0)
               CALL ACMODK(6,BRTMP,100,16,-15,5,0,0)
               BRTMP=0.5D0
               CALL ACMODK(6,BRTMP,100,12,-11,5,0,0)
               CALL ACMODK(6,BRTMP,100,14,-13,5,0,0)
            ELSEIF (ISEM.EQ.5) THEN
               BRTMP=0.D0
               CALL ACMODK(6,BRTMP,100,2,-1,5,0,0)
               CALL ACMODK(6,BRTMP,100,4,-3,5,0,0)
               CALL ACMODK(6,BRTMP,100,16,-15,5,0,0)
               BRTMP=0.5D0
               CALL ACMODK(6,BRTMP,100,12,-11,5,0,0)
               CALL ACMODK(6,BRTMP,100,14,-13,5,0,0)
            ENDIF
         ENDIF
         
      ENDIF ! IAMOD FLAG
      
      RETURN
      END

CC -----------------------------------------------------------
      SUBROUTINE ACMODK(IDKTMP,BRTMP,IMETMP,
     & IATMP,IBTMP,ICTMP,IDTMP,IETMP)

C-------------------------------------------------------------C
C                                                             C  
C    HERWIG INTERFACE - AN ANALOGUE TO HWMODK BUT DOES NOT    C
C    PROTEST AFTER REMOVAL/ADDITION OF BRANCHING MODES        C
C    SINCE THOSE ARE DONE OFTEN FOR SEMI-LEPTONIC TOP DECAYS  C
C                                                             C
C-------------------------------------------------------------C

#include "HERWIG65.INC"
      DOUBLE PRECISION HWUPCM,BRTMP,EPS
      INTEGER IDKTMP,IMETMP,IATMP,IBTMP,ICTMP,IDTMP,IETMP,IDKY,ITMP(5),
     & L,I,J,K
      LOGICAL MATCH(5)
      CHARACTER*8 CDUM
      EXTERNAL HWUPCM
      PARAMETER (EPS=1.D-6)
C Convert to internal format
      CALL HWUIDT(1,IDKTMP,IDKY,CDUM)
      IF (IDKY.EQ.20) THEN
        WRITE(*,10) IDKTMP
  10    FORMAT(1X,'ACMODK:Particle decaying,',I7,',  not recognised')
        RETURN
      ENDIF
      CALL HWUIDT(1,IATMP,ITMP(1),CDUM)
      CALL HWUIDT(1,IBTMP,ITMP(2),CDUM)
      CALL HWUIDT(1,ICTMP,ITMP(3),CDUM)
      CALL HWUIDT(1,IDTMP,ITMP(4),CDUM)
      CALL HWUIDT(1,IETMP,ITMP(5),CDUM)

C First search to see if mode pre-exists
      IF ((ITMP(2).GE.1.AND.ITMP(2).LE.13).OR.
     &     (ITMP(3).GE.1.AND.ITMP(3).LE.13)) THEN
C Partonic respect order
         L=LSTRT(IDKY)
         DO 30 K=1,NMODES(IDKY)
            IF (ITMP(1).EQ.IDKPRD(1,L).AND.
     &           ITMP(2).EQ.IDKPRD(2,L).AND.
     &           ITMP(3).EQ.IDKPRD(3,L).AND.
     &           ITMP(4).EQ.IDKPRD(4,L).AND.
     &           ITMP(5).EQ.IDKPRD(5,L)) GOTO 90
 30      L=LNEXT(L)
      ELSE
C Allow for different order in matching
         L=LSTRT(IDKY)
         DO 70 I=1,NMODES(IDKY)
            DO 40 J=1,5
 40         MATCH(J)=.FALSE.
            DO 60 J=1,5
               DO 50 K=1,5
            IF (.NOT.MATCH(K).AND.ITMP(K).EQ.IDKPRD(J,L)) THEN
               MATCH(K)=.TRUE.
               GOTO 60
            ENDIF
 50            CONTINUE
 60         CONTINUE
            IF (MATCH(1).AND.MATCH(2).AND.MATCH(3).AND.
     &           MATCH(4).AND.MATCH(5)) GOTO 90
 70      L=LNEXT(L)
      ENDIF

      WRITE (*,*)'MODE NOT FOUND??'

C A Pre-existing mode, line L, add/update ME code and BR, scaling all
C other branching fractions
 90   IF (IMETMP.GT.0) NME(L)=IMETMP
C--> HERE JUST UPDATE THE BRANCHINGS, DO NOT(!) REMOVE THE MODES
      BRFRAC(L)=BRTMP

      IF (ABS(BRTMP-1.).LT.EPS) THEN
         BRFRAC(L)=ONE
         J=LSTRT(IDKY) ! SET OTHERS TO 0
         DO 100 I=1,NMODES(IDKY)
            IF (J.NE.L) BRFRAC(J)=0.D0
 100     J=LNEXT(J)
      ENDIF

      IF (ABS(BRTMP).LT.EPS)  BRFRAC(L)=0.D0

  999 RETURN
      END
