!
!> Tracks the 8-vector VIN in mag. field (+m.s.) up to
!> the 8-vector VOUT, the performed step being PAPAS
!>  ISENS = 1  ===>  Away from the vertex  ===>  dE/dx < 0
!>  ISENS = 2  ===>  Toward the vertex     ===>  dE/dx > 0
!> \author M.Virchaux
!
 SUBROUTINE VINVOU(VIN,VOUT,PAPAS,NVIN,ISENS)
 USE M_MB_PRECIS
 USE M_MB_General_CONSTANTS, ONLY : CP1000
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: NVIN, ISENS
 REAL(8), INTENT(IN)  :: VIN(8,NVIN)
 REAL(8), INTENT(OUT) :: VOUT(8,NVIN), PAPAS
 REAL(8), PARAMETER :: BIG = 9999999.D0
 REAL(8), SAVE      :: R2ON = 0.D0
 INTEGER :: IMS0, IMS, I, K
 REAL(8) :: AZON, R3IN, R3OUT, PAPA0, REDUELOS, SISENS, PPPP, ENERGY
 REAL(8) :: TH1, COSTH1, SINTH1, TH2, COSTH2, SINTH2, VERIF
 REAL(8) :: VMSU(3),VMSW(3)
 REAL(8) :: InvDifR3OUTR3IN
!
   AZON = ABS( VIN(3,1)+VIN(6,1) )
   IF( AZON < AZBMIN ) THEN
     R2ON = (VIN(1,1)+VIN(4,1))**2 + (VIN(2,1)+VIN(5,1))**2
     IF( AZON < ZCYLIN(3) .AND. R2ON < RCYLI2(3) ) THEN
       PAPAS = STEP(2)
     ELSE
       PAPAS = STEP(3)
     ENDIF
   ELSE
     PAPAS = STEP(4)
   ENDIF
   CALL ELEMHC(VIN,NVIN,PAPAS, VOUT)
!
!>> Possible multiple scattering and energy loss
   IF( NBMASC >= 1 ) THEN
     R3IN  = getRMUSC(  VIN(1,1),  VIN(2,1),  VIN(3,1) )
     R3OUT = getRMUSC( VOUT(1,1), VOUT(2,1), VOUT(3,1) )
     InvDifR3OUTR3IN = 1.d0 / (R3OUT-R3IN)
     IMS0 = 0
     PAPA0 = BIG
     DO IMS=1,NBMASC
       IF( IDMASC(IMS) /= 0 )                              CYCLE ! already crossed 
       IF( (RRMASC(IMS)-R3IN)*(RRMASC(IMS)-R3OUT) > 0.D0 ) CYCLE
       PPPP = PAPAS * (RRMASC(IMS)-R3IN)* InvDifR3OUTR3IN
       IF( PPPP > PAPA0 )                                  CYCLE
       IMS0 = IMS
       PAPA0 = PPPP
     ENDDO
     IF( IMS0 >= 1 ) THEN
       PAPAS = PAPA0 + CP1000
       CALL ELEMHC(VIN,NVIN,PAPAS, VOUT)
       R3OUT = getRMUSC( VOUT(1,1), VOUT(2,1), VOUT(3,1) )
       IF( (RRMASC(IMS0)-R3IN)*(RRMASC(IMS0)-R3OUT) > 0.D0 )  RETURN
       IDMASC(IMS0) = 1
       IF( ISENS == 1 ) THEN
         VinVouMASC(:,IMS0) =VOUT(:,1)
       ELSE
         VinVou2MASC(:,IMS0)=VOUT(:,1)
       ENDIF
       DO I=1,NVIN
         ENERGY = VOUT(7,I)
!>> Possibly, apply energy loss
         IF( MOD(LLMASC(IMS0),2) == 1 ) THEN
           IF( SignEloss == 1 ) THEN
!>> Here, one looses energy  ( ALMASC<1 , BLMASC<0 )
             VOUT(7,I) = ENERGY*ALMASC(IMS0)+BLMASC(IMS0)
           ELSE
!>> Here, one gains energy  ( ALMASC<1 , BLMASC<0 )
             VOUT(7,I) = (ENERGY -BLMASC(IMS0) )/ALMASC(IMS0)
           ENDIF
           IF( AZON > ZCYLIN(3) .OR. R2ON > RCYLI2(3) ) THEN
             REDUELOS = MIN( 1.D0, ENERGY*0.1d0 )
           ELSE
             REDUELOS = ENMASC
           ENDIF
           VOUT(7,I) = ENERGY + REDUELOS*(VOUT(7,I)-ENERGY)
           IF( SignEloss == 2 .AND. ( AZON > ZCYLIN(3) .OR. R2ON > RCYLI2(3) ) ) THEN
