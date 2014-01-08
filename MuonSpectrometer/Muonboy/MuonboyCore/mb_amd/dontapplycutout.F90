!
 SUBROUTINE DontApplyCutOut(CARTYP)
! do not apply cutout if not in simulation
 USE M_MB_MuGeom
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN) :: CARTYP(*)
 INTEGER :: IC, INOF, INOZ, IGEOM, ICT, JTYP, JFF, JZZ, JOB
 INTEGER :: KTYP, KFF, KZZ, KOB
!
    WRITE(*,'(" >>> cutout not applied in Muonboy, waiting for their implementation in simulation !")')
!
    DO IC=1,NCOUCH                               ! CSC cutout already implemented in simul
      DO INOF=1,8
        DO INOZ=1,NZM
          IGEOM = IGEOMA(IC,INOF,INOZ)
          IF( IGEOM <= 0 )                CYCLE
          CALL IDBCHA(IC,INOF,INOZ,KTYP,KFF,KZZ,KOB)
          IF( CARTYP(KTYP) == 'BOG' )  CYCLE  ! BOG cutout already implemented in simul
          ISTCUTV(IGEOM) = 0
        ENDDO
      ENDDO
    ENDDO
!
    DO ICT=1,NTRITO
      CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
      IF( JTYP <= 0 )                     CYCLE  ! BOG cutout already implemented in simul
      IF( CARTYP(JTYP) == 'BOG' )         CYCLE
      ITRCUT(ICT) = 0
    ENDDO
!
 END SUBROUTINE DontApplyCutOut
!
