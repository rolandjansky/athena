!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetTubePositionFromMDTindex( Ctyp,Jff,Jzz,Job,Jsl, Jtube, MDTnu, XYZabs, SZTloc, Radius )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jsl,Jtube,MDTnu
 REAL(8),      INTENT(OUT) :: XYZabs(3,2),SZTloc(3,2),Radius
 INTEGER :: JTROU3
 INTEGER :: Jtyp
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
!
    CALL FGetTubePositionFromMDTindexJtypBased( Jtyp,Jff,Jzz,Job,Jsl, Jtube, MDTnu, XYZabs, SZTloc, Radius )
!
 END SUBROUTINE FGetTubePositionFromMDTindex
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetTubePositionFromMDTindexJtypBased( Jtyp,Jff,Jzz,Job,Jsl, Jtube, MDTnu, XYZabs, SZTloc, Radius )
!
      IMPLICIT REAL(8) (A-H,O-Z)
      INTEGER, INTENT(IN)  :: Jtyp
      INTEGER, INTENT(IN)  :: Jff,Jzz,Job,Jsl,Jtube,MDTnu
      REAL(8), INTENT(OUT) :: XYZabs(3,2),SZTloc(3,2),Radius
!
      REAL(8)            :: SZT(3),SZTtru(3,2)
      REAL(8), PARAMETER :: Decal = 1.000D0
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      XYZabs(1:3,1:2) = 0.D0
      SZTloc(1:3,1:2) = 0.D0
      Radius          = 0.D0
!
      IF( Jtyp < 1 )               RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )               RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'MDT' )  RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )               RETURN
      Nsl  = NLAZ(Jtec,Jsta)
      IF( Jsl < 1 .OR. Jsl > Nsl ) RETURN
!
      DzLoc = STAOO(Jtec,Jsta,Jsl) + STAPP(Jtec,Jsta)*(Jtube-1)
      DtLoc = STATT(Jtec,Jsta,Jsl)
!
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        Tpos = Pos_R(Jtyp,Jff,Jzz) + Geo_dz(Jtyp,Jgeo,Job) + DtLoc
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          IF( Jzz < 0 .AND. MDTnu < 0 ) THEN
            Zpos = Pos_Z(Jtyp,Jff,Jzz) - Geo_dy(Jtyp,Jgeo,Job) - DzLoc + 20.D0*ZC_Sta
          ELSE
            Zpos = Pos_Z(Jtyp,Jff,Jzz) - Geo_dy(Jtyp,Jgeo,Job) - Geo_Le(Jtyp,Jgeo,Job) + DzLoc + 20.D0*ZC_Sta
          ENDIF
          Siin = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)        &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Wl(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)        &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Ws(Jtyp,Jgeo,Job)
        ELSE
          IF( Jzz < 0 .AND. MDTnu < 0 ) THEN
            Zpos = Pos_Z(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job) + Geo_Le(Jtyp,Jgeo,Job) - DzLoc
          ELSE
            Zpos = Pos_Z(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job) + DzLoc
          ENDIF
          Siin = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)         &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0    &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)         &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0    &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ELSE
        Sisi = SIGN( 1.D0 , Pos_Z(Jtyp,Jff,Jzz) )
        Zpos    = Pos_R(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job) + DzLoc
        sis0 = 1.D0
        IF( IVERSION_AMDC > 6 .AND. Jzz > 0 ) sis0 = -1.D0
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          Tpos = Pos_Z(Jtyp,Jff,Jzz) - Sisi*Geo_dz(Jtyp,Jgeo,Job)       &
                                     - Sisi*DtLoc + Sisi*20.D0*TC_Sta
          Siin = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)               &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0   &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)               &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0   &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
            Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ELSE
          Tpos = Pos_Z(Jtyp,Jff,Jzz) + Sisi*Geo_dz(Jtyp,Jgeo,Job)    &
                                     + Sisi*DtLoc
          Siin = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)               &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0   &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)               &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0   &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ENDIF
!
      Dstaca = Geo_D3(Jtyp,Jgeo,Job)
      IF( Dstaca > 1.D0 ) THEN
        Nstaca = NINT( Geo_Le(Jtyp,Jgeo,Job) / Dstaca )
        Xstaca = 1.D0 + 1.D0 / (Nstaca-1)
        Istaca = (DzLoc-Decal) / Dstaca
        EzLoc  = Istaca*Dstaca*Xstaca / Geo_Le(Jtyp,Jgeo,Job)
      ELSE
        EzLoc  =        DzLoc         / Geo_Le(Jtyp,Jgeo,Job)
      ENDIF
      Spos1 = Siin + EzLoc * (Saan-Siin)
      Spos2 = Siip + EzLoc * (Saap-Siip)
!
      SZT(1:3) = ((/ Spos1, Zpos, Tpos /))
      CALL FPerformDirLocDisplacAndDeformJtypBased(Jtyp,Jff,Jzz,Job,SZT,SZTtru(1,1))
      SZT(1:3) = ((/ Spos2, Zpos, Tpos /))
      CALL FPerformDirLocDisplacAndDeformJtypBased(Jtyp,Jff,Jzz,Job,SZT,SZTtru(1,2))
!
      SZTloc(1:3,1:2) = SZTtru(1:3,1:2)
      IF( IBAREL(Jtyp,Jff,Jzz) < 1 ) THEN
        CALL SWAP_ZT( SZTtru(2,1), SZTtru(3,1) )
        CALL SWAP_ZT( SZTtru(2,2), SZTtru(3,2) )
      ENDIF
      SiFi = SIN( Pos_Phi(Jtyp,Jff,Jzz) )
      CoFi = COS( Pos_Phi(Jtyp,Jff,Jzz) )
      DO I=1,2
        XYZabs(1,I)   = CoFi*SZTtru(3,I) - SiFi*SZTtru(1,I)
        XYZabs(2,I)   = SiFi*SZTtru(3,I) + CoFi*SZTtru(1,I)
        XYZabs(3,I)   =      SZTtru(2,I)
      ENDDO
      Radius         = STARR(Jtec,Jsta)
!
      END SUBROUTINE FGetTubePositionFromMDTindexJtypBased
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE FGetTGCindexFromChannelPos(Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz, XYZabs, Jstri )
!
      IMPLICIT REAL(8) (A-H,O-Z)
      CHARACTER(3), INTENT(IN)  :: Ctyp
      INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jspli,Jsl,Jsz
      REAL(8),      INTENT(IN)  :: XYZabs(3)
      INTEGER,      INTENT(OUT) :: Jstri
      REAL(8) :: SZT(3),SZTide(3)
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
!#include "AmdcStand/copipi.inc"
!  
#include "AmdcStand/amdcsim_com.inc"
!  
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Jstri = 0
!
      Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
      IF( Jtyp < 1 )                   RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )                   RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'TGC' )      RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )                   RETURN
      IF(     IFORMA(Jtec,Jsta) == 1 ) THEN
        NSsl = NLAS(Jtec,Jsta)
        NZsl = NLAZ(Jtec,Jsta)
      ELSEIF( IFORMA(Jtec,Jsta) >= 2 .AND. IFORMA(Jtec,Jsta) <= 6 ) THEN
        NSsl = ( NLAZ(Jtec,Jsta) - 3 ) / 4
        NZsl = NSsl
      ELSE
                                       RETURN
      ENDIF
      IF(     Jsz == 1 ) THEN  
        IF( Jsl < 1 .OR. Jsl > NSsl ) RETURN
      ELSEIF( Jsz == 2 ) THEN
        IF( Jsl < 1 .OR. Jsl > NZsl ) RETURN
      ELSE
        IF( Jspli < 999999999 )       RETURN
      ENDIF
!
      IF( IFORMA(Jtec,Jsta) == 1 ) THEN
        Sstep = STAPP(Jtec,Jsta)
        Zstep = STAPP(Jtec,Jsta)
        Soffs = Sstep/2.D0 * MOD(Jsl-1,2)
        Zoffs = Zstep/2.D0 * MOD(Jsl-1,2)
      ELSE
        IF( IFORMA(Jtec,Jsta) == 2 ) THEN
          Sstep  = 30.0D0
          Zstep  = 30.0D0
          Soffs  = Sstep/2.D0 * MOD(Jsl-1,2)
          Zoffs  = Zstep/2.D0 * MOD(Jsl-1,2)
        ELSEIF( IFORMA(Jtec,Jsta) == 3 ) THEN
          Sstep  = STATT(Jtec,Jsta,NLAZ(Jtec,Jsta)+1)
          Zstep  = STAOO(Jtec,Jsta,NLAZ(Jtec,Jsta)+1)
          Soffs  = Sstep/2.D0 * MOD(Jsl-1,2)
          Zoffs  = Zstep/2.D0 * MOD(Jsl-1,2)
        ELSEIF( IFORMA(Jtec,Jsta) >= 4 .AND.   IFORMA(Jtec,Jsta) <= 6 ) THEN
          Wstep  = XtgcAdd(Jsta,4)
          Sstep  = XtgcAdd(Jsta,5)
          NstriS = NtgcAdd(Jsta,Jsl+3)
          IF( NstriS <= 1 ) NstriS = MAX( NtgcAdd(Jsta,4), NtgcAdd(Jsta,5), NtgcAdd(Jsta,6) )
          NstriZ = NtgcAdd(Jsta,Jsl)
          NtotGang = SUM( NtgcGang(Jsta,1:NstriZ,Jsl) )
          Soffs  = ( Geo_Ws(Jtyp,Jgeo,Job) + Geo_Wl(Jtyp,Jgeo,Job) ) / 4.D0   &
                 - NstriS * Sstep / 2.D0  - Sstep / 2.D0
          Zoffs  = ( Geo_Le(Jtyp,Jgeo,Job) - NtotGang * Wstep ) / 2.D0
        ENDIF
      ENDIF
