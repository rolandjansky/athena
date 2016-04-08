
C     DECK  ID&gt;, UPEVNT.
C*     CMZ :-           -13/02/02  07.20.46  by  Peter Richardson
C*--   Author :    Michelangelo Mangano
C----------------------------------------------------------------------
      SUBROUTINE USEALPGEN
C----------------------------------------------------------------------
c     Puts Alpgen event into GUPI common block HEPEU
c----------------------------------------------------------------------
      implicit none
#include "AlpGen_i/alpsho.inc"
C--   GUP Event common block
#include "GeneratorFortranCommon/hepeup.inc"
C--   GUP Run common block
#include "GeneratorFortranCommon/heprup.inc"
c
c     local variables
C      INTEGER INIT
C      DATA INIT/0/
C      CHARACTER *3 CSHO
      INTEGER MAXPAR
      PARAMETER (MAXPAR=100)
      INTEGER NEV,IPROC,IFL(MAXPAR)
      REAL SQ,SP(3,MAXPAR),SM(MAXPAR),SWGTRES
      INTEGER I,IUP,IWCH,IST
C      REAL *8 TMP,WGTRES
      REAL *8 TMP
C     LOCAL VARIABLES FOR TOP DECAYS
      INTEGER IT,ITB,IW,IWDEC,IBUP,IWUP
C     LOCAL VARIABLES TOP HIGGS DECAYS
C      INTEGER IH
C     LOCAL VARIABLES FOR GAUGE BOSON  DECAYS
      INTEGER IVSTART,IVEND,NVB
C

C...=======================================================
C...HEPEVT Common block (05.16.2011, Soshi.Tsuno@cern.ch)
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
      SAVE /hepevt/

C...random number
      integer imode,itau1,itau2,itau3
      real*8 polz0
      real*8 pyr
      external pyr

      real*8 plzapx
      external plzapx
C...=======================================================


      integer istg
      common /evtcon/ istg
      istg=0
C
C UPDATE MAXIMUM NMBER OF ALLOWED ERRORS
      CALL ALSHER(I)
C
      IST=0
C     INPUT EVENT NUMBER, PROCESS TYPE, N PARTONS, SAMPLE'S AVERAGE
C     WEIGHT AND QSCALE
      READ(NUNIT,2,END=500,ERR=501) NEV,IPROC,NPART,SWGTRES,SQ
 2    FORMAT(I8,1X,I4,1X,I2,2(1X,E12.6))
C     FLAVOUR, COLOUR AND Z-MOMENTUM OF INCOMING PARTONS
      READ(NUNIT,8) IFL(1),ICOLUP(1,1),ICOLUP(2,1),SP(3,1)
      READ(NUNIT,8) IFL(2),ICOLUP(1,2),ICOLUP(2,2),SP(3,2)
C     FLAVOUR, COLOUR, 3-MOMENTUM AND MASS OF OUTGOING PARTONS
      DO I=3,NPART
        READ(NUNIT,9) IFL(I),ICOLUP(1,I),ICOLUP(2,I),SP(1,I),SP(2,I)
     $       ,SP(3,I),SM(I)
      ENDDO
 8    FORMAT(I8,1X,2(I4,1X),F10.3)
 9    FORMAT(I8,1X,2(I4,1X),4(1X,F10.3))
C
C     START PROCESSING INPUT DATA
C
C     SCALES AND WEIGHTS
      SCALUP=DBLE(SQ)
      IF(IDWTUP.EQ.3) THEN
         XWGTUP=DBLE(SWGTRES)   !AVGWGT
      ELSE
        WRITE(*,*) 'ONLY UNWEIGHTED EVENTS ACCEPTED AS INPUT, STOP'
        STOP
      ENDIF
c
c---  incoming lines
      do 100 i=1,2
        iup=i
        idup(iup)=ifl(i)
        istup(iup)=-1
        mothup(1,iup)=0
        mothup(2,iup)=0
        pup(1,iup)=0.
        pup(2,iup)=0.
        pup(3,iup)=dble(Sp(3,iup))
        pup(4,iup)=abs(pup(3,iup))
        pup(5,iup)=0d0
 100  continue
