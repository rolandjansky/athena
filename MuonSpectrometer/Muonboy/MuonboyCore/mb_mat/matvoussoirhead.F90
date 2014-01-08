!
 SUBROUTINE MATVOUSSOIRHEAD(F0Q,JFF,IVOUS,YVOUSSOIR)
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: JFF, IVOUS
 REAL(8) :: F0Q, YVOUSSOIR
!
#include "AmdcStand/comamu.inc"
!
      INTEGER :: IV1, IV2, IVC
      REAL(8) :: X0, Y0, ROT, SINROT, COSROT
      REAL(8) :: X1, X2, Y1, Y2, R0, X1R, X2R, Y1R, Y2R
      REAL(8) :: XV(6), YV(6), VV(18)

!- Voussoir Head core volume
      X0=-266.6
      Y0= 643.6
      ROT=Pi/8.D0
      SINROT = SIN(ROT)
      COSROT = COS(ROT)

      XV(1)= 41.7
      XV(2)= 31.7+32.3
      XV(3)= 31.7
      XV(4)=-XV(3)
      XV(5)=-XV(2)
      XV(6)=-XV(1)

      YV((/1,6/))= 0.
      YV((/2,5/))= 0.-10.
      YV((/3,4/))= 0.-88.

      VV(1:16:3) = X0 + XV(1:6)*COSROT - YV(1:6)*SINROT
      VV(2:17:3) = Y0 + XV(1:6)*SINROT + YV(1:6)*COSROT
      VV(3:18:3) = ZVOUS(IVOUS)-55.0/2.
      YVOUSSOIR=VV(8)

!- Voussoir Head
      CALL NEWMAT( 'VousHead', MATHEA, JFF, IVOUS )
      CALL PLAMATXX( F0Q, 6, VV )
      VV(3:18:3) = ZVOUS(IVOUS)+55.0/2.
      CALL PLAMATXX( F0Q, 6, VV )
      CALL CLOMAT
      CALL ENDMAT
      IV1 = NVMATI

!- Voussoir Head cuts
      CALL NEWMAT( 'PCutVoHe', MATVOU, JFF, IVOUS )

      YV((/1,4/))=     10.
      YV((/2,3/))=-88.-10.
      XV((/1,2/))=30.+50.
      XV((/3,4/))=30.
      VV(2:11:3) = Y0 + XV(1:4)*SINROT + YV(1:4)*COSROT
      VV(1:10:3) = X0 + XV(1:4)*COSROT - YV(1:4)*SINROT
      VV(3:12:3) = ZVOUS(IVOUS)+40./2.
      CALL PLAMATXX( F0Q, 4, VV )
      VV(3:12:3) = ZVOUS(IVOUS)+50.
      CALL PLAMATXX( F0Q, 4, VV )
      CALL CLOMAT

      VV(3:12:3) = ZVOUS(IVOUS)-40./2.
      CALL PLAMATXX( F0Q, 4, VV )
      VV(3:12:3) = ZVOUS(IVOUS)-50.
      CALL PLAMATXX( F0Q, 4, VV )
      CALL CLOMAT

      VV(1:10:3) = X0 + (-1.)*XV(1:4)*COSROT - YV(1:4)*SINROT
      VV(2:11:3) = Y0 + (-1.)*XV(1:4)*SINROT + YV(1:4)*COSROT
      VV(3:12:3) = ZVOUS(IVOUS)+40./2.
      CALL PLAMATXX( F0Q, 4, VV )
      VV(3:12:3) = ZVOUS(IVOUS)+50.
      CALL PLAMATXX( F0Q, 4, VV )
      CALL CLOMAT

      VV(3:12:3) = ZVOUS(IVOUS)-40./2.
      CALL PLAMATXX( F0Q, 4, VV )
      VV(3:12:3) = ZVOUS(IVOUS)-50.
      CALL PLAMATXX( F0Q, 4, VV )
      CALL CLOMAT

      X1=0.
      X2=0.
      Y1=-88.-10.
      Y2=-10.
      R0= 46.8/2.
      X1R = X0 + X1*COSROT - Y1*SINROT
      Y1R = Y0 + X1*SINROT + Y1*COSROT
      X2R = X0 + X2*COSROT - Y2*SINROT
      Y2R = Y0 + X2*SINROT + Y2*COSROT
      CALL DVPIPE( F0Q, 16, Zero,Z360,    &
         X1R,Y1R,ZVOUS(IVOUS), R0,Zero,   &
         X2R,Y2R,ZVOUS(IVOUS), R0,Zero )

      Y1= 10.
      Y2= -9.9
      R0= 12./2.
      X1R = X0 + X1*COSROT - Y1*SINROT
      Y1R = Y0 + X1*SINROT + Y1*COSROT
      X2R = X0 + X2*COSROT - Y2*SINROT
      Y2R = Y0 + X2*SINROT + Y2*COSROT
      CALL DVPIPE( F0Q, 16, Zero,Z360,    &
         X1R,Y1R,ZVOUS(IVOUS), R0,Zero,   &
         X2R,Y2R,ZVOUS(IVOUS), R0,Zero )

      CALL ENDMAT
      IV2 = NVMATI

!- Perform cuts
      CALL BOOLMAT( 1,0, IV1,IV2, 'VousHead',MATVOU,JFF,IVOUS, IVC )

 END SUBROUTINE MATVOUSSOIRHEAD
!
