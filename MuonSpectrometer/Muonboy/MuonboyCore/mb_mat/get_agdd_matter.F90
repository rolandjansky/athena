!
!> \author M.Virchaux
!
 SUBROUTINE Get_AGDD_Matter(NameAGDDTop, NbFLAG)
 USE M_MB_Control, ONLY : MBPri
 USE M_MB_Xml
 IMPLICIT NONE
 CHARACTER(len=*), INTENT(IN)  :: NameAGDDTop
 INTEGER,          INTENT(OUT) :: NbFLAG
 CHARACTER(len=80) :: NameCurTop
 INTEGER           :: NbCurRemove,Iset,Istatus
!
   CALL Get_Xml_Branch_Top( NameCurTop , NbCurRemove )
!
!>> If not alreday done, write out matter in a temporary AGDD xml file
!>> from the amdc COMMON's  and  read in this AGDD xml file
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
!>> Set top volume of the branch of the tree to put into MB matter
   CALL Set_Xml_Branch_Top( NameAGDDTop , 0 )
!
!>> Put all the volumes of the above branch of tree into MB matter
!>>  Tree depth <----+  +----> MB matter (1) or MB general volumes (0)
!>>                  |  |    +---> returned as > 0 if volumes of the above branch actually exist
!>>                  |  |    |
   CALL PUT_XML( 32, 1, NbFLAG )
!
!>> Reset branch top volume 
   CALL Set_Xml_Branch_Top( NameCurTop , NbCurRemove )
!
   IF( NbFLAG > 0 ) THEN
     IF( MBPri >= 1 ) WRITE(*,*) ' XML matter : ',TRIM(NameAGDDTop),' loaded'
   ELSE
     WRITE(*,*) ' XML matter : ',TRIM(NameAGDDTop),' not found'
   ENDIF
!
 END SUBROUTINE Get_AGDD_Matter
!
