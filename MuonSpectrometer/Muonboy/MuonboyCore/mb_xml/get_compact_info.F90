!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE GET_COMPACT_INFO(Iobj, Name,Envelope,Type,IobjC,IeleC)
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER,           INTENT(IN)  :: Iobj
 CHARACTER (len=*), INTENT(OUT) :: Name,Envelope,Type
 INTEGER,           INTENT(OUT) :: IobjC,IeleC
 INTEGER :: Iele,NCAR
!
   Iele = Iele_xob(Iobj)
   IF( Iele /= Iel1_xml_cmpac ) THEN
     PRINT 7771,Iobj
7771 FORMAT(/' GET_COMPACT_INFO was called for the xml object',I6,' which is not a proper compact element  =====>  STOP !')
     STOP
   ENDIF
   IF( Iob_xob_inf0(Iobj) /= Iob_xob_inf1(Iobj) .OR. Iob_xob_inf0(Iobj) < 1 ) THEN
     PRINT 7772,Iobj,Iob_xob_inf0(Iobj),Iob_xob_inf1(Iobj)
7772 FORMAT(/' The xml object',I6,' which is a compact element, do not have a single child (',2I5,')  =====>  STOP !')
     STOP
   ENDIF
!
   CALL GIV_XML_ATT(Iobj,     'name', Name )
   CALL GIV_XML_ATT(Iobj, 'envelope', Envelope )
!
   IobjC    = Iob_xob_inf0(Iobj)
   IeleC    = Iele_xob(IobjC)
   CALL GIV_NAM_XML_ELE(IeleC, Type,NCAR)
!
 END
