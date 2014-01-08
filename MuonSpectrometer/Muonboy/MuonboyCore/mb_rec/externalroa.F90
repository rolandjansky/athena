!-----------------------------------------------------
 SUBROUTINE ReSetExtROA
 USE M_MB_External_ROA
 IMPLICIT NONE
 
 NberExtROA = 0

 END SUBROUTINE ReSetExtROA

!-----------------------------------------------------
 SUBROUTINE LoadExtROA( ExtROATheIN, ExtROAPhiIN, ExtROAR3dIN )
 USE M_MB_Control, ONLY : MBPri
 USE M_MB_External_ROA
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: ExtROATheIN
 REAL(8), INTENT(IN) :: ExtROAPhiIN
 REAL(8), INTENT(IN) :: ExtROAR3dIN

 IF( NberExtROA >= MaxExtROA ) THEN
   IF( MBPri >= 1 ) WRITE(*,*) ' MuonboyCore/LoadExtRoa : maximum number of external roa reached '
   RETURN
 ENDIF
 NberExtROA = NberExtROA + 1
 ExtROAThe(NberExtROA) = ExtROATheIN
 ExtROAPhi(NberExtROA) = ExtROAPhiIN
 ExtROAR3d(NberExtROA) = ExtROAR3dIN
 ExtROAWei(NberExtROA) = 1.D0 
 
 END SUBROUTINE LoadExtROA
