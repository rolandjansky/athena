      subroutine INITMADCUP
      implicit none
#include "GeneratorFortranCommon/heprup.inc"      
#include "GeneratorFortranCommon/hepeup.inc"
#include "MadCUP_i/iofiles.inc"
#include "MadCUP_i/hepup.inc"

c  local variables
      integer nevt,lineskip
      character*50 text,title

      integer i
c     ,ievt
c      data ievt /0/
c      save ievt
      logical ldebug
      parameter (ldebug=.false.)

c  variables for figuring out status of io file.
C      character*50 howConnected
C      integer EDF_is_open
      logical exists
      logical file_opened,unit_opened

c  open input file if necessary

      inquire(UNIT=unitnumber,OPENED=unit_opened)
      inquire(FILE=eventDataFile,EXIST=exists,OPENED=file_opened,
     &          NUMBER=unitnumber)

      if(.not.file_opened ) then
c       start at unitnumber=14 and check units until we find one that-s not open.
        unitnumber=14  !need to set this again because of the 2nd inquire statement above
        if(ldebug) then
          write(*,*) 'looking for valid unit number for
     & event data file.....'
        endif
        do while(unit_opened)
          if(ldebug) then    
            write(*,*) 'unit number ',unitnumber,' is already taken;
     &  trying again...'
          endif
          unitnumber=unitnumber+1
          inquire(UNIT=unitnumber,OPENED=unit_opened)   
        enddo
c       now we heve established that eventDataFile and unitnumber are not connected
        if(ldebug) then
          write(*,*) 'looks like unit number',unitnumber,
     &              ' is ok, opening file...' 
        endif
        open(unitnumber,file=eventDataFile,status='old')
      endif



c  read in lookup table and HEPRUP common block
      read(unitnumber,'(a)') title
      read(unitnumber,'(i10,a)') nevt,text
      read(unitnumber,'(i10,a)') lineskip,text
      do i = 1,lineskip
         read(unitnumber,'(a)') text
      enddo
      read(unitnumber,'(2i8,2g14.4,a)') IDBMUP(1),IDBMUP(2),
     &                                EBMUP(1),EBMUP(2),text
      read(unitnumber,'(4i8,a)') PDFGUP(1),PDFGUP(2),PDFSUP(1),
     &                          PDFSUP(2),text
      read(unitnumber,'(2i8,a)') IDWTUP, NPRUP,text
      if (nprup.ne.1) then
         print*,' You requested ',nprup,' subprocesses. '
         print*,' Sorry. Not implemented yet. Max number is 1. '
         stop
      endif
      do i = 1,NPRUP
        read(unitnumber,'(2g14.4,g18.8,i3,a)') XSECUP(I), XERRUP(I),
     1                                XMAXUP(I), LPRUP(I),text
      enddo
      call loadTable(lnup,lidprup,listup,lidup,
     &               lmothup,licolup,numcolor,numdecay,numEntry)

      end