! be sure that the Eloss is the same independantly of direction (when P < 10)
             DO K=1,4
               VERIF = VOUT(7,I)*ALMASC(IMS0)+BLMASC(IMS0)
               REDUELOS = MIN( 1.D0, VOUT(7,I)*0.1d0 )
               VERIF = VOUT(7,I) + REDUELOS*(VERIF-VOUT(7,I))
               VOUT(7,I) = VOUT(7,I) - VERIF + ENERGY
               IF( ABS(VERIF-ENERGY)/ENERGY < 0.0001d0 ) EXIT
             ENDDO
           ENDIF
           ENERGY = (ENERGY+VOUT(7,I)) *0.5d0
         ENDIF
!>> Possibly, apply scattering in (toroid) bending plane
         IF( LLMASC(IMS0) >= 4 ) THEN
           IF( ISENS == 1 ) THEN
             SISENS =  1.D0
           ELSE
             SISENS = -1.D0
           ENDIF
           VMSU(1) =   VOUT(1,I)*VOUT(6,I)
           VMSU(2) =   VOUT(2,I)*VOUT(6,I)
           VMSU(3) = - VOUT(1,I)*VOUT(4,I) - VOUT(2,I)*VOUT(5,I)
           CALL NORM3V(VMSU)
           THMASC(IMS0) = 0.0136D0 * SQRT(X0MASC(IMS0)) * (1.D0 + 0.038D0*LOG(X0MASC(IMS0))) / ENERGY
           IF( inderiv(I) == 5+IMS0 ) THEN
             TH1 = SISENS*(S1MASC(IMS0)+DS1MASC(IMS0))*THMASC(IMS0)
           ELSE IF( inderiv(I) == -5-IMS0 ) THEN
             TH1 = SISENS*(S1MASC(IMS0)-DS1MASC(IMS0))*THMASC(IMS0)
           ELSE
             TH1 = SISENS* S1MASC(IMS0)               *THMASC(IMS0)
           ENDIF
           COSTH1 = COS(TH1)
           SINTH1 = SIN(TH1)
           VOUT(4,I) = COSTH1*VOUT(4,I) + SINTH1*VMSU(1)
           VOUT(5,I) = COSTH1*VOUT(5,I) + SINTH1*VMSU(2)
           VOUT(6,I) = COSTH1*VOUT(6,I) + SINTH1*VMSU(3)
!>> Possibly, apply scattering in non bending plane
           IF( LLMASC(IMS0) >= 6 ) THEN
             CALL PVEC3V(VMSU,VOUT(4,I), VMSW)
             CALL NORM3V(VMSW)
             TH2 = SISENS * S2MASC(IMS0) * THMASC(IMS0)
             COSTH2 = COS(TH2)
             SINTH2 = SIN(TH2)
             VOUT(4,I) = COSTH2*VOUT(4,I) + SINTH2*VMSW(1)
             VOUT(5,I) = COSTH2*VOUT(5,I) + SINTH2*VMSW(2)
             VOUT(6,I) = COSTH2*VOUT(6,I) + SINTH2*VMSW(3)
           ENDIF
         ENDIF
       ENDDO
       IF( ISENS == 1 ) THEN
         VinVou2MASC(:,IMS0)=VOUT(:,1)
       ELSE
          VinVouMASC(:,IMS0) =VOUT(:,1)
       ENDIF
!>>-----------------------------------------------------------------------
     ENDIF
   ENDIF
!
 END SUBROUTINE VINVOU
!
 SUBROUTINE setSignEloss(Isens, Vdir)
 USE M_MB_Control
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Isens
 REAL(8), INTENT(IN) :: Vdir(3)
!> gives the sign of Eloss correction : 1 means Eloss>0, i.e. momentum decreases
!!                                      2 means Eloss<0, i.e. momentum increases
   SELECT CASE( AtCosm )
     CASE( 0 )        ! collisions ISENS=1=(away from vertex), ISENS=2=(towards vertex)
       SignEloss = ISENS
     CASE( 1 )        ! cosmics
       SignEloss = 1
       IF( Vdir(2) > 0.d0 )  SignEloss = 2
     CASE( 2 )        ! beam halo with beam from z- to z+
       SignEloss = 1
       IF( Vdir(3) < 0.d0 )  SignEloss = 2
     CASE( 3 )        ! beam halo with beam from z+ to z-
       SignEloss = 1
       IF( Vdir(3) > 0.d0 )  SignEloss = 2
     CASE DEFAULT     ! put collision mode for safety, but should never go here
       SignEloss = ISENS
     END SELECT
!
 END SUBROUTINE setSignEloss
!
