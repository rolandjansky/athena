!
 SUBROUTINE MuboSZTtoXYZ( barrel, SiFi,CoFi, Sin,Zin,Tin, Xout,Yout,Zout )
 IMPLICIT NONE
 LOGICAL, INTENT(IN)  :: barrel
 REAL(8), INTENT(IN)  :: SiFi,CoFi, Sin,Zin,Tin
 REAL(8), INTENT(OUT) :: Xout,Yout,Zout
!
    IF( barrel ) THEN
       Xout = CoFi*Tin - SiFi*Sin
       Yout = SiFi*Tin + CoFi*Sin
       Zout =      Zin
    ELSE
       Xout = CoFi*Zin - SiFi*Sin
       Yout = SiFi*Zin + CoFi*Sin
       Zout =      Tin
    ENDIF
!
 END SUBROUTINE MuboSZTtoXYZ
!
