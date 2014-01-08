!
!> \author M.Virchaux
!
 SUBROUTINE PLAMATII( NN, IAD )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: NN, IAD(*)
 INTEGER :: L, IABAD
!
   NPMATI = NPMATI + 1
   NEMAT0(NPMATI) = NEMATI + 1
   NEMAT1(NPMATI) = NEMATI + NN
   DO L=1,NN
     NEMATI = NEMATI + 1
     IAMATI(NEMATI) = IAD(L)
     IABAD = IABS( IAD(L) )
     IF( IABAD < 1 .OR. IABAD > NAMATI ) THEN
       PRINT 1000,L,IAD(L),NAMATI
1000   FORMAT(//' In PLAMATII : Address  IAD(',I3,') =',I7,  &
                '  is out of range (NAMATI =',I7,')  =====>  STOP !')
       STOP
     ENDIF
   ENDDO
!
   IF( NPMATI > KPLAN ) THEN
     PRINT 1100,'KPLAN'
     STOP
   ENDIF
   IF( NEMATI.GT.KEDGE ) THEN
     PRINT 1100,'KEDGE'
     STOP
   ENDIF
!
1100 FORMAT(//' In PLAMATII of MUONBOY : Parameter ',A5,' is not big enough  =====>  STOP !!!')
 END SUBROUTINE PLAMATII
!
