!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE CREAT_XML_COMPACT(Iobj)
 USE M_MB_Xml
 IMPLICIT NONE
!
!>>  Create the generic volume coresponding to the XML "compact" object Iobj
!
 INTEGER, INTENT(IN) :: Iobj
 CHARACTER (len=80) :: Name,Envel,Type
 INTEGER            :: IobjPre,Iobj1,Iele1
!
   IobjPre = Iob_xob_sup(Iobj)
   CALL GET_COMPACT_INFO(IobjPre, Name,Envel,Type,Iobj1,Iele1)
   IF( Iobj /= Iobj1 ) THEN
     PRINT 7770,TRIM(Name),TRIM(Envel),TRIM(Type),Iobj,Iobj1
7770 FORMAT(/' Fatal error in CREAT_XML_COMPACT :',/             &
             ' For :  Name = ',A,'  Envel = ',A,'  Type = ',A,/  &
             ' One has :  Iobj,Iobj1 =',2I7,'  =====>  STOP !')
     STOP
   ENDIF
!
   IF(     Type == 'MDT_Chamber' ) THEN
     CALL CREAT_XML_MDT_Chamber(Iobj)
!* ELSEIF( Type == 'RPC_Chamber' ) THEN
!*   CALL CREAT_XML_RPC_Chamber(Iobj)
!* ELSEIF( ...
!*   CALL ...
   ELSE
     PRINT 7771,Iobj,TRIM(Name),TRIM(Type)
7771 FORMAT(/' Method CREAT_XML_COMPACT was called for the xml object',I7,' of Name : ',A,/   &
             ' which is of an UNKNOWN xml compact type : ',A,' ???')
   ENDIF
!
 END
