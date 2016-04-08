      SUBROUTINE INITSGLUON
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...User process initialization commonblock.
      INTEGER MAXPUP
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/
C... PYTHIA
C...Parameters. 
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP, PARI
C... sgluon steering
      COMMON/SGLUON/qqss,ggss
      LOGICAL qqss,ggss
      
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM

C...      COMMON/PYINT3/XSFX(2,-40:40),ISIG(1000,3),SIGH(1000)
C... local stuff
      INTEGER IEVT

C...

      
      qqss=.True.
      ggss=.True.


      IDBMUP(1) = 2212
      IDBMUP(2) = 2212

C ABC: take energy from runargs
      ecm=0.D0
      OPEN(66,FILE='./sgluonecm.dat',status='old',err=111)
      READ(66,*) ecm
      CLOSE(66)
 111  continue

      if (ecm.eq.0.) then 
         EBMUP(1) = 4000.D0
         EBMUP(2) = 4000.D0
      else
         EBMUP(1) = 0.5 * ecm
         EBMUP(2) = 0.5 * ecm
      endif
      print *,'Sgluon using beam energy = ',EBMUP(1)

      STOTAL = (EBMUP(1)+EBMUP(2))**2
      
      PDFGUP(1) = 4
      PDFSUP(1) = 46
      PDFGUP(2) = PDFGUP(1)
      PDFSUP(2) = PDFSUP(1)
C... CTEQ 5L
c      MSTP(51) = 7
C... weight      
      IDWTUP = 1
C... number of external processes
      NPRUP = 2
C... set up qqb sigma sigma
      XSECUP(1)= 0.D0
      XERRUP(1)= 0.D0
      XMAXUP(1)= 0.D0
      LPRUP(1) = 661
      if ( qqss ) THEN
         DO IEVT = 1,1000,1
            CALL GXSECQQSS(SIGEV)
            IF ( XMAXUP(1) .LT. SIGEV ) THEN
               XMAXUP(1) = SIGEV
            ENDIF
         ENDDO
         XMAXUP(1) = XMAXUP(1)*1.2D0
      ENDIF
C...Set up gg -> sigma sigma
      XSECUP(2)= 0.D0
      XERRUP(2)= 0.D0
      XMAXUP(2)= 0.D0
      LPRUP(2) = 662
      if ( ggss ) THEN
         DO IEVT = 1,1000,1
            CALL GXSECGGSS(SIGEV)
            IF ( XMAXUP(2) .LT. SIGEV ) THEN
               XMAXUP(2) = SIGEV
            ENDIF
         ENDDO
         XMAXUP(2) = XMAXUP(2)*1.2D0
      ENDIF

      print *,'END UPINIT',XMAXUP(1),XMAXUP(2)
      
      
c       CALL PYLIST(12)
c      CALL EXIT(0)
       RETURN
       END




      SUBROUTINE GXSECQQSS(SIGEV)
C... process qqbar -> sigma sigma TOTAL cross section
      IMPLICIT NONE
      DOUBLE PRECISION SIGEV
C...
      DOUBLE PRECISION PYMASS,PYR,DUMMY
      INTEGER PYCOMP
      EXTERNAL PYMASS,PYR,PYCOMP
C...      
      DOUBLE PRECISION Q2, X1, X2, XPP1(-25:25), XPP2(-25:25)
      DOUBLE PRECISION XSEC
      INTEGER IQ
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM
C...
      DOUBLE PRECISION DJAC,TAU0,UNIT
      DOUBLE PRECISION CROSS1, DGAUSS
      EXTERNAL CROSS1, DGAUSS
      INTEGER KCHG
      DOUBLE PRECISION  PMAS, PARF, VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYDAT2/
C...
C... conversion from GeV-1 to pb
      DOUBLE PRECISION UGEVPB
      DATA UGEVPB/0.389379D9/

C...
      SIGEV=0.D0
C...
C... get x1 and x2      
C...Parton distributions (multiplied by x) of p and p.
C      Q2 = PYMASS(5100021)**2
      Q2 = PMAS(PYCOMP(5100021),1)**2
