C*************************************************************************
C INITGRAVADD is called by UPINIT from upinit_py.F in Pythia_i
C This indirection allows for a
C joboption configurable process. See example jobOptions in share.
C   

      SUBROUTINE INITGRAVADD
 
C     Initialization of USER process
C     ------------------------------

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)    
C      INTEGER PYK,PYCHGE,PYCOMP

C...Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
C...To be used with CHARYBDIS
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM, GSONLY
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM

C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms, Mp/Ms=PFACTOR, 
C...GRAVEV is a flag to see if it was a gravitationally mediated event
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
C...Parameters used during the run, to be printed in the data files, therefore needed by WRIPAR
      DOUBLE PRECISION UBMUL,WTPA
C...  For chosing clustering algorithm
C      character CLALG*6
      COMMON/RUNPAR/UBMUL,WTPA
      SAVE /RUNPAR/,/MASSES/,/BLACKH/,/BHPARM/
C...use CTEQ5L
C      MSTP(51)=7


C...By default we should have KK-QCD interference
C      NOINT=0
C...To improve statistics for small scales we can generate only grav and no QCD
C     NOQCD=0

C...  Default weight parameter
C      WTPA=1.0D0
C... The Upper integration Bound over KK-modes is UBMUL*MSAT
      UBMUL=1.0D0
C...  Set default extra condition on hardness of hardest jet
C...  when looking for particles in rapidity region between jets 
      KKMASS=-1.0D0

C...  Measure the sum ET outside cones instead of Nch
      ISUMET=0

C...  If not zero, only look at gravitational scatterings from this region.
      GRAVSEL=0

C...  Number of random numbers to throw away for each event.
      IRAND=1

      CMEN = 7000d0
      index=2
      CALL readGravADDparamdbl(index,CMEN)


C...Set CHARYBDIS parameters
      CALL SETCHA(CMEN)
      if(MS.le.0) MS=MPLNCK
      
C...Translation factor between our Mp and MsAtwood in hep-ph/9911231 (Atwood)
C...Mp/MsAtwood=PFACTOR
      PFACTOR=2.0D0**((5.0D0-TOTDIM)/(-2.0D0+TOTDIM))*
     $        3.14159**((4.0D0-TOTDIM)/(-4.0D0+2.0D0*TOTDIM))*
     $        Gammafn(dble(1/2.0D0*(-4.0D0+TOTDIM)))**
     $     ((1.0D0/(2.0D0-TOTDIM)))


C...Atwoods mass MSAT and the Upper integration Bound of KK-modes
C...Changed default cutoff to MPLNCK
      MSAT=MPLNCK/PFACTOR
       if(KKMASS.le.0) KKMASS=MPLNCK*UBMUL


      GSONLY= 0
      index=6
      CALL readGravADDparamint(index,GSONLY)


      if (GSONLY.eq.0) then
         CALL INITCHARYBDIS
      endif

      write(*,*) 'initGravADD params: ',CMEN,' ',MPLNCK,' ',MS  

      RETURN


      END Subroutine

C*************************************************************
C...SETCHA
C...Routine to set charybdis parameters

      SUBROUTINE SETCHA(CMEN)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C...Commonblocks.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
C...To be used with CHARYBDIS
       
      DOUBLE PRECISION CMEN
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100,PI=3.141593D0)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &                IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &                XMAXUP(MAXPUP),LPRUP(MAXPUP)
      INTEGER MSSDEF,NBODY,IPRINT,MSSDEC
      DOUBLE PRECISION MPLNCK,MINMSS,MAXMSS,INTMPL,Gammafn
      LOGICAL TIMVAR,GTSCA,GRYBDY,KINCUT
      COMMON /BHPARM/MPLNCK,MINMSS,MAXMSS,INTMPL,MSSDEF,NBODY,IPRINT,
     &     MSSDEC,TIMVAR,GTSCA,GRYBDY,KINCUT  
      DOUBLE PRECISION RHFACT,BHMASS,GENPOW,FACTOR
      INTEGER TOTDIM, PLDEF
      COMMON /BLACKH/ RHFACT,BHMASS,GENPOW,FACTOR,TOTDIM
C...KKMASS to be used for KK-mass maximum integration, and MSAT as Atwoods Ms, Mp/Ms=PFACTOR, 
C...GRAVEV is a flag to see if it was a gravitationally mediated event
      DOUBLE PRECISION KKMASS,MSAT,PFACTOR,MS
      INTEGER GRAVEV
      COMMON/MASSES/KKMASS,MSAT,PFACTOR,MS,GRAVEV
      DOUBLE PRECISION MEFF,MSPRAT
c      CHARACTER*20 PDFIN
      SAVE  /HEPRUP/
      SAVE /MASSES/,/BLACKH/,/BHPARM/
      

C...Set default value for paremeters

C...Set beam paramters
C      write(*,*) 'CMEN: ',CMEN
      EBMUP(1)  = CMEN/2.0D0
      EBMUP(2)  = CMEN/2.0D0

      IDBMUP(1)=2212
      IDBMUP(2)=2212
      IF ( CMEN.LT.5000.0D0 ) then
         IDBMUP(1)=-2212
         IDBMUP(2)=2212
      endif

