!
!> Get  :  VDEB(1 to 8)
!> From :  ISCI,X1,Y1,Z1,X2,Y2,Z2,DS1,DS2,DZT1,DZT2,CFI,SFI,CFA,SFA,SMU
! \author M.Virchaux
!
 SUBROUTINE GIVDEB1(IVDEB)
 USE M_MB_MuGeom
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IVDEB
 REAL(8) :: XA, YA, ZA, XB, YB, ZB, OBMA, ASMU
!
   IF(ISCI <= NCOUBAS2) THEN
     XA = X1 - SFI * DS1
     YA = Y1 + CFI * DS1
     ZA = Z1 + DZT1
     XB = X2 - SFI * DS2
     YB = Y2 + CFI * DS2
     ZB = Z2 + DZT2
   ELSEIF(ISCI <= NMATTR) THEN
     XA = X1 - SFI * DS1 + CFI * DZT1
     YA = Y1 + CFI * DS1 + SFI * DZT1
     ZA = Z1
     XB = X2 - SFI * DS2 + CFI * DZT2
     YB = Y2 + CFI * DS2 + SFI * DZT2
     ZB = Z2
   ELSE
     XA = X1 - SFI * DS1 + CFI * CFA * DZT1
     YA = Y1 + CFI * DS1 + SFI * CFA * DZT1
     ZA = Z1                   - SFA * DZT1
     XB = X2 - SFI * DS2 + CFI * CFA * DZT2
     YB = Y2 + CFI * DS2 + SFI * CFA * DZT2
     ZB = Z2                   - SFA * DZT2
   ENDIF
   OBMA = 1.d0 / SQRT( (XB-XA)**2 + (YB-YA)**2 + (ZB-ZA)**2 )
   VDEB(4,IVDEB) = (XB-XA) * OBMA
   VDEB(5,IVDEB) = (YB-YA) * OBMA
   VDEB(6,IVDEB) = (ZB-ZA) * OBMA
   VDEB(1,IVDEB) = XA - VDEB(4,IVDEB)
   VDEB(2,IVDEB) = YA - VDEB(5,IVDEB)
   VDEB(3,IVDEB) = ZA - VDEB(6,IVDEB)
   ASMU = MAX( ABS(SMU), 0.000001d0 )
   VDEB(7,IVDEB) = 100.d0 / ASMU
   VDEB(8,IVDEB) = SIGN( 1.d0, SMU )
!
 END SUBROUTINE GIVDEB1
!
