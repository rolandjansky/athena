!
!> \author SaMuSoG
!
 SUBROUTINE DEF_GcylRZ_NRZDF( Name, Jcolor, NRZ, RDR, ZDZ, Phi0,dPhi,Nphi )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN) :: Name
 INTEGER,          INTENT(IN) :: Jcolor,NRZ,Nphi
 REAL(8),          INTENT(IN) :: RDR(*),ZDZ(*),Phi0,dPhi
 INTEGER :: J,J_Atlas_Ref_Syst,Jop(2),NI(NRZ)
 REAL(8) :: Ang1,Ang2,YI(NRZ)
!
   NI(1:NRZ) = 1 
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   YI(1:NRZ) = Zero
   DO J=1,Nphi
    Ang1 = ( Phi0 + (J-1)*dPhi/Nphi ) * PiS180
    Ang2 = ( Phi0 +   J  *dPhi/Nphi ) * PiS180
    IF( J_Atlas_Ref_Syst >= 1 ) THEN
       CALL DEFPLX( Ang1, Jcolor, NRZ, NI, RDR,ZDZ,YI )
       CALL DEFPLX( Ang2, Jcolor, NRZ, NI, RDR,ZDZ,YI )
    ELSE
       CALL DEFPLX( Ang1, Jcolor, NRZ, NI, RDR,YI,ZDZ )
       CALL DEFPLX( Ang2, Jcolor, NRZ, NI, RDR,YI,ZDZ )
    ENDIF
    Jop(1:2) = 0
    IF( J ==  1   ) Jop(1) = 1
    IF( J == Nphi ) Jop(2) = 1
    CALL CLOSE_OPEN( Jcolor, Jop(1),Jop(2), 1 )
   ENDDO
   CALL ENDVOL_PUR
   CALL ENDSUV
!
 END SUBROUTINE DEF_GcylRZ_NRZDF
