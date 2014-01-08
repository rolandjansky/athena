!
!> \author J.F.Laporte
!
 SUBROUTINE TRABAZ(NVIN,NSENS,IOPT)
 USE M_MB_Control
 USE M_MB_General_CONSTANTS, ONLY : CP0001
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 use M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NVIN, NSENS, IOPT
 INTEGER :: ISENS1, ISENS2, IFLIP, ISENS, IPAS
 INTEGER :: IZTPLA, I
 REAL(8) :: DFREE, PAPAS
 REAL(8) :: R2OUT, AZOUT, UFREE, PAPABO, CF0, SF0, CA0, SA0
 REAL(8) :: TZIN, TZOUT, UCI, UCO, UOI, PAPABI, TZOUTBI
 REAL(8) :: ADJU, R2IN, AZIN, A
!REAL(8) :: VIN(8,NVIN), VOUT(8,NVIN), VOUTBI(8,NVIN)
 REAL(8), SAVE :: VIN(8,KVDEB), VOUT(8,KVDEB), VOUTBI(8,KVDEB)
 REAL(8), PARAMETER :: EPS = 0.0010D0, BIG = 9999999.D0
 REAL(8) :: D0SCAT, Z0SCAT, SINT, COST, VNOR
 REAL(8), PARAMETER :: RREF=300.D0, ZREF=450.D0, ANGSCAT=0.15D0
 INTEGER, SAVE :: IWARNI = 0
 LOGICAL :: perigeeInsideSpectro
!>>
!>> Input (but NVIN and NSENS) and output in common CTRABAZ
!>> Input : NVIN   = number of tracks to be simultaneously tracked (index i)
!>>         NSENS  = 1 or 2 according to tracking in 1 or 2 direction(s)
!>>         IOPT   = 0 standard way of running TRABAZ
!>>                = 1 stop TRABAZ away from the vertex when all planes crossed
!>>                = 2 stop TRABAZ when all planes crossed
!>>         NZTPLA = number of ZT planes (index j)
!>>         ICCPLA(j) = -1 for barrel MDT plane
!>>                   =  1 for end-cap MDT plane
!>>                   =  0 for end-cap tilted CSC plane
!>>         CF0PLA(j) = cos phi of plane sector
!>>         SF0PLA(j) = sin phi of plane sector
!>>         CA0PLA(j) = cos alfa of plane sector
!>>         SA0PLA(j) = sin alfa of plane sector
!>>         TT0PLA(j) = z/t of plane
!>> Output: ICRPLA(j) = 1 when crossing, unchanged (i.e. 0) otherwise
!>>         ZTTPLA(j,i) = ZZTT at crossing point
!>>         SSSPLA(j,i) = SSSS at crossing point
!>>         VTZPLA(j,i) = VVTZ at crossing point
!>>         VZTPLA(j,i) = VVZT at crossing point
!>>         VSSPLA(j,i) = VVSS at crossing point
!>>
!
!>> Is the starting direction pointing toward the vertex (IFLIP=1)
!>> or away from the vertex (IFLIP=2) or not to be touched (IFLIP=0)
   ISENS1 = 1
   ISENS2 = 2
   IF( NSENS == 1 ) THEN
     IFLIP = 0
     IF( DOT_PRODUCT(VDEB(1:2,1),VDEB(4:5,1)) >= 0.D0 ) THEN
       ISENS2 = 1
     ELSE
       ISENS1 = 2
     ENDIF
   ELSEIF( NSENS == 2 ) THEN
     IF( DOT_PRODUCT(VDEB(1:2,1),VDEB(4:5,1)) >= 0.D0 ) THEN
       IFLIP = 2
     ELSE
       IFLIP = 1
     ENDIF
   ELSEIF( NSENS == 3 ) THEN  ! used by trafax_p for matcov, so only away from the vertex
     IFLIP = 0
     ISENS2 = 1
   ELSE
     PRINT 7770,NSENS
7770 FORMAT(/' Routine TRABAZ was called with NSENS =',I6,'  which is meaningless  =====>  STOP !')
     STOP
   ENDIF
!
   IF( NZTPLA >= MAXPLA-1 ) THEN
     IF( IWARNI <= 10 ) THEN
       IWARNI = IWARNI + 1
       WRITE(*,'(/," WARNING : In routine Muonboy:TRABAZ, parameter MAXPLA is not big enough !!!")')
     ENDIF
   ENDIF
   ICRPLA(1:NZTPLA) = 0  !< reset list of crossed planes
