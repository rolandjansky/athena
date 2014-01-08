!
!> Returns the dimensions of the Station JTYP,JFF,JZZ in cm !!!
!> \author SaMuSoG   13/11/96
!
 SUBROUTINE Dim_Station(JTYP,JFF,JZZ,                      &
                        SC_Station,ZC_Station,TC_Station,  &
                        DS_Station,DZ_Station,DT_Station)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: JTYP, JFF, JZZ
 REAL(8), INTENT(OUT) :: SC_Station, ZC_Station, TC_Station
 REAL(8), INTENT(OUT) :: DS_Station, DZ_Station, DT_Station
 INTEGER :: Jgeo, Jcut, IsValid
 REAL(8) :: DimLocX,    DimLocY,    DimLocZ
 REAL(8) :: CenterLocX, CenterLocY, CenterLocZ
!
#include "AmdcStand/amdcsim_com.inc"
!
      SC_Station = 0.D0
      ZC_Station = 0.D0
      TC_Station = 0.D0
      DS_Station = 0.D0
      DZ_Station = 0.D0
      DT_Station = 0.D0
!
      Jgeo = IGEO_V(JTYP_F_Z(JTYP,JFF,JZZ))
      IF( Jgeo <= 0 )    RETURN
      Jcut = ICUT_V(JTYP_F_Z(JTYP,JFF,JZZ))
      CALL FGetStationDimensionsJtypBased( JTYP,Jgeo,Jcut, IsValid,   &
          DimLocX,DimLocY,DimLocZ, CenterLocX,CenterLocY,CenterLocZ )
!
      IF( IsValid <= 0 ) RETURN
!
      SC_Station = CenterLocX / 10.D0
      ZC_Station = CenterLocY / 10.D0
      TC_Station = CenterLocZ / 10.D0
      DS_Station = DimLocX    / 10.D0
      DZ_Station = DimLocY    / 10.D0
      DT_Station = DimLocZ    / 10.D0
!
 END SUBROUTINE Dim_Station
!
