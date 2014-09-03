CDECK  ID>, HWDSM2.
*CMZ :-        -09/04/02  13:46:07  by  Peter Richardson
*-- Author :    Peter Richardson
C-----------------------------------------------------------------------
      SUBROUTINE HWDSM2(ID,IOUT1,IOUT2,IMODE,RHOIN,IDSPIN)
C-----------------------------------------------------------------------
C  Subroutine to calculate the two body matrix element for spin
C  correlations
C-----------------------------------------------------------------------
      INCLUDE 'HERWIG65.INC'
      INTEGER IOUT1,IOUT2,IMODE,IDSPIN,ID,I,J,IDP(3),P0,P1,P2,O(2),P0P,
     &     NTRY
      DOUBLE PRECISION XMASS,PLAB,PRW,PCM,PREF(5),P(5,3),PM(5,3),PCMA,
     &     HWUPCM,MA(3),MA2(3),HWULDO,PP,HWVDOT,N(3),EPS,PRE,PHS,A(2),
     &     WGT,WTMAX,HWRGEN
      DOUBLE COMPLEX RHOIN(2,2),S,D,ME(2,2,2),F1(2,2,8),F0(2,2,8),
     &     F2M(2,2,8),F1M(2,2,8),F1F(2,2,8),F2(2,2,8,8),F0B(2,2,8,8)
      COMMON/HWHEWP/XMASS(10),PLAB(5,10),PRW(5,2),PCM(5,10)
      SAVE O,PREF
      DATA PREF/1.0D0,0.0D0,0.0D0,1.0D0,0.0D0/
      DATA O/2,1/
      COMMON/HWHEWS/S(8,8,2),D(8,8)
      PARAMETER(EPS=1D-20)
      EXTERNAL HWUPCM,HWULDO,HWVDOT,HWRGEN
C--first setup if this is the start of a new spin chain
      IF(NSPN.EQ.0) THEN
C--zero the elements of the array
        CALL HWVZRI(  NMXHEP,ISNHEP)
        CALL HWVZRI(  NMXSPN,JMOSPN)
        CALL HWVZRI(2*NMXSPN,JDASPN)
        CALL HWVZRI(  NMXSPN, IDSPN)
        NSPN = NSPN+1
        JMOSPN(NSPN) = 0
        IDSPN (NSPN) = ID
        DECSPN(NSPN) = .FALSE.
        IF(RSPIN(IDHW(ID)).EQ.ZERO) THEN
          RHOSPN(1,1,NSPN) = ONE
          RHOSPN(2,1,NSPN) = ZERO
          RHOSPN(1,2,NSPN) = ZERO
          RHOSPN(2,2,NSPN) = ZERO
        ELSE
          RHOSPN(1,1,NSPN) = HALF
          RHOSPN(2,1,NSPN) = ZERO
          RHOSPN(1,2,NSPN) = ZERO
          RHOSPN(2,2,NSPN) = HALF
        ENDIF
        ISNHEP(ID)    = NSPN
      ENDIF
C--MA is mass for this decay (OFF-SHELL)
C--generate the momenta for a two body decay
      P(5,1) = PHEP(5,   ID)
      P(5,2) = PHEP(5,IOUT1)
      P(5,3) = PHEP(5,IOUT2)
      IDP(1) = IDHW(ID)
      IDP(2) = IDHW(IOUT1)
      IDP(3) = IDHW(IOUT2)
      DO 1 I=1,3
      MA(I)  = P(5,I)
 1    MA2(I) = MA(I)**2
      PCMA   = HWUPCM(P(5,1),P(5,2),P(5,3))
C--setup the couplings
      DO 2 I=1,2
 2    A(I) = A2MODE(I,IMODE)
C--phase space factor
      PHS = PCMA/MA2(1)/8.0D0/PIFAC
