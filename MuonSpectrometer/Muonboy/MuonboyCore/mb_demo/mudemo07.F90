!
!> \author J.F.Laporte
!
 SUBROUTINE MUDEMO07
 USE M_MB_Control, ONLY : LunDumSeg
 USE M_MB_Digis
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: IBINTR, L
 LOGICAL, SAVE :: FIRST=.TRUE.
 INTEGER, SAVE :: iPseudoEvtKounter=0
!
   IF( LunDumSeg > 0 ) THEN
     IF( FIRST ) THEN
       FIRST=.FALSE.
       Open(unit=LunDumSeg, file='Out.RecSegments.txt',   status='unknown')
     ENDIF
!
     iPseudoEvtKounter = iPseudoEvtKounter + 1
     WRITE(LunDumSeg,7004) iPseudoEvtKounter,NBINTR
     IF( NBINTR /= 0 ) THEN
       DO IBINTR=1,NBINTR
         WRITE(LunDumSeg,7000) iPseudoEvtKounter,NBINTR,    &
           IBINTR,CARISC(ISCINTR(IBINTR)),NUINTR(IBINTR),   &
           PTINTR(1:3,IBINTR),VEINTR(1:3,IBINTR)
         DO L=1,NUINTR(IBINTR)
           WRITE(LunDumSeg,7002) iPseudoEvtKounter,NBINTR,  &
             IBINTR,CARISC(ISCINTR(IBINTR)),NUINTR(IBINTR), &
             L,RdigINTR(L,IBINTR),RtraINTR(L,IBINTR),       &
             RtraINTR(L,IBINTR)-RdigINTR(L,IBINTR),         &
             DTUBHI(IUINTR(L,IBINTR))
         ENDDO
       ENDDO
     ENDIF
   ENDIF
!
7004 FORMAT('EVT ',2i6                      )
7000 FORMAT('SEG ',2i6,i6,1x,a3,i6,   6F12.5)
7002 FORMAT('DIG ',2i6,i6,1x,a3,i6,i6,4F12.5)
!
 END SUBROUTINE MUDEMO07
!
