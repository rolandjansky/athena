!
!> \author SaMuSoG  27/03/97
!
 SUBROUTINE WRITE_AMDC_FROMCOM(LUN)
 IMPLICIT REAL(8) (A-H,O-Z)
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
 CHARACTER(1) :: CAR1
 INTEGER      :: LFI(8),LFItot(8)
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
!---------------------------------------------
1000  FORMAT('*')
1001  FORMAT('******************************************************')
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
      WRITE (LUN,1010) C_AMDC_NAME
1010  FORMAT('N ',A16)
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
      WRITE (LUN,1020) IVERSION_AMDC
1020  FORMAT('V ',I5)
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
!---------------------------------------------
!
!
!---------------------------------------------
      DO 110 JTYP=1,MTYP
      WRITE (LUN,1000)
      DO 100 JGEO=1,MGEO
!
      IF(NOBJ(JTYP,JGEO).GE.1) THEN
         WRITE (LUN,1100) CARTYP(JTYP),JGEO,NOBJ(JTYP,JGEO)
1100     FORMAT('D ',A3,I2,2X,I2)
         DO 10 JOB=1,NOBJ(JTYP,JGEO)
         JTEC = ITEC(JTYP,JGEO,JOB)
         JSTA = ISTA(JTYP,JGEO,JOB)
         NLLL = NLAZ(JTEC,JSTA)+NLAS(JTEC,JSTA)
         IF(NLLL.LE.0) THEN
            PRINT 7100,CARTEC(JTEC),JSTA
7100        FORMAT(/' In WRITE_AMDC_FROMCOM, Stacking type : ',A3,I3,           &
                    ' is used but is not defined =====> STOP !'/31X,'********')
            STOP
         ENDIF
         IF( Geo_dy(JTYP,JGEO,JOB).GT.9999.99D0 ) THEN
           WRITE (LUN,1201) Geo_dx(JTYP,JGEO,JOB),Geo_dy(JTYP,JGEO,JOB)   &
                           ,Geo_dz(JTYP,JGEO,JOB),JOB,CARTEC(JTEC),JSTA   &
                           ,ISPLIX(JTYP,JGEO,JOB),ISPLIY(JTYP,JGEO,JOB)   &
                           ,ISHAPE(JTYP,JGEO,JOB)                         &
                           ,Geo_Ws(JTYP,JGEO,JOB),Geo_Wl(JTYP,JGEO,JOB)   &
                           ,Geo_Le(JTYP,JGEO,JOB),Geo_Ex(JTYP,JGEO,JOB)   &
                           ,Geo_D1(JTYP,JGEO,JOB),Geo_D2(JTYP,JGEO,JOB)   &
                           ,Geo_D3(JTYP,JGEO,JOB)
1201       FORMAT(F7.2,F7.1,F7.2,I2,1X,A3,4I2,3F8.2,4F7.2)
         ELSE
           WRITE (LUN,1200) Geo_dx(JTYP,JGEO,JOB),Geo_dy(JTYP,JGEO,JOB)   &
                           ,Geo_dz(JTYP,JGEO,JOB),JOB,CARTEC(JTEC),JSTA   &
                           ,ISPLIX(JTYP,JGEO,JOB),ISPLIY(JTYP,JGEO,JOB)   &
                           ,ISHAPE(JTYP,JGEO,JOB)                         &
                           ,Geo_Ws(JTYP,JGEO,JOB),Geo_Wl(JTYP,JGEO,JOB)   &
                           ,Geo_Le(JTYP,JGEO,JOB),Geo_Ex(JTYP,JGEO,JOB)   &
                           ,Geo_D1(JTYP,JGEO,JOB),Geo_D2(JTYP,JGEO,JOB)   &
                           ,Geo_D3(JTYP,JGEO,JOB)
1200       FORMAT(3F7.2,I2,1X,A3,4I2,3F8.2,4F7.2)
         ENDIF
10       ENDDO
      ENDIF
!
      DO 30 JCUT=1,4
      IF(INOCUT(JTYP,JGEO,JCUT).GE.1) THEN
         ITOTCU = INOCUT(JTYP,JGEO,JCUT)
         WRITE (LUN,1300) CARTYP(JTYP),JGEO,JCUT,NOBCUT(ITOTCU)
1300     FORMAT('H ',A3,3I2)
         DO 20 JOB=1,NOBCUT(ITOTCU)
         IF( IOBCUT(ITOTCU,JOB).LE.0 .OR. IOBCUT(ITOTCU,JOB).GT.NOBJ(JTYP,JGEO) ) THEN
            PRINT 7200,CARTYP(JTYP),JGEO,JCUT,IOBCUT(ITOTCU,JOB)
