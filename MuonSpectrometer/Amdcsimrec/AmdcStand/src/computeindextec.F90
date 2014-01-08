!
!> \author SaMuSoG
!
      SUBROUTINE ComputeIndexTEC
      IMPLICIT REAL(8) (A-H,O-Z)
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/coindextec.inc"
!
#include "AmdcStand/copipi.inc"
!
      INTEGER  IoMDT(10),IoCSC(10)
      INTEGER  IoRPC(20),IoTGC(20),IfTGC(180),Norder(180)
      REAL(8)  DzMDT(10),DzCSC(10)
      REAL(8)  DxRPC(20),DyRPC(20),DzRPC(20),DyTGC(20),DfTGC(180)
      LOGICAL  ContainsTGCs(MTYP),NotYetDealtWith(MTYP,MGEO)
!
      INTEGER     IVersAmdc, KVersAmdc
!
      DATA               Dz0MDT      /                  70.D0        /
      DATA               Dz0CSC      /                  60.D0        /
      DATA Dx0RPC,Dy0RPC,Dz0RPC      / 500.D0, 200.D0, 250.D0        /
      DATA        Dy0TGC,      Df0TGC/         300.D0,        0.03D0 /
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      IF( MTYP_F_Z.LT.MTYP_G_O ) THEN
         PRINT 1000
1000     FORMAT(/' In routine  ComputeIndexTEC  of the AMDC package,',   &
                /' the parameter MTYP_F_Z (',I5,') is lower than',       &
                /' the parameter MTYP_G_O (',I5,')  =====>  STOP !')
         STOP
      ENDIF
      IndexMDT(0:MTYP_G_O) = 0
      IndexCSC(0:MTYP_G_O) = 0
      IndexRPC(0:MTYP_G_O) = 0
      IndexTGC(0:MTYP_F_Z) = 0
!
      ContainsTGCs(1:MTYP) = .FALSE.
      NotYetDealtWith(1:MTYP,1:MGEO) = .TRUE.
!
      DO 200 Jtyp=1,MTYP
       DO 110 Jff=1,8
        DO 100 Jzz=-MZZ,MZZ
         Jgeo = IGEO(Jtyp,Jff,Jzz)
         IF( Jgeo.LE.0 ) CYCLE
!
         NbMDT = 0
         NbCSC = 0
         NbRPC = 0
         NbTGC = 0
         IF( NotYetDealtWith(Jtyp,Jgeo) ) THEN
            Jtyp_ff_zz = JTYP_F_Z(Jtyp,Jff,Jzz)
            DO Job=1,NOBJ(Jtyp,Jgeo)
             Jtyp_geo_obj = JTYP_G_O(Jtyp,Jgeo,Job)
             Jtec = ITEC_V(Jtyp_geo_obj)
             IF(     CARTEC(Jtec).EQ.'MDT' ) THEN
                NbMDT = MIN( NbMDT+1 , 10 )
                IoMDT(NbMDT) = Jtyp_geo_obj
                DzMDT(NbMDT) = Geo_dz_V(Jtyp_geo_obj)
             ELSEIF( CARTEC(Jtec).EQ.'CSC' ) THEN
                NbCSC = MIN( NbCSC+1 , 10 )
                IoCSC(NbCSC) = Jtyp_geo_obj
                DzCSC(NbCSC) = Geo_dz_V(Jtyp_geo_obj)
             ELSEIF( CARTEC(Jtec).EQ.'RPC' ) THEN
                SignZ = 1.D0
                IF( Jzz < 0 )  SignZ = - SignZ
                IF( ABS(Pos_Alfa_V(Jtyp_ff_zz)-180.D0) < 0.1D0 .AND. IVERSION_AMDC < 7 ) SignZ = - SignZ
                NbRPC = MIN( NbRPC+1 , 20 )
                IoRPC(NbRPC) = Jtyp_geo_obj
                DxRPC(NbRPC) = Geo_dx_V(Jtyp_geo_obj)
                DyRPC(NbRPC) = Geo_dy_V(Jtyp_geo_obj) * SignZ
                DzRPC(NbRPC) = Geo_dz_V(Jtyp_geo_obj)
             ELSEIF( CARTEC(Jtec).EQ.'TGC' ) THEN
                NbTGC = MIN( NbTGC+1 , 20 )
                IoTGC(NbTGC) = Jtyp_geo_obj
                DyTGC(NbTGC) = Geo_dy_V(Jtyp_geo_obj)
                ContainsTGCs(Jtyp) = .TRUE.
             ENDIF
            ENDDO
            NotYetDealtWith(Jtyp,Jgeo) = .FALSE.
         ENDIF
