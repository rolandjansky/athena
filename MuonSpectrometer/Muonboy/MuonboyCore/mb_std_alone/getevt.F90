!
 SUBROUTINE GETEVT(LUNIN,LUNPILEUP,IEVtoRead,TOSTORE,IRET,IDEVE,STORE)
 USE M_MB_Control
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: LUNIN, LUNPILEUP, IEVtoRead, TOSTORE
 INTEGER, INTENT(OUT)   :: IRET, IDEVE
 INTEGER, INTENT(INOUT) :: STORE
 INTEGER :: NREAD, IRETR, I
 INTEGER, SAVE :: NLUEVE = 0, NPROEV = 0
!>>
!>> Read new event
!>>
!>> Input:
!>>        LUNIN     : logical unit for the input evts file
!>>        LUNPILEUP : logical unit for the "pile up" evts file (for LHCTOR evts only)
!>>        IEVtoRead : event number to be read, -1 for next one
!>> Output:
!>>        IRET  = 0    : no more evt is availbale or should be read
!>>              = 1/-1 : new evt is available and selected/rejected
!>>
!
    IRET = 0
!
!>> One print out various statistics
    IF (LUNIN <= 0) THEN
      PRINT 9000,NLUEVE,NPROEV
9000  FORMAT(/////'  NUMBER OF EVENTS READ               :',I6    &
                //'  NUMBER OF EVENTS PROCESSED          :',I6)
      RETURN
    ENDIF
!
!>> One read one event
    CALL GEVTDUMPDI(LUNIN,IEVtoRead,TOSTORE,IRETR,IDEVE,STORE)
    NREAD = IRETR
!
!>> No more evt is available
    IRET = 0
    IF(IRETR == 0)      RETURN
!
!>> No more evt should be read
    IRET = 0
    IF(IDEVE > MBev1)   RETURN
    IF(NPROEV >= MBevt) RETURN
!
!>> The current evt should be skipped
    IRET = -1
    IF(IDEVE < MBev0)  RETURN
    IF(MBidn >= 1) THEN
      DO I=1,MBidn
        IF(IDEVE.EQ.MBide(I)) GO TO 170
      ENDDO
      RETURN
    ENDIF
!
!>> The current evt is accepted
170 IRET = 1
!
!>> One update various statistics
    NPROEV = NPROEV + 1
    NLUEVE = NLUEVE + NREAD
!
!-- Re-arange (and possibly update) module M_MB_Digis
    CALL ktime_mubo('finicoco',1)
    CALL Finish_MB_Digis(1)
    CALL ktime_mubo('finicoco',2)
!
 END SUBROUTINE GETEVT
!