!
      Zii  = Pos_R(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
      Zaa  = Zii + Geo_Le(Jtyp,Jgeo,Job)
      Siin = - Geo_Ws(Jtyp,Jgeo,Job) / 2.D0
      Siip =   Geo_Ws(Jtyp,Jgeo,Job) / 2.D0
      Saan = - Geo_Wl(Jtyp,Jgeo,Job) / 2.D0
      Saap =   Geo_Wl(Jtyp,Jgeo,Job) / 2.D0
      Soon = (Siin+Saan)/2.D0
      Soop = (Siip+Saap)/2.D0
      Zoo  = (Zaa +Zii )/2.D0
!---- Scen = Siin + (Siip-Siin) * (Saan-Siin) / (Saan-Siin-Saap+Siip)
!---- Zcen = Zii  + (Siip-Siin) * (Zaa -Zii ) / (Saan-Siin-Saap+Siip)
      Scen = 0.D0
      Zcen = Zii - Geo_dy(Jtyp,Jgeo,Job)
!
!---- Sisi = SIGN( 1.D0 , Pos_Z(Jtyp,Jff,Jzz) )
!---- F0   = Pos_Phi(Jtyp,Jff,Jzz) + Sisi*Geo_dx(Jtyp,Jgeo,Job)*PIS180
      F0   = Pos_Phi(Jtyp,Jff,Jzz)
      SiFi = SIN( F0 )
      CoFi = COS( F0 )
      SZT(1) = - SiFi*XYZabs(1) + CoFi*XYZabs(2)
      SZT(2) =        XYZabs(3)
      SZT(3) =   CoFi*XYZabs(1) + SiFi*XYZabs(2)
      IF( IBAREL(Jtyp,Jff,Jzz) < 1 ) CALL SWAP_ZT( SZT(2),SZT(3) )
      CALL FPerformBakLocDisplacAndDeformJtypBased(Jtyp,Jff,Jzz,Job,SZT,SZTide)
!
      Side = SZTide(1)
      Zide = SZTide(2)
      IF( Jsz == 1 ) THEN
        IF( IFORMA(Jtec,Jsta) < 6 ) THEN
          SideCen = Scen + (Side-Scen) * (Zoo-Zcen) / (Zide-Zcen)
          Jstri = NINT( (SideCen      - Soon - Soffs) / Sstep )
          Jstma = NINT( (Soop - Soffs - Soon - Soffs) / Sstep )
          IF( Jstri < 1 .OR. Jstri > Jstma ) Jstri = 0
!>> Bug Yoji ----------------------------------------
          IF( Jstri > 0 ) Jstri = Jstri - 1
!>> -------------------------------------------------
        ELSE
          Jstri = 0
          Zinf = Zii + XtgcAdd(Jsta,2)
          Zsup = Zaa - XtgcAdd(Jsta,3)
          IF(  Zide < Zinf .OR. Zide > Zsup ) RETURN
          Zee = (Zide-Zinf) / (Zsup-Zinf)
          IF( Jsl <= 1 )THEN
            DO I=1,NstriS
              Sinf =    XtgcStrp(Jsta,I  ,2) + Zee * ( XtgcStrp(Jsta,I  ,1) - XtgcStrp(Jsta,I  ,2) )
              Ssup =    XtgcStrp(Jsta,I+1,2) + Zee * ( XtgcStrp(Jsta,I+1,1) - XtgcStrp(Jsta,I+1,2) )
              IF( Side < Sinf .OR. Side > Ssup ) CYCLE
              Jstri = I
              EXIT
            ENDDO
          ELSE
            DO I=NstriS,1,-1
              Sinf =  - XtgcStrp(Jsta,I+1,2) - Zee * ( XtgcStrp(Jsta,I+1,1) - XtgcStrp(Jsta,I+1,2) )
              Ssup =  - XtgcStrp(Jsta,I  ,2) - Zee * ( XtgcStrp(Jsta,I  ,1) - XtgcStrp(Jsta,I  ,2) )
              IF( Side < Sinf .OR. Side > Ssup ) CYCLE
              Jstri = NstriS - I + 1
              EXIT
            ENDDO
          ENDIF
        ENDIF
      ELSE
        IF( IFORMA(Jtec,Jsta) < 4 ) THEN
          Jstri = NINT( (Zide        - Zii - Zoffs) / Zstep )
          Jstma = NINT( (Zaa - Zoffs - Zii - Zoffs) / Zstep )
          IF( Jstri < 1 .OR. Jstri > Jstma ) Jstri = 0
        ELSE
          Jstri = 0
          Zcur  = Zii + Zoffs
          IF( Zide >= Zcur ) THEN
            DO I=1,NstriZ
              Zcur = Zcur + NtgcGang(Jsta,I,Jsl)*Wstep
              IF( Zide > Zcur ) CYCLE
              Jstri = I
              EXIT
            ENDDO
          ENDIF 
        ENDIF 
      ENDIF 
!
      END SUBROUTINE FGetTGCindexFromChannelPos
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStripPositionFromRPCindex(Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8),      INTENT(OUT) :: XYZabs(3,4),SZTloc(3,4)
 REAL(8) :: SZT(3,4),SZTtru(3,4)
 REAL(8) :: SiFi,CoFi
 INTEGER :: Ier,Jtyp,I,JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    XYZabs(1:3,1:4) = 0.D0
    SZTloc(1:3,1:4) = 0.D0
!
    Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
    CALL FGetIdealStripPositionFromRPCindex(Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,SZT,Ier)
    IF( Ier /= 0 ) RETURN
    CALL FPerformDirLocDisplacAndDeformJtypBasedMany( Jtyp,Jff,Jzz,Job,SZT(1,1),SZTtru)
!
    SZTloc(1:3,1:4) = SZTtru(1:3,1:4)
    IF( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) < 1 ) THEN
      DO I=1,4
        CALL SWAP_ZT( SZTtru(2,I), SZTtru(3,I) )
      ENDDO
    ENDIF
    SiFi = SIN( Pos_Phi_V(JTYP_F_Z(Jtyp,Jff,Jzz)) )
    CoFi = COS( Pos_Phi_V(JTYP_F_Z(Jtyp,Jff,Jzz)) )
    DO I=1,4
      XYZabs(1,I)   = CoFi*SZTtru(3,I) - SiFi*SZTtru(1,I)
      XYZabs(2,I)   = SiFi*SZTtru(3,I) + CoFi*SZTtru(1,I)
      XYZabs(3,I)   =      SZTtru(2,I)
    ENDDO
!
 END SUBROUTINE FGetStripPositionFromRPCindex
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationType( Jtyp, NameType )
 IMPLICIT NONE
 INTEGER,      INTENT(IN)  :: Jtyp
 CHARACTER(3), INTENT(OUT) :: NameType
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    NameType = '???'
    IF( Jtyp < 1 .OR. Jtyp > MTYP ) RETURN
    NameType = CARTYP( Jtyp )
!
 END SUBROUTINE FGetStationType
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetObjectTech( Jtyp,Jff,Jzz,Job, NameTec )
 IMPLICIT NONE
 INTEGER,      INTENT(IN)  :: Jtyp,Jff,Jzz,Job
 CHARACTER(3), INTENT(OUT) :: NameTec
 INTEGER :: Jgeo, Jtec
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
    NameTec = '???'
    IF( Jtyp < 1 .OR. Jtyp > MTYP ) RETURN
    Jgeo = IGEO_V( JTYP_F_Z(Jtyp,Jff,Jzz)  )
    Jtec = ITEC_V( JTYP_G_O(Jtyp,Jgeo,Job) )
    NameTec = CARTEC( Jtec )
!
 END SUBROUTINE FGetObjectTech
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationPositionParam( NameType,Jff,Jzz,                                       &
                                      PosiIsValid, PosiJgeo,PosiJcut,PosiIsBarrel,            &
                                      PosiPhi,PosiZ,PosiR,PosiS,PosiAlfa,PosiBeta,PosiGamma )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jff,Jzz
 INTEGER,      INTENT(OUT) :: PosiIsValid
 INTEGER,      INTENT(OUT) :: PosiJgeo,PosiJcut,PosiIsBarrel
 REAL(8),      INTENT(OUT) :: PosiPhi,PosiZ,PosiR,PosiS
 REAL(8),      INTENT(OUT) :: PosiAlfa,PosiBeta,PosiGamma
 INTEGER :: Jtyp, JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
    PosiIsValid = 0
    IF( Jff < 1    .OR. Jff > 8   ) RETURN
    IF( Jzz < -MZZ .OR. Jff > MZZ ) RETURN
    Jtyp = JTROU3(NameType,MTYP,CARTYP)
    IF( Jtyp < 1 )                  RETURN
    PosiJgeo     = IGEO(Jtyp,Jff,Jzz)
    IF( PosiJgeo < 1 )              RETURN
    PosiIsValid = 1
!
    PosiJcut     = ICUT    (Jtyp,Jff,Jzz)
    PosiIsBarrel = IBAREL  (Jtyp,Jff,Jzz)
    PosiPhi      = Pos_Phi (Jtyp,Jff,Jzz)
    PosiZ        = Pos_Z   (Jtyp,Jff,Jzz)
    PosiR        = Pos_R   (Jtyp,Jff,Jzz)
    PosiS        = Pos_S   (Jtyp,Jff,Jzz)
    PosiAlfa     = Pos_Alfa(Jtyp,Jff,Jzz)
    PosiBeta     = Pos_Beta(Jtyp,Jff,Jzz)
    PosiGamma    = Pos_Gama(Jtyp,Jff,Jzz)
