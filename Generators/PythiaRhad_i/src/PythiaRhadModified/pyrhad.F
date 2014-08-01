C*********************************************************************
 
C...PYGGPRO
C Sets the PROBGG parameter in the fragmentation

C...  Double precision and integer declarations.
      SUBROUTINE PYGGPRO(PROB)

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      INTEGER IKFRH(20),IKCHGRH(20),IKANTRH(20)
      REAL*8 PROBGG
      COMMON/RHADB/IKFRH,IKCHGRH,IKANTRH,PROBGG


      PROBGG=PROB

      write(*,*) 'Gluino ball formation probability has been set to '
     &     ,PROBGG

      RETURN
      END


 
C*********************************************************************
 
C...PYGLFR
C...Fragments the string near to a gluino, to form a gluino-hadron, 
C...either by producing a new g-g pair or two new q-qbar ones.

      SUBROUTINE PYGLFRRE(IERR)

C...  Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C      INTEGER PYK,PYCHGE,PYCOMP
C...  Parameter statement to help give large particle numbers
C...  (left- and righthanded SUSY, excited fermions).
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KEXCIT=4000000)
C...  Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...  Note that dimensions below grew from 4000 to 8000 in Pythia 6.2!
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/,/PYDAT3/,/PYPARS/,/PYINT1/,
     &     /PYINT2/
      CHARACTER*16 CHRHAD


C...  Local array.
      DIMENSION PSUM(5),KFSAV(2),PMSAV(2),PSAV(2,5)

      INTEGER IKFRH(20),IKCHGRH(20),IKANTRH(20)
      integer ihad1,ihad2
      REAL*8 PROBGG
      COMMON/RHADB/IKFRH,IKCHGRH,IKANTRH,PROBGG

C...  Free parameter: relative probability for gluino-gluon-ball.
C...  (But occasional low-mass string will never become it anyway.)
c     Rasmus: Moved to rhadb common block to make configurable
c      PROBGG=0.1D0

C...  Free parameter: gluon constituent mass.
      PMGLU=0.7D0

C...  Free parameter: max kinetic energy in gluino-hadron.
      PMKIN=0.5D0

C...  Switch off popcorn baryon production. (Not imperative, but more
C...  failed events when popcorn is allowed.)
      MSTJ12=MSTJ(12)
      MSTJ(12)=1

C...  Convenient shorthand.
      KFGL=KSUSY1+21

C***  Reset error code.
      IERR=0

c      write(*,*) 'In PYGLFRRE'

C...  Loopback point for serious problems, with new try.
      LOOP=0
      CALL PYEDIT(21)
      CHGSAV=PYP(0,6)
 90   LOOP=LOOP+1
      IF(LOOP.GT.1) CALL PYEDIT(22)

C***  Begin new code for testing.
C...  Find if gluino put first or last in closed loop.
 400  IBEGCL=0
      IENDCL=0
      IF(K(1,2).EQ.KFGL.AND.K(1,1).LE.2) IBEGCL=1
      DO 410 I=2,N
         IF(K(I,2).EQ.KFGL) THEN
            IF(K(I-1,1).NE.2.AND.K(I,1).LE.2) IBEGCL=I
            IF(K(I,1).EQ.1) IENDCL=I
         ENDIF
 410  CONTINUE
      IF(IBEGCL.GT.0) IENDCL=0

C...  Range of system beginning or ending with gluino.
      IF(IBEGCL.NE.0) THEN
         IBEG=IBEGCL
         IEND=IBEG
 420     IEND=IEND+1
         IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 420
         IF(IEND.GT.N) IEND=N
      ELSEIF(IENDCL.NE.0) THEN
         IEND=IENDCL
         IBEG=IEND
 430     IBEG=IBEG-1
C JK REMOVING COMPILER WARNING
         IF(IBEG.GE.2.AND.K(IBEG-1,1).EQ.2) THEN
 431        IEND=IEND+1
            IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 431
            IF(IEND.GT.N) IEND=N
         ENDIF
C JK 
      ENDIF

C...  Count number of gluinos in system. Error return if only gluinos.
      IF(IBEGCL.NE.0.OR.IENDCL.NE.0) THEN
         WRITE(*,*) ' Gluino found in closed string; ',
     &        ' special processing begun '
         NGLSYS=0
         DO 435 I=IBEG,IEND
            IF(K(I,2).EQ.KFGL) NGLSYS=NGLSYS+1
 435     CONTINUE
         IF(NGLSYS.EQ.IEND-IBEG+1) THEN
            IERR=1
            MSTJ(12)=MSTJ12
            WRITE(*,*) ' Fail to fragment closed string of gluinos only'
            RETURN
         ENDIF

C...  Decide to split system at random gluon.
 440     ISPLIT=IBEG+INT(PYR(0)*(IEND+1-IBEG))
         IF(ISPLIT.GT.IEND.OR.K(ISPLIT,2).EQ.KFGL) GOTO 440

C...  Copy system from ISPLIT to end, beginning with a d or u quark.
         KFSPL=INT(1.5D0+PYR(0))
         DO 470 I=ISPLIT,IEND
            N=N+1
            DO 450 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 450        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,2)=KFSPL
               DO 460 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 460           CONTINUE
            ENDIF
 470     CONTINUE

C...  Copy system from beginning to ISPLIT, ending with a dbar or ubar quark.
         DO 500 I=IBEG,ISPLIT
            N=N+1
            DO 480 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 480        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,1)=1
               K(N,2)=-KFSPL
               DO 490 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 490           CONTINUE
            ENDIF
 500     CONTINUE

C...  Loop back in case both gluinos need to be addressed this way.
         GOTO 400
      ENDIF
C***  End new code for testing.

C...  Take copy of string system(s), leaving extra free slot after gluino.
C...  (Eventually to be overwritten by one q and one qbar string break.)
      NOLD=N
      NGLUI=0
      DO 120 I=1,NOLD
         ICOPY=0
         IF(K(I,1).EQ.2) ICOPY=1
         IF(K(I,1).EQ.1.AND.I.GE.2) THEN
            IF(K(I-1,1).EQ.12) ICOPY=1
         ENDIF
         IF(ICOPY.EQ.1) THEN
            N=N+1
            DO 100 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 100        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,3)=I
            IF(K(I,2).EQ.KFGL) THEN
               NGLUI=NGLUI+1
               N=N+1
               DO 110 J=1,5
                  K(N,J)=K(N-1,J)
                  P(N,J)=0D0
                  V(N,J)=V(I,J)
 110           CONTINUE
               K(I,5)=N
               K(N,2)=21
            ENDIF
         ENDIF
 120  CONTINUE

C...  Loop over (up to) two gluinos per event.
      DO 300 IGLUI=1,NGLUI

C...  Identify position of gluino (randomize order of treatment).
         IGL=0
         NGL=0
         DO 130 I=1,N
            IF(K(I,1).EQ.2.AND.K(I,2).EQ.KFGL) THEN
               NGL=NGL+1
               IF(IGLUI.EQ.NGLUI) THEN
                  IGL=I
               ELSEIF(NGL.EQ.1) THEN
                  IF(PYR(0).LT.0.5D0) IGL=I
               ELSEIF(IGL.EQ.0) THEN
                  IGL=I
               ENDIF
            ENDIF
 130     CONTINUE

C...  Identify range of partons on string the gluino belongs to.
         IMIN=IGL
 140     IMIN=IMIN-1
         IF(K(IMIN-1,1).EQ.2) GOTO 140
         IMAX=IGL
 150     IMAX=IMAX+1
         IF(K(IMAX,1).EQ.2) GOTO 150

C...  Find mass of this gluino-string.
         DO 170 J=1,5
            PSUM(J)=0D0
            DO 160 I=IMIN,IMAX
               PSUM(J)=PSUM(J)+P(I,J)
 160        CONTINUE
 170     CONTINUE
         PSUM(5)=SQRT(MAX(0D0,PSUM(4)**2-PSUM(1)**2-PSUM(2)**2-
     &        PSUM(3)**2))

