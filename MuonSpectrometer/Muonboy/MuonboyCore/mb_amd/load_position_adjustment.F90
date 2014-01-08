!
!> Load the A and B lines. In this routine, NBtransGeo is the number of
!> stations with alignment corrections.
!
 SUBROUTINE load_position_adjustment(ier)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_TransGeo
 IMPLICIT NONE
 INTEGER, INTENT(OUT) :: ier  !< error flag, should be 0 if everything was ok
 INTEGER :: JTYP, JFF, JZZ, JGEO, JOB, JTEC, ISC, Iok1, Iok2
 INTEGER :: IGEOMT, indtec, IC, INOF, INOZ, ICT
 INTEGER :: I, Jcsc, Jlay
 REAL(8) :: TransDirLoc(3,4),TransBakLoc(3,4) 
 REAL(8) :: TransDeformLoc(11)
 REAL(8) :: TransDeformSS(4),TransDeformZZ(4),TransDeformTT(4)
 REAL(8) :: sigt
 INTEGER :: TransDeformBA
 INTEGER :: intpos
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!-- Stations are, a priori, in their nominal positions
!-- (i.e. no "transformations" to their true positions)
      NBtransGeo = 0
      ItransCHA(:) = 0
      ItransTRI(:) = 0
      lastTrans    = -1
      IF( Ignore_Misali /= 0 )  RETURN

!-- Loops over all the stations of the data base
      DO 503 JTYP=1,MTYP
        ISC  = ISCISC(JTYP)
        IF( ISC  < 1 ) CYCLE
        DO 502 JFF=1,8
          DO 501 JZZ=-MZZ,MZZ
            JGEO = IGEO_V( JTYP_F_Z(JTYP,JFF,JZZ) )
            IF( JGEO < 1 ) CYCLE
            DO 500 JOB=1,NOBJ(JTYP,JGEO)
              JTEC = ITEC_V( JTYP_G_O(JTYP,JGEO,JOB) )
              SELECT CASE( CARTEC(JTEC) )
                CASE( 'MDT' ) ; indtec = 1
                CASE( 'CSC' ) ; indtec = 2
                CASE( 'RPC' ) ; indtec = 3
                CASE( 'TGC' ) ; indtec = 4
                CASE DEFAULT  ; CYCLE
              END SELECT
!
!-- Get the station position adjustement (in the S,Z,T local frame)
!-- according to  Alfa,Beta,Gama  and  corrections from the alignement
!-- system
              CALL GET_STATION_CSC_ABG_SZT_TRANSFORM( JTYP,JFF,JZZ,JOB,Iok1,TransDirLoc,TransBakLoc )
              CALL GET_STATION_SZT_DEFORMATION( JTYP,JFF,JZZ,JOB,Iok2,                      &
                  TransDeformLoc,TransDeformSS,TransDeformZZ ,TransDeformTT,TransDeformBA)
!-- NB: GET_STATION_SZT_DEFORMATION outputs are in mm while here cm are used
              TransDeformLoc(1:8) = TransDeformLoc(1:8) / 10.d0
              TransDeformLoc(9:11)= TransDeformLoc(9:11)/1000.d0 
              IF( Iok1+Iok2 <= 0 ) CYCLE


              IF( NBtransGeo >= KtransGeo ) THEN
                PRINT 8881,NBtransGeo
 8881           FORMAT(/' Muonboy:load_position_adjustment,  Parameter KtransGeo =',I7,' is not big enough ')
                ier = 2
                RETURN
              ENDIF
              NBtransGeo = NBtransGeo + 1

              TransGeoDir(1:3,1:3,NBtransGeo) = TransDirLoc(1:3,1:3)
              TransGeoDir(1:3, 4 ,NBtransGeo) = TransDirLoc(1:3, 4 ) / 10.D0
              TransGeoBak(1:3,1:3,NBtransGeo) = TransBakLoc(1:3,1:3)
              TransGeoBak(1:3, 4 ,NBtransGeo) = TransBakLoc(1:3, 4 ) / 10.D0
              TransGeoDeform(1:11,NBtransGeo) = TransDeformLoc(1:11) 
              TransGeoSS(1:4,NBtransGeo) = TransDeformSS(1:4) / 10.D0
              TransGeoZZ(1:4,NBtransGeo) = TransDeformZZ(1:4) / 10.D0
              TransGeoTT(1:4,NBtransGeo) = TransDeformTT(1:4) / 10.D0
              TransGeoBA(    NBtransGeo) = TransDeformBA
              IF( Iok2 > 0 ) THEN
                LDeform(NBtransGeo) = .true.
              ELSE
                LDeform(NBtransGeo) = .false.
              ENDIF
!
              CALL GETIND(JTYP,JFF,JZZ,JOB, IC,INOF,INOZ, ICT)
              IF( indtec <= 2 ) THEN   ! MDT or CSC
                IF( IC*INOF*INOZ <= 0 ) THEN
                  write(*,*) ' Muonboy:load_position_adjustment, '       &
                    ,'FATAL error : jtyp,jff,jzz,job ',jtyp,jff,jzz,job  &
                    ,' is not a valid MDT or CSC'
                  CYCLE
                ENDIF
                IGEOMT = IGEOMA(IC,INOF,INOZ)
                IF( IGEOMT <= 0 ) THEN                
                  write(*,*) ' Muonboy:load_position_adjustment, '       &
                    ,'FATAL error : jtyp,jff,jzz,job ',jtyp,jff,jzz,job  &
                    ,' is not a valid MDT or CSC'
                  CYCLE
                ENDIF
                ItransCHA(IGEOMT) = NBtransGeo