C...
      X1SEL = PYR(DUMMY)
      X2SEL = PYR(DUMMY)
      IF (X1SEL*X2SEL*STOTAL.LT.(4.*Q2)) THEN
         SIGEV = 0.D0
         RETURN
      ENDIF
      CALL PYPDFU(2212,X1SEL,Q2,XPP1)
      CALL PYPDFU(2212,X2SEL,Q2,XPP2)
C...Loop over quark flavours (up to b).
      SUM=0D0
      DO IQ=1,5,1
C...q from p, qbar from pbar.
        TERM(2*IQ-1)=XPP1(IQ)/X1SEL*XPP2(-IQ)/X2SEL
C...qbar from p, q from pbar.
        TERM(2*IQ)=XPP1(-IQ)/X1SEL*XPP2(IQ)/X2SEL
        SUM=SUM+TERM(2*IQ-1)+TERM(2*IQ)
      ENDDO
C... integrate over cos(theta)
      SHAT = X1SEL*X2SEL*STOTAL
      xsec = DGAUSS(cross1,-1.D0,1.D0,0.01D0)


C... to be improved
C      TAU0 = 4.d0*(PYMASS(5100021))**2/STOTAL
C      DJAC = (-dlog(TAU0))*(-dlog(tau0/X1SEL))*2.d0*X1SEL*X2SEL
      SIGEV = xsec*SUM*UGEVPB
C...
      END



      SUBROUTINE GXSECGGSS(SIGEV)
C... process gg -> sigma sigma TOTAL cross section
      IMPLICIT NONE
      DOUBLE PRECISION SIGEV
C...
      DOUBLE PRECISION PYMASS,PYR,DUMMY
      INTEGER PYCOMP
      EXTERNAL PYMASS,PYR,PYCOMP
C...      
      DOUBLE PRECISION Q2, X1, X2, XPP1(-25:25), XPP2(-25:25)
      DOUBLE PRECISION XSEC
      INTEGER IQ
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM
C...
      DOUBLE PRECISION DJAC,TAU0,UNIT
      DOUBLE PRECISION CROSS2, DGAUSS
      EXTERNAL CROSS2, DGAUSS


      INTEGER KCHG
      DOUBLE PRECISION  PMAS, PARF, VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYDAT2/
C...
C... conversion from GeV-1 to pb
      DOUBLE PRECISION UGEVPB
      DATA UGEVPB/0.389379D9/
C...
      SIGEV=0.D0
C... get x1 and x2      
C...Parton distributions (multiplied by x) of p and p.
C      Q2 = PYMASS(5100021)**2
      Q2 = PMAS(PYCOMP(5100021),1)**2
C...
      X1SEL = PYR(DUMMY)
      X2SEL = PYR(DUMMY)
      IF (X1SEL*X2SEL*STOTAL.LT.(4.*Q2)) THEN
         SIGEV = 0.D0
         RETURN
      ENDIF
      CALL PYPDFU(2212,X1SEL,Q2,XPP1)
      CALL PYPDFU(2212,X2SEL,Q2,XPP2)
C... get the gluon from the protons
      SUM=XPP1(0)/X1SEL*XPP2(0)/X2SEL
C... integrate over cos(theta)
      SHAT = X1SEL*X2SEL*STOTAL
      xsec = DGAUSS(cross2,-1.D0,1.D0,0.01D0)

C... to be improved
C      TAU0 = 4.d0*(PYMASS(5100021))**2/STOTAL
C      DJAC = (-dlog(TAU0))*(-dlog(tau0/X1SEL))*2.d0*X1SEL*X2SEL
      SIGEV = xsec*SUM*UGEVPB
C...
      END





      double precision function cross1(ctp)
