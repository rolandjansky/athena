!
 SUBROUTINE NoisyStrips
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER :: ICT, IRPC, IP, IS, first, last, ipcur, Icode, IPside
!
   DO ICT=1,NTRITO
     IRPC = IRPTGC(ICT)
     DO IP=1,N2BAND(IRPC)
       last  = -1
       ipcur = -1
       DO IS=NBANH0(ICT,IP)+1,NBANH0(ICT,IP)+NBANHI(ICT,IP)
         Icode  = KBANHI(IS) / 10
         IPside = KBANHI(IS) - 10*Icode
         IF( last < 0 .OR. IPside /= ipcur ) THEN
           IF( last > 0 .AND. last-first+1 >= MaxConStr ) LBANHI(first:last) = - LBANHI(first:last)
           first = IS
           last  = IS
           ipcur = IPside
         ELSE IF( LBANHI(IS)/10-LBANHI(last)/10 == 1 ) THEN
           last  = IS
         ELSE
           IF( last-first+1 >= MaxConStr ) LBANHI(first:last) = - LBANHI(first:last)
           first = IS
           last  = IS
           ipcur = IPside
         ENDIF
       ENDDO
       IF( last > 0 .AND. last-first+1 >= MaxConStr ) LBANHI(first:last) = - LBANHI(first:last)
     ENDDO
   ENDDO
!
 END SUBROUTINE NoisyStrips
!
