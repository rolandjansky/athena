!
!> \author M.Virchaux
!
 SUBROUTINE Get_Chi2_LibrFixe_Min(ISCM,ChiM)
 USE M_MB_General_CONSTANTS, ONLY : CP0001
 USE M_MB_PACONS
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: ISCM
 REAL(8), INTENT(OUT) :: ChiM
 INTEGER :: L
 REAL(8) :: C, Chi2_Libr, Chi2_Fixe
!
   IF( ABS(SMU) < CP0001 ) SMU = CP0001
   CALL TRAKMU(1)
!
   ISCM = 0
   ChiM = 99999999.D0
   DO L=1,NBLIBR
     C = Chi2_Libr(L)
     IF( ISENFI == 1 .AND. L >= 2 ) C = C + DCLOOK/2.D0
     IF( C >= ChiM ) CYCLE
     ChiM = C
     ISCM = ISCLIBR(L)
   ENDDO
!
   DO L=2,NBFIXE
     ChiM = ChiM + Chi2_Fixe(L)
   ENDDO
!
 END SUBROUTINE Get_Chi2_LibrFixe_Min
!
