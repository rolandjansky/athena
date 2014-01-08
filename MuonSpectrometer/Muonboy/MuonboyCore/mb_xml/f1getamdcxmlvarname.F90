!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE F1GetAmdcXmlVarName( Ivar, Lvar,Cvar1)
 IMPLICIT NONE
!
 INTEGER,           INTENT(IN)  :: Ivar
 INTEGER,           INTENT(OUT) :: Lvar
 CHARACTER (Len=1), INTENT(OUT) :: Cvar1(512)
!
 CHARACTER (len=512)  :: Cvar
 INTEGER              :: I
!
    call GetAmdcXmlVarName( IVar, Cvar )
    Lvar = MIN( LEN_TRIM(Cvar) , 512 )
    DO I=1,Lvar
     Cvar1(I) = Cvar(I:I)
    ENDDO
!
 END
