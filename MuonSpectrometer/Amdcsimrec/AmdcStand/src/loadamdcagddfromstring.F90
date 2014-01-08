!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE LoadAmdcAgddFromString( Namdc, Camdc, Nagdd, Cagdd )
 IMPLICIT NONE
 INTEGER,           INTENT(IN) :: Namdc,   Nagdd
 CHARACTER (len=1), INTENT(IN) :: Camdc(*),Cagdd(*)
 INTEGER, PARAMETER :: IFLAG = 100
!--------------------------------------------------------------------
! IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
! IFLAG = 100  ===>  No conversion !
!--------------------------------------------------------------------
      call LoadAmdcAgddFromStringn( Namdc, Camdc, Nagdd, Cagdd,IFLAG )
!
 END SUBROUTINE LoadAmdcAgddFromString
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE LoadAmdcAgddFromStringn( Namdc, Camdc, Nagdd, Cagdd,IFLAG )
 IMPLICIT NONE
 INTEGER,           INTENT(IN) :: Namdc,   Nagdd
 CHARACTER (len=1), INTENT(IN) :: Camdc(*),Cagdd(*)
 INTEGER,           INTENT(IN) :: IFLAG
 INTEGER :: I,Iset,Istatus, ierr
 character(22) :: filename
 character(27) :: filetmp
!--------------------------------------------------------------------
! IFLAG =   0  ===>  Conversion CYLINDRICAL -> CARTESIAN during reading
! IFLAG = 100  ===>  No conversion !
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      IF( Namdc > MaxAMDC ) THEN
        PRINT 7771,Namdc,MaxAMDC
7771    FORMAT(/'>>> In the AMDC package, Atempt to load an AMDC ',   &
                'Character String (',I7,' characters)',               &
               /'>>> too long compared to the current String ',       &
                'Allocation of the package (',I7,' characters)',      &
               /'>>>  =====>  STOP !!!')
        STOP
      ENDIF
      IF( Nagdd > MaxAGDD ) THEN
        PRINT 7772,Nagdd,MaxAGDD
7772    FORMAT(/'>>> In the AMDC package, Atempt to load an AGDD ',   &
                'Character String (',I7,' characters)',               &
               /'>>> too long compared to the current String ',       &
                'Allocation of the package (',I7,' characters)',      &
               /'>>>  =====>  STOP !!!')
        STOP
      ENDIF
!
!-- AMDC string
      LenAMDC = Namdc
      DO I=1,Namdc
        CarAMDC(I) = Camdc(I)
      ENDDO
      call GiveIrfuFileName(filename)
      open( unit=37, file=filename, status='new', iostat=ierr )
      IF( ierr /= 0 ) THEN
        write(*,*) ' in AmdcStand/LoadAmdcAgddFromString :'
        write(*,*) '     file opening failed with error ',ierr
        write(*,*) '     usually because disk full'
        write(*,*) '     try opening on /tmp ... '
        filetmp = '/tmp/' // filename
        open( unit=37, file=filetmp, status='new', iostat=ierr )
        IF( ierr /= 0 ) THEN
          write(*,*) '         ... but still failed with error ',ierr
          STOP
        ENDIF
      ENDIF
      call WRITE_AMDC(37)
      rewind( unit=37 )
      CALL REAMDC( 37, IFLAG ) 
      close(  unit=37 ,status='delete')
!
!-- AGDD string    
      LenAGDD = Nagdd
      DO I=1,Nagdd
        CarAGDD(I) = Cagdd(I)
      ENDDO
      Iset    = 1 
      Istatus = 0 
      CALL AgddReadStatus( Iset, Istatus )
!
 END SUBROUTINE LoadAmdcAgddFromStringn
!
