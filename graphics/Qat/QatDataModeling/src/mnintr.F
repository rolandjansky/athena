*
* $Id: mnintr.F,v 1.1 2008/10/27 20:58:48 jfb Exp $
*
* $Log: mnintr.F,v $
* Revision 1.1  2008/10/27 20:58:48  jfb
* commit minuit
*
* Revision 1.1  2007/01/04 15:44:42  joe
* Populating...
*
* Revision 1.1.1.1  2000/06/08 11:19:20  andras
* import of MINUIT from CERNlib 2000
*
* Revision 1.1.1.1  1996/03/07 14:31:30  mclareni
* Minuit
*
*
#include "QatDataModeling/pilot.h"
      SUBROUTINE MNINTR(FCN,FUTIL)
#include "QatDataModeling/d506dp.inc"
CC       Called by user. Interfaces to MNREAD to allow user to change
CC       easily from Fortran-callable to interactive mode.
CC
#include "QatDataModeling/d506cm.inc"
      EXTERNAL FCN,FUTIL
      IFLGIN = 3
      CALL MNREAD(FCN,IFLGIN,IFLGUT,FUTIL)
      WRITE (ISYSWR,'(2A/)')  ' END OF MINUIT COMMAND INPUT. ',
     +      '   RETURN TO USER PROGRAM.'
      RETURN
      END
