!
!> \author SaMuSoG
!
   MODULE M_MB_KindDef
     IMPLICIT NONE
     INTEGER, PARAMETER :: kSP = 4    ! kind for single precision REAL
     INTEGER, PARAMETER :: kDP = 8    ! kind for double precision REAL
!
   CONTAINS
     SUBROUTINE Check_Compiler_Compatibility
       IF( KIND(1.0) == kSP .AND. KIND(1.D0) == kDP )  RETURN
       PRINT '(" === KIND not standard on this compiler !! ",2I4,"  ==> STOP ")',KIND(1.0),KIND(1.D0)
       STOP
     END SUBROUTINE Check_Compiler_Compatibility
   END MODULE M_MB_KindDef
!
