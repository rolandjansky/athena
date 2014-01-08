!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE XML_IND(Iobj,Jrep, INDEX)
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Iobj,Jrep
 INTEGER, INTENT(OUT) :: INDEX(Mxmli)
 CHARACTER (len=80) :: CAR80E,CAR80E2,CAR80A,CFIEL
 INTEGER            :: IELE,Iobj2,IELE2,IATT2,NCAR,IVALU,ISTEP,IND
 INTEGER            :: IA,II,IR,Iobj2_Beg,Iobj2_End,Iobj2_Sup
 CHARACTER (len=80) :: ATAB( 5)
 INTEGER            :: ITAB(10)
 REAL(8)            :: RTAB(10)
!
   INDEX(1:Mxmli) = -999999
   IELE = Iele_xob(Iobj)
   IF( IELE < Iel0_xml_sipos .OR. IELE > Iel1_xml_mupos ) RETURN
!
   IF( Iob_xob_inf1(Iobj) > 0 ) THEN
!>> ...... fix for bug in the Pacific_Sierra F90 Linux compiler ......
     Iobj2_Beg = Iob_xob_inf0(Iobj)
     Iobj2_End = Iob_xob_inf1(Iobj)
     DO Iobj2=Iobj2_Beg,Iobj2_End
       Iobj2_Sup = Iob_xob_sup(Iobj2)
       IF( Iobj2_Sup /= Iobj ) CYCLE
!>> ..................................................................
       IELE2 = Iele_xob(Iobj2)
       IF( IELE2 < Iel0_xml_ident .OR. IELE2 > Iel1_xml_ident ) THEN
         CALL GIV_NAM_XML_ELE(IELE, CAR80E ,NCAR)
         CALL GIV_NAM_XML_ELE(IELE2,CAR80E2,NCAR)
         PRINT 7772,CAR80E2,CAR80E
7772     FORMAT(/' An XML element of type         : ',A80,    &
                /' is embedded in another of type : ',A80,    &
                /' ===============================> One STOPs !!!')
         STOP
       ENDIF
       CFIEL = ''
       IVALU = 0
       ISTEP = 0
       DO IATT2=1,Nb_xml_att(IELE2)
         CALL GIV_NAM_XML_ATT(IELE2,IATT2, CAR80A,NCAR)
         CALL GIV_VAL_XML_ATT(Iobj2,IATT2, IA,II,IR,ATAB,ITAB,RTAB)
         IF(     CAR80A == 'field' ) THEN
           CFIEL = ATAB(1)
         ELSEIF( CAR80A == 'value' ) THEN
           IVALU = ITAB(1)
         ELSEIF( CAR80A == 'step'  ) THEN
           ISTEP = ITAB(1)
         ENDIF
       ENDDO
       DO IND=1,Nb_xob_ind
         CALL GIV_NAM_XML_IND( IND, CAR80A )
         IF( CAR80A == CFIEL ) GO TO 100
       ENDDO
       CALL ADD_XML_IND(CFIEL)
       IND = Nb_xob_ind
100    INDEX(IND) = IVALU + (Jrep-1)*ISTEP
     ENDDO
   ENDIF
!
 END
