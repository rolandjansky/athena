!
!> find the tracks to be reversed based on position of first hit 
!> and spectrometer entrance point. It is used only in cosmics.
!
 SUBROUTINE reverseTracks
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: I
   IF( AtCosm /= 1 ) RETURN
   DO I=1,NBCAN
     IF( CHICAN(I) > 999.d0 ) CYCLE
     SELECT CASE( AtCosm )
       CASE(1)    ! cosmics
         IF( YDCCAN(1,I) > VXXCAN(2,I) )  FLGCAN(I) = FLGCAN(I) + 10
       CASE(2)    ! beam halo with beam from z- to z+
         IF( ZDCCAN(1,I) < VXXCAN(3,I) )  FLGCAN(I) = FLGCAN(I) + 10
       CASE(3)    ! beam halo with beam from z+ to z-
         IF( ZDCCAN(1,I) > VXXCAN(3,I) )  FLGCAN(I) = FLGCAN(I) + 10
       CASE DEFAULT
         WRITE(*,*) ' reverseTracks : should not be here with AtCosm ',AtCosm
     END SELECT
     IF( MBPri >= 3 ) THEN
       IF( FLGCAN(I) > 10 ) WRITE(*,'(" Track ",I3," should be reversed")') I
     ENDIF
   ENDDO
!
 END SUBROUTINE reverseTracks
!
