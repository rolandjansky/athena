!
!> \author SaMuSoG
!
 SUBROUTINE IsThereADisplaFromAMDC(TypChar1,Jff,Jzz,Job, ThereIs)
 IMPLICIT NONE
 CHARACTER(1), INTENT(IN)  :: TypChar1(*)
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job
 INTEGER,      INTENT(OUT) :: ThereIs
 CHARACTER(3) :: Car3
 INTEGER      :: Jtyp, Ideb, Ifin
 INTEGER      :: JTROU3, FindObject
!
#include "AmdcStand/amdcsim_com.inc"
!
      ThereIs = 0
!
      Car3(1:1)=TypChar1(1)
      Car3(2:2)=TypChar1(2)
      Car3(3:3)=TypChar1(3)
      Jtyp = JTROU3(Car3,MTYP,CARTYP)
      IF( Jtyp < 1 )  RETURN
!
      Ifin = LastAdjustByType(Jtyp)
      IF( Ifin <= 0 ) RETURN
      Ideb = FirstAdjustByType(Jtyp)
      IF( FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLadjust) > 0 ) ThereIs = 1
!
 END SUBROUTINE IsThereADisplaFromAMDC
!
