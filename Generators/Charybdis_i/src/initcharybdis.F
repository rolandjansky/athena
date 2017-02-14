CDECK  ID>, INITUSER.
*CMZ :-        -17/07/03  18.11.30  by  Peter Richardson
*-- Author :    Chris Harris and Peter Richardson
C----------------------------------------------------------------------
      SUBROUTINE INITCHARYBDIS
C----------------------------------------------------------------------
C     Les Houches initialisation routine
C----------------------------------------------------------------------
#include "Charybdis_i/charybdis1003.inc"
      external chdata

C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
C--event common block
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &              IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &              ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &              SPINUP(MAXNUP)
C--local variables
      INTEGER NCHSRCH,I
      DOUBLE PRECISION CHWSUM,CHWSQ,SPNZRO,SPNHLF,SPNONE,PTOTAL
      DOUBLE PRECISION PPROBS(6,3)
      DATA PPROBS /1.0d0, 1.0d0, 1.0d0, 1.0d0, 1.0d0, 1.0d0,
     &     0.773d0, 0.776d0, 0.761d0, 0.743d0, 0.726d0, 0.711d0,
     &     0.688d0, 0.827d0, 0.909d0, 0.959d0, 0.990d0, 1.011d0/

C--DEV added by nick brett
      INTEGER arg
      INTEGER index
C--DEV end

C--parameters for the initial maximum weight search
      DATA NCHSRCH /10000/
C--BW mod 16/08/06: set default values in BLOCK DATA CHDATA,
C  so that they can be reset in main program.
C--You can still change them here instead if you want.
C--Beam particles and energies (only proton=2212 and antiproton=-2212)
C--N.B.for HERWIG these are overidden by the values in the main program
      IDBMUP(1) = 2212
      IDBMUP(2) = 2212
C--      EBMUP(1)  = 7000.0D0
C--      EBMUP(2)  = 7000.0D0
C--      EBMUP(1)  = 5000.0D0
C--      EBMUP(2)  = 5000.0D0
      index=17
      CALL readcharybdisparamdbl(index,EBMUP(1))
      CALL readcharybdisparamdbl(index,EBMUP(2))



C--Set MPLNCK and define what is meant by it:
C--MSSDEF=1 means M_GT, MSSDEF=2 means M_DL and MSSDEF=3 means M_D
C      MPLNCK=1000.0D0
C      MSSDEF=2
      index=1
      CALL readcharybdisparamdbl(index,MPLNCK)      
      index=13
      CALL readcharybdisparamint(index,MSSDEF)
C--Set number of dimensions (number of EXTRA dimensions is n=TOTDIM-4)
C--TOTDIM can be in the range 6-11
C      TOTDIM=6
      index=2
      CALL readcharybdisparamint(index,TOTDIM)
C--Use Giddings+Thomas momentum scale for calling pdfs?
C--(See page 12 of hep-ph/0106219)
C      GTSCA=.FALSE.
      index=14
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         GTSCA=.FALSE.
      else
         GTSCA=.TRUE.
      endif
C--Set mass window for black holes produced
C      MINMSS=5000.0D0
C      MAXMSS=EBMUP(1)+EBMUP(2)
      index=3
      CALL readcharybdisparamdbl(index,MINMSS)
      index=4
      CALL readcharybdisparamdbl(index,MAXMSS)
C--Set NBODY decay of BH remnant - NBODY can be in range 2-5.
C      NBODY=2
      index=5
      CALL readcharybdisparamint(index,NBODY)
C--Turn time variation of BH Hawking temperature in decay on or off
C      TIMVAR=.TRUE.
      index=6
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         TIMVAR=.FALSE.
      else
         TIMVAR=.TRUE.
      endif
C--Set which decay products are to be used:
C--MSSDEC=1 gives no heavy particles
C--MSSDEC=2 gives t, W and Z as well
C--MSSDEC=3 gives t, W, Z and Higgs as well  
C      MSSDEC=3
      index=15
      CALL readcharybdisparamint(index,MSSDEC)

C--Turn grey-body factors on/off
C      GRYBDY=.TRUE.
      index=7
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         GRYBDY=.FALSE.
      else
         GRYBDY=.TRUE.
      endif
