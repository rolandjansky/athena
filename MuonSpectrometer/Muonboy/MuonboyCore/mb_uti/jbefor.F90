!
!> Gets an integer whose first decimal digits are those
!> of J1 and last decimal digits those of J2
!
 PURE INTEGER FUNCTION JBEFOR(J1,J2)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: J1, J2
 INTEGER :: JJ1, JJ2, L, JJT
!
    JJ1 = J1
    JJ2 = J2
10  DO 30 L=1,3
      IF( JJ1 < 100**L ) THEN
20      IF( JJ2 < 100**(4-L) ) GO TO 40
        JJ2 = JJ2 / 100
        GO TO  20
      ENDIF
30  ENDDO
    JJ1 = JJ1 / 100
    GO TO 10
!
40  JJT = JJ2
50  JJ1 = JJ1 * 100
    JJT = JJT / 100
    IF( JJT > 0 ) GO TO 50
    JBEFOR = JJ1 + JJ2
!
 END FUNCTION JBEFOR
!
