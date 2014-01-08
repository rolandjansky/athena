!
!> \author M.Virchaux   08/10/96
!
 SUBROUTINE UPJTUB(N,ICC,IWW,JLTUB, JFLAG)
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: N, ICC(*), IWW(*), JLTUB(*)
 INTEGER, INTENT(INOUT) :: JFLAG(*)
 INTEGER :: J, IP, ITDEB, ITFIN, IT
 INTEGER :: KTUB10, KTUB01, JFLA10, JFLA01
!
   DO J=1,N
     DO IP=1,JLTUB(J)
       ITDEB = NTUBH0(ICC(J),IWW(J),IP) + 1
       ITFIN = NTUBH0(ICC(J),IWW(J),IP) + NTUBHI(ICC(J),IWW(J),IP)
       DO 100 IT=ITDEB,ITFIN
         IF( KTUBHI(IT) /= 0 ) THEN
           KTUB10 = KTUBHI(IT) / 10
           KTUB01 = KTUBHI(IT) - 10*KTUB10
           JFLA10 = JFLAG(IT)  / 10
           JFLA01 = JFLAG(IT)  - 10*JFLA10
           IF( KTUB01 /= 0 ) THEN
             IF( JFLA01 == 0 ) THEN
               JFLA01 = KTUB01
             ELSEIF( JFLA01 /= KTUB01 ) THEN
               JFLA01 =  3
             ENDIF
           ENDIF
           IF( KTUB10 /= 0 ) THEN
             IF( JFLA10 == 0 ) THEN
               JFLA10 = KTUB10
             ELSEIF( JFLA10 /= KTUB10 ) THEN
               JFLA10 =  3
             ENDIF
           ENDIF
           JFLAG(IT) = 10*JFLA10 + JFLA01
         ENDIF
100    ENDDO
     ENDDO
   ENDDO
!
 END SUBROUTINE UPJTUB
!