C--maximum weight
      WTMAX = WT2MAX(IMODE)
      NTRY = 0
 1000 NTRY = NTRY+1
      CALL HWVEQU(5,PHEP(1,ID),P(1,1))
      CALL HWDTWO(P(1,1),P(1,2),P(1,3),PCMA,2.0D0,.TRUE.)
      DO 3 I=1,3
C--compute the references vectors
C--not important if SM particle which can't have spin measured
C--ie anything other the top and tau
C--also not important if particle is approx massless
C--first the SM particles other than top and tau
      IF(IDP(I).LT.400.AND.(IDP(I).NE.6.AND.IDP(I).NE.12
     &                .AND.IDP(I).NE.125.AND.IDP(I).NE.131)) THEN
        CALL HWVEQU(5,PREF,PLAB(1,I+3))
C--all other particles
      ELSE
        PP = SQRT(HWVDOT(3,P(1,I),P(1,I)))
        CALL HWVSCA(3,ONE/PP,P(1,I),N)
        PLAB(4,I+3) = HALF*(P(4,I)-PP)
        PP = HALF*(PP-MA(I)-PP**2/(MA(I)+P(4,I)))
        CALL HWVSCA(3,PP,N,PLAB(1,I+3))
        CALL HWUMAS(PLAB(1,I+3))
        PP = HWVDOT(3,PLAB(1,I+3),PLAB(1,I+3))
C--fix to avoid problems if approx massless due to energy
        IF(PP.LT.EPS) CALL HWVEQU(5,PREF,PLAB(1,I+3))
      ENDIF
C--now the massless vectors
      PP = HALF*P(5,I)**2/HWULDO(PLAB(1,I+3),P(1,I))
      DO 4 J=1,4
 4    PLAB(J,I) = P(J,I)-PP*PLAB(J,I+3)
 3    CALL HWUMAS(PLAB(1,I))
C--change order of momenta for call to HE code
      DO 5 I=1,3
      PM(1,I) = P(3,I)
      PM(2,I) = P(1,I)
      PM(3,I) = P(2,I)
      PM(4,I) = P(4,I)
 5    PM(5,I) = P(5,I)
      DO 6 I=1,6
      PCM(1,I)=PLAB(3,I)
      PCM(2,I)=PLAB(1,I)
      PCM(3,I)=PLAB(2,I)
      PCM(4,I)=PLAB(4,I)
 6    PCM(5,I)=PLAB(5,I)
C--compute the S functions
      CALL HWHEW2(6,PCM(1,1),S(1,1,2),S(1,1,1),D)
      DO 7 I=1,6
      DO 7 J=1,6
      S(I,J,2) = -S(I,J,2)
 7    D(I,J)   = TWO*D(I,J)
C--now compute the F functions needed
      CALL HWH2F2(6,F1 ,5,PM(1,2), MA(2))
      CALL HWH2F2(6,F0 ,4,PM(1,1), MA(1))
      CALL HWH2F2(6,F1M,5,PM(1,2),-MA(2))
      CALL HWH2F2(6,F2M,6,PM(1,3),-MA(3))
      CALL HWH2F1(6,F1F,5,PM(1,2), MA(2))
      CALL HWH2F3(6,F2   ,PM(1,3),ZERO  )
      CALL HWH2F3(6,F0B  ,PM(1,1),ZERO  )
C--now compute the diagrams
C--fermion --> fermion scalar
      IF(I2DRTP(IMODE).EQ.1) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,2),PCM(1,5))
        PRE = HALF/SQRT(PRE)
        DO 10 P0=1,2
        DO 10 P1=1,2
        ME(P0,P1,2) = (0.0D0,0.0D0)
 10     ME(P0,P1,1) = PRE*( A(O(P1))*S(5,2,O(P1))*F0(  P1 ,O(P0),2)
     &                     +A(  P1 )*MA(2)*       F0(O(P1),O(P0),5))
