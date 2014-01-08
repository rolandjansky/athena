!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_SNAKE_NXYZNRF( Name, Jcolor, Npt,Xpt,Ypt,Zpt,Nr,Rr,Ff )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,Npt,Nr
 REAL(8),           INTENT(IN) :: Xpt(*),Ypt(*),Zpt(*),Rr,Ff
!
 INTEGER :: NadgeDeb, I,J, Ip1
 INTEGER :: Iadd(Nr+2), Ivue(Nr+2)
 REAL(8) :: XYZ(3,Npt), VeX(3),VeY(3),VeZ(3), VfZ(3), VgZ(3)
 REAL(8) :: Ve0(3,Nr), dPhi,Phi0,Angle,Ambda
!
   IF( Npt < 2 .OR. Nr < 3 ) RETURN
!
   dPhi = TwoPi / Nr
   Phi0 = PiS180 * Ff
   DO J=1,Npt
    XYZ(1:3,J) = (/ Xpt(J), Ypt(J), Zpt(J) /)
   ENDDO
   NadgeDeb = Nadge
!
   IF( SIZE(XYZadge)/3 < Nadge+Nr*Npt ) CALL Push_MB_Gen_Vol_MADGE
!
   VeZ(1:3) = XYZ(1:3,2) - XYZ(1:3,1)
   CALL NORM3V( VeZ )
   IF( ABS(VeZ(3)) < 0.999D0 ) THEN
      VeX(1:3) = (/ VeZ(2), -VeZ(1), Zero /)
   ELSE
      VeX(1:3) = (/ VeZ(3), Zero, -VeZ(1) /)
   ENDIF
   CALL NORM3V( VeX )
   CALL PVEC3V( VeZ,VeX, VeY )
   DO I=1,Nr
    Angle = (I-1)*dPhi + Phi0
    Ve0(1:3,I) = Rr * ( COS(Angle)*VeX(1:3) + SIN(Angle)*VeY(1:3) )
    Nadge = Nadge + 1
    XYZadge(1:3,Nadge) = Ve0(1:3,I) + XYZ(1:3,1)
   ENDDO
!
   DO J=3,Npt+1
    IF( J < Npt+1 ) THEN
       VfZ(1:3) = XYZ(1:3,J) - XYZ(1:3,J-1)
       CALL NORM3V( VfZ )
       VgZ(1:3) = VeZ(1:3) + VfZ(1:3)
    ELSE
       VgZ(1:3) = VeZ(1:3)
    ENDIF
    DO I=1,Nr
     Ambda = - DOT_PRODUCT(Ve0(1:3,I),VgZ) / DOT_PRODUCT(VeZ,VgZ)
     Ve0(1:3,I) = Ve0(1:3,I) + Ambda*VeZ(1:3)
     Nadge = Nadge + 1
     XYZadge(1:3,Nadge) = Ve0(1:3,I) + XYZ(1:3,J-1)
    ENDDO
    VeZ(1:3) = VfZ(1:3)
   ENDDO
!
!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
!
   Ivue(1:Nr+2) = 1
   DO I=1,Nr
    Iadd(I) = NadgeDeb + I
   ENDDO
   CALL DEFPLA( Jcolor, Nr, Ivue, Iadd )
   DO J=2,Npt
    DO I=1,Nr
     Ip1 = MOD(I,Nr) + 1
     Iadd(1) = NadgeDeb + (J-2)*Nr + I
     Iadd(2) = NadgeDeb + (J-2)*Nr + Ip1
     Iadd(3) = NadgeDeb + (J-1)*Nr + Ip1
     Iadd(4) = NadgeDeb + (J-1)*Nr + I
     CALL DEFPLA( Jcolor, 4, Ivue, Iadd )
    ENDDO
   ENDDO
   DO I=1,Nr
    Iadd(I) = NadgeDeb + (Npt-1)*Nr + I
   ENDDO
   CALL DEFPLA( Jcolor, Nr, Ivue, Iadd )
!
   CALL ENDVOL_SUV
!
 END
