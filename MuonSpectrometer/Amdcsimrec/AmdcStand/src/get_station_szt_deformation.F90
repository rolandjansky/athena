!
!> Get the deformation parameters for the element Jtyp,Jff,Jzz,Job
!> GET_STATION_SZT_DEFORMATION outputs are in mm
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE GET_STATION_SZT_DEFORMATION( Jtyp,Jff,Jzz,Job, Iok, DeformParam,DeformSS,DeformZZ ,DeformTT,DeformBA)
!
      IMPLICIT NONE
      INTEGER, INTENT(IN)  :: Jtyp,Jff,Jzz,Job
      INTEGER, INTENT(OUT) :: Iok
      REAL(8), INTENT(OUT) :: DeformParam(11) 
      REAL(8), INTENT(OUT) :: DeformSS(4),DeformZZ(4),DeformTT(4)
      INTEGER, INTENT(OUT) :: DeformBA
      INTEGER :: Jgeo, Jtec, Ideb, Ifin, IprintLoc
      INTEGER :: FindObject
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      IprintLoc = 0
!
      call SetDeformationsParameters
!
      Iok = 0
      DeformParam(1:11) = 0.D0
      DeformSS   (1:4)  = 0.D0
      DeformZZ   (1:4)  = 0.D0
      DeformTT   (1:4)  = 0.D0
!
      IF( Jtyp < 1 )              RETURN
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      IF( Jgeo < 1 )              RETURN
      Jtec = ITEC(Jtyp,Jgeo,Job)
      IF( CARTEC(Jtec) /= 'MDT' ) RETURN
!
      Ifin = LastDeformByType(Jtyp)
      IF( Ifin <= 0 )             RETURN
      Ideb = FirstDeformByType(Jtyp)
      Iok = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLdeform)
      IF( Iok <= 0 ) RETURN
!     
      if (IprintLoc.ge.1) then
        PRINT 1000,CARTYP(JTYP), JFF,JZZ,JOB  
1000    FORMAT('GSD*                   ',A3,3(' ',I3))
      endif
!
      DeformParam(1:11) = SZTdeform(1:11,Iok)
      DeformSS(1:4)     = SZTDeformSS(1:4,Iok)
      DeformZZ(1:4)     = SZTDeformZZ(1:4,Iok)
      DeformTT(1:4)     = SZTDeformTT(1:4,Iok)
!
      DeformBA = 0
      IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) DeformBA = 1
!
      if (IprintLoc.ge.1) then
         PRINT 1110,'DeformSS',DeformSS(1:4) 
         PRINT 1110,'DeformZZ',DeformZZ(1:4) 
         PRINT 1110,'DeformTT',DeformTT(1:4) 
1110     FORMAT('GSD  ',A8,'         ',4(' ',F12.3))
      endif
!
      END SUBROUTINE GET_STATION_SZT_DEFORMATION
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE SetDeformationsParameters
!
      IMPLICIT NONE
!
      INTEGER :: Jtyp,Jff,Jzz,Jgeo,Job,jtec
      INTEGER :: Iok,IsStationWithMDT
      REAL(8) :: DeformSS(4),DeformZZ(4),DeformTT(4)
      INTEGER :: IzzDone, Ideb, Ifin
      REAL(8) :: Zii_c,Zaa_c,Zii,Zaa
      REAL(8) :: Siin_c,Siin,Siip_c,Siip
      REAL(8) :: Saan_c,Saan,Saap_c,Saap
      REAL(8) :: sis0,siz0
