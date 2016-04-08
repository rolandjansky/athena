      SUBROUTINE USEHORACE
c     M. Bellomo - C.M. Carloni Calame (october 2007)
c     Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c     #include "GeneratorModules/heprup.inc"
c     #include "GeneratorModules/hepeup.inc"
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
      common/horaceinterface/alpha,iu
c      double precision pw(0:3),p3(0:3),p4(0:3),qph(40,0:3),ptmp(0:3)
      character*1 tic
      DOUBLE PRECISION Q2,ALEM,ALPS
      COMMON /TRASFVAR/ Q2, ALEM, ALPS
c     Constants  for run 
      DOUBLE PRECISION SS,SQRTS
      COMMON  /SAVV/   SS,SQRTS
c      INTEGER ISUB
      INTEGER I
c     Counters
      INTEGER NNT1,NNT2
      DOUBLE PRECISION SIGSUM
      COMMON/COUNTER/NNT1,NNT2,SIGSUM
      data nnt1,nnt2,sigsum /0,0,0.d0/
c     save kinematics for ntuples
      DOUBLE PRECISION PSHAT,COSTH,X1,X2,EE1,SIGEVS,RABS
      COMMON/SAVMAN/PSHAT,COSTH,X1,X2,EE1,SIGEVS,RABS
c      DOUBLE PRECISION SIGWE 
c      DOUBLE PRECISION PP1(4),PP2(4)
      DOUBLE PRECISION ETA1,ETA2,PTLEP
c      DOUBLE PRECISION qtmp(0:3),minv
      COMMON/CUCU/ETA1,ETA2,PTLEP

c     alpha for QED radiation from HORACE
      AQEDUP = alpha            
c     AQCDUP = ALPS

      read(iu,*)tic,nev,npart,XWGTUP,SCALUP
      read(iu,*)idup(1),icolup(1,1),icolup(2,1),pz1
      read(iu,*)idup(2),icolup(1,2),icolup(2,2),pz2
      
      pup(1,1) = 0.d0
      pup(2,1) = 0.d0
      pup(3,1) = pz1
      pup(4,1) = sqrt(pz1**2)
      pup(5,1) = 0.d0

      pup(1,2) = 0.d0
      pup(2,2) = 0.d0
      pup(3,2) = pz2
      pup(4,2) = sqrt(pz2**2)
      pup(5,2) = 0.d0
      
      NUP = npart + 1

      do i = 3+1,NUP
         read(iu,*)idup(i),icolup(1,i),icolup(2,i),
     $        pup(1,i),pup(2,i),pup(3,i),pup(5,i)
         pup(4,i)=sqrt(pup(1,i)**2+pup(2,i)**2+pup(3,i)**2+pup(5,i)**2)
      enddo

c     trick to get correct mother pdg id for W and Z processes
      if(abs(idup(4)).eq.abs(idup(5))) then
         idup(3) = 23
      else
         if(abs(idup(4)).eq.13) then
            idup(3) = -((idup(4)/abs(idup(4)))*24)
         else if(abs(idup(5)).eq.13) then
            idup(3) = -((idup(5)/abs(idup(5)))*24)
         endif   
      endif
c     "intermediate" boson, invariant mass to be conserved, sum of lepton momenta!!
c     see hep-ph/0109068 -- intermediate particles have idup = 0
      
c      idup(3)  = 0
      pup(1,3) = pup(1,4) + pup(1,5)
      pup(2,3) = pup(2,4) + pup(2,5)
      pup(3,3) = pup(3,4) + pup(3,5)
      pup(4,3) = pup(4,4) + pup(4,5)
      am2 = pup(4,3)**2 - pup(3,3)**2 - pup(2,3)**2 - pup(1,3)**2
      pup(5,3) = am2/abs(am2) * sqrt(abs(am2))
     
      PSHAT=(pup(4,1) + pup(4,2))**2
      
      SIGSUM = SIGSUM+XWGTUP
      
      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)= 2  ! intermeditate time-like boson
      do k = 4,NUP
         ISTUP(k) = 1         
      enddo
      
c     Mother codes.
      MOTHUP(1,1) = 0
      MOTHUP(2,1) = 0
      MOTHUP(1,2) = 0
      MOTHUP(2,2) = 0
      MOTHUP(1,3) = 1
      MOTHUP(2,3) = 2
      MOTHUP(1,4) = 3
      MOTHUP(2,4) = 3
      MOTHUP(1,5) = 3
      MOTHUP(2,5) = 3
      
      if (NUP.gt.5) then
         do k = 6,NUP
            MOTHUP(1,k) = 3
            MOTHUP(2,k) = 3
         enddo
      endif
      
      RETURN
      END
