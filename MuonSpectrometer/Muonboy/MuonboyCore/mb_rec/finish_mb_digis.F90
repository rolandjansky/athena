!
!> \author M.Virchaux
!
 SUBROUTINE Finish_MB_Digis(ISTATIS)
 USE M_MB_Control
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ISTATIS
 INTEGER, SAVE :: KOUNTE = 0
 REAL(8), SAVE :: XTUBHM = 0.D0, XBANHM = 0.D0
!
!-- One print out various statistics
    IF(ISTATIS <= 0) THEN
      IF(KOUNTE /= 0) XTUBHM = XTUBHM / KOUNTE
      IF(KOUNTE /= 0) XBANHM = XBANHM / KOUNTE
      PRINT 9600,XTUBHM,XBANHM
9600  FORMAT(/'  MEAN NUMBER OF TUBE HITS  per EVENT :',F10.1,/'  MEAN NUMBER OF STRIP HITS per EVENT :',F10.1////)
      RETURN
    ENDIF
!
!-- Terminate module M_MB_Digis filling
!
!-- Re-shuffles the digi bank (in module M_MB_Digis)
    CALL arrangeTubes
!
!-- Remove noisy strips from ROA making
    CALL noisyStrips
!
!-- One updates various statistics
    XTUBHM = XTUBHM + NTUBHT
    XBANHM = XBANHM + NBANHT
    KOUNTE = KOUNTE + 1
!
!-- Print module M_MB_Digis
    IF( MBPri >= 2 ) CALL Print_MB_Digis
!
 END SUBROUTINE Finish_MB_Digis
!
