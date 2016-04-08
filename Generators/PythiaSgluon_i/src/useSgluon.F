      SUBROUTINE USESGLUON

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      DOUBLE PRECISION qV1(MAXNUP)
      DOUBLE PRECISION qV2(MAXNUP)
      DOUBLE PRECISION mass_sg
      DOUBLE PRECISION mass_sg2
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/
      DOUBLE PRECISION PYR
      EXTERNAL PYR
C...
      DATA PI/3.141592653589793D0/
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM

      INTEGER PYCOMP
c      DOUBLE PRECISION  PMAS, PARF, VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYDAT2/

      SIGEV = 0.D0
      Q2 = PMAS(PYCOMP(5100021),1)**2
      AQEDUP = PYALEM(Q2)
C      AQCDUP = PYALPS(Q2) 
      AQCDUP = ALPSSGL(Q2) 
C...Define number of partons involved and reset to zero
C...(except status code).
      NUP=4
      ISTUP(1) = -1
      ISTUP(2) = -1
      ISTUP(3) = 1
      ISTUP(4) = 1
      DO I = 1, NUP
         MOTHUP(1,I) = 0
         MOTHUP(2,I) = 0
         IDUP(I) = 0
         VTIMUP(I) = 0.D0
         SPINUP(I) = 9
         ICOLUP(1,I) = 0
         ICOLUP(2,I) = 0
         DO J = 1, 5
            PUP(J,I) = 0.D0
         ENDDO
      ENDDO

C...Call the respective routine to generate event.
      IF(IDPRUP.EQ.661) THEN
C... select combination according to SUM, ISIDE=1: q + qbar, ISIDE=2 qbar+q (prod)
         CALL GXSECQQSS(SIGEV)
         if ( SIGEV.EQ.0.D0 ) THEN
c            print *,'IDPRUP=', IDPRUP, ' CROSS SECTION:',SIGEV
            GOTO 4722
         ENDIF
         RANSUM=PYR(0.D0)*SUM
         ISEL=0
 110     ISEL=ISEL+1
         RANSUM=RANSUM-TERM(ISEL)
         IF(ISEL.LT.10.AND.RANSUM.GT.0D0) GOTO 110
         KFLQ=(ISEL+1)/2
         ISIDE=1
         IF(ISEL-2*KFLQ.EQ.0) ISIDE=2         
C...Flavour codes for entries. 
         IF ( ISIDE. EQ. 1) THEN
            IDUP(1) = KFLQ
            IDUP(2) = -KFLQ
            IDUP(3) = 5100021
            IDUP(4) = -5100021
         ELSE
            IDUP(1) = -KFLQ
            IDUP(2) = KFLQ
            IDUP(3) = 5100021
            IDUP(4) = -5100021
         ENDIF
C...  Masses of final state entries (initial assumed massless).
         PUP(5,4)=PMAS(PYCOMP(5100021),1)
         PUP(5,3)=PMAS(PYCOMP(5100021),1)
c         print *,"mass : ", PUP(5,4)

C...Four-momenta of the incoming partons simple.
         PUP(4,1)=0.5D0*X1SEL*DSQRT(STOTAL)
         PUP(3,1)=PUP(4,1)
         PUP(4,2)=0.5D0*X2SEL*DSQRT(STOTAL)
         PUP(3,2)=-PUP(4,2)
C...Energies and absolute momentum of the outgoing partons in 
C...the subsystem frame.
         ESGL = 0.5*DSQRT(SHAT)
         PABS = 0.5D0*DSQRT(SHAT -4.0D0*PMAS(PYCOMP(5100021),1)**2)
C...Subsystem scattering angle defined neglecting quark mass.
C... qqbar is 1-cothe**2 thus maximum at ctp=0, 10% safety
         PCTHETAM = CROSS1(0.D0)*1.1D0
 4711    CONTINUE
         COSTHE    = PYR(0.D0)*2.D0-1.D0
         CTHETAAMP = PYR(0.D0)*PCTHETAM
C         print *,'DIRK CTHETA',PCTHETAM,COSTHE
         if ( cross1(COSTHE).LT.CTHETAAMP ) GOTO 4711
c         PRINT *,'end call cross1'
         SINTHE=SQRT(MAX(0D0,1D0-COSTHE**2))
