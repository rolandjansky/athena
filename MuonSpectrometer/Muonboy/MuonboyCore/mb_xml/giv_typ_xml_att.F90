!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GIV_TYP_XML_ATT(IELE,IATT, CARAC,NCAR)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*) :: CARAC
 INTEGER           :: IELE,IATT,NCAR
!
   CALL GetXmlCar(Ibeg_xml_typ(IELE,IATT),Iend_xml_typ(IELE,IATT),CARAC,NCAR)
!
 END
