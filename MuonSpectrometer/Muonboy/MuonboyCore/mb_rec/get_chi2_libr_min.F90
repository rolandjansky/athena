!
!>\author M.Virchaux
!
 SUBROUTINE Get_Chi2_Libr_Min(ISCM,ChiM)
 USE M_MB_General_CONSTANTS, ONLY : CP0001
 USE M_MB_Fit
 IMPLICIT None
 INTEGER, INTENT(OUT) :: ISCM
 REAL(8), INTENT(OUT) :: ChiM
 INTEGER :: L
 REAL(8) :: C, Chi2_Libr
!
   IF( ABS(SMU) < CP0001 ) SMU = CP0001
   CALL TRAKMU(1)
!
   ISCM = 0
   ChiM = 99999999.D0
   DO L=1,NBLIBR
     C = Chi2_Libr(L)
     IF( C >= ChiM ) CYCLE
     ChiM = C
     ISCM = ISCLIBR(L)
   ENDDO
!
 END SUBROUTINE Get_Chi2_Libr_Min
!
