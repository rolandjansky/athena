!
!> \author M.Virchaux
!
 SUBROUTINE DROIKI(ZZ,TT,SS,WZ0,WT0,VST, CTOT, dcapoin, I1ML)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_COTEMH
 IMPLICIT NONE
 REAL(8),        INTENT(IN)  :: ZZ, TT, SS, WZ0, WT0, VST
 REAL(8),        INTENT(OUT) :: CTOT
 INTEGER,        INTENT(OUT) :: I1ML
 TYPE(DcaPoint), INTENT(OUT) :: dcapoin(2) 
 REAL(8) :: ZI, SI, WZ, WT, CCC1, CCC2, CCCC, TTT
 INTEGER :: NNN1, NNN2, NNNN, II, JKI0, IC, ISC, ICI, IGEOMI, INOF,INOZ
 LOGICAL :: NAKCHA, isBadlyBuilt
 REAL(8), PARAMETER :: TOLZ=-0.75D0, TOLS=-1.50D0
!
   CTOT = 999999.D0
   CALL ResetDCA( dcapoin(1) )
   CALL ResetDCA( dcapoin(2) )
   I1ML = 0
!
   IF( WT0 < 0.D0 ) THEN
     WZ = - WZ0
     WT = - WT0
   ELSE
     WZ =   WZ0
     WT =   WT0
   ENDIF
   IF( ITEMC(2)*ITEMC(5) >= 1 ) THEN
     CALL CHIHIT(2, ZZ,TT,SS,WZ,WT,VST, 10, NNN1,CCC1, dcapoin(1))
     IF( CCC1 > 900000.d0 ) RETURN
     CALL CHIHIT(5, ZZ,TT,SS,WZ,WT,VST, 10, NNN2,CCC2, dcapoin(2))
     NNNN = NNN1 + NNN2
     CTOT = (CCC1+CCC2) / (NNNN-2.D0)
   ELSE
     IF( ITEMC(2) >= 1 ) THEN
       II = 1
     ELSEIF( ITEMC(5) >= 1 ) THEN
       II = 2
     ELSE
       RETURN
     ENDIF
     JKI0 = 3*II-1
     CALL CHIHIT(JKI0,ZZ,TT,SS,WZ,WT,VST,  1, NNNN,CCCC, dcapoin(II))
     CTOT = CCCC / (NNNN-2.D0) + 0.15D0*SIGMOK**2
! add penalty for single layer segment when two should have been possible
     IF( MBSto >= 0 ) THEN   ! do not apply it for segment search in single multilayers only
       IC   = ITEMC(JKI0)
       INOF = ITEMF(JKI0)
       INOZ = ITEMZ(JKI0)
       ISC  = (IC+1) / 2
       ICI  = 4*ISC - 1 - IC
       IF( isBadlyBuilt(ICI,INOF,INOZ) ) RETURN  ! allow 1ML in badly built stations
       IGEOMI = IGEOMA(ICI,INOF,INOZ)
       IF( IGEOMI > 0 ) THEN  ! station with two multilayer (i.e. not BEE , BIS_Z1 , BIS_Z10)
         ZI = ZZ + (TCECHAV(IGEOMI)-TT)*WZ/WT
         IF( INOZ < NZM .AND. ZI > ZCECHAV(IGEOMI)+ZLOCHAV(IGEOMI)/2.D0 ) THEN
           INOZ = INOZ + 1
         ELSE IF( INOZ > 1 .AND. ZI < ZCECHAV(IGEOMI)-ZLOCHAV(IGEOMI)/2.D0 ) THEN
           INOZ = INOZ - 1
         ENDIF
         TTT = TCECHAV(IGEOMI)
         IGEOMI = IGEOMA(ICI,INOF,INOZ)
         IF( IGEOMI > 0 ) THEN
           IF( .NOT.isOffChav(IGEOMI) .AND. ABS(TTT-TCECHAV(IGEOMI)) < DTcut ) THEN ! do not apply penalty if other multilayer Off
             ZI = ZZ + (TCECHAV(IGEOMI)-TT)*WZ/WT                                   ! or if dT too big
             SI = SS + (TCECHAV(IGEOMI)-TT)*VST
             IF( .NOT.NAKCHA(ICI,INOZ,IGEOMI,ZI,SI,TCECHAV(IGEOMI),TOLZ,TOLS) ) THEN
               CTOT = CTOT + CHI1ML
               I1ML = 1
             ENDIF
           ENDIF
         ENDIF
       ENDIF
     ENDIF
   ENDIF
!
 END SUBROUTINE DROIKI
!
 SUBROUTINE DroiPena(ZZ,TT,SS,WZ,WT,VST, PENA)
 USE M_MB_Control
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: ZZ, TT, SS, WZ, WT, VST
 REAL(8), INTENT(OUT) :: PENA
 REAL(8) :: VZT
!> Add a (very small) penalty to favor pointing segments when all combinations
!> give same chi2 (i.e. only 2 hits)
!
   IF( AtCosm == 0 ) THEN 
     VZT = WZ / WT
     PENA = MAX( 0.d0 , 1.d0-(TT+ZZ*VZT+SS*VST)/SQRT(TT**2+ZZ**2+SS**2)/SQRT(1.d0+VZT**2+VST**2) )
     PENA = 0.0001d0 * SQRT(PENA)
   ELSE
     PENA = 0.d0
   ENDIF
!
 END SUBROUTINE DroiPena
!
