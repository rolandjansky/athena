!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEFPL3(F0, ICOLOR, N1,N2,N3, X1,X2,X3, Y1,Y2,Y3, Z1,Z2,Z3 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3
 REAL(8) :: F0,X1,X2,X3,Y1,Y2,Y3,Z1,Z2,Z3
 INTEGER :: NI(3)
 REAL(8) :: XI(3),YI(3),ZI(3)
!
   NI(:) = (/ N1, N2, N3 /)
   XI(:) = (/ X1, X2, X3 /)
   YI(:) = (/ Y1, Y2, Y3 /)
   ZI(:) = (/ Z1, Z2, Z3 /)
!
   CALL DEFPLX( F0, ICOLOR, 3, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL3
!***********************************************************************
 SUBROUTINE DEFPL4(F0, ICOLOR, N1,N2,N3,N4, X1,X2,X3,X4,  &
                               Y1,Y2,Y3,Y4, Z1,Z2,Z3,Z4 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4
 REAL(8) :: F0,X1,X2,X3,X4,Y1,Y2,Y3,Y4,Z1,Z2,Z3,Z4
 INTEGER :: NI(4)
 REAL(8) :: XI(4),YI(4),ZI(4)
!
   NI(:) = (/ N1, N2, N3, N4 /)
   XI(:) = (/ X1, X2, X3, X4 /)
   YI(:) = (/ Y1, Y2, Y3, Y4 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4 /)
!
   CALL DEFPLX( F0, ICOLOR, 4, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL4
!***********************************************************************
 SUBROUTINE DEFPL5(F0, ICOLOR, N1,N2,N3,N4,N5, X1,X2,X3,X4,X5,  &
                               Y1,Y2,Y3,Y4,Y5, Z1,Z2,Z3,Z4,Z5 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5
 REAL(8) :: F0,X1,X2,X3,X4,X5,Y1,Y2,Y3,Y4,Y5,Z1,Z2,Z3,Z4,Z5
 INTEGER :: NI(5)
 REAL(8) :: XI(5),YI(5),ZI(5)
!
   NI(:) = (/ N1, N2, N3, N4, N5 /)
   XI(:) = (/ X1, X2, X3, X4, X5 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5 /)
!
   CALL DEFPLX( F0, ICOLOR, 5, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL5
!***********************************************************************
 SUBROUTINE DEFPL6(F0, ICOLOR, N1,N2,N3,N4,N5,N6, X1,X2,X3,X4,X5,X6,  &
                               Y1,Y2,Y3,Y4,Y5,Y6, Z1,Z2,Z3,Z4,Z5,Z6 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5,N6
 REAL(8) :: F0,    X1,X2,X3,X4,X5,X6
 REAL(8) ::         Y1,Y2,Y3,Y4,Y5,Y6
 REAL(8) ::         Z1,Z2,Z3,Z4,Z5,Z6
 INTEGER :: NI(6)
 REAL(8) :: XI(6),YI(6),ZI(6)
!
   NI(:) = (/ N1, N2, N3, N4, N5, N6 /)
   XI(:) = (/ X1, X2, X3, X4, X5, X6 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5, Y6 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5, Z6 /)
!
   CALL DEFPLX( F0, ICOLOR, 6, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL6
!***********************************************************************
 SUBROUTINE DEFPL7(F0, ICOLOR, N1,N2,N3,N4,N5,N6,N7, X1,X2,X3,X4,X5,X6,X7,  &
                               Y1,Y2,Y3,Y4,Y5,Y6,Y7, Z1,Z2,Z3,Z4,Z5,Z6,Z7 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5,N6,N7
 REAL(8) :: F0,    X1,X2,X3,X4,X5,X6,X7
 REAL(8) ::        Y1,Y2,Y3,Y4,Y5,Y6,Y7
 REAL(8) ::        Z1,Z2,Z3,Z4,Z5,Z6,Z7
 INTEGER :: NI(7)
 REAL(8) :: XI(7),YI(7),ZI(7)
!
   NI(:) = (/ N1, N2, N3, N4, N5, N6, N7 /)
   XI(:) = (/ X1, X2, X3, X4, X5, X6, X7 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5, Y6, Y7 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5, Z6, Z7 /)
!
   CALL DEFPLX( F0, ICOLOR, 7, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL7
!***********************************************************************
 SUBROUTINE DEFPL8(F0, ICOLOR, N1,N2,N3,N4,N5,N6,N7,N8, X1,X2,X3,X4,X5,X6,X7,X8,  &
                               Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8, Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5,N6,N7,N8
 REAL(8) :: F0,    X1,X2,X3,X4,X5,X6,X7,X8
 REAL(8) ::        Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8
 REAL(8) ::        Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8
 INTEGER :: NI(8)
 REAL(8) :: XI(8),YI(8),ZI(8)
!
   NI(:) = (/ N1, N2, N3, N4, N5, N6, N7, N8 /)
   XI(:) = (/ X1, X2, X3, X4, X5, X6, X7, X8 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8 /)
!
   CALL DEFPLX( F0, ICOLOR, 8, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL8
!***********************************************************************
 SUBROUTINE DEFPL9(F0, ICOLOR, N1,N2,N3,N4,N5,N6,N7,N8,N9, X1,X2,X3,X4,X5,X6,X7,X8,X9,  &
                               Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9, Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5,N6,N7,N8,N9
 REAL(8) :: F0,    X1,X2,X3,X4,X5,X6,X7,X8,X9
 REAL(8) ::        Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9
 REAL(8) ::        Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9
 INTEGER :: NI(9)
 REAL(8) :: XI(9),YI(9),ZI(9)
!
   NI(:) = (/ N1, N2, N3, N4, N5, N6, N7, N8, N9 /)
   XI(:) = (/ X1, X2, X3, X4, X5, X6, X7, X8, X9 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9 /)
!
   CALL DEFPLX( F0, ICOLOR, 9, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL9
!***********************************************************************
 SUBROUTINE DEFPL0(F0, ICOLOR, N1,N2,N3,N4,N5,N6,N7,N8,N9,N0, X1,X2,X3,X4,X5,X6,X7,X8,X9,X0,  &
                               Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9,Y0, Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9,Z0 )
 IMPLICIT NONE
 INTEGER :: ICOLOR,N1,N2,N3,N4,N5,N6,N7,N8,N9,N0
 REAL(8) :: F0,    X1,X2,X3,X4,X5,X6,X7,X8,X9,X0
 REAL(8) ::        Y1,Y2,Y3,Y4,Y5,Y6,Y7,Y8,Y9,Y0
 REAL(8) ::        Z1,Z2,Z3,Z4,Z5,Z6,Z7,Z8,Z9,Z0
 INTEGER :: NI(10)
 REAL(8) :: XI(10),YI(10),ZI(10)
!
   NI(:) = (/ N1, N2, N3, N4, N5, N6, N7, N8, N9, N0 /)
   XI(:) = (/ X1, X2, X3, X4, X5, X6, X7, X8, X9, X0 /)
   YI(:) = (/ Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9, Y0 /)
   ZI(:) = (/ Z1, Z2, Z3, Z4, Z5, Z6, Z7, Z8, Z9, Z0 /)
!
   CALL DEFPLX( F0, ICOLOR, 10, NI, XI,YI,ZI )
!
 END SUBROUTINE DEFPL0
!
