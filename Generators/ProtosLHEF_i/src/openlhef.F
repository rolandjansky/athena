      SUBROUTINE OPENLHEF
      IMPLICIT NONE

#include "ProtosLHEF_i/Protos.inc"
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"


!     Write Lhef initialization and HEPRUP commonblock

      OPEN (24,file='events.lhe')
      WRITE (24,1000) '<LesHouchesEvents version="1.0">'
      WRITE (24,1000) '<!--'
      WRITE (24,1000) "                               "
      WRITE (24,1000) "  ______          _            "
      WRITE (24,1000) "  | ___ \        | |           "
      WRITE (24,1000) "  | |_/ / __ ___ | |_ ___  ___ "
      WRITE (24,1000) "  |  __/ '__/ _ \| __/ _ \/ __|"
      WRITE (24,1000) "  | |  | | | (_) | || (_) \__ \"
      WRITE (24,1000) "  \_|  |_|  \___/ \__\___/|___/"
      WRITE (24,1000) "                               "
      WRITE (24,1000) " ***************************************"
      WRITE (24,1000) " * ProtosLHEF_i interface for ATHENA   *"
      WRITE (24,1000) " * version 00-00-03        27/3/2015   *"
      WRITE (24,1000) " *                                     *"
      WRITE (24,1000) " * Patricia Fernandez                  *"
      WRITE (24,1000) " * patricia.fernandez.martinez@cern.ch *"
      WRITE (24,1000) " *                                     *"
      WRITE (24,1000) " *                                     *"
      WRITE (24,1000) " * Protos generator developed by       *"
      WRITE (24,1000) " * Juan Antonio Aguilar Saavedra       *"
      WRITE (24,1000) " * Universidad de Granada              *"
      WRITE (24,1000) " ***************************************"
      WRITE (24,1000) "                                        "
      WRITE (24,1000) " ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
      WRITE (24,999) " :: LHC considered: pp collisions at sqrt(s) = ",ET," GeV  ::"
      WRITE (24,999) " ::                 Beam energy = ",ET/2d0," GeV               ::"
      WRITE (24,1000) " ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::"
      WRITE (24,1000) " ______________________________________________________________ "
      WRITE (24,1000) "|                          protoslhef.dat                          | "
      WRITE (24,1000) " -------------------------------------------------------------- "
      DO i=1,in
         WRITE (24,998) string(i)
      ENDDO
      WRITE (24,1000) " ______________________________________________________________ "
      WRITE (24,1000) " -------------------------------------------------------------- "
      WRITE (24,1000) '-->'
 
      WRITE (24,1000) '<header>'
      WRITE (24,1000) ' <!-- individually designed XML tags, in fancy XML style -->'
      WRITE (24,1000) '</header>'

      WRITE (24,1000) '<init>'
      WRITE (24,1001) IDBMUP(1),IDBMUP(2),EBMUP(1),EBMUP(2),PDFGUP(1),PDFGUP(2),PDFSUP(1),PDFSUP(2),IDWTUP,NPRUP
      DO i=1,NPRUP
         WRITE (24,1002) XSECUP(i),XERRUP(i),XMAXUP(i),LPRUP(i)
      ENDDO
      WRITE (24,1000) '</init>'

C---------------------------------------------------------------------------------

 998    FORMAT (' ',A)
 999    FORMAT (A,F8.2,A)
 1000   FORMAT (A)
 1001   FORMAT (' ',I4,' ',I4,' ',F8.2,' ',F8.2,' ',I5,' ',I5,' ',I5,' ',I5,' ',I2,' ',I3)
 1002   FORMAT (' ',F8.2,' ',F8.2,' ',F8.2,' ',I5)

      END