!
 END SUBROUTINE FGetStationPositionParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationObjectParam( NameType,Jgeo,Job,                            &
                                    GeomIsValid, GeomTechnoName,GeomTechnoIndex,  &
                                    GeomSplitX,GeomSplitY,GeomShapeIndex,         &
                                    GeomDx,GeomDy,GeomDz,GeomWs,GeomWl,           &
                                    GeomLe,GeomEx,GeomD1,GeomD2,GeomD3 )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jgeo,Job
 CHARACTER(3), INTENT(OUT) :: GeomTechnoName
 INTEGER,      INTENT(OUT) :: GeomIsValid,GeomTechnoIndex
 INTEGER,      INTENT(OUT) :: GeomSplitX,GeomSplitY,GeomShapeIndex
 REAL(8),      INTENT(OUT) :: GeomDx,GeomDy,GeomDz,GeomWs,GeomWl
 REAL(8),      INTENT(OUT) :: GeomLe,GeomEx,GeomD1,GeomD2,GeomD3
 INTEGER :: Jtyp, JTROU3, Jtec
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      GeomIsValid = 0
      IF( Jgeo < 1 .OR. Jgeo > MGEO )            RETURN
      Jtyp = JTROU3(NameType,MTYP,CARTYP)
      IF( Jtyp < 1 )                             RETURN
      IF( Job  < 1 .OR. Job  > NOBJ(Jtyp,Jgeo) ) RETURN
      GeomIsValid = 1
!
      Jtec            = ITEC(Jtyp,Jgeo,Job)
      GeomTechnoName  = CARTEC(Jtec)
      GeomTechnoIndex = ISTA  (Jtyp,Jgeo,Job)
      GeomSplitX      = ISPLIX(Jtyp,Jgeo,Job)
      GeomSplitY      = ISPLIY(Jtyp,Jgeo,Job)
      GeomShapeIndex  = ISHAPE(Jtyp,Jgeo,Job)
      GeomDx          = Geo_dx(Jtyp,Jgeo,Job)
      GeomDy          = Geo_dy(Jtyp,Jgeo,Job)
      GeomDz          = Geo_dz(Jtyp,Jgeo,Job)
      GeomWs          = Geo_Ws(Jtyp,Jgeo,Job)
      GeomWl          = Geo_Wl(Jtyp,Jgeo,Job)
      GeomLe          = Geo_Le(Jtyp,Jgeo,Job)
      GeomEx          = Geo_Ex(Jtyp,Jgeo,Job)
      GeomD1          = Geo_D1(Jtyp,Jgeo,Job)
      GeomD2          = Geo_D2(Jtyp,Jgeo,Job)
      GeomD3          = Geo_D3(Jtyp,Jgeo,Job)
!
 END SUBROUTINE FGetStationObjectParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationDimensions( NameType,Jgeo,Jcut, IsValid,                                &
                                   DimLocX,DimLocY,DimLocZ, CenterLocX,CenterLocY,CenterLocZ )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut
 INTEGER,      INTENT(OUT) :: IsValid
 REAL(8),      INTENT(OUT) :: DimLocX,DimLocY,DimLocZ
 REAL(8),      INTENT(OUT) :: CenterLocX,CenterLocY,CenterLocZ
 INTEGER :: JTROU3
 INTEGER :: Jtyp
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    Jtyp = JTROU3(NameType,MTYP,CARTYP)
!
    CALL FGetStationDimensionsJtypBased(Jtyp,Jgeo,Jcut, IsValid,                                    &
                                        DimLocX,DimLocY,DimLocZ, CenterLocX,CenterLocY,CenterLocZ )
!
 END SUBROUTINE FGetStationDimensions
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationDimensionsJtypBased(Jtyp,Jgeo,Jcut, IsValid,                                    &
                                           DimLocX,DimLocY,DimLocZ, CenterLocX,CenterLocY,CenterLocZ )
 IMPLICIT REAL(8) (A-H,O-Z)
 INTEGER, INTENT(IN)  :: Jtyp
 INTEGER, INTENT(IN)  :: Jgeo,Jcut
 INTEGER, INTENT(OUT) :: IsValid
 REAL(8), INTENT(OUT) :: DimLocX,DimLocY,DimLocZ
 REAL(8), INTENT(OUT) :: CenterLocX,CenterLocY,CenterLocZ
 INTEGER, PARAMETER :: madim = 30
 REAL(8), PARAMETER :: epsilon = 0.020D0
 REAL(8) :: zzin (madim),ssin (madim),ttmin,ttmax
 REAL(8) :: zzout(madim),ssout(madim)
 REAL(8) :: zzcut(5),    sscut(5)
 INTEGER :: jjin (madim),jjout(madim)
 INTEGER :: jjcut(5),nout(2)
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
    IsValid = 0
      
    IF( Jtyp < 1 )                  RETURN
    IF( Jgeo < 1 .OR. Jgeo > MGEO ) RETURN
    IF( NOBJ(Jtyp,Jgeo) < 1 )       RETURN
    IsValid = 1
!
    Xmin =  999999.D0
    Xmax = -999999.D0
    Ymin =  999999.D0
    Ymax = -999999.D0
    Zmin =  999999.D0
    Zmax = -999999.D0
    jjin (:) = 1
    jjout(:) = 1
    jjcut(:) = 1
!
!--------------------------------------------------------
    DO Job=1,NOBJ(Jtyp,Jgeo)
      Jtec = ITEC  (Jtyp,Jgeo,Job)
      Jsta = ISTA  (Jtyp,Jgeo,Job)
      Jsha = ISHAPE(Jtyp,Jgeo,Job)
!
      xc  = Geo_dx(Jtyp,Jgeo,Job)
      yc  = Geo_dy(Jtyp,Jgeo,Job)
      zc  = Geo_dz(Jtyp,Jgeo,Job)
      ws2 = Geo_Ws(Jtyp,Jgeo,Job) / 2.D0
      wl2 = Geo_Wl(Jtyp,Jgeo,Job) / 2.D0
      el  = Geo_Le(Jtyp,Jgeo,Job)
      ex  = Geo_Ex(Jtyp,Jgeo,Job)
      ex2 = ex / 2.D0
      IF( CARTEC(Jtec) == 'TGC' .AND. Jsha == 0 ) xc = 0.D0
      IF( CARTEC(Jtec) == 'CSC' .AND. Jsha == 2 ) THEN 
         nin = 5
         zzin(1) = yc
         zzin(2) = yc
         zzin(3) = yc + el
         zzin(4) = yc + ex
         zzin(5) = yc + el
         ssin(1) = xc - ws2
         ssin(2) = xc + ws2
         ssin(3) = xc + wl2
         ssin(4) = xc 
         ssin(5) = xc - wl2
!------? ssin(3) = xc 
!------? ssin(4) = xc + wl2
!------? ssin(5) = xc - wl2
         ttmin   = zc 
         ttmax   = zc + STAEE(Jtec,Jsta)
      ELSEIF( CARTEC(Jtec) == 'SUP' .AND. Jsha == 0 ) THEN
         nin = 4
         ssin(1)       = xc + ex2 - ws2
         ssin(2)       = xc + ex2 + ws2
         ssin(3)       = xc - ex2 + wl2
         ssin(4)       = xc - ex2 - wl2
         IF( NLAS(Jtec,Jsta) == 2 .AND. NLAZ(Jtec,Jsta) == 1 ) THEN
            xx1 = statt(Jtec,Jsta,1)
            xx2 = statt(Jtec,Jsta,2)
            zz1 = staoo(Jtec,Jsta,1)
            IF( zz1 > 0.D0 ) THEN
               zzin(1) = yc - xx1
               zzin(2) = yc - xx1
               zzin(3) = yc + zz1
               zzin(4) = yc + zz1
            ELSE
               zzin(1) = yc + zz1
               zzin(2) = yc + zz1
               zzin(3) = yc + xx1
               zzin(4) = yc + xx1
            ENDIF
            ttmin      = zc - xx1
            ttmax      = zc + xx2 + xx1
         ELSEIF( NLAS(Jtec,Jsta) == 4 .AND. NLAZ(Jtec,Jsta) == 4 ) THEN
            xx2 = statt(Jtec,Jsta,2)
            xx3 = statt(Jtec,Jsta,3)
            xx4 = statt(Jtec,Jsta,4)
            zz2 = staoo(Jtec,Jsta,2)
            zz3 = staoo(Jtec,Jsta,3)
            zz4 = staoo(Jtec,Jsta,4)
            zzin(1) = yc - zz2 - zz3 - zz4
            zzin(2) = yc - zz2 - zz3 - zz4
            zzin(3) = yc + zz2 + zz3 + zz4
            zzin(4) = yc + zz2 + zz3 + zz4
            ttmin      = zc - xx2
            ttmax      = zc + xx2 + xx3 + xx4
         ELSE
            nin = 0
         ENDIF
      ELSE
         nin = 4
         IF( CARTEC(Jtec) == 'CHV' .OR. CARTEC(Jtec) == 'CMI' .OR.    &
             CARTEC(Jtec) == 'CRO' .OR. CARTEC(Jtec) == 'LB ' ) THEN
            dtt = STAPP(Jtec,Jsta)
         ELSE
            dtt = STAEE(Jtec,Jsta)
         ENDIF
         zzin(1) = yc
         zzin(2) = yc
         zzin(3) = yc + el
         zzin(4) = yc + el
         ssin(1) = xc + ex2 - ws2
         ssin(2) = xc + ex2 + ws2
         ssin(3) = xc - ex2 + wl2
         ssin(4) = xc - ex2 - wl2
         ttmin   = zc
         ttmax   = zc + dtt
      ENDIF
!
      IF( Jcut > 0 ) THEN
        JtotCu = INOCUT(Jtyp,Jgeo,Jcut)
        DO JobCut=1,NOBCUT(JtotCu)
          IF( IOBCUT(JtotCu,JobCut) /= Job ) CYCLE
          xc_c  = Cut_dx(JtotCu,JobCut)
          yc_c  = Cut_dy(JtotCu,JobCut)
          ws2_c = Cut_Ws(JtotCu,JobCut) / 2.D0
          wl2_c = Cut_Wl(JtotCu,JobCut) / 2.D0
          el_c  = Cut_Le(JtotCu,JobCut)
          ex2_c = Cut_Ex(JtotCu,JobCut) / 2.D0
          ncut     = 4
          zzcut(1) = yc_c                 - epsilon
          zzcut(2) = yc_c                 - epsilon
          zzcut(3) = yc_c + el_c          + epsilon
          zzcut(4) = yc_c + el_c          + epsilon
          sscut(1) = xc_c + ex2_c + ws2_c + epsilon
          sscut(2) = xc_c + ex2_c - ws2_c - epsilon
          sscut(3) = xc_c - ex2_c - wl2_c - epsilon
          sscut(4) = xc_c - ex2_c + wl2_c + epsilon
          CALL CUTOUT( nin , jjin,zzin ,ssin ,   &
                      ncut,jjcut,zzcut,sscut,   &
          madim,1, ll, nout,jjout,zzout,ssout )
          nin = nout(1)
          DO J=1,nout(1)
            zzin(J) = zzout(J)
            ssin(J) = ssout(J)
          ENDDO
        ENDDO
      ENDIF
