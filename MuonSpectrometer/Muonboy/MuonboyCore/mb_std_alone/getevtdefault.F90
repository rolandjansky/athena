subroutine getevtdefault(tostore,iret,ideve,store)
  use m_mb_standalone
  implicit none
  integer :: tostore
  integer :: iret
  integer :: ideve
  integer :: store
  call getevt(LUNHIT,0,-1,tostore,iret,ideve,store)
end subroutine getevtdefault