C...Azimuthal angle at random.
         PHI=2D0*PI*PYR(0.D0)
C...Momenta of outgoing partons in the subsystem frame.
         PUP(1,3)=PABS*SINTHE*COS(PHI)
         PUP(2,3)=PABS*SINTHE*SIN(PHI)
         PZ3     =PABS*COSTHE
         PUP(1,4)=-PUP(1,3)
         PUP(2,4)=-PUP(2,3)
         PZ4     =-PZ3
         qV1(1)=PUP(1,3)
         qV1(2)=PUP(2,3)
         qV1(3)=PZ3
         qV2(1)=PUP(1,4)
         qV2(2)=PUP(2,4)
         qV2(3)=PZ4
         mass_sg=DSQRT(ESGL**2-qV1(1)**2-qV1(2)**2-qV1(3)**2)
C         mass_sg(2)=DSQRT(ESGL**2-qV2(1)**2-qV2(2)**2-qV2(3)**2)
C...Longitudinal boost of outgoing partons to cm frame.
         BETA=(X1SEL-X2SEL)/(X1SEL+X2SEL)
         GAMMA=0.5D0*(X1SEL+X2SEL)/SQRT(X1SEL*X2SEL)
         PUP(3,3)=GAMMA*(PZ3+BETA*ESGL)
         PUP(4,3)=GAMMA*(ESGL+BETA*PZ3)
         PUP(3,4)=GAMMA*(PZ4+BETA*ESGL)
         PUP(4,4)=GAMMA*(ESGL+BETA*PZ4)
         mass_sg2=DSQRT(PUP(4,3)**2-qV1(1)**2-qV1(2)**2-PUP(3,3)**2)
C         mass_sg(4)=PUP(4,4)**2 - qV2(1)**2 - qV2(2)**2 - PUP(3,4)**2

C..............debug impulsions
C..         PRINT *,'nrj',PUP(4,3)
         IF (PUP(4,3)<PUP(5,3)) THEN
            PRINT *,'#####  Before Boost  #####'
            PRINT *,'m : ',PUP(5,3)
            PRINT *,'px,py,pz,e : ',qV1(1),qV1(2),qV1(3),ESGL
            PRINT *,'mass from the 4-V',mass_sg
            PRINT *,'gamma, beta : ',GAMMA,BETA 
            PRINT *,'#####  After Boost  #####'
            PRINT *,'m : ',PUP(5,3)
            PRINT *,'px,py,pz,e : ',qV1(1),qV1(2),PUP(3,3),PUP(4,3)
            PRINT *,'mass from the 4-V',mass_sg2
         ENDIF


         IF (PUP(4,4)<PUP(5,4)) THEN
            PRINT *,'#####  Before Boost  #####'
            PRINT *,'m : ',PUP(5,4)
            PRINT *,'px,py,pz,e : ',qV2(1),qV2(2),qV2(3),ESGL
C            PRINT *,'mass from the 4-V',mass_sg(2)
            PRINT *,'#####  After Boost  #####'
            PRINT *,'m : ',PUP(5,4)
            PRINT *,'px,py,pz,e : ',qV2(1),qV2(2),PUP(3,4),PUP(4,4)
C            PRINT *,'mass from the 4-V',DSQRT(mass_sg(4))
         ENDIF
C........... fin de debug impulsions
C...  Store colour flow.
C         IF(KFLQ.GT.0) THEN
C...Colour stretched from initial quark to gluon to final quark.
         IF ( IDUP(1).GT.0 ) THEN
C... connect color of quark and sigma
            ICOLUP(1,1) = 101
            ICOLUP(1,3) = 101
C... connect colour of anti-quark and anit-sigma
            ICOLUP(2,2) = 201
            ICOLUP(2,4) = 201
C... connect sigma, sigma*?
            ICOLUP(2,3) = 100
            ICOLUP(1,4) = 100
         ELSE
C... connect color of anti-quark and anti-sigma
            ICOLUP(2,1) = 101
            ICOLUP(2,3) = 101
C... connect colour of quark and sigma
            ICOLUP(1,2) = 201
            ICOLUP(1,4) = 201
C... connect sigma, sigma*?
            ICOLUP(1,3) = 100
            ICOLUP(2,4) = 100
         ENDIF