C--fermion --> scalar fermion   diagrams
      ELSEIF(I2DRTP(IMODE).EQ.2) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,3),PCM(1,6))
        PRE = HALF/SQRT(PRE)
        DO 20 P0=1,2
        DO 20 P2=1,2
        ME(P0,2,P2) = (0.0D0,0.0D0)
 20     ME(P0,1,P2) = PRE*( A(O(P2))*S(6,3,O(P2))*F0(  P2 ,O(P0),3)
     &                     +A(  P2 )*MA(3)*       F0(O(P2),O(P0),6))
C--fermion --> scalar antifermion
      ELSEIF(I2DRTP(IMODE).EQ.3) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,3),PCM(1,6))
        PRE =(-HALF)/SQRT(PRE)
        DO 30 P0=1,2
        DO 30 P2=1,2
        ME(P0,2,P2) = (0.0D0,0.0D0)
 30     ME(P0,1,P2) = PRE*( A(  P0 )*S(4,1,P0)*F2M(O(P0),O(P2),1)
     &                     -A(O(P0))*MA(1)    *F2M(  P0 ,O(P2),4))
C--fermion --> fermion gauge boson
      ELSEIF(I2DRTP(IMODE).EQ.4) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,2),PCM(1,5))*
     &        HWULDO(PM(1,3),PCM(1,6))
        PRE = HALF/SQRT(PRE)
        DO 40 P0=1,2
        DO 40 P1=1,2
        ME(P0,P1,1) =(-PRE)*A(1)*F1F(O(P1),2,3)*S(3,6,2)*F0(1,O(P0),3)
 40     ME(P0,P1,2) = PRE*     F1F(O(P1),1,3)*S(3,6,1)*F0(2,O(P0),3)
C--scalar  --> fermion antifermion
      ELSEIF(I2DRTP(IMODE).EQ.5) THEN
        PRE = HWULDO(PM(1,2),PCM(1,5))*HWULDO(PM(1,3),PCM(1,6))
        PRE =(-HALF)/SQRT(PRE)
        DO 50 P1=1,2
        DO 50 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 50     ME(1,P1,P2) = PRE*( A(O(P1))*S(5,2,O(P1))*F2M(  P1 ,O(P2),2)
     &                     +A(  P1 )*MA(2)*       F2M(O(P1),O(P2),5))
C--scalar --> fermion fermion
      ELSEIF(I2DRTP(IMODE).EQ.6) THEN
        PRE = HWULDO(PM(1,2),PCM(1,5))*HWULDO(PM(1,3),PCM(1,6))
        PRE = HALF/SQRT(PRE)
        DO 60 P1=1,2
        DO 60 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 60     ME(1,P1,P2) = PRE*( A(O(P2))*S(6,3,O(P2))*F1M(  P2 ,P1,3)
     &                     +A(  P2 )*MA(3)*       F1M(O(P2),P1,6))
C--fermion --> fermion pion
      ELSEIF(I2DRTP(IMODE).EQ.7) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,2),PCM(1,5))
        PRE = 0.25D0/SQRT(PRE)/RMASS(198)**2
        DO 70 P0=1,2
        DO 70 P1=1,2
        ME(P0,P1,2) = (0.0D0,0.0D0)
 70     ME(P0,P1,1) =PRE*(
     &              MA(1)*A(O(P0))*( S(5,2,O(P1))*F2(  P1 ,O(P0),2,4)
     &                                     +MA(2)*F2(O(P1),O(P0),5,4))
     &            +A(P0)*S(1,4,P0)*( S(5,2,O(P1))*F2(  P1 ,  P0 ,2,1)
     &                                     +MA(2)*F2(O(P1),  P0 ,5,1)))
C--scalar  --> antifermion fermion
      ELSEIF(I2DRTP(IMODE).EQ.8) THEN
        PRE = HWULDO(PM(1,2),PCM(1,5))*HWULDO(PM(1,3),PCM(1,6))
        PRE =(-HALF)/SQRT(PRE)
        DO 80 P1=1,2
        DO 80 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 80     ME(1,P1,P2) = PRE*( A(O(P2))*S(6,3,O(P2))*F1M(  P2 ,O(P1),3)
     &                     +A(  P2 )*MA(3)*       F1M(O(P2),O(P1),6))
