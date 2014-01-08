!
 SUBROUTINE AmdcAgddFromCar
 IMPLICIT NONE
 INTEGER, PARAMETER :: Iflag = 100
 INTEGER :: Iset,Istatus
 character(22) :: filename
!>> AMDC string
    call GiveIrfuFileName(filename)
    open(   unit=37, file=filename, status='new' )
    call WRITE_AMDC(37)
    rewind( unit=37 )
    CALL REAMDC( 37, Iflag )
    close(  unit=37 ,status='delete')
!     
!>> AGDD string    
    Iset    = 1
    Istatus = 0
    CALL AgddReadStatus( Iset, Istatus )
!      
 END SUBROUTINE AmdcAgddFromCar
!