!
!-- Compute specific MDT's indexes along T (or R) which is locally z in the barrel
         IF( NbMDT.GE.1 ) THEN
            CALL CROISS(DzMDT,NbMDT,Norder)
            Ind = 110
            DzLast = -999999999.D0
            DO J=1,NbMDT
             Jord = Norder(J)
             IF( DzMDT(Jord)-DzLast.GT.Dz0MDT ) THEN
                Ind = Ind + 1
                DzLast = DzMDT(Jord)
             ENDIF
             IndexMDT(IoMDT(Jord)) = IndexMDT(IoMDT(Jord)) + Ind
            ENDDO
         ENDIF
!
!-- Compute specific CSC's indexes along Z which is locally z in the end-caps
         IF( NbCSC.GE.1 ) THEN
            CALL CROISS(DzCSC,NbCSC,Norder)
            Ind = 110
            DzLast = -999999999.D0
            DO J=1,NbCSC
             Jord = Norder(J)
             IF( DzCSC(Jord)-DzLast.GT.Dz0CSC ) THEN
                Ind = Ind + 1
                DzLast = DzCSC(Jord)
             ENDIF
             IndexCSC(IoCSC(Jord)) = IndexCSC(IoCSC(Jord)) + Ind
            ENDDO
!-- BE AWARE Correction for R.03   
            IVersAmdc = ICHAR( C_AMDC_NAME(1:1) )
!--         ICHAR( 'A' 'R' 'Z' 'a' 'r' 'z' ) = 65 82 90 97 114 122
            IF( IVersAmdc >= 97 ) IVersAmdc = IVersAmdc - 32
            READ(C_AMDC_NAME(3:4),'(I2)') KVersAmdc
            IF( ( IVersAmdc == 82 .and. KVersAmdc >= 3 ) .or. IVersAmdc >  82 )  THEN
              DO J=1,NbCSC
                IndexCSC(IoCSC(J)) = IndexCSC(IoCSC(J)) + 1
              ENDDO
            ENDIF
         ENDIF
!
!-- Compute specific RPC's indexes
         IF( NbRPC.GE.1 ) THEN
!-- Sort RPC's along T (or R) which is locally z in the barrel
            CALL CROISS(DzRPC,NbRPC,Norder)
            Ind = 0
            DzLast = -999999999.D0
            DO J=1,NbRPC
             Jord = Norder(J)
             IF( DzRPC(Jord)-DzLast.GT.Dz0RPC ) THEN
                Ind = Ind + 1
                DzLast = DzRPC(Jord)
             ENDIF
             IndexRPC(IoRPC(Jord)) = IndexRPC(IoRPC(Jord)) + Ind
            ENDDO
!-- Sort RPC's along Z which is locally y in the barrel, individually for each T
            CALL CROISS(DyRPC,NbRPC,Norder)
            Lmax = Ind
            IndMax = 0
            DO L=1,Lmax
             Ind = 0
             DyLast = -999999999.D0
             NbZmeasurLast = 1
             DO J=1,NbRPC
              Jord = Norder(J)
              IF( IndexRPC(IoRPC(Jord)).EQ.L ) THEN
                 IF( DyRPC(Jord)-DyLast.GT.Dy0RPC ) THEN
                    Ind = Ind + NbZmeasurLast*10
                    Jtec = ITEC_V(IoRPC(Jord))
                    Jsta = ISTA_V(IoRPC(Jord))
                    DyLast = DyRPC(Jord)
                    NbZa   = MAX( 1 , ISPLIY_V(IoRPC(Jord))    )
                    NbZb   = MAX( 1 , NINT(STAOO(Jtec,Jsta,3)) )
                    IF( IFORMA(Jtec,Jsta).NE.2 ) NbZb = 1
                    NbZmeasurLast = MAX( NbZa , NbZb )
                    IndMax = MAX( IndMax , Ind )
                 ENDIF
                 IndexRPC(IoRPC(Jord)) = IndexRPC(IoRPC(Jord)) + Ind
              ENDIF
             ENDDO
            ENDDO
