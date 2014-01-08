!
 SUBROUTINE MuboXYZtoSZT( barrel, SiFi,CoFi, Xin,Yin,Zin, Sout,Zout,Tout )
 IMPLICIT NONE
 LOGICAL, INTENT(IN)  :: barrel
 REAL(8), INTENT(IN)  :: SiFi,CoFi, Xin,Yin,Zin
 REAL(8), INTENT(OUT) :: Sout,Zout,Tout
!
    IF( barrel ) THEN
       Sout = - SiFi*Xin + CoFi*Yin
       Zout =        Zin
       Tout =   CoFi*Xin + SiFi*Yin
    ELSE
       Sout = - SiFi*Xin + CoFi*Yin
       Zout =   CoFi*Xin + SiFi*Yin
       Tout =        Zin
    ENDIF
!
 END SUBROUTINE MuboXYZtoSZT
!
