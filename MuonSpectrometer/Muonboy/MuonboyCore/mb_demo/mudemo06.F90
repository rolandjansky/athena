!
!> \author J.F.Laporte
!
 SUBROUTINE MUDEMO06
 USE M_MB_Control, ONLY : LunDumTra
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: ican
 LOGICAL, SAVE :: FIRST=.TRUE.
 INTEGER, SAVE :: iPseudoEvtKounter=0
 INTEGER :: kounter, kounterMax, L
!
   IF( LunDumTra > 0 ) THEN
     IF( FIRST ) THEN
       FIRST=.FALSE.
       Open(unit=LunDumTra, file='Out.RecTracks.txt', status='unknown')
     ENDIF
!
     iPseudoEvtKounter = iPseudoEvtKounter + 1
     kounter = 0
     DO ican=1,nbcan
       IF (chican(ican) >= 999.d0)  CYCLE
       kounter = kounter + 1
     enddo
     kounterMax = kounter
     WRITE(LunDumTra,7004) iPseudoEvtKounter,kounterMax
     kounter = 0
     DO ican=1,nbcan
       IF (chican(ican) >= 999.d0)  CYCLE
       kounter = kounter + 1
       WRITE(LunDumTra,7000) iPseudoEvtKounter,kounterMax,&
       kounter, NZXCAN(ican),&
       vxxcan(1:8,ican)
       DO L=1,NZXCAN(ican)
         WRITE(LunDumTra,7002) iPseudoEvtKounter,kounterMax,&
           kounter, NZXCAN(ican),&
           L,RZXCAN(L,ican),RZICAN(L,ican),RZICAN(L,ican)-RZXCAN(L,ican),&
           DTUBHI(IZXCAN(L,ican))
       ENDDO
     ENDDO
   ENDIF
!
7004 FORMAT('EVT ',2i6)
7000 FORMAT('TRA ',2i6,i6,i6,8F12.5)
7002 FORMAT('DIG ',2i6,i6,i6,i6,4F12.5)
!
 END SUBROUTINE MUDEMO06
!
