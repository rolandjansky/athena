!
!> \author SamuSog
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE F1PerformDirectDisplacAndDeform( NameType1,Jff,Jzz, Xin,Yin,Zin, Xout,Yout,Zout )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz
 REAL(8),      INTENT(IN)  :: Xin,Yin,Zin
!
 REAL(8),      INTENT(OUT) :: Xout,Yout,Zout
 INTEGER      :: Job
!
    Job        = 0
    call F1PerformDirectDisplacAndDeformBase( NameType1,Jff,Jzz, Job, Xin,Yin,Zin, Xout,Yout,Zout )
!
 END SUBROUTINE F1PerformDirectDisplacAndDeform
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE F1PerformDirectDisplacAndDeformBase( NameType1,Jff,Jzz, Job, Xin,Yin,Zin, Xout,Yout,Zout )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)  :: Xin,Yin,Zin
 REAL(8),      INTENT(OUT) :: Xout,Yout,Zout
!
 CHARACTER(3) :: Ctyp
 REAL(8)      :: XYZin(3),XYZout(3)
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    XYZin(1)  = Xin
    XYZin(2)  = Yin
    XYZin(3)  = Zin
    CALL FPerformDirectDisplacAndDeform( Ctyp,Jff,Jzz,Job, XYZin, XYZout )
    Xout = XYZout(1)
    Yout = XYZout(2)
    Zout = XYZout(3)
!
 END SUBROUTINE F1PerformDirectDisplacAndDeformBase
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE F1PerformBackDisplacAndDeform( NameType1,Jff,Jzz, Xin,Yin,Zin, Xout,Yout,Zout )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz
 REAL(8),      INTENT(IN)  :: Xin,Yin,Zin
 REAL(8),      INTENT(OUT) :: Xout,Yout,Zout
!
 INTEGER      :: Job
!
    Job        = 0
    call F1PerformBackDisplacAndDeformBase( NameType1,Jff,Jzz,Job, Xin,Yin,Zin, Xout,Yout,Zout )
!
 END SUBROUTINE F1PerformBackDisplacAndDeform
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE F1PerformBackDisplacAndDeformBase( NameType1,Jff,Jzz,Job, Xin,Yin,Zin, Xout,Yout,Zout )
!
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: NameType1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 REAL(8),      INTENT(IN)  :: Xin,Yin,Zin
 REAL(8),      INTENT(OUT) :: Xout,Yout,Zout
!
 CHARACTER(3) :: Ctyp
 REAL(8)      :: XYZin(3),XYZout(3)
!
    Ctyp(1:1) = NameType1(1)
    Ctyp(2:2) = NameType1(2)
    Ctyp(3:3) = NameType1(3)
    XYZin(1)  = Xin
    XYZin(2)  = Yin
    XYZin(3)  = Zin
    CALL FPerformBackDisplacAndDeform( Ctyp,Jff,Jzz,Job, XYZin, XYZout )
    Xout = XYZout(1)
    Yout = XYZout(2)
    Zout = XYZout(3)
!
 END SUBROUTINE F1PerformBackDisplacAndDeformBase
!
