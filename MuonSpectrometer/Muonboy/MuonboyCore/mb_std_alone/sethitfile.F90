subroutine sethitfile(filename)
  use m_mb_standalone
  implicit none
  character(len=*) :: filename
  open( unit=LUNHIT, file=filename, status='old')
end subroutine sethitfile