7200        FORMAT(/' In WRITE_AMDC_FROMCOM, In cut-out type : ',       &
                    A3,2I3,' object index : ',I3,                       &
                    ' is not defined =====> STOP !'/33X,'***********')
            STOP
         ENDIF
         IF( Cut_dx(ITOTCU,JOB).LT.-999.99D0 ) THEN
            WRITE (LUN,1401) Cut_dx(ITOTCU,JOB), Cut_dy(ITOTCU,JOB),   &
                             IOBCUT(ITOTCU,JOB),                       &
                             Cut_Ws(ITOTCU,JOB), Cut_Wl(ITOTCU,JOB),   &
                             Cut_Le(ITOTCU,JOB),                       &
                             Cut_Ex(ITOTCU,JOB), Cut_An(ITOTCU,JOB)
1401        FORMAT(F7.1,F7.2,7X,I2,12X,3F8.2,2F7.2)
         ELSE
            WRITE (LUN,1400) Cut_dx(ITOTCU,JOB), Cut_dy(ITOTCU,JOB),   &
                             IOBCUT(ITOTCU,JOB),                       &
                             Cut_Ws(ITOTCU,JOB), Cut_Wl(ITOTCU,JOB),   &
                             Cut_Le(ITOTCU,JOB),                       &
                             Cut_Ex(ITOTCU,JOB), Cut_An(ITOTCU,JOB)
1400        FORMAT(2F7.2,7X,I2,12X,3F8.2,2F7.2)
         ENDIF
20       CONTINUE
      ENDIF
30    CONTINUE
!
100   CONTINUE
110   CONTINUE
!---------------------------------------------
!
!
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
!
!
!---------------------------------------------
      DO 230 JTYP=1,MTYP
      WRITE (LUN,1000)
      DO 220 JZZ=-MZZ,MZZ
      LFItot(1:8) = 0
      NSECTO = 8
      DO 210 JFF=1,8
         IF( LFItot(JFF).GE.1 ) CYCLE
         JGEO = IGEO(JTYP,JFF,JZZ)
         IF(JGEO.LE.0)          CYCLE
         JCUT = ICUT(JTYP,JFF,JZZ)
         IF(NOBJ(JTYP,JGEO).LE.0) THEN
            PRINT 7300,CARTYP(JTYP),JGEO
7300        FORMAT(/' In WRITE_AMDC_FROMCOM, Station type : ',         &
                    A3,I3,' is used but is not defined =====> STOP !'  &
                   /30X,'********')
            STOP
         ENDIF
         IF(JCUT.GE.1) THEN
          IF(INOCUT(JTYP,JGEO,JCUT).LE.0) THEN
            PRINT 7400,CARTYP(JTYP),JGEO,JCUT
7400        FORMAT(/' In WRITE_AMDC_FROMCOM, Cut-out type : ',         &
                    A3,2I3,' is used but is not defined =====> STOP !' &
                   /30X,'***********')
            STOP
          ENDIF
         ENDIF
         PI2SN = TWOPI / NSECTO
         PHI0 = ( Pos_Phi(JTYP,JFF,JZZ) - (JFF-1.)*PI2SN ) / PIS180
!
         LFI(1:8) = 0
         DO KFF=1,8
           IF( IGEO(JTYP,KFF,JZZ).NE.JGEO )   CYCLE
           PHI0loc = ( Pos_Phi(JTYP,KFF,JZZ) - (KFF-1.)*PI2SN ) / PIS180
           IF( ABS(PHI0-PHI0loc).GT.0.001D0 ) CYCLE
           IF( ABS(Pos_Z(JTYP,JFF,JZZ)-Pos_Z(JTYP,KFF,JZZ)).GT.0.001D0        &
           .OR.ABS(Pos_R(JTYP,JFF,JZZ)-Pos_R(JTYP,KFF,JZZ)).GT.0.001D0        &
           .OR.ABS(Pos_S(JTYP,JFF,JZZ)-Pos_S(JTYP,KFF,JZZ)).GT.0.001D0        &
           .OR.ABS(Pos_Alfa(JTYP,JFF,JZZ)-Pos_Alfa(JTYP,KFF,JZZ)).GT.0.01D0   &
           .OR.ABS(Pos_Beta(JTYP,JFF,JZZ)-Pos_Beta(JTYP,KFF,JZZ)).GT.0.01D0   &
           .OR.ABS(Pos_Gama(JTYP,JFF,JZZ)-Pos_Gama(JTYP,KFF,JZZ)).GT.0.01D0 )  CYCLE
           LFI(KFF)    = 1
           IF( IVERSION_AMDC > 6 ) LFI(JFF) = 2
           LFItot(KFF) = 1
         ENDDO
