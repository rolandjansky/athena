!
!> Find the "amdc_simrec" indexes KTYP,KFF,KZZ,KOB
!> corresponding to the precision chamber IC,INOF,INOZ
!
 SUBROUTINE IDBCHA(IC,INOF,INOZ ,KTYP,KFF,KZZ,KOB)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IC, INOF, INOZ         !< muonboy indices of a MDT multilayer
 INTEGER, INTENT(OUT) :: KTYP, KFF, KZZ, KOB    !< amdc indices of a MDT multilayer
 INTEGER :: IGEOM, NN
!
   IGEOM = IGEOMA(IC,INOF,INOZ)
   IF( IGEOM < 1 ) RETURN
!
   NN = ISTTYPV(IGEOM)
   CALL IDECI(NN, KTYP,KFF,KZZ,KOB)
!
 END SUBROUTINE IDBCHA
!