!
      DO J=1,nin
        Xmin = MIN( Xmin , ssin(J) )
        Xmax = MAX( Xmax , ssin(J) )
        Ymin = MIN( Ymin , zzin(J) )
        Ymax = MAX( Ymax , zzin(J) )
      ENDDO
      Zmin  = MIN( Zmin , ttmin   )
      Zmax  = MAX( Zmax , ttmax   )
!
    ENDDO
!--------------------------------------------------------
!
    CenterLocX = ( Xmax + Xmin ) / 2.D0
    CenterLocY = ( Ymax + Ymin ) / 2.D0
    CenterLocZ = ( Zmax + Zmin ) / 2.D0
    DimLocX    =   Xmax - Xmin
    DimLocY    =   Ymax - Ymin
    DimLocZ    =   Zmax - Zmin
!
 END SUBROUTINE FGetStationDimensionsJtypBased
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStationCutOutObjParam(NameType,Jgeo,Jcut,JcutObj,                        &
                                      CutJob,CutDx,CutDy,CutWs,CutWl,CutLe,CutEx,CutAn)
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut,JcutObj
 INTEGER,      INTENT(OUT) :: CutJob
 REAL(8),      INTENT(OUT) :: CutDx,CutDy
 REAL(8),      INTENT(OUT) :: CutWs,CutWl,CutLe,CutEx,CutAn
 INTEGER :: Jtyp, JTROU3, JTotCut
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      CutJob = 0
      IF( Jgeo < 1 .OR. Jgeo > MGEO )                  RETURN
      IF( Jcut < 1 .OR. Jcut > 4 )                     RETURN
      Jtyp = JTROU3(NameType,MTYP,CARTYP)
      IF( Jtyp < 1 )                                   RETURN
      JTotCut = INOCUT(Jtyp,Jgeo,Jcut)
      IF( JTotCut < 1 )                                RETURN
      IF( JcutObj < 1 .OR. JcutObj > NOBCUT(JTotCut) ) RETURN
!
      CutJob = IOBCUT( JTotCut, JcutObj )
      CutDx  = Cut_dx( JTotCut, JcutObj )
      CutDy  = Cut_dy( JTotCut, JcutObj )
      CutWs  = Cut_Ws( JTotCut, JcutObj )
      CutWl  = Cut_Wl( JTotCut, JcutObj )
      CutLe  = Cut_Le( JTotCut, JcutObj )
      CutEx  = Cut_Ex( JTotCut, JcutObj )
      CutAn  = Cut_An( JTotCut, JcutObj )
!
 END SUBROUTINE FGetStationCutOutObjParam
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetRPCindexFromStripPosition(Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz, XYZabs, Jstri)
 IMPLICIT REAL(8) (A-H,O-Z)
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jspli,Jsl,Jsz
 REAL(8),      INTENT(IN)  :: XYZabs(3)
 INTEGER,      INTENT(OUT) :: Jstri
 REAL(8) :: SZT(3),SZTide(3)
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Jstri = 0
!
      Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
      IF( Jtyp < 1 )                   RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )                   RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'RPC' )      RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )                   RETURN
      NSsl = NLAS(Jtec,Jsta)
      NZsl = NLAZ(Jtec,Jsta)
      IF(     Jsz == 1 ) THEN
         IF( Jsl < 1 .OR. Jsl > NSsl ) RETURN
      ELSEIF( Jsz == 2 ) THEN
         IF( Jsl < 1 .OR. Jsl > NZsl ) RETURN
      ELSE
                                       RETURN
      ENDIF
!
      JspliLoc = 1 
      IF( IFORMA(Jtec,Jsta) == 0 ) THEN
         Sstep  = STAPP(Jtec,Jsta)
         Zstep  = STARR(Jtec,Jsta)
         Soffs1 = STAOO(Jtec,Jsta,Jsl)
         Soffs2 = STAOO(Jtec,Jsta,Jsl)
         Zoffs  = STAOO(Jtec,Jsta,Jsl+NSsl)
      ELSEIF( IFORMA(Jtec,Jsta) == 1 ) THEN
         Sstep  =  STAPP(Jtec,Jsta)
         Zstep  =  STAPP(Jtec,Jsta)
         Soffs1 = 0.D0
         Soffs2 = 0.D0
         Zoffs  = 0.D0
      ELSEIF( IFORMA(Jtec,Jsta) == 2 .OR. IFORMA(Jtec,Jsta) == 4 ) THEN
         Sstep  =  STATT(Jtec,Jsta,10)
         Zstep  =  STATT(Jtec,Jsta,11)
         NstriS =  NINT( STAOO(Jtec,Jsta,14) ) 
         NstriZ =  NINT( STAOO(Jtec,Jsta,15) )
         Nspli  =  NINT( STAOO(Jtec,Jsta,2) )
         Ngazv  =  NINT( STAOO(Jtec,Jsta,3) )
         TotLe  =  Geo_Le(Jtyp,Jgeo,Job)
         TotWi  = (Geo_Wl(Jtyp,Jgeo,Job)+Geo_Ws(Jtyp,Jgeo,Job))/2.D0
         DedSi  =  Geo_D1(Jtyp,Jgeo,Job)
         DedCe  =  STATT(Jtec,Jsta,17)
         IF( NstriS <= 1 ) NstriS = NINT( (TotWi-DedCe)/Sstep/Nspli )
         IF( NstriZ <= 1 ) NstriZ = NINT( (TotLe-DedCe)/Zstep )
         IF( Jspli == 2 .OR. Jspli == 4 ) JspliLoc = 2
         IF( Nspli <= 1 ) THEN
            Soffs1 = ( TotWi - Sstep*NstriS - Sstep ) / 2.D0
            Soffs2 = 0.D0
            JspliLoc = 1
         ELSEIF( Ngazv <= 1 ) THEN
            DedXX = (TotWi -2.D0*Sstep*NstriS -2.D0*DedSi) / 4.D0
            Soffs1 = DedSi      + DedXX - Sstep / 2.D0
            Soffs2 =              DedXX - Sstep / 2.D0
         ELSE
            DedXX = (TotWi -2.D0*Sstep*NstriS -2.D0*DedSi -DedCe) / 4.D0
            Soffs1 = DedSi      + DedXX - Sstep / 2.D0
            Soffs2 = DedCe/2.D0 + DedXX - Sstep / 2.D0
         ENDIF
         Zoffs  = ( TotLe - Zstep*NstriZ - Zstep ) / 2.D0
      ELSE
         RETURN
      ENDIF
!
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          Zaa  = Pos_Z(Jtyp,Jff,Jzz) - Geo_dy(Jtyp,Jgeo,Job) + 20.D0*ZC_Sta
          Zii  = Zaa - Geo_Le(Jtyp,Jgeo,Job)
          Siin = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)       &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0  &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Wl(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)       &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0  &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Ws(Jtyp,Jgeo,Job)
        ELSE
          Zii  = Pos_Z(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
          Zaa  = Zii + Geo_Le(Jtyp,Jgeo,Job)
          Siin = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)       &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0  &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)       &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0  &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ELSE
        Zii     = Pos_R(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
        Zaa     = Zii + Geo_Le(Jtyp,Jgeo,Job)
        sis0 = 1.D0
        IF( IVERSION_AMDC > 6 .AND. Jzz > 0 ) sis0 = -1.D0
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Siin = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)              &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0  &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)              &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0  &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ELSE
          Siin = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)              &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0  &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)              &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0  &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ENDIF
      Soon = (Siin+Saan)/2.D0
      Soop = (Siip+Saap)/2.D0
      Soo  = (Soon+Soop)/2.D0
      Zoo  = (Zaa +Zii )/2.D0
      Dzz  = (Zaa-Zii)/2.D0
      A0   = (Saap-Saan) / (Siip-Siin)
      AM1  = A0 - 1.D0
      AP1  = A0 + 1.D0
      ASS  = A0 * (Saap-Siip)
      IF( IFORMA(Jtec,Jsta) < 2 ) THEN
        NstriS = NINT( (Soop-Soon-2.D0*Soffs1) / Sstep )
        NstriZ = NINT( (Zaa -Zii -2.D0*Zoffs ) / Zstep )
      ENDIF
!
      SiFi = SIN( Pos_Phi(Jtyp,Jff,Jzz) )
      CoFi = COS( Pos_Phi(Jtyp,Jff,Jzz) )
      SZT(1) = - SiFi*XYZabs(1) + CoFi*XYZabs(2)
      SZT(2) =        XYZabs(3)
      SZT(3) =   CoFi*XYZabs(1) + SiFi*XYZabs(2)
      IF( IBAREL(Jtyp,Jff,Jzz) < 1 ) CALL SWAP_ZT( SZT(2),SZT(3) )
      CALL FPerformBakLocDisplacAndDeformJtypBased(Jtyp,Jff,Jzz,Job,SZT,SZTide)
!
      Side = SZTide(1)
      Zide = SZTide(2)
      IF( Jsz == 1 ) THEN
        SideCen = Side + (Zide-Zoo) * (ASS+AM1*(Side-Saap)) / (AM1*(Zoo-Zide)-AP1*Dzz)
        IF( JspliLoc == 1 ) THEN
          Jstri = NINT( (SideCen - Soon - Soffs1) / Sstep )
        ELSE
          Jstri = NINT( (SideCen - Soo  - Soffs2) / Sstep )
        ENDIF
        Jstma = NstriS
      ELSE
        IF( Jzz < 0 ) THEN
          Jstri = NINT( (Zaa - Zoffs - Zide) / Zstep )
        ELSE
          Jstri = NINT( (Zide - Zii - Zoffs) / Zstep )
        ENDIF
        Jstma = NstriZ
      ENDIF
      IF( Jstri < 1 .OR. Jstri > Jstma ) Jstri = 0
