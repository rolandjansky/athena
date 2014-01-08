!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE VOLABOOLB_NAME(IBOOL1,IBOOL2,Jcolor,NameA,NameB,NameC)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 CHARACTER (len=*) :: NameA,NameB,NameC
 INTEGER           :: IBOOL1,IBOOL2,Jcolor
 INTEGER :: IS1,IV1,IS2,IV2,IVC
!
   DO IS1=1,Nsuvo
     DO IV1=NvoluI(IS1),NvoluA(IS1)
       IF( CHAVOL(IV1) == NameA ) GO TO 10
     ENDDO
   ENDDO
   PRINT 1111,NameA
   RETURN
!
10 DO IS2=1,Nsuvo
     DO IV2=NvoluI(IS2),NvoluA(IS2)
       IF( CHAVOL(IV2) == NameB ) GO TO 20
     ENDDO
   ENDDO
   PRINT 1111,NameB
   RETURN
!
20 IF( IS1 /= IS2 ) THEN
     PRINT 1112,NameA,NameB,IV1,IV2,IS1,IS2
     RETURN
   ENDIF
!
   CALL VOLABOOLB_IV( IBOOL1,IBOOL2, IV1,IV2,NameC,Jcolor, IVC)
!
1111 FORMAT(' VOLABOOLB_NAME was called with volume name :'/A,'  that does not exist',  &
                '  =====>  Nothing was done !!!')
1112 FORMAT(' VOLABOOLB_NAME was called with :'/' NameA,NameB =  ',A,2X,A/  &
            ' Coresponding to IV1,IV2,IS1,IS2 =',4I6,'  =====>  Nothing was done !!!')
 END
