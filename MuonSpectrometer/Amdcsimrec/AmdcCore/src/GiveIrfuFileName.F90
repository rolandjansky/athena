 subroutine GiveIrfuFileName(filename)
 implicit none
 character(22), intent(out) :: filename
 integer :: i,getpid
 logical :: ex
   filename(1:8) = 'IrfuFile'
   i = mod( getpid(), 1000000000 )
   write(filename(1:9),'(i9.9)') i
   filename(1:18) = 'IrfuFile' // filename(1:9) // '.'
   do i=1000,9999
     write(filename(19:22),'(i4.4)')  i
!    print *,'GiveIrfuFileName: try ',filename
     inquire( file=filename, exist=ex )
     if( .not.ex )  return
   enddo
   write(*,*)  ' GiveIrfuFileName : can not build filename'
   stop
 end
