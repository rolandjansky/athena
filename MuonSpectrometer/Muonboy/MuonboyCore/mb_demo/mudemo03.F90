!
!>  Demonstration of
!>  the use of common for tracks after final fit
!
 SUBROUTINE MUDEMO03
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: I
!
   WRITE(*,'(/,"*Module  M_MB_Reco_Tracks  content: ",/," There are ",I5," tracks")') NBCAN
   IF (NBCAN /= 0) THEN
     DO I=1,NBCAN
       WRITE(*,'("    For track nber ",I4," at spectro entrance:")') I
       WRITE(*,'("     VThetaPhi(1 to 5) =",5F12.5)')         PAXCAN(1:5,I)
       WRITE(*,'("     X ,Y ,Z           =",3F12.5)')         VXXCAN(1:3,I)
       WRITE(*,'("     Vx,Vy,Vz          =",3F12.5)')         VXXCAN(4:6,I)
       WRITE(*,'("     Preconstructed    =", F12.5)')         VXXCAN(7  ,I)
       WRITE(*,'("     Charge            =", F12.5)')         VXXCAN(8  ,I)
       WRITE(*,'(" Quality factor and Flag =", F12.5,8X,I3)') CHICAN(I),FLGCAN(I)
     ENDDO
   ENDIF
!
 END SUBROUTINE MUDEMO03
!
