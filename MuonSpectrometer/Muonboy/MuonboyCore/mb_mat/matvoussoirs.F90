!
 SUBROUTINE MATVOUSSOIRS(F0Q,JFF,IVOUS,ZHEAD,YVOUSSOIR)
 USE M_MB_General_CONSTANTS, ONLY : Zero, Z360
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: JFF, IVOUS
 REAL(8) :: F0Q, ZHEAD, YVOUSSOIR
!
#include "AmdcStand/comamu.inc"
!
      INTEGER :: IV1, IV2, IVC
      REAL(8) :: X0, Y0, R0, DANGLE
      REAL(8) :: XV(8), YV(8), VV(24)


!- Voussoir core volume
      XV((/1,2/))=-200.
      XV((/3,4/))= 200.
      YV((/1,4/))= YVOUSSOIR
      YV((/2,3/))= YVOUSSOIR+85.
      VV(1:10:3) = XV(1:4)
      VV(2:11:3) = YV(1:4)
      VV(3:12:3) = ZVOUS(IVOUS)-ZHEAD
      CALL NEWMAT( 'Voussoir', MATVOU, JFF, IVOUS )
      CALL PLAMATXX( F0Q, 4, VV )
      VV(3:12:3) = ZVOUS(IVOUS)+ZHEAD
      CALL PLAMATXX( F0Q, 4, VV )
      CALL CLOMAT
      CALL ENDMAT
      IV1 = NVMATI


!- Voussoir round cuts
      X0=-200.+ 5.+59.5+5.+81.5/2.
      Y0= YVOUSSOIR+12.+(85.-12.-8.)/2.
      R0= 30.
      CALL NEWMAT( 'RCutVous', MATVOU, JFF, IVOUS )
      CALL DVPIPE( F0Q, 16, Zero,Z360,      &
         X0,Y0,ZVOUS(IVOUS)-2.51, R0,Zero,  &
         X0,Y0,ZVOUS(IVOUS)+2.51, R0,Zero )
      CALL DVPIPE( F0Q, 16, Zero,Z360,      &
        -X0,Y0,ZVOUS(IVOUS)-2.51, R0,Zero,  &
        -X0,Y0,ZVOUS(IVOUS)+2.51, R0,Zero )
      CALL ENDMAT
      IV2 = NVMATI

!- Perform round cuts
      CALL BOOLMAT( 1,0, IV1,IV2, 'Voussoir',MATVOU,JFF,IVOUS, IVC )
      IV1 = IVC


!- Voussoir "square" cuts
      CALL NEWMAT( 'PCutVous', MATVOU, JFF, IVOUS )

!- Voussoir cut 1
      DANGLE=3.
      XV((/2,3/))=-200.+ 5.
      XV((/1,4/))=-200.+ 5.      + DANGLE
      XV((/5,8/))=-200.+ 5.+59.5 - DANGLE
      XV((/6,7/))=-200.+ 5.+59.5
      YV((/1,8/))= YVOUSSOIR+12.
      YV((/2,7/))= YVOUSSOIR+12.    + DANGLE
      YV((/3,6/))= YVOUSSOIR+85.-8. - DANGLE
      YV((/4,5/))= YVOUSSOIR+85.-8.
      VV(1:22:3) = XV(1:8)
      VV(2:23:3) = YV(1:8)
      VV(3:24:3) = ZVOUS(IVOUS)-2.*ZHEAD

!- Voussoir cut 1a
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)+2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 1b
      VV(1:22:3) = -XV(1:8)
      VV(3:24:3) = ZVOUS(IVOUS)-2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)+2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 2
      XV((/2,3/))=-200.+ 5.+59.5+5.
      XV((/1,4/))=-200.+ 5.+59.5+5.      + DANGLE
      XV((/5,8/))=-200.+ 5.+59.5+5.+81.5 - DANGLE
      XV((/6,7/))=-200.+ 5.+59.5+5.+81.5
      VV(1:22:3) = XV(1:8)

!- Voussoir cut 2a+
      VV(3:24:3) = ZVOUS(IVOUS)+2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)+ 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 2a-
      VV(3:24:3) = ZVOUS(IVOUS)-2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)- 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

      VV(1:22:3) = -XV(1:8)

!- Voussoir cut 2b+
      VV(3:24:3) = ZVOUS(IVOUS)+2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)+ 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 2b-
      VV(3:24:3) = ZVOUS(IVOUS)-2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)- 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 3
      XV((/2,3/))=-200.+ 5.+59.5+5.+81.5+5.
      XV((/1,4/))=-200.+ 5.+59.5+5.+81.5+5. + DANGLE
      XV((/5,8/))=-XV(1)
      XV((/6,7/))=-XV(2)
      VV(1:22:3) = XV(1:8)

!- Voussoir cut 3+
      VV(3:24:3) = ZVOUS(IVOUS)+2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)+ 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

!- Voussoir cut 3-
      VV(3:24:3) = ZVOUS(IVOUS)-2.*ZHEAD
      CALL PLAMATXX( F0Q, 8, VV )
      VV(3:24:3) = ZVOUS(IVOUS)- 2.5
      CALL PLAMATXX( F0Q, 8, VV )
      CALL CLOMAT

      CALL ENDMAT
      IV2 = NVMATI


!- Perform cuts
      CALL BOOLMAT( 1,0, IV1,IV2, 'Voussoir',MATVOU,JFF,IVOUS, IVC )

 END SUBROUTINE MATVOUSSOIRS
!