C... process qqbar -> sigma sigma differential cross section
      IMPLICIT NONE
      DOUBLE PRECISION PYMASS,PYALPS,ALPSSGL
      EXTERNAL PYMASS,PYALPS,ALPSSGL

      INTEGER PYCOMP
      EXTERNAL PYCOMP

      DOUBLE PRECISION S, CTP
      DOUBLE PRECISION Q2, BETA
      DOUBLE PRECISION SGLMAS,ALPHAS
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM
C...
      DOUBLE PRECISION PI
      DATA PI/3.141592653589793D0/
C... 

      INTEGER KCHG
      DOUBLE PRECISION  PMAS, PARF, VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYDAT2/

      SGLMAS = PMAS(PYCOMP(5100021),1)
      Q2     = SGLMAS**2
C      ALPHAS = PYALPS(Q2)
      ALPHAS = ALPSSGL(Q2)
c... alphas or als: 2% difference probably for RGE modification due to the dirac gluinos
c      ALPHAS = 12.*3.14 / (23.d0) / dlog(Q2/0.165d0**2)
c      ALPHAS = ALPHAS/(1.d0+ALPHAS/(6.d0*3.14)*dlog((170.d0)**2/Q2))

      beta=dsqrt(1.d0-4.d0*SGLMAS**2/shat)
      cross1 = (4.*PI*ALPHAS**2)/(9.*shat)*beta**3
      cross1 = cross1*0.75d0*(1.d0-ctp**2)

      return
      end 



      double precision function cross2(ctp)
C... process gg -> sigma sigma differential cross section
      IMPLICIT NONE
      DOUBLE PRECISION PYMASS, PYALPS, ALPSSGL
      EXTERNAL PYMASS, PYALPS, ALPSSGL


      INTEGER PYCOMP
      EXTERNAL PYCOMP

      DOUBLE PRECISION S, CTP
      INTEGER io
      DOUBLE PRECISION Q2, BETA, BETA2, ALS, CTP2, STP4, CR
      DOUBLE PRECISION SGLMAS,ALPHAS
C...
      COMMON/XSECS/STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM(20)
      DOUBLE PRECISION STOTAL,SHAT,X1SEL,X2SEL,SUM,TERM
C...
      DOUBLE PRECISION PI
      DATA PI/3.141592653589793D0/
C...

      INTEGER KCHG
      DOUBLE PRECISION  PMAS, PARF, VCKM
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      SAVE /PYDAT2/

      SGLMAS = PMAS(PYCOMP(5100021),1)
      Q2 = SGLMAS**2
C      ALPHAS = PYALPS(Q2)
      ALPHAS = ALPSSGL(Q2)
      
      beta=dsqrt(1.d0-4.d0*SGLMAS**2/shat)
      cross2 = PI*(ALPHAS**2)*beta/(6.*shat)*(9./8.)*
     1     (
     1     27.-17.*beta**2-6.*(1.d0-beta**2)*(3.d0+beta**2)
     1      /(2.d0*beta)*dlog((1.d0+beta)/(1.d0-beta))
     1     )     
      beta2 = beta**2
      ctp2 = ctp**2
      stp4 = (1.d0-ctp2)**2 
      cr = 1.5d0*((1.d0-beta2)**2+beta**4*(1.d0-ctp2)**2)/
     1     (1.d0-beta2*ctp2)**2*
     1     (3.d0+beta2*ctp2)/
     1     (
     1      27.d0-17.d0*beta2-6.d0*(1.d0-beta2)*(3.d0+beta2)/
     1      (2.d0*beta)*dlog((1.d0+beta)/(1.d0-beta))
     1      )
      cross2 = cross2*cr

      return
      end 
      DOUBLE PRECISION FUNCTION ALPSSGL(Q2)
      DOUBLE PRECISION Q2
c... alphas or als: 2% difference probably for RGE modification due to the dirac gluinos
      ALPSSGL = 12.*3.14 / (23.d0) / dlog(Q2/0.165d0**2)
      ALPSSGL = ALPSSGL/(1.d0+ALPSSGL/(6.d0*3.14)*dlog((170.d0)**2/Q2))
C...
      RETURN 
      END


