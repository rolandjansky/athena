!
 INTEGER FUNCTION NTrCommonHits(nudr,iudr)
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: nudr, iudr(nudr)
 INTEGER :: ican
 LOGICAL :: hasCommonHits
!
! Give the number of track candidates with at list one of the hits included in iudr(1:nudr)
!
   NTrCommonHits = 0
   DO ican=1,nbcan
     IF( chican(ican) > 999.d0 ) CYCLE
     IF( hasCommonHits(ican, nudr, iudr) )  NTrCommonHits = NTrCommonHits + 1
   ENDDO
!
 END FUNCTION NTrCommonHits
!
 INTEGER FUNCTION NGoodTrCommonHits(nudr,iudr)
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: nudr, iudr(nudr)
 INTEGER :: ican, i
 LOGICAL :: hasCommonHits
!
! Give the number of "very good" track candidates with at list one of the hits included in iudr(1:nudr)
!
     NGoodTrCommonHits = 0
can: DO ican=1,nbcan
       IF( chican(ican)  > 999.d0   ) CYCLE
       IF( cxxcan(2,ican) >= 6.d0   ) CYCLE
       IF( ITXCAN(2,ican) == 0      ) CYCLE  ! at least 2 stations
       IF( NSSCAN(ican) < 2         ) CYCLE  ! at least 2 phi strips
       DO i=1,12
         IF( ITXCAN(i,ican) == 0              ) EXIT
         IF( ITXCAN(i,ican) /= ITFCAN(i,ican) ) CYCLE can  ! all crossed layers have hits
       ENDDO
       IF( hasCommonHits(ican, nudr, iudr) )  NGoodTrCommonHits = NGoodTrCommonHits + 1
     ENDDO can
!
 END FUNCTION NGoodTrCommonHits
!
 LOGICAL FUNCTION hasCommonHits(ican, nudr, iudr)
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ican, nudr, iudr(nudr)
 INTEGER :: i, it, ic
 INTEGER :: intpos
!
   hasCommonHits = .false.
   DO i=1,nzxcan(ican)
     it = izxcan(i,ican)
     ic = ktubhi(it) / 10000
     if( ic > NCOUCH .AND. RTUBHI(it) < -10.d0 )  CYCLE  ! do not count S strips
     IF( intpos(it, iudr, nudr) /= 0 ) THEN
       hasCommonHits = .true.
       RETURN
     ENDIF
   ENDDO
!
 END FUNCTION hasCommonHits
!