!--!-- JFL: samedi 15 mai 2010, 19:26:15 (UTC+0200)
!--!-- The following means that the S origin for deformation is taken on positive S side
!--!-- Comment: It is not clear why the TransGeoSS are not taken equal to TransDeformSS, idem for TransGeoZZ/TransDeformZZ
!--!-- since apparently same computation is done in GET_STATION_SZT_DEFORMATION
!--!-- I do the same for TransGeoBA/TransDeformBA and TransGeoTT/TransDeformTT
!--!-- NB: GET_STATION_SZT_DEFORMATION outputs are in mm while here cm are used
!--               IF( TransDeformSS(3) > 0.D0 ) THEN
!--                 TransGeoSS(1,NBtransGeo) = (SNACHAV(IGEOMT)+SNICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(2,NBtransGeo) = (SNACHAV(IGEOMT)-SNICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(3,NBtransGeo) = (SPACHAV(IGEOMT)+SPICHAV(IGEOMT)        &
!--                                             -SNACHAV(IGEOMT)-SNICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(4,NBtransGeo) = (SPACHAV(IGEOMT)-SPICHAV(IGEOMT)        &
!--                                             -SNACHAV(IGEOMT)+SNICHAV(IGEOMT))/2.D0
!--                ELSE
!--                  TransGeoSS(1,NBtransGeo) = (SPACHAV(IGEOMT)+SPICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(2,NBtransGeo) = (SPACHAV(IGEOMT)-SPICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(3,NBtransGeo) =-(SPACHAV(IGEOMT)+SPICHAV(IGEOMT)        &
!--                                             -SNACHAV(IGEOMT)-SNICHAV(IGEOMT))/2.D0
!--                  TransGeoSS(4,NBtransGeo) =-(SPACHAV(IGEOMT)-SPICHAV(IGEOMT)        &
!--                                             -SNACHAV(IGEOMT)+SNICHAV(IGEOMT))/2.D0
!--                ENDIF
!--                TransGeoZZ(1,NBtransGeo) = ZCECHAV(IGEOMT)
!--                TransGeoZZ(2,NBtransGeo) = ZLOCHAV(IGEOMT)*0.5d0
!--                TransGeoTT(1,NBtransGeo) = TREFABSO(IGEOMT)
!--                TransGeoTT(2,NBtransGeo) = TREFSIZE(IGEOMT)
!--                IF( IBAREL_V(JTYP_F_Z(Jtyp,Jff,Jzz)) > 0 ) TransGeoBA(NBtransGeo)      = 1
              ELSE                   ! RPC or TGC
                IF( ICT <= 0 ) THEN
                  write(*,*) ' Muonboy:load_position_adjustment, '       &
                    ,'FATAL error : jtyp,jff,jzz,job ',jtyp,jff,jzz,job  &
                    ,' is not a valid RPC or TGC'
                  CYCLE
                ENDIF
                ItransTRI(ICT) = NBtransGeo
              ENDIF
!
 500        ENDDO
 501      ENDDO
 502    ENDDO
 503  ENDDO

!... now decode I lines (internal CSC geometry
      DO Jcsc=1,NChaCSC
        DO Jlay=1,4
          transCSC(1:4,Jlay,Jcsc) = (/ 1.d0, 0.d0, 0.d0, 0.d0 /)
        ENDDO
      ENDDO
      DO I=1,NBdwnlay
        CALL DECO_JTYP_F_Z_O_L(JTYPFZOLdwnlay(I), Jtyp,Jff,Jzz,Job,Jlay)
        CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
        IGEOMT = IGEOMA(IC,INOF,INOZ)
        Jcsc = intpos(IGEOMT,IGEOMCSC,NChaCSC)
        IF( Jcsc <= 0 ) CYCLE
        IF( MBChkCsc > 0 ) THEN
          IF( abs(SZTrotdwnlay(1,I)) > 0.000001d0 ) WRITE(MBChkCsc,*) ' WARNING CSC S rotation    ',SZTrotdwnlay(1,I)
          IF( abs(SZTrotdwnlay(2,I)) > 0.000001d0 ) WRITE(MBChkCsc,*) ' WARNING CSC Z rotation    ',SZTrotdwnlay(2,I)
          IF( abs(SZTtradwnlay(3,I)) > 0.000010d0 ) WRITE(MBChkCsc,*) ' WARNING CSC T translation ',SZTtradwnlay(3,I)
        ENDIF
! SZTrotdwnlay(3) = rotation around T
! SZTtradwnlayCentred(1:2) = center of rotation (in S,Z)
! SZTtradwnlay(1:2) = translation on S and Z
        sigt = SIGN( 1.d0 , TCECHAV(IGEOMT) )
        transCSC(1,Jlay,Jcsc) =        cos(SZTrotdwnlay(3,I))
        transCSC(2,Jlay,Jcsc) = sigt * sin(SZTrotdwnlay(3,I))
        transCSC(3,Jlay,Jcsc) = 0.1d0 * (   (1.d0-transCSC(1,Jlay,Jcsc))*SZTtradwnlayCentred(1,I) &
                                          -       transCSC(2,Jlay,Jcsc) *SZTtradwnlayCentred(2,I) &
                                          -                         sigt*SZTtradwnlay(1,I) )
        transCSC(4,Jlay,Jcsc) = 0.1d0 * (         transCSC(2,Jlay,Jcsc) *SZTtradwnlayCentred(1,I) &
                                          + (1.d0-transCSC(1,Jlay,Jcsc))*SZTtradwnlayCentred(2,I) &
                                          +                              SZTtradwnlay(2,I) )
      ENDDO

!... compare the way these A lines are applied in Amdc and Mboy
      IF( MBChkCsc > 0 ) CALL check_position_adjustment

 END SUBROUTINE load_position_adjustment
!
