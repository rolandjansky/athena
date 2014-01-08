!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ELIM_VOL_NAME(Name)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  Eliminate the first encountered volume of name  Name
!>>
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 CHARACTER (len=*) :: Name
 INTEGER :: IS0,IV0,NADMI,NADMA,IV,IP,IE,IA
!
   DO IS0=1,Nsuvo
     DO IV0=NvoluI(IS0),NvoluA(IS0)
       IF( CHAVOL(IV0) == Name ) GO TO 10
     ENDDO
   ENDDO
   RETURN
10 CONTINUE
!
   NADMI = Nadge
   NADMA = 1
   DO IV=NvoluI(IS0),NvoluA(IS0)
     DO IP=NplanI(1,IV),NplanA(M4,IV)
       DO IE=NedgeI(IP),NedgeA(IP)
         IA = IadEdge(IE)
         NADMI = MIN( NADMI , IA )
         NADMA = MAX( NADMA , IA )
       ENDDO
     ENDDO
   ENDDO
!
   CALL ELIM_VOL_D(NADMI,NADMA,IS0,IV0)
!
 END
