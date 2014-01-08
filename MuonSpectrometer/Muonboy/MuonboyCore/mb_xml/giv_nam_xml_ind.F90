!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GIV_NAM_XML_IND(INDE, CARAC)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*) :: CARAC
 INTEGER           :: INDE,NCAR
!
   CALL GetXmlCar(Ibeg_xob_ind(INDE),Iend_xob_ind(INDE), CARAC,NCAR)
!
 END
