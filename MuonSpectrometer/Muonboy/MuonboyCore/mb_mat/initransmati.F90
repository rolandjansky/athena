!
!> \author M.Virchaux
!
 SUBROUTINE INITRANSMATI
 USE M_MB_General_CONSTANTS, ONLY : PiSect
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: JFF
 REAL(8) :: F0, SINF0, COSF0
!
   NTMATI = 18
   IF( NTMATI > KtransMati ) THEN
     PRINT '(//" In INITRANSMATI : Parameter KtransMati is not big enough  =====>  STOP !")'
     STOP
   ENDIF
!
!>> Initialise all transformations to identity
   TRANSMATI( 1:3, 1:4, 1:KtransMati ) =  0.D0
   TRANSMATI(   1,   1, 1:KtransMati ) =  1.D0
   TRANSMATI(   2,   2, 1:KtransMati ) =  1.D0
   TRANSMATI(   3,   3, 1:KtransMati ) =  1.D0
!
!>> Transformation  11  is the  Z+/Z- symetry
   TRANSMATI(3,3,11) = -1.D0
!
   DO JFF=2,8
     F0 = (JFF-1)*PiSect
     SINF0 = SIN(F0)
     COSF0 = COS(F0)
!>> Transformations  2 to 8  are  Phi rotations
     TRANSMATI(1,1,JFF)    =  COSF0
     TRANSMATI(1,2,JFF)    = -SINF0
     TRANSMATI(2,1,JFF)    =  SINF0
     TRANSMATI(2,2,JFF)    =  COSF0
     TRANSMATI(3,3,JFF)    =   1.D0
!>> Transformations  12 to 18  are  Phi rotation  +  Z+/Z- symetry
     TRANSMATI(1,1,JFF+10) =  COSF0
     TRANSMATI(1,2,JFF+10) = -SINF0
     TRANSMATI(2,1,JFF+10) =  SINF0
     TRANSMATI(2,2,JFF+10) =  COSF0
     TRANSMATI(3,3,JFF+10) =  -1.D0
   ENDDO
!
 END SUBROUTINE INITRANSMATI
!