!
   PFIMIN =  BIG
   PFIMAX = -BIG
   IDMASC(1:NBMASC) = 0
   perigeeInsideSpectro = .false.
!
!>> Loop on the 2 (or 1, according to NSENS) direction(s) to perform tracking
!>>  ISENS = 1  ===>  Away from the vertex  ===>  dE/dx < 0
!>>  ISENS = 2  ===>  Toward the vertex     ===>  dE/dx > 0
!
   DO 30 ISENS=ISENS1,ISENS2
     VIN(1:8,1:NVIN) = VDEB(1:8,1:NVIN)
     IF( ISENS == IFLIP ) THEN
       VIN(4:6,1:NVIN) = - VIN(4:6,1:NVIN)
       VIN( 8 ,1:NVIN) = - VIN( 8 ,1:NVIN)
     ENDIF
     R2IN = SQRT( VIN(1,1)**2 + VIN(2,1)**2 )
!
!>> Perform traking in one direction
     DFREE = 0.D0
     CALL setSignEloss(ISENS,VIN(4:6,1))
     DO 20 IPAS=1,2000
       IF( InsideCylinder ) THEN  ! Necessary because cylinder of TrackRecord smaller than Muonboy Cylinder 
         IF( ISENS == 1 ) THEN
           IF( ( R2IN > RCYLIN(3) .OR. ABS(VIN(3,1)) > ZCYLIN(3) ) ) THEN
             VDEB(1:8,1:NVIN) = VIN(1:8,1:NVIN)
             IF( ISENS == IFLIP ) THEN
               VDEB(4:6,1:NVIN) = - VDEB(4:6,1:NVIN)
               VDEB( 8 ,1:NVIN) = - VDEB( 8 ,1:NVIN)
             ENDIF
             RCHAMI = RCYLIN(3)
             ZCHAMI = ZCYLIN(3)
             InsideCylinder = .false.
           ENDIF
         ENDIF
       ENDIF
!
       CALL VINVOU(VIN,VOUT,PAPAS,NVIN,ISENS)
       R2OUT = SQRT( VOUT(1,1)**2 + VOUT(2,1)**2 )
       AZOUT = ABS( VOUT(3,1) )
!
       DFREE = DFREE - PAPAS
       IF( DFREE > 0.D0 ) GO TO 15
!
       UFREE  =  BIG
       PAPABO = -BIG
!
       DO IZTPLA=1,NZTPLA
         CF0 = CF0PLA(IZTPLA)
         SF0 = SF0PLA(IZTPLA)
         CA0 = CA0PLA(IZTPLA)
         SA0 = SA0PLA(IZTPLA)
         IF( ICCPLA(IZTPLA) <= -1 ) THEN
           TZIN  = CF0*VIN(1,1)  + SF0*VIN(2,1)
           TZOUT = CF0*VOUT(1,1) + SF0*VOUT(2,1)
         ELSEIF( ICCPLA(IZTPLA) >= 1 ) THEN
           TZIN  = VIN(3,1)
           TZOUT = VOUT(3,1)
         ELSE
           TZIN  = CA0 * VIN(3,1)  + SA0 * (CF0*VIN(1,1)  + SF0*VIN(2,1) )
           TZOUT = CA0 * VOUT(3,1) + SA0 * (CF0*VOUT(1,1) + SF0*VOUT(2,1))
         ENDIF
         UCI = TT0PLA(IZTPLA) - TZIN
         UCO = TT0PLA(IZTPLA) - TZOUT
         UOI = TZOUT          - TZIN
         IF( UOI*UCO >= 0.D0 ) UFREE = MIN( UFREE , ABS(UCO) )
         IF( UCI*UCO >= 0.D0 ) CYCLE
         IF( ICRPLA(IZTPLA) == 1 ) THEN
           IF( ISENS == 1 ) CYCLE  ! If plane already crossed, skip it when away from vertex
         ENDIF
         PAPABI = PAPAS * UCI / UOI
         IF( ABS(PAPABI-PAPABO) > EPS ) THEN
           CALL ELEMHC(VIN,NVIN,PAPABI, VOUTBI)
           PAPABO = PAPABI
         ENDIF
