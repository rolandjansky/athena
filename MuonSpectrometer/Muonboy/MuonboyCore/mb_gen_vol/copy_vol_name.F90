!
 SUBROUTINE COPY_VOL_NAME(NameA,NameB,Jcolor)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: NameA,NameB
 INTEGER           :: Jcolor
 INTEGER :: IS0,IV0,Jvolclo,IVC
!
   DO IS0=1,Nsuvo
     DO IV0=NvoluI(IS0),NvoluA(IS0)
       IF( CHAVOL(IV0) == NameA ) GO TO 10
     ENDDO
   ENDDO
!
10 Jvolclo = IclVolu(IV0)
   CALL COPY_VOL_IV( IV0, NameB,Jvolclo,Jcolor, IVC )
!
 END SUBROUTINE COPY_VOL_NAME
!