C--Turn kinematic cut-off of decay on (rather than M=MPLANCK cut-off)
C      KINCUT=.FALSE.
      index=8
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         KINCUT=.FALSE.
      else
         KINCUT=.TRUE.
      endif
C--BW mod 16/08/06: new parameters for version 1.003
C--Use Yoshino-Rychkov factors in cross-section
C      YRCSEC= .FALSE.
      index=9
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         YRCSEC=.FALSE.
      else
         YRCSEC=.TRUE.
      endif
C--Max Hawking temperature
C      THWMAX =1000.0D0
      index=10
      CALL readcharybdisparamdbl(index,THWMAX)
C--Remnant decay model: boiling at THWMAX
C      RMBOIL = .FALSE.
      index=11
      CALL readcharybdisparamint(index,arg)
      if(arg.EQ.0)then
         RMBOIL=.FALSE.
      else
         RMBOIL=.TRUE.
      endif
C--Min BH mass, ends boiling
C      RMMINM = 1000.D0
      index=12
      CALL readcharybdisparamdbl(index,RMMINM)
C--print out option (0=no printout, 1 =errors only, 2=errors+info)
C      IBHPRN = 1
      index=16
      CALL readcharybdisparamint(index,IBHPRN)
C--pdf's for the beams (use the generator default)
C--MUST BE THE SAME FOR BOTH BEAMS
      PDFGUP(1) = -1
      PDFGUP(2) = -1
      PDFSUP(1) = -1
      PDFSUP(2) = -1
C--------------------------------------------------------------------
C     END OF USER VARIABLES DON'T TOUCH ANYTHING BELOW HERE
C--------------------------------------------------------------------
c#if !defined(PYTHIA)
C--HERWIG beams now passed from main program
c      CALL HWUIDT(3,IDBMUP(1),I,PART1)
c      CALL HWUIDT(3,IDBMUP(2),I,PART2)
c      EBMUP(1)=PBEAM1
c      EBMUP(2)=PBEAM2
c#endif
C--end mod
C--probabilities for particles of different spin
C--(arbitrary normalisation)
      IF (GRYBDY) THEN
         SPNZRO = PPROBS(TOTDIM-5,1)
         SPNHLF = PPROBS(TOTDIM-5,2)
         SPNONE = PPROBS(TOTDIM-5,3)
      ELSE
         SPNZRO = 1d0
         SPNHLF = 0.750d0
         SPNONE = 1d0
      ENDIF