!>> Loop on the NVIN tracks that are tracked simultaneously ...............
         DO I=1,NVIN
           SSSPLA(IZTPLA,I) = -SF0*VOUTBI(1,I) + CF0*VOUTBI(2,I)
           VSSPLA(IZTPLA,I) = -SF0*VOUTBI(4,I) + CF0*VOUTBI(5,I)
           IF( ICCPLA(IZTPLA) <= -1 ) THEN
             ZTTPLA(IZTPLA,I) = VOUTBI(3,I)
             VTZPLA(IZTPLA,I) = CF0*VOUTBI(4,I) + SF0*VOUTBI(5,I)
             VZTPLA(IZTPLA,I) = VOUTBI(6,I)
             TZOUTBI          = CF0*VOUTBI(1,I) + SF0*VOUTBI(2,I)
           ELSEIF( ICCPLA(IZTPLA) >= 1 ) THEN
             ZTTPLA(IZTPLA,I) = CF0*VOUTBI(1,I) + SF0*VOUTBI(2,I)
             VTZPLA(IZTPLA,I) = VOUTBI(6,I)
             VZTPLA(IZTPLA,I) = CF0*VOUTBI(4,I) + SF0*VOUTBI(5,I)
             TZOUTBI          = VOUTBI(3,I)
           ELSE
             ZTTPLA(IZTPLA,I) = - SA0 * VOUTBI(3,I) + CA0 * (CF0*VOUTBI(1,I) + SF0*VOUTBI(2,I))
             VTZPLA(IZTPLA,I) =   CA0 * VOUTBI(6,I) + SA0 * (CF0*VOUTBI(4,I) + SF0*VOUTBI(5,I))
             VZTPLA(IZTPLA,I) = - SA0 * VOUTBI(6,I) + CA0 * (CF0*VOUTBI(4,I) + SF0*VOUTBI(5,I))
             TZOUTBI          =   CA0 * VOUTBI(3,I) + SA0 * (CF0*VOUTBI(1,I) + SF0*VOUTBI(2,I))
           ENDIF
           ADJU = (TT0PLA(IZTPLA)-TZOUTBI) / VTZPLA(IZTPLA,I)
           ZTTPLA(IZTPLA,I) = ZTTPLA(IZTPLA,I) + ADJU*VZTPLA(IZTPLA,I)
           SSSPLA(IZTPLA,I) = SSSPLA(IZTPLA,I) + ADJU*VSSPLA(IZTPLA,I)
         ENDDO
!>> .......................................................................
         ICRPLA(IZTPLA) = 1
       ENDDO
!
       IF( UFREE >= BIG ) THEN
         IF( ISENS == 1 ) THEN
           IF( IOPT >= 1 ) GO TO 25
         ELSE IF( ISENS == 2 ) THEN
           IF( IOPT == 2 ) RETURN
         ENDIF
       ENDIF 
       DFREE = UFREE
!
15     CONTINUE
       IF( R2OUT > RCHAMA ) GO TO 25
       IF( AZOUT > ZCHAMA ) GO TO 25
       IF( R2OUT < RCHAMI ) THEN
         IF( AZOUT < ZCHAMI ) GO TO 25
       ENDIF
       IF( ISENS == 2 ) THEN
         IF( DOT_PRODUCT(VOUT(1:2,1),VOUT(4:5,1)) > 0.d0 ) THEN
           IF( DOT_PRODUCT(VIN(1:2,1),VIN(4:5,1)) <= 0.d0 ) THEN
             VOUT(1:8,1:NVIN) = VIN(1:8,1:NVIN)
             perigeeInsideSpectro = .true.
             GO TO 25
           ENDIF
         ENDIF
       ENDIF
       VIN(1:8,1:NVIN) = VOUT(1:8,1:NVIN)
       R2IN = R2OUT
20   ENDDO
     GO TO 40
!
25   PFIMIN = MIN( PFIMIN , VOUT(7,1) )
     PFIMAX = MAX( PFIMAX , VOUT(7,1) )