!
      INTEGER :: FindObject
      REAL(8) :: Tpoints(4)
      INTEGER :: Npts,Imin,Imax,Imin2,Imax2
      REAL(8) :: Xn(10),Xp(10),Yk(10) 
      REAL(8) :: Vtemp,VkeN,VkeP 
      REAL(8) :: X0,De 
      REAL(8) :: CoFi,SiFi
      INTEGER :: IprintLoc
      REAL(8) :: Xorigin_LA
      REAL(8) :: Yorigin_LA
      REAL(8) :: Zorigin_LA
      REAL(8) :: Sorigin_LA
      REAL(8) :: Torigin_LA
      REAL(8) :: Xorigin_LB
      REAL(8) :: Yorigin_LB
      REAL(8) :: Zorigin_LB
      REAL(8) :: Sorigin_LB
      REAL(8) :: Torigin_LB
      REAL(8) :: DZorigin
      REAL(8) :: DSorigin
      REAL(8) :: DTorigin
      REAL(8) :: DsZorigin
      REAL(8) :: DsSorigin
      REAL(8) :: DsTorigin
      REAL(8) :: chfb_width_min  
      REAL(8) :: chfb_width_max  
      REAL(8) :: chfb_length 
      REAL(8) :: chfb_height 
      REAL(8) :: ActualWl,Dstaca
!
      REAL(8) :: Zii_c_cor 
      REAL(8) :: Zaa_c_cor 
      Character(32) :: TheHARDNAME
      Character(2) :: TheHardSectorCar
      INTEGER      :: TheIentryHardSoft,I1,I2,i3
      INTEGER      :: JBARREL,ITURNJZZ,ITURNJFF,ITURNJFFmax
      INTEGER      :: TheHardSector,TheHARDENTRIES
!
      INTEGER :: ipt,JSTA,JZZP,NtubeZ,NTUTU
      REAL(8) :: RTUTU,Y0,YL,SIGOO,OFFSETOO
!
      INTEGER :: AdjustToTubeInZ
      INTEGER :: AdjustToTubeInS
      INTEGER :: IncludeFoam
!
      INTEGER :: IAmdcVar
!
!--------------------------------------------------------------------
! Set the deformation parameters for the element Jtyp,Jff,Jzz,Job
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      if (DeformSSZZTTDone.ne.0) return
!     
      IprintLoc = 0
!
      AdjustToTubeInS = 1
      AdjustToTubeInZ = 1
      IncludeFoam     = 1
!
      TheHARDNAME = 'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'
      i1 = 0
      i2 = 0
      i3 = 0
      TheHARDENTRIES = IAmdcVar( 'HARDENTRIES', i1,i2,i3 )
      ITURNJFFmax = 2
      if (TheHARDENTRIES.eq.0) ITURNJFFmax =  1
!     
      if (IprintLoc.eq.1) then
        PRINT 4000,AdjustToTubeInS,AdjustToTubeInZ,IncludeFoam
4000    FORMAT(&
        'SDP|',&
        ' AdjustToTubeInS = ',I3 &
        '   AdjustToTubeInZ = ',I3 &
        '   IncludeFoam = ',I3 &
        )
        PRINT 3000 
3000    FORMAT(&
        'SDP|',&
        ' Name              ||',&
        ' X, Y, Z A line                  ||',&
        ' X, Y, Z B line                  ||',&
        ' s, z, t B line                  ||',&
        ' WidthS, widthL, length, height B lines     ||' &
        )
      endif
!
      DO JBARREL=1,2
      DO JFF=1,8
      DO ITURNJFF=1,ITURNJFFmax
      DO JTYP=1,MTYP
      DO JZZP=0,MZZ
      DO ITURNJZZ=1,2
      JZZ=JZZP
      if (ITURNJZZ.eq.2) JZZ=-JZZP
!
        if (ITURNJZZ.eq.2.AND.JZZ.eq.0) cycle
!
        JGEO = IGEO(JTYP,JFF,JZZ)
        IF(JGEO.LE.0) CYCLE
!
        IsStationWithMDT = 0
        DO JOB=1,NOBJ(JTYP,JGEO)
          Jtec = ITEC(Jtyp,Jgeo,Job)
          IF( CARTEC(Jtec) /= 'MDT' ) CYCLE
          IsStationWithMDT = 1 
          EXIT
        ENDDO
!
        If ( IsStationWithMDT.eq.0 ) cycle
