      
C...UPEVNT
C...Routine called by PYEVNT or PYEVNW to get user process event
      
      SUBROUTINE USEMADGRAPH

      IMPLICIT NONE

C...Pythia parameters.
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/
C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)
C...Pythia common blocks
      INTEGER PYCOMP,KCHG,MINT,NPART,NPARTD,IPART,MAXNUR
      DOUBLE PRECISION PMAS,PARF,VCKM,VINT,PTPART
C...Particle properties + some flavour parameters.
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYINT1/MINT(400),VINT(400)
      PARAMETER (MAXNUR=1000)
      COMMON/PYPART/NPART,NPARTD,IPART(MAXNUR),PTPART(MAXNUR)
      save /pypart/
C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Inputs for the matching algorithm
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

C...Commonblock to transfer event-by-event matching info
      INTEGER NLJETS,IEXC,Ifile
      DOUBLE PRECISION PTCLUS
      COMMON/MEMAEV/PTCLUS(20),NLJETS,IEXC,Ifile

C...Local variables
      INTEGER I,J,IBEG,NEX,KP(MAXNUP),MOTH,NUPREAD,II,iexcl
      INTEGER irem(5),nrem
      DOUBLE PRECISION PSUM,ESUM,PM1,PM2,A1,A2,A3,A4,A5
      DOUBLE PRECISION SCALLOW(MAXNUP),PNONJ(4),PMNONJ!,PT2JETS
c$$$C...Local variables
c$$$      INTEGER I,J,IBEG,NEX,KP(MAXNUP),MOTH,NUPREAD,II,iexcl
c$$$      DOUBLE PRECISION PSUM,ESUM
C...Lines to read in assumed never longer than 200 characters. 
      INTEGER MAXLEN
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING

C...Functions
      INTEGER iexclusive
      EXTERNAL iexclusive

C...Format for reading lines.
      CHARACTER*6 STRFMT
      CHARACTER*1 CDUM

      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN

C...Loop until finds line beginning with "<event>" or "<event ". 
  100 READ(LNHIN,STRFMT,END=900,ERR=900) STRING
      IBEG=0
  110 IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-6) GOTO 110 
      IF(STRING(IBEG:IBEG+6).NE.'<event>'.AND.
     &STRING(IBEG:IBEG+6).NE.'<event ') GOTO 100

C...Read first line of event info.
      READ(LNHIN,*,END=900,ERR=900) NUPREAD,IDPRUP,XWGTUP,SCALUP,
     &AQEDUP,AQCDUP

C...Read NUP subsequent lines with information on each particle.
      ESUM=0d0
      PSUM=0d0
      NEX=2
      NUP=1
      NREM=0
      DO 120 I=1,NUPREAD
         READ(LNHIN,*,END=900,ERR=900) IDUP(NUP),ISTUP(NUP),
     &        MOTHUP(1,NUP),MOTHUP(2,NUP),ICOLUP(1,NUP),ICOLUP(2,NUP),
     &        (PUP(J,NUP),J=1,5),VTIMUP(NUP),SPINUP(NUP)
C...  Reset resonance momentum to prepare for mass shifts
         DO J=1,nremres
            IF(ISTUP(NUP).EQ.2 .AND. IDUP(NUP).EQ.remres(J)) THEN
               NREM=NREM+1
               IF(NREM.GT.5)THEN
                  WRITE(*,*)'TOO MANY REMOVED RESONANCES IN ONE EVENT'
                  NREM=NREM-1
                  EXIT
               ENDIF
               IREM(NREM)=I
               GOTO 120
            ENDIF
         ENDDO
         IF(ISTUP(NUP).EQ.2) PUP(3,NUP)=0
         IF(ISTUP(NUP).EQ.1)THEN
            NEX=NEX+1
            IF(PUP(5,NUP).EQ.0D0.AND.IABS(IDUP(NUP)).GT.3
     $           .AND.IDUP(NUP).NE.21) THEN
C...  Set massless particle masses to Pythia default. Adjust z-momentum. 
               PUP(5,NUP)=PMAS(IABS(PYCOMP(IDUP(NUP))),1)
               PUP(3,NUP)=SIGN(SQRT(MAX(0d0,PUP(4,NUP)**2-PUP(5,NUP)**2-
     $              PUP(1,NUP)**2-PUP(2,NUP)**2)),PUP(3,NUP))
            ENDIF
            PSUM=PSUM+PUP(3,NUP)
C...  Set mother resonance momenta
            MOTH=MOTHUP(1,NUP)
            DO J=1,NREM
               IF (MOTH .eq. IREM(J)) THEN
                  MOTHUP(1,NUP)=1
                  MOTHUP(2,NUP)=2
                  MOTH=1
               ENDIF
            ENDDO
            DO J=NREM,1,-1
               IF(MOTH.GT.IREM(J)) THEN
                  MOTH=MOTH-1
                  MOTHUP(1,NUP)=MOTHUP(1,NUP)-1
                  MOTHUP(2,NUP)=MOTHUP(2,NUP)-1
               ENDIF
            ENDDO 
            DO WHILE (MOTH.GT.2)
               PUP(3,MOTH)=PUP(3,MOTH)+PUP(3,NUP)
               MOTH=MOTHUP(1,MOTH)
            ENDDO
         ENDIF
         NUP=NUP+1
 120  CONTINUE
      NUP=NUP-1

C...  Increment event number
      IEVNT=IEVNT+1

C..   Adjust mass of resonances
      DO I=1,NUP
         IF(ISTUP(I).EQ.2)THEN
            PUP(5,I)=SQRT(PUP(4,I)**2-PUP(1,I)**2-PUP(2,I)**2-
     $           PUP(3,I)**2)
         ENDIF
      ENDDO
      
C...  Adjust energy and momentum of incoming particles
C...  In massive case need to solve quadratic equation
c     PM1=PUP(5,1)**2
c     PM2=PUP(5,2)**2
c     A1=4d0*(ESUM**2-PSUM**2)
c     A2=ESUM**2-PSUM**2+PM2-PM1
c     A3=2d0*PSUM*A2
c     A4=A3/A1
c     A5=(A2**2-4d0*ESUM**2*PM2)/A1
c     
c     PUP(3,2)=A4+SIGN(SQRT(A4**2+A5),PUP(3,2))
c      PUP(3,1)=PSUM-PUP(3,2)
c     PUP(4,1)=SQRT(PUP(3,1)**2+PM1)
c     PUP(4,2)=SQRT(PUP(3,2)**2+PM2)
      
      ESUM=PUP(4,1)+PUP(4,2)
      
C...  Assuming massless incoming particles - otherwise Pythia adjusts
C...  the momenta to make them massless
c     IF(IDBMUP(1).GT.100.AND.IDBMUP(2).GT.100)THEN
c     DO I=1,2
c     PUP(3,I)=0.5d0*(PSUM+SIGN(ESUM,PUP(3,I)))
c     PUP(5,I)=0d0
c     ENDDO
c     PUP(4,1)=ABS(PUP(3,1))
c     PUP(4,2)=ESUM-PUP(4,1)
c     ENDIF
      