!
30 ENDDO
!
   IF( NSENS == 3 ) THEN
     VOUT(1:8,1:NVIN) = VDEB(1:8,1:NVIN)
   ELSE IF( .NOT.perigeeInsideSpectro ) THEN
     IPAS = 3
     R2IN = SQRT( VIN(1,1)**2 + VIN(2,1)**2 )
     AZIN = ABS( VIN(3,1) )
     PAPABI = PAPAS
     IF( R2OUT > RCHAMA ) THEN
       PAPABI = PAPAS * (RCHAMA-R2IN) / (R2OUT-R2IN)
     ELSEIF( AZOUT > ZCHAMA ) THEN
       PAPABI = PAPAS * (ZCHAMA-AZIN) / (AZOUT-AZIN)
     ELSEIF( R2OUT < RCHAMI .AND. AZOUT < ZCHAMI ) THEN
       IPAS = 1
       IF( R2IN >= RCHAMI .AND. AZIN < ZCHAMI ) THEN
         PAPABI = PAPAS * (RCHAMI-R2IN) / (R2OUT-R2IN)
       ELSEIF( AZIN >= ZCHAMI .AND. R2IN < RCHAMI) THEN
         PAPABI = PAPAS * (ZCHAMI-AZIN) / (AZOUT-AZIN)
       ELSE
         PAPABI = MAX( PAPAS * (RCHAMI-R2IN) / (R2OUT-R2IN) ,   &
                       PAPAS * (ZCHAMI-AZIN) / (AZOUT-AZIN) )
       ENDIF
     ENDIF
     IF( PAPABI > CP0001 ) THEN
       CALL ELEMHC(VIN,NVIN,PAPABI, VOUT)
       IF( IPAS <= 1 ) THEN
         IPAS  = IPAS + 1
         R2OUT = SQRT( VOUT(1,1)**2 + VOUT(2,1)**2 )
         AZOUT = ABS( VOUT(3,1) )
         IF( ABS(R2OUT-RCHAMI) > 0.01d0 .AND. ABS(AZOUT-ZCHAMI) > 0.01d0 ) THEN
           PAPAS = PAPABI
           IF( R2IN >= RCHAMI .AND. AZIN < ZCHAMI ) THEN
             PAPABI = PAPAS * (RCHAMI-R2IN) / (R2OUT-R2IN)
           ELSEIF( AZIN >= ZCHAMI .AND. R2IN < RCHAMI) THEN
             PAPABI = PAPAS * (ZCHAMI-AZIN) / (AZOUT-AZIN)
           ELSE
             PAPABI = MAX( PAPAS * (RCHAMI-R2IN) / (R2OUT-R2IN) ,   &
                           PAPAS * (ZCHAMI-AZIN) / (AZOUT-AZIN) )
           ENDIF
           IF( PAPABI > CP0001 ) CALL ELEMHC(VIN,NVIN,PAPABI, VOUT)
         ENDIF
       ENDIF
     ELSE
       VOUT(1:8,1:NVIN) = VIN(1:8,1:NVIN)
     ENDIF
   ENDIF
!
40 DO I=1,NVIN
     A = - ( VOUT(1,I)*VOUT(4,I) + VOUT(2,I)*VOUT(5,I) ) / ( VOUT(4,I)**2  + VOUT(5,I)**2 )
     ZEXTRA(I) =         VOUT(3,I) + A*VOUT(6,I)
     REXTRA(I) = SQRT( ( VOUT(1,I) + A*VOUT(4,I) )**2 + ( VOUT(2,I) + A*VOUT(5,I) )**2 )
     IF( ItypFit <= 2 ) THEN
       DREXTR(I) = DRVERT(ItypFit)
       DZEXTR(I) = DZVERT(ItypFit)
     ELSE
       VNOR = SQRT(VOUT(4,I)**2+VOUT(5,I)**2+VOUT(6,I)**2)
       SINT = SQRT(VOUT(4,I)**2+VOUT(5,I)**2) / VNOR
       COST =               ABS(VOUT(6,I))    / VNOR
       IF( COST/SINT < ZREF/RREF ) THEN  ! barrel
         D0SCAT = ANGSCAT * RREF / SINT
       ELSE                              ! endcap
         D0SCAT = ANGSCAT * ZREF / COST
       ENDIF
       Z0SCAT = D0SCAT / SINT
       DREXTR(I) = SQRT( DRVERT(ITYPFIT)**2 + (D0SCAT/VOUT(7,I))**2 )
       DZEXTR(I) = SQRT( DZVERT(ITYPFIT)**2 + (Z0SCAT/VOUT(7,I))**2 )
     ENDIF
   ENDDO
   VFIN(1:8) = VOUT(1:8,1)
   IF( ISENS2 == 2 ) THEN
     VFIN(4) = - VFIN(4)
     VFIN(5) = - VFIN(5)
     VFIN(6) = - VFIN(6)
     VFIN(8) = - VFIN(8)
   ENDIF
!
 END SUBROUTINE TRABAZ
!