C...  If low-mass, then consider gluino-hadron already formed.
         IF(PSUM(5).LE.P(IGL,5)+P(IMIN,5)+P(IMAX,5)+PMKIN) THEN
            DO 180 I=IMIN,IMAX
               K(I,1)=15+IGLUI
 180        CONTINUE
            GOTO 300
         ENDIF

C...  Else break string by production of new gg or two new qqbar pairs.
C...  (Also diquarks allowed, but not popcorn, and not two adjacent.)
         IF(PYR(0).LT.PROBGG) THEN
C...  Let a gluon occupy two slots, to make administration work the same
C...  way as for the qqbar case.
            KFSAV(1)=21
            KFSAV(2)=21
            PMSAV(1)=0.5D0*PMGLU
            PMSAV(2)=0.5D0*PMGLU
         ELSE
 185        CALL PYDCYK(K(IMIN,2),0,KFSAV(1),KFDUM)
            CALL PYDCYK(K(IMAX,2),0,KFSAV(2),KFDUM)
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(KFSAV(2)).GT.10) GOTO 185
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(K(IGL-1,2)).GT.10) GOTO 185
            IF(IABS(KFSAV(2)).GT.10.AND.IABS(K(IGL+2,2)).GT.10) GOTO 185
            KFSAV(1)=ISIGN(MOD(IABS(KFSAV(1)),10000),KFSAV(1))
            KFSAV(2)=ISIGN(MOD(IABS(KFSAV(2)),10000),KFSAV(2))
            MSTJ(93)=1
            PMSAV(1)=PYMASS(KFSAV(1))
            MSTJ(93)=1
            PMSAV(2)=PYMASS(KFSAV(2))
         ENDIF

         KFSVMX=MAX(IABS(KFSAV(1)),IABS(KFSAV(2)))
         KFSVMN=MIN(IABS(KFSAV(1)),IABS(KFSAV(2)))

C...  Mass of gluino-hadron.
         PMGSAV=P(IGL,5)
         PMGB=P(IGL,5)+0.7

C...  Pick at random order in which both sides of gluino string break.
         ISIDE=INT(1.5D0+PYR(0))
         DO 220 ISDE=1,2
            IF(ISDE.EQ.1) IS=ISIDE
            IF(ISDE.EQ.2) IS=3-ISIDE

C...Pick momentum sharing according to fragmentation function as if bottom.
            M11SAV=MSTJ(11)
            PMBSAV=PARF(105)
            MSTJ(11)=4
            PARF(105)=PMGSAV
            CALL PYZDIS(5,0,PMGB**2,ZGL)
            MSTJ(11)=M11SAV
            PARF(105)=PMBSAV
            ZGL=MAX(0.9D0,MIN(0.9999D0,ZGL))
            DO 190 J=1,5
               PSAV(IS,J)=(1D0-ZGL)*P(IGL,J)
               P(IGL,J)=ZGL*P(IGL,J)
 190        CONTINUE

C...  Target gluino-hadron mass for this stage of momentum reshuffling.
            PMOLD=P(IGL,5)
            IF(ISDE.EQ.1) PMNEW=PMGSAV+PMSAV(IS)
            IF(ISDE.EQ.2) PMNEW=PMGB

C...  Recoiling parton from which to shuffle momentum. System momentum.
            IF(IS.EQ.1) IREC=IGL-1
            IF(IS.EQ.2) IREC=IGL+2
 200        DO 210 J=1,4
               PSUM(J)=P(IGL,J)+P(IREC,J)
 210        CONTINUE

C...  Boost to rest frame of system, and align gluino along +z axis.
            CALL PYROBO(IGL,IGL,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            CALL PYROBO(IREC,IREC,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            PHI=PYANGL(P(IGL,1),P(IGL,2))
            CALL PYROBO(IGL,IGL,0D0,-PHI,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,0D0,-PHI,0D0,0D0,0D0)
            THETA=PYANGL(P(IGL,3),P(IGL,1))
            CALL PYROBO(IGL,IGL,-THETA,0D0,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,-THETA,0D0,0D0,0D0,0D0)

C...  Calculate new kinematics in this frame, for desired gluino mass.
            ETOT=P(IGL,4)+P(IREC,4)
            IF(ETOT.GT.PMNEW+P(IREC,5)) THEN
               IFAIL=0
               PZNEW=0.5D0
     &              *SQRT(MAX(0D0,(ETOT**2-PMNEW**2-P(IREC,5)**2)**2-
     &              4D0*PMNEW**2*P(IREC,5)**2))/ETOT
               P(IGL,3)=PZNEW
               P(IGL,4)=SQRT(PZNEW**2+PMNEW**2)
               P(IGL,5)=PMNEW
               P(IREC,3)=-PZNEW
               P(IREC,4)=SQRT(PZNEW**2+P(IREC,5)**2)

C...  If not enough momentum, take what can be taken.
            ELSE
               IFAIL=1
               PMOLD=ETOT-P(IREC,5)
               P(IGL,3)=0D0
               P(IGL,4)=PMOLD
               P(IGL,5)=PMOLD
               P(IREC,3)=0D0
               P(IREC,4)=P(IREC,5)
            ENDIF

C...  Bost back to lab frame.
            CALL PYROBO(IGL,IGL,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))
            CALL PYROBO(IREC,IREC,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))

C...  Loop back when not enough momentum could be shuffled.
C...  (As long as there is something left on either side.)
            IF(IFAIL.EQ.1) THEN
 215           IF(IS.EQ.1.AND.IREC.GT.IMIN) THEN
                  IREC=IREC-1
                  GOTO 200
               ELSEIF(IS.EQ.2.AND.IREC.LT.IMAX) THEN
                  IREC=IREC+1
                  GOTO 200
               ELSEIF(ISDE.EQ.2.AND.IS.EQ.3-ISIDE) THEN
                  IS=ISIDE
                  IREC=IRECSV
                  GOTO 215
               ENDIF
            ENDIF

C...  End loop over fragmentation of two sides around gluino.
            IRECSV=IREC
 220     CONTINUE

C...  New slot at end of record for gluino R-hadron.
         DO 230 J=1,5
            K(N+1,J)=0
            P(N+1,J)=P(IGL,J)
            V(N+1,J)=V(IGL,J)
 230     CONTINUE

C...  Status and code of this slot.
         K(N+1,1)=15+IGLUI

C...  Gluino-ball.
         IF(KFSVMX.EQ.21) THEN
            K(N+1,2)=KSUSY1+993
