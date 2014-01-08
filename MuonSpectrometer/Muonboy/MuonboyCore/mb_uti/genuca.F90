!
!> \author M.Virchaux
!
 SUBROUTINE GENUCA(CAR,NAA,NII,NFF,  AAA,III,FFF,IERROR)
 IMPLICIT NONE
 CHARACTER(*),   INTENT(IN)  :: CAR
 INTEGER       , INTENT(IN)  :: NAA,NII,NFF
 INTEGER       , INTENT(OUT) :: IERROR
 CHARACTER(80) , INTENT(OUT) :: AAA(*)
 INTEGER       , INTENT(OUT) :: III(*)
 REAL(8)       , INTENT(OUT) :: FFF(*)
 CHARACTER(1)      :: CAR1(LEN_TRIM(CAR)+1)
 INTEGER           :: I, J, LENGTA, LENGTI, ISINGI, LENGTF, IPOINT, L09, LENCAR
 INTEGER, EXTERNAL :: J_ValNum_Char1
 REAL(8)           :: SIGNEF
!
   LENCAR = LEN_TRIM(CAR)
   IF( LENCAR > 0 ) THEN
     FORALL (I=1:LENCAR) CAR1(I) = CAR(I:I)
     WHERE( IACHAR(CAR1(1:LENCAR)) == 9 )   CAR1(1:LENCAR) = ' '   ! remove TABs if any
   ENDIF
   LENCAR = LENCAR + 1
   CAR1(LENCAR) = ' '
!
   J = 0
   IERROR = 0
!
!>> Characters <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   I = 0
10 I = I + 1
   IF( I > NAA ) GO TO 30
   LENGTA = 0
20 J = J + 1
   IF( J > LENCAR ) THEN
     IERROR = 1
     RETURN
   ENDIF
   IF( CAR1(J) == ' ' ) THEN
     IF( LENGTA <= 0 ) THEN
       GO TO 20
     ELSE
       GO TO 10
     ENDIF
   ELSE
     IF( LENGTA <= 0 ) THEN
       LENGTA = 1
       AAA(I) = CAR1(J)
     ELSE IF( LENGTA < 80 ) THEN
       LENGTA = LENGTA + 1       
       AAA(I)(LENGTA:LENGTA) = CAR1(J)
     ENDIF
     GO TO 20
   ENDIF
!
!>> Integers <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
30 I = 0
40 I = I + 1
   IF( I > NII ) GO TO 60
   III(I) = 0
   LENGTI = 0
   ISINGI = 1
50 J = J + 1
   IF( J > LENCAR ) THEN
     IERROR = 1
     RETURN
   ENDIF
   IF( CAR1(J) == ' ' ) THEN
     IF( LENGTI <= 0 ) THEN
       GO TO 50
     ELSE
       III(I) = ISINGI*III(I)
       GO TO 40
     ENDIF
   ELSE IF( CAR1(J) == '+' ) THEN
     ISINGI = 1
     GO TO 50
   ELSE IF( CAR1(J) == '-' ) THEN
     ISINGI = -1
     GO TO 50
   ENDIF
   L09 = J_ValNum_Char1( CAR1(J) )
   IF( L09 >= 0 ) THEN
     LENGTI = LENGTI + 1
     III(I) = III(I)*10 + L09
     GO TO 50
   ENDIF
   IERROR = 1
   RETURN
!
!>> Reals <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
60 I = 0
70 I = I + 1
   IF( I > NFF ) RETURN
   FFF(I) = 0.D0
   LENGTF = 0
   SIGNEF = 1.D0
   IPOINT = 0
80 J = J + 1
   IF( J > LENCAR ) THEN
     IERROR = 1
     RETURN
   ENDIF
   IF( CAR1(J) == ' ' ) THEN
     IF( LENGTF <= 0 ) THEN
       GO TO 80
     ELSE
       FFF(I) = SIGNEF*FFF(I)
       GO TO 70
     ENDIF
   ELSE IF( CAR1(J) == '+' ) THEN
     SIGNEF = 1.D0
     GO TO 80
   ELSE IF( CAR1(J) == '-' ) THEN
     SIGNEF = -1.D0
     GO TO 80
   ELSE IF( CAR1(J) == '.' ) THEN
     IF( IPOINT >= 1 ) THEN
       IERROR = 1
       RETURN
     ELSE
       IPOINT = 1
     ENDIF
     GO TO 80
   ENDIF
   L09 = J_ValNum_Char1( CAR1(J) )
   IF( L09 >= 0 ) THEN
     LENGTF = LENGTF + 1
     IF( IPOINT <= 0 ) THEN
       FFF(I) = FFF(I)*10.D0 + L09
     ELSE
       FFF(I) = FFF(I) + L09/10.D0**IPOINT
       IPOINT = IPOINT + 1
     ENDIF
     GO TO 80
   ENDIF
   IERROR = 1
   RETURN
!
 END SUBROUTINE GENUCA
!
