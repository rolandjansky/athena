!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE ELIM_VOL_IV(IV0)
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
 INTEGER :: IV0
 INTEGER :: IS0,NADMI,NADMA,IV,IP,IE,IA
!
   DO IS0=1,Nsuvo
     IF( NvoluI(IS0) <= IV0 .AND. IV0 <= NvoluA(IS0) ) GO TO 10
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