!
 END SUBROUTINE FGetRPCindexFromStripPosition
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetNumberOfStationObjects( NameType,Jgeo, NbObj )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jgeo
 INTEGER,      INTENT(OUT) :: NbObj
 INTEGER :: Jtyp
 INTEGER :: JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      NbObj = 0
      IF( Jgeo < 1 .OR. Jgeo > MGEO ) RETURN
      Jtyp  = JTROU3(NameType,MTYP,CARTYP)
      IF( Jtyp < 1 )                  RETURN
      NbObj = NOBJ(Jtyp,Jgeo)
!
 END SUBROUTINE FGetNumberOfStationObjects
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetNumberOfCutOutObjects( NameType,Jgeo,Jcut, NbObj )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: NameType
 INTEGER,      INTENT(IN)  :: Jgeo,Jcut
 INTEGER,      INTENT(OUT) :: NbObj
 INTEGER :: Jtyp, JTotCut
 INTEGER :: JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
      NbObj = 0
      IF( Jgeo < 1 .OR. Jgeo > MGEO ) RETURN
      IF( Jcut < 1 .OR. Jcut > 4 )    RETURN
      Jtyp  = JTROU3(NameType,MTYP,CARTYP)
      IF( Jtyp < 1 )                  RETURN
      JTotCut = INOCUT(Jtyp,Jgeo,Jcut)
      IF( JTotCut < 1 )               RETURN
      NbObj = NOBCUT(JTotCut)
!
 END SUBROUTINE FGetNumberOfCutOutObjects
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetMDTindexFromTubePosition( Ctyp,Jff,Jzz,Job,Jsl, MDTnu, XYZabs, Jtube )
 IMPLICIT REAL(8) (A-H,O-Z)
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jsl,MDTnu
 REAL(8),      INTENT(IN)  :: XYZabs(3)
 INTEGER,      INTENT(OUT) :: Jtube
 REAL(8), PARAMETER :: Eps = 0.100D0
 REAL(8) :: SZT(3),SZTide(3)
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Jtube = 0
!
      Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
      IF( Jtyp < 1 )               RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )               RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'MDT' )  RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )               RETURN
      Nsl  = NLAZ(Jtec,Jsta)
      IF( Jsl < 1 .OR. Jsl > Nsl ) RETURN
!
      SiFi = SIN( Pos_Phi(Jtyp,Jff,Jzz) )
      CoFi = COS( Pos_Phi(Jtyp,Jff,Jzz) )
      SZT(1) = - SiFi*XYZabs(1) + CoFi*XYZabs(2)
      SZT(2) =        XYZabs(3)
      SZT(3) =   CoFi*XYZabs(1) + SiFi*XYZabs(2)
      IF( IBAREL(Jtyp,Jff,Jzz) < 1 ) CALL SWAP_ZT( SZT(2),SZT(3) )
      CALL FPerformBakLocDisplacAndDeformJtypBased(Jtyp,Jff,Jzz,Job,SZT,SZTide)
!
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          IF( Jzz < 0 .AND. MDTnu < 0 ) THEN
            DzLoc = Pos_Z(Jtyp,Jff,Jzz) + 20.D0*ZC_Sta                        &
                  - Geo_dy(Jtyp,Jgeo,Job) - SZTide(2) - STAOO(Jtec,Jsta,Jsl)
          ELSE
            DzLoc = SZTide(2) - Pos_Z(Jtyp,Jff,Jzz) - 20.D0*ZC_Sta            &
                  + Geo_dy(Jtyp,Jgeo,Job) + Geo_Le(Jtyp,Jgeo,Job) - STAOO(Jtec,Jsta,Jsl)
          ENDIF
        ELSE
          IF( Jzz < 0 .AND. MDTnu < 0 ) THEN
            DzLoc = Pos_Z(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)               &
                  + Geo_Le(Jtyp,Jgeo,Job) - SZTide(2) - STAOO(Jtec,Jsta,Jsl)
          ELSE
            DzLoc = SZTide(2) - Pos_Z(Jtyp,Jff,Jzz)                           &
                  - Geo_dy(Jtyp,Jgeo,Job) - STAOO(Jtec,Jsta,Jsl)
          ENDIF
        ENDIF
      ELSE
        DzLoc = SZTide(2) - Pos_R(Jtyp,Jff,Jzz)               &
              - Geo_dy(Jtyp,Jgeo,Job) - STAOO(Jtec,Jsta,Jsl)
      ENDIF
!
      TotLe = Geo_Le(Jtyp,Jgeo,Job) - Eps
      Jtuma = NINT( (TotLe-STAOO(Jtec,Jsta,Jsl))/STAPP(Jtec,Jsta) ) + 1
      Jtube = NINT(            DzLoc            /STAPP(Jtec,Jsta) ) + 1
      IF( Jtube < 1 .OR. Jtube > Jtuma ) Jtube = 0
!
 END SUBROUTINE FGetMDTindexFromTubePosition
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetIdealStripPositionFromRPCindex( Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, SZT, Ier )
 IMPLICIT REAL(8) (A-H,O-Z)
 INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8), INTENT(OUT) :: SZT(3,4)
 INTEGER, INTENT(OUT) :: Ier
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      SZT(1:3,1:4) = 0.D0
      Ier = 1
!
      IF( Jtyp < 1 )                   RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )                   RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'RPC' )      RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )                   RETURN
      NSsl = NLAS(Jtec,Jsta)
      NZsl = NLAZ(Jtec,Jsta)
      IF(     Jsz == 1 ) THEN
         IF( Jsl < 1 .OR. Jsl > NSsl ) RETURN
      ELSEIF( Jsz == 2 ) THEN
         IF( Jsl < 1 .OR. Jsl > NZsl ) RETURN
      ELSE
                                       RETURN
      ENDIF
!
      JspliLoc = 1
      IF( IFORMA(Jtec,Jsta) == 0 ) THEN
        Sstep  = STAPP(Jtec,Jsta)
        Zstep  = STARR(Jtec,Jsta)
        Soffs1 = STAOO(Jtec,Jsta,Jsl)
        Soffs2 = STAOO(Jtec,Jsta,Jsl)
        Zoffs  = STAOO(Jtec,Jsta,Jsl+NSsl)
        DtLoc  = STATT(Jtec,Jsta,Jsl+(Jsz-1)*NSsl)
      ELSEIF( IFORMA(Jtec,Jsta) == 1 ) THEN
        Sstep  =  STAPP(Jtec,Jsta)
        Zstep  =  STAPP(Jtec,Jsta)
        Soffs1 = 0.D0
        Soffs2 = 0.D0
        Zoffs  = 0.D0
        DtLoc  =           STAOO(Jtec,Jsta,1) + STAOO(Jtec,Jsta,2)/2.D0   &
               + (Jsl-1)*( STAOO(Jtec,Jsta,2) + STAOO(Jtec,Jsta,3) )
      ELSEIF( IFORMA(Jtec,Jsta) == 2 .OR. IFORMA(Jtec,Jsta) == 4 ) THEN
        Sstep  =  STATT(Jtec,Jsta,10)
        Zstep  =  STATT(Jtec,Jsta,11)
        NstriS =  NINT( STAOO(Jtec,Jsta,14) )
        NstriZ =  NINT( STAOO(Jtec,Jsta,15) )
        Nspli  =  NINT( STAOO(Jtec,Jsta,2) )
        Ngazv  =  NINT( STAOO(Jtec,Jsta,3) )
        TotLe  =  Geo_Le(Jtyp,Jgeo,Job)
        TotWi  = (Geo_Wl(Jtyp,Jgeo,Job)+Geo_Ws(Jtyp,Jgeo,Job))/2.D0
        DedSi  =  Geo_D1(Jtyp,Jgeo,Job)
        DedCe  =  STATT(Jtec,Jsta,17)
        IF( NstriS <= 1 ) NstriS = NINT( (TotWi-DedCe)/Sstep/Nspli )
        IF( NstriZ <= 1 ) NstriZ = NINT( (TotLe-DedCe)/Zstep )
        IF( Jspli == 2 .OR. Jspli == 4 ) JspliLoc = 2
        IF( Nspli <= 1 ) THEN
          Soffs1 = ( TotWi - Sstep*NstriS - Sstep ) / 2.D0
          Soffs2 = 0.D0
          JspliLoc = 1
        ELSEIF( Ngazv <= 1 ) THEN
          DedXX = (TotWi -2.D0*Sstep*NstriS -2.D0*DedSi) / 4.D0
          Soffs1 = DedSi      + DedXX - Sstep / 2.D0
          Soffs2 =              DedXX - Sstep / 2.D0
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!--JFL mercredi 30 juillet 2008, 15:02:55 (UTC+0200)
          DedXX = (TotWi -2.D0*Sstep*NstriS            ) / 4.D0
          Soffs1 =              DedXX - Sstep / 2.D0
          Soffs2 =              DedXX - Sstep / 2.D0
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        ELSE
          DedXX = (TotWi -2.D0*Sstep*NstriS -2.D0*DedSi -DedCe) / 4.D0
          Soffs1 = DedSi      + DedXX - Sstep / 2.D0
          Soffs2 = DedCe/2.D0 + DedXX - Sstep / 2.D0
        ENDIF
        Zoffs  = ( TotLe - Zstep*NstriZ - Zstep ) / 2.D0
        Toffs  = STATT(Jtec,Jsta,1) + STATT(Jtec,Jsta,7) + STATT(Jtec,Jsta,4)/2.D0
        Tstep  = STATT(Jtec,Jsta,4) + STATT(Jtec,Jsta,5)
        IF( ISHAPE(Jtyp,Jgeo,Job) == -1 ) Toffs  = STAEE(Jtec,Jsta) - Toffs - Tstep*(NSsl-1)
        DtLoc  = Toffs + Tstep*(Jsl-1)
      ELSE
        RETURN
      ENDIF
