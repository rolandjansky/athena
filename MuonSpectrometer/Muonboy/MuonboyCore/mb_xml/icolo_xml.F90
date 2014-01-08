!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 INTEGER FUNCTION ICOLO_XML(CAR80)
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER(80) :: CAR80
 CHARACTER(80) :: ATAB(2)
 INTEGER       :: ITAB(5),Iobj,IELE,IA,II,IR
 REAL(8)       :: RTAB(5)
!
   ICOLO_XML = 0
   DO Iobj=1,Nb_xob
     IELE = Iele_xob(Iobj)
     IF( IELE < Iel0_xml_solid .OR. IELE > Iel1_xml_solid ) CYCLE
!>>    'name' ===================> 1
     CALL GIV_VAL_XML_ATT(Iobj,  1  , IA,II,IR,ATAB,ITAB,RTAB)
     IF( ATAB(1) /= CAR80 )                                 CYCLE
     ICOLO_XML = Icol_info(Info_xob(Iobj))
     EXIT
   ENDDO
!
 END
