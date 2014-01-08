!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE ELIMAT( IV0 )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IV0
 INTEGER :: NADMA 
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
   NADMA = NAMATI
   CALL ELIMAT_DIM( NADMA, IV0 )
!
 END
