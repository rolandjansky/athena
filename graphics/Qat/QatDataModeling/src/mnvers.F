*
* $Id: mnvers.F,v 1.1 2008/10/27 20:58:48 jfb Exp $
*
* $Log: mnvers.F,v $
* Revision 1.1  2008/10/27 20:58:48  jfb
* commit minuit
*
* Revision 1.1  2007/01/04 15:44:42  joe
* Populating...
*
* Revision 1.1.1.1  2000/06/08 11:19:20  andras
* import of MINUIT from CERNlib 2000
*
* Revision 1.1.1.1  1996/03/07 14:31:32  mclareni
* Minuit
*
*
#include "QatDataModeling/pilot.h"
      SUBROUTINE MNVERS(CV)
#include "QatDataModeling/d506dp.inc"
CC         Returns the Minuit version in CV, char*6
CC
#include "QatDataModeling/d506cm.inc"
      CHARACTER*(*) CV
      CV = CVRSN
      RETURN
      END
