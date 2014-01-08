!
 SUBROUTINE XML_ROT(Iobj, ROT)
 USE M_MB_Xml
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Iobj
 REAL(8), INTENT(OUT) :: ROT(3,3)
 REAL(8), PARAMETER :: EpsLoc = 0.000001D0
 CHARACTER (len=80) :: NameEle,NameAtt
 INTEGER            :: IELE,Irot,IA,II,IR,Iok,NCAR
 REAL(8)            :: SINA,COSA,AngX,AngY,AngZ
 REAL(8)            :: RXT(3,3),RYT(3,3),RZT(3,3)
 CHARACTER (len=80) :: ATAB( 5)
 INTEGER            :: ITAB(10)
 REAL(8)            :: RTAB(10)
 CHARACTER (len=1)  :: CharSym
!>>                               | |            
!>>                     column <--+ +--> line            
!>>
!>>                         1,1   2,1   3,1
!>>                         1,2   2,2   3,2
!>>                         1,3   2,3   3,3
!
   ROT(1:3,1:3) = 0.D0
   ROT( 1 , 1 ) = 1.D0
   ROT( 2 , 2 ) = 1.D0
   ROT( 3 , 3 ) = 1.D0
!
   IELE = Iele_xob(Iobj)
   IF( IELE < Iel0_xml_sipos .OR. IELE > Iel1_xml_mupos ) RETURN
!
   ATAB(1) = ''
   CALL GIV_VAL_XML_ATT(Iobj,'sym', IA,II,IR,ATAB,ITAB,RTAB)
   NameAtt = ADJUSTL(ATAB(1))
   CharSym = NameAtt(1:1)
   IF(     CharSym == 'X' .OR. CharSym == 'x' ) THEN
     ROT(1,1) = -1.D0
   ELSEIF( CharSym == 'Y' .OR. CharSym == 'y' ) THEN
     ROT(2,2) = -1.D0
   ELSEIF( CharSym == 'Z' .OR. CharSym == 'z' ) THEN
     ROT(3,3) = -1.D0
   ENDIF
!
   RTAB(1:9) = 0.D0
   CALL GIV_VAL_XML_ATT(Iobj,'rot', IA,II,IR,ATAB,ITAB,RTAB)
   IF( IR /= 3 )                                          RETURN
   IF( SUM( ABS(RTAB(1:3)) ) < EpsLoc ) THEN
     CALL GIV_VAL_XML_ATT(Iobj,'rotMat', IA,II,IR,ATAB,ITAB,RTAB)
     IF( IR /= 9 )                                        RETURN
     IF( SUM( ABS(RTAB(1:9)) ) < EpsLoc )                 RETURN
     RXT(1:3,1) = RTAB(1:3)
     RXT(1:3,2) = RTAB(4:6)
     RXT(1:3,3) = RTAB(7:9)
     CALL GetAnglesFromRotMat( RXT, Iok, AngX,AngY,AngZ )
     IF( Iok < 1 ) THEN
       CALL GIV_NAM_XML_ELE(IELE, NameEle,NCAR)
       CALL GIV_XML_ATT(Iobj,'volume', NameAtt )
       PRINT 7771,TRIM(NameEle),TRIM(NameAtt),RXT(1:3,1),RXT(1:3,2),RXT(1:3,3)
7771   FORMAT(/'>>> An XML Element <',A,'>, positionning the Volume : ',A,    &
              /'>>> has an INVALID rotation Matrix :',3F13.8,                 &
              /'>>>                                 ',3F13.8,                 &
              /'>>>                                 ',3F13.8,                 &
              /'>>> ==========> It is IGNORED !!!'/)
     ELSE
       ROT(1:3,1:3) = RXT(1:3,1:3)
     ENDIF
   ELSE
     Irot = 0
     IF( ABS(RTAB(1)) > EpsLoc ) THEN
       Irot = Irot + 1
       SINA = SIN(RTAB(1)*PIS180)
       COSA = COS(RTAB(1)*PIS180)
       RXT(1:3  ,1:3) = 0.D0
       RXT( 1   , 1 ) = 1.D0
       RXT(2:3  , 2 ) = (/  COSA , -SINA /)
       RXT(2:3  , 3 ) = (/  SINA ,  COSA /)
     ENDIF
     IF( ABS(RTAB(2)) > EpsLoc ) THEN
       Irot = Irot + 10
       SINA = SIN(RTAB(2)*PIS180)
       COSA = COS(RTAB(2)*PIS180)
       RYT(1:3  ,1:3) = 0.D0
       RYT( 2   , 2 ) = 1.D0
       RYT(1:3:2, 1 ) = (/  COSA ,  SINA /)
       RYT(1:3:2, 3 ) = (/ -SINA ,  COSA /)
     ENDIF
     IF( ABS(RTAB(3)) > EpsLoc ) THEN
       Irot = Irot + 100
       SINA = SIN(RTAB(3)*PIS180)
       COSA = COS(RTAB(3)*PIS180)
       RZT(1:3  ,1:3) = 0.D0
       RZT( 3   , 3 ) = 1.D0
       RZT(1:2  , 1 ) = (/  COSA , -SINA /)
       RZT(1:2  , 2 ) = (/  SINA ,  COSA /)
     ENDIF
     IF(     Irot ==   1 ) THEN
       ROT(1:3,1:3) = RXT(1:3,1:3)
     ELSEIF( Irot ==  10 ) THEN
       ROT(1:3,1:3) = RYT(1:3,1:3)
     ELSEIF( Irot == 100 ) THEN
       ROT(1:3,1:3) = RZT(1:3,1:3)
     ELSEIF( Irot ==  11 ) THEN
       CALL MULT3M3M(RYT,RXT, ROT)
     ELSEIF( Irot == 101 ) THEN
       CALL MULT3M3M(RZT,RXT, ROT)
     ELSEIF( Irot == 110 ) THEN
       CALL MULT3M3M(RZT,RYT, ROT)
     ELSEIF( Irot == 111 ) THEN
       CALL MULT3M3M(RZT,RYT, ROT)
       CALL MULT3M3M(ROT,RXT, RZT)
       ROT(1:3,1:3) = RZT(1:3,1:3)
     ENDIF
   ENDIF
!
   IF(     CharSym == 'X' .OR. CharSym == 'x' ) THEN
     ROT(1:3,1) = - ROT(1:3,1)
   ELSEIF( CharSym == 'Y' .OR. CharSym == 'y' ) THEN
     ROT(1:3,2) = - ROT(1:3,2)
   ELSEIF( CharSym == 'Z' .OR. CharSym == 'z' ) THEN
     ROT(1:3,3) = - ROT(1:3,3)
   ENDIF
!
!****      CALL GetAnglesFromRotMat( ROT, Iok, AngX,AngY,AngZ )
!****      PRINT 7777,Iok,RTAB(1:3),AngX/PIS180,AngY/PIS180,AngZ/PIS180
!****7777  FORMAT(/'>>> GetAnglesFromRotMat :',I3,3F12.6/
!****     +        '>>>                         ',3F12.6)
!
 END SUBROUTINE XML_ROT
!