C...  If you want to use some other scale for parton showering then the 
C...  factorisation scale given by MadEvent, please implement the function PYMASC
C...  (example function included below) 
      
      IF(ickkw.eq.0.AND.MSCAL.GT.0) CALL PYMASC(SCALUP)
c      IF(MINT(35).eq.3.AND.ickkw.EQ.1) SCALUP=SQRT(PARP(67))*SCALUP
      
C...  Read FSR scale for all FS particles (as comment in event file)
      IF(ickkw.eq.1)THEN
         READ(LNHIN,*,END=900,ERR=130) CDUM,(PTPART(I),I=1,NEX)
 130     CONTINUE
      ENDIF
      
      IF(ickkw.gt.0) THEN
c     
c     Set up number of jets
c     
         NLJETS=0
         NPART=0
         do i=3,NUP
            if(ISTUP(i).ne.1) cycle
            NPART=NPART+1
            IPART(NPART)=i
            if(iabs(IDUP(i)).gt.nqmatch.and.IDUP(i).ne.21) cycle
            if(MOTHUP(1,i).gt.2) cycle
C     Remove final-state partons that combine to color singlets
            IF((ABS(IDBMUP(1)).NE.11.OR.IDBMUP(1).NE.-IDBMUP(2)).AND.
     $           nosingrad) THEN
               DO II=3,NUP
                  IF(II.NE.i.AND.ISTUP(II).EQ.1)THEN
                     IF((IDUP(II).EQ.-IDUP(i).OR.
     $                    IDUP(i).EQ.21.AND.IDUP(II).EQ.21).AND.
     $                    ICOLUP(1,II).EQ.ICOLUP(2,i).AND.
     $                    ICOLUP(2,II).EQ.ICOLUP(1,i))then
c     print *,'Found color singlet'
                        CALL PYLIST(7)
                        GOTO 140
                     endif
                  ENDIF
               ENDDO
            ENDIF
            NLJETS=NLJETS+1
            PTCLUS(NLJETS)=PTPART(NPART)
 140        continue
         enddo
         CALL ALPSOR(PTCLUS,nljets,KP,1)
         
         if(jetprocs) IDPRUP=LPRUP(NLJETS-MINJETS+1)
         
         IF(ickkw.eq.1) THEN
c     ... and decide whether exclusive or inclusive
            iexcl=iexclusive(IDPRUP)
            if((IEXCFILE.EQ.0.and.NLJETS.eq.MAXJETS.or.
     $           iexcl.eq.0).and.
     $           iexcl.ne.1)then
               IEXC=0
            else if(iexcl.eq.-1)then
               IEXC=-1
            else
               IEXC=1
            endif
         ENDIF
      ENDIF
      
      RETURN
      
C...  Error exit, typically when no more events.
 900  WRITE(*,*) ' Failed to read LHEF event information,'
      WRITE(*,*) ' assume end of file has been reached.'
      NUP=0
      MINT(51)=2
      RETURN
      END

C*********************************************************************
C...UPVETO
C...Subroutine to implement the MLM jet matching criterion
C*********************************************************************
      SUBROUTINE VETOMADGRAPH(IPVETO)

      IMPLICIT NONE

C...Pythia common blocks
      INTEGER MINT
      DOUBLE PRECISION VINT
      COMMON/PYINT1/MINT(400),VINT(400)
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

C...GUP Event common block
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &              IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &              ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &              SPINUP(MAXNUP)
C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...HEPEVT commonblock.
      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
      INTEGER IPVETO
c$$$C...GETJET commonblocks
      INTEGER NCJET
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI,NJMAX,JETNO,NCJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL,PCJET,ETJET
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALORM/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI
c$$$      PARAMETER (NJMAX=500)
c$$$      COMMON/GETCOMM/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(MNCY,MNCPHI),
c$$$     $NCJET
      DOUBLE PRECISION PI
      PARAMETER (PI=3.141593D0)
c$$$C     
      DOUBLE PRECISION PSERAP
c$$$      INTEGER K(NJMAX),KP(NJMAX),kpj(njmax)

C...Variables for the kT-clustering
      INTEGER NMAX,NN,NSUB,JET,NJETM,IHARD,IP1,IP2
      DOUBLE PRECISION PP,PJET
      DOUBLE PRECISION ECUT,Y,YCUT,RAD
      PARAMETER (NMAX=512)
      DIMENSION JET(NMAX),Y(NMAX),PP(4,NMAX),PJET(4,NMAX),
     $   PJETM(4,NMAX)
      INTEGER NNM
      DOUBLE PRECISION YM(NMAX),PPM(4,NMAX),PJETM

C...kt clustering common block
      INTEGER NMAXKT,NUM,HIST
      PARAMETER (NMAXKT=512)
      DOUBLE PRECISION PPP,KT,ETOT,RSQ,KTP,KTS,KTLAST
      COMMON /KTCOMM/ETOT,RSQ,PPP(9,NMAXKT),KTP(NMAXKT,NMAXKT),
     $   KTS(NMAXKT),KT(NMAXKT),KTLAST(NMAXKT),HIST(NMAXKT),NUM

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Inputs for the matching algorithm
C   clfact determines how jet-to parton matching is done
C   kt-jets: default=1
C    clfact >= 0: Max mult. if within clfact*max(qcut,Q(partNmax)) from jet, others within clfact*qcut
C    clfact < 0: Max mult. if within |clfact|*Q(jetNmax) from jet, other within |clfact|*qcut
C   cone-jets: default=1.5
C    Matching if within clfact*RCLMAX 

      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs

C...Commonblock to transfer event-by-event matching info
      INTEGER NLJETS,IEXC,Ifile
      DOUBLE PRECISION PTCLUS
      COMMON/MEMAEV/PTCLUS(20),NLJETS,IEXC,Ifile

      INTEGER nvarev,nvar2
      PARAMETER (nvarev=57,nvar2=6)

      REAL*4 varev(nvarev)
      COMMON/HISTDAT/varev


C   local variables
      double precision tiny
      parameter (tiny=1d-3)
      integer icount
      data icount/0/
      integer idbg
      data idbg/0/

      integer i,j,ihep,nmatch,jrmin,KPT(MAXNUP),nres,ii
      double precision etajet,phijet,delr,dphi,delrmin,ptjet
      double precision p(4,10),pt(10),eta(10),phi(10)
      INTEGER ISTOLD(NMXHEP),IST,IMO
      logical norad(20)
      REAL*4 var2(nvar2)

c      if(NLJETS.GT.0)then
c        idbg=1
c      else
c        idbg=0
c      endif

      IPVETO=0
c     Return if not MLM matching (or non-matched subprocess)

