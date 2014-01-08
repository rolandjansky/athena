!
!> \author SaMuSoG
!
!--------------------------------------------------------------------
!
!   Read GET_STATION_CSC_ABG_SZT_TRANSFORM header for the maths
!         
!--------------------------------------------------------------------
      SUBROUTINE GET_STATION_ABG_SZT_TRANSFORM(  Jtyp,Jff,Jzz,Job, Iok, TransformDir,TransformBak  )
      IMPLICIT NONE
      INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job
      INTEGER, INTENT(OUT) :: Iok
      REAL(8), INTENT(OUT) :: TransformDir(3,4),TransformBak(3,4)
      REAL(8) :: RotaABG(3,3),RotaSZT(3,3)
      REAL(8) :: Translation(3),Rotation(3,3)
      REAL(8) :: RotS(3,3),RotZ(3,3),RotT(3,3),RotX(3,3)
      REAL(8) :: Alfa(3),SZT(3)
      REAL(8) :: Eps
      INTEGER :: IokABG,IokSZT, Ideb,Ifin
      REAL(8) :: COSA1,SINA1
      REAL(8) :: COSA2,SINA2
      REAL(8) :: COSA3,SINA3
      INTEGER :: FindObject
      INTEGER :: JtypL,JffL,JzzL,JobL,JlayL,jgeo,JTEC
      REAL(8) :: sis0,OriVector(3),OriVectorRot(3)
!      INTEGER :: JobLL
!      INTEGER :: idzmini,idzmaxi
!      REAL(8) :: dzmini,dzmaxi 
!      REAL(8) :: dzthi,dzcurrent
      INTEGER :: DoBOption
!
#include "AmdcStand/copipi.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
!     DATA Eps/ 0.000010D0 /
      DATA Eps/ 0.0000001D0 /
      DATA DoBOption / 1 /
!
      Iok = 0
      TransformDir( 1 ,1:4) = (/ 1.D0, 0.D0, 0.D0, 0.D0 /)
      TransformDir( 2 ,1:4) = (/ 0.D0, 1.D0, 0.D0, 0.D0 /)
      TransformDir( 3 ,1:4) = (/ 0.D0, 0.D0, 1.D0, 0.D0 /)
      TransformBak(1:3,1:4) = TransformDir(1:3,1:4)
!
      Alfa(1) = Pos_Gama(Jtyp,Jff,Jzz) * PIS180
      Alfa(2) = Pos_Beta(Jtyp,Jff,Jzz) * PIS180
      Alfa(3) = Pos_Alfa(Jtyp,Jff,Jzz) * PIS180
      IF( ABS(Alfa(1))+ABS(Alfa(2))+ABS(Alfa(3)) > Eps ) THEN
         IokABG = 1
         IF( IBAREL(Jtyp,Jff,Jzz) < 1 ) THEN
            IF( Jzz > 0 ) THEN
               Alfa(2:3) = - Alfa(2:3)
            ELSE
               Alfa(1:2) = - Alfa(1:2)
            ENDIF
         ENDIF
!         
         COSA1 = COS(Alfa(1))
         SINA1 = SIN(Alfa(1))
         COSA2 = COS(Alfa(2))
         SINA2 = SIN(Alfa(2))
         COSA3 = COS(Alfa(3))
         SINA3 = SIN(Alfa(3))
!
         RotS(1:3,1:3) =   0.D0
         RotZ(1:3,1:3) =   0.D0
         RotT(1:3,1:3) =   0.D0
!
         RotS( 1 , 1 ) =   1.D0
         RotS( 2 , 2 ) =   COSA1
         RotS( 3 , 2 ) =   SINA1
         RotS( 2 , 3 ) = - SINA1
         RotS( 3 , 3 ) =   COSA1
!
         RotZ( 2 , 2 ) =   1.D0
         RotZ( 1 , 1 ) =   COSA2
         RotZ( 3 , 1 ) = - SINA2
         RotZ( 1 , 3 ) =   SINA2
         RotZ( 3 , 3 ) =   COSA2
!
         RotT( 3 , 3 ) =   1.D0
         RotT( 1 , 1 ) =   COSA3
         RotT( 2 , 1 ) =   SINA3
         RotT( 1 , 2 ) = - SINA3
         RotT( 2 , 2 ) =   COSA3
!
         CALL MULT3M3M( RotT,RotZ, RotX )
         CALL MULT3M3M( RotX,RotS, RotaABG )
!
      ELSE
         IokABG = 0
         RotaABG( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
         RotaABG( 2 ,1:3) = (/ 0.D0, 1.D0, 0.D0 /)
         RotaABG( 3 ,1:3) = (/ 0.D0, 0.D0, 1.D0 /)
      ENDIF
!
      Ifin = LastAdjustByType(Jtyp)
      IF( Ifin <= 0 ) THEN
        IokSZT = 0
      ELSE
        Ideb = FirstAdjustByType(Jtyp)
        IokSZT = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLadjust)
      ENDIF
      IF( IokABG+IokSZT < 1 ) RETURN
      Iok = MAX( 1 , IokSZT )
