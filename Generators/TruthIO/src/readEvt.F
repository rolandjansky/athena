      subroutine openFile(ifile,filename)
      integer ifile
      character*(*) filename
      logical ex

      inquire(FILE=filename,EXIST=ex)
      if(.not. ex) write(*,*) 'File does not exist:',filename
      open(ifile,FILE=filename,status='old')
      end

      subroutine readEvt(ifile)
      integer ifile
c...Hepevt common block
      integer nmxhep,nevhep,nhep,isthep,idhep,jmohep,jdahep
      double precision phep,vhep
      PARAMETER (NMXHEP=10000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)

      integer i

      read(ifile,*) nevhep,nhep
      do i=1,nhep
        read(ifile,*) dummy,isthep(i),idhep(i),jmohep(1,i),
     & jmohep(2,i),jdahep(1,i),jdahep(2,i)
        read(ifile,*) phep(1,i),phep(2,i),phep(3,i),phep(4,i),phep(5,i)
        read(ifile,*) vhep(1,i),vhep(2,i),vhep(3,i),vhep(4,i)
      enddo

      end

      subroutine closeFile(ifile)
      integer ifile

      close(ifile)

      end