C...  Gluino-meson.
         ELSEIF(KFSVMX.LT.10) THEN
            IF(KFSVMX.eq.KFSVMN.and.KFSVMX.gt.1) then
               KFSVMN=1
               KFSVMX=1
            endif
            K(N+1,2)=KSUSY1+9000+100*KFSVMX+10*KFSVMN+3
            IF(KFSVMX.EQ.KFSVMN) THEN
            ELSEIF(MOD(KFSVMX,2).EQ.0) THEN
               IF(KFSVMX.EQ.KFSAV(1).OR.KFSVMX.EQ.KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ELSE
               IF(KFSVMX.EQ.-KFSAV(1).OR.KFSVMX.EQ.-KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ENDIF
C...  Gluino-baryon.
         ELSE
            KFSVX1=KFSVMX/1000
            KFSVX2=MOD(KFSVMX/100,10)
            KFA=MAX(KFSVX1,KFSVX2,KFSVMN)
            KFC=MIN(KFSVX1,KFSVX2,KFSVMN)
            KFB=KFSVX1+KFSVX2+KFSVMN-KFA-KFC
            K(N+1,2)=SIGN(KSUSY1+90000+1000*KFA+100*KFB+10*KFC+2,
     &           -KFSAV(1))
            if(iabs(K(N+1,2)).ne.1093212) goto 90
         ENDIF

        CALL PYNAME(K(N+1,2),CHRHAD)
        print*, 'R-hadron KF = ',K(N+1,2),' and name ',CHRHAD
c        print*, 'RMA KC = ',PYCOMP(K(N+1,2))
c        print*, 'RMA CHAF = ',CHAF(PYCOMP(K(N+1,2)),1),', '
c     &       ,CHAF(PYCOMP(K(N+1,2)),1)
         K(N+1,3)=K(IGL,3)
         N=N+1

C...  Code and momentum of two new string endpoints.
         K(IGL,2)=KFSAV(1)
         K(IGL+1,2)=KFSAV(2)
         IF(KFSAV(1).NE.21) K(IGL,1)=1
         DO 240 J=1,5
            P(IGL,J)=PSAV(1,J)
            P(IGL+1,J)=PSAV(2,J)
 240     CONTINUE

C...  End of loop over two gluinos.
 300  CONTINUE

C...  Cleanup: remove zero-energy gluons.
      NNOW=N
      N=NOLD
      DO 330 I=NOLD+1,NNOW
         IF(K(I,2).EQ.21.AND.P(I,4).LT.1D-10) THEN
         ELSEIF(I.EQ.N+1) THEN
            N=N+1
         ELSE
            N=N+1
            DO 320 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 320        CONTINUE
         ENDIF
 330  CONTINUE

C...  Finish off with standard hadronization.
C...  Note that the R-hadrons are not affected here, since they
C...  (deliberately erroneously) have been bookkept as decayed.
      IF(IERR.GT.0) THEN
         WRITE(*,*) '...skipping pyexec for this event!'
      ELSE
c         call pylist(2)
         CALL PYEXEC
      ENDIF

C...  Restore code of gluino-hadrons to non-fragmented numbers.
      N6=0
      N7=0
      DO 340 I=1,N
        IF(K(I,1).EQ.16.OR.K(I,1).EQ.17) K(I,1)=K(I,1)-10
C...      First R-hadron
          IF(K(I,1).EQ.6) then 
            ihad1=i
c            WRITE(*,*) 'R-hadron 1 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
            N6=N6+1
          ENDIF
C...      Second R-hadron         
          IF(K(I,1).EQ.7) then 
            N7=N7+1
            ihad2=i
c            WRITE(*,*) 'R-hadron 2 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
          ENDIF

 340  CONTINUE
      IF(N6.GT.1.OR.N7.GT.1) MSTU(24)=1

C...  Extracheck charge.

      chgnew=pyp(0,6)

      IF(ABS(CHGNEW-CHGSAV).GT.0.1D0) MSTU(24)=1
c      WRITE(*,*) 'CHGNEW CHGSAV',CHGNEW,CHGSAV
 
C...  In case of trouble, make up to five attempts.
      IF(MSTU(24).NE.0.AND.LOOP.LT.5) THEN
         WRITE(*,*) '...give it new try...'
         MSTU(23)=MSTU(23)-1
         GOTO 90
      ELSEIF(MSTU(24).NE.0) THEN
         WRITE(*,*) '...but still fail after repeated attempts!'
C***  Set error flag.
         IERR=2
      ELSEIF(MSTU(24).EQ.0.AND.LOOP.GT.1) THEN
         WRITE(*,*) '...and now it worked!'
      ENDIF

c.... ACH - check for more errors from pyexec
      IF(MSTU(24).GT.0) THEN
         IERR=MSTU(24)
         WRITE(*,*) 'ACH123 IERR being set to ', IERR
      ENDIF

C...  Finished. Restore baryon production model.
      MSTJ(12)=MSTJ12
c      call pylist(2)
      RETURN
      END

C*********************************************************************


 
C...PYGLDC
C...Decays the gluino inside a gluino-hadron.
 
      SUBROUTINE PYGLDC
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C      INTEGER PYK,PYCHGE,PYCOMP
C...Parameter statement to help give large particle numbers
C...(left- and righthanded SUSY, excited fermions).
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KEXCIT=4000000)
C...Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...Note that dimensions below grew from 4000 to 8000 in Pythia 6.2!
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/,/PYDAT3/,/PYPARS/,/PYINT1/,
     &/PYINT2/

C...Optional offset of constituent quark/diquark mass,
C...representing gluon cloud around gluino not part of the decay.
      PMOFF = 0.2D0

C...The probability that it has spin 1.
C...(Except for identical flavours, where spin 1 is only possibility.)
C...(Recall that all R-baryon codes were given as if spin 1, sloppily.)
      PROBS1=0.5D0

C...Loop through to find R-hadrons.
      DO 150 I=1,N
C...If only one of the R-hadrons should decay inside the detector, 
C...you could here pick at random whether to allow the one with 
C...K(I,1) = 6 or the one with =7. (Where the random choice of course
C...could depend on the position of an imagined displaced vertex.)
        IF(K(I,1).EQ.6.OR.K(I,1).EQ.7) THEN

C...Begin resolve R-hadron flavour content to q + qbar or q + qq.
          KFRH=(IABS(K(I,2))-KSUSY1)/10
C...Gluino - g : split g to u ubar or d dbar.
          IF(KFRH.LT.100) THEN
            KF1=1+INT(0.5D0+PYR(0))             
            KF2=-KF1
          ELSEIF(KFRH.LT.1000) THEN
C...Gluino-meson: split to q + qbar.
            KFRH=KFRH-900
            KF1=KFRH/10
            KF2=-MOD(KFRH,10)
            IF(MOD(KF1,2).EQ.1) THEN
              KF1=MOD(KFRH,10)
              KF2=-(KFRH/10)
            ENDIF
c...Gluino-baryon: split to q + qq (diquark). Pick diquark at random.
          ELSE 
            KFRH=KFRH-9000
            KFA=KFRH/100
            KFB=MOD(KFRH/10,10) 
            KFC=MOD(KFRH,10)
            RDIQ=3D0*PYR(0)
            IF(RDIQ.LT.1D0) THEN
              KF1=KFA
              KF2=1000*KFB+100*KFC+3
              IF(KFB.NE.KFC.AND.PROBS1.LT.PYR(0)) KF2=KF2-2
            ELSEIF(RDIQ.LT.2D0) THEN
              KF1=KFB
              KF2=1000*KFA+100*KFC+3
              IF(KFA.NE.KFC.AND.PROBS1.LT.PYR(0)) KF2=KF2-2
            ELSE
              KF1=KFC
              KF2=1000*KFA+100*KFB+3
              IF(KFA.NE.KFB.AND.PROBS1.LT.PYR(0)) KF2=KF2-2
            ENDIF
          ENDIF
C...Flip for anti-R-hadron. 
          IF(K(I,2).LT.0) THEN
            KFSV=KF1
            KF1=-KF2
            KF2=-KFSV
          ENDIF 

C...Subdivide R-hadron into gluino + 2 light (di)quarks in new slots. 
          DO 120 I1=N+1,N+3
            K(I1,1)=3
            K(I1,3)=I
            K(I1,4)=0
            K(I1,5)=0
            DO 110 J=1,5
              V(I1,J)=0D0
  110       CONTINUE
  120     CONTINUE 

C...Store new flavours.
          K(N+1,2)=KSUSY1+21
          K(N+2,2)=KF1
          K(N+3,2)=KF2

C...Set up colour flow.
          K(N+1,4)=MSTU(5)*(N+3) 
          K(N+1,5)=MSTU(5)*(N+2) 
          K(N+2,4)=MSTU(5)*(N+1) 
          K(N+3,5)=MSTU(5)*(N+1)

C...Define effective quark/diquark masses.
          MSTJ(93)=1 
          PM1=PYMASS(KF1)+PMOFF         
          MSTJ(93)=1 
          PM2=PYMASS(KF2)+PMOFF         
         
C...Share gluino"hadron" momentum with two light quarks.
          FAC1=PM1/P(I,5)
          FAC2=PM2/P(I,5)
          DO 130 J=1,5
            P(N+1,J)=(1D0-FAC1-FAC2)*P(I,J)
            P(N+2,J)=FAC1*P(I,J)
            P(N+3,J)=FAC2*P(I,J)
  130     CONTINUE

C...If you want a displaced vertex, you could also write that info
C...into the V array, but you have to calculate that vertex yourself.

C...Mark R-hadron decayed and update number counter.
          K(I,1)=K(I,1)+10
          K(I,4)=N+1
          K(I,5)=N+3
          N=N+3

C...Let gluino decay now.
          CALL PYRESD(N-2)

C...End of loop over two R-hadrons.
        ENDIF
 150  CONTINUE

C...And then the decay products can fragment, etc.
      CALL PYEXEC

    
      RETURN
      END       


CCCCCCCCCCCC

C*********************************************************************
 
C...PYGLFR
C... Gluino fragmentation for Regge model
 
      SUBROUTINE PYGLFR(IERR)

C...  Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C      INTEGER PYK,PYCHGE,PYCOMP
C...  Parameter statement to help give large particle numbers
C...  (left- and righthanded SUSY, excited fermions).
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KEXCIT=4000000)
C...  Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...  Note that dimensions below grew from 4000 to 8000 in Pythia 6.2!
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/,/PYDAT3/,/PYPARS/,/PYINT1/,
     &     /PYINT2/
