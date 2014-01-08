!
!> Print out output of pattern contained in Module  M_MB_Reco_Tracks
!> \author M.Virchaux
!
 SUBROUTINE PRCOCA
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: NOGOCA(20)
 INTEGER :: NBGOCA, N
!
   NBGOCA = 0
   DO N=1,NBCAN
     IF( CHICAN(N) > 999.D0 ) CYCLE
     NBGOCA = MIN( 20 , NBGOCA+1 )
     NOGOCA(NBGOCA) = N
   ENDDO
   PRINT 1000,NBGOCA,NOGOCA(1:NBGOCA)
   PRINT 2000
!
   CALL WRICAN(6,0)
!
1000  FORMAT(/' In Module  M_MB_Reco_Tracks , there are ',I3,'  "good" track candidates :',20I4)
2000  FORMAT( '                                         ****' )
 END SUBROUTINE PRCOCA
!
