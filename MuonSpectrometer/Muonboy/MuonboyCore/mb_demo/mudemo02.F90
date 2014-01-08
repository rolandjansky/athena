!
!> Demonstration of
!> the use of common for tracks after straight segments fit
!
 SUBROUTINE MUDEMO02
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: I
!
   WRITE(*,'(/,"*COFIDR content: ",/," There are ",I5," tracks")') NCFIDR
   IF (NCFIDR /= 0) THEN
     DO I=1,NCFIDR
       WRITE(*,'("    For track nber ",I4," at spectro entrance:")') I
       WRITE(*,'("     VThetaPhi(1 to 5) =",5F12.5)')         PAFIDR(1:5,I)
       WRITE(*,'("     X ,Y ,Z           =",3F12.5)')         VMFIDR(1:3,I)
       WRITE(*,'("     Vx,Vy,Vz          =",3F12.5)')         VMFIDR(4:6,I)
       WRITE(*,'("     Preconstructed    =", F12.5)')         VMFIDR(7  ,I)
       WRITE(*,'("     Charge            =", F12.5)')         VMFIDR(8  ,I)
     ENDDO
   ENDIF
!
 END SUBROUTINE MUDEMO02
!