C...  Local array.
      DIMENSION PSUM(5),KFSAV(2),PMSAV(2),PSAV(2,5)

      INTEGER IKFRH(20),IKCHGRH(20),IKANTRH(20)
      integer ihad1,ihad2
      REAL*8 PROBGG
      COMMON/RHADB/IKFRH,IKCHGRH,IKANTRH,PROBGG
      CHARACTER*16 CHRHAD

C...  Free parameter: relative probability for gluino-gluon-ball.
C...  (But occasional low-mass string will never become it anyway.)
c     Rasmus: Moved to rhadb common block to make configurable
c      PROBGG=0.1D0

C...  Free parameter: gluon constituent mass.
      PMGLU=0.7D0

C...  Free parameter: max kinetic energy in gluino-hadron.
      PMKIN=0.5D0

C...  Switch off popcorn baryon production. (Not imperative, but more
C...  failed events when popcorn is allowed.)
      MSTJ12=MSTJ(12)
      MSTJ(12)=1

C...  Convenient shorthand.
      KFGL=KSUSY1+21

C***  Reset error code.
      IERR=0

      write(*,*) 'In PYGLFR'

C...  Loopback point for serious problems, with new try.
      LOOP=0
      CALL PYEDIT(21)
      CHGSAV=PYP(0,6)
 90   LOOP=LOOP+1
      IF(LOOP.GT.1) CALL PYEDIT(22)

C***  Begin new code for testing.
C...  Find if gluino put first or last in closed loop.
 400  IBEGCL=0
      IENDCL=0
      IF(K(1,2).EQ.KFGL.AND.K(1,1).LE.2) IBEGCL=1
      DO 410 I=2,N
         IF(K(I,2).EQ.KFGL) THEN
            IF(K(I-1,1).NE.2.AND.K(I,1).LE.2) IBEGCL=I
            IF(K(I,1).EQ.1) IENDCL=I
         ENDIF
 410  CONTINUE
      IF(IBEGCL.GT.0) IENDCL=0

C...  Range of system beginning or ending with gluino.
      IF(IBEGCL.NE.0) THEN
         IBEG=IBEGCL
         IEND=IBEG
 420     IEND=IEND+1
         IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 420
         IF(IEND.GT.N) IEND=N
      ELSEIF(IENDCL.NE.0) THEN
         IEND=IENDCL
         IBEG=IEND
 430     IBEG=IBEG-1
C JK REMOVING COMPILER WARNING
         IF(IBEG.GE.2.AND.K(IBEG-1,1).EQ.2) THEN
 431        IEND=IEND+1
            IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 431
            IF(IEND.GT.N) IEND=N
         ENDIF
C JK 
      ENDIF

C...  Count number of gluinos in system. Error return if only gluinos.
      IF(IBEGCL.NE.0.OR.IENDCL.NE.0) THEN
         WRITE(*,*) ' Gluino found in closed string; ',
     &        ' special processing begun '
         NGLSYS=0
         DO 435 I=IBEG,IEND
            IF(K(I,2).EQ.KFGL) NGLSYS=NGLSYS+1
 435     CONTINUE
         IF(NGLSYS.EQ.IEND-IBEG+1) THEN
            IERR=1
            MSTJ(12)=MSTJ12
            WRITE(*,*) ' Fail to fragment closed string of gluinos only'
            RETURN
         ENDIF

C...  Decide to split system at random gluon.
 440     ISPLIT=IBEG+INT(PYR(0)*(IEND+1-IBEG))
         IF(ISPLIT.GT.IEND.OR.K(ISPLIT,2).EQ.KFGL) GOTO 440

C...  Copy system from ISPLIT to end, beginning with a d or u quark.
         KFSPL=INT(1.5D0+PYR(0))
         DO 470 I=ISPLIT,IEND
            N=N+1
            DO 450 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 450        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,2)=KFSPL
               DO 460 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 460           CONTINUE
            ENDIF
 470     CONTINUE

C...  Copy system from beginning to ISPLIT, ending with a dbar or ubar quark.
         DO 500 I=IBEG,ISPLIT
            N=N+1
            DO 480 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 480        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,1)=1
               K(N,2)=-KFSPL
               DO 490 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 490           CONTINUE
            ENDIF
 500     CONTINUE

C...  Loop back in case both gluinos need to be addressed this way.
         GOTO 400
      ENDIF
C***  End new code for testing.

C...  Take copy of string system(s), leaving extra free slot after gluino.
C...  (Eventually to be overwritten by one q and one qbar string break.)
      NOLD=N
      NGLUI=0
      DO 120 I=1,NOLD
         ICOPY=0
         IF(K(I,1).EQ.2) ICOPY=1
         IF(K(I,1).EQ.1.AND.I.GE.2) THEN
            IF(K(I-1,1).EQ.12) ICOPY=1
         ENDIF
         IF(ICOPY.EQ.1) THEN
            N=N+1
            DO 100 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 100        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,3)=I
            IF(K(I,2).EQ.KFGL) THEN
               NGLUI=NGLUI+1
               N=N+1
               DO 110 J=1,5
                  K(N,J)=K(N-1,J)
                  P(N,J)=0D0
                  V(N,J)=V(I,J)
 110           CONTINUE
               K(I,5)=N
               K(N,2)=21
            ENDIF
         ENDIF
 120  CONTINUE

C...  Loop over (up to) two gluinos per event.
      DO 300 IGLUI=1,NGLUI

C...  Identify position of gluino (randomize order of treatment).
         IGL=0
         NGL=0
         DO 130 I=1,N
            IF(K(I,1).EQ.2.AND.K(I,2).EQ.KFGL) THEN
               NGL=NGL+1
               IF(IGLUI.EQ.NGLUI) THEN
                  IGL=I
               ELSEIF(NGL.EQ.1) THEN
                  IF(PYR(0).LT.0.5D0) IGL=I
               ELSEIF(IGL.EQ.0) THEN
                  IGL=I
               ENDIF
            ENDIF
 130     CONTINUE

C...  Identify range of partons on string the gluino belongs to.
         IMIN=IGL
 140     IMIN=IMIN-1
         IF(K(IMIN-1,1).EQ.2) GOTO 140
         IMAX=IGL
 150     IMAX=IMAX+1
         IF(K(IMAX,1).EQ.2) GOTO 150

C...  Find mass of this gluino-string.
         DO 170 J=1,5
            PSUM(J)=0D0
            DO 160 I=IMIN,IMAX
               PSUM(J)=PSUM(J)+P(I,J)
 160        CONTINUE
 170     CONTINUE
         PSUM(5)=SQRT(MAX(0D0,PSUM(4)**2-PSUM(1)**2-PSUM(2)**2-
     &        PSUM(3)**2))