!
      IF( IokSZT > 0 ) THEN
!
         IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
            Translation(1:3) =  SZTtraadjust(1:3,IokSZT)
            Alfa(1:3)        =  SZTrotadjust(1:3,IokSZT)
         ELSE
            IF( Jzz > 0 ) THEN
               Translation( 1 ) = -SZTtraadjust( 1 ,IokSZT)
               Translation(2:3) =  SZTtraadjust(2:3,IokSZT)
               Alfa( 1 )        =  SZTrotadjust( 1 ,IokSZT)
               Alfa(2:3)        = -SZTrotadjust(2:3,IokSZT)
            ELSE
               Translation(1:2) =  SZTtraadjust(1:2,IokSZT)
               Translation( 3 ) = -SZTtraadjust( 3 ,IokSZT)
               Alfa(1:2)        = -SZTrotadjust(1:2,IokSZT)
               Alfa( 3 )        =  SZTrotadjust( 3 ,IokSZT)
            ENDIF
!
            if (DoBOption.eq.1) then
            if (cartyp(jtyp)(1:1).eq.'T') then
              call DECO_JTYP_F_Z_O_L(JTYPFZOLadjust(IokSZT), JtypL,JffL,JzzL,JobL,JlayL)
              if (JobL.ne.0) then
                Jgeo = IGEO(JTYP,JFF,JZZ)
                if (jgeo.gt.0) then
                  JTEC = ITEC(JTYP,JGEO,JobL)
                  IF (CARTEC(JTEC) == 'TGC') THEN
                    sis0 = 1.D0
!                    idzmini = 0
!                    dzmini  = 0.d0
!                    idzmaxi = 0
!                    dzmaxi  = 0.d0
!                    DO JobLL=1,NOBJ(Jtyp,Jgeo)
!                      JTEC = ITEC(JTYP,JGEO,JobLL)
!                      IF (CARTEC(JTEC) == 'TGC') THEN
!                        dzcurrent = Geo_dz(Jtyp,Jgeo,JobLL)  
!                        if (idzmini.eq.0.or.dzmini.ge.dzcurrent) then
!                          idzmini = 1
!                          dzmini  = dzcurrent
!                        endif
!                        if (idzmaxi.eq.0.or.dzmaxi.le.dzcurrent) then
!                          idzmaxi = 1
!                          dzmaxi  = dzcurrent
!                        endif
!                      endif
!                    enddo
!                    dzthi = ( dzmaxi - dzmini ) / 2.d0
!                    IF( Jzz > 0 ) sis0 = -1.D0
!                    OriVector(1) = Pos_S(Jtyp,Jff,Jzz)                  &
!                                 - ( Geo_dx(Jtyp,Jgeo,JobL)             &
!                                 + Geo_Ex(Jtyp,Jgeo,JobL)/2.D0 ) * sis0  
!                    OriVector(2) = Pos_R(Jtyp,Jff,Jzz)                  &
!                                 + Geo_dy(Jtyp,Jgeo,JobL)
!                    OriVector(3) = Pos_Z(Jtyp,Jff,Jzz)                  &
!                                 - Geo_dz(Jtyp,Jgeo,JobL) * sis0
                    IF( Jzz > 0 ) sis0 = -1.D0
                    OriVector(1) =                                      &
                                 - ( Geo_dx(Jtyp,Jgeo,JobL)             &
                                 + Geo_Ex(Jtyp,Jgeo,JobL)/2.D0 ) * sis0  
                    OriVector(2) =                                      &
                                 + Geo_dy(Jtyp,Jgeo,JobL)
                    OriVector(3) =                                      &
                                 - Geo_dz(Jtyp,Jgeo,JobL)  * sis0
!
                    COSA1 = COS(Alfa(1))
                    SINA1 = SIN(Alfa(1))
                    COSA2 = COS(Alfa(2))
                    SINA2 = SIN(Alfa(2))
                    COSA3 = COS(Alfa(3))
                    SINA3 = SIN(Alfa(3))
!                    
                    RotS(1:3,1:3) =   0.D0
                    RotZ(1:3,1:3) =   0.D0
                    RotT(1:3,1:3) =   0.D0
!
                    RotS( 1 , 1 ) =   1.D0
                    RotS( 2 , 2 ) =   COSA1
                    RotS( 3 , 2 ) =   SINA1
                    RotS( 2 , 3 ) = - SINA1
                    RotS( 3 , 3 ) =   COSA1
