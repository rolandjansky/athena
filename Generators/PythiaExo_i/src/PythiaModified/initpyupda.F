C...UPINIT
C...Routine called by PYINIT to set up user-defined processes.
      
      SUBROUTINE INITPYUPDA(filnam)
      character*(*)  filnam


C--GUP common block
#include "GeneratorFortranCommon/heprup.inc"

C...EXTERNAL statement links PYDATA on most machines
      EXTERNAL PYDATA

C     PYTHIA Particles Properties common Blocks
      INTEGER KCHG,MDCY,MDME,KFDP
      DOUBLE PRECISION PMAS,PARF,VCKM,BRAT
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)

      WRITE(*,*) filnam
      OPEN(UNIT=76,STATUS='OLD',FILE=filnam)
      CALL PYUPDA(3,76)
      CLOSE(76)

      RETURN
      END