C...  If low-mass, then consider gluino-hadron already formed.
         IF(PSUM(5).LE.P(IGL,5)+P(IMIN,5)+P(IMAX,5)+PMKIN) THEN
            DO 180 I=IMIN,IMAX
               K(I,1)=15+IGLUI
 180        CONTINUE
            GOTO 300
         ENDIF

C...  Else break string by production of new gg or two new qqbar pairs.
C...  (Also diquarks allowed, but not popcorn, and not two adjacent.)
         IF(PYR(0).LT.PROBGG) THEN
C...  Let a gluon occupy two slots, to make administration work the same
C...  way as for the qqbar case.
            KFSAV(1)=21
            KFSAV(2)=21
            PMSAV(1)=0.5D0*PMGLU
            PMSAV(2)=0.5D0*PMGLU
         ELSE
 185        CALL PYDCYK(K(IMIN,2),0,KFSAV(1),KFDUM)
            CALL PYDCYK(K(IMAX,2),0,KFSAV(2),KFDUM)
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(KFSAV(2)).GT.10) GOTO 185
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(K(IGL-1,2)).GT.10) GOTO 185
            IF(IABS(KFSAV(2)).GT.10.AND.IABS(K(IGL+2,2)).GT.10) GOTO 185
            KFSAV(1)=ISIGN(MOD(IABS(KFSAV(1)),10000),KFSAV(1))
            KFSAV(2)=ISIGN(MOD(IABS(KFSAV(2)),10000),KFSAV(2))
            MSTJ(93)=1
            PMSAV(1)=PYMASS(KFSAV(1))
            MSTJ(93)=1
            PMSAV(2)=PYMASS(KFSAV(2))
         ENDIF

C...  Mass of gluino-hadron.
         PMGSAV=P(IGL,5)
         PMGB=P(IGL,5)+PMSAV(1)+PMSAV(2)

C...  Pick at random order in which both sides of gluino string break.
         ISIDE=INT(1.5D0+PYR(0))
         DO 220 ISDE=1,2
            IF(ISDE.EQ.1) IS=ISIDE
            IF(ISDE.EQ.2) IS=3-ISIDE

C...Pick momentum sharing according to fragmentation function as if bottom.
            M11SAV=MSTJ(11)
            PMBSAV=PARF(105)
            MSTJ(11)=4
            PARF(105)=PMGSAV
            CALL PYZDIS(5,0,PMGB**2,ZGL)
            MSTJ(11)=M11SAV
            PARF(105)=PMBSAV
            ZGL=MAX(0.9D0,MIN(0.9999D0,ZGL))
            DO 190 J=1,5
               PSAV(IS,J)=(1D0-ZGL)*P(IGL,J)
               P(IGL,J)=ZGL*P(IGL,J)
 190        CONTINUE

C...  Target gluino-hadron mass for this stage of momentum reshuffling.
            PMOLD=P(IGL,5)
            IF(ISDE.EQ.1) PMNEW=PMGSAV+PMSAV(IS)
            IF(ISDE.EQ.2) PMNEW=PMGB

C...  Recoiling parton from which to shuffle momentum. System momentum.
            IF(IS.EQ.1) IREC=IGL-1
            IF(IS.EQ.2) IREC=IGL+2
 200        DO 210 J=1,4
               PSUM(J)=P(IGL,J)+P(IREC,J)
 210        CONTINUE

C...  Boost to rest frame of system, and align gluino along +z axis.
            CALL PYROBO(IGL,IGL,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            CALL PYROBO(IREC,IREC,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            PHI=PYANGL(P(IGL,1),P(IGL,2))
            CALL PYROBO(IGL,IGL,0D0,-PHI,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,0D0,-PHI,0D0,0D0,0D0)
            THETA=PYANGL(P(IGL,3),P(IGL,1))
            CALL PYROBO(IGL,IGL,-THETA,0D0,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,-THETA,0D0,0D0,0D0,0D0)

C...  Calculate new kinematics in this frame, for desired gluino mass.
            ETOT=P(IGL,4)+P(IREC,4)
            IF(ETOT.GT.PMNEW+P(IREC,5)) THEN
               IFAIL=0
               PZNEW=0.5D0
     &              *SQRT(MAX(0D0,(ETOT**2-PMNEW**2-P(IREC,5)**2)**2-
     &              4D0*PMNEW**2*P(IREC,5)**2))/ETOT
               P(IGL,3)=PZNEW
               P(IGL,4)=SQRT(PZNEW**2+PMNEW**2)
               P(IGL,5)=PMNEW
               P(IREC,3)=-PZNEW
               P(IREC,4)=SQRT(PZNEW**2+P(IREC,5)**2)

C...  If not enough momentum, take what can be taken.
            ELSE
               IFAIL=1
               PMOLD=ETOT-P(IREC,5)
               P(IGL,3)=0D0
               P(IGL,4)=PMOLD
               P(IGL,5)=PMOLD
               P(IREC,3)=0D0
               P(IREC,4)=P(IREC,5)
            ENDIF

C...  Bost back to lab frame.
            CALL PYROBO(IGL,IGL,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))
            CALL PYROBO(IREC,IREC,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))

C...  Loop back when not enough momentum could be shuffled.
C...  (As long as there is something left on either side.)
            IF(IFAIL.EQ.1) THEN
 215           IF(IS.EQ.1.AND.IREC.GT.IMIN) THEN
                  IREC=IREC-1
                  GOTO 200
               ELSEIF(IS.EQ.2.AND.IREC.LT.IMAX) THEN
                  IREC=IREC+1
                  GOTO 200
               ELSEIF(ISDE.EQ.2.AND.IS.EQ.3-ISIDE) THEN
                  IS=ISIDE
                  IREC=IRECSV
                  GOTO 215
               ENDIF
            ENDIF

C...  End loop over fragmentation of two sides around gluino.
            IRECSV=IREC
 220     CONTINUE

C...  New slot at end of record for gluino R-hadron.
         DO 230 J=1,5
            K(N+1,J)=0
            P(N+1,J)=P(IGL,J)
            V(N+1,J)=V(IGL,J)
 230     CONTINUE

C...  Status and code of this slot.
         K(N+1,1)=15+IGLUI
         KFSVMX=MAX(IABS(KFSAV(1)),IABS(KFSAV(2)))
         KFSVMN=MIN(IABS(KFSAV(1)),IABS(KFSAV(2)))
C...  Gluino-ball.
         IF(KFSVMX.EQ.21) THEN
            K(N+1,2)=KSUSY1+993
