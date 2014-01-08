!
!> \author M.Virchaux
!
 SUBROUTINE INCR_NTMATI
 USE M_MB_COMATI
 IMPLICIT NONE
!
   NTMATI = NTMATI + 1
   IF( NTMATI > KtransMati ) THEN
     PRINT '(//" In INCR_NTMATI of MUONBOY : Parameter KtransMati is not big enough  =====>  STOP !!!")'
     STOP
   ENDIF
!
 END SUBROUTINE INCR_NTMATI
!
