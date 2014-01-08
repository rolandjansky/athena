!
!> Compute the number of "good" candidate tracks going through station ISC0
!> \author M.Virchaux
!
 INTEGER FUNCTION NCANGO(ISC0)
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_PACONS
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: ISC0
 INTEGER :: ICAN, L
!
   NCANGO = 0
!
   IF( MBSto == 2 ) THEN
     DO ICAN=1,NCFIDR
       IF( CHICAN(ICAN) > 999.d0 )    CYCLE
       IF( ITFIDR(3,ICAN) <= 0 )      CYCLE
       DO L=1,12
         IF( ITFIDR(L,ICAN) /= ISC0 ) CYCLE
         NCANGO = NCANGO + 1
         EXIT
       ENDDO
     ENDDO
   ELSE
     DO ICAN=1,NBCAN
       IF( CHICAN(ICAN) > 999.d0 )         CYCLE
       IF( CXXCAN(2,ICAN) >= FLOOK2/5.D0 ) CYCLE
       IF( ITICAN(3,ICAN) <= 0 )           CYCLE
       DO L=1,12
         IF( ITICAN(L,ICAN) /= ISC0 ) CYCLE
         NCANGO = NCANGO + 1
         EXIT
       ENDDO
     ENDDO
   ENDIF
!
 END FUNCTION NCANGO
!
