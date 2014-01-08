!
!> give the number of missing stations (nsta) and missing multi-layers (nml)
!
 SUBROUTINE getMissingStations( ican, nsta, nml )
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ican     !< track candidate number
 INTEGER, INTENT(OUT) :: nsta     !< number of missing stations
 INTEGER, INTENT(OUT) :: nml      !< number of missing multi-layers
 INTEGER :: ki, it, ic, isc, jml
 INTEGER :: icross(1:2), ifound(1:2)
!
! first : list of stations crossed = ICROSS
    icross(:) = 0
    DO ki=1,12
      IF( itxcan(ki,ican) > 0 ) THEN
        isc = MOD(itxcan(ki,ican), 100)
        IF(itxcan(ki,ican) < 200)                             icross(1) = IBSET(icross(1), isc)
        IF(itxcan(ki,ican) < 100 .OR. itxcan(ki,ican) > 200)  icross(2) = IBSET(icross(2), isc)
      ENDIF
    ENDDO
! then list of stations found = IFOUND
    ifound(:) = 0
    DO ki=1,nzxcan(ican)
      it = izxcan(ki,ican)
      ic  = ktubhi(it)/10000
      IF( ic > NCOUCH .AND. rtubhi(it) < -10.d0 ) CYCLE  ! do not count S strips as found
      isc = (ic+1)/2
      IF( ic /= 2*isc ) THEN
        ifound(1) = IBSET(ifound(1), isc)
      ELSE
        ifound(2) = IBSET(ifound(2), isc)
      ENDIF
    ENDDO
! Now kill tracks if too many missing stations
    nsta = 0
    nml  = 0
    DO ki=1,NMATMA
      jml = 0
      IF( BTEST(icross(1),ki) .AND. .NOT.BTEST(ifound(1),ki) )   jml = jml + 1
      IF( BTEST(icross(2),ki) .AND. .NOT.BTEST(ifound(2),ki) )   jml = jml + 1
      IF( jml == 2 ) THEN            ! full station missing
        nsta = nsta + 1
      ELSE IF( jml == 1 ) THEN       ! only one ML missing
        nml  = nml + 1
      ENDIF
    ENDDO
!
 END SUBROUTINE getMissingStations
!
