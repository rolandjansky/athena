!
!> Find the MUONBOY MDT chamber indexes  IC,INOF,INOZ
!>  or  the MUONBOY trigger chamber index  ICT
!> from the "amdc_simrec" indexes  JTYP,JFF,JZZ,JOB
!> \author M.Virchaux
!
 SUBROUTINE GETIND(JTYP,JFF,JZZ,JOB, IC,INOF,INOZ, ICT)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ,JOB
 INTEGER, INTENT(OUT) :: IC,INOF,INOZ, ICT
 INTEGER :: IIII, ICP, INOZP, IGEOM
 INTEGER :: ILO, IME, IHI
!
#include "AmdcStand/amdcsim_param.inc"
!
    IC   = 0
    INOF = 0
    INOZ = 0
    ICT  = 0
    IF( JTYP < 1 .OR. JTYP > MTYP ) RETURN
    IF( ISCISC(JTYP) <= 0 )         RETURN
!
    CALL CDECI(JTYP,JFF,JZZ,JOB, IIII)
!
    IF( ISCISC(JTYP) <= NMACSC ) THEN
      DO ICP=2*ISCISC(JTYP)-1,2*ISCISC(JTYP)
        DO INOZP=1,NZM
          IGEOM = IGEOMA(ICP,JFF,INOZP)
          IF( IGEOM < 1 ) CYCLE
          IF( ISTTYPV(IGEOM) /= IIII ) CYCLE
          IC   = ICP
          INOF = JFF
          INOZ = INOZP
          RETURN
        ENDDO
      ENDDO
      ILO = 1
      IHI = NTRIBA
    ELSE
      ILO = NTRIBA + 1
      IHI = NTRITO
    ENDIF
!
100 IME = (IHI+ILO) / 2
    IF( IIII < JDBTRI(IME) ) THEN
      IF( IME-ILO <= 1 ) THEN
        IF( IIII == JDBTRI(ILO) ) ICT = ILO
        RETURN
      ELSE
        IHI = IME
      ENDIF
    ELSE
      IF( IHI-IME <= 1 ) THEN
        IF( IIII == JDBTRI(IME) ) ICT = IME
        IF( IIII == JDBTRI(IHI) ) ICT = IHI
        RETURN
      ELSE
        ILO = IME
      ENDIF
    ENDIF
    GO TO 100
!
 END SUBROUTINE GETIND
!