C...  Gluino-meson.
         ELSEIF(KFSVMX.LT.10) THEN
            K(N+1,2)=KSUSY1+9000+100*KFSVMX+10*KFSVMN+3
            IF(KFSVMX.EQ.KFSVMN) THEN
            ELSEIF(MOD(KFSVMX,2).EQ.0) THEN
               IF(KFSVMX.EQ.KFSAV(1).OR.KFSVMX.EQ.KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ELSE
               IF(KFSVMX.EQ.-KFSAV(1).OR.KFSVMX.EQ.-KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ENDIF
C...  Gluino-baryon.
         ELSE
            KFSVX1=KFSVMX/1000
            KFSVX2=MOD(KFSVMX/100,10)
            KFA=MAX(KFSVX1,KFSVX2,KFSVMN)
            KFC=MIN(KFSVX1,KFSVX2,KFSVMN)
            KFB=KFSVX1+KFSVX2+KFSVMN-KFA-KFC
            K(N+1,2)=SIGN(KSUSY1+90000+1000*KFA+100*KFB+10*KFC+4,
     &           -KFSAV(1))
         ENDIF
        CALL PYNAME(K(N+1,2),CHRHAD)
        print*, 'R-hadron KF = ',K(N+1,2),' and name ',CHRHAD


         K(N+1,3)=K(IGL,3)
         N=N+1

C...  Code and momentum of two new string endpoints.
         K(IGL,2)=KFSAV(1)
         K(IGL+1,2)=KFSAV(2)
         IF(KFSAV(1).NE.21) K(IGL,1)=1
         DO 240 J=1,5
            P(IGL,J)=PSAV(1,J)
            P(IGL+1,J)=PSAV(2,J)
 240     CONTINUE

C...  End of loop over two gluinos.
 300  CONTINUE

C...  Cleanup: remove zero-energy gluons.
      NNOW=N
      N=NOLD
      DO 330 I=NOLD+1,NNOW
         IF(K(I,2).EQ.21.AND.P(I,4).LT.1D-10) THEN
         ELSEIF(I.EQ.N+1) THEN
            N=N+1
         ELSE
            N=N+1
            DO 320 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 320        CONTINUE
         ENDIF
 330  CONTINUE

C...  Finish off with standard hadronization.
C...  Note that the R-hadrons are not affected here, since they
C...  (deliberately erroneously) have been bookkept as decayed.
      IF(IERR.GT.0) THEN
         WRITE(*,*) '...skipping pyexec for this event!'
      ELSE
c         call pylist(2)
         CALL PYEXEC
      ENDIF

C...  Restore code of gluino-hadrons to non-fragmented numbers.
      N6=0
      N7=0
      DO 340 I=1,N
        IF(K(I,1).EQ.16.OR.K(I,1).EQ.17) K(I,1)=K(I,1)-10
C...      First R-hadron
          IF(K(I,1).EQ.6) then 
            ihad1=i
c            WRITE(*,*) 'R-hadron 1 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
            N6=N6+1
          ENDIF
C...      Second R-hadron         
          IF(K(I,1).EQ.7) then 
            N7=N7+1
            ihad2=i
c            WRITE(*,*) 'R-hadron 2 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
          ENDIF

 340  CONTINUE
      IF(N6.GT.1.OR.N7.GT.1) MSTU(24)=1

C...  Extracheck charge.

      chgnew=pyp(0,6)

      IF(ABS(CHGNEW-CHGSAV).GT.0.1D0) MSTU(24)=1
c      WRITE(*,*) 'CHGNEW CHGSAV',CHGNEW,CHGSAV
 
C...  In case of trouble, make up to five attempts.
      IF(MSTU(24).NE.0.AND.LOOP.LT.5) THEN
         WRITE(*,*) '...give it new try...'
         MSTU(23)=MSTU(23)-1
         GOTO 90
      ELSEIF(MSTU(24).NE.0) THEN
         WRITE(*,*) '...but still fail after repeated attempts!'
C***  Set error flag.
         IERR=2
      ELSEIF(MSTU(24).EQ.0.AND.LOOP.GT.1) THEN
         WRITE(*,*) '...and now it worked!'
      ENDIF

c.... ACH - check for more errors from pyexec
      IF(MSTU(24).GT.0) THEN
         IERR=MSTU(24)
         WRITE(*,*) 'ACH123 IERR being set to ', IERR
      ENDIF

C...  Finished. Restore baryon production model.
      MSTJ(12)=MSTJ12
c      call pylist(2)
      RETURN
      END

cccccccccccc

C*********************************************************************
 
C...PYGLFRIN
C... Gluino fragmentation for Regge model
 
      SUBROUTINE PYGLFRIN(IERR)

C...  Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C      INTEGER PYK,PYCHGE,PYCOMP
C...  Parameter statement to help give large particle numbers
C...  (left- and righthanded SUSY, excited fermions).
      PARAMETER (KSUSY1=1000000,KSUSY2=2000000,KEXCIT=4000000)
C...  Commonblocks.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
C...  Note that dimensions below grew from 4000 to 8000 in Pythia 6.2!
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/,/PYDAT3/,/PYPARS/,/PYINT1/,
     &     /PYINT2/
C...  Local array.
      DIMENSION PSUM(5),KFSAV(2),PMSAV(2),PSAV(2,5)

      INTEGER IKFRH(20),IKCHGRH(20),IKANTRH(20)
      integer ihad1,ihad2
      REAL*8 PROBGG
      COMMON/RHADB/IKFRH,IKCHGRH,IKANTRH,PROBGG
      CHARACTER*16 CHRHAD

C...  Free parameter: relative probability for gluino-gluon-ball.
C...  (But occasional low-mass string will never become it anyway.)
c     Rasmus: Moved to rhadb common block to make configurable
c      PROBGG=0.1D0

C...  Free parameter: gluon constituent mass.
      PMGLU=0.33D0

C...  Free parameter: max kinetic energy in gluino-hadron.
      PMKIN=0.5D0

C...  Switch off popcorn baryon production. (Not imperative, but more
C...  failed events when popcorn is allowed.)
      MSTJ12=MSTJ(12)
      MSTJ(12)=1

C...  Convenient shorthand.
      KFGL=KSUSY1+21

C***  Reset error code.
      IERR=0

      write(*,*) 'In PYGLFRIN'

C...  Loopback point for serious problems, with new try.
      LOOP=0
      CALL PYEDIT(21)
      CHGSAV=PYP(0,6)
 90   LOOP=LOOP+1
      IF(LOOP.GT.1) CALL PYEDIT(22)

C***  Begin new code for testing.
C...  Find if gluino put first or last in closed loop.
 400  IBEGCL=0
      IENDCL=0
      IF(K(1,2).EQ.KFGL.AND.K(1,1).LE.2) IBEGCL=1
      DO 410 I=2,N
         IF(K(I,2).EQ.KFGL) THEN
            IF(K(I-1,1).NE.2.AND.K(I,1).LE.2) IBEGCL=I
            IF(K(I,1).EQ.1) IENDCL=I
         ENDIF
 410  CONTINUE
      IF(IBEGCL.GT.0) IENDCL=0

C...  Range of system beginning or ending with gluino.
      IF(IBEGCL.NE.0) THEN
         IBEG=IBEGCL
         IEND=IBEG
 420     IEND=IEND+1
         IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 420
         IF(IEND.GT.N) IEND=N
      ELSEIF(IENDCL.NE.0) THEN
         IEND=IENDCL
         IBEG=IEND
 430     IBEG=IBEG-1
C JK REMOVING COMPILER WARNING
         IF(IBEG.GE.2.AND.K(IBEG-1,1).EQ.2) THEN
 431        IEND=IEND+1
            IF(IEND.LT.N.AND.K(IEND,1).EQ.2) GOTO 431
            IF(IEND.GT.N) IEND=N
C JK 
         ENDIF
      ENDIF

C...  Count number of gluinos in system. Error return if only gluinos.
      IF(IBEGCL.NE.0.OR.IENDCL.NE.0) THEN
         WRITE(*,*) ' Gluino found in closed string; ',
     &        ' special processing begun '
         NGLSYS=0
         DO 435 I=IBEG,IEND
            IF(K(I,2).EQ.KFGL) NGLSYS=NGLSYS+1
 435     CONTINUE
         IF(NGLSYS.EQ.IEND-IBEG+1) THEN
            IERR=1
            MSTJ(12)=MSTJ12
            WRITE(*,*) ' Fail to fragment closed string of gluinos only'
            RETURN
         ENDIF

C...  Decide to split system at random gluon.
 440     ISPLIT=IBEG+INT(PYR(0)*(IEND+1-IBEG))
         IF(ISPLIT.GT.IEND.OR.K(ISPLIT,2).EQ.KFGL) GOTO 440

C...  Copy system from ISPLIT to end, beginning with a d or u quark.
         KFSPL=INT(1.5D0+PYR(0))
         DO 470 I=ISPLIT,IEND
            N=N+1
            DO 450 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 450        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,2)=KFSPL
               DO 460 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 460           CONTINUE
            ENDIF
 470     CONTINUE

C...  Copy system from beginning to ISPLIT, ending with a dbar or ubar quark.
         DO 500 I=IBEG,ISPLIT
            N=N+1
            DO 480 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 480        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,1)=2
            K(N,3)=I
            IF(I.EQ.ISPLIT) THEN
               K(N,1)=1
               K(N,2)=-KFSPL
               DO 490 J=1,5
                  P(N,J)=0.5D0*P(N,J)
 490           CONTINUE
            ENDIF
 500     CONTINUE

