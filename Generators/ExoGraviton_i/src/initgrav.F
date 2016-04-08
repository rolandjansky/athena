   

      SUBROUTINE INITGRAV
 
C     Authors: Pierre-Hugues Beauchemin<pierre-hugues.beauchemin@cern.ch>
C              Georges Azuelos<georges.azuelos@umontreal.ca>

C     Initialization of USER process
C     ------------------------------


C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16

C...User process initialization common block.
      INTEGER MAXPUP,INDEX
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

C.. User
      COMMON/USER/SIGMAX, ECM, sigev2, sqrshat
      common /user2/sigtot, ebeam, amgravmin, amgravmax,
     +              ptcut, shatmin, xmin, shatmax, tmin, tmax,
     +              phspv, amd, coupl, ccoupl, g, g5, ndim, ntot, 
     +              iproc, igrav, iq, icutoff
      save /user2/
      dimension xp1(-25:25),xp2(-25:25), sigi(-5:5,-5:5)
      COMMON /USER3/ PT2, Q2, shat, that, uhat, sig0, wgt1, wgt2,
     +               conv, xp1, xp2, sigi, amgrav, amgrav2, costhe,
     +               F1, F2, F3


C....Pythia common block - needed for setting PDF's; see below.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/      

C.. Initial parameters
      NDIM = 2
      IPROC = 1111
      SIGMAX = 2.5E+1
      AMGRAVMIN = 0.
      AMGRAVMAX = 3500.
      PTCUT = 300.
      ECM = 7000.
      AMD = 3500.
      ICUTOFF = 0


C...Set incoming beams: LHC.
      IDBMUP(1)=2212
      IDBMUP(2)=2212
      EBMUP(1)=5000D0
      EBMUP(2)=5000D0

      index=1
      CALL readgravparamint(index,NDIM)

      index=2
      CALL readgravparamint(index,IPROC)

      index=3
      CALL readgravparamdbl(index,ECM)
      EBMUP(1) = ECM/2.
      EBMUP(2) = ECM/2.

      index=4
      CALL readgravparamdbl(index,AMD)

      index=5
      CALL readgravparamdbl(index,PTCUT)

      index=6
      CALL readgravparamdbl(index,AMGRAVMAX)

      index=7
      CALL readgravparamdbl(index,SIGMAX)

      index=8
      CALL readgravparamint(index,ICUTOFF)


      write(6,*) ' >>> INITGrav: '
      write(6,*) ' NDIM, IPROC, AMGRAVMAX, PTCUT, AMD, CutOff = ',
     & NDIM, IPROC, AMGRAVMAX, PTCUT, AMD, ICUTOFF 



C...Decide on weighting strategy: weighted on input, cross section unknown.
      IDWTUP=1
C...Decide on weighting strategy: weighted on input, cross section known.
c      IDWTUP=2

C...Number of different external processes. 
      NPRUP=1

C...Set up Graviton
C ======================
C.. cross section unknown [XSECUP:cross section for each external process,in pb]
      XSECUP(1) = 0.
C.. stat. error in cross section for each external process,in pb (Statistical Error)
      XERRUP(1) = 0.
C.. max. value of cross section (pb)
      XMAXUP(1) = SIGMAX
C.. process ID
      LPRUP(1)=IPROC


      RETURN

      END Subroutine

