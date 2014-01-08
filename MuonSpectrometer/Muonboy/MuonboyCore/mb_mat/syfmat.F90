!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE SYFMAT( IVMDEB, IVMFIN )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: IVMDEB, IVMFIN
 INTEGER :: IV, JJJ, JFF, JFFP, IT0, ITresult, IP
!
   DO IV=IVMDEB,IVMFIN
     JJJ = JMATI(IV)     / KMAJFF
     JFF = JMATI(IV) - JJJ*KMAJFF
     IF( JFF /= 1 ) THEN
       PRINT '(//" In SYFMAT : Variable JFF is not equal to 1  =====>  STOP !")'
       STOP
     ENDIF
!
     DO 200 JFFP=2,8
       NVMATI = NVMATI + 1
       IF( NVMATI > KVOLU ) THEN
         PRINT 3000,'KVOLU'
         STOP
       ENDIF
       IT0 = ITRANSMATI(IV)
       IF( IT0 == 0 .OR. IT0 == 1 ) THEN
         ITRANSMATI(NVMATI) = JFFP
       ELSEIF( IT0 == 11 ) THEN
         ITRANSMATI(NVMATI) = JFFP + 10
       ELSE
         CALL COMPOS_TRANSMATI( IT0, JFFP, ITresult )
         ITRANSMATI(NVMATI) = ITresult
       ENDIF
       JMATI(NVMATI)  = JJJ*KMAJFF + JFFP
       NPMAT0(NVMATI) = NPMATI + 1
       DO IP=NPMAT0(IV),NPMAT1(IV)
         NPMATI = NPMATI + 1
         IF( NPMATI > KPLAN ) THEN
           PRINT 3000,'KPLAN'
           STOP
         ENDIF
         NEMAT0(NPMATI) = NEMAT0(IP)
         NEMAT1(NPMATI) = NEMAT1(IP)
       ENDDO
       NPMAT1(NVMATI) = NPMATI
200  ENDDO
!
   ENDDO
!
3000 FORMAT(//' In SYFMAT : Parameter ',A5,' is not big enough  =====>  STOP !')
 END
