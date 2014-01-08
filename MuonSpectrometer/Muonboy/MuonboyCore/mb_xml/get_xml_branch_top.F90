!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Get_Xml_Branch_Top(Name,NbRemove)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(OUT) :: Name
 INTEGER,           INTENT(OUT) :: NbRemove
!
   Name     = ''
   NbRemove = 0
   IF( Nb_xml_vis > 0 ) THEN
     Name = Nam_xml_vis(1)
     NbRemove = Nb_xml_vis - 1
   ENDIF
!
 END