C...  Loop back in case both gluinos need to be addressed this way.
         GOTO 400
      ENDIF
C***  End new code for testing.

C...  Take copy of string system(s), leaving extra free slot after gluino.
C...  (Eventually to be overwritten by one q and one qbar string break.)
      NOLD=N
      NGLUI=0
      DO 120 I=1,NOLD
         ICOPY=0
         IF(K(I,1).EQ.2) ICOPY=1
         IF(K(I,1).EQ.1.AND.I.GE.2) THEN
            IF(K(I-1,1).EQ.12) ICOPY=1
         ENDIF
         IF(ICOPY.EQ.1) THEN
            N=N+1
            DO 100 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 100        CONTINUE
            K(I,1)=K(I,1)+10
            K(I,4)=N
            K(I,5)=N
            K(N,3)=I
            IF(K(I,2).EQ.KFGL) THEN
               NGLUI=NGLUI+1
               N=N+1
               DO 110 J=1,5
                  K(N,J)=K(N-1,J)
                  P(N,J)=0D0
                  V(N,J)=V(I,J)
 110           CONTINUE
               K(I,5)=N
               K(N,2)=21
            ENDIF
         ENDIF
 120  CONTINUE

C...  Loop over (up to) two gluinos per event.
      DO 300 IGLUI=1,NGLUI

C...  Identify position of gluino (randomize order of treatment).
         IGL=0
         NGL=0
         DO 130 I=1,N
            IF(K(I,1).EQ.2.AND.K(I,2).EQ.KFGL) THEN
               NGL=NGL+1
               IF(IGLUI.EQ.NGLUI) THEN
                  IGL=I
               ELSEIF(NGL.EQ.1) THEN
                  IF(PYR(0).LT.0.5D0) IGL=I
               ELSEIF(IGL.EQ.0) THEN
                  IGL=I
               ENDIF
            ENDIF
 130     CONTINUE

C...  Identify range of partons on string the gluino belongs to.
         IMIN=IGL
 140     IMIN=IMIN-1
         IF(K(IMIN-1,1).EQ.2) GOTO 140
         IMAX=IGL
 150     IMAX=IMAX+1
         IF(K(IMAX,1).EQ.2) GOTO 150

C...  Find mass of this gluino-string.
         DO 170 J=1,5
            PSUM(J)=0D0
            DO 160 I=IMIN,IMAX
               PSUM(J)=PSUM(J)+P(I,J)
 160        CONTINUE
 170     CONTINUE
         PSUM(5)=SQRT(MAX(0D0,PSUM(4)**2-PSUM(1)**2-PSUM(2)**2-
     &        PSUM(3)**2))

C...  If low-mass, then consider gluino-hadron already formed.
         IF(PSUM(5).LE.P(IGL,5)+P(IMIN,5)+P(IMAX,5)+PMKIN) THEN
            DO 180 I=IMIN,IMAX
               K(I,1)=15+IGLUI
 180        CONTINUE
            GOTO 300
         ENDIF

C...  Else break string by production of new gg or two new qqbar pairs.
C...  (Also diquarks allowed, but not popcorn, and not two adjacent.)
         IF(PYR(0).LT.PROBGG) THEN
C...  Let a gluon occupy two slots, to make administration work the same
C...  way as for the qqbar case.
            KFSAV(1)=21
            KFSAV(2)=21
            PMSAV(1)=0.5D0*PMGLU
            PMSAV(2)=0.5D0*PMGLU
         ELSE
 185        CALL PYDCYK(K(IMIN,2),0,KFSAV(1),KFDUM)
            CALL PYDCYK(K(IMAX,2),0,KFSAV(2),KFDUM)
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(KFSAV(2)).GT.10) GOTO 185
            IF(IABS(KFSAV(1)).GT.10.AND.IABS(K(IGL-1,2)).GT.10) GOTO 185
            IF(IABS(KFSAV(2)).GT.10.AND.IABS(K(IGL+2,2)).GT.10) GOTO 185
            KFSAV(1)=ISIGN(MOD(IABS(KFSAV(1)),10000),KFSAV(1))
            KFSAV(2)=ISIGN(MOD(IABS(KFSAV(2)),10000),KFSAV(2))
            MSTJ(93)=1
            PMSAV(1)=PYMASS(KFSAV(1))
            MSTJ(93)=1
            PMSAV(2)=PYMASS(KFSAV(2))
         ENDIF

C...  Mass of gluino-hadron.
         PMGSAV=P(IGL,5)
c         PMGB=P(IGL,5)+PMSAV(1)+PMSAV(2)
         if(IABS(KFSAV(1)).lt.10.and.IABS(KFSAV(2)).lt.10) then
            if(IABS(KFSAV(1)).ne.3.and.IABS(KFSAV(2)).ne.3) then
               PMGB=P(IGL,5)+0.33
            else
               PMGB=P(IGL,5)+0.46
            endif
         else
            PMGB=P(IGL,5)+PMSAV(1)+PMSAV(2)
         endif

C...  Pick at random order in which both sides of gluino string break.
         ISIDE=INT(1.5D0+PYR(0))
         DO 220 ISDE=1,2
            IF(ISDE.EQ.1) IS=ISIDE
            IF(ISDE.EQ.2) IS=3-ISIDE

C...Pick momentum sharing according to fragmentation function as if bottom.
            M11SAV=MSTJ(11)
            PMBSAV=PARF(105)
            MSTJ(11)=4
            PARF(105)=PMGSAV
            CALL PYZDIS(5,0,PMGB**2,ZGL)
            MSTJ(11)=M11SAV
            PARF(105)=PMBSAV
            ZGL=MAX(0.9D0,MIN(0.9999D0,ZGL))
            DO 190 J=1,5
               PSAV(IS,J)=(1D0-ZGL)*P(IGL,J)
               P(IGL,J)=ZGL*P(IGL,J)
 190        CONTINUE

C...  Target gluino-hadron mass for this stage of momentum reshuffling.
            PMOLD=P(IGL,5)
            IF(ISDE.EQ.1) PMNEW=PMGSAV+PMSAV(IS)
            IF(ISDE.EQ.2) PMNEW=PMGB

C...  Recoiling parton from which to shuffle momentum. System momentum.
            IF(IS.EQ.1) IREC=IGL-1
            IF(IS.EQ.2) IREC=IGL+2
 200        DO 210 J=1,4
               PSUM(J)=P(IGL,J)+P(IREC,J)
 210        CONTINUE

C...  Boost to rest frame of system, and align gluino along +z axis.
            CALL PYROBO(IGL,IGL,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            CALL PYROBO(IREC,IREC,0D0,0D0,-(PSUM(1)/PSUM(4)),
     &           -(PSUM(2)/PSUM(4)),-(PSUM(3)/PSUM(4)))
            PHI=PYANGL(P(IGL,1),P(IGL,2))
            CALL PYROBO(IGL,IGL,0D0,-PHI,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,0D0,-PHI,0D0,0D0,0D0)
            THETA=PYANGL(P(IGL,3),P(IGL,1))
            CALL PYROBO(IGL,IGL,-THETA,0D0,0D0,0D0,0D0)
            CALL PYROBO(IREC,IREC,-THETA,0D0,0D0,0D0,0D0)

C...  Calculate new kinematics in this frame, for desired gluino mass.
            ETOT=P(IGL,4)+P(IREC,4)
            IF(ETOT.GT.PMNEW+P(IREC,5)) THEN
               IFAIL=0
               PZNEW=0.5D0
     &              *SQRT(MAX(0D0,(ETOT**2-PMNEW**2-P(IREC,5)**2)**2-
     &              4D0*PMNEW**2*P(IREC,5)**2))/ETOT
               P(IGL,3)=PZNEW
               P(IGL,4)=SQRT(PZNEW**2+PMNEW**2)
               P(IGL,5)=PMNEW
               P(IREC,3)=-PZNEW
               P(IREC,4)=SQRT(PZNEW**2+P(IREC,5)**2)

C...  If not enough momentum, take what can be taken.
            ELSE
               IFAIL=1
               PMOLD=ETOT-P(IREC,5)
               P(IGL,3)=0D0
               P(IGL,4)=PMOLD
               P(IGL,5)=PMOLD
               P(IREC,3)=0D0
               P(IREC,4)=P(IREC,5)
            ENDIF

C...  Bost back to lab frame.
            CALL PYROBO(IGL,IGL,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))
            CALL PYROBO(IREC,IREC,THETA,PHI,PSUM(1)/PSUM(4),
     &           PSUM(2)/PSUM(4),PSUM(3)/PSUM(4))

