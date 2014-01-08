!
!> Determine track positions and directions at MS centers 
!> (adapted from persint_pro/trmuon.FF90)
!> \author J.F.Laporte
!
 SUBROUTINE POSMUSC
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: IPAS,IMASC
 REAL(8) :: PAPAS,R3IN,R3OUT,R2OUT,AZOUT,DR3IN,DR3OUT,PAPABI
 REAL(8) :: VIN(8),VOUT(8),VOUTBI(8)
!
    VIN(1:8) = VDEB(1:8,1)
    IDMASC(1:NBMASC) = 0
    IValMASC(1:NBMASC) = 0
!
    CALL setSignEloss(1,Vin(4:6)) 
    DO 600 IPAS=1,2000
      R3IN = getRMUSC(VIN)
!--                        Isens = 1  ===>  Tracking is made away from the vertex
!--                                |
!--                       Nvin = 1 |
!--                              | |
      CALL VINVOU(VIN,VOUT,PAPAS,1,1)
      R3OUT = getRMUSC(VOUT)
      R2OUT = SQRT( VOUT(1)**2 + VOUT(2)**2 )
      AZOUT = ABS( VOUT(3) )
!
      DO IMASC=1,NBMASC
        IF( IValMASC(IMASC) /= 0 ) CYCLE ! already crossed 
        DR3IN  = R3IN  - RRMASC(IMASC)
        DR3OUT = R3OUT - RRMASC(IMASC)
        IF( DR3IN*DR3OUT > 0.D0 )  CYCLE
        IValMASC(IMASC) = 1
        PAPABI = PAPAS * (-DR3IN)/(DR3OUT-DR3IN)
        CALL ELEMHC(VIN,1,PAPABI, VOUTBI)
        XposMASC(IMASC) = VOUTBI(1)
        YposMASC(IMASC) = VOUTBI(2)
        ZposMASC(IMASC) = VOUTBI(3)
        XdirMASC(IMASC) = VOUTBI(4)
        YdirMASC(IMASC) = VOUTBI(5)
        ZdirMASC(IMASC) = VOUTBI(6)
        PmomMASC(IMASC) = VOUTBI(7)
        ChgeMASC(IMASC) = VOUTBI(8)
      ENDDO
!
      IF( R2OUT > RCHAMA .OR. AZOUT > ZCHAMA ) EXIT
      VIN(1:8) = VOUT(1:8)
600 ENDDO
!
 END SUBROUTINE POSMUSC
!