cccc      WRITE(LNHOUT,*) '*** Entering UPVETO *** '
 
      IF(ICKKW.LE.0.OR.IEXC.eq.-1) RETURN

      IF(NLJETS.LT.MINJETS.OR.NLJETS.GT.MAXJETS)THEN
        if(idbg.eq.1)
     $     WRITE(LNHOUT,*) 'Failed due to NLJETS ',NLJETS,' < ',MINJETS,
     $        ' or > ',MAXJETS
         GOTO 999
      ENDIF

C   Throw event if it contains an excluded resonance
      NRES=0
      DO I=1,NUP
        IF(ISTUP(I).EQ.2)THEN
           DO J=1,nexcres
              IF(IDUP(I).EQ.EXCRES(J)) NRES=NRES+1
           ENDDO
        ENDIF
      ENDDO
      IF(NRES.GT.0)THEN
         if(idbg.eq.1)
     $        PRINT *,'Event',IEVNT,' thrown because of ',NRES,
     $        ' excluded resonance(s)'
c     CALL PYLIST(7)
         GOTO 999
      ENDIF

C   Set up vetoed mothers
c      DO I=1,MAXNUP
c        INORAD(I)=0
c      ENDDO
c      DO IHEP=1,NUP-2      
c        if(ISTHEP(ihep).gt.1.and.iabs(IDHEP(ihep)).gt.8) then
c        if(iabs(IDHEP(ihep)).gt.5.and.IDHEP(ihep).ne.21) then
c          INORAD(ihep)=1
c        endif
c      ENDDO

C
c     reconstruct parton-level event
c     Set norad for daughters of decayed particles, to not include
c     radiation from these in matched jets
c
      if(idbg.eq.1) then
        write(LNHOUT,*) ' '
        write(LNHOUT,*) 'new event '
c        CALL PYLIST(1)
        CALL PYLIST(7)
        CALL PYLIST(5)
        write(LNHOUT,*) 'PARTONS'
      endif
      i=0
      do ihep=3,nup
         NORAD(ihep)=.false.
        if((ABS(IDBMUP(1)).NE.11.OR.IDBMUP(1).NE.-IDBMUP(2)).AND.
     $        MOTHUP(1,ihep).gt.2) goto 100
        if(ISTUP(ihep).ne.1.or.
     $     (iabs(IDUP(ihep)).gt.nqmatch.and.IDUP(ihep).ne.21)) cycle
c     If quark or gluon making singlet system with other final-state parton
c     remove (since unseen singlet resonance) unless e+e- collision
        IF((ABS(IDBMUP(1)).NE.11.OR.IDBMUP(1).NE.-IDBMUP(2)).AND.
     $       nosingrad)THEN
           DO II=3,NUP
              IF(II.NE.ihep.AND.ISTUP(II).EQ.1)THEN
                 IF((IDUP(II).EQ.-IDUP(ihep).OR.
     $                IDUP(ihep).EQ.21.AND.IDUP(II).EQ.21).AND.
     $                ICOLUP(1,II).EQ.ICOLUP(2,ihep).AND.
     $                ICOLUP(2,II).EQ.ICOLUP(1,ihep))
     $                GOTO 100
              ENDIF
           ENDDO
        ENDIF
        i=i+1
        do j=1,4
          p(j,i)=pup(j,ihep)
        enddo
        pt(i)=sqrt(p(1,i)**2+p(2,i)**2)
        if(i.LE.4) varev(50+i)=pt(i)
        eta(i)=-log(tan(0.5d0*atan2(pt(i)+tiny,p(3,i))))
        phi(i)=atan2(p(2,i),p(1,i))
        if(idbg.eq.1) then
          write(LNHOUT,*) pt(i),eta(i),phi(i)
        endif
        cycle
 100    norad(ihep)=.true.
      enddo
      if(i.ne.NLJETS)then
        print *,'Error in UPVETO: Wrong number of jets found ',i,NLJETS
        CALL PYLIST(7)
        CALL PYLIST(2)
        stop
      endif

C Bubble-sort PTs in descending order
      DO I=1,3
         DO J=4,I+1,-1
            IF(varev(50+J).GT.varev(50+I))THEN
               PTJET=varev(50+J)
               varev(50+J)=varev(50+I)
               varev(50+I)=PTJET
            ENDIF
         ENDDO
      ENDDO

C     Set status for non-clustering partons to 2
      DO ihep=1,NHEP
c         ISTORG(ihep)=ISTHEP(ihep)
         IF(ISTHEP(ihep).EQ.1.AND.iabs(IDHEP(ihep)).GT.5.AND.
     $        IDHEP(ihep).NE.21) THEN
            ISTHEP(ihep)=2
         ELSEIF(ISTHEP(ihep).EQ.1.AND.JMOHEP(1,ihep).GT.0) then
            IMO=JMOHEP(1,ihep)
            DO WHILE(IMO.GT.0)
c           Trace mothers, if non-radiating => daughter is decay - remove
              IF(IMO.le.NUP-2.and.norad(IMO+2)) GOTO 105
              IMO=JMOHEP(1,IMO)
            ENDDO
            cycle
 105        ISTHEP(ihep)=2
         ENDIF
      ENDDO

C     Prepare histogram filling
        DO I=1,4
          var2(1+I)=-1
          varev(46+I)=-1
          varev(50+I)=-1
        ENDDO

      I=0
      if(idbg.eq.1) then
        do i=1,nhep
          write(LNHOUT,1000)i,isthep(i),idhep(i),jmohep(1,i),jmohep(2,i)
     $         ,phep(1,i),phep(2,i),phep(3,i)
        enddo
 1000   format(5(i4,1x),3(f12.5,1x))
      endif

      IF(ICKKW.EQ.2) GOTO 150

      IF(MSTP(61).eq.0..and.MSTP(71).eq.0)then
c     No showering - just print out event
      ELSE IF(qcut.le.0d0)then
           PRINT *,' ERROR: qcut < 0, --> STOP'
           STOP