!-- Sort RPC's along S (or Phi) which is locally x, individually for each T and Z
            CALL CROISS(DxRPC,NbRPC,Norder)
            L10max = IndMax
            DO L=1,Lmax
             DO L10=10,L10max,10
              Ind = 0
              DxLast = -999999999.D0
              NbSmeasurLast = 1
              DO J=1,NbRPC
               Jord = Norder(J)
               IF( IndexRPC(IoRPC(Jord)).EQ.L10+L ) THEN
                  IF( DxRPC(Jord)-DxLast.GT.Dx0RPC ) THEN
                     Ind = Ind + NbSmeasurLast*100
                     Jtec = ITEC_V(IoRPC(Jord))
                     Jsta = ISTA_V(IoRPC(Jord))
                     DxLast = DxRPC(Jord)
                     NbSa   = MAX( 1 , ISPLIX_V(IoRPC(Jord))    )
                     NbSb   = MAX( 1 , NINT(STAOO(Jtec,Jsta,2)) )
                     IF( IFORMA(Jtec,Jsta).LT.2 ) NbSb = 1
                     NbSmeasurLast = MAX( NbSa , NbSb )
                  ENDIF
                  IndexRPC(IoRPC(Jord)) = IndexRPC(IoRPC(Jord)) + Ind
               ENDIF
              ENDDO
             ENDDO
            ENDDO
         ENDIF
!
!-- Compute specific TGC's indexes along T (or R) which is locally y in the end-caps
         IF( NbTGC.GE.1 ) THEN
            CALL CROISS(DyTGC,NbTGC,Norder)
            Ind = 0
            DyLast = -999999999.D0
            DO J=1,NbTGC
             Jord = Norder(J)
             IF( DyTGC(Jord)-DyLast > Dy0TGC ) THEN
                Ind = Ind + 1
                DyLast = DyTGC(Jord)
             ENDIF
             IndexTGC(IoTGC(Jord)) = IndexTGC(IoTGC(Jord)) + Ind
            ENDDO
         ENDIF
!
100   CONTINUE
110   CONTINUE
!
!-- Continue computation of specific TGC's indexes along Phi
      IF( ContainsTGCs(Jtyp) ) THEN
         NbPhiTGC = 0
         DO 160 Jff=1,8
           DO 150 Jzz=-MZZ,MZZ
             IF( IGEO(Jtyp,Jff,Jzz).LE.0 ) CYCLE
             NbPhiTGC = MIN( NbPhiTGC+1 , 180 )
             Jtyp_ff_zz = JTYP_F_Z(Jtyp,Jff,Jzz)
             F = Pos_Phi_V(Jtyp_ff_zz)
             IF( F < 0.D0 ) F = F + TWOPI
             IfTGC(NbPhiTGC) = Jtyp_ff_zz
             DfTGC(NbPhiTGC) = F
150        ENDDO
160      ENDDO
         CALL CROISS(DfTGC,NbPhiTGC,Norder)
         Ind = 0
         DfLast = -999999999.D0
         DO J=1,NbPhiTGC
          Jord = Norder(J)
          IF( DfTGC(Jord)-DfLast > Df0TGC ) THEN
             Ind = Ind + 100
             DfLast = DfTGC(Jord)
          ENDIF
          IndexTGC(IfTGC(Jord)) = IndexTGC(IfTGC(Jord)) + Ind
         ENDDO
      ENDIF
!
200   CONTINUE
!
      END SUBROUTINE ComputeIndexTEC
!