! 
        if (JBARREL.eq.1.and.IBAREL(Jtyp,Jff,Jzz).le.0) cycle  
        if (JBARREL.eq.2.and.IBAREL(Jtyp,Jff,Jzz).gt.0) cycle  
!     
        if (TheHARDENTRIES.ne.0) then
          TheIentryHardSoft = IAmdcVar( 'IentryHardSoft', JTYP,JFF,JZZ )
          call CAmdcVar('HARDNAME', TheIentryHardSoft,I2,I3, TheHARDNAME )
          TheHardSectorCar=TheHARDNAME(6:7)
          if (TheHardSectorCar.eq.'01') TheHardSector =  1
          if (TheHardSectorCar.eq.'02') TheHardSector =  2
          if (TheHardSectorCar.eq.'03') TheHardSector =  3
          if (TheHardSectorCar.eq.'04') TheHardSector =  4
          if (TheHardSectorCar.eq.'05') TheHardSector =  5
          if (TheHardSectorCar.eq.'06') TheHardSector =  6
          if (TheHardSectorCar.eq.'07') TheHardSector =  7
          if (TheHardSectorCar.eq.'08') TheHardSector =  8
          if (TheHardSectorCar.eq.'09') TheHardSector =  9
          if (TheHardSectorCar.eq.'10') TheHardSector = 10
          if (TheHardSectorCar.eq.'11') TheHardSector = 11
          if (TheHardSectorCar.eq.'12') TheHardSector = 12
          if (TheHardSectorCar.eq.'13') TheHardSector = 13
          if (TheHardSectorCar.eq.'14') TheHardSector = 14
          if (TheHardSectorCar.eq.'15') TheHardSector = 15
          if (TheHardSectorCar.eq.'16') TheHardSector = 16
          if ( TheHardSector.ne.(2*(JFF-1)+ITURNJFF)) cycle
        endif
!
        if (IprintLoc.ge.2) then
          PRINT 1000,&
           CARTYP(JTYP), JFF,JZZ  
1000      FORMAT('SDP*                  ',&
           ' ',A3, &
           2(' ',I3))
        endif
!
        DeformSS(1:4) = 0.d0
        DeformZZ(1:4) = 0.d0
        DeformTT(1:4) = 0.d0
!
        IzzDone = 0
        Zii = 0.d0
        Zaa = 0.d0
!
        Npts = 0 
        DO JOB=1,NOBJ(JTYP,JGEO)
          Jtec = ITEC(Jtyp,Jgeo,Job)
          IF( CARTEC(Jtec) /= 'MDT' ) CYCLE
          JSTA = ISTA(JTYP,JGEO,JOB)
!         Number of tube layers
          NTUTU = NLAZ(JTEC,JSTA)
!         Tube radius
          RTUTU = STARR(JTEC,JSTA)  
          RTUTU = RTUTU + staoo(jtec,jsta,NTUTU+1) 
!
!         print *,'*Geo_dy = ',Geo_dy(Jtyp,Jgeo,Job)
!
          ActualWl = Geo_Wl(Jtyp,Jgeo,Job)
          if (AdjustToTubeInS.ne.0) then
            Dstaca = Geo_D3(Jtyp,Jgeo,Job)
            IF( Dstaca > 1.D0 ) THEN
              ActualWl = Geo_Wl(Jtyp,Jgeo,Job)                    &
              + ( Geo_Wl(Jtyp,Jgeo,Job) - Geo_Ws(Jtyp,Jgeo,Job) ) &
              * Dstaca /(Geo_Le(Jtyp,Jgeo,Job) - Dstaca )
              if (AdjustToTubeInZ.ne.0) then 
                ActualWl = ActualWl                                 &
                + ( Geo_Wl(Jtyp,Jgeo,Job) - Geo_Ws(Jtyp,Jgeo,Job) ) &
                * STAPP(jtec,jsta)*0.5d0 /(Geo_Le(Jtyp,Jgeo,Job) - Dstaca )
              endif 
            endif
          endif
