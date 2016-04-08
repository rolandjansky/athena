      subroutine loadTable(lnup,lidprup,listup,lidup,
     &                     lmothup,licolup,
     &                     numcolor,numdecay,numEntry)

c     loads the hepup-format lookup table

      implicit none
#include "MadCUP_i/iofiles.inc"

      integer maxNumSubProc
      parameter( maxNumSubProc = 1000 )
      integer LMAXNUP
      parameter ( LMAXNUP = 20 )

      integer thisProcess
      integer lnup(maxNumSubProc), lidprup(maxNumSubProc)
      integer LISTUP(LMAXNUP,maxNumSubProc)
      integer LIDUP(LMAXNUP,maxNumSubProc)
      integer LMOTHUP(2,LMAXNUP,maxNumSubProc)
      integer LICOLUP(2,LMAXNUP,maxNumSubProc)
      integer numcolor,numdecay, numEntry(maxNumSubProc)

c  local variables
      integer i,j,np,procNum


      read(unitnumber,'(7i5)') numdecay,thisProcess
c      write(*,*) 'numdecay=',numdecay
      do i=1,numdecay
         read(unitnumber,'(3i8)') ProcNum,lnup(i),numEntry(i)
         lidprup(i) = thisprocess
c         write(*,*)'i=',i
         if(i.ne.ProcNum) then
            write(*,*)'error: decaylookup data not in order'
            write(*,*)'for decayprocess = ',ProcNum
            print*,i,numdecay,numcolor
c            stop
         endif

         np=lnup(i)-numEntry(i)
         do j=np+1,lnup(i)
            read(unitnumber,'(i4,i6,4i5)') listup(j,i), lidup(j,i),
     &           lmothup(1,j,i),lmothup(2,j,i),
     &           licolup(1,j,i),licolup(2,j,i)
c            print*,' l info for procnum ',i
c            write(*,'(i4,i6,4i5)')
c     &           listup(j,i), lidup(j,i),
c     &           lmothup(1,j,i),lmothup(2,j,i),
c     &           licolup(1,j,i),licolup(2,j,i)
         enddo
      enddo

      read(unitnumber,'(7i5)') numcolor,thisProcess
      do i=1+numdecay,numcolor+numdecay
         read(unitnumber,'(3i8)') ProcNum,lnup(i),numEntry(i)
         lidprup(i) = thisprocess
c        write(*,*) "i=",i
         if(i-numdecay.ne.ProcNum) then
            write(*,*)'error: colorlookup data not in order.'
            write(*,*)'for colorprocess = ',ProcNum
            stop
         endif

         do j=1,numEntry(i)
            read(unitnumber,'(i4,i6,4i5)') listup(j,i), lidup(j,i),
     &           lmothup(1,j,i),lmothup(2,j,i),
     &           licolup(1,j,i),licolup(2,j,i)
c            write(*,'(i4,i6,4i5)')
c     &           listup(j,i), lidup(j,i),
c     &           lmothup(1,j,i),lmothup(2,j,i),
c     &           licolup(1,j,i),licolup(2,j,i)
         enddo
      enddo
      end