!
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
        Tpos    = Pos_R(Jtyp,Jff,Jzz) + Geo_dz(Jtyp,Jgeo,Job) + DtLoc
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          Zaa  = Pos_Z(Jtyp,Jff,Jzz) - Geo_dy(Jtyp,Jgeo,Job) + 20.D0*ZC_Sta
          Zii  = Zaa - Geo_Le(Jtyp,Jgeo,Job)
          Siin = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)        &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Wl(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - Geo_dx(Jtyp,Jgeo,Job)        &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Ws(Jtyp,Jgeo,Job)
        ELSE
          Zii  = Pos_Z(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
          Zaa  = Zii + Geo_Le(Jtyp,Jgeo,Job)
          Siin = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)        &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + Geo_dx(Jtyp,Jgeo,Job)        &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0   &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ELSE
        Sisi = SIGN( 1.D0 , Pos_Z(Jtyp,Jff,Jzz) )
        Zii     = Pos_R(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
        Zaa     = Zii + Geo_Le(Jtyp,Jgeo,Job)
        sis0 = 1.D0
        IF( IVERSION_AMDC > 6 .AND. Jzz > 0 ) sis0 = -1.D0
        IF( ABS(Pos_Alfa(Jtyp,Jff,Jzz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) THEN
          Call Dim_Station(Jtyp,Jff,Jzz, SC_Sta,ZC_Sta,TC_Sta, DS_Sta,DZ_Sta,DT_Sta)
          Tpos = Pos_Z(Jtyp,Jff,Jzz) - Sisi*Geo_dz(Jtyp,Jgeo,Job)             &
                                     - Sisi*DtLoc + Sisi*20.D0*TC_Sta
          Siin = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)                &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0    &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) + ( Geo_dx(Jtyp,Jgeo,Job)                &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0    &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ELSE
          Tpos = Pos_Z(Jtyp,Jff,Jzz) + Sisi*Geo_dz(Jtyp,Jgeo,Job)             &
                                     + Sisi*DtLoc
          Siin = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)                &
                                     + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0    &
                                     - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
          Siip = Siin + Geo_Ws(Jtyp,Jgeo,Job)
          Saan = Pos_S(Jtyp,Jff,Jzz) - ( Geo_dx(Jtyp,Jgeo,Job)                &
                                     - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0    &
                                     - Geo_Wl(Jtyp,Jgeo,Job)/2.D0
          Saap = Saan + Geo_Wl(Jtyp,Jgeo,Job)
        ENDIF
      ENDIF
      Soon = (Siin+Saan)/2.D0
      Soop = (Siip+Saap)/2.D0
      Soo  = (Soon+Soop)/2.D0
      Zoo  = (Zaa +Zii )/2.D0
      Dzz  = (Zaa-Zii)/2.D0
      A0   = (Saap-Saan) / (Siip-Siin)
      AM1  = A0 - 1.D0
      AP1  = A0 + 1.D0
      ASS  = A0 * (Saap-Siip)
      IF( Jsz == 1 ) THEN
        Zpos1 = Zii + Zoffs + Zstep/2.D0
        Zpos2 = Zpos1
        Zpos3 = Zaa - Zoffs - Zstep/2.D0
        Zpos4 = Zpos3
        IF( JspliLoc == 1 ) THEN
          S0n = Soon + Soffs1 + Sstep*(Jstri-0.5D0)
          S0p = Soon + Soffs1 + Sstep*(Jstri+0.5D0)
        ELSE
          S0n = Soo  + Soffs2 + Sstep*(Jstri-0.5D0)
          S0p = Soo  + Soffs2 + Sstep*(Jstri+0.5D0)
        ENDIF
        Spos1 = S0n + (Zpos1-Zoo) * (ASS+AM1*(S0n-Saap)) / (AP1*Dzz)
        Spos2 = S0p + (Zpos2-Zoo) * (ASS+AM1*(S0p-Saap)) / (AP1*Dzz)
        Spos3 = S0p + (Zpos3-Zoo) * (ASS+AM1*(S0p-Saap)) / (AP1*Dzz)
        Spos4 = S0n + (Zpos4-Zoo) * (ASS+AM1*(S0n-Saap)) / (AP1*Dzz)
      ELSE
        IF( Jzz < 0 ) THEN
          Zpos1 = Zaa - Zoffs - Zstep*(Jstri-0.5D0)
          Zpos2 = Zaa - Zoffs - Zstep*(Jstri+0.5D0)
        ELSE
          Zpos1 = Zii + Zoffs + Zstep*(Jstri-0.5D0)
          Zpos2 = Zii + Zoffs + Zstep*(Jstri+0.5D0)
        ENDIF
        Zpos3 = Zpos2
        Zpos4 = Zpos1
        IF( IFORMA(Jtec,Jsta) < 2 ) THEN
          S0n = Soon + Soffs1 + Sstep/2.D0
          S0p = Soop - Soffs1 - Sstep/2.D0
        ELSEIF( JspliLoc == 1 ) THEN
          S0n = Soon + Soffs1 + Sstep/2.D0
          S0p = S0n           + Sstep*NstriS
        ELSE
          S0n = Soo  + Soffs2 + Sstep/2.D0
          S0p = S0n           + Sstep*NstriS
        ENDIF
        Spos1 = S0n + (Zpos1-Zoo) * (ASS+AM1*(S0n-Saap)) / (AP1*Dzz)
        Spos2 = S0n + (Zpos2-Zoo) * (ASS+AM1*(S0n-Saap)) / (AP1*Dzz)
        Spos3 = S0p + (Zpos3-Zoo) * (ASS+AM1*(S0p-Saap)) / (AP1*Dzz)
        Spos4 = S0p + (Zpos4-Zoo) * (ASS+AM1*(S0p-Saap)) / (AP1*Dzz)
      ENDIF
!
      SZT(1:3,1) = (/ Spos1, Zpos1, Tpos /)
      SZT(1:3,2) = (/ Spos2, Zpos2, Tpos /)
      SZT(1:3,3) = (/ Spos3, Zpos3, Tpos /)
      SZT(1:3,4) = (/ Spos4, Zpos4, Tpos /)
      Ier = 0
!
 END SUBROUTINE FGetIdealStripPositionFromRPCindex
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetIdealChannelPositionFromTGCind( Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, SZT, Ier )
 IMPLICIT REAL(8) (A-H,O-Z)
 INTEGER,      INTENT(IN)  :: Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8),      INTENT(OUT) :: SZT(3,4)
 INTEGER,      INTENT(OUT) :: Ier
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      SZT(1:3,1:4) = 0.D0
      Ier = 1
!
      IF( Jtyp < 1 )                  RETURN
      IF( Jff  < 1 )                  RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )                  RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'TGC' )     RETURN
      Jsta = ISTA(Jtyp,Jgeo,Job)
      IF( Jsta < 1 )                  RETURN
      IF( IFORMA(Jtec,Jsta) == 1 ) THEN
        NSsl = NLAS(Jtec,Jsta)
        NZsl = NLAZ(Jtec,Jsta)
      ELSEIF( IFORMA(Jtec,Jsta) >= 2 .AND. IFORMA(Jtec,Jsta) <= 6 ) THEN
        NSsl = ( NLAZ(Jtec,Jsta) - 3 ) / 4
        NZsl = NSsl
      ELSE
                                      RETURN
      ENDIF
      IF( Jsz == 1 ) THEN
        IF( Jsl < 1 .OR. Jsl > NSsl ) RETURN
      ELSEIF( Jsz == 2 ) THEN
        IF( Jsl < 1 .OR. Jsl > NZsl ) RETURN
      ELSE
        IF( Jspli < 999999999 )       RETURN
      ENDIF
!
      IF( IFORMA(Jtec,Jsta) == 1 ) THEN
        Sstep = STAPP(Jtec,Jsta)
        Zstep = STAPP(Jtec,Jsta)
        Soffs = Sstep/2.D0 * MOD(Jsl-1,2)
        Zoffs = Zstep/2.D0 * MOD(Jsl-1,2)
        DtLoc =           STAOO(Jtec,Jsta,1) + STAOO(Jtec,Jsta,2)/2.D0   &
              + (Jsl-1)*( STAOO(Jtec,Jsta,2) + STAOO(Jtec,Jsta,3) )
      ELSE
        IF(     IFORMA(Jtec,Jsta) == 2 ) THEN
          Sstep  = 30.0D0
          Zstep  = 30.0D0
          Soffs  = Sstep/2.D0 * MOD(Jsl-1,2)
          Zoffs  = Zstep/2.D0 * MOD(Jsl-1,2)
        ELSEIF( IFORMA(Jtec,Jsta) == 3 ) THEN
          Sstep  = STATT(Jtec,Jsta,NLAZ(Jtec,Jsta)+1)
          Zstep  = STAOO(Jtec,Jsta,NLAZ(Jtec,Jsta)+1)
          Soffs  = Sstep/2.D0 * MOD(Jsl-1,2)
          Zoffs  = Zstep/2.D0 * MOD(Jsl-1,2)
        ELSEIF( IFORMA(Jtec,Jsta) >= 4 .AND. IFORMA(Jtec,Jsta) <= 6 ) THEN
          Wstep  = XtgcAdd(Jsta,4)
          Zstep  = Wstep * NtgcGang(Jsta,1,Jsl)
          NstriS = NtgcAdd(Jsta,Jsl+3)
          IF( NstriS <= 1 ) NstriS = MAX( NtgcAdd(Jsta,4), NtgcAdd(Jsta,5), NtgcAdd(Jsta,6) )
          NstriZ = NtgcAdd(Jsta,Jsl)
          NtotGang = SUM( NtgcGang(Jsta,1:NstriZ,Jsl) )
          IF( IFORMA(Jtec,Jsta) < 6 ) THEN
            Sstep = XtgcAdd(Jsta,5)
          ELSE
            Sstep = ( XtgcStrp(Jsta,NstriS,1) - XtgcStrp(Jsta,1,1)                   &
                    + XtgcStrp(Jsta,NstriS,2) - XtgcStrp(Jsta,1,2) ) / 2.D0 / NstriS
          ENDIF
          IF( Jsz == 2 ) THEN
            NtriGangP =        SUM( NtgcGang(Jsta,1:Jstri,Jsl) )
            NtriGangN = NtriGangP - NtgcGang(Jsta, Jstri ,Jsl)
          ENDIF
          Soffs  = ( Geo_Ws(Jtyp,Jgeo,Job) + Geo_Wl(Jtyp,Jgeo,Job) ) / 4.D0           &
                   - NstriS * Sstep / 2.D0 - Sstep / 2.D0
          Zoffs  = ( Geo_Le(Jtyp,Jgeo,Job) - NtotGang * Wstep ) / 2.D0 - Zstep / 2.D0
        ENDIF
        DtLoc = ( STATT(Jtec,Jsta,4*Jsl) + STATT(Jtec,Jsta,4*Jsl+1) ) / 2.D0
      ENDIF