!
         CAR1 = ' '
         IF( CARTYP(JTYP)(1:1).EQ.'B' ) THEN
            IF( IBAREL(JTYP,JFF,JZZ) < 1 ) CAR1 = 'e'
         ELSE
            IF( IBAREL(JTYP,JFF,JZZ) > 0 ) CAR1 = 'b'
         ENDIF
         IF( NSECTO.EQ.8 ) THEN
           WRITE (LUN,2100) CAR1,CARTYP(JTYP),JGEO,JCUT,       &
                   LFI(1:8), JZZ, PHI0,                        &
           Pos_Z   (JTYP,JFF,JZZ),    Pos_R   (JTYP,JFF,JZZ),  &
           Pos_S   (JTYP,JFF,JZZ),    Pos_Alfa(JTYP,JFF,JZZ),  &
           Pos_Beta(JTYP,JFF,JZZ),    Pos_Gama(JTYP,JFF,JZZ)
         ELSE
           WRITE (LUN,2200) CAR1,CARTYP(JTYP),JGEO,JCUT,       &
           NSECTO, LFI(1:6), JZZ, PHI0,                        &
           Pos_Z   (JTYP,JFF,JZZ),    Pos_R   (JTYP,JFF,JZZ),  &
           Pos_S   (JTYP,JFF,JZZ),    Pos_Alfa(JTYP,JFF,JZZ),  &
           Pos_Beta(JTYP,JFF,JZZ),    Pos_Gama(JTYP,JFF,JZZ)
         ENDIF
2100     FORMAT('P',A1,A3,2I2,1X,   8I1,   I3,3F9.2,4F8.2)
2200     FORMAT('P',A1,A3,2I2,1X,I1,1X,6I1,I3,3F9.2,4F8.2)
210   CONTINUE
220   CONTINUE
230   CONTINUE
!---------------------------------------------
!
!
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
!
!
!---------------------------------------------
      DO 330 JTEC=1,MTEC
      WRITE (LUN,1000)
      DO 320 JSTA=1,MSTA
        NL1 = NLAS(JTEC,JSTA)
        NL2 = NLAZ(JTEC,JSTA)
        IF( NL1.LE.0 .AND. NL2.LE.0 ) CYCLE
        X0X0 = STAX0(JTEC,JSTA)
        PP  = STAPP(JTEC,JSTA)
        RR  = STARR(JTEC,JSTA)
        EE  = STAEE(JTEC,JSTA)
        PG  = STAPG(JTEC,JSTA)
        EMI = 0.D0
        IF( CARTEC(JTEC).EQ.'TGC' .AND. IFORMA(JTEC,JSTA).GE.2 ) THEN
           PP  = STAEE(JTEC,JSTA)
           EE  = STAPP(JTEC,JSTA)
           IF( IFORMA(JTEC,JSTA).GE.3 ) THEN
              PG  = STATT(JTEC,JSTA,NL2+1)
              EMI = STAOO(JTEC,JSTA,NL2+1)
           ENDIF
        ENDIF
        IF( X0X0.GT.990.D0 ) THEN
          IF( NL1.LE.0 ) THEN
            WRITE (LUN,3110) CARTEC(JTEC),JSTA,    NL2,PP,RR,EE,PG,EMI
3110        FORMAT('W ',A3,I2,  7X, I2,8F8.2)
          ELSE
            WRITE (LUN,3120) CARTEC(JTEC),JSTA,NL1,NL2,PP,RR,EE,PG,EMI
3120        FORMAT('W ',A3,I2,  5X,2I2,8F8.2)
          ENDIF
        ELSE
          X0X0 = MIN( 9.999D0 , X0X0 )
          IF( X0X0.LT.1.D0 ) THEN
            IX0X0  = 1000.D0* X0X0
            IX0X01 =  IX0X0 / 100
            IX0X02 = (IX0X0-IX0X01*100) / 10
            IX0X03 =  IX0X0-IX0X01*100-IX0X02*10
            IF( NL1.LE.0 ) THEN
              WRITE (LUN,3210) CARTEC(JTEC),JSTA,IX0X01,IX0X02,IX0X03,NL2,PP,RR,EE,PG,EMI
3210          FORMAT('W ',A3,I2,' .',3I1, I4,8F8.2)
            ELSE
              WRITE (LUN,3220) CARTEC(JTEC),JSTA,IX0X01,IX0X02,IX0X03,NL1,NL2,PP,RR,EE,PG,EMI