c$$$         
c$$$         IF(clfact.EQ.0d0) clfact=1.5d0
c$$$         
c$$$c     CALL PYLIST(7)
c$$$c     CALL PYLIST(2)
c$$$c     CALL PYLIST(5)
c$$$c     Start from the partonic system
c$$$         IF(NLJETS.GT.0) CALL ALPSOR(pt,nljets,KP,2)  
c$$$c     reconstruct showered jets
c$$$c     
c$$$         YCMAX=ETACLMAX+RCLMAX
c$$$         YCMIN=-YCMAX
c$$$         CALL CALINIM
c$$$         CALL CALDELM(1,1)
c$$$         CALL GETJETM(RCLMAX,ETCJET,ETACLMAX)
c$$$c     analyse only events with at least nljets-reconstructed jets
c$$$         IF(NCJET.GT.0) CALL ALPSOR(ETJET,NCJET,K,2)              
c$$$         if(idbg.eq.1) then
c$$$            write(LNHOUT,*) 'JETS'
c$$$            do i=1,ncjet
c$$$               j=k(ncjet+1-i)
c$$$               ETAJET=PSERAP(PCJET(1,j))
c$$$               PHIJET=ATAN2(PCJET(2,j),PCJET(1,j))
c$$$               write(LNHOUT,*) etjet(j),etajet,phijet
c$$$            enddo
c$$$         endif
c$$$         IF(NCJET.LT.NLJETS) THEN
c$$$            if(idbg.eq.1)
c$$$     $           WRITE(LNHOUT,*) 'Failed due to NCJET ',NCJET,' < ',NLJETS
c$$$            GOTO 999
c$$$         endif
c$$$c     associate partons and jets, using min(delr) as criterion
c$$$         NMATCH=0
c$$$         DO I=1,NCJET
c$$$            KPJ(I)=0
c$$$         ENDDO
c$$$         DO I=1,NLJETS
c$$$            DELRMIN=1D5
c$$$            DO 110 J=1,NCJET
c$$$               IF(KPJ(J).NE.0) GO TO 110
c$$$               ETAJET=PSERAP(PCJET(1,J))
c$$$               PHIJET=ATAN2(PCJET(2,J),PCJET(1,J))
c$$$               DPHI=ABS(PHI(KP(NLJETS-I+1))-PHIJET)
c$$$               IF(DPHI.GT.PI) DPHI=2.*PI-DPHI
c$$$               DELR=SQRT((ETA(KP(NLJETS-I+1))-ETAJET)**2+(DPHI)**2)
c$$$               IF(DELR.LT.DELRMIN) THEN
c$$$                  DELRMIN=DELR
c$$$                  JRMIN=J
c$$$               ENDIF
c$$$ 110        CONTINUE
c$$$            IF(DELRMIN.LT.clfact*RCLMAX) THEN
c$$$               NMATCH=NMATCH+1
c$$$               KPJ(JRMIN)=I
c$$$            ENDIF
c$$$C     WRITE(*,*) 'PARTON-JET',I,' best match:',k(ncjet+1-jrmin)
c$$$c     $           ,delrmin
c$$$         ENDDO
c$$$         IF(NMATCH.LT.NLJETS)  THEN
c$$$            if(idbg.eq.1)
c$$$     $           WRITE(LNHOUT,*) 'Failed due to NMATCH ',NMATCH,' < ',NLJETS
c$$$            GOTO 999
c$$$         endif
c$$$C     REJECT EVENTS WITH LARGER JET MULTIPLICITY FROM EXCLUSIVE SAMPLE
c$$$         IF(NCJET.GT.NLJETS.AND.IEXC.EQ.1)  THEN
c$$$            if(idbg.eq.1)
c$$$     $           WRITE(LNHOUT,*) 'Failed due to NCJET ',NCJET,' > ',NLJETS
c$$$            GOTO 999
c$$$         endif
c$$$C     VETO EVENTS WHERE MATCHED JETS ARE SOFTER THAN NON-MATCHED ONES
c$$$         IF(IEXC.NE.1) THEN
c$$$            J=NCJET
c$$$            DO I=1,NLJETS
c$$$               IF(KPJ(K(J)).EQ.0) GOTO 999
c$$$               J=J-1
c$$$            ENDDO
c$$$         ENDIF

      else                      ! qcut.gt.0

      if(showerkt) then
C     Use "shower emission pt method"
C     Veto events where first shower emission has kt > YCUT

        IF(NLJETS.EQ.0)THEN
           VINT(358)=0
        ENDIF

        IF(idbg.eq.1) THEN
           PRINT *,'Using shower emission pt method'
           PRINT *,'qcut, ptclus(1), vint(357),vint(358),vint(360): ',
     $          qcut,ptclus(1),vint(357),vint(358),vint(360)
        ENDIF
        YCUT=qcut**2

        IF(NLJETS.GT.0.AND.PTCLUS(1)**2.LT.YCUT) THEN
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Failed due to KT ',
     $       PTCLUS(1),' < ',SQRT(YCUT)
          GOTO 999
        ENDIF

c        PRINT *,'Y,VINT:',SQRT(Y(NLJETS+1)),SQRT(VINT(390))

        IF(IEXC.EQ.1.AND.
     $       ((mektsc.eq.1.and.MAX(VINT(357),VINT(358)).GT.SQRT(YCUT))
     $       .OR.
     $       (mektsc.eq.2.and.MAX(VINT(360),VINT(358)).GT.SQRT(YCUT))))
     $       THEN
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*),
     $       'Failed due to ',max(VINT(357),VINT(358)),' > ',SQRT(YCUT)
          GOTO 999
        ENDIF
c        PRINT *,NLJETS,IEXC,SQRT(VINT(390)),PTCLUS(1),SQRT(YCUT)
c     Highest multiplicity case
        IF(IEXC.EQ.0.AND.NLJETS.GT.0.AND.
     $       ((mektsc.eq.1.and.MAX(VINT(357),VINT(358)).GT.PTCLUS(1))
     $       .OR.
     $       (mektsc.eq.2.and.MAX(VINT(360),VINT(358)).GT.PTCLUS(1))))
     $       THEN
c     $     VINT(390).GT.PTCLUS(1)**2)THEN
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*),
     $       'Failed due to ',max(VINT(357),VINT(358)),' > ',PTCLUS(1)
          GOTO 999
        ENDIF
c     
      else                      ! not shower kt method

        IF(clfact.EQ.0d0) clfact=1d0

C---FIND FINAL STATE COLOURED PARTICLES
        NN=0
        DO IHEP=1,NHEP
          IF (ISTHEP(IHEP).EQ.1
     $       .AND.(ABS(IDHEP(IHEP)).LE.5.OR.IDHEP(IHEP).EQ.21)) THEN
            PTJET=sqrt(PHEP(1,IHEP)**2+PHEP(2,IHEP)**2)
            ETAJET=ABS(LOG(MIN((SQRT(PTJET**2+PHEP(3,IHEP)**2)+
     $       ABS(PHEP(3,IHEP)))/PTJET,1d5)))
            IF(ETAJET.GT.etaclmax) cycle
            NN=NN+1
            IF (NN.GT.NMAX) then
              CALL PYLIST(2)
              PRINT *, 'Too many particles: ', NN
              NN=NN-1
              GOTO 120
            endif
            DO I=1,4
              PP(I,NN)=PHEP(I,IHEP)
            ENDDO
          ELSE if(idbg.eq.1)THEN
            PRINT *,'Skipping particle ',IHEP,ISTHEP(IHEP),IDHEP(IHEP)
          ENDIF
        ENDDO

C...Cluster event to find values of Y including jet matching but not veto of too many jets
C...Only used to fill the beforeveto Root tree
 120    ECUT=1
        IF (NN.GT.1) then
          CALL KTCLUS(KTSCHE,PP,NN,ECUT,Y,*999)
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Clustering values:',
     $       (SQRT(Y(i)),i=1,MIN(NN,3))