C--calculation of probability of emission for different particle types
C--only light SM particles
      IF (MSSDEC.EQ.1) THEN
         PTOTAL = (78.0d0*SPNHLF)+(18.0d0*SPNONE)
         PQUARK = (60.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PWBOSN = 0.0D0
         PZBOSN = 0.0D0
         PHIGGS = 0.0D0   
C--light SM partcles + top W/Z      
      ELSEIF (MSSDEC.EQ.2) THEN
         PTOTAL = (3.0d0*SPNZRO)+(90.0d0*SPNHLF)+(24.0d0*SPNONE)
         PQUARK = (72.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PZBOSN = (SPNZRO+(2.0d0*SPNONE))/PTOTAL
         PWBOSN = 2.0d0*PZBOSN
         PHIGGS = 0.0D0 
C--light SM particles +top W/Z and Higgs
      ELSE
         PTOTAL = (4.0d0*SPNZRO)+(90.0d0*SPNHLF)+(24.0d0*SPNONE)
         PQUARK = (72.0d0*SPNHLF)/PTOTAL
         PLEPT  = (12.0d0*SPNHLF)/PTOTAL
         PNEUT  = (6.0d0*SPNHLF)/PTOTAL
         PGLUON = (16.0d0*SPNONE)/PTOTAL
         PGAMMA = (2.0d0*SPNONE)/PTOTAL
         PZBOSN = (SPNZRO+(2.0d0*SPNONE))/PTOTAL
         PWBOSN = 2.0d0*PZBOSN
         PHIGGS = SPNZRO/PTOTAL
      ENDIF
C--what do do with the weights(here are generating weighted events)
      IDWTUP = 1
C--only one process
      NPRUP  = 1
C--communication code
      LPRUP(1) = 1
C--calculate the maximum weight
      CHWSUM = 0.0D0
      CHWSQ = 0.0D0
      XMAXUP(1) = 0.0D0
      DO I=1,NCHSRCH
         CALL CHEVNT(.FALSE.)
         CHWSUM = CHWSUM+XWGTUP
         CHWSQ  = CHWSQ+XWGTUP**2
         XMAXUP(1) = MAX(XMAXUP(1),XWGTUP)
      ENDDO
      CHWSUM = CHWSUM/DBLE(NCHSRCH)
      CHWSQ  = MAX(CHWSQ/DBLE(NCHSRCH)-CHWSUM**2,0.0D0)
      CHWSQ  = SQRT(CHWSQ/ DBLE(NCHSRCH))
C--cross section
      XSECUP(1) = CHWSUM
C--error on the cross section
      XERRUP(1) = CHWSQ
C--output initialisation information
C--header
      WRITE(*,10)
C--beam parameters
      WRITE(*,11) IDBMUP(1),EBMUP(1),IDBMUP(2),EBMUP(2)
C--basic parameters
      WRITE(*,12) MINMSS,MAXMSS,MPLNCK,THWMAX,RMMINM,MSSDEF,TOTDIM,
     &     YRCSEC,TIMVAR,GRYBDY,KINCUT,RMBOIL
      IF (RMBOIL) THEN
         IF (KINCUT) THEN
            WRITE (*,8)
 8          FORMAT(/10X,'KINEMATIC CUT INCONSISTENT'/
     &              10X,'WITH BOILING REMNANT MODEL:'/
     &              10X,'RESETTING KINCUT = .FALSE.'/)
            KINCUT=.FALSE.
         ENDIF
      ELSE
         IF (RMMINM.LT.MPLNCK) THEN
            WRITE (*,9)
 9          FORMAT(/10X,'BOILING REMNANT MODEL NOT USED:'/ 
     &              10X,'RESETTING MIN REMNANT MASS = MPLNCK'/)
            RMMINM=MPLNCK
         ENDIF
      ENDIF
C--choice of outgoing particles
      IF(MSSDEC.EQ.1) THEN
         WRITE(*,13)
      ELSEIF(MSSDEC.EQ.2) THEN
         WRITE(*,14)
      ELSEIF(MSSDEC.EQ.3) THEN
         WRITE(*,15)
      ENDIF
C--choice of scale
      IF(GTSCA) THEN
         WRITE(*,17)
      ELSE
         WRITE(*,18)
      ENDIF
C--information on the cross section
      WRITE(*,19) XSECUP(1),XERRUP(1),XMAXUP(1)
C--particle production probabilites
      WRITE(*,20) PQUARK,PLEPT,PNEUT,PGLUON,PGAMMA,PZBOSN,PWBOSN,PHIGGS
      RETURN
C--format statements for the output
 10   FORMAT(//10X,'       CHARYBDIS 1.003   August 2006          ',//,
     &         10X,'Please reference: C.M.Harris, P.Richardson &',/,
     &         10X,'B.R.Webber,JHEP0308(2003)033 [hep-ph/0307305]'/)
 11   FORMAT(/10X,'INPUT CONDITIONS FOR THIS RUN'//
     &        10X,'BEAM 1 (',I8,') ENG. =',F10.2/
     &        10X,'BEAM 2 (',I8,') ENG. =',F10.2/)
 12   FORMAT(
     &     10X,'MINIMUM BH MASS    =',F11.2/
     &     10X,'MAXIMUM BH MASS    =',F11.2/
     &     10X,'PLANCK MASS        =',F11.2/
     &     10X,'MAX HAWKING TEMP   =',F11.2/
     &     10X,'MIN REMNANT MASS   =',F11.2/
     &     10X,'DEFN OF PLANCK MASS=',I5/
     &     10X,'NO. OF DIMENSIONS  =',I5/
     &     10X,'YOSHINO-RYCHKOV C-S=',L5/
     &     10X,'TIME VARIATION     =',L5/
     &     10X,'GREY BODY EFFECTS  =',L5/
     &     10X,'KINEMATIC CUT      =',L5/
     &     10X,'BOILING REMN. MODEL=',L5)
 13   FORMAT(
     &     10X,'ONLY LIGHT SM PARTICLES PRODUCED')
 14   FORMAT(
     &     10X,'ALL SM PARTICLES BUT HIGGS PRODUCED')
 15   FORMAT(
     &     10X,'ALL SM PARTICLES PRODUCED')
      WRITE(*,16) NBODY
 16   FORMAT(
     &     10X,'PRODUCING ',I1,' PARTICLES IN REMNANT DECAY')
 17   FORMAT(
     &     10X,'USING BLACK HOLE RADIUS AS SCALE')
 18   FORMAT(
     &     10X,'USING BLACK HOLE MASS AS SCALE')
 19   FORMAT(
     &     10X,'CROSS SECTION (PB) =',G12.4/
     &     10X,'ERROR IN C-S  (PB) =',G12.4/
     &     10X,'MAXIMUM WEIGHT     =',E12.4)
 20   FORMAT(/10X,'PARTICLE PRODUCTION PROBABILITIES'//
     &        10X,'QUARK                  =',F10.4/
     &        10X,'CHARGED LEPTON         =',F10.4/
     &        10X,'NEUTRINO               =',F10.4/
     &        10X,'GLUON                  =',F10.4/
     &        10X,'PHOTON                 =',F10.4/
     &        10X,'Z BOSON                =',F10.4/
     &        10X,'W BOSON                =',F10.4/
     &        10X,'HIGGS BOSON            =',F10.4)
      END

*DECK  ID>, CHDATA.
*CMZ :-        -21/08/06  10.33.53  by  Peter Richardson
*-- Author :    
C-----------------------------------------------------------------------
      BLOCK DATA CHDATA
C-----------------------------------------------------------------------
C    BLOCK DATA TO SET INITIAL VALUES OF PARAMETERS
C-----------------------------------------------------------------------
      IMPLICIT NONE
#include "Charybdis_i/charybdis1003.inc"
C--Les Houches run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
C--event common block
C--Set MPLNCK and define what is meant by it:
C--MSSDEF=1 means M_GT, MSSDEF=2 means M_DL and MSSDEF=3 means M_D
      DATA MPLNCK /1000.0D0/
      DATA MSSDEF /2/
C--Set number of dimensions (number of EXTRA dimensions is n=TOTDIM-4)
C--TOTDIM can be in the range 6-11
      DATA TOTDIM/6/
C--Use Giddings+Thomas momentum scale for calling pdfs?
C--(See page 12 of hep-ph/0106219)
      DATA GTSCA /.FALSE./
C--Set mass window for black holes produced
      DATA  MINMSS /5000.0D0/
      DATA  MAXMSS /14000.0D0/
C--Set NBODY decay of BH remnant - NBODY can be in range 2-5.
      DATA NBODY /2/
C--Turn time variation of BH Hawking temperature in decay on or off
      DATA TIMVAR /.TRUE./
C--Set which decay products are to be used:
C--MSSDEC=1 gives no heavy particles
C--MSSDEC=2 gives t, W and Z as well
C--MSSDEC=3 gives t, W, Z and Higgs as well  
      DATA MSSDEC /3/
C--Turn grey-body factors on/off
      DATA GRYBDY /.TRUE./
C--Turn kinematic cut-off of decay on (rather than M=MPLANCK cut-off)
      DATA KINCUT /.FALSE./
C--BW mod 16/08/06: new parameters for version 1.003
C--Use Yoshino-Rychkov factors in cross-section
      DATA YRCSEC /.FALSE./
C--Max Hawking temperature
      DATA THWMAX /1000.0D0/
C--Remnant decay model: boiling at THWMAX
      DATA RMBOIL /.FALSE./
C--Min BH mass, ends boiling
      DATA RMMINM /1000.D0/
C--print out option (0=no printout, 1 =errors only, 2=errors+info)
      DATA IBHPRN /1/
C--pdf's for the beams (use the generator default)
C--MUST BE THE SAME FOR BOTH BEAMS
      DATA PDFGUP /-1,-1/
      DATA PDFSUP /-1,-1/
C--Beam particles and energies (only proton and/or antiproton)
      DATA IDBMUP /2212,2212/
      DATA EBMUP /7000.0D0,7000.0D0/
      END
