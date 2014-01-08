!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE PRINT_XML
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER :: IOBJ
!
   PRINT '(/" Number of XML objects =",I7)',Nb_xob
   DO IOBJ=1,Nb_xob
     CALL PRINT_XOB(IOBJ)
   ENDDO
!
 END
