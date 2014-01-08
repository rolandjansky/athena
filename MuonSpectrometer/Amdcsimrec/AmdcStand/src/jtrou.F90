!
!> \author SamuSog
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 PURE INTEGER FUNCTION JTROU3(AA,NN,CC)
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN) :: AA, CC(*) 
 INTEGER     , INTENT(IN) :: NN
 INTEGER :: I
!
    DO I=1,NN
      IF( AA == CC(I) ) THEN
        JTROU3 = I
        RETURN
      ENDIF
    ENDDO
    JTROU3 = 0
!
 END FUNCTION JTROU3
!
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 PURE INTEGER FUNCTION JTROU1(AA,NN,CC)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN) :: AA, CC(*)
 INTEGER     , INTENT(IN) :: NN
 INTEGER :: I
!
    DO I=1,NN
      IF( AA == CC(I) ) THEN
        JTROU1 = I
        RETURN
      ENDIF
    ENDDO
    JTROU1 = 0
!
 END FUNCTION JTROU1
!
