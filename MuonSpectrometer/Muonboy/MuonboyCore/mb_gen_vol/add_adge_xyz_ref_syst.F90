!
 SUBROUTINE ADD_ADGE_XYZ_Ref_Syst(X0,Y0,Z0,Iaddress)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER  :: Iaddress
 REAL(8)  :: X0, Y0, Z0
!
   SELECT CASE( I_Atlas_Ref_Syst )
   CASE( 1 )
      CALL ADD_ADGE_XYZ( X0,-Z0, Y0 )
   CASE DEFAULT
      CALL ADD_ADGE_XYZ( X0, Y0, Z0 )
   END SELECT
   Iaddress = Nadge
!
 END SUBROUTINE ADD_ADGE_XYZ_Ref_Syst
!
