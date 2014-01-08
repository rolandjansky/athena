!
 SUBROUTINE F1GetAmdcXmlVarValue( Long, Name1, Iok, Value )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long
 INTEGER,           INTENT(OUT) :: Iok
 REAL(8),           INTENT(OUT) :: Value
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    call GetAmdcXmlVarValue( VarName, Iok,Value )
!
 END SUBROUTINE F1GetAmdcXmlVarValue
!