C...  Pick Q2 scales for initial state shower evolution.
         SCALUP=PYMASS(5100021)**2






C...Done.
      ELSEIF(IDPRUP.EQ.662) THEN
C...  call PYUPEV_qgqG(SIGEV)
C... select combination according to SUM, ISIDE=1: q + qbar, ISIDE=2 qbar+q (prod)
         CALL GXSECGGSS(SIGEV)
         IF ( SIGEV.EQ.0.D0 ) THEN
c            print *,'IDPRUP=', IDPRUP, ' CROSS SECTION:',SIGEV
            GOTO 4722
         ENDIF
C...Flavour codes for entries. 
         IDUP(1) = 21
         IDUP(2) = 21
         IDUP(3) = 5100021
         IDUP(4) = -5100021
C...  Masses of final state entries (initial assumed massless).
         PUP(5,4)=PMAS(PYCOMP(5100021),1)
         PUP(5,3)=PMAS(PYCOMP(5100021),1)

C...Four-momenta of the incoming partons simple.
         PUP(4,1)=0.5D0*X1SEL*DSQRT(STOTAL)
         PUP(3,1)=PUP(4,1)
         PUP(4,2)=0.5D0*X2SEL*DSQRT(STOTAL)
         PUP(3,2)=-PUP(4,2)
C...Energies and absolute momentum of the outgoing partons in 
C...the subsystem frame.
         ESGL = 0.5*DSQRT(SHAT)
         PABS = 0.5D0*DSQRT(SHAT -4.0D0*PMAS(PYCOMP(5100021),1)**2)
C...Subsystem scattering angle defined neglecting quark mass.
C... gg need to find max
         PCTHETAM = 0.
         DO ITHE=1,2000,1
            XTMP = DBLE(ITHE-1000)/1.D3
            XTMP = CROSS2(XTMP)
            IF (XTMP.GT.PCTHETAM) THEN
               PCTHETAM = XTMP
            ENDIF
         ENDDO
         PCTHETAM = PCTHETAM*1.1D0
C...
 4712    CONTINUE
         COSTHE    = PYR(0.D0)*2.D0-1.D0
         CTHETAAMP = PYR(0.D0)*PCTHETAM
C         print *,'DIRK CTHETA',PCTHETAM,COSTHE
         if ( cross2(COSTHE).LT.CTHETAAMP ) GOTO 4712
         SINTHE=SQRT(MAX(0D0,1D0-COSTHE**2))
C...Azimuthal angle at random.
         PHI=2D0*PI*PYR(0.D0)
C...Momenta of outgoing partons in the subsystem frame.
         PUP(1,3)=PABS*SINTHE*COS(PHI)
         PUP(2,3)=PABS*SINTHE*SIN(PHI)
         PZ3     =PABS*COSTHE
         PUP(1,4)=-PUP(1,3)
         PUP(2,4)=-PUP(2,3)
         PZ4     =-PZ3

C...Longitudinal boost of outgoing partons to cm frame.
         BETA=(X1SEL-X2SEL)/(X1SEL+X2SEL)
         GAMMA=0.5D0*(X1SEL+X2SEL)/SQRT(X1SEL*X2SEL)
         PUP(3,3)=GAMMA*(PZ3+BETA*ESGL)
         PUP(4,3)=GAMMA*(ESGL+BETA*PZ3)
         PUP(3,4)=GAMMA*(PZ4+BETA*ESGL)
         PUP(4,4)=GAMMA*(ESGL+BETA*PZ4)
C...  Store colour flow.
C... connect color of gluon and sigma
         ICOLUP(1,1) = 101
         ICOLUP(1,3) = 101
C... connect color of 2nd gluon and anti-sigma
         ICOLUP(2,2) = 102
         ICOLUP(2,4) = 102
C... connect colour of gluon and gluon
         ICOLUP(2,1) = 201
         ICOLUP(1,2) = 201
C... connect colour of sigma and anti-sigma
         ICOLUP(2,3) = 202
         ICOLUP(1,4) = 202
C...  Pick Q2 scales for initial state shower evolution.
      ELSE
         WRITE(*,*) 'Fatal error! Unknown process',IDPRUP 
         STOP  
      ENDIF

 4722 CONTINUE

      SCALUP = Q2
      XWGTUP = SIGEV

      RETURN
      END

