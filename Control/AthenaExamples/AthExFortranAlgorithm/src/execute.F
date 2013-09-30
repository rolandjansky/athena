      subroutine execute(lun)
      integer lun
*
      read (lun,1000) a1
      
      print 2001,a1
*     reading 2
      read (lun,1000) a1
      print 2002,a1
*     rewind
      print 3000
      rewind lun
*     reading 3
      read (lun,1000) a1
      print 2003,a1
*     backspace 
      print 3010
      backspace lun
*     reading 4
      read (lun,1000) a1
      print 2004,a1
*     reading 5
      read (lun,1000) a1
      print 2005,a1
 1000 format(a1)
*     
 2001 format('Reading 1',a1)
 2002 format('Reading 2',a1)
 2003 format('Reading 3',a1)
 2004 format('Reading 4',a1)
 2005 format('Reading 5',a1)
*     
 3000 format('Before the Rewind')
 3010 format('Before the backspace')
*     
      return
      end      
