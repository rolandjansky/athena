!
!> try to improve the starting estimate of momentum
!> \author M.Virchaux
!
 SUBROUTINE IMPRO_ESTIM(PESTIM,SESTIM)
 USE M_MB_Control
 IMPLICIT NONE
 REAL(8) :: PESTIM, SESTIM
 INTEGER :: L_MIN, L
 REAL(8) :: CHIPEST_MIN, XL, SMUPEST, CHIPEST
 REAL(8) :: CHIP(-1:1)
!
   L_MIN       = 99
   CHIPEST_MIN = 999999.d0
!
   DO L=-1,1
     XL = L
     SMUPEST = SESTIM + XL
     CALL TRAPEST(SMUPEST, CHIP(L))
     IF( MBPri >= 3 ) WRITE(*,11) XL,SMUPEST,100.d0/(ABS(SMUPEST)+0.000001d0),CHIP(L)
     IF( CHIP(L) < CHIPEST_MIN ) THEN
       L_MIN       = L
       CHIPEST_MIN = CHIP(L)
     ENDIF
   ENDDO
11 FORMAT(' L,Sestim,Pestim,Chi2 =',F5.1,F12.3,F12.2,F18.2)
!
   IF( L_MIN >= 99 )     RETURN
!
   IF( CHIP(0) >= CHIP(-1) .AND. CHIP(0) >= CHIP(1) ) THEN
     CHIPEST = 999999.d0
   ELSE
     XL = ( CHIP(-1) - CHIP(1) ) / ( 2.D0*CHIP(1) + 2.D0*CHIP(-1) - 4.D0*CHIP(0) )
     IF( ABS(XL) > 12.d0 ) RETURN
     SMUPEST = SESTIM + XL
     CALL TRAPEST(SMUPEST, CHIPEST)
     IF( MBPri >= 3 ) WRITE(*,11) XL,SMUPEST,100.d0/(ABS(SMUPEST)+0.000001d0),CHIPEST
   ENDIF
!
   IF( CHIPEST > CHIPEST_MIN ) THEN
     XL = L_MIN
     SMUPEST = SESTIM + XL
     CHIPEST = CHIP(L_MIN)
   ENDIF
!
   IF( CHIPEST > 9999.d0 ) RETURN
!
   SESTIM = SMUPEST
   PESTIM = 100.d0 / ( ABS(SESTIM) + 0.000001d0 )
!
 END SUBROUTINE IMPRO_ESTIM
!
!
!> \author M.Virchaux
!
 SUBROUTINE TRAPEST(SMUPEST, CHIPEST)
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: SMUPEST
 REAL(8), INTENT(OUT) :: CHIPEST
 REAL(8) :: XM, YM, ZM, SM, TM, RM
 REAL(8) :: X1P, Y1P, Z1P, R1P, S1P
 REAL(8) :: X2P, Y2P, Z2P, R2P, S2P
 REAL(8) :: ZVM, RVM, ZVV, RVV, ZVO, RVO
 REAL(8) :: ZCUR, RCUR, P_M_ZCHAMI, ANOR3V, VDEB123
 REAL(8), PARAMETER :: SIGPEST = 0.0100D0
!
   CHIPEST = 999999999.D0
!
!>> Define Vdeb
   XM = (X1+X2) / 2.D0
   YM = (Y1+Y2) / 2.D0
   ZM = (Z1+Z2) / 2.D0
   SM = (S1+S2) / 2.D0
   TM = (T1+T2) / 2.D0
   RM = SQRT( XM**2 + YM**2 )
   IF( ISCI <= NCOUBAS2 ) THEN
     S1P = SM * T1 / TM
     S2P = SM * T2 / TM
   ELSE
     S1P = SM * Z1 / ZM
     S2P = SM * Z2 / ZM
   ENDIF
   X1P = CFI * T1 - SFI * S1P
   Y1P = SFI * T1 + CFI * S1P
   Z1P = Z1
   R1P = SQRT( X1P**2 + Y1P**2 )
   X2P = CFI * T2 - SFI * S2P
   Y2P = SFI * T2 + CFI * S2P
   Z2P = Z2
   R2P = SQRT( X2P**2 + Y2P**2 )
   ZVM =   - ZM    / SQRT(    ZM**2     +     RM**2    )
   RVM =   - RM    / SQRT(    ZM**2     +     RM**2    )
   ZVV = (Z2P-Z1P) / SQRT( (Z2P-Z1P)**2 + (R2P-R1P)**2 )
   RVV = (R2P-R1P) / SQRT( (Z2P-Z1P)**2 + (R2P-R1P)**2 )
   ZVO = (ZVM+ZVV) / SQRT( (ZVM+ZVV)**2 + (RVM+RVV)**2 )
   RVO = (RVM+RVV) / SQRT( (ZVM+ZVV)**2 + (RVM+RVV)**2 )
   ZCUR = ZM + (RCHAMI-RM) * ZVO / RVO
   IF( ABS(ZCUR) <= ZCHAMI ) THEN
     VDEB(1,1) = RCHAMI * XM / RM
     VDEB(2,1) = RCHAMI * YM / RM
     VDEB(3,1) = ZCUR
   ELSE
     P_M_ZCHAMI = SIGN( ZCHAMI , ZM )
     RCUR = RM + (P_M_ZCHAMI-ZM) * RVO / ZVO
     IF( RCUR < 15.d0 )  RETURN
     IF( RCUR <= RCHAMI ) THEN
       VDEB(1,1) = RCUR * XM / RM
       VDEB(2,1) = RCUR * YM / RM
       VDEB(3,1) = P_M_ZCHAMI
     ELSE
!***** PRINT 7770,ZCUR,RCUR,ZCHAMI,RCHAMI
!7770   FORMAT(' in TRAPEST : Z_R_cur,Z_R_chami =',4F12.4,'  ???')
       RETURN
     ENDIF
   ENDIF
   VDEB123 = ANOR3V( VDEB(1,1) )
   VDEB(4,1) = VDEB(1,1) / VDEB123
   VDEB(5,1) = VDEB(2,1) / VDEB123
   VDEB(6,1) = VDEB(3,1) / VDEB123
   VDEB(7,1) = 100.D0 / ( ABS(SMUPEST) + 0.000001D0 )
   VDEB(8,1) = - SIGN( C1P000, SMUPEST )
!
!>> Define the 2 planes
   NZTPLA = 2
   CF0PLA(1:2) = CFI
   SF0PLA(1:2) = SFI
   CA0PLA(1:2) = 1.D0
   SA0PLA(1:2) = 0.D0
   IF( ISCI <= NCOUBAS2 ) THEN
     ICCPLA(1) = -1
     ICCPLA(2) = -1
     TT0PLA(1) = T1
     TT0PLA(2) = T2
   ELSE
     ICCPLA(1) =  1
     ICCPLA(2) =  1
     TT0PLA(1) = Z1
     TT0PLA(2) = Z2
   ENDIF
!
!>> Track and catch the crossings                     
   CALL TRABAZ(1,2,2)
!
!>> Compute Chi2
   IF( ICRPLA(1) <= 0 .OR. ICRPLA(2) <= 0 ) RETURN
   IF( ISCI <= NCOUBAS2 ) THEN
     CHIPEST = (ZTTPLA(1,1)-ZTTPLA(2,1)-Z1+Z2)**2 / SIGPEST**2
   ELSE
     CHIPEST = (ZTTPLA(1,1)-ZTTPLA(2,1)-T1+T2)**2 / SIGPEST**2
   ENDIF
!
 END SUBROUTINE TRAPEST
!