c---  outgoing lines
      do 110 i=3,npart
        iup=i
        idup(iup)=ifl(i)
        istup(iup)=1
        mothup(1,iup)=1
        mothup(2,iup)=2
        pup(1,iup)=dble(Sp(1,i))
        pup(2,iup)=dble(Sp(2,i))
        pup(3,iup)=dble(Sp(3,i))
        pup(5,iup)=dble(Sm(i))
        tmp=(pup(5,iup)**2+pup(1,iup)**2+pup(2,iup)**2+pup(3,iup)**2)
        pup(4,iup)=sqrt(tmp)
 110  continue
c
      nup=npart
c---  set up colour structure labels
      Do iup=1,nup
        if(icolup(1,iup).ne.0) icolup(1,iup)=icolup(1,iup)+500
        if(icolup(2,iup).ne.0) icolup(2,iup)=icolup(2,iup)+500
      Enddo
c
c
c     and now consider assignements specific to individual hard
C     processes
c
c---  W/Z/gamma b bbar + jets, or W/Z + jets
      if (ihrd.le.4.or.ihrd.eq.10.OR.IHRD.EQ.14.OR.IHRD.EQ.15) then
        iwch=0
        do iup=nup-1,nup
          mothup(1,iup)=nup+1
          mothup(2,iup)=0
          if(ihrd.ne.2) iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) => -(1)+0 = -1  => W-
c     positron+nu    -> -11+ 12    => -(-1)+0 = -1 => W+
c     u dbar -> 2 -1  => 0 -(-1) = 1 => W+
c     c dbar -> 4 -1  => W+
c etc.
        enddo
        iup=nup+1
        If (iwch.gt.0) then
          idup(iup)=24
        Elseif (iwch.lt.0) then
          idup(iup)=-24
        Else
          idup(iup)=23
        Endif
        istup(iup)=2
        mothup(1,iup)=1
        mothup(2,iup)=2
        tmp=pup(4,iup-2)+pup(4,iup-1)
        pup(4,iup)=tmp
        tmp=tmp**2
        do i=1,3
          pup(i,iup)=pup(i,iup-2)+pup(i,iup-1)
          tmp=tmp-pup(i,iup)**2
        enddo
        pup(5,iup)=sqrt(tmp)
        nup=nup+1
        icolup(1,nup)=0
        icolup(2,nup)=0
c---  nW + mZ + kH + jets
      elseif (ihrd.eq.5) then
c     find first gauge bosons
        ivstart=0
        ivend=0
        do i=1,npart
          if(abs(idup(i)).eq.24.or.idup(i).eq.23) then
            istup(i)=2
            if(ivstart.eq.0) ivstart=i
            ivend=i+1
          endif
        enddo
        nvb=ivend-ivstart
c     decay products pointers, starting from the end
        do i=1,nvb
          mothup(1,npart-2*i+2)=ivend-i
          mothup(1,npart-2*i+1)=ivend-i
          mothup(2,npart-2*i+2)=0
          mothup(2,npart-2*i+1)=0
        enddo
c---  t tbar + jets
c     t tb jets f fbar f fbar W+ b W- bbar
      elseif ((ihrd.eq.6.or.ihrd.eq.16).and.abs(ifl(3)).eq.6) then
c     reset top status codes
        istup(3)=2
        istup(4)=2
        if(ifl(3).eq.6) then
          it=3
          itb=4
        else
          it=4
          itb=3
        endif
c     reconstruct W's from decay products
        do iw=1,2
          iwdec=nup-5+2*iw
          iwup=nup+iw
          ibup=iwup+2
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          If (iwch.gt.0) then
            idup(iwup)=24
            idup(ibup)=5
            mothup(1,iwup)=it
            mothup(2,iwup)=0
            mothup(1,ibup)=it
            mothup(2,ibup)=0
          Elseif (iwch.lt.0) then
            idup(iwup)=-24
            idup(ibup)=-5
            mothup(1,iwup)=itb
            mothup(2,iwup)=0
            mothup(1,ibup)=itb
            mothup(2,ibup)=0
          Endif
          istup(iwup)=2
          istup(ibup)=1
