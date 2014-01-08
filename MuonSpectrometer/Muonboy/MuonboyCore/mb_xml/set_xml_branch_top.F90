!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Set_Xml_Branch_Top(Name,NbRemove)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: NbRemove
!
   Nam_xml_vis(1) = Name
   Nb_xml_vis     = 1 + NbRemove
   IF( Name == '' ) Nb_xml_vis = 0
!
 END