C       Print out values in the case where all jets are matched at the
C       value of the NLJETS:th clustering
        var2(1)=NLJETS
        var2(6)= Ifile

        if(NLJETS.GT.MINJETS)then
          YCUT=Y(NLJETS)
          CALL KTRECO(MOD(KTSCHE,10),PP,NN,ECUT,YCUT,YCUT,PJET,JET,
     $       NCJET,NSUB,*999)        

C     Cluster jets with first hard parton
          DO I=1,NLJETS
            DO J=1,4
              PPM(J,I)=PJET(J,I)
            ENDDO
          ENDDO
          
          NJETM=NLJETS
          DO IHARD=1,NLJETS
            NNM=NJETM+1
            DO J=1,4
              PPM(J,NNM)=p(J,IHARD)
            ENDDO
            CALL KTCLUS(KTSCHE,PPM,NNM,ECUT,YM,*999)
            IF(YM(NNM).GT.YCUT) THEN
C       Parton not clustered
              GOTO 130
            ENDIF
            
C       Find jet clustered with parton

            IP1=HIST(NNM)/NMAXKT
            IP2=MOD(HIST(NNM),NMAXKT)
            IF(IP2.NE.NNM.OR.IP1.LE.0)THEN
              GOTO 130
            ENDIF
            DO I=IP1,NJETM-1
              DO J=1,4
                PPM(J,I)=PPM(J,I+1)
              ENDDO
            ENDDO
            NJETM=NJETM-1
          ENDDO                 ! IHARD=1,NLJETS
        endif                   ! NLJETS.GT.MINJETS

        DO I=1,MIN(NN,4)
          var2(1+I)=SQRT(Y(I))
        ENDDO
        WRITE(15,4001) (var2(I),I=1,nvar2)

 130    CONTINUE

C   Now perform jet clustering at the value chosen in qcut

        CALL KTCLUS(KTSCHE,PP,NN,ECUT,Y,*999)

        YCUT=qcut**2
        NCJET=0
          
C     Reconstruct jet momenta
          CALL KTRECO(MOD(KTSCHE,10),PP,NN,ECUT,YCUT,YCUT,PJET,JET,
     $       NCJET,NSUB,*999)        

        ELSE IF (NN.EQ.1) THEN

          Y(1)=PP(1,1)**2+PP(2,1)**2
          IF(Y(1).GT.YCUT)THEN
            NCJET=1
            DO I=1,4
              PJET(I,1)=PP(I,1)
            ENDDO
          ENDIF
        endif

        if(idbg.eq.1) then
          write(LNHOUT,*) 'JETS'
          do i=1,ncjet
            PTJET =SQRT(PJET(1,i)**2+PJET(2,i)**2)
            ETAJET=PSERAP(PJET(1,i))
            PHIJET=ATAN2(PJET(2,i),PJET(1,i))
            write(LNHOUT,*) ptjet,etajet,phijet
          enddo
        endif

        IF(NCJET.LT.NLJETS) THEN
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Failed due to NCJET ',NCJET,' < ',NLJETS
          GOTO 999
        endif

C...Right number of jets - but the right jets?        
C     For max. multiplicity case, count jets only to the NHARD:th jet
        IF(IEXC.EQ.0)THEN
           IF(NLJETS.GT.0)THEN
              YCUT=Y(NLJETS)
              CALL KTRECO(MOD(KTSCHE,10),PP,NN,ECUT,YCUT,YCUT,PJET,JET,
     $             NCJET,NSUB,*999)
              IF(clfact.GE.0d0) THEN
                 CALL ALPSOR(PTCLUS,nljets,KPT,2)
                 YCUT=MAX(qcut,PTCLUS(KPT(1)))**2
              ENDIF
           ENDIF
        ELSE IF(NCJET.GT.NLJETS) THEN
           if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Failed due to NCJET ',NCJET,' > ',NLJETS
           GOTO 999
        ENDIF

C     Cluster jets with hard partons, one at a time
        DO I=1,NLJETS
          DO J=1,4
            PPM(J,I)=PJET(J,I)
          ENDDO
        ENDDO

        NJETM=NLJETS
        IF(clfact.NE.0) YCUT=clfact**2*YCUT
c        YCUT=qcut**2
c        YCUT=(1.5*qcut)**2

        DO 140 IHARD=1,NLJETS
          NN=NJETM+1
          DO J=1,4
            PPM(J,NN)=p(J,IHARD)
          ENDDO
          CALL KTCLUS(KTSCHE,PPM,NN,ECUT,Y,*999)

          IF(Y(NN).GT.YCUT) THEN
C       Parton not clustered
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Failed due to parton ',IHARD,
     $         ' not clustered: ',Y(NN)
            GOTO 999
          ENDIF
          
C       Find jet clustered with parton

          IP1=HIST(NN)/NMAXKT
          IP2=MOD(HIST(NN),NMAXKT)
          IF(IP2.NE.NN.OR.IP1.LE.0)THEN
          if(idbg.eq.1)
     $       WRITE(LNHOUT,*) 'Failed due to parton ',IHARD,
     $         ' not clustered: ',IP1,IP2,NN,HIST(NN)
            GOTO 999
          ENDIF
C     Remove jet clustered with parton
          DO I=IP1,NJETM-1
            DO J=1,4
              PPM(J,I)=PPM(J,I+1)
            ENDDO
          ENDDO
          NJETM=NJETM-1
 140    CONTINUE

      endif                     ! pt-ordered showers
      endif                     ! qcut.gt.0

C...Cluster particles with |eta| < etaclmax for histograms
 150  NN=0
      DO IHEP=1,NHEP
         IF (ISTHEP(IHEP).EQ.1
     $        .AND.(ABS(IDHEP(IHEP)).LE.5.OR.IDHEP(IHEP).EQ.21)) THEN
            PTJET=sqrt(PHEP(1,IHEP)**2+PHEP(2,IHEP)**2)
            ETAJET=ABS(LOG(MIN((SQRT(PTJET**2+PHEP(3,IHEP)**2)+
     $           ABS(PHEP(3,IHEP)))/PTJET,1d5)))
            IF(ETAJET.GT.etaclmax) cycle
            NN=NN+1
            IF (NN.GT.NMAX) then
               CALL PYLIST(2)
               PRINT *, 'Too many particles: ', NN
               NN=NN-1
               GOTO 160
            ENDIF
            DO I=1,4
               PP(I,NN)=PHEP(I,IHEP)
            ENDDO
         ELSE if(idbg.eq.1)THEN
            PRINT *,'Skipping particle ',IHEP,ISTHEP(IHEP),IDHEP(IHEP)
         ENDIF
      ENDDO
      
 160  ECUT=1
      IF (NN.GT.1) THEN
         CALL KTCLUS(KTSCHE,PP,NN,ECUT,Y,*999)
      ELSE IF(NN.EQ.1) THEN
         Y(1)=SQRT(PP(1,NN)**2+PP(2,NN)**2)
      ENDIF

      DO I=1,MIN(NN,4)
         varev(46+I)=SQRT(Y(I))
      ENDDO


      RETURN
 4001 FORMAT(50E15.6)
