!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Explode_Car( CAR,IAmax, IA,AAA,NNN )
 IMPLICIT NONE
 CHARACTER (len=*) :: CAR,     AAA(*)
 INTEGER           :: IAmax,IA,NNN(*)
!
 INTEGER            :: J,Iopen,Length,LenCAR
 INTEGER, EXTERNAL  :: J_ValBlank_Char1
 CHARACTER(1), SAVE :: CAR2QUOT = '"'
!
   J      = 0
   IA     = 0
   Iopen  = 0
   Length = 0
   LenCAR = MIN( LEN(CAR) , LEN_TRIM(CAR)+1 )
!
10 IF( Length > 0 ) THEN
     IA = IA + 1
     AAA(IA) = CAR(J-Length:J-1)
     NNN(IA) = Length
     IF( IA >= IAmax ) RETURN
   ENDIF
   Length = 0
!
20 J = J + 1
   IF( J > LenCAR ) RETURN
   IF( Iopen == 1 ) THEN
     IF( CAR(J:J) == CAR2QUOT ) THEN
       Iopen = 0
       GO TO 10
     ENDIF
   ELSE
     IF(  CAR(J:J) == CAR2QUOT ) Iopen = 1
     IF( J_ValBlank_Char1( CAR(J:J) ) > 0 ) GO TO 10
   ENDIF
   Length = Length + 1
   GO TO 20
!
 END