!                    
                    RotZ( 2 , 2 ) =   1.D0
                    RotZ( 1 , 1 ) =   COSA2
                    RotZ( 3 , 1 ) = - SINA2
                    RotZ( 1 , 3 ) =   SINA2
                    RotZ( 3 , 3 ) =   COSA2
!                    
                    RotT( 3 , 3 ) =   1.D0
                    RotT( 1 , 1 ) =   COSA3
                    RotT( 2 , 1 ) =   SINA3
                    RotT( 1 , 2 ) = - SINA3
                    RotT( 2 , 2 ) =   COSA3
!                    
                    CALL MULT3M3M( RotT,RotZ, RotX )
                    CALL MULT3M3M( RotX,RotS, RotaSZT )
!                    
                    OriVectorRot(1:3) = RotaSZT(1:3,1)*OriVector(1) &
                                      + RotaSZT(1:3,2)*OriVector(2) &
                                      + RotaSZT(1:3,3)*OriVector(3)
                    SZT(1:3) = Translation(1:3)
                    Translation(1:3) = SZT(1:3)          &
                                     + OriVector(:3)     &
                                     - OriVectorRot(1:3)
!
                  endif
                endif
              endif
            endif
            endif
!
         ENDIF
!                          
         SZT(1:3) = Translation(1:3)
         Translation(1:3) = RotaABG(1:3,1)*SZT(1)  &
                          + RotaABG(1:3,2)*SZT(2)  &
                          + RotaABG(1:3,3)*SZT(3)
!                          
         COSA1 = COS(Alfa(1))
         SINA1 = SIN(Alfa(1))
         COSA2 = COS(Alfa(2))
         SINA2 = SIN(Alfa(2))
         COSA3 = COS(Alfa(3))
         SINA3 = SIN(Alfa(3))
!
         RotS(1:3,1:3) =   0.D0
         RotZ(1:3,1:3) =   0.D0
         RotT(1:3,1:3) =   0.D0
!         
         RotS( 1 , 1 ) =   1.D0
         RotS( 2 , 2 ) =   COSA1
         RotS( 3 , 2 ) =   SINA1
         RotS( 2 , 3 ) = - SINA1
         RotS( 3 , 3 ) =   COSA1
!
         RotZ( 2 , 2 ) =   1.D0
         RotZ( 1 , 1 ) =   COSA2
         RotZ( 3 , 1 ) = - SINA2
         RotZ( 1 , 3 ) =   SINA2
         RotZ( 3 , 3 ) =   COSA2
!
         RotT( 3 , 3 ) =   1.D0
         RotT( 1 , 1 ) =   COSA3
         RotT( 2 , 1 ) =   SINA3
         RotT( 1 , 2 ) = - SINA3
         RotT( 2 , 2 ) =   COSA3
!
         CALL MULT3M3M( RotT,RotZ, RotX )
         CALL MULT3M3M( RotX,RotS, RotaSZT )
!
      ELSE
         Translation(1:3)     = 0.D0
         RotaSZT( 1 ,1:3) = (/ 1.D0, 0.D0, 0.D0 /)
         RotaSZT( 2 ,1:3) = (/ 0.D0, 1.D0, 0.D0 /)
         RotaSZT( 3 ,1:3) = (/ 0.D0, 0.D0, 1.D0 /)
      ENDIF
!
      CALL MULT3M3M( RotaSZT,RotaABG, Rotation )
!
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
         SZT(1) = Pos_S(Jtyp,Jff,Jzz)
         SZT(2) = Pos_Z(Jtyp,Jff,Jzz)
         SZT(3) = Pos_R(Jtyp,Jff,Jzz)
      ELSE
         SZT(1) = Pos_S(Jtyp,Jff,Jzz)
         SZT(2) = Pos_R(Jtyp,Jff,Jzz)
         SZT(3) = Pos_Z(Jtyp,Jff,Jzz)
      ENDIF
!
      TransformDir(1:3,1:3) = Rotation(1:3,1:3)
      TransformDir(1:3, 4 ) = SZT(1:3) + Translation(1:3)              &
                            - Rotation(1:3,1)*SZT(1)                   &
                            - Rotation(1:3,2)*SZT(2)                   &
                            - Rotation(1:3,3)*SZT(3)
!
      Rotation = TRANSPOSE(Rotation)                 ! inversion
      TransformBak(1:3,1:3) = Rotation(1:3,1:3)
      TransformBak(1:3, 4 ) = SZT(1:3)                                 &
                            - Rotation(1:3,1)*(SZT(1)+Translation(1))  &
                            - Rotation(1:3,2)*(SZT(2)+Translation(2))  &
                            - Rotation(1:3,3)*(SZT(3)+Translation(3))
!
      END SUBROUTINE GET_STATION_ABG_SZT_TRANSFORM
!