C...  Loop back when not enough momentum could be shuffled.
C...  (As long as there is something left on either side.)
            IF(IFAIL.EQ.1) THEN
 215           IF(IS.EQ.1.AND.IREC.GT.IMIN) THEN
                  IREC=IREC-1
                  GOTO 200
               ELSEIF(IS.EQ.2.AND.IREC.LT.IMAX) THEN
                  IREC=IREC+1
                  GOTO 200
               ELSEIF(ISDE.EQ.2.AND.IS.EQ.3-ISIDE) THEN
                  IS=ISIDE
                  IREC=IRECSV
                  GOTO 215
               ENDIF
            ENDIF

C...  End loop over fragmentation of two sides around gluino.
            IRECSV=IREC
 220     CONTINUE

C...  New slot at end of record for gluino R-hadron.
         DO 230 J=1,5
            K(N+1,J)=0
            P(N+1,J)=P(IGL,J)
            V(N+1,J)=V(IGL,J)
 230     CONTINUE

C...  Status and code of this slot.
         K(N+1,1)=15+IGLUI
         KFSVMX=MAX(IABS(KFSAV(1)),IABS(KFSAV(2)))
         KFSVMN=MIN(IABS(KFSAV(1)),IABS(KFSAV(2)))
C...  Gluino-ball.
         IF(KFSVMX.EQ.21) THEN
            K(N+1,2)=KSUSY1+991
C...  Gluino-meson.
         ELSEIF(KFSVMX.LT.10) THEN
            IF(KFSVMX.eq.KFSVMN.and.KFSVMX.gt.1) then
               KFSVMN=1
               KFSVMX=1
            endif
            K(N+1,2)=KSUSY1+9000+100*KFSVMX+10*KFSVMN+1
            IF(KFSVMX.EQ.KFSVMN) THEN
            ELSEIF(MOD(KFSVMX,2).EQ.0) THEN
               IF(KFSVMX.EQ.KFSAV(1).OR.KFSVMX.EQ.KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ELSE
               IF(KFSVMX.EQ.-KFSAV(1).OR.KFSVMX.EQ.-KFSAV(2))
     &              K(N+1,2)=-K(N+1,2)
            ENDIF
C...  Gluino-baryon.
         ELSE
            KFSVX1=KFSVMX/1000
            KFSVX2=MOD(KFSVMX/100,10)
            KFA=MAX(KFSVX1,KFSVX2,KFSVMN)
            KFC=MIN(KFSVX1,KFSVX2,KFSVMN)
            KFSPIN=(MOD(KFSVMX,10)-1)/2
            KFB=KFSVX1+KFSVX2+KFSVMN-KFA-KFC
 235        continue
            K(N+1,2)=SIGN(KSUSY1+90000+1000*KFA+100*KFB+10*KFC+2*KFSPIN
     &           +2, -KFSAV(1))
            if(KFA.eq.KFC.and.KFA.eq.KFB) goto 90
            if(CHRHAD.eq.' '.and.KFA.eq.3) then
               KFA=1
 238           if(KFC.gt.KFA.or.KFB.gt.KFA.or.KFC.gt.KFB) then
                  if(KFB.gt.KFA) then
                     KFTMP=KFA
                     KFA=KFB
                     KFB=KFTMP
                  endif
                  if(KFC.gt.KFB) then
                     KFTMP=KFB
                     KFB=KFC
                     KFC=KFTMP
                  endif
                  goto 238
               endif
               goto 235
            endif
            CALL PYNAME(K(N+1,2),CHRHAD)
            if(CHRHAD(1:1).ne.'~') goto 90
         ENDIF
         CALL PYNAME(K(N+1,2),CHRHAD)
         if(CHRHAD(1:1).ne.'~') goto 90
         print*, 'R-hadron KF = ',K(N+1,2),' and name ',CHRHAD
         P(N+1,5)=PYMASS(K(N+1,2))
         P(N+1,4)=SQRT(
     &        P(N+1,1)**2+P(N+1,2)**2+P(N+1,3)**2+P(N+1,5)**2
     &        )
         
         
         K(N+1,3)=K(IGL,3)
         N=N+1

C...  Code and momentum of two new string endpoints.
         K(IGL,2)=KFSAV(1)
         K(IGL+1,2)=KFSAV(2)
         IF(KFSAV(1).NE.21) K(IGL,1)=1
         DO 240 J=1,5
            P(IGL,J)=PSAV(1,J)
            P(IGL+1,J)=PSAV(2,J)
 240     CONTINUE

C...  End of loop over two gluinos.
 300  CONTINUE

C...  Cleanup: remove zero-energy gluons.
      NNOW=N
      N=NOLD
      DO 330 I=NOLD+1,NNOW
         IF(K(I,2).EQ.21.AND.P(I,4).LT.1D-10) THEN
         ELSEIF(I.EQ.N+1) THEN
            N=N+1
         ELSE
            N=N+1
            DO 320 J=1,5
               K(N,J)=K(I,J)
               P(N,J)=P(I,J)
               V(N,J)=V(I,J)
 320        CONTINUE
         ENDIF
 330  CONTINUE

C...  Finish off with standard hadronization.
C...  Note that the R-hadrons are not affected here, since they
C...  (deliberately erroneously) have been bookkept as decayed.
      IF(IERR.GT.0) THEN
         WRITE(*,*) '...skipping pyexec for this event!'
      ELSE
c         call pylist(2)
         CALL PYEXEC
      ENDIF

C...  Restore code of gluino-hadrons to non-fragmented numbers.
      N6=0
      N7=0
      DO 340 I=1,N
        IF(K(I,1).EQ.16.OR.K(I,1).EQ.17) K(I,1)=K(I,1)-10
C...      First R-hadron
          IF(K(I,1).EQ.6) then 
            ihad1=i
c            WRITE(*,*) 'R-hadron 1 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
            N6=N6+1
          ENDIF
C...      Second R-hadron         
          IF(K(I,1).EQ.7) then 
            N7=N7+1
            ihad2=i
c            WRITE(*,*) 'R-hadron 2 chg:',PYCHGE(K(I,2))/3D0,K(I,2)
          ENDIF

 340  CONTINUE
      IF(N6.GT.1.OR.N7.GT.1) MSTU(24)=1

C...  Extracheck charge.

      chgnew=pyp(0,6)

      IF(ABS(CHGNEW-CHGSAV).GT.0.1D0) MSTU(24)=1
c      WRITE(*,*) 'CHGNEW CHGSAV',CHGNEW,CHGSAV
 
C...  In case of trouble, make up to five attempts.
      IF(MSTU(24).NE.0.AND.LOOP.LT.5) THEN
         WRITE(*,*) '...give it new try...'
         MSTU(23)=MSTU(23)-1
         GOTO 90
      ELSEIF(MSTU(24).NE.0) THEN
         WRITE(*,*) '...but still fail after repeated attempts!'
C***  Set error flag.
         IERR=2
      ELSEIF(MSTU(24).EQ.0.AND.LOOP.GT.1) THEN
         WRITE(*,*) '...and now it worked!'
      ENDIF

c.... ACH - check for more errors from pyexec
      IF(MSTU(24).GT.0) THEN
         IERR=MSTU(24)
         WRITE(*,*) 'ACH123 IERR being set to ', IERR
      ENDIF

C...  Finished. Restore baryon production model.
      MSTJ(12)=MSTJ12
c      call pylist(2)
      RETURN
      END
