!
!> Define the crossings of eta and phi RPC/TGC strips and create ROA in which segments
!> will be looked for
!> \author A.Ouraou
!
 SUBROUTINE doAllowCell(NROA, TEROA,FIROA,DTEROA,DFIROA)
 USE M_MB_MuGeom
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NROA
 REAL(8), INTENT(IN) :: TEROA(*), FIROA(*), DTEROA(*), DFIROA(*)
 REAL(8), PARAMETER :: FACLUS = 1.9D0
 INTEGER, PARAMETER :: NSTRIMU=200
 INTEGER :: JSBUN(NSTRIMU), JZBUN(NSTRIMU)
 REAL(8) :: XSBUN(NSTRIMU), XZBUN(NSTRIMU)
 INTEGER :: NbSBUN, NbZBUN
 INTEGER :: ICT, IRPC, ItrTRI, IP, IS, LS, LZ
 REAL(8) :: Z0, DZ, S1P, S1N, S2P, S2N, AA, AM1, AP1, ASS
 REAL(8) :: FI, TI, S, Z, Strue, Ztrue, Ttrue
 REAL(8) :: FFF,RRR, THEcur,PHIcur,R3Dcur
 REAL(8) :: PIMIPI
 LOGICAL :: LOKROA
!
    DO ICT=1,NTRITO
      IRPC  = IRPTGC(ICT)
!
!-- Loops on all the strips of all the sensitive layers (IP) of trigger chamber ICT
      NbSBUN = 0
      NbZBUN = 0
      DO IP=1,N2BAND(IRPC)
        DO IS=NBANH0(ICT,IP)+1,NBANH0(ICT,IP)+NBANHI(ICT,IP)
          IF( LBANHI(IS) < 0  )  CYCLE
!         IF( rejectStrip(IS) )  CYCLE
          IF( IP <= N1BAND(IRPC) ) THEN
!-- Define the allowed S value for each hit S strip of trigger chamber ICT -> XSBUN
            NbSBUN = MIN( NbSBUN+1 , NSTRIMU )
            JSBUN(NbSBUN) = 1
            XSBUN(NbSBUN) = SBANHI(IS)
          ELSE
!-- Define the allowed Z value for each hit Z strip of trigger chamber ICT -> XZBUN
            NbZBUN = MIN( NbZBUN+1 , NSTRIMU )
            JZBUN(NbZBUN) = 1
            XZBUN(NbZBUN) = SBANHI(IS)
          ENDIF
        ENDDO
      ENDDO
!
      IF( NbSBUN <= 0 .OR. NbZBUN <= 0 )  CYCLE
      ItrTRI= ItransTRI(ICT)
      FI  = FCETRI(ICT)
      TI  = TCETRI(ICT)
      Z0  = ZCETRI(ICT)
      DZ  = ZLOTRI(ICT) / 2.D0
      S1P = SPITRI(ICT)
      S1N = SNITRI(ICT)
      S2P = SPATRI(ICT)
      S2N = SNATRI(ICT)
      AA  = (S2P-S2N) / (S1P-S1N)
      AM1 = AA - 1.D0
      AP1 = AA + 1.D0
      ASS = AA * (S2P-S1P)
!
!-- Clusterizes the strips in trigger chamber ICT :  XSBUN  and  XZBUN
      CALL Mubo_Cluster( FACLUS*DSBAND(IRPC), NbSBUN, XSBUN,JSBUN )
      CALL Mubo_Cluster( FACLUS*DZBAND(IRPC), NbZBUN, XZBUN,JZBUN )
!
!-- Accumulate the allowed S values for multilayer IC,IW in XSBIN :  XSBUN -> XSBIN
      DO LS=1,NbSBUN
        IF( JSBUN(LS) < 1 ) CYCLE
!-- If the current (Theta,Phi) is inside ROAs,
!-- update the "allowed" (Theta,Phi) cells from the strip hits
        DO LZ=1,NbZBUN
          IF( JZBUN(LZ) < 1 ) CYCLE
          S = XSBUN(LS)
          Z = XZBUN(LZ)
          S = S + (Z-Z0)*(ASS+AM1*(S-S2P))/AP1/DZ
          CALL Pos_True_from_Ideal( ItrTRI, S, Z, TI, Strue, Ztrue, Ttrue )
          IF( ICT <= NTRIBA ) THEN
            RRR = SQRT( Ttrue**2 + Strue**2 )
            FFF = FI + ATAN2( Strue , Ttrue )
            THEcur = ATAN2( RRR , Ztrue )
            PHIcur = PIMIPI( FFF )
          ELSE
            FFF = FI + ATAN2( Strue , Ztrue )
            THEcur = ATAN2( Ztrue , Ttrue )
            PHIcur = PIMIPI( FFF )
          ENDIF
          IF( LOKROA(THEcur,PHIcur,NROA,TEROA,FIROA,DTEROA,DFIROA) ) THEN
            R3Dcur = SQRT( Strue**2 + Ztrue**2 + Ttrue**2 )
            CALL CreateAlow(R3Dcur,THEcur,PHIcur)
          ENDIF
        ENDDO
      ENDDO
!
    ENDDO
!
 END SUBROUTINE doAllowCell
!
