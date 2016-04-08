      subroutine printHepupContents
      implicit none
        
#include "GeneratorFortranCommon/heprup.inc"      
#include "GeneratorFortranCommon/hepeup.inc"

C      integer i,j,k,l,ievt
      integer i,ievt
      logical firstTime
      data firstTime /.true./, ievt /0/
      save firstTime

      firstTime=.false.

      ievt=ievt+1
      write(*,*) "info in hepeup:",ievt
      write(*,*) NUP, IDPRUP  !, XWGTUP, SCALUP, AQEDUP, AQCDUP
      do i=1,nup
      write(*,*) idup(i),istup(i),mothup(1,i),mothup(2,i)
      write(*,*) icolup(1,i),icolup(2,i), vtimup(i),spinup(i)
      write(*,'(5g10.4)') pup(1,i),pup(2,i),pup(3,i),pup(4,i),
     &                    pup(5,i)
      enddo
   
      end