c     reconstruct W momentum
          tmp=pup(4,iwdec)+pup(4,iwdec+1)
          pup(4,iwup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
            tmp=tmp-pup(i,iwup)**2
          enddo
          pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
          tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
          pup(4,ibup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
            tmp=tmp-pup(i,ibup)**2
          enddo
          pup(5,ibup)=sqrt(tmp)
          icolup(1,iwup)=0
          icolup(2,iwup)=0
          icolup(1,ibup)=icolup(1,mothup(1,iwup))
          icolup(2,ibup)=icolup(2,mothup(1,iwup))
        enddo
c     stop
        nup=nup+4
c---  H t tbar + jets
c     H t tb jets f fbar f fbar W+ b W- bbar
      elseif (ihrd.eq.8.and.abs(ifl(4)).eq.6) then
c     reset top status codes
        istup(4)=2
        istup(5)=2
        if(ifl(4).eq.6) then
          it=4
          itb=5
        else
          it=5
          itb=4
        endif
c     reconstruct W's from decay products
        do iw=1,2
          iwdec=nup-5+2*iw
          iwup=nup+iw
          ibup=iwup+2
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          If (iwch.gt.0) then
            idup(iwup)=24
            idup(ibup)=5
            mothup(1,iwup)=it
            mothup(2,iwup)=0
            mothup(1,ibup)=it
            mothup(2,ibup)=0
          elseif (iwch.lt.0) then
            idup(iwup)=-24
            idup(ibup)=-5
            mothup(1,iwup)=itb
            mothup(2,iwup)=0
            mothup(1,ibup)=itb
            mothup(2,ibup)=0
          endif
          istup(iwup)=2
          istup(ibup)=1
c     reconstruct W momentum
          tmp=pup(4,iwdec)+pup(4,iwdec+1)
          pup(4,iwup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
            tmp=tmp-pup(i,iwup)**2
          enddo
          pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
          tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
          pup(4,ibup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
            tmp=tmp-pup(i,ibup)**2
          enddo
          pup(5,ibup)=sqrt(tmp)
          icolup(1,iwup)=0
          icolup(2,iwup)=0
          icolup(1,ibup)=icolup(1,mothup(1,iwup))
          icolup(2,ibup)=icolup(2,mothup(1,iwup))
        enddo
c     stop
        nup=nup+4
c---  SINGLE TOP
c     Input: T
c     output: jets t b w f fbar t b w f fbar
      elseif (ihrd.eq.13) then
        nw=1
        if(itopprc.ge.3) nw=2
c     assign mass to the incoming bottom quark, if required
        DO I=1,2
          IF(ABS(IFL(I)).EQ.5) THEN
            IUP=I
            PUP(5,IUP)=mb
            PUP(4,IUP)=SQRT(PUP(3,IUP)**2+PUP(5,IUP)**2)
          ENDIF
        ENDDO
        istup(3)=2
        it=0
        itb=0
        if(ifl(3).eq.6) then
          it=3
        elseif(ifl(3).eq.-6) then
          itb=3
        else
          write(*,*) 'wrong assumption about top position, stop'
          stop
        endif
c
c     TOP DECAY
c     reconstruct W's from decay products
c
c iwdec: 1st W decay product.
        if(nw.eq.1) then
          iwdec=nup-1
        elseif(nw.eq.2) then
          iwdec=nup-3
        endif
c put W and b  at the end
        iwup=nup+1
        ibup=iwup+1
c
        iwch=0
        do iup=iwdec,iwdec+1
          mothup(1,iup)=iwup
          mothup(2,iup)=0
          iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
        enddo
        If (iwch.gt.0) then
          idup(iwup)=24
          idup(ibup)=5
          mothup(1,iwup)=it
          mothup(2,iwup)=0
          mothup(1,ibup)=it
          mothup(2,ibup)=0
        Elseif (iwch.lt.0) then
          idup(iwup)=-24
          idup(ibup)=-5
          mothup(1,iwup)=itb
          mothup(2,iwup)=0
          mothup(1,ibup)=itb
          mothup(2,ibup)=0
        Endif
        istup(iwup)=2
        istup(ibup)=1
c     reconstruct W momentum
        tmp=pup(4,iwdec)+pup(4,iwdec+1)
        pup(4,iwup)=tmp
        tmp=tmp**2
        do i=1,3
          pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
          tmp=tmp-pup(i,iwup)**2
        enddo
        pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
        tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
        pup(4,ibup)=tmp
        tmp=tmp**2
        do i=1,3
          pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
          tmp=tmp-pup(i,ibup)**2
        enddo
c        write(*,*) (pup(i,ibup),i=1,4),sqrt((tmp))
        pup(5,ibup)=sqrt(tmp)
        icolup(1,iwup)=0
        icolup(2,iwup)=0
        icolup(1,ibup)=icolup(1,mothup(1,iwup))
        icolup(2,ibup)=icolup(2,mothup(1,iwup))
c
        nup=nup+2
        if(nw.eq.2) then
c
c     W DECAY
c
c iwdec: 1st W decay product.
          iwdec=nup-3
c iwup: location of the W in the event record
          iwup=nup-6
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          istup(iwup)=2
          icolup(1,iwup)=0
          icolup(2,iwup)=0
        endif
      endif
c     herwig debugging:
c      call HWUPUP



C...=======================================================
C...Fill spin information for tauola.
C...Specify the decal mode.
C...imode: 1:W, 2:WW, 4:Z, 5:WZ, 8:ZZ, 16:H, 17:WH, 20:ZH.
      imode = 0
      itau1 = 0
      itau2 = 0
      itau3 = 0
      do i=1,nup
        if (iabs(idup(i)).eq.24) then
          imode = imode + 1
        else if (iabs(idup(i)).eq.23) then
          imode = imode + 4
        else if (iabs(idup(i)).eq.25) then
          imode = imode + 16
        endif
      enddo
      do i=1,nup
         if(mothup(1,i).gt.0) then
            if (imode.eq.1) then ! single W production
               if (iabs(idup(i)).eq.15
     &              .and.iabs(idup(mothup(1,i))).eq.24) then
                  itau1 = i
               endif
            else if (imode.eq.2) then ! WW production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.24) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.-24) then
                  itau2 = i
               endif
            else if (imode.eq.4) then ! single Z production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.23) then
                  itau1 = i
                  itau3 = mothup(1,i)
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.23) then
                  itau2 = i
               endif
            else if (imode.eq.5) then ! WZ production (so far, only assign for W decay.)
               if (iabs(idup(i)).eq.15
     &              .and.iabs(idup(mothup(1,i))).eq.24)then
                  itau1 = i
               endif
            else if (imode.eq.8) then ! ZZ production (so far do nothing.)
            else if (imode.eq.16) then ! single H production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
            else if (imode.eq.17) then ! WH production
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
               if (iabs(idup(i)).eq.15.and
     &              .iabs(idup(mothup(1,i))).eq.24) then
                  itau3 = i
               endif
            else if (imode.eq.20) then ! ZH production (so far nothing for Z side.)
               if (idup(i).eq.-15.and.idup(mothup(1,i)).eq.25) then
                  itau1 = i
               endif
               if (idup(i).eq.15.and.idup(mothup(1,i)).eq.25) then
                  itau2 = i
               endif
            endif
         endif
      enddo

      if (itau1.ne.0) then
        spinup(itau1) = 0
      endif
      if (itau2.ne.0) then
        spinup(itau2) = 0
      endif
      if (itau3.ne.0) then
        spinup(itau3) = 0
      endif

