!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ADD_XML_IND(Name)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
!
!>> Increase XML indexes number
   CALL Increm_Index(Nb_xob_ind,Mxmli,'Mxmli')
!
!>> XML index name
   CALL SetXmlCar( Name,Ibeg_xob_ind(Nb_xob_ind),Iend_xob_ind(Nb_xob_ind) )
!
 END