c... Add copy of DGAUSS function, taken from kernlib

      DOUBLE PRECISION FUNCTION DGAUSS(F,A,B,EPS)
      DOUBLE PRECISION F,A,B,EPS
      DOUBLE PRECISION W(12),X(12),AA,BB,C1,C2,U,S8,S16,CONST
      LOGICAL MFLAG,RFLAG
      EXTERNAL F
C
C     ******************************************************************
C
C     ADAPTIVE DOUBLE PRECISION GAUSSIAN QUADRATURE.
C
C     DGAUSS IS SET EQUAL TO THE APPROXIMATE VALUE OF THE INTEGRAL OF
C     THE FUNCTION F OVER THE INTERVAL (A,B), WITH ACCURACY PARAMETER
C     EPS.
C
C     ******************************************************************
C
C
      DATA W / 0.10122 85362 90376 25915 25313 543D0,
     1         0.22238 10344 53374 47054 43559 944D0,
     2         0.31370 66458 77887 28733 79622 020D0,
     3         0.36268 37833 78361 98296 51504 493D0,
     4         0.27152 45941 17540 94851 78057 246D-1,
     5         0.62253 52393 86478 92862 84383 699D-1,
     6         0.95158 51168 24927 84809 92510 760D-1,
     7         0.12462 89712 55533 87205 24762 822D0,
     8         0.14959 59888 16576 73208 15017 305D0,
     9         0.16915 65193 95002 53818 93120 790D0,
     A         0.18260 34150 44923 58886 67636 680D0,
     B         0.18945 06104 55068 49628 53967 232D0/
C
      DATA X / 0.96028 98564 97536 23168 35608 686D0,
     1         0.79666 64774 13626 73959 15539 365D0,
     2         0.52553 24099 16328 98581 77390 492D0,
     3         0.18343 46424 95649 80493 94761 424D0,
     4         0.98940 09349 91649 93259 61541 735D0,
     5         0.94457 50230 73232 57607 79884 155D0,
     6         0.86563 12023 87831 74388 04678 977D0,
     7         0.75540 44083 55003 03389 51011 948D0,
     8         0.61787 62444 02643 74844 66717 640D0,
     9         0.45801 67776 57227 38634 24194 430D0,
     A         0.28160 35507 79258 91323 04605 015D0,
     B         0.95012 50983 76374 40185 31933 543D-1/
C
C     ******************************************************************
C
C  START.
      DGAUSS=0.0D0
      IF(B.EQ.A) RETURN
      CONST=0.005D0/(B-A)
      BB=A
C
C  COMPUTATIONAL LOOP.
    1 AA=BB
      BB=B
    2    C1=0.5D0*(BB+AA)
         C2=0.5D0*(BB-AA)
         S8=0.0D0
         DO 3 I=1,4
            U=C2*X(I)
            S8=S8+W(I)*(F(C1+U)+F(C1-U))
    3    CONTINUE
         S8=C2*S8
         S16=0.0D0
         DO 4 I=5,12
            U=C2*X(I)
            S16=S16+W(I)*(F(C1+U)+F(C1-U))
    4    CONTINUE
         S16=C2*S16
         IF( ABS(S16-S8) .LE. EPS*(1.+ABS(S16)) ) GO TO 5
         BB=C1
         IF( 1.D0+ABS(CONST*C2) .NE. 1.D0) GO TO 2
      DGAUSS=0.0D0
      CALL KERMTR('D103.1',LGFILE,MFLAG,RFLAG)
      IF(MFLAG) THEN
         IF(LGFILE.EQ.0) THEN
            WRITE(*,6)
         ELSE
            WRITE(LGFILE,6)
         ENDIF
      ENDIF
      IF(.NOT. RFLAG) CALL ABEND
      RETURN
    5 DGAUSS=DGAUSS+S16
      IF(BB.NE.B) GO TO 1
      RETURN
C
    6 FORMAT( 4X, 'FUNCTION DGAUSS ... TOO HIGH ACCURACY REQUIRED')
      END

