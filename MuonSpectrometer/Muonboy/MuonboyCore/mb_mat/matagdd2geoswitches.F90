!-----------------------------------------------------
 SUBROUTINE ReSetMatAGDD2GeoSwitches
 USE M_MB_Mat_AGDD2GeoSwitches
 IMPLICIT NONE
 
 NewAGDD2GeoSwitches = 0
 NberAGDD2GeoSwitches = 0

 END SUBROUTINE ReSetMatAGDD2GeoSwitches

!-----------------------------------------------------
 SUBROUTINE F1SetMatAGDD2GeoSwitches( Long, Name)
 IMPLICIT NONE

 INTEGER,           INTENT(IN) :: Long 
 CHARACTER (Len=1), INTENT(IN) :: Name(*)
 
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I

 DO I=1,Long
  VarName(I:I) = Name(I)
 ENDDO
 CALL SetMatAGDD2GeoSwitches( Long, VarName)

 END SUBROUTINE F1SetMatAGDD2GeoSwitches

!-----------------------------------------------------
 SUBROUTINE SetMatAGDD2GeoSwitches( Long, Name )
 USE M_MB_Mat_AGDD2GeoSwitches
 IMPLICIT NONE

 INTEGER,           INTENT(IN) :: Long 
 CHARACTER (len=*), INTENT(IN) :: Name

 CHARACTER(100) :: ChaTemp
 INTEGER        :: I
 
 NewAGDD2GeoSwitches = 1
 NberAGDD2GeoSwitches = NberAGDD2GeoSwitches + 1

 DO I=1,100
  if (I.le.Long) then
    ChaTemp(I:I) = Name(I:I)
  else
    ChaTemp(I:I) =" "
  endif
 ENDDO
 ChaAGDD2GeoSwitches(NberAGDD2GeoSwitches) = ChaTemp

 END SUBROUTINE SetMatAGDD2GeoSwitches
!-----------------------------------------------------
 SUBROUTINE RemoveMatAGDD2GeoSwitches( Name )
 USE M_MB_Mat_AGDD2GeoSwitches
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER :: I

 DO I=1,NberAGDD2GeoSwitches
   IF( TRIM(Name) == TRIM(ChaAGDD2GeoSwitches(I)) ) THEN
     AGDD2GeoAlreadyUsed(I) = .true.
     EXIT
   ENDIF
 ENDDO

 END SUBROUTINE RemoveMatAGDD2GeoSwitches 
