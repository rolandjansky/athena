!
!> Find the "amdc_simrec" indexes JTYP,JFF,JZZ,JOB
!> corresponding to the trigger chamber ICT
!
 SUBROUTINE IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: ICT                !< muonboy index of trigger station
 INTEGER, INTENT(OUT) :: JTYP, JFF, JZZ, JOB !< amdc indices of trigger station
 INTEGER :: NN
!
   IF(ICT < 1 .OR. ICT > NTRITO) THEN
     JTYP = 0
     JFF  = 0
     JZZ  = 0
     JOB  = 0
   ELSE
     NN = JDBTRI(ICT)
     CALL IDECI(NN, JTYP,JFF,JZZ,JOB)
   ENDIF
!
 END SUBROUTINE IDBTRI
!