3220          FORMAT('W ',A3,I2,' .',3I1,2I2,8F8.2)
            ENDIF
          ELSE
            IF( NL1.LE.0 ) THEN
              WRITE (LUN,3310) CARTEC(JTEC),JSTA,X0X0,NL2,PP,RR,EE,PG,EMI
3310          FORMAT('W ',A3,I2,F5.3, I4,8F8.2)
            ELSE
              WRITE (LUN,3320) CARTEC(JTEC),JSTA,X0X0,NL1,NL2,PP,RR,EE,PG,EMI
3320          FORMAT('W ',A3,I2,F5.3,2I2,8F8.2)
            ENDIF
          ENDIF
        ENDIF
        IF( CARTEC(JTEC).EQ.'MDT' ) THEN
!-------------------------> *****
          WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,NL2)
          WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,NL2+1)
3400      FORMAT(16X,8F8.2)
        ELSEIF( CARTEC(JTEC).EQ.'RPC' ) THEN
!-----------------------------> *****
          IF( IFORMA(JTEC,JSTA).EQ.0 ) THEN
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,8)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,8)
          ELSEIF( IFORMA(JTEC,JSTA).EQ.1 ) THEN
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,5)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=6,10)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,4)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=5,6)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=7,11)
          ELSEIF( IFORMA(JTEC,JSTA).EQ.2 ) THEN
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 6)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=2,3)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 4)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=5,6)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=7,8)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 9)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=10,12)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=13,14)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=15,16)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=17,18)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=19,20)
            WRITE (LUN,3400)  STAOO(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 4),  &
                              STAOO(JTEC,JSTA, 5)
            WRITE (LUN,3500) (NINT(STAOO(JTEC,JSTA,IL)),IL=2,3)
3500        FORMAT(16X,8I8)
          ELSEIF( IFORMA(JTEC,JSTA).EQ.4 ) THEN
            IF( JSTA.EQ.1 ) THEN
              WRITE (LUN,3600)  STATT(JTEC,JSTA, 4)
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=5,6)
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=7,8)
              WRITE (LUN,3600)  STATT(JTEC,JSTA, 9)
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=10,12)
              WRITE (LUN,3600)  STATT(JTEC,JSTA,13),STATT(JTEC,JSTA,14),  &
                                STAOO(JTEC,JSTA, 7),STAOO(JTEC,JSTA, 8)
              WRITE (LUN,3600)  STATT(JTEC,JSTA,15),STATT(JTEC,JSTA,16),  &
                                STAOO(JTEC,JSTA, 9),STAOO(JTEC,JSTA,10)
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=17,18)
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=19,20)
              WRITE (LUN,3600)  STAOO(JTEC,JSTA, 1),STAOO(JTEC,JSTA, 4),  &
                                STAOO(JTEC,JSTA, 5)
3600          FORMAT(8F8.2)
            ELSE
              WRITE (LUN,3600) (STATT(JTEC,JSTA,IL),IL=10,12)
            ENDIF
            INBID3 = NINT( STAOO(JTEC,JSTA,14) )
            INBID4 = NINT( STAOO(JTEC,JSTA,15) )
            INBID1 = NINT( STAOO(JTEC,JSTA, 2) )
            INBID2 = NINT( STAOO(JTEC,JSTA, 3) )
            WRITE (LUN,3700) INBID3,INBID4
3700        FORMAT(8I8)
            WRITE (LUN,3600) (STAOO(JTEC,JSTA,IL),IL=11,13)
            WRITE (LUN,3700) INBID1,INBID2
          ENDIF
        ELSEIF( CARTEC(JTEC).EQ.'TGC' ) THEN
!-----------------------------> *****
          IF( IFORMA(JTEC,JSTA) == 1 ) THEN
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,8)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,8)
          ELSEIF( IFORMA(JTEC,JSTA) >= 2 ) THEN
            DO IL=1,NL2
             WRITE (LUN,3800) IL,ISTAMA(JTEC,JSTA,IL),                &
                              STATT(JTEC,JSTA,IL),STAOO(JTEC,JSTA,IL)
3800         FORMAT(I4,I3,2F8.3)
            ENDDO
            IF( IFORMA(JTEC,JSTA) >= 4 ) THEN
              WRITE (LUN,3333)
