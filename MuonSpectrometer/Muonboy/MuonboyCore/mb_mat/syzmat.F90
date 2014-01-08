!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE SYZMAT( IVMDEB, IVMFIN )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IVMDEB, IVMFIN
 INTEGER :: JZZMAX, JZZMIN, IV, JJJ, III, JZZ, JFF, JZZNEW, IT0, ITresult, IP
!
   JZZMAX = 0
   JZZMIN = KMAJZZ
   DO IV=IVMDEB,IVMFIN
     JJJ = JMATI(IV) / KMAJFF
     III =   JJJ     / KMAJZZ
     JZZ =   JJJ - III*KMAJZZ
     JZZMAX = MAX( JZZMAX , JZZ )
     JZZMIN = MIN( JZZMIN , JZZ )
   ENDDO
!
   DO IV=IVMDEB,IVMFIN
     JJJ = JMATI(IV)     / KMAJFF
     JFF = JMATI(IV) - JJJ*KMAJFF
     III =   JJJ         / KMAJZZ
     JZZ =   JJJ     - III*KMAJZZ
     JZZNEW = JZZMAX + 1 + JZZ - JZZMIN
     IF( JZZNEW >= KMAJZZ ) THEN
       PRINT '(//" In SYZMAT : Parameter KMAJZZ is not big enough  =====>  STOP !")'
       STOP
     ENDIF
     NVMATI = NVMATI + 1
     IF( NVMATI > KVOLU ) THEN
       PRINT '(//" In SYZMAT : Parameter KVOLU is not big enough  =====>  STOP !")'
       STOP
     ENDIF
!
     IT0 = ITRANSMATI(IV)
     IF( IT0 == 0 ) THEN
       ITRANSMATI(NVMATI) = 11
     ELSEIF( IT0 >= 1 .AND. IT0 <= 8 ) THEN
       ITRANSMATI(NVMATI) = IT0 + 10
     ELSEIF( IT0 >= 11 .AND. IT0 <= 18 ) THEN
       ITRANSMATI(NVMATI) = IT0 - 10
     ELSE
       CALL COMPOS_TRANSMATI( IT0, 11, ITresult )
       ITRANSMATI(NVMATI) = ITresult
     ENDIF
     JMATI(NVMATI)  = III*KMAJFF*KMAJZZ + JZZNEW*KMAJFF + JFF
     NPMAT0(NVMATI) = NPMATI + 1
     DO IP=NPMAT0(IV),NPMAT1(IV)
       NPMATI = NPMATI + 1
       IF( NPMATI.GT.KPLAN ) THEN
         PRINT '(//" In SYZMAT : Parameter KPLAN is not big enough  =====>  STOP !")'
         STOP
       ENDIF
       NEMAT0(NPMATI) = NEMAT0(IP)
       NEMAT1(NPMATI) = NEMAT1(IP)
     ENDDO
     NPMAT1(NVMATI) = NPMATI
   ENDDO
!
 END
