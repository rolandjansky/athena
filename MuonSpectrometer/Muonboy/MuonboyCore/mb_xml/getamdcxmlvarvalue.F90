!
! Author : M.Virchaux
!
 SUBROUTINE GetAmdcXmlVarValue( NameVar, Iok,Value )
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN)  :: NameVar
 INTEGER,           INTENT(OUT) :: Iok
 REAL(8),           INTENT(OUT) :: Value
 INTEGER :: Iset,Istatus
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
   CALL GetVar_MB_XML( NameVar, Iok,Value )
!
 END SUBROUTINE GetAmdcXmlVarValue
!
