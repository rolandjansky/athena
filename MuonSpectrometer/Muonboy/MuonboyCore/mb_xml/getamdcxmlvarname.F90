!
! Author : M.Virchaux
!
 SUBROUTINE GetAmdcXmlVarName( IVar, NameVar )
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER,          INTENT(IN)    :: IVar
 CHARACTER(len=*), INTENT(INOUT) :: NameVar
 INTEGER :: Leng,Iset,Istatus
!
   Iset    = 0
   Istatus = 0
   CALL AgddReadStatus( Iset, Istatus )
   IF( Current_xml_file /= 'Lun=19' .OR. Istatus < 1 ) THEN
     CALL Read_MB_Xml( 19 )
     Iset    = 1
     Istatus = 1
     CALL AgddReadStatus( Iset, Istatus )
   ENDIF
!
   NameVar= ''
   IF( IVar < 1 .OR. IVar > Nb_xml_var ) RETURN
   CALL GetXmlCar( Ipos_xml_var(1,IVar),Ipos_xml_var(2,IVar), NameVar,Leng )
!
 END SUBROUTINE GetAmdcXmlVarName
!
