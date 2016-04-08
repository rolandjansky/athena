*
* $Id: mnseti.F,v 1.1 2008/10/27 20:58:48 jfb Exp $
*
* $Log: mnseti.F,v $
* Revision 1.1  2008/10/27 20:58:48  jfb
* commit minuit
*
* Revision 1.1  2007/01/04 15:44:42  joe
* Populating...
*
* Revision 1.1.1.1  2000/06/08 11:19:20  andras
* import of MINUIT from CERNlib 2000
*
* Revision 1.1.1.1  1996/03/07 14:31:31  mclareni
* Minuit
*
*
#include "QatDataModeling/pilot.h"
      SUBROUTINE MNSETI(TIT)
#include "QatDataModeling/d506dp.inc"
CC       Called by user to set or change title of current task.
CC
#include "QatDataModeling/d506cm.inc"
      CHARACTER*(*) TIT
      CTITL = TIT
      RETURN
      END