c HERWIG/PYTHIA TERMINATION:
 999  IPVETO=1
      END
      
C*********************************************************************
C   PYMASC
C   Implementation of scale used in Pythia parton showers
C*********************************************************************
      SUBROUTINE PYMASC(scale)
      IMPLICIT NONE

C...Arguments
      REAL*8 scale

C...Functions
      REAL*8 SMDOT5

C...User process initialization commonblock.
C #include "GeneratorModules/heprup.inc"
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &   IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &   LPRUP(MAXPUP)
C...User process event common block.
C #include "GeneratorModules/hepeup.inc"
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &   ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &   VTIMUP(MAXNUP),SPINUP(MAXNUP)

C...Extra commonblock to transfer run info.
      INTEGER LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE
      COMMON/MGUPPRIV/LNHIN,LNHOUT,MSCAL,IEVNT,ICKKW,ISCALE

C...Local variables
      INTEGER ICC1,ICC2,IJ,IDC1,IDC2,IC,IC1,IC2
      REAL*8 QMIN,QTMP

C   Just use the scale read off the event record
      scale=SCALUP

C   Alternatively:

C...  Guesses for the correct scale
C     Assumptions:
C     (1) if the initial state is a color singlet, then
C     use s-hat for the scale
C     
C     (2) if color flow to the final state, use the minimum
C     of the dot products of color connected pairs
C     (times two for consistency with above)

        QMIN=SMDOT5(PUP(1,1),PUP(1,2))
        ICC1=1
        ICC2=2
C     
C     For now, there is no generic way to guarantee the "right"
C     scale choice.  Here, we take the HERWIG pt. of view and
C     choose the dot product of the colored connected "primary"
C     pairs.
C     

        DO 101 IJ=1,NUP
          IF(MOTHUP(2,IJ).GT.2) GOTO 101
          IDC1=ICOLUP(1,IJ)
          IDC2=ICOLUP(2,IJ)
          IF(IDC1.EQ.0) IDC1=-1
          IF(IDC2.EQ.0) IDC2=-2
          
          DO 201 IC=IJ+1,NUP
            IF(MOTHUP(2,IC).GT.2) GOTO 201
            IC1=ICOLUP(1,IC)
            IC2=ICOLUP(2,IC)
            IF(ISTUP(IC)*ISTUP(IJ).GE.1) THEN
              IF(IDC1.EQ.IC2.OR.IDC2.EQ.IC1) THEN
                QTMP=SMDOT5(PUP(1,IJ),PUP(1,IC))
                IF(QTMP.LT.QMIN) THEN
                  QMIN=QTMP
                  ICC1=IJ
                  ICC2=IC
                ENDIF
              ENDIF
            ELSEIF(ISTUP(IC)*ISTUP(IJ).LE.-1) THEN
              IF(IDC1.EQ.IC1.OR.IDC2.EQ.IC2) THEN
                QTMP=SMDOT5(PUP(1,IJ),PUP(1,IC))          
                IF(QTMP.LT.QMIN) THEN
                  QMIN=QTMP
                  ICC1=IJ
                  ICC2=IC
                ENDIF
              ENDIF
            ENDIF
 201      CONTINUE
 101    CONTINUE

        scale=QMIN

      RETURN
      END

C...SMDOT5
C   Helper function

      FUNCTION SMDOT5(V1,V2)
      IMPLICIT NONE
      REAL*8 SMDOT5,TEMP
      REAL*8 V1(5),V2(5)
      INTEGER I

      SMDOT5=0D0
      TEMP=V1(4)*V2(4)
      DO I=1,3
        TEMP=TEMP-V1(I)*V2(I)
      ENDDO

      SMDOT5=SQRT(ABS(TEMP))

      RETURN
      END

C-----------------------------------------------------------------------
      SUBROUTINE ALPSOR(A,N,K,IOPT)
C-----------------------------------------------------------------------
C     Sort A(N) into ascending order
C     IOPT = 1 : return sorted A and index array K
C     IOPT = 2 : return index array K only
C-----------------------------------------------------------------------
      DOUBLE PRECISION A(N),B(5000)
      INTEGER N,I,J,IOPT,K(N),IL(5000),IR(5000)
      IF (N.GT.5000) then
        write(*,*) 'Too many entries to sort in alpsrt, stop'
        stop
      endif
      if(n.le.0) return
      IL(1)=0
      IR(1)=0
      DO 10 I=2,N
      IL(I)=0
      IR(I)=0
      J=1
   2  IF(A(I).GT.A(J)) GOTO 5
   3  IF(IL(J).EQ.0) GOTO 4
      J=IL(J)
      GOTO 2
   4  IR(I)=-J
      IL(J)=I
      GOTO 10
   5  IF(IR(J).LE.0) GOTO 6
      J=IR(J)
      GOTO 2
   6  IR(I)=IR(J)
      IR(J)=I
  10  CONTINUE
      I=1
      J=1
      GOTO 8
  20  J=IL(J)
   8  IF(IL(J).GT.0) GOTO 20
   9  K(I)=J
      B(I)=A(J)
      I=I+1
      IF(IR(J)) 12,30,13
  13  J=IR(J)
      GOTO 8
  12  J=-IR(J)
      GOTO 9
  30  IF(IOPT.EQ.2) RETURN
      DO 31 I=1,N
  31  A(I)=B(I)
c 999  END
      END

