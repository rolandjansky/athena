!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GIV_XML_OBJ_INFO(Iobj,Info)
 USE M_MB_Increm_Index
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER :: Iobj,Info
!
   Info = Info_xob(Iobj)
   IF( Info <= 0 ) THEN
     CALL Increm_Index(Nb_info,Mxmlf,'Mxmlf')
     Info_xob(Iobj) = Nb_info
     Info = Nb_info
   ENDIF
!
 END
