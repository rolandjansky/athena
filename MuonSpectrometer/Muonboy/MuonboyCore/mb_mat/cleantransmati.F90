!
!> \author SaMuSoG
!
 SUBROUTINE CLEANTRANSMATI
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER              :: IV,IT,ITnew
 INTEGER, ALLOCATABLE :: NewAddTrans(:)
!
   IF( NTMATI < 19 ) RETURN
!
   ALLOCATE( NewAddTrans(0:NTMATI) )
!
   NewAddTrans(0:NTMATI) = 0
   DO IV=1,NVMATI
     IT = ITRANSMATI(IV)
     NewAddTrans(IT) = 1
   ENDDO
   NewAddTrans(  0 ) = 0
   NewAddTrans(1:18) = 1
!
   ITnew = 0
   DO IT=1,NTMATI
    IF( NewAddTrans(IT) < 1 ) CYCLE
       ITnew = ITnew + 1
       NewAddTrans(IT) = ITnew
       TRANSMATI(1:3,1:4,ITnew) = TRANSMATI(1:3,1:4,IT)
   ENDDO
   NTMATI = ITnew
!
   DO IV=1,NVMATI
    IT = ITRANSMATI(IV)
    ITRANSMATI(IV) = NewAddTrans(IT)
   ENDDO
!
   DEALLOCATE( NewAddTrans )
!
 END SUBROUTINE CLEANTRANSMATI
!
