!
!> \author M.Virchaux
!
 SUBROUTINE INIMAT
 USE M_MB_Control
 USE M_MB_COMATI
 IMPLICIT NONE
 REAL(8), PARAMETER :: BIGBIG=999999999.D0, BIG=999000000.D0, EPS=0.0001D0, PETIT=0.00001D0
 INTEGER, SAVE :: Iwarning=0
 REAL(8) :: XYZ1(3), XYZ2(3), XYZ3(3)
 REAL(8) :: X, Y, ANOR3V
 INTEGER :: IV, IP, IE, IE1, IE2, IE3, I
!
   XXMATI(1:NEMATI) = BIGBIG
   YYMATI(1:NEMATI) = BIGBIG
   NVPOSMATI(:) = 0
   NVNEGMATI(:) = 0
   NVCENMATI(:) = 0
!
   DO IV=1,NVMATI
     XMIMATI(IV) =  BIGBIG
     YMIMATI(IV) =  BIGBIG
     ZMIMATI(IV) =  BIGBIG
     XMAMATI(IV) = -BIGBIG
     YMAMATI(IV) = -BIGBIG
     ZMAMATI(IV) = -BIGBIG
     DO IP=NPMAT0(IV),NPMAT1(IV)
       IE1 = NEMAT0(IP)
       IE2 = IE1 + 1
       IE3 = IE1 + 2
       CALL GETXYZMAT( IV, IE1, XYZ1 )
       CALL GETXYZMAT( IV, IE2, XYZ2 )
       VXMATI(1:3,IP) = XYZ2(1:3) - XYZ1(1:3)
       IF( ABS(VXMATI(1,IP)) < PETIT ) VXMATI(1,IP)=0.D0
       IF( ABS(VXMATI(2,IP)) < PETIT ) VXMATI(2,IP)=0.D0
       IF( ABS(VXMATI(3,IP)) < PETIT ) VXMATI(3,IP)=0.D0
       IF( SQRT( SUM( VXMATI(1:3,IP)**2 ) ) < PETIT ) THEN
! known problem with new shielding with two z positions identicals
         VXMATI(3,IP) = 1.0d0
         IF( MBPri >= 6 ) THEN
           WRITE(*,*) ' probleme INIMAT iv ip ',IV,IP
           DO IE=NEMAT0(IP),NEMAT1(IP)
             CALL GETXYZMAT( IV, IE, XYZ3 )
             WRITE(*,*) ' iv,ip,ie xyz ',iv,ip,ie,xyz3(1:3)
           ENDDO
         ENDIF
       ENDIF
       CALL NORM3V( VXMATI(1,IP) )
10     CALL GETXYZMAT( IV, IE3, XYZ3 )
       VYMATI(1:3,IP) = XYZ3(1:3) - XYZ2(1:3)
       IF( ABS(VYMATI(1,IP)) < PETIT ) VYMATI(1,IP)=0.D0
       IF( ABS(VYMATI(2,IP)) < PETIT ) VYMATI(2,IP)=0.D0
       IF( ABS(VYMATI(3,IP)) < PETIT ) VYMATI(3,IP)=0.D0
       CALL PVEC3V( VXMATI(1,IP), VYMATI(1,IP), VZMATI(1,IP) )
       IF( ANOR3V(VZMATI(1,IP)) < EPS ) THEN
         IE3 = IE3 + 1
         IF( IE3 > NEMAT1(IP) ) THEN
           PRINT '(/" In MuonBoy : Fatal Problem in INIMAT  =====>  STOP !")'
           IF( MBPri >= 6 ) THEN
             WRITE(*,*) 'ANOR3V(VZMATI(1,IP)) ',ANOR3V(VZMATI(1,IP))
             WRITE(*,*) ' probleme INIMAT iv ip ',IV,IP
             DO IE=NEMAT0(IP),NEMAT1(IP)
               CALL GETXYZMAT( IV, IE, XYZ3 )
               WRITE(*,*) ' iv,ip,ie xyz ',iv,ip,ie,xyz3(1:3)
             ENDDO
           ENDIF
           STOP
         ENDIF
         GO TO 10
       ENDIF
       CALL NORM3V( VZMATI(1,IP) )
       CALL PVEC3V( VZMATI(1,IP), VXMATI(1,IP), VYMATI(1,IP) )
       VXMATI(4,IP) = DOT_PRODUCT( XYZ1, VXMATI(1:3,IP) )
       VYMATI(4,IP) = DOT_PRODUCT( XYZ1, VYMATI(1:3,IP) )
       VZMATI(4,IP) = DOT_PRODUCT( XYZ1, VZMATI(1:3,IP) )
       DO IE=NEMAT0(IP),NEMAT1(IP)
         CALL GETXYZMAT( IV, IE, XYZ1 )
         XMIMATI(IV) = MIN( XMIMATI(IV) , XYZ1(1) )
         YMIMATI(IV) = MIN( YMIMATI(IV) , XYZ1(2) )
         ZMIMATI(IV) = MIN( ZMIMATI(IV) , XYZ1(3) )
         XMAMATI(IV) = MAX( XMAMATI(IV) , XYZ1(1) )
         YMAMATI(IV) = MAX( YMAMATI(IV) , XYZ1(2) )
         ZMAMATI(IV) = MAX( ZMAMATI(IV) , XYZ1(3) )
         X = DOT_PRODUCT( XYZ1, VXMATI(1:3,IP) ) - VXMATI(4,IP)
         Y = DOT_PRODUCT( XYZ1, VYMATI(1:3,IP) ) - VYMATI(4,IP)
         IF( ABS(X) < PETIT ) X=0.D0
         IF( ABS(Y) < PETIT ) Y=0.D0
         IF( XXMATI(IE) > BIG ) THEN
           XXMATI(IE) = X
           YYMATI(IE) = Y
         ELSE
           IF( ABS(XXMATI(IE)-X) > EPS .OR. ABS(YYMATI(IE)-Y) > EPS ) THEN
             Iwarning = Iwarning + 1
             IF(Iwarning <= 100) PRINT 1000,X,XXMATI(IE),Y,YYMATI(IE)
