      subroutine initialize(lun, fileName)
      character *(*) fileName
      integer lun
      open( unit=lun, file=fileName, status='old')
      return
      end      
