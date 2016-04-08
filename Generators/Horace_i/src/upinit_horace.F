      SUBROUTINE INITHORACE
c     M. Bellomo - C.M. Carloni Calame (october 2007)
c     Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      character*60 tmpchar
c     #include "GeneratorModules/heprup.inc"
c     LES HOUCHES COMMON BLOCKS
c     Les Houches accord common block hep-ph/0109068
c     User process event commonblock.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &     ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),
     &     PUP(5,MAXNUP),VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/
c     User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &     IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &     LPRUP(MAXPUP)
      SAVE /HEPRUP/      
      logical opyn
      character*100 horaceevents
      DOUBLE PRECISION SS,SQRTS
      COMMON  /SAVV/   SS,SQRTS
      common/horaceinterface/alpha,iu

      write(*,*)'*************************************************'
      WRITE(*,*)'* Horace_i: ATLAS interface to HORACE generator *'
      write(*,*)'*************************************************'

      iconf = 49
      opyn = .TRUE.
      do while(opyn)
         inquire(unit=iconf,OPENED=opyn)
         iconf = iconf + 1
      enddo
      
      iu   = iconf + 1
      opyn = .TRUE.
      do while(opyn)
         inquire(unit=iu,OPENED=opyn)
         iu = iu + 1
      enddo
      
      write(*,*)'==============================================='
      write(*,*)'Reading in horace.conf Horace LHA configuration'
      write(*,*)'file units are ',iconf,' and ',iu
      write(*,*)'==============================================='
      open(iconf,file='horace.conf',status='old')
      read(iconf,*)
      read(iconf,*)
      read(iconf,*)
      read(iconf,*)horaceevents
      
c     linking unit iu to event file
      WRITE(*,*) 'reading HORACE events from ',horaceevents
      open(iu,file=horaceevents,status='old')
      read(iconf,*)tmpchar
      write(*,*)'Events for ',tmpchar
      read(iconf,*)tmpchar
      write(*,*)'selected pdfs are: ',tmpchar
      read(iconf,*)
      read(iconf,*)sqrts
      read(iconf,*)
      read(iconf,*)IDBMUP(1)
      read(iconf,*)IDBMUP(2)
      read(iconf,*)
      read(iconf,*)
      read(iconf,*)idwtup
      read(iconf,*)
      read(iconf,*)xsecup(1)
      read(iconf,*)
      read(iconf,*)xerrup(1)
      read(iconf,*)
      read(iconf,*)xmaxup(1)
      read(iconf,*)
      read(iconf,*)alpha
      
      alpha = 1.d0/alpha
      
      SS=SQRTS**2
      EBMUP(1) = 0.5D0*SQRTS
      EBMUP(2) = 0.5D0*SQRTS
      
c     Set up the external process.
      NPRUP    = 1
      LPRUP(1) = 1001
      
c     PDF'S FOR THE BEAMS; WILL BE EVALUATED USING THE NDNS VARIABLE
c     READ IN EARLIER
      PDFGUP(1) = -1
      PDFGUP(2) = -1
      PDFSUP(1) = -1
      PDFSUP(2) = -1
      
c     PDFGUP(1) = 1
c     PDFGUP(2) = 1
c     PDFSUP(1) = 1
c     PDFSUP(2) = 1
      
      RETURN
      END