C--neutralino --> gravitino photon
      ELSEIF(I2DRTP(IMODE).EQ.9) THEN
        PRE = TWO*HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,3),PCM(1,6))
        PRE = TWO/SQRT(PRE)
        DO 90 P1=1,2
        DO 90 P2=1,2
        ME(P1,P2,O(P2)) = (0.0D0,0.0D0)
 90     ME(P1,P2,  P2 ) = PRE*S(2,3,P2)*S(3,6,O(P2))*
     &       S(3,2,P2)*F0(O(P2),P1,2)
C--neutralino --> gravitino scalar
      ELSEIF(I2DRTP(IMODE).EQ.10) THEN
        PRE = TWO*HWULDO(PM(1,1),PCM(1,4))
        PRE = ONE/SQRT(PRE)
        DO 100 P1=1,2
        DO 100 P2=1,2
        ME(P1,P2,2) = (0.0D0,0.0D0)
 100    ME(P1,P2,1) = PRE*F2(P2,1,2,2)*F0(1,O(P1),2)
C--sfermion --> fermion gravitino
      ELSEIF(I2DRTP(IMODE).EQ.11) THEN
        PRE = TWO*HWULDO(PM(1,2),PCM(1,5))
        PRE = ONE/SQRT(PRE)
        DO 110 P1=1,2
        DO 110 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 110    ME(1,P1,P2) = PRE*A(O(P2))*F1M(O(P1),P2,3)*F0B(P2,P2,3,3)
C--antisfermion --> antifermion gravitino
      ELSEIF(I2DRTP(IMODE).EQ.12) THEN
        PRE = TWO*HWULDO(PM(1,2),PCM(1,5))
        PRE = ONE/SQRT(PRE)
        DO 120 P1=1,2
        DO 120 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 120    ME(1,P1,P2) = PRE*A(O(P2))*F0B(P2,P2,3,3)*F1(P2,O(P1),3)
C--scalar --> antifermion antifermion
      ELSEIF(I2DRTP(IMODE).EQ.13) THEN
        PRE = HWULDO(PM(1,2),PCM(1,5))*HWULDO(PM(1,3),PCM(1,6))
        PRE = HALF/SQRT(PRE)
        DO 130 P1=1,2
        DO 130 P2=1,2
        ME(2,P1,P2) = (0.0D0,0.0D0)
 130    ME(1,P1,P2) = PRE*( A(  P1 )*S(5,2,  P1 )*F2M(O(P1),O(P2),2)
     &                     +A(O(P1))*MA(2)       *F2M(  P1 ,O(P2),5))
C--antifermion --> scalar antifermion
      ELSEIF(I2DRTP(IMODE).EQ.14) THEN
        PRE = HWULDO(PM(1,1),PCM(1,4))*HWULDO(PM(1,3),PCM(1,6))
        PRE = HALF/SQRT(PRE)
        DO 140 P0=1,2
        DO 140 P2=1,2
        ME(P0,2,P2) = (0.0D0,0.0D0)
 140    ME(P0,1,P2) = PRE*( A(O(P0))*S(4,1,O(P0))*F2M(  P0 ,O(P2),1)
     &                     -A(  P0 )*MA(1)       *F2M(O(P0),O(P2),4))
C--unrecognized type of diagram
      ELSE
        CALL HWWARN('HWDSM2',500)
      ENDIF
C--compute the maximum weight
      WTMAX=0.0d0
      DO 490 P0=1,2
      DO 490 P1=1,2
      DO 490 P2=1,2
 490  WTMAX = WTMAX+DREAL(ME(P0,P1,P2)*DCONJG(ME(P0,P1,P2)))
      WTMAX = WTMAX*PHS*P2MODE(IMODE)