1000         FORMAT(' In MuonBoy INIMAT : X,XXMATI,Y,YYMATI =',4F12.4)
           ENDIF
         ENDIF
       ENDDO
     ENDDO
     DO I=1,4
       IF( I == 1 .AND. ( XMIMATI(IV) >  Xmat1 .OR. YMIMATI(IV) >  Ymat1 ) ) CYCLE  ! 1 = X-,Y-
       IF( I == 2 .AND. ( XMAMATI(IV) < -Xmat1 .OR. YMIMATI(IV) >  Ymat1 ) ) CYCLE  ! 2 = X+,Y-
       IF( I == 3 .AND. ( XMIMATI(IV) >  Xmat1 .OR. YMAMATI(IV) < -Ymat1 ) ) CYCLE  ! 3 = X-,Y+
       IF( I == 4 .AND. ( XMAMATI(IV) < -Xmat1 .OR. YMAMATI(IV) < -Ymat1 ) ) CYCLE  ! 4 = X+,Y+
       IF( ZMAMATI(IV) > Zmat1 ) THEN
         NVPOSMATI(I) = NVPOSMATI(I) + 1
         IF( NVPOSMATI(I) <= KVOLUmat ) IVPOSMATI(NVPOSMATI(I),I) = IV
         IF( ZMIMATI(IV) > Zmat2  ) CYCLE
       ENDIF
       IF( ZMIMATI(IV) < -Zmat1 ) THEN
         NVNEGMATI(I) = NVNEGMATI(I) + 1
         IF( NVNEGMATI(I) <= KVOLUmat ) IVNEGMATI(NVNEGMATI(I),I) = IV
         IF( ZMAMATI(IV) < -Zmat2 ) CYCLE
       ENDIF
       NVCENMATI(I) = NVCENMATI(I) + 1
       IF( NVCENMATI(I) <= KVOLUmat ) IVCENMATI(NVCENMATI(I),I) = IV
     ENDDO
   ENDDO
   DO I=1,4
     IF( NVPOSMATI(I) > KVOLUmat .OR. NVNEGMATI(I) > KVOLUmat .OR. NVCENMATI(I) > KVOLUmat ) THEN
       WRITE(*,*) ' FATAL error in MuonboyCore/inimat : sizes too small with KVOLUmat ',KVOLUmat
       WRITE(*,*) '               NVPOSMATI ',NVPOSMATI(:)
       WRITE(*,*) '               NVNEGMATI ',NVNEGMATI(:)
       WRITE(*,*) '               NVCENMATI ',NVCENMATI(:)
       NVPOSMATI(:) = MIN(NVPOSMATI(:),KVOLUmat)
       NVNEGMATI(:) = MIN(NVNEGMATI(:),KVOLUmat)
       NVCENMATI(:) = MIN(NVCENMATI(:),KVOLUmat)
       EXIT
     ENDIF
   ENDDO
!
 END SUBROUTINE INIMAT
!
