!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE PRINT_XOB( IOBJ )
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IOBJ
 INTEGER             :: L,NrefMax,IELE,IATT,IAD0,IAD1,NCAR
 CHARACTER (len=10)  :: CAR10
 CHARACTER (len=20)  :: CAR20
 CHARACTER (len=80)  :: CAR80
 CHARACTER (len=160) :: CAR160
!
   IELE = Iele_xob(IOBJ)
   PRINT 1100,(Xml_car(L),L=Ibeg_xml_ele(IELE),Iend_xml_ele(IELE))
1100 FORMAT(/10X,80A1)
   NrefMax = 0
   DO L=1,Mxmlr
     IF( Iob_xob_ref(IOBJ,L) > 0 ) NrefMax = L
   ENDDO
   IF( Iob_xob_inf1(IOBJ) < 1 ) THEN
     IF( NrefMax < 1 ) THEN
       PRINT 1200,Iob_xob_sup(IOBJ),IOBJ
1200   FORMAT(I5,' ==>',I5)
     ELSE
       PRINT 1201,Iob_xob_sup(IOBJ),IOBJ,Iob_xob_ref(IOBJ,1:NrefMax)
1201   FORMAT(I5,' ==>',I5,      17X,         10X,'ref.==>',12I5)
     ENDIF
   ELSE
     IF( NrefMax < 1 ) THEN
       PRINT 1210,Iob_xob_sup(IOBJ),IOBJ,Iob_xob_inf0(IOBJ),Iob_xob_inf1(IOBJ)
1210   FORMAT(I5,' ==>',I5,' ==>',I5,' to',I5)
     ELSE
       PRINT 1211,Iob_xob_sup(IOBJ),IOBJ,Iob_xob_inf0(IOBJ),Iob_xob_inf1(IOBJ),Iob_xob_ref(IOBJ,1:NrefMax)
1211   FORMAT(I5,' ==>',I5,' ==>',I5,' to',I5,10X,'ref.==>',12I5)
     ENDIF
   ENDIF
   DO IATT=1,Nb_xml_att(IELE)
     CALL GIV_NAM_XML_ATT(IELE,IATT, CAR80,NCAR)
     CAR20 = CAR80(1:20)
     CALL GIV_TYP_XML_ATT(IELE,IATT, CAR80,NCAR)
     CAR10 = CAR80(1:10)
     IAD0 = Ibeg_xob_att(IOBJ,IATT)
     IAD1 = Iend_xob_att(IOBJ,IATT)
     IF( IAD1 < 1 ) THEN
       IAD0 = Ibeg_xml_def(IELE,IATT)
       IAD1 = Iend_xml_def(IELE,IATT)
     ENDIF
     CALL GetXmlCar(IAD0,IAD1, CAR160,NCAR)
     IF( IATT == 1 ) THEN
       PRINT 1300,CAR20,CAR10,TRIM(CAR160)
1300   FORMAT(10X,'*****   ',A20,A10,' = ',A)
     ELSE
       PRINT 1310,CAR20,CAR10,TRIM(CAR160)
1310   FORMAT(18X,           A20,A10,' = ',A)
     ENDIF
   ENDDO
!
 END