C--now compute the weight
      WGT = ZERO
      DO 500 P0 =1,2
      DO 500 P0P=1,2
      DO 500 P1 =1,2
      DO 500 P2 =1,2
 500  WGT = WGT+PHS*P2MODE(IMODE)*DREAL(
     &       ME(P0,P1,P2)*DCONJG(ME(P0P,P1,P2))*RHOIN(P0,P0P))
      IF(I2DRTP(IMODE).EQ.5.OR.I2DRTP(IMODE).EQ.6.OR.
     &   I2DRTP(IMODE).EQ.8.OR.I2DRTP(IMODE).EQ.13) GOTO 300
C--issue warning if greater than maximum
      IF(WGT.GT.WTMAX) THEN
        CALL HWWARN('HWDSM2',1)
        WRITE(6,2000) RNAME(IDK(ID2PRT(IMODE))),
     &   RNAME(IDKPRD(1,ID2PRT(IMODE))),RNAME(IDKPRD(2,ID2PRT(IMODE))),
     &   WTMAX,1.1D0*WGT
        WT2MAX(IMODE) = 1.1D0*WGT
        WTMAX         = WT2MAX(IMODE)
      ENDIF
      IF(HWRGEN(0)*WTMAX.GT.WGT.AND.NTRY.LT.NSNTRY) GOTO 1000
      IF(NTRY.GE.NSNTRY) THEN
        CALL HWWARN('HWDSM2',100)
        GOTO 999
      ENDIF
C--now enter the momenta in the common block
 300  CALL HWVEQU(5,P(1,2),PHEP(1,IOUT1))
      CALL HWVEQU(5,P(1,3),PHEP(1,IOUT2))
C--set up the spin information
C--setup for all decays
      JMOSPN(NSPN+1) = IDSPIN
      JMOSPN(NSPN+2) = IDSPIN
      JDASPN(1,IDSPIN) = NSPN+1
      JDASPN(2,IDSPIN) = NSPN+2
      IDSPN(NSPN+1) = IOUT1
      IDSPN(NSPN+2) = IOUT2
      DO 11 I=1,2
      DECSPN(NSPN+I) = .FALSE.
      DO 11 J=1,2
 11   JDASPN(I,NSPN+J) = 0
      ISNHEP(IOUT1) = NSPN+1
      ISNHEP(IOUT2) = NSPN+2
      DO 12 I=1,2
        IF(RSPIN(IDHW(IDSPN(NSPN+I))).EQ.ZERO) THEN
          RHOSPN(1,1,NSPN+I) = ONE
          RHOSPN(2,1,NSPN+I) = ZERO
          RHOSPN(1,2,NSPN+I) = ZERO
          RHOSPN(2,2,NSPN+I) = ZERO
        ELSE
          RHOSPN(1,1,NSPN+I) = HALF
          RHOSPN(2,1,NSPN+I) = ZERO
          RHOSPN(1,2,NSPN+I) = ZERO
          RHOSPN(2,2,NSPN+I) = HALF
        ENDIF
 12   CONTINUE
      NSPN = NSPN+2
C--now enter the matrix element
      DO 150 P0=1,2
      DO 150 P1=1,2
      DO 150 P2=1,2
      MESPN(P0,P1,P2,2,1,IDSPIN) = (0.0D0,0.0D0)
 150  MESPN(P0,P1,P2,1,1,IDSPIN) = ME(P0,P1,P2)
      SPNCFC(1,1,IDSPIN) = ONE
      NCFL(IDSPIN) = 1
      RETURN
C--format statements
 2000 FORMAT(/'WEIGHT FOR DECAY ',A8,' --> ',A8,' ',A8, 'EXCEEDS MAX',
     &       /10X,'    MAXIMUM WEIGHT =',1PG24.16,
     &       /10X,'NEW MAXIMUM WEIGHT =',1PG24.16)
 999  RETURN
      END