c$$$C-----------------------------------------------------------------------
c$$$C----Calorimeter simulation obtained from Frank Paige 23 March 1988-----
c$$$C
c$$$C          USE
c$$$C
c$$$C     CALL CALINIM
c$$$C     CALL CALSIMM
c$$$C
c$$$C          THEN TO FIND JETS WITH A SIMPLIFIED VERSION OF THE UA1 JET
c$$$C          ALGORITHM WITH JET RADIUS RJET AND MINIMUM SCALAR TRANSVERSE
c$$$C          ENERGY EJCUT
c$$$C            (RJET=1., EJCUT=5. FOR UA1)
c$$$C          USE
c$$$C
c$$$C     CALL GETJETM(RJET,EJCUT)
c$$$C
c$$$C
c$$$C-----------------------------------------------------------------------
c$$$C 
c$$$C          ADDED BY MIKE SEYMOUR: PARTON-LEVEL CALORIMETER. ALL PARTONS
c$$$C          ARE CONSIDERED TO BE HADRONS, SO IN FACT RESEM IS IGNORED
c$$$C
c$$$C     CALL CALPARM
c$$$C
c$$$C          HARD PARTICLE CALORIMETER. ONLY USES THOSE PARTICLES WHICH
c$$$C          CAME FROM THE HARD PROCESS, AND NOT THE UNDERLYING EVENT
c$$$C
c$$$C     CALL CALHARM
c$$$C
c$$$C-----------------------------------------------------------------------
c$$$      SUBROUTINE CALINIM
c$$$C                
c$$$C          INITIALIZE CALORIMETER FOR CALSIMM AND GETJETM.  NOTE THAT
c$$$C          BECAUSE THE INITIALIZATION IS SEPARATE, CALSIMM CAN BE
c$$$C          CALLED MORE THAN ONCE TO SIMULATE PILEUP OF SEVERAL EVENTS.
c$$$C
c$$$      IMPLICIT NONE
c$$$C...GETJET commonblocks
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI,NJMAX,JETNO,NCJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL,PCJET,ETJET
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALORM/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI
c$$$      PARAMETER (NJMAX=500)
c$$$      COMMON/GETCOMM/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(MNCY,MNCPHI),
c$$$     $     NCJET
c$$$
c$$$      INTEGER IPHI,IY
c$$$      DOUBLE PRECISION PI,PHIX,YX,THX
c$$$      PARAMETER (PI=3.141593D0)
c$$$      LOGICAL FSTCAL
c$$$      DATA FSTCAL/.TRUE./
c$$$C
c$$$C          INITIALIZE ET ARRAY.
c$$$      DO 100 IPHI=1,NCPHI
c$$$      DO 100 IY=1,NCY
c$$$100   ET(IY,IPHI)=0.
c$$$C
c$$$      IF (FSTCAL) THEN
c$$$C          CALCULATE TRIG. FUNCTIONS.
c$$$        DELPHI=2.*PI/FLOAT(NCPHI)
c$$$        DO 200 IPHI=1,NCPHI
c$$$        PHIX=DELPHI*(IPHI-.5)
c$$$        CPHCAL(IPHI)=COS(PHIX)
c$$$        SPHCAL(IPHI)=SIN(PHIX)
c$$$200     CONTINUE
c$$$        DELY=(YCMAX-YCMIN)/FLOAT(NCY)
c$$$        DO 300 IY=1,NCY
c$$$        YX=DELY*(IY-.5)+YCMIN
c$$$        THX=2.*ATAN(EXP(-YX))
c$$$        CTHCAL(IY)=COS(THX)
c$$$        STHCAL(IY)=SIN(THX)
c$$$300     CONTINUE
c$$$        FSTCAL=.FALSE.
c$$$      ENDIF
c$$$      END
c$$$C
c$$$      SUBROUTINE CALSIMM
c$$$C                
c$$$C          SIMPLE CALORIMETER SIMULATION.  ASSUME UNIFORM Y AND PHI
c$$$C          BINS
c$$$C...HEPEVT commonblock.
c$$$      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
c$$$      PARAMETER (NMXHEP=10000)
c$$$      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
c$$$     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
c$$$      DOUBLE PRECISION PHEP,VHEP
c$$$      SAVE /HEPEVT/
c$$$
c$$$C...GETJET commonblocks
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI,NJMAX,JETNO,NCJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL,PCJET,ETJET
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALORM/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI
c$$$      PARAMETER (NJMAX=500)
c$$$      COMMON/GETCOMM/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(MNCY,MNCPHI),
c$$$     $     NCJET
c$$$
c$$$      INTEGER IHEP,ID,IY,IPHI
c$$$      DOUBLE PRECISION PI,YIP,PSERAP,PHIIP,EIP
c$$$      PARAMETER (PI=3.141593D0)
c$$$C
c$$$C          FILL CALORIMETER
c$$$C
c$$$      DO 200 IHEP=1,NHEP
c$$$      IF (ISTHEP(IHEP).EQ.1) THEN
c$$$        YIP=PSERAP(PHEP(1,IHEP))
c$$$        IF(YIP.LT.YCMIN.OR.YIP.GT.YCMAX) GOTO 200
c$$$        ID=ABS(IDHEP(IHEP))
c$$$C---EXCLUDE TOP QUARK, LEPTONS, PROMPT PHOTONS
c$$$        IF ((ID.GE.11.AND.ID.LE.16).OR.ID.EQ.6.OR.ID.EQ.22) GOTO 200
c$$$C
c$$$        PHIIP=ATAN2(PHEP(2,IHEP),PHEP(1,IHEP))
c$$$        IF(PHIIP.LT.0.) PHIIP=PHIIP+2.*PI
c$$$        IY=INT((YIP-YCMIN)/DELY)+1
c$$$        IPHI=INT(PHIIP/DELPHI)+1
c$$$        EIP=PHEP(4,IHEP)
c$$$C            WEIGHT BY SIN(THETA)
c$$$        ET(IY,IPHI)=ET(IY,IPHI)+EIP*STHCAL(IY)
c$$$      ENDIF
c$$$  200 CONTINUE
c$$$  999 END
c$$$      SUBROUTINE GETJETM(RJET,EJCUT,ETAJCUT)
c$$$C                
c$$$C          SIMPLE JET-FINDING ALGORITHM (SIMILAR TO UA1).
c$$$C
c$$$C     FIND HIGHEST REMAINING CELL > ETSTOP AND SUM SURROUNDING
c$$$C          CELLS WITH--
c$$$C            DELTA(Y)**2+DELTA(PHI)**2<RJET**2
c$$$C            ET>ECCUT.
c$$$C          KEEP JETS WITH ET>EJCUT AND ABS(ETA)<ETAJCUT
c$$$C          THE UA1 PARAMETERS ARE RJET=1.0 AND EJCUT=5.0
c$$$C                  
c$$$      IMPLICIT NONE
c$$$C...GETJET commonblocks
c$$$      INTEGER MNCY,MNCPHI,NCY,NCPHI,NJMAX,JETNO,NCJET
c$$$      DOUBLE PRECISION YCMIN,YCMAX,DELY,DELPHI,ET,STHCAL,CTHCAL,CPHCAL,
c$$$     &  SPHCAL,PCJET,ETJET
c$$$      PARAMETER (MNCY=200)
c$$$      PARAMETER (MNCPHI=200)
c$$$      COMMON/CALORM/DELY,DELPHI,ET(MNCY,MNCPHI),
c$$$     $CTHCAL(MNCY),STHCAL(MNCY),CPHCAL(MNCPHI),SPHCAL(MNCPHI),
c$$$     $YCMIN,YCMAX,NCY,NCPHI
c$$$      PARAMETER (NJMAX=500)
c$$$      COMMON/GETCOMM/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(MNCY,MNCPHI),
c$$$     $     NCJET
c$$$
c$$$      INTEGER IPHI,IY,J,K,NPHI1,NPHI2,NY1,
c$$$     &  NY2,IPASS,IYMX,IPHIMX,ITLIS,IPHI1,IPHIX,IY1,IYX
c$$$      DOUBLE PRECISION PI,RJET,
c$$$     &  ETMAX,ETSTOP,RR,ECCUT,PX,EJCUT
c$$$      PARAMETER (PI=3.141593D0)
c$$$      DOUBLE PRECISION ETAJCUT,PSERAP
c$$$C
c$$$C          PARAMETERS
c$$$      DATA ECCUT/0.1D0/
c$$$      DATA ETSTOP/1.5D0/
c$$$      DATA ITLIS/6/
c$$$C
c$$$C          INITIALIZE
c$$$C
c$$$      DO 100 IPHI=1,NCPHI
c$$$      DO 100 IY=1,NCY
c$$$100   JETNO(IY,IPHI)=0
c$$$      DO 110 J=1,NJMAX
c$$$      ETJET(J)=0.
c$$$      DO 110 K=1,4
c$$$110   PCJET(K,J)=0.
c$$$      NCJET=0
c$$$      NPHI1=RJET/DELPHI
c$$$      NPHI2=2*NPHI1+1
c$$$      NY1=RJET/DELY
c$$$      NY2=2*NY1+1
c$$$      IPASS=0
c$$$C
c$$$C          FIND HIGHEST CELL REMAINING
c$$$C
c$$$1     ETMAX=0.
c$$$      DO 200 IPHI=1,NCPHI
c$$$      DO 210 IY=1,NCY
c$$$      IF(ET(IY,IPHI).LT.ETMAX) GOTO 210
c$$$      IF(JETNO(IY,IPHI).NE.0) GOTO 210
c$$$      ETMAX=ET(IY,IPHI)
c$$$      IYMX=IY
c$$$      IPHIMX=IPHI
c$$$210   CONTINUE
c$$$200   CONTINUE
c$$$      IF(ETMAX.LT.ETSTOP) RETURN
c$$$C
c$$$C          SUM CELLS
c$$$C
c$$$      IPASS=IPASS+1
c$$$      IF(IPASS.GT.NCY*NCPHI) THEN
c$$$        WRITE(ITLIS,8888) IPASS
c$$$8888    FORMAT(//' ERROR IN GETJETM...IPASS > ',I6)
c$$$        RETURN
c$$$      ENDIF
c$$$      NCJET=NCJET+1
c$$$      IF(NCJET.GT.NJMAX) THEN
c$$$        WRITE(ITLIS,9999) NCJET
c$$$9999    FORMAT(//' ERROR IN GETJETM...NCJET > ',I5)
c$$$        RETURN
c$$$      ENDIF
c$$$      DO 300 IPHI1=1,NPHI2
c$$$      IPHIX=IPHIMX-NPHI1-1+IPHI1
c$$$      IF(IPHIX.LE.0) IPHIX=IPHIX+NCPHI
c$$$      IF(IPHIX.GT.NCPHI) IPHIX=IPHIX-NCPHI
c$$$      DO 310 IY1=1,NY2
c$$$      IYX=IYMX-NY1-1+IY1
c$$$      IF(IYX.LE.0) GOTO 310
c$$$      IF(IYX.GT.NCY) GOTO 310
c$$$      IF(JETNO(IYX,IPHIX).NE.0) GOTO 310
c$$$      RR=(DELY*(IY1-NY1-1))**2+(DELPHI*(IPHI1-NPHI1-1))**2
c$$$      IF(RR.GT.RJET**2) GOTO 310
c$$$      IF(ET(IYX,IPHIX).LT.ECCUT) GOTO 310
c$$$      PX=ET(IYX,IPHIX)/STHCAL(IYX)
c$$$C          ADD CELL TO JET
c$$$      PCJET(1,NCJET)=PCJET(1,NCJET)+PX*STHCAL(IYX)*CPHCAL(IPHIX)
c$$$      PCJET(2,NCJET)=PCJET(2,NCJET)+PX*STHCAL(IYX)*SPHCAL(IPHIX)
c$$$      PCJET(3,NCJET)=PCJET(3,NCJET)+PX*CTHCAL(IYX)
c$$$      PCJET(4,NCJET)=PCJET(4,NCJET)+PX
c$$$      ETJET(NCJET)=ETJET(NCJET)+ET(IYX,IPHIX)
c$$$      JETNO(IYX,IPHIX)=NCJET
c$$$310   CONTINUE
c$$$300   CONTINUE
c$$$C
c$$$C          DISCARD JET IF ET < EJCUT.
c$$$C
c$$$      IF(ETJET(NCJET).GT.EJCUT.AND.ABS(PSERAP(PCJET(1,NCJET))).LT
c$$$     $     .ETAJCUT) GOTO 1
c$$$      ETJET(NCJET)=0.
c$$$      DO 400 K=1,4
c$$$400   PCJET(K,NCJET)=0.
c$$$      NCJET=NCJET-1
c$$$      GOTO 1
c$$$      END
c$$$C-----------------------------------------------------------------------
c$$$      SUBROUTINE CALDELM(ISTLO,ISTHI)
c$$$C     LABEL ALL PARTICLES WITH STATUS BETWEEN ISTLO AND ISTHI (UNTIL A
c$$$C     PARTICLE WITH STATUS ISTOP IS FOUND) AS FINAL-STATE, CALL CALSIMM
c$$$C     AND THEN PUT LABELS BACK TO NORMAL
c$$$C-----------------------------------------------------------------------
c$$$      IMPLICIT NONE
c$$$      INTEGER MAXNUP
c$$$      PARAMETER(MAXNUP=500)
c$$$C...HEPEVT commonblock.
c$$$      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
c$$$      PARAMETER (NMXHEP=10000)
c$$$      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
c$$$     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
c$$$      DOUBLE PRECISION PHEP,VHEP
c$$$      SAVE /HEPEVT/
c$$$      INTEGER ISTLO,ISTHI
c$$$
c$$$
c$$$      CALL CALSIMM
c$$$      END

C****************************************************
C iexclusive returns whether exclusive process or not
C****************************************************

      integer function iexclusive(iproc)
      implicit none
      
      integer iproc, i

C...Inputs for the matching algorithm
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      double precision etcjet,rclmax,etaclmax,qcut,clfact
      integer maxjets,minjets,iexcfile,ktsche,mektsc,nexcres,excres(30)
      integer nremres, remres(30)
      integer nqmatch,nexcproc,iexcproc(MAXPUP),iexcval(MAXPUP)
      logical nosingrad,showerkt,jetprocs
      common/MEMAIN/etcjet,rclmax,etaclmax,qcut,clfact,maxjets,minjets,
     $   iexcfile,ktsche,mektsc,nexcres,excres,nremres,remres,
     $   nqmatch,nexcproc,iexcproc,iexcval,nosingrad,showerkt,jetprocs
      
      iexclusive=-2
      do i=1,nexcproc
         if(iproc.eq.iexcproc(i)) then
            iexclusive=iexcval(i)
            return
         endif
      enddo

      return
      end
C-----------------------------------------------------------------------
      FUNCTION PSERAP(P)
C     PSEUDO-RAPIDITY (-LOG TAN THETA/2)
C-----------------------------------------------------------------------
      DOUBLE PRECISION PSERAP,P(3),PT,PL,TINY,THETA
      PARAMETER (TINY=1D-3)
      PT=SQRT(P(1)**2+P(2)**2)+TINY
      PL=P(3)
      THETA=ATAN2(PT,PL)
      PSERAP=-LOG(TAN(0.5*THETA))
      END 
C----------------------------------------------------------------------- 
                                                                              
