       SUBROUTINE INITBC
 
C...UPINIT
C...Routine to be called by user to set up user-defined processes.
C...Code below only intended as example, without any claim of realism.
C...Especially it shows what info needs to be put in HEPRUP.
 
CC      SUBROUTINE UPINIT
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...User process initialization commonblock.
      INTEGER MAXPUP,PYCOMP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...Extra commonblock to transfer run info.
      COMMON/PRIV/MODE,NLIM
     &         ,amb,amc,ambc,tau0
     &         ,asqcd,scalbc,PTCUT,IPAR1,IPAR2
      SAVE/PRIV/

C....Pythia commonblock - needed for setting PDF's; see below.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/      
      PARAMETER (NSUB=500,NPAR=8000,NMDCY=3,NMDME=2,NKFDP=5)
      INTEGER MDCY,MDME,KFDP
      DOUBLE PRECISION BRAT
      COMMON/PYDAT3/MDCY(NSUB,NMDCY),MDME(NPAR,NMDME),BRAT(NPAR),
     &     KFDP(NPAR,NKFDP)
                             

*
      real*8 asqcd,scalbc
*
c      external pydata
*
C.. Initial parameters
      IPAR1 = 0
      IPAR2 = 0
      ECM = 7000.
      DPAR1 = 0.
      PTCUT = 10.
      DPAR2 = 1.5d0
      DPAR3 = 4.8d0


C...Set incoming beams: LHC
      IDBMUP(1)=2212
      IDBMUP(2)=2212
c      EBMUP(1)=5000D0
c      EBMUP(2)=5000D0

      index=1
      CALL readpythiabcparamint(index,IPAR1)

      index=2
      CALL readpythiabcparamint(index,IPAR2)

      index=3
      CALL readpythiabcparamdbl(index,ECM)

      EBMUP(1) = ECM/2.
      EBMUP(2) = ECM/2.

      index=4
      CALL readpythiabcparamdbl(index,DPAR1)

      index=5
      CALL readpythiabcparamdbl(index,PTCUT)

      index=6
      CALL readpythiabcparamdbl(index,DPAR2)

      index=7
      CALL readpythiabcparamdbl(index,DPAR3)

      amc=DPAR2
      amb=DPAR3
      ambc=amb+amc

      write(6,*) ' >>> INITBc: '
      write(6,*) ' IPAR1, IPAR2, ECM, DPAR1, PTCUT, M_c, M_b = ',
     & IPAR1, IPAR2, ECM, DPAR1, PTCUT, DPAR2, DPAR3
      write(6,*) ' amc, amb, ambc ', amc, amb, ambc

c   MAKE Bc stable for EvtGen
      IF(IPAR2.eq.0)then
         MDCY(pycomp(541),1)=0    ! stable
        write(6,*) 'Bc (KF=541) is now stable'
      endif

C...Set PDF's of incoming beams: CTEQ 4L.
C...Note that Pythia will not look at PDFGUP and PDFSUP.  
C      PDFGUP(1)=4
C      PDFSUP(1)=32
C      PDFGUP(2)=PDFGUP(1)
C      PDFSUP(2)=PDFSUP(1)
      
C...If you want Pythia to use PDFLIB, you have to set it by hand.
C...(You also have to ensure that the dummy routines
C...PDFSET, STRUCTM and STRUCTP in Pythia are not linked.)      
C      MSTP(52)=2
C      MSTP(51)=1000*PDFGUP(1)+PDFSUP(1)

C...Decide on weighting strategy: unweighted on input.
      IDWTUP=1

C...Number of external processes. 
      NPRUP=1

C...Set up gg --> Bc b c~
      XSECUP(1)=0.D0
      if(IPAR1.eq.0)then
*         XMAXUP(1)=4.3D8
         XMAXUP(1)=2.1D7
      else 
         XMAXUP(1)=1.7D7
      endif
*      XMAXUP(1)=4.3D8
*      XMAXUP(1)=4.D7
*      XMAXUP(1)=1.7D7
      LPRUP(1)=661
* some initializations: masses of b, c, Bc
c      amb=4.8d0
c      amc=1.5d0
c      ambc=amb+amc
      
c      PMAS(PYCOMP(541),1)=ambc
c      PMAS(PYCOMP(543),1)=ambc
c      PMAS(PYCOMP(-541),1)=ambc
c      PMAS(PYCOMP(-543),1)=ambc
      
* more initializations: something for x1,x2 generation
      x1x2min=((ambc+amb+amc)/2.d0/ebmup(1))**2
      tau0=x1x2min
*
      RETURN
      END
