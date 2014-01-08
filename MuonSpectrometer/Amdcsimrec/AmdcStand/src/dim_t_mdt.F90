!
!> Returns the dimensions of the Station JTYP,JFF,JZZ in cm  
!> \author SamuSog
!
      SUBROUTINE dim_t_mdt(JTYP,JFF,JZZ,Tpoints)
      IMPLICIT NONE
!
      INTEGER, INTENT(IN) :: JTYP, JFF, JZZ
      REAL(8), INTENT(OUT) :: Tpoints(4)
!
      INTEGER :: IncludeFoam
!
      IncludeFoam = 0
!
      call dim_t_mdtBase(JTYP,JFF,JZZ,Tpoints,IncludeFoam)
!    
      END SUBROUTINE dim_t_mdt
!
!> Returns the dimensions of the Station JTYP,JFF,JZZ in cm  
!> \author SamuSog
!
      SUBROUTINE dim_t_mdtBase(JTYP,JFF,JZZ,Tpoints,IncludeFoam)
      IMPLICIT NONE
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER, INTENT(IN) :: JTYP, JFF, JZZ
      REAL(8), INTENT(OUT) :: Tpoints(4)
      INTEGER, INTENT(IN) ::IncludeFoam
!
      REAL(8) :: SBAREL,Z0,T0,SIZ0 
      INTEGER :: Jgeo, JOB, JTEC, JSTA, JSHA
      REAL(8) :: RTUTU,TubeLow,TubeHigh,T3Keep,T4Keep
      INTEGER :: NTUTU,iLow, iHigh
!
!   Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Tpoints(1:4 ) = 0.
!
      if (JTYP<1) return
      if ( JFF<1) return
      Jgeo = IGEO(JTYP,JFF,JZZ)
      IF( Jgeo <= 0 )    RETURN

      SBAREL = 1.D0
      IF( IBAREL(JTYP,JFF,JZZ) < 1 ) SBAREL = -1.D0
      
!   Get the station dimentions
      Z0 = Pos_Z(JTYP,JFF,JZZ) 
      T0 = Pos_R(JTYP,JFF,JZZ) 
      SIZ0 = SIGN(1.d0,Z0)

      iLow  =-1
      iHigh = 0 
      DO JOB=1,NOBJ(JTYP,JGEO)
        JTEC = ITEC(JTYP,JGEO,JOB)
        JSTA = ISTA(JTYP,JGEO,JOB)
        JSHA = ISHAPE(JTYP,JGEO,JOB)
        IF (CARTEC(JTEC) == 'MDT'.AND.JSHA == 0) THEN
!   Number of tube layers
          NTUTU = NLAZ(JTEC,JSTA)
!   Tube radius
          RTUTU = STARR(JTEC,JSTA)  
          RTUTU = RTUTU + staoo(jtec,jsta,NTUTU+1) 
!   Low/High points
          if (IncludeFoam.eq.0) then
            TubeLow  = statt(jtec,jsta,    1) - RTUTU   
            TubeHigh = statt(jtec,jsta,NTUTU) + RTUTU  
            TubeLow  = TubeLow  + Geo_dz(JTYP,JGEO,JOB) 
            TubeHigh = TubeHigh + Geo_dz(JTYP,JGEO,JOB) 
!   Alternative including foams
          else 
            TubeLow  = Geo_dz(JTYP,JGEO,JOB) 
            TubeHigh = TubeLow +STAEE(jtec,jsta) 
          endif
          IF(SBAREL > 0.) THEN
!   Rectangular (or trapezoidal) Barrel MDT chambers <<<<<<<<<<<<<<<<<<<<<<<<<<<
            iLow  = iLow  + 2
            iHigh = iHigh + 2
            Tpoints(iLow ) = T0 + TubeLow
            Tpoints(iHigh) = T0 + TubeHigh
          ELSEIF(SBAREL < 0.) THEN
!   Trapezoidal (or rectangular) End-Cap MDT chambers <<<<<<<<<<<<<<<<<<<<<<<<<<
            iLow  = iLow  + 2
            iHigh = iHigh + 2 
            Tpoints(iLow ) = Z0 + SIZ0 * TubeLow
            Tpoints(iHigh) = Z0 + SIZ0 * TubeHigh
          ENDIF      
        ENDIF
      ENDDO
      if (iLow == 1) then
        Tpoints(3) = Tpoints(1)
        Tpoints(4) = Tpoints(2)
      endif
      if ( abs(Tpoints(1)) > abs(Tpoints(3)) ) then
        T3Keep     = Tpoints(3) 
        T4Keep     = Tpoints(4)
        Tpoints(3) = Tpoints(1)
        Tpoints(4) = Tpoints(2)
        Tpoints(1) = T3Keep
        Tpoints(2) = T4Keep
      endif
      
      END SUBROUTINE dim_t_mdtBase
!
