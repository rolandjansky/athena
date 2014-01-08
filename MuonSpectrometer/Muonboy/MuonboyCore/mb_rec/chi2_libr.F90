!
!> \author M.Virchaux
!
 REAL(8) FUNCTION Chi2_Libr(L)
 USE M_MB_PACONS
 USE M_MB_Fit
 IMPLICIT None
 INTEGER, INTENT(IN) :: L
 REAL(8) :: XDENOM
!                                    
   XDENOM = NZLIBR(L,1,1) + NZLIBR(L,2,1) + NSLIBR(L,1,1)*FacSZ + NSLIBR(L,2,1)*FacSZ - 1.D0
   XDENOM = MAX( XDENOM , 1.D0 )
   Chi2_Libr = ( DZLIBR(L,1,1) + DZLIBR(L,2,1) + DSLIBR(L,1,1) + DSLIBR(L,2,1) ) / XDENOM
!
 END FUNCTION Chi2_Libr
!