C...Assign spin information.
      if (imode.eq.1) then         ! single W production
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
      else if (imode.eq.2) then    ! WW production
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
        if (itau2.ne.0) then
          spinup(itau2) = -1.0d0
        endif
      else if (imode.eq.4
     &       .and. itau1.ne.0.and.itau2.ne.0.and.itau3.ne.0) then ! single Z production
C...Special case. Temporaly create the HEPEVT block. Later delete it.
        nhep = 9
        isthep(4) = 121
        isthep(5) = 122
        isthep(6) = 120
        isthep(7) = 124
        isthep(8) = 1
        isthep(9) = 1
        idhep(4) = idup(1)
        idhep(5) = idup(2)
        idhep(6) = 0
        idhep(7) = idup(itau3)
        idhep(8) = idup(itau1)
        idhep(9) = idup(itau2)
        jmohep(1,4) = 6
        jmohep(2,4) = 5
        jmohep(1,5) = 6
        jmohep(2,5) = 4
        jmohep(1,6) = 4
        jmohep(2,6) = 5
        jmohep(1,7) = 6
        jmohep(2,7) = 7
        jmohep(1,8) = 7
        jmohep(2,8) = 8
        jmohep(1,9) = 7
        jmohep(2,9) = 9
        jdahep(1,4) = 5
        jdahep(2,4) = 5
        jdahep(2,5) = 4
        jdahep(2,5) = 4
        jdahep(2,6) = 7
        jdahep(2,6) = 7
        jdahep(2,7) = 8
        jdahep(2,7) = 9
        jdahep(2,8) = 0
        jdahep(2,8) = 0
        jdahep(2,9) = 0
        jdahep(2,9) = 0
        do i=1,5
          phep(i,4) = pup(i,1)
          phep(i,5) = pup(i,2)
          phep(i,6) = pup(i,1)+pup(i,2)
          phep(i,7) = pup(i,itau3)
          phep(i,8) = pup(i,itau1)
          phep(i,9) = pup(i,itau2)
        enddo
        phep(5,6) = sqrt((pup(4,1)+pup(4,2))**2
     $                  -(pup(1,1)+pup(1,2))**2
     $                  -(pup(2,1)+pup(2,2))**2
     $                  -(pup(3,1)+pup(3,2))**2)
        do i=1,4
          vhep(i,4) = 0.0d0
          vhep(i,5) = 0.0d0
          vhep(i,6) = 0.0d0
          vhep(i,7) = 0.0d0
          vhep(i,8) = 0.0d0
          vhep(i,9) = 0.0d0
        enddo

        polz0 = plzapx(1,7,8,9)
        if (pyr(0).LT.polz0) THEN
          spinup(itau1) = 1.0d0
          spinup(itau2) = 1.0d0
        else
          spinup(itau1) = -1.0d0
          spinup(itau2) = -1.0d0
        endif
