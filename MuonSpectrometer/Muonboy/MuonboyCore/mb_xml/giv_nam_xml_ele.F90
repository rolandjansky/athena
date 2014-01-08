!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GIV_NAM_XML_ELE(IELE, CARAC,NCAR)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*) :: CARAC
 INTEGER           :: IELE,NCAR
!
   CALL GetXmlCar(Ibeg_xml_ele(IELE),Iend_xml_ele(IELE), CARAC,NCAR)
!
 END
