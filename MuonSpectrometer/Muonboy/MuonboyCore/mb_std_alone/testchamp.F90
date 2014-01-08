!
 SUBROUTINE testChamp
 USE M_MB_General_CONSTANTS, ONLY : Pi
 IMPLICIT NONE
 INTEGER, PARAMETER :: NPAWC=100000
 REAL(4) :: H(NPAWC)
 COMMON/PAWC/H
 REAL(4) :: phi,r,z,der,cal
 INTEGER :: jr
 COMMON/derivChamp/phi,r,z,der(9),cal(9),jr
 INTEGER :: I, JF, JZ, IER, ICYCLE
 REAL(8) :: FI, RR, ZZ
 REAL(8) :: XYZ(3), BXYZA(12), BXYZ1(12), BXYZ2(12), BXYZB(12)
 REAL(8), PARAMETER :: delta=0.1d0
!
   CALL HLIMIT(NPAWC)
   CALL HROPEN(49,'champ','champ.ntu','N',1024,IER)
   CALL HBNT(100,'deriv',' ')
   CALL HBNAME(100,'derivChamp',phi,'phi,r,z,der(9),cal(9),jr')
   DO JR=1,12
     IF( JR == 1 ) THEN
       ZZ = 200.d0
       RR = 430.d0
     ELSE IF( JR == 2 ) THEN
       ZZ = 200.d0
       RR = 600.d0
     ELSE IF( JR == 3 ) THEN
       ZZ = 200.d0
       RR = 900.d0
     ELSE IF( JR == 4 ) THEN
       ZZ = 680.d0
       RR = 200.d0
     ELSE IF( JR == 5 ) THEN
       ZZ = 680.d0
       RR = 600.d0
     ELSE IF( JR == 6 ) THEN
       ZZ = 680.d0
       RR = 900.d0
     ELSE IF( JR == 7 ) THEN
       ZZ = 680.d0
       RR = 1210.d0
     ELSE IF( JR == 8 ) THEN
       ZZ = 1200.d0
       RR =  200.d0
     ELSE IF( JR == 9 ) THEN
       ZZ = 1200.d0
       RR =  600.d0
     ELSE IF( JR == 10 ) THEN
       ZZ = 1200.d0
       RR = 1210.d0
     ELSE IF( JR == 11 ) THEN
       ZZ = 2250.d0
       RR =  200.d0
     ELSE
       ZZ = 2250.d0
       RR = 1210.d0
     ENDIF
     DO JZ=1,2
       IF( JZ == 2 ) ZZ = - ZZ
       DO JF=0,255
         FI = JF * PI / 128.d0
         XYZ(1) = RR * COS(FI)
         XYZ(2) = RR * SIN(FI)
         XYZ(3) = ZZ
!         write(48,*) ' >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> test pour FI RR ZZ ',FI*180./PI,RR,ZZ
!         write(48,*) ' au point ',XYZ(1:3)
         CALL DFIELD(XYZ, 12, BXYZA)
         DO I=1,3
           XYZ(I) = XYZ(I) + delta
           CALL DFIELD(XYZ, 3, BXYZ1)
           XYZ(I) = XYZ(I) - 2.d0*delta
           CALL DFIELD(XYZ, 3, BXYZ2)
           XYZ(I) = XYZ(I) + delta
           BXYZB(3+I) = (BXYZ1(1)-BXYZ2(1)) / delta / 2.d0
           BXYZB(6+I) = (BXYZ1(2)-BXYZ2(2)) / delta / 2.d0
           BXYZB(9+I) = (BXYZ1(3)-BXYZ2(3)) / delta / 2.d0
         ENDDO
!         write(48,*) ' ======= derivees fournies '
!         write(48,'(3F12.8,/,3F12.8,/,3F12.8)') BXYZA(4:12)
!         write(48,*) ' ======= derivees calculees '
!         write(48,'(3F12.8,/,3F12.8,/,3F12.8)') BXYZB(4:12)
         phi   = FI
         r     = RR
         z     = ZZ
         der(1:9) = BXYZA(4:12)
         cal(1:9) = BXYZB(4:12)
         CALL HFNT(100)
       ENDDO
     ENDDO
   ENDDO
   CALL HCDIR('//champ',' ')
   CALL HROUT(100,ICYCLE,' ')
   CALL HREND('champ')
! 
 END SUBROUTINE testChamp
!