C...Delete temporal HEPEVT.
        nhep = 3
        do i=4,9
          isthep(i) = 0
          idhep(i)  = 0
          jmohep(1,i) = 0
          jmohep(2,i) = 0
          jdahep(1,i) = 0
          jdahep(2,i) = 0
          phep(1,i) = 0.0d0
          phep(2,i) = 0.0d0
          phep(3,i) = 0.0d0
          phep(4,i) = 0.0d0
          phep(5,i) = 0.0d0
          vhep(1,i) = 0.0d0
          vhep(2,i) = 0.0d0
          vhep(3,i) = 0.0d0
          vhep(4,i) = 0.0d0
        enddo
      else if (imode.eq.5) then    ! WZ production (so far, only assign for W decay.)
        if (itau1.ne.0) then
          spinup(itau1) = -1.0d0
        endif
      else if (imode.eq.8) then    ! ZZ production (so far do nothing.)
      else if (imode.eq.16) then   ! single H production
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
      else if (imode.eq.17) then   ! WH production
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
        if (itau3.ne.0) then
          spinup(itau3) = -1.0d0
        endif
      else if (imode.eq.20) then   ! ZH production (so far nothing for Z side.)
        if (itau1.ne.0.and.itau2.ne.0) then
          if (pyr(0).lt.0.5d0) then
            spinup(itau1) =  1.0d0
            spinup(itau2) = -1.0d0
          else
            spinup(itau1) = -1.0d0
            spinup(itau2) =  1.0d0
          endif
        endif
      endif

C...Debug.
C      write(*,*) "ST...Alpgen",imode,itau1,itau2,itau3
C      if (itau1.ne.0) then
C        write(*,*) "ST...Alpgen after fill 1",idup(itau1),spinup(itau1)
C      endif
C      if (itau2.ne.0) then
C        write(*,*) "ST...Alpgen after fill 2",idup(itau2),spinup(itau2)
C      endif
C      if (itau3.ne.0) then
C        write(*,*) "ST...Alpgen after fill 3",idup(itau3),spinup(itau3)
C      endif
C...=======================================================



      return
c
c
c     end of file
 500  ist=1
c     error reading file
 501  if(ist.eq.0) ist=2
C     RESET CROSS-SECTION INFORMATION FOR END OF RUN AND FINALIZE
      istg=ist
      IF(IST.GT.0) THEN
        CALL ALSFIN
      ENDIF
      close(Nunit)
      close(NunitOut)
      END
