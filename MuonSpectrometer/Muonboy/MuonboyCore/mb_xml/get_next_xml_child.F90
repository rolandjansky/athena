!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Get_Next_Xml_Child( IobjParent, IobjChild )
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: IobjParent
 INTEGER, INTENT(INOUT) :: IobjChild
 INTEGER  :: Ir,Ii,Iref
!
   IF( IobjParent < 1 ) THEN
     IobjChild = IobjChild + 1
     RETURN
   ENDIF
!
   Ir = 999999999
   DO Iref=1,Mxmlr
     IF( IobjChild < Iob_xob_ref(IobjParent,Iref) ) THEN
       Ir = Iob_xob_ref(IobjParent,Iref)
       EXIT
     ENDIF
   ENDDO
!
   Ii = 999999999
   IF(     IobjChild < Iob_xob_inf0(IobjParent) ) THEN
     Ii = Iob_xob_inf0(IobjParent)
   ELSEIF( IobjChild < Iob_xob_inf1(IobjParent) ) THEN
     Ii = IobjChild + 1
   ENDIF
!
   IobjChild = MIN( Ir , Ii )
!
 END