3333          FORMAT('--------------------------------------------!')
              WRITE (LUN,3400) NtgcAdd(JSTA,1:3)
              WRITE (LUN,3400) NtgcAdd(JSTA,4:6)
              WRITE (LUN,3400) NtgcAdd(JSTA,7:9)
              WRITE (LUN,3400) XtgcAdd(JSTA,1:3)
              WRITE (LUN,3400) XtgcAdd(JSTA,4:5)
              IF( IFORMA(JTEC,JSTA) >= 5 ) THEN
                 WRITE (LUN,3603) XtgcAdd(JSTA, 6: 7)
                 WRITE (LUN,3603) XtgcAdd(JSTA, 8:12)
                 WRITE (LUN,3603) XtgcAdd(JSTA,13:16)
3603             FORMAT(8F8.3)
              ENDIF
              DO I=1,3
               IF( NtgcAdd(JSTA,I) >= 1 ) THEN
                 WRITE (LUN,3333)
                 WRITE (LUN,3900) (NtgcGang(JSTA,IL,I),IL=1,NtgcAdd(JSTA,I))
3900             FORMAT(10I4)
               ENDIF
              ENDDO
              IF( IFORMA(JTEC,JSTA) >= 6 ) THEN
                 NBstrp = MAX( NtgcAdd(JSTA,4), NtgcAdd(JSTA,5), NtgcAdd(JSTA,6) )
                 DO I=1,2
                  WRITE (LUN,3333)
                  WRITE (LUN,3950) XtgcStrp(JSTA,1:NBstrp+1,I)
3950              FORMAT(5F9.2)
                 ENDDO
              ENDIF
            ENDIF
          ENDIF
        ELSEIF( CARTEC(JTEC).EQ.'CSC' ) THEN
!-----------------------------> *****
          IF( IFORMA(JTEC,JSTA).EQ.1 ) THEN
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,8)
            WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,8)
          ELSEIF( IFORMA(JTEC,JSTA).EQ.2 .OR. IFORMA(JTEC,JSTA).EQ.3 ) THEN
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 1)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 2)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 3)
            WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=4,5)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 6),STAOO(JTEC,JSTA,4)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 7)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 8)
            WRITE (LUN,3400)  STATT(JTEC,JSTA, 9)
            IF( IFORMA(JTEC,JSTA).EQ.3) THEN
              WRITE (LUN,3400)  STATT(JTEC,JSTA,10)
              WRITE (LUN,3400)  STATT(JTEC,JSTA,11)
              WRITE (LUN,3400)  STATT(JTEC,JSTA,12)
              WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=13,14)
              WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=15,16)
              WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=17,18)
              WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL= 1, 3)
            ENDIF
          ENDIF
        ELSE
!----> ******
          WRITE (LUN,3400) (STATT(JTEC,JSTA,IL),IL=1,8)
          WRITE (LUN,3400) (STAOO(JTEC,JSTA,IL),IL=1,8)
        ENDIF
320   CONTINUE
330   CONTINUE
!---------------------------------------------
!
!
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
      WRITE (LUN,1000)
!
!
!---------------------------------------------
      DO I=1,NBadjust
        Icode = JTYPFZOLadjust(I)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYPI,JFFI,JZZI,JOBI,JLAYI)
        WRITE(LUN,4100) CARTYP(JTYPI),JFFI,JZZI,JOBI,SZTtraadjust(1:3,I),SZTrotadjust(1:3,I)
4100    FORMAT('A ',A3,3I3,3F10.3,3F10.6)
      ENDDO
!---------------------------------------------
!
      WRITE (LUN,1000)
      WRITE (LUN,1001)
      WRITE (LUN,1000)
!
!---------------------------------------------
      DO I=1,NBdeform
        Icode = JTYPFZOLdeform(I)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYPI,JFFI,JZZI,JOBI,JLAYI)
        WRITE(LUN,4200) CARTYP(JTYPI),JFFI,JZZI,JOBI,SZTdeform(1:11,I)
4200    FORMAT('B ',A3,3I3,11F7.3)
      ENDDO
!---------------------------------------------
!
!
!---------------------------------------------
      IF( LenAGDD.GT.0 ) THEN
         WRITE (LUN,1000)
         WRITE (LUN,1000)
         WRITE (LUN,1001)
         WRITE (LUN,4300)
4300     FORMAT('X --------------------------------------------')
         CALL WRITE_AGDD(LUN)
         WRITE (LUN,4300)
         WRITE (LUN,1001)
         WRITE (LUN,1000)
         WRITE (LUN,1000)
      ENDIF
!---------------------------------------------
!
!
      WRITE (LUN,1000)
      WRITE (LUN,1000)
      WRITE (LUN,5100)
5100  FORMAT('End')
      WRITE (LUN,1000)
!
!
      END SUBROUTINE WRITE_AMDC_FROMCOM
! 
