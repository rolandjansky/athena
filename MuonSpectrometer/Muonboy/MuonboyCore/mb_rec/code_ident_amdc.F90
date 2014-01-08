!
!> \author M.Virchaux
!
 SUBROUTINE Code_Ident_AMDC(JTYP,JFF,JZZ,JOB,JSL,JSZ,Jchan, Ident)
 IMPLICIT NONE
#include "AmdcStand/amdcsim_param.inc"
 INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ,JOB,JSL,JSZ,Jchan
 INTEGER, INTENT(OUT) :: Ident
 INTEGER              :: MZZ2P1,MCHAN
!
    MZZ2P1 = 2*MZZ+1
    MCHAN  = 2140000000 / 128 / MZZ2P1 / MOBJ / (MTYP+1)
!
    Ident = MAX(1,MIN(  JTYP   ,MTYP    ))*MCHAN*4*4*MOBJ*MZZ2P1*8   &
          + MAX(0,MIN(  (JFF-1),   7    ))*MCHAN*4*4*MOBJ*MZZ2P1     &
          + MAX(0,MIN((JZZ+MZZ),MZZ2P1-1))*MCHAN*4*4*MOBJ            &
          + MAX(0,MIN(  (JOB-1),MOBJ-1  ))*MCHAN*4*4                 &
          + MAX(0,MIN(  (JSL-1),   3    ))*MCHAN*4                   &
          + MAX(0,MIN(  (JSZ-1),   3    ))*MCHAN                     &
          + MAX(0,MIN((Jchan-1),MCHAN-1 ))
!
 END SUBROUTINE Code_Ident_AMDC
!
!
!
 SUBROUTINE Deco_Ident_AMDC(Ident, JTYP,JFF,JZZ,JOB,JSL,JSZ,Jchan)
 IMPLICIT NONE
#include "AmdcStand/amdcsim_param.inc"
 INTEGER, INTENT(IN)  :: Ident
 INTEGER, INTENT(OUT) :: JTYP,JFF,JZZ,JOB,JSL,JSZ,Jchan
 INTEGER              :: MZZ2P1,MCHAN,J,J1,J2,J3,J4,J5,J6,J7
!
    MZZ2P1 = 2*MZZ+1
    MCHAN  = 2140000000 / 128 / MZZ2P1 / MOBJ / (MTYP+1)
!
    J1 = Ident / (MCHAN*4*4*MOBJ*MZZ2P1*8)
    J  = Ident - (MCHAN*4*4*MOBJ*MZZ2P1*8)*J1
    J2 =   J   / (MCHAN*4*4*MOBJ*MZZ2P1)
    J  =   J   - (MCHAN*4*4*MOBJ*MZZ2P1)  *J2
    J3 =   J   / (MCHAN*4*4*MOBJ)
    J  =   J   - (MCHAN*4*4*MOBJ)         *J3
    J4 =   J   / (MCHAN*4*4)
    J  =   J   - (MCHAN*4*4)              *J4
    J5 =   J   / (MCHAN*4)
    J  =   J   - (MCHAN*4)                *J5
    J6 =   J   /  MCHAN
    J7 =   J   -  MCHAN                   *J6
!
    JTYP  = J1
    JFF   = J2 + 1
    JZZ   = J3 - MZZ
    JOB   = J4 + 1
    JSL   = J5 + 1
    JSZ   = J6 + 1
    Jchan = J7 + 1
!
 END SUBROUTINE Deco_Ident_AMDC
!
