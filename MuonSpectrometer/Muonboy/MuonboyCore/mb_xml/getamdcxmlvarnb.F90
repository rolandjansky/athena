!
! Author : M.Virchaux
!
 SUBROUTINE GetAmdcXmlVarNb( NbVar )
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: NbVar
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
   NbVar = Nb_xml_var
!
 END SUBROUTINE GetAmdcXmlVarNb
!
