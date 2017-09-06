
c     Switching routine that enables the choice of top pair decays
c     for AcerMC 1.x generator as done in the fortran setup
c     Adapted for Pythia 6.2 by Borut Paul Kersevan (February 2003)
      SUBROUTINE ACERMC_ACSET12_PY(IAMOD)

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

#include "GeneratorFortranCommon/heprup.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "AcerMC_i/AcerMC_acset.inc"

      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)

      INTEGER IAMOD,IACPROC,ISEM,IWP,JJ
      DOUBLE PRECISION VLBRAT_T,VLBRAT_W,VLBRAT,ACBRAT,RWP
      LOGICAL NOIMP

      DOUBLE PRECISION PYR
      EXTERNAL PYR

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
         
            
         IF (ISEM.GE.1) THEN
            VLBRAT_T=BRAT(46)**2  ! FORCE ONLY T-B DECAYS BR=1 IN HERWIG
            VLBRAT_W=0.D0
            DO JJ=190,200       ! HADRONIC W BR
               VLBRAT_W=VLBRAT_W+BRAT(JJ)
            ENDDO
            IF (ISEM.EQ.1) THEN ! ELECTRON BR
               VLBRAT_W=VLBRAT_W*BRAT(206)
            ELSEIF (ISEM.EQ.2) THEN ! MUON BR
               VLBRAT_W=VLBRAT_W*BRAT(207)
            ELSEIF (ISEM.EQ.3) THEN ! TAU BR
               VLBRAT_W=VLBRAT_W*BRAT(208)
            ELSEIF (ISEM.EQ.4) THEN ! E+MU BR
               VLBRAT_W=VLBRAT_W*(BRAT(206)+BRAT(207))
            ELSEIF (ISEM.EQ.5) THEN ! SL DECAY MODE FOR WTT -> W^(+/-) E/MU^(+/-) JJ BB 
               IF (IACPROC.NE.4) THEN
               WRITE(*,*) 
     #'        ----<             STOPPED BY ACERMC                >----'
               WRITE(*,*) 
     #'        ----<  ACSET12=5 WORKS ONLY FOR ONLY FOR PROC=4    >----'
               STOP
               ENDIF
               VLBRAT_W=0.5D0*VLBRAT_W*(BRAT(206)+BRAT(207))
            ENDIF
            VLBRAT=VLBRAT_T*VLBRAT_W
            ACBRAT=2.D0*VLBRAT  ! SEMI-LEPTONICS ALLOW 2 OPTIONS 1-2+2-1
            WRITE(*,*) ' '
            WRITE(*,*)
     #'        ----<          ACERMC  1.3  & PYTHIA 6.2           >----'
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
     #'        ---->          ACERMC  1.3  & PYTHIA 6.2           <----'
            WRITE(*,*) ' '
         
         ENDIF
      
      ELSEIF (IAMOD.EQ.0) THEN ! SWAP THE BR-S
               
C 'SEMI-LEPTONIC' DECAYS FOR TOP PAIRS - SWITCH
C  ALLOWED W+/- DECAY MODES

         IF(ISEM.EQ.0) RETURN   ! <---- BUGFIX THANKS TO BJARTE MOHN
C FORCE ONLY T-B DECAYS
         MDME(44,1)=0
         MDME(45,1)=0

         RWP=PYR(9)
         IF (ISEM.EQ.5) THEN ! SL DECAY MODE FOR WTT -> W^(+/-) E/MU^(+/-) JJ BB  
            IWP=-24*SIGN(1,IDUP(6))
            IF (IWP.EQ.-24) RWP=1.D0
            IF (IWP.EQ.24) RWP=-1.D0
            print *,'iwps',iwp,idup(6)
         ENDIF

         IF (RWP.GT.0.5D0) THEN
            MDME(190,1)=2
            MDME(191,1)=2
            MDME(192,1)=2
            MDME(194,1)=2
            MDME(195,1)=2
            MDME(196,1)=2
            MDME(198,1)=2
            MDME(199,1)=2
            MDME(200,1)=2
            IF (ISEM.EQ.1) THEN
               MDME(206,1)=3
               MDME(207,1)=0
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.2) THEN
               MDME(206,1)=0
               MDME(207,1)=3
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.3) THEN
               MDME(206,1)=0
               MDME(207,1)=0
               MDME(208,1)=3
            ELSEIF (ISEM.EQ.4) THEN
               MDME(206,1)=3
               MDME(207,1)=3
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.5) THEN
               MDME(206,1)=3
               MDME(207,1)=3
               MDME(208,1)=0
            ENDIF
         ELSE
            MDME(190,1)=3
            MDME(191,1)=3
            MDME(192,1)=3
            MDME(194,1)=3
            MDME(195,1)=3
            MDME(196,1)=3
            MDME(198,1)=3
            MDME(199,1)=3
            MDME(200,1)=3
            IF (ISEM.EQ.1) THEN
               MDME(206,1)=2
               MDME(207,1)=0
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.2) THEN
               MDME(206,1)=0
               MDME(207,1)=2
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.3) THEN
               MDME(206,1)=0
               MDME(207,1)=0
               MDME(208,1)=2
            ELSEIF (ISEM.EQ.4) THEN
               MDME(206,1)=2
               MDME(207,1)=2
               MDME(208,1)=0
            ELSEIF (ISEM.EQ.5) THEN
               MDME(206,1)=2
               MDME(207,1)=2
               MDME(208,1)=0
            ENDIF
         ENDIF
      ENDIF ! IAMOD FLAG
      
      RETURN
      END
