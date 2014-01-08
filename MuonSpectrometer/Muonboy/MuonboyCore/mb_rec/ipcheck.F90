!
 SUBROUTINE IPcheck
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER :: ican, jcan, ki, it, intpos, nstamiss, nmlmiss
 REAL(8) :: cov(2,2)
!
! loose track selection based on compatibility with primary vertex
!
   DO ican=1,nbcan
     IF( flgcan(ican) /= 3 .AND. chican(ican) < 1000. ) chican(ican) = chican(ican) + 1000. 
     IF( chican(ican) > 1000.d0 ) CYCLE
     cov(1:2,1:2) = cozcan(1:2,1:2,ican)
     cov(1,1) = cov(1,1) +  2.d0**2
     cov(2,2) = cov(2,2) + 20.d0**2
     cxxcan(3,ican) = ( cov(2,2)*pazcan(1,ican)*pazcan(1,ican) - cov(1,2)*pazcan(1,ican)*pazcan(2,ican)   &
                      - cov(2,1)*pazcan(2,ican)*pazcan(1,ican) + cov(1,1)*pazcan(2,ican)*pazcan(2,ican) ) &
                    / ( cov(1,1)*cov(2,2) - cov(1,2)*cov(2,1) )
#if !defined(ATLAS_NOPRINTMU)
     IF( MBPri >= 3 ) write(*,'(" IPcheck : candidate ",I3," with chi2 vs IP ",F6.1)') ican,cxxcan(3,ican)
#endif
   ENDDO
!
! Remove all tracks incompatible with IP and with at least one hit in common with a track compatible with IP
!
b10: DO 10 ican=1,nbcan
       IF( chican(ican) > 1000.d0 )   CYCLE
       IF( cxxcan(3,ican) <= 5.d0 )   CYCLE
       DO jcan=1,nbcan
         IF( chican(jcan) > 1000.d0 ) CYCLE
         IF( cxxcan(3,jcan) >  5.d0 ) CYCLE
         DO ki=1,nzxcan(ican)
           it  = izxcan(ki,ican)
           IF( intpos( it, izxcan(:,jcan), nzxcan(jcan) ) /= 0 ) THEN
#if !defined(ATLAS_NOPRINTMU)
             IF( MBPri >= 3 ) WRITE(*,'(" IPcheck : candidate removed ",I3,"  chi2 vs IP ",F6.1," (common hits)")')  &
                              ican,cxxcan(3,ican)
#endif
             chican(ican) = chican(ican) + 1000.d0
             CYCLE b10
           ENDIF
         ENDDO
         DO ki=1,nsxcan(ican)
           it  = isxcan(ki,ican)
           IF( intpos( it, isxcan(:,jcan), nsxcan(jcan) ) /= 0 ) THEN
#if !defined(ATLAS_NOPRINTMU)
             IF( MBPri >= 3 ) WRITE(*,'(" IPcheck : candidate removed ",I3,"  chi2 vs IP ",F6.1," (common hits)")')  &
                              ican,cxxcan(3,ican)
#endif
             chican(ican) = chican(ican) + 1000.d0
             CYCLE b10
           ENDIF
         ENDDO
       ENDDO
10   ENDDO b10
!
! Then remove also all tracks incompatible with IP and with one missing station
!
    DO ican=1,nbcan
      IF( chican(ican) > 1000.D0 )   CYCLE
      IF( cxxcan(3,ican) <= 20.d0 )  CYCLE
      CALL getMissingStations( ican, nstamiss, nmlmiss)
      IF( 3*nstamiss+nmlmiss >= 3 ) THEN
#if !defined(ATLAS_NOPRINTMU)
        IF( MBPri >= 3 ) WRITE(*,'(" IPcheck : candidate removed ",I3,"  chi2 vs IP ",F6.1," (missing station)")')  &
                         ican,cxxcan(3,ican)
#endif
        chican(ican) = chican(ican) + 1000.D0
        CYCLE
      ENDIF
    ENDDO
!
! And remove all tracks incompatible with IP and with a big chi2/ndof
!
    DO ican=1,nbcan
      IF( chican(ican) > 1000.D0 )   CYCLE
      IF( cxxcan(3,ican) <= 20.d0 )  CYCLE
      IF( cxxcan(1,ican) > 15.D0 * ndfcan(ican) ) THEN
#if !defined(ATLAS_NOPRINTMU)
        IF( MBPri >= 3 ) WRITE(*,'(" IPcheck : candidate removed ",I3,"  chi2 vs IP ",F6.1," (big chi2/ndof)")')  &
                         ican,cxxcan(3,ican)
#endif
        chican(ican) = chican(ican) + 1000.D0
        CYCLE
      ENDIF
    ENDDO
!
 END SUBROUTINE IPcheck
!