c     call get_string (npara,param,value," pdlabel ",PDFIN,'cteq6l1')
c     WRITE(LNHOUT,*) 'Trying LHAPDF/PDFlib for PDF ',PDFIN
c      PDFIN='cteq6l1'
c     call get_lhapdf(PDFIN,MPDF)
c
c      IF(MPDF.NE.-1)THEN
c         PDFGUP(1)=MPDF/10000
c         PDFSUP(1)=MOD(MPDF,10000)
c         PDFGUP(2)=PDFGUP(1)
c         PDFSUP(2)=PDFSUP(1)
c      ELSE
c         WRITE(*,*) 'Error: PDF ',PDFIN,' not found. Quitting.'
c         STOP
c      ENDIF
c      MSTP(52)=2
c      MSTP(51)=MPDF


C...  The effective mass scale for gravitational interactions. If larger
C...  than zero this will be used together with MSPRAT to calculate
C...  MPLNCK and MS.
      MEFF=-1.0D0
      MSPRAT=2.0D0
C...  Our cut-off scale (We want to change Ms after running 
C...  SETCHA (such that MPLNCK is set) only if it was not set
C..   while reading parmeters
      MS=-1.0D0

C...  IF MSBHCUT is set, MINMSS will also be influenced by MS
      MSBHCUT=0

C--MSSDEF=1 means M_GT, MSSDEF=2 means M_DL and MSSDEF=3 means M_D
      MPLNCK=1000.0D0
      MSSDEF=2
C--Set number of dimensions (number of EXTRA dimensions is n=D-4)
C--TOTDIM can be in the range 6-11
      TOTDIM=6

C--Set mass window for black holes produced
c      MINMSS=5000.0D0
C...MAXMASS is set in UPINIT since EBMUP(1)+EBMUP(2) are not defined here
      MAXMSS=EBMUP(1)+EBMUP(2) 
C--Use Giddings+Thomas momentum scale for calling pdfs?
C--(See page 12 of hep-ph/0106219)
      GTSCA=.TRUE.
C--Set NBODY decay of BH remnant - NBODY can be in range 2-5.
      NBODY=2
C--Turn time variation of BH Hawking temperature in decay on or off
      TIMVAR=.TRUE.
C--Set which decay products are to be used:
C--MSSDEC=1 gives no heavy particles
C--MSSDEC=2 gives t, W and Z as well
C--MSSDEC=3 gives t, W, Z and Higgs as well  
      MSSDEC=3
C--Turn grey-body factors on/off
      GRYBDY=.TRUE.
C--Turn kinematic cut-off of decay on (rather than M=MPLANCK cut-off)
      KINCUT=.TRUE.
      

      TOTDIM = 10
      index=1
      CALL readGravADDparamint(index,TOTDIM)
      
      MEFF = 1000d0
      index=4
      CALL readGravADDparamdbl(index,MEFF)

      MSPRAT = 1d0
      index=3
      CALL readGravADDparamdbl(index,MSPRAT)

C... GravADD is internally working with Planck mass definition from hep-ph/0608210
C... If the users sets Planck mass definition to PDG choice, then also Ms/Mp is PDG
C... Convert Ms/Mp to original definition
      NE=TOTDIM-4
      PLDEF = 1
      index=5
      CALL readGravADDparamint(index,PLDEF)
      if (PLDEF.eq.2) then
        FAC = (2.0d0**((NE-3.0d0)/(NE+2.0d0)))
     $        *(PI**((NE-1.0d0)/(NE+2.0d0)))
        MSPRAT = MSPRAT*FAC
      endif 
      
      MSBHCUT=1
      FACTOR=1.0D0  

     
      if ( MEFF.GT.0.0D0 ) then
        FAC=0.5D0*(((NE-2.0d0)*2.0D0**NE)*(PI**(NE/2.0d0-1.0d0))*
     $       (MSPRAT**(2.0d0-NE)))**0.25d0
        MPLNCK=MEFF/FAC
        MS=MPLNCK*MSPRAT
      endif
      IF ( MS.LT.0.0D0 ) MS=MPLNCK*MSPRAT



      write(*,*) 'in SETCHA: ',TOTDIM,' ',MEFF,' ',MSPRAT


C... When MPLNCK and TOTDIM are known set the mimimal mass
C...Number of extra dimensions
       MINMSS=MPLNCK*2.0D0**((ne + 1.0D0)/(ne + 2.0D0))
     &      *(1/(sqrt(3.14159))*
     &      (8.0D0*Gammafn(dble((ne+3.0D0)/2.0D0))/(ne + 2.0D0))
     &      **(1.0D0/(ne + 1.0D0)))**(-((ne + 1.0D0)/(ne + 2.0D0)))

       IF ( FACTOR.LT.0.0D0 ) THEN
         FACTOR=-FACTOR
       ELSE
         MINMSS=MAX(MPLNCK,
     $        MINMSS/(SQRT(FACTOR)**((ne + 1.0D0)/(ne + 2.0D0))))
       ENDIF

       if ( MSBHCUT.EQ.1 ) THEN
         MINMSS=MAX(MINMSS,(MPLNCK**(2+NE))*(MS**(-NE-1))*(2.0d0+NE)*
     $                     (PI**((NE+1)/2.0D0))/
     $                     (16.0d0*GAMMAFN(0.5d0*(3.0d0+NE))))
       endif
       MINMSS=MIN(MINMSS,MAXMSS)
       GENPOW=1.0D0

       RETURN
       END