!
          IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
            Zii_c  = Pos_Z(Jtyp,Jff,Jzz)                 &
                   + Geo_dy(Jtyp,Jgeo,Job)
            Zaa_c  = Zii_c + Geo_Le(Jtyp,Jgeo,Job) 
            Siin_c = Pos_S(Jtyp,Jff,Jzz)                 &
                   + Geo_dx(Jtyp,Jgeo,Job)               &
                   + Geo_Ex(Jtyp,Jgeo,Job)/2.D0          &
                   - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
            Saan_c = Pos_S(Jtyp,Jff,Jzz)                 &
                   + Geo_dx(Jtyp,Jgeo,Job)               &
                   - Geo_Ex(Jtyp,Jgeo,Job)/2.D0          &
                   - ActualWl/2.D0
            Siip_c = Siin_c + Geo_Ws(Jtyp,Jgeo,Job)
            Saap_c = Saan_c + ActualWl
          ELSE
            Zii_c  = Pos_R(Jtyp,Jff,Jzz)                 &
                   + Geo_dy(Jtyp,Jgeo,Job)
            Zaa_c  = Zii_c + Geo_Le(Jtyp,Jgeo,Job)
            sis0 = 1.D0
            IF( Jzz > 0 ) sis0 = -1.D0
            Siin_c = Pos_S(Jtyp,Jff,Jzz)                 &
                   - ( Geo_dx(Jtyp,Jgeo,Job)             &
                   + Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0 &
                   - Geo_Ws(Jtyp,Jgeo,Job)/2.D0
            Saan_c = Pos_S(Jtyp,Jff,Jzz)                 &
                   - ( Geo_dx(Jtyp,Jgeo,Job)             &
                   - Geo_Ex(Jtyp,Jgeo,Job)/2.D0 ) * sis0 &
                   - ActualWl/2.D0
            Siip_c = Siin_c + Geo_Ws(Jtyp,Jgeo,Job)
            Saap_c = Saan_c + ActualWl
          ENDIF
!
          if (AdjustToTubeInZ.ne.0) then
            SIGOO = 1.
            OFFSETOO = 0.
            IF ((IBAREL(Jtyp,Jff,Jzz) > 0) .AND. (Jzz<0)) THEN
              SIGOO = -1.
              OFFSETOO = STAPP(jtec,jsta)
            ENDIF

            Zii_c_cor =  SIGOO*staoo(jtec,jsta,    1) 
            if ( Zii_c_cor.ge.(SIGOO*staoo(jtec,jsta,2)) )   &
            Zii_c_cor = SIGOO*staoo(jtec,jsta,2)
            Zii_c_cor = Zii_c_cor - RTUTU + OFFSETOO
            Zii_c = Zii_c         + Zii_c_cor
!
            NtubeZ = nint(Geo_Le(Jtyp,Jgeo,Job)/STAPP(jtec,jsta))
            Zaa_c_cor =  SIGOO*staoo(jtec,jsta,1) 
            if ( Zaa_c_cor.le.(SIGOO*staoo(jtec,jsta,2)) )       &
            Zaa_c_cor = SIGOO*staoo(jtec,jsta,2)
            Zaa_c_cor = Zaa_c_cor + RTUTU + OFFSETOO 
            Zaa_c_cor = Zaa_c_cor + (NtubeZ-1.d0)*STAPP(jtec,jsta)
            Zaa_c_cor = Zaa_c_cor - Geo_Le(Jtyp,Jgeo,Job)
            Zaa_c     = Zaa_c     + Zaa_c_cor
          endif
!
          if (IprintLoc.ge.3) then
            PRINT 1010,&
             Zaa_c , Saan_c, Saap_c  
1010        FORMAT('SDP  za Sn Sp         ',&
             3(' ',F12.3))  
            PRINT 1020,&
             Zii_c , Siin_c, Siip_c  
1020        FORMAT('SDP  zi Sn Sp         ',&
             3(' ',F12.3))  
          endif
!         

          if ( IzzDone .eq. 0) then
            IzzDone = 1
            Zii = Zii_c
            Zaa = Zaa_c
          else
            if (Zii_c .le. Zii) Zii = Zii_c
            if (Zaa_c .ge. Zaa) Zaa = Zaa_c
          endif

          if (Npts.le.8) then
            Xn(Npts+1) = Saan_c
            Xn(Npts+2) = Siin_c
            Xp(Npts+1) = Saap_c
            Xp(Npts+2) = Siip_c
            Yk(Npts+1) = Zaa_c
            Yk(Npts+2) = Zii_c
            Npts = Npts + 2
          endif
         
        ENDDO
!
        DeformZZ(1) = (Zaa+Zii) / 2.D0
        DeformZZ(2) = (Zaa-Zii) / 2.D0
        Y0 = DeformZZ(1)
        YL = DeformZZ(2)
!
        if (IprintLoc.ge.3) then
          PRINT 1030,&
           Y0 , YL  
1030      FORMAT('SDP  Y0 YL            ',&
           2(' ',F12.3))  
        endif
!
        Imin = 0
        Imax = 0
        VkeN = 0.d0
        VkeP = 0.d0
        do ipt=1,Npts
          if (Imin.eq.0 .or. VkeN.ge.Xn(ipt)) then
            Imin = ipt
            VkeN = Xn(ipt)
          endif
          if (Imax.eq.0 .or. VkeP.le.Xp(ipt)) then
            Imax = ipt
            VkeP = Xp(ipt)
          endif
        enddo
!
        if (IprintLoc.ge.3) then
          PRINT 1040,&
           Xn(Imin) , Xp(Imax) 
1040      FORMAT('SDP  xnmin xpmax      ',&
           2(' ',F12.3))  
          PRINT 1050,&
           Yk(Imin) , Yk(Imax) 
1050      FORMAT('SDP  ynmin ypmax      ',&
           2(' ',F12.3))  
        endif
!
        Imax2 = 0
        Imin2 = 0
        VkeP  = 0.d0
        VkeN  = 0.d0
        do ipt=1,Npts
!
          if (IprintLoc.ge.3) then
            PRINT 1060,&
             Xn(ipt) , Yk(ipt) 
1060        FORMAT('SDP  Vtemp            ',&
             2(' ',F12.3))  
          endif
!
          if (abs(Y0-Yk(Imin)).ge.0.010) then
          if (ipt.ne.Imin) then
          if (abs(Yk(ipt)-Yk(Imin)).ge.0.010) then
          if ( (Yk(Imin).ge.Y0.and.Yk(Imin).ge.Yk(ipt))  &
          .or. (Yk(Imin).lt.Y0.and.Yk(Imin).lt.Yk(ipt))  &
          )then
!
            if (IprintLoc.ge.3) then
              PRINT 1070,&
               Xn(ipt) , Yk(ipt) 
1070          FORMAT('SDP  Vtemp min        ',&
               2(' ',F12.3))  
            endif
!
            Vtemp =           Xn(ipt ) - Xn(Imin) 
            Vtemp = Vtemp / ( Yk(ipt ) - Yk(Imin) )
            Vtemp = Vtemp * ( Y0      -  Yk(Imin) ) 
            Vtemp = abs(Vtemp)
            if (Imin2.eq.0.or.VkeN.ge.Vtemp) then
              Imin2 = ipt
              VkeN  = Vtemp
            endif
          endif
          endif
          endif
          endif
!          
          if (abs(Y0-Yk(Imax)).ge.0.010) then
          if (ipt.ne.Imax) then
          if (abs(Yk(ipt)-Yk(Imax)).ge.0.010) then
          if ( (Yk(Imax).ge.Y0.and.Yk(Imax).ge.Yk(ipt))  &
          .or. (Yk(Imax).lt.Y0.and.Yk(Imax).lt.Yk(ipt))  &
          )then
!
            if (IprintLoc.ge.3) then
              PRINT 1080,&
               Xn(ipt) , Yk(ipt) 
1080          FORMAT('SDP  Vtemp max        ',&
               2(' ',F12.3))  
            endif
!
            Vtemp =           Xp(ipt ) - Xp(Imax) 
            Vtemp = Vtemp / ( Yk(ipt ) - Yk(Imax) )
            Vtemp = Vtemp * ( Y0      -  Yk(Imax) ) 
            Vtemp = abs(Vtemp)
            if (Imax2.eq.0.or.VkeP.ge.Vtemp) then
              Imax2 = ipt
              VkeP  = Vtemp
            endif
          endif
          endif
          endif
          endif
        enddo             
!
        X0 = 0.d0
        De = 0.d0
        if (Imin2.ne.0) then
          Vtemp =           Xn(Imin2) - Xn(Imin)     
          Vtemp = Vtemp / ( Yk(Imin2) - Yk(Imin) ) 
          X0    = Vtemp * ( Y0        - Yk(Imin) ) 
          De    = Vtemp * YL
        endif
        X0 = Xn(Imin) + X0
        Saan = X0 + De
        Siin = X0 - De
!
        X0 = 0.d0
        De = 0.d0
        if (Imax2.ne.0) then
          Vtemp =           Xp(Imax2) - Xp(Imax)     
          Vtemp = Vtemp / ( Yk(Imax2) - Yk(Imax) ) 
          X0    = Vtemp * ( Y0        - Yk(Imax) ) 
          De    = Vtemp * YL
        endif
        X0 = Xp(Imax) + X0
        Saap = X0 + De
        Siip = X0 - De
!
        if (IprintLoc.ge.3) then
          PRINT 1090,&
           Zaa , Saan, Saap  
1090      FORMAT('SDP  za Sn Sp final   ',&
           3(' ',F12.3))  
          PRINT 1100,&
           Zii , Siin, Siip  
1100      FORMAT('SDP  zi Sn Sp final   ',&
           3(' ',F12.3))  
        endif
!
        IF(  IBAREL(Jtyp,Jff,Jzz) > 0                    &
        .OR.(IBAREL(Jtyp,Jff,Jzz) .eq. 0 .and. JZZ < 0 ) &
        ) THEN
          DeformSS(1)   =   (Saan+Siin) / 2.D0
          DeformSS(2)   =   (Saan-Siin) / 2.D0
          DeformSS(3)   =   (Saap+Siip-Saan-Siin) / 2.D0
          DeformSS(4)   =   (Saap-Siip-Saan+Siin) / 2.D0
        ELSE
          DeformSS(1)   =   (Saap+Siip) / 2.D0
          DeformSS(2)   =   (Saap-Siip) / 2.D0
          DeformSS(3)   = - (Saap+Siip-Saan-Siin) / 2.D0
          DeformSS(4)   = - (Saap-Siip-Saan+Siin) / 2.D0
        ENDIF
!
        CALL dim_t_mdtBase(Jtyp,Jff,Jzz,Tpoints,IncludeFoam)
        DeformTT(1) = Tpoints(1) 
        DeformTT(2) = abs(Tpoints(4)-Tpoints(1))
!
        if (IprintLoc.ge.2) then
!
          PRINT 1110,&
           DeformSS(1:4) 
1110      FORMAT('SDP  DeformSS         ',&
           4(' ',F12.3))  
          PRINT 1112,&
           DeformZZ(1:4) 
1112      FORMAT('SDP  DeformZZ         ',&
           4(' ',F12.3))  
          PRINT 1114,&
           DeformTT(1:4) 
1114      FORMAT('SDP  DeformTT         ',&
           4(' ',F12.3))  
!
        endif
!
        if (IprintLoc.eq.1) then
!
          CoFi = COS( Pos_Phi(Jtyp,Jff,Jzz) )
          SiFi = SIN( Pos_Phi(Jtyp,Jff,Jzz) )
          Sorigin_LA = Pos_S(Jtyp,Jff,Jzz)
          Torigin_LA = Pos_R(Jtyp,Jff,Jzz)
          Zorigin_LA = Pos_Z(Jtyp,Jff,Jzz)
          Xorigin_LA = CoFi*Torigin_LA-SiFi*Sorigin_LA
          Yorigin_LA = SiFi*Torigin_LA+CoFi*Sorigin_LA
!
          IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
            Zorigin_LB = DeformZZ(1)-DeformZZ(2)
            Torigin_LB = DeformTT(1) 
            Sorigin_LB = DeformSS(1)-DeformSS(2)
            Sorigin_LB = Sorigin_LB + (DeformSS(3)-DeformSS(4))/2.d0
          else
            Zorigin_LB = DeformTT(1)
            Torigin_LB = DeformZZ(1)-DeformZZ(2)
          endif
          Sorigin_LB = DeformSS(1)-DeformSS(2)
          Sorigin_LB = Sorigin_LB + (DeformSS(3)-DeformSS(4))/2.d0
          Xorigin_LB = CoFi*Torigin_LB-SiFi*Sorigin_LB
          Yorigin_LB = SiFi*Torigin_LB+CoFi*Sorigin_LB
!
          DZorigin = Zorigin_LB - Zorigin_LA
          DSorigin = Sorigin_LB - Sorigin_LA
          DTorigin = Torigin_LB - Torigin_LA
          IF( IBAREL(Jtyp,Jff,Jzz) > 0 ) THEN
            DsZorigin = DZorigin
            DsSorigin = DSorigin
            DsTorigin = DTorigin
          else
            siz0 = 1.D0
            IF( Jzz < 0 ) siz0 = -1.D0
            DsZorigin = DTorigin
            DsSorigin =-siz0*DSorigin
            DsTorigin = siz0*DZorigin
          endif
!
          chfb_width_min  = abs( DeformSS(3)-DeformSS(4) )                               
          chfb_width_max  = abs( DeformSS(3)+DeformSS(4) )                                
          chfb_length = 2.d0 * DeformZZ(2)
          chfb_height = DeformTT(2)
!
          if (TheHARDENTRIES.ne.0) then
            TheIentryHardSoft = IAmdcVar( 'IentryHardSoft', JTYP,JFF,JZZ )
            call CAmdcVar('HARDNAME', TheIentryHardSoft,I2,I3, TheHARDNAME )
          endif
!
          PRINT 2000,&
           TheHARDNAME(1:7)  ,&
           CARTYP(JTYP), JFF,JZZ  ,&
           Xorigin_LA , &
           Yorigin_LA , &
           Zorigin_LA , &
           Xorigin_LB , &
           Yorigin_LB , &
           Zorigin_LB , &
           DsSorigin  , &
           DsZorigin  , &
           DsTorigin  , &
           chfb_width_min  , &
           chfb_width_max  , &
           chfb_length , &
           chfb_height  
2000      FORMAT(&
          'SDP|',&
           A7, &
           ' ',A3, &
           2(' ',I3),'||', &
           3(' ',F10.3),'||',&
           3(' ',F10.3),'||',&
           3(' ',F10.3),'||',&
           4(' ',F10.3),'||' &
          )            
!
        endif
!
        Ifin = LastDeformByType(Jtyp)
        IF( Ifin <= 0 ) CYCLE
        Job  = 0
        Ideb = FirstDeformByType(Jtyp)
        Iok  = FindObject(Jtyp,Jff,Jzz,Job, Ideb,Ifin, JTYPFZOLdeform)
        IF( Iok <= 0 )  CYCLE
        SZTDeformSS(1:4,Iok) = DeformSS(1:4)
        SZTDeformZZ(1:4,Iok) = DeformZZ(1:4)
        SZTDeformTT(1:4,Iok) = DeformTT(1:4)
!
      ENDDO
      ENDDO
      ENDDO
      ENDDO
      ENDDO
      ENDDO
!
      DeformSSZZTTDone = 1
!
      END SUBROUTINE SetDeformationsParameters