!
      Sisi = SIGN( 1.D0 , Pos_Z(Jtyp,Jff,Jzz) )
      Zii  = Pos_R(Jtyp,Jff,Jzz) + Geo_dy(Jtyp,Jgeo,Job)
      Zaa  = Zii + Geo_Le(Jtyp,Jgeo,Job)
      Siin = - Geo_Ws(Jtyp,Jgeo,Job) / 2.D0
      Siip =   Geo_Ws(Jtyp,Jgeo,Job) / 2.D0
      Saan = - Geo_Wl(Jtyp,Jgeo,Job) / 2.D0
      Saap =   Geo_Wl(Jtyp,Jgeo,Job) / 2.D0
      Soon = (Siin+Saan)/2.D0
      Soop = (Siip+Saap)/2.D0
      Zoo  = (Zaa +Zii )/2.D0
!---- Scen = Siin + (Siip-Siin) * (Saan-Siin) / (Saan-Siin-Saap+Siip)
!---- Zcen = Zii  + (Siip-Siin) * (Zaa -Zii ) / (Saan-Siin-Saap+Siip)
      Scen = 0.D0
      Zcen = Zii - Geo_dy(Jtyp,Jgeo,Job)
      IF( Jsz == 1 ) THEN
        IF( IFORMA(Jtec,Jsta) < 6 ) THEN
          Zpos1 = Zii + Zoffs + Zstep/2.D0
          Zpos2 = Zpos1
          Zpos3 = Zaa - Zoffs - Zstep/2.D0
          Zpos4 = Zpos3
!-- Bug Yoji ----------------------------------------
!-------- S0n   = Soon + Soffs + Sstep*(Jstri-0.5D0)
!-------- S0p   = Soon + Soffs + Sstep*(Jstri+0.5D0)
          S0n   = Soon + Soffs + Sstep*(Jstri+0.5D0)
          S0p   = Soon + Soffs + Sstep*(Jstri+1.5D0)
!----------------------------------------------------
          Spos1 = Scen + (S0n-Scen) * (Zpos1-Zcen) / (Zoo-Zcen)
          Spos2 = Scen + (S0p-Scen) * (Zpos2-Zcen) / (Zoo-Zcen)
          Spos3 = Scen + (S0p-Scen) * (Zpos3-Zcen) / (Zoo-Zcen)
          Spos4 = Scen + (S0n-Scen) * (Zpos4-Zcen) / (Zoo-Zcen)
        ELSE
          Zpos1 = Zii + XtgcAdd(Jsta,2)
          Zpos2 = Zpos1
          Zpos3 = Zaa - XtgcAdd(Jsta,3)
          Zpos4 = Zpos3
!--JFL Fri Jan 21 12:29:57 CET 2005
          JstriC=Jstri
          if (Jzz < 0) JstriC = NstriS - Jstri + 1
          IF( Jsl <= 1 ) THEN
            Spos1 =   XtgcStrp(Jsta,JstriC  ,2)
            Spos2 =   XtgcStrp(Jsta,JstriC+1,2)
            Spos3 =   XtgcStrp(Jsta,JstriC+1,1)
            Spos4 =   XtgcStrp(Jsta,JstriC  ,1)
          ELSE
            JstriInv = NstriS - JstriC + 1
            Spos1 = - XtgcStrp(Jsta,JstriInv  ,2)
            Spos2 = - XtgcStrp(Jsta,JstriInv+1,2)
            Spos3 = - XtgcStrp(Jsta,JstriInv+1,1)
            Spos4 = - XtgcStrp(Jsta,JstriInv  ,1)
          ENDIF
          if( Jzz < 0 ) then
            Spos1 = - Spos1
            Spos2 = - Spos2
            Spos3 = - Spos3
            Spos4 = - Spos4
          endif
!--JFL
        ENDIF
      ELSE
        IF( IFORMA(Jtec,Jsta) < 4 ) THEN
          Zpos1 = Zii + Zoffs + Zstep*(Jstri-0.5D0)
          Zpos2 = Zii + Zoffs + Zstep*(Jstri+0.5D0)
        ELSE
          Zpos1 = Zii + Zoffs + Zstep/2.D0 + NtriGangN*Wstep
          Zpos2 = Zii + Zoffs + Zstep/2.D0 + NtriGangP*Wstep
        ENDIF
        Zpos3 = Zpos2 
        Zpos4 = Zpos1
        IF( IFORMA(Jtec,Jsta) < 6 ) THEN
          S0n   = Soon + Soffs + Sstep/2.D0
          S0p   = Soop - Soffs - Sstep/2.D0
          Spos1 = Scen + (S0n-Scen) * (Zpos1-Zcen) / (Zoo-Zcen)
          Spos2 = Scen + (S0n-Scen) * (Zpos2-Zcen) / (Zoo-Zcen)
          Spos3 = Scen + (S0p-Scen) * (Zpos3-Zcen) / (Zoo-Zcen)
          Spos4 = Scen + (S0p-Scen) * (Zpos4-Zcen) / (Zoo-Zcen)
        ELSE
          Ez1 = ( Zpos1 - Zii - XtgcAdd(Jsta,2) ) / ( Zaa - Zii - XtgcAdd(Jsta,2) - XtgcAdd(Jsta,3) )
          Ez2 = ( Zpos2 - Zii - XtgcAdd(Jsta,2) ) / ( Zaa - Zii - XtgcAdd(Jsta,2) - XtgcAdd(Jsta,3) )
          NsM = NstriS + 1
          Spos1 = XtgcStrp(Jsta, 1 ,2) + Ez1 *(XtgcStrp(Jsta, 1 ,1)-XtgcStrp(Jsta, 1 ,2))
          Spos2 = XtgcStrp(Jsta, 1 ,2) + Ez2 *(XtgcStrp(Jsta, 1 ,1)-XtgcStrp(Jsta, 1 ,2))
          Spos3 = XtgcStrp(Jsta,NsM,2) + Ez2 *(XtgcStrp(Jsta,NsM,1)-XtgcStrp(Jsta,NsM,2))
          Spos4 = XtgcStrp(Jsta,NsM,2) + Ez1 *(XtgcStrp(Jsta,NsM,1)-XtgcStrp(Jsta,NsM,2))
        ENDIF
      ENDIF
      Tpos = Pos_Z(Jtyp,Jff,Jzz) + Sisi*Geo_dz(Jtyp,Jgeo,Job) + Sisi*DtLoc
!
      SZT(1:3,1) = (/ Spos1, Zpos1, Tpos /)
      SZT(1:3,2) = (/ Spos2, Zpos2, Tpos /)
      SZT(1:3,3) = (/ Spos3, Zpos3, Tpos /)
      SZT(1:3,4) = (/ Spos4, Zpos4, Tpos /)
      Ier = 0
!
 END SUBROUTINE FGetIdealChannelPositionFromTGCind
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetCSCindexFromStripPosition( Ctyp,Jff,Jzz,Job,Jsl,SZflag, XYZ, JStrip )
 IMPLICIT REAL(8) (A-H,O-Z)
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jsl
 REAL(8),      INTENT(IN)  :: SZflag
 REAL(8),      INTENT(IN)  :: XYZ(3)
 INTEGER,      INTENT(OUT) :: JStrip
 REAL(8) :: SZT_Direct(3)
 INTEGER :: Iprint
 INTEGER :: IStatus
 INTEGER :: IAmdcVersion
 REAL(8) :: SZTFixed(3),SZTCc(3)
 REAL(8) :: SzzttCc(3),SzzttCcF(3),SzzttCcR(3)
 REAL(8) :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab
 REAL(8) :: Ziib,Zoob,Zaab
 REAL(8) :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
 REAL(8) :: Ziip,Zoop,Zaap
 REAL(8) :: PitchEta1stStrip,PitchPhi2ndStrip
 INTEGER :: NberWireLayer
 REAL(8) :: CUFOAMG10,GASGAP
 REAL(8) :: Gama0,FOs
 REAL(8) :: ttLayer(2,4)
 REAL(8) :: ShiftStrip
 REAL(8) :: SzzttStrip(3,5)
 REAL(8) :: XYZStrip(3,5)
 INTEGER :: NbStrips2ndCo
 INTEGER :: NbStrips1stCo
!--------------------------------------------------------------------
! Compute The strip number of CSC Strip 
! from the other Ids and Global coordinates
!
! Ctyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the CSC multilayer
!  Jsl   is the CSC strips monolayer index (1 to 3 or 4)
!  SZflag   is a flag which is  <0  for the 2nd coordinate strips of CSC
!                          and  >0  for the 1rst coordinate strips of CSC
! XYZ are Global coordinates
!
! JStrip  is the hit CSC strips number in the monolayer
!
!--------------------------------------------------------------------
!
!--Validity
      JStrip = 0
      call CscStripValid( IStatus,Ctyp,Jff,Jzz,Job,Jsl,SZflag )
      if ( IStatus.eq.0 ) return                            
