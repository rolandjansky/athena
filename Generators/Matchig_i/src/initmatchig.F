*****************************
      SUBROUTINE INITMATCHIG
*****************************
C...DOUBLE PRECISION AND INTEGER DECLARATIONS.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...EXTERNAL statement links PYDATA on most machines.
      EXTERNAL PYDATA

C...USER PROCESS INITIALIZATION COMMONBLOCK.
#include "GeneratorFortranCommon/heprup.inc"

      DOUBLE PRECISION MATCHIG

      DIMENSION DCPARM(10)
      DATA DCPARM/10*0.5d0/

      WRITE(6,*) 'MATCHIG_I: Initialising MATCHIG 2.02 for ATLAS'

      IF(EBMUP(1).EQ.0) WRITE(6,*) 'Setting ECM to default'
      IF(EBMUP(1).EQ.0) EBMUP(1)=0.5d4
      IF(EBMUP(2).EQ.0) EBMUP(2)=0.5d4

      WRITE(6,*) 'MATCHIG_I: ECM= ',EBMUP(1)+EBMUP(2)


C...Initialize widths and partial widths for resonances.
      CALL PYINRE

C...Don't use total energy as PS scale for the 2->3 process
      CALL PYGIVE('MSTP(68)=0')
C...Set the fact scale of the 2->2 process equal to the parton shower scale
      CALL PYGIVE('MSTP(32)=13')

C   Set the factor for the ISR scale of the 2->2 process to 1
c      CALL PYGIVE('PARP(67)=1d0')

C...CM Energy (LHC)
c      ECM=1.4d4    ! now obtained from Pythia

C...Set incoming beams to protons
      IDBMUP(1)=2212
      IDBMUP(2)=2212
C...Set beam energies
c      EBMUP(1)=0.5*ECM   ! now obtained from Pythia
c      EBMUP(2)=0.5*ECM   ! now obtained from Pythia

C...INTERPRETATION AF EVENT WEIGHTS: STANDARD, ALLOWING NEGATIVE WEIGHTS
      IDWTUP=-1

C...NUMBER OF DIFFERENT PROCESSES
      NPRUP=1

C...IDENTIFIER OF EXTERNAL PROCESS
      LPRUP(1)=10000

C...MAXIMUM EVENT WEIGHT FOR PROCESS (IN PB) - TRY MIDDLE VALUE OF ALL:
      XMAXUP(1)=MATCHIG(DCPARM,1d0)

      RETURN
      END

