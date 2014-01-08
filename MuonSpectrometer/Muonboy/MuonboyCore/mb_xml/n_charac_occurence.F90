!
 PURE INTEGER FUNCTION N_Charac_Occurence( Name, Car )
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN) :: Name,Car
 INTEGER :: N,L1
!
   N_Charac_Occurence = 0
   L1 = 1
!
   DO
     N = SCAN( Name(L1:), Car )
     IF( N < 1 ) EXIT
     N_Charac_Occurence = N_Charac_Occurence + 1
     L1 = L1 + N
   ENDDO
!
 END FUNCTION N_Charac_Occurence
!