!
      Iprint = 0
      if (Iprint.eq.1) then
        PRINT 1000,Ctyp,Jff,Jzz,Job,Jsl,SZflag
1000    format(/'     /Name/Jff/Jzz/ Job/Jsl/SZflag/',A3,4I4,F6.1)
      endif
!
!-- Local coordinates
      call CSCGlobalToLocal( Ctyp,Jff,Jzz,Job,Jsl, SZT_Direct, XYZ )
!
!--Basics of Csc Geometry
      call CscGeoBasics(Iprint,                                          &
                        Ctyp,Jff,Jzz,Job,                                &
                        IAmdcVersion,                                    &
                        SZTFixed,SZTCc,                                  &
                        SzzttCc,SzzttCcF,SzzttCcR,                       &
                        S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,   &
                        S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,   &
                        PitchEta1stStrip,PitchPhi2ndStrip,               &
                        NberWireLayer,CUFOAMG10,GASGAP,                  &
                        Gama0,FOs)
!
!--Wire Layer Model
      call CscWireLayerModel(Iprint,                            &
                             IAmdcVersion,                      &
                             Ctyp,Jff,Jzz,Job,                  &
                             SzzttCc,SzzttCcF,SzzttCcR,         &
                             NberWireLayer,CUFOAMG10,GASGAP,    &
                             ttLayer)
!
      JStrip = 1 
!
!-- Strip Model
      IF( SZflag < 0.D0 ) THEN
!--Phi-strip
!
        call CscPhi2ndCoStripModelBase(Iprint,                                           &
                                       IAmdcVersion,                                     &
                                       Ctyp,Jff,Jzz,Job,Jsl,JStrip,                      &
                                       SzzttCc,SzzttCcF,SzzttCcR,                        &
                                       S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,    &
                                       PitchPhi2ndStrip,                                 &
                                       ttLayer,                                          &
                                       ShiftStrip,                                       &
                                       SzzttStrip,XYZStrip,                              &
                                       NbStrips2ndCo)
!
        RelocatedS=SZT_Direct(1)
        if ( IAmdcVersion < 7 ) then
          SInput  = SZT_Direct(1) - SzzttCc(1)
          zzInput = SZT_Direct(2) - SzzttCc(2)
          ZRelocated = 0.
          call CscGeoNonParallel(Ctyp,                                             &
                                 ZRelocated,                                       &
                                 S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,    &
                                 SInput,zzInput,                                   &
                                 RelocatedS,Relocatedzz)
          RelocatedS=SZTCc(1)+RelocatedS
        endif
!
        JStrip = NINT((RelocatedS-ShiftStrip)/PitchPhi2ndStrip) + 1
        if (JStrip.LT.1.OR.JStrip.GT.NbStrips2ndCo) JStrip = 0
!
      ELSE
!--R-strip
!
        call CscEta1stCoStripModelBase(Iprint,                                         &
                                       IAmdcVersion,                                   &
                                       Ctyp,Jff,Jzz,Job,Jsl,JStrip,                    &
                                       SzzttCc,SzzttCcF,SzzttCcR,                      &
                                       S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,  &
                                       PitchEta1stStrip,                               &
                                       ttLayer,                                        &
                                       ShiftStrip,                                     &
                                       SzzttStrip,XYZStrip,                            &
                                       NbStrips1stCo)
!
       JStrip = NINT((SZT_Direct(2)-ShiftStrip)/PitchEta1stStrip) + 1
       if (JStrip.LT.1.OR.JStrip.GT.NbStrips1stCo) JStrip = 0
!
      ENDIF
!
 END SUBROUTINE FGetCSCindexFromStripPosition
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetChannelPositionFromTGCind( Ctyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, XYZabs, SZTloc )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri
 REAL(8),      INTENT(OUT) :: XYZabs(3,4),SZTloc(3,4)
 REAL(8) :: SZT(3,4),SZTtru(3,4)
 REAL(8) :: SiFi,CoFi
 INTEGER :: Ier,Jtyp,I,JTROU3
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
!
    XYZabs(1:3,1:4) = 0.D0
    SZTloc(1:3,1:4) = 0.D0
!
    Jtyp = JTROU3(Ctyp,MTYP,CARTYP)
    CALL FGetIdealChannelPositionFromTGCind( Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri, SZT, Ier )
    IF( Ier /= 0 ) RETURN
    CALL FPerformDirLocDisplacAndDeformJtypBasedMany( Jtyp,Jff,Jzz,Job,SZT(1,1),SZTtru)
!
    SZTloc(1:3,1:4) = SZTtru(1:3,1:4)
    IF( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) < 1 ) THEN
      DO I=1,4
        CALL SWAP_ZT( SZTtru(2,I), SZTtru(3,I) )
      ENDDO 
    ENDIF
    SiFi = SIN( Pos_Phi_V(JTYP_F_Z(Jtyp,Jff,Jzz)) )
    CoFi = COS( Pos_Phi_V(JTYP_F_Z(Jtyp,Jff,Jzz)) )
    DO I=1,4
      XYZabs(1,I)   = CoFi*SZTtru(3,I) - SiFi*SZTtru(1,I)
      XYZabs(2,I)   = SiFi*SZTtru(3,I) + CoFi*SZTtru(1,I)
      XYZabs(3,I)   =      SZTtru(2,I) 
    ENDDO
!
 END SUBROUTINE FGetChannelPositionFromTGCind
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE FGetStripPositionFromCSCindex( Ctyp,Jff,Jzz,Job,Jsl,JStrip,SZflag, XYZabs, SZTloc )
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN)  :: Ctyp
 INTEGER,      INTENT(IN)  :: Jff,Jzz,Job,Jsl,JStrip
 REAL(8),      INTENT(IN)  :: SZflag
 REAL(8),      INTENT(OUT) :: XYZabs(3,5),SZTloc(3,5)
 INTEGER :: Iprint, IStatus, IAmdcVersion
 REAL(8) :: SZTFixed(3),SZTCc(3)
 REAL(8) :: SzzttCc(3),SzzttCcF(3),SzzttCcR(3)
 REAL(8) :: S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab      
 REAL(8) :: Ziib,Zoob,Zaab
 REAL(8) :: S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap
 REAL(8) :: Ziip,Zoop,Zaap
 REAL(8) :: PitchEta1stStrip,PitchPhi2ndStrip
 INTEGER :: NberWireLayer
 REAL(8) :: CUFOAMG10,GASGAP
 REAL(8) :: Gama0,FOs 
 REAL(8) :: ttLayer(2,4)              
 REAL(8) :: ShiftStrip
 INTEGER :: NbStrips2ndCo
 INTEGER :: NbStrips1stCo
!--------------------------------------------------------------------
!
! Compute Local and global coordinates of CSC Strip defined by Ids
!
! Input :
!   Ctyp,Jff,Jzz,Job  are the AMDC_SIMREC indexes defining the CSC multilayer
!   Jsl               is the CSC strips monolayer index (1 to 3 or 4)
!   JStrip            is the hit CSC strips number in the monolayer
!   SZflag            is a flag which is  <0  for the 2nd  coordinate strips of CSC
!                                  and  >0  for the 1rst coordinate strips of CSC
!
! Output :
!   XYZabs(1:3,1:4)  are Global coordinates of the 4 corners of the strip
!   XYZabs(1:3, 5 )  are Global coordinates of the strip center
!   SZTloc(1:3,1:4)  are Local  coordinates of the 4 corners of the strip
!   SZTloc(1:3, 5 )  are Local  coordinates of the strip center
!
!--------------------------------------------------------------------
!>>Validity
      call CscStripValid( IStatus, Ctyp,Jff,Jzz,Job,Jsl,SZflag )
      if( IStatus == 0 ) return                            
!
      Iprint = 0
      if( Iprint == 1 ) then
        PRINT 1000,Ctyp,Jzz,Jff,Job,Jsl,JStrip,SZflag
1000    format(/'     /Name/Jzz/Jff/ Job/Jsl/Jtube/SZflag/',4x,A3,5I4,F6.1)
      endif
!
!>>Basics of Csc Geometry
      call CscGeoBasics(Iprint, Ctyp,Jff,Jzz,Job, IAmdcVersion,           &       
                        SZTFixed,SZTCc, SzzttCc,SzzttCcF,SzzttCcR,        &
                        S1Pb,S2Pib,S2Po1b,S2Po2b,S2Pab,Ziib,Zoob,Zaab,    &
                        S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,    &
                        PitchEta1stStrip,PitchPhi2ndStrip,                &
                        NberWireLayer,CUFOAMG10,GASGAP,                   &
                        Gama0,FOs)
!
!>>Wire Layer Model
      call CscWireLayerModel(Iprint, IAmdcVersion, Ctyp,Jff,Jzz,Job,    &  
                             SzzttCc,SzzttCcF,SzzttCcR,                 &
                             NberWireLayer,CUFOAMG10,GASGAP, ttLayer)
!
!>> Strip Model
      IF( SZflag < 0.D0 ) THEN
!>> Phi-strip ----------------------------------------------------------
!
        call CscPhi2ndCoStripModelBase(Iprint, IAmdcVersion, Ctyp,Jff,Jzz,Job,Jsl,JStrip,     &
                                       SzzttCc,SzzttCcF,SzzttCcR,                             &
                                       S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,         &
                                       PitchPhi2ndStrip, ttLayer, ShiftStrip, SZTloc,XYZabs,  &
                                       NbStrips2ndCo)
!
      ELSE
!>> R-strip -----------------------------------------------------------
!
        call CscEta1stCoStripModelBase(Iprint, IAmdcVersion, Ctyp,Jff,Jzz,Job,Jsl,JStrip,     &
                                       SzzttCc,SzzttCcF,SzzttCcR,                             &
                                       S1Pp,S2Pip,S2Po1p,S2Po2p,S2Pap,Ziip,Zoop,Zaap,         &
                                       PitchEta1stStrip, ttLayer, ShiftStrip, SZTloc,XYZabs,  &
                                       NbStrips1stCo)
      ENDIF
!
 END SUBROUTINE FGetStripPositionFromCSCindex
!
