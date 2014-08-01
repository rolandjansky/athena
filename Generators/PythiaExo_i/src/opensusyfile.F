      Subroutine opensusyfile(filnam,iunit)
      character*(*)  filnam
c      integer status
      integer iunit
c-----------------------------------------------------------------------
c  open the SUSY file
c-----------------------------------------------------------------------
      write(*,*) ' '
      write (*,6660)
 6660 format (' SUSY filename is ')
      write (*,*) filnam
      write (*,6664) iunit
 6664 format ('      and unit number is ',i6)
C
      open(unit=iunit,file=filnam,status='old',err=101)
      goto 102
c-----------------------------------------------------------------------
c  error: can't open SUSY file
c-----------------------------------------------------------------------
 101  continue
      write(*,*) ' '
      write(*,*) ' ********************************************* '
      write(*,*) ' '
      write(*,*) 
     $     ' Error in opening SUSY file for READING'
      write(*,*)
     $     ' Program will abort.'
      write(*,*) ' '
      write(*,*) ' ********************************************* '
      write(*,*) ' '
 102  continue
      end
