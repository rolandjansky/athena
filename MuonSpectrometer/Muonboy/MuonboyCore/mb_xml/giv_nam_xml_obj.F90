!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GIV_NAM_XML_OBJ( Iobj, Name )
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*)  :: Name
 CHARACTER (len=80) :: NameC,EnveC,TypeC
 CHARACTER (len=80) :: ATAB( 5)
 INTEGER            :: ITAB(10),Iobj,Iele,IobjPre,IA,II,IR
 REAL(8)            :: RTAB(10)
!
   Name = '?'
   Iele = Iele_xob(Iobj)
   IF( Iele >= Iel0_xml_cmpac .AND. Iele < Iel1_xml_cmpac ) THEN
!>> specific-compact
     IobjPre = Iob_xob_sup(Iobj)
     CALL GET_COMPACT_INFO(IobjPre, NameC,EnveC,TypeC,IA,II)
     Name = 'CMPAC(' // TRIM(TypeC) // ')' // TRIM(NameC)
   ELSEIF( Iele >  Iel0_xml_additional ) THEN
!>> unknown specific-compact
     IobjPre = Iob_xob_sup(Iobj)
     CALL GET_COMPACT_INFO(IobjPre, NameC,EnveC,TypeC,IA,II)
     Name = 'CMPAC_UNKNOWN(' // TRIM(TypeC) // ')'
   ELSEIF( Iele >= Iel0_xml_sipos .AND. Iele <= Iel1_xml_mupos ) THEN
!>> single  or  multiple  positionning
     CALL GIV_VAL_XML_ATT(Iobj,'volume', IA,II,IR,ATAB,ITAB,RTAB)
     IF( IA == 1 ) Name = ATAB(1)
   ELSE
!>> section  or  solid  or  proper-compact  or  composition  or  stack  or  variable  or ...
     CALL GIV_VAL_XML_ATT(Iobj,'name', IA,II,IR,ATAB,ITAB,RTAB)
     IF( IA == 1 ) Name = ATAB(1)
   ENDIF
!
 END
