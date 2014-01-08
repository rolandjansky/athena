!
 SUBROUTINE ELIM_SUV_NAME(Name)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 CHARACTER(len=*), INTENT(IN) :: Name
 INTEGER :: IS0
!
   DO IS0=1,Nsuvo
     IF( CHASUV(IS0) == Name ) THEN
       CALL ELIM_SUV_IS(IS0)
       EXIT
     ENDIF
   ENDDO
!
 END SUBROUTINE ELIM_SUV_NAME
!
