!
!> This routine cleans the AMDC Commons
!> and performs basic initialisation (e.g. for the calorimeter matter)
!> \author SaMuSoG
!
      SUBROUTINE CLDBCO
      IMPLICIT NONE
!
#include "AmdcStand/comamu.inc"
#include "AmdcStand/comama.inc"
#include "AmdcStand/coforc.inc"
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER :: JVERIF, NBSET, I
      REAL(8) :: XVERIF
      INTEGER, PARAMETER :: NBDATA=27 
      INTEGER :: IIDATA(NBDATA)
      REAL(8) :: X0DATA(NBDATA),EADATA(NBDATA),EBDATA(NBDATA),ZZDATA(NBDATA)
!
      DATA XVERIF/ 777770.7D0 /
      DATA JVERIF/ 7777770 /
!
      DATA IIDATA/     1,     2,     4,     6,          &
                       8,     9,     7,     3,          &
                      14,    18,    11,    17,          &
                      19,    29,    28,    23,          &
                      36,    37,    15,    35,          &
                      99,    41,    43,    48,          &
                      49,    44,    47 /
      DATA X0DATA/  1.76D0,  8.90D0,  8.90D0,  8.90D0,  &
                    1.43D0,  1.76D0, 47.00D0, 18.80D0,  &
                    2.47D0,  8.90D0,  8.90D0, 17.80D0,  &
                    2.04D0,  2.57D0,  1.86D0,  1.65D0,  &
                    0.60D0,  0.60D0, 14.00D0,  1.76D0,  &
                    0.56D0,  8.90D0,  8.90D0,  8.90D0,  &
                    8.90D0, 47.00D0, 47.00D0 /
      DATA EADATA/ 61.88D0, 12.88D0, 12.88D0, 12.88D0,  &
                   75.50D0, 61.88D0,  1.00D0,  7.01D0,  &
                   44.37D0, 12.88D0, 12.88D0,  6.44D0,  &
                   56.96D0, 44.82D0, 58.91D0, 65.70D0,  &
                  180.00D0,180.00D0, 10.18D0, 61.88D0,  &
                  202.37D0, 12.88D0, 12.88D0, 12.88D0,  &
                   12.88D0,  1.00D0,  1.00D0 /
      DATA EBDATA/ 15.03D0,  5.73D0,  5.73D0,  5.73D0,  &
                   16.54D0, 15.03D0,  1.00D0,  5.11D0,  &
                   11.29D0,  5.73D0,  5.73D0,  2.88D0,  &
                    7.05D0,  6.32D0, 13.25D0, 14.59D0,  &
                   15.70D0, 15.70D0,  3.58D0, 15.03D0,  &
                   15.78D0,  5.73D0,  5.73D0,  5.73D0,  &
                    5.73D0,  1.00D0,  1.00D0 /
      DATA ZZDATA/  26.0D0,  13.0D0,  13.0D0,  13.0D0,  &
                    29.0D0,  26.0D0,   0.0D0,   6.0D0,  &
                   -26.0D0,  13.0D0,  13.0D0, -13.0D0,  &
                     0.0D0,   0.0D0, -29.0D0, -29.0D0,  &
                   -82.0D0, -82.0D0,  18.0D0,  26.0D0,  &
                    82.0D0,  13.0D0,  13.0D0,  13.0D0,  &
                    13.0D0,   0.0D0,   0.0D0 /
!
!-- /COVERS/
      CALL GET_AMDC_VERSION( I_AMDC_VERSION )
      C_AMDC_NAME   = 'No special name '
      IVERSION_AMDC = 0
!
!-- Initialisation of Common /COFORC/
      IPFORC = 0
      ICFORC = 0
!
!-- /CHACAR/
      CARTYP(1:MTYP) = 'XXX'
      CARTEC(1:MTEC)='XXX'
!-- /CHAPOS/
      NBSET = (MTYP_F_Z+1) * 7
      Pos_Gama_V(MTYP_F_Z) = XVERIF
      CALL SETNUL( Pos_Phi_V(0), NBSET, XVERIF )
      NBSET = (MTYP_F_Z+1) * 4  +  MTYP*8*(2*MZZ+1)  +  1
      NB_JTYP_F_Z = JVERIF
      CALL SETNIL( IGEO_V(0), NBSET, JVERIF )
!-- /CHAGEO/
      NBSET = (MTYP_G_O+1) * 10
      Geo_D3_V(MTYP_G_O) = XVERIF+1.D0
      CALL SETNUL( Geo_dx_V(0), NBSET, XVERIF+1.D0 )
      NBSET = (MTYP_G_O+1) * 5  +  MTYP*MGEO  +  MTYP*MGEO*MOBJ  +  1
      NB_JTYP_G_O = JVERIF+1
      CALL SETNIL( NOBJ(1,1), NBSET, JVERIF+1 )
!-- /CHACUT/
      NBSET = MCUT * MOCU * 7
      Cut_An(MCUT,MOCU) = XVERIF+2.D0
      CALL SETNUL( Cut_dx(1,1), NBSET, XVERIF+2.D0 )
      NBSET = MTYP*MGEO*4 + MCUT + MCUT*MOCU
      IOBCUT(MCUT,MOCU) = JVERIF+2
      CALL SETNIL( INOCUT(1,1,1), NBSET, JVERIF+2 )
!-- /CHASTA/
      NBSET = MTEC * MSTA * 45
      STAOO(MTEC,MSTA,20) = XVERIF+3.D0
      CALL SETNUL( STAX0(1,1), NBSET, XVERIF+3.D0 )
      NBSET = MTEC * MSTA * 24
      IVALIDSTA(MTEC,MSTA) = JVERIF+3
      CALL SETNIL( ISTAMA(1,1,1), NBSET, JVERIF+3 )
!-- /CHASTATGC/
      NBSET = MSTA * (16+2*Mstrp)
      XtgcStrp(MSTA,Mstrp,2) = XVERIF+4.D0
      CALL SETNUL( XtgcAdd(1,1), NBSET, XVERIF+4.D0 )
      NBSET = MSTA * (10+3*Mgang)
      NtgcGang(MSTA,Mgang,3) = JVERIF+4
      CALL SETNIL( NtgcAdd(1,1), NBSET, JVERIF+4 )
!-- /CHADJUST/
      SZTrotadjust(3,Madjust) = XVERIF+5.D0
      CALL SETNUL( SZTtraadjust(1,1), 6*Madjust, XVERIF+5.D0 )
      NBadjust = JVERIF+5
      CALL SETNIL( JTYPFZOLadjust(1), Madjust+2*MTYP+1, JVERIF+5 )
!-- /CHdwnlay/
      SZTrotdwnlay(3,Mdwnlay) = XVERIF+5.D0
      CALL SETNUL( SZTtradwnlayCentred(1,1),12*Mdwnlay, XVERIF+5.D0 )
      NBdwnlay = JVERIF+5
      CALL SETNIL( JTYPFZOLdwnlay(1), Mdwnlay+1, JVERIF+5 )
!-- /CHADEFOR/
      SZTdeform(11,Mdeform) = XVERIF+6.D0
      CALL SETNUL( SZTdeform(1,1), 11*Mdeform, XVERIF+6.D0 )
      SZTDeformSS(4,Mdeform) = XVERIF+6.D0
      CALL SETNUL( SZTDeformSS(1,1), 4*Mdeform, XVERIF+6.D0 )
      SZTDeformZZ(4,Mdeform) = XVERIF+6.D0
      CALL SETNUL( SZTDeformZZ(1,1), 4*Mdeform, XVERIF+6.D0 )
      SZTDeformTT(4,Mdeform) = XVERIF+6.D0
      CALL SETNUL( SZTDeformTT(1,1), 4*Mdeform, XVERIF+6.D0 )
      DeformSSZZTTDone = JVERIF+6
      CALL SETNIL( JTYPFZOLdeform(1), Mdeform+2*MTYP+2, JVERIF+6 )
!-- /CHADEADCHA/
      NBdeadChan = JVERIF+7
      CALL SETNIL( JTYPFZOLdeadChan(1), 6*MdeadChan+2, JVERIF+7 )
!-- /CHAAGDD/
      LenAGDD = 0
      CarAGDD(1:MaxAGDD) = ''
!-- /CHAAMDC/
      LenAMDC = 0
      CarAMDC(1:MaxAMDC) = ''
!-- /HARDSOFT/
      HARDENTRIES = 0
      DO I=1,MTYP_F_Z
        HARDNAME         (I) = ''
        SOFTNAME         (I) = ''
        SOFTOCTANT       (I) =  0
        SOFTIZ           (I) =  0
        IentryHardSoft_V (I) = -1
      ENDDO
!-- /XTOMO/
      XTOMOENTRIES = 0
      XtomoChberName (1:XTOMOMAX)  = ''
      XtomoSite      (1:XTOMOMAX)  = ''
      XtomoSiteId    (1:XTOMOMAX)  = 0 
      XtomoTime      (1:XTOMOMAX)  = 0
      XtomoPassed    (1:XTOMOMAX)  = 0
      XtomoSide      (1:XTOMOMAX)  = ''
      XtomoNberTube1 (1:XTOMOMAX)  = 0 
      XtomoNberTube2 (1:XTOMOMAX)  = 0
      XtomoNberML    (1:XTOMOMAX)  = 0
      XtomoNberLayer (1:XTOMOMAX)  = 0
      XtomoML1NYtub (1:XTOMOMAX)  = 0.d0
      XtomoML1NZtub (1:XTOMOMAX)  = 0.d0
      XtomoML1NDely (1:XTOMOMAX)  = 0.d0
      XtomoML1NDelz (1:XTOMOMAX)  = 0.d0 
      XtomoML1NDela (1:XTOMOMAX)  = 0.d0 
      XtomoML1NYpit (1:XTOMOMAX)  = 0.d0 
      XtomoML1NZpit (1:XTOMOMAX)  = 0.d0
      XtomoML1PYtub (1:XTOMOMAX)  = 0.d0
      XtomoML1PZtub (1:XTOMOMAX)  = 0.d0
      XtomoML1PDely (1:XTOMOMAX)  = 0.d0 
      XtomoML1PDelz (1:XTOMOMAX)  = 0.d0 
      XtomoML1PDela (1:XTOMOMAX)  = 0.d0 
      XtomoML1PYpit (1:XTOMOMAX)  = 0.d0
      XtomoML1PZpit (1:XTOMOMAX)  = 0.d0
      XtomoML2NYtub (1:XTOMOMAX)  = 0.d0
      XtomoML2NZtub (1:XTOMOMAX)  = 0.d0 
      XtomoML2NDely (1:XTOMOMAX)  = 0.d0 
      XtomoML2NDelz (1:XTOMOMAX)  = 0.d0 
      XtomoML2NDela (1:XTOMOMAX)  = 0.d0
      XtomoML2NYpit (1:XTOMOMAX)  = 0.d0
      XtomoML2NZpit (1:XTOMOMAX)  = 0.d0
      XtomoML2PYtub (1:XTOMOMAX)  = 0.d0 
      XtomoML2PZtub (1:XTOMOMAX)  = 0.d0 
      XtomoML2PDely (1:XTOMOMAX)  = 0.d0
      XtomoML2PDelz (1:XTOMOMAX)  = 0.d0
      XtomoML2PDela (1:XTOMOMAX)  = 0.d0
      XtomoML2PYpit (1:XTOMOMAX)  = 0.d0
      XtomoML2PZpit (1:XTOMOMAX)  = 0.d0
      IentryXtomo_V (1:XTOMOMAX)  = -1
!
!
!-- /COMAMA/ is not cleared but filled with default values !
      IIMAMA(1:MAMAMA) = 0
      X0MAMA(1:MAMAMA) = 999999.d0
      EAMAMA(1:MAMAMA) = 0.d0
      EBMAMA(1:MAMAMA) = 0.d0
      ZZMAMA(1:MAMAMA) = 0.d0
      NBMAMA = NBDATA
      IIMAMA(       1:NBDATA ) = IIDATA(1:NBDATA) 
      X0MAMA(IIDATA(1:NBDATA)) = X0DATA(1:NBDATA)
      EAMAMA(IIDATA(1:NBDATA)) = EADATA(1:NBDATA)
      EBMAMA(IIDATA(1:NBDATA)) = EBDATA(1:NBDATA)
      ZZMAMA(IIDATA(1:NBDATA)) = ZZDATA(1:NBDATA)
      MATALI3 = JVERIF+6
      CALL SETNIL( MATDUMMY(1), 51+MSHIE, JVERIF+6 )
!-- /CO_MAT_INTEGER/
      Lfi_Align(MALIGN) = JVERIF+5
      CALL SETNIL( IISCTZ(1), 36+MSHIE+MALIGN, JVERIF+5 )
!-- /CO_MAT_BARREL/
      ZANNO = XVERIF+5.D0
      CALL SETNUL( ZVOUS(1), 60, XVERIF+5.D0 )
!-- /CO_MAT_ENDCAP/
      DY3ETU = XVERIF+6.D0
      CALL SETNUL( ZECDUMMY(1), 38, XVERIF+6.D0 )
!-- /CO_MAT_FEET/
      DY2FEB = XVERIF+7.D0
      CALL SETNUL( ZFEE(1), 58, XVERIF+7.D0 )
!-- /CO_MAT_RAIL/
      CHAMFR = XVERIF+8.D0
      CALL SETNUL( RAIDUMMY(1), 10, XVERIF+8.D0 )
!-- /CO_MAT_GIRDER/
!-----------------------> Iron girder
      MATGIRD =  1
      ZMAGIRD = 611.0D0
      ZMIGIRD =   0.0D0
      DZHGIRD =   0.0D0
      RAOGIRD = 430.0D0
      RIOGIRD = 421.0D0
      RAIGIRD = 407.0D0
      RIIGIRD = 400.0D0
      DS1GIRD =   4.5D0
      DS2GIRD =  23.0D0
!-- /CO_MAT_ECAL_BAR/
!-----------------------> Barrel e.m. liquid argon "warm" cryostat
      MATCALCR = 11
      ZAECA1   = 340.5D0
      RIECA1   = 115.0D0
      RAECA1   = 225.0D0
      EPECA1   =   4.0D0
      EPECA2   =   2.0D0
      COECA1   =  40.0D0
      ZAECA2   = 285.0D0
      RAECA2   = 277.5D0
      MATFEED = 11
      NNFEED  = 32
      RRFEED  =  15.0D0
      HHFEED  =  26.0D0
!-----------------------> Barrel e.m. "cold" cryostat
      MATCOC = 17
      ZACOCR = 331.1D0
      ZMCOCR = 294.0D0
      EPCOCR =   4.4D0
      RICOCR = 134.6D0
      RMCOCR = 220.5D0
      RACOCR = 230.0D0
      COCOCR =  20.6D0
!-----------------------> Barrel e.m. pre-shower calo.
      MATPSBAR =  6
      RIPSBAR  = 141.0D0
      RAPSBAR  = 145.0D0
      ZAPSBAR  = 305.0D0
!-----------------------> Barrel e.m. liquid argon accordion calo.
      MATACORD1 = 19
      MATACORD2 = 29
      MATACORD3 = 29
      RIACORD   = 147.0D0
      RAACORD   = 201.0D0
      ZAACORD   = 317.0D0
      ETACORD0  = 0.000D0
      ETACORD1  = 0.800D0
      ETACORD2  = 1.400D0
      ETACORD3  = 1.475D0
      DSAMPL11  = 10.0D0
      DSAMPL12  = 37.0D0
      DSAMPL21  =  8.0D0
      DSAMPL22  = 45.0D0
      DSAMPL31  =  8.0D0
!-----------------------> Barrel e.m. accordion calo. support rings
      MATSURI   = 1
      NNSURI    = 7
      HHSURI    =  10.0D0
      AASURI    =   8.0D0
      EESURI    =   1.1D0
      ZZSURI(1) =  10.0D0
      ZZSURI(2) =  39.8D0
      ZZSURI(3) =  81.4D0
      ZZSURI(4) = 125.7D0
      ZZSURI(5) = 175.0D0
      ZZSURI(6) = 232.0D0
      ZZSURI(7) = 288.0D0
!-- /CO_MAT_CALO_END/
!-----------------------> End-cap cryostat
      MATECCR = 11
      ZIECC1  = 350.D0
      ZIECC2  = 457.D0
      ZIECC3  = 612.5D0
      ZAECC1  = 664.D0
      RIECC1  =  23.D0
      RIECC2  = RIECC1 * ZIECC2/ZIECC1
      RIECC3  =   5.D0
      RAECC1  = 222.D0
      RAECC2  = 250.D0
      EPECC1  =   4.D0
      EPECC2  =   2.D0
      EPECC3  =   6.D0
      MATFEEC = 11
      NNFEEC  = 32
      RRFEEC  =  15.D0
      HHFEEC  =  20.D0
!-----------------------> End-cap had. liquid argon calo.
      MATECHAD1 = 28
      MATECHAD2 = 23
      ZIECHAD1  = 428.0D0
      ZMECHAD1  = 455.0D0
      ZAECHAD1  = 509.6D0
      RIECHAD   =  37.5D0
      RMECHAD   =  48.0D0
      RAECHAD   = 210.0D0
      ZIECHAD2  = 513.8D0
      ZAECHAD2  = 610.0D0
!-----------------------> End-cap "very forward" calorimeters
      MATECF1 = 36
      MATECF2 = 37
      ZIECF1 = 467.00D0
      ZAECF1 = 512.00D0
      RIECF1 =   7.20D0
      ZIECF2 = 512.30D0
      ZAECF2 = 557.30D0
      RIECF2 =   7.90D0
      ZIECF3 = 559.75D0
      ZAECF3 = 604.75D0
      RIECF3 =   8.60D0
      ZIECF4 = 607.20D0
      ZAECF4 = 653.00D0
      RIECF4 =   9.30D0
      RAECFN =  45.50D0
!-----------------------> End-cap e.m. liquid argon accordion ("spanish fan") calo.
!-----------------------> E.m. end-cap inner-wheel <-> outer-wheel separartion
      MATECEL   = 19
      ECELOR    =  203.4D0        ! outer radius of the active part
      ECELIR    =   30.2D0        ! inner radius of the active part (<=> theta=4.7 deg)
      ECELIZ    =  370.4D0        ! inner z (front face position) of the active part
      ECELDEEP  =   51.0D0        ! nominal depth of the active volume of the EM EndCap
      ECELGBW   =    0.4D0        ! dead gap between active part of outer/inner wheel
!-----------------------> Different regions of the e.m. end-cap
      ECELOETA1 =   1.375D0      ! outer wheel
      ECELOETA2 =   1.500D0
      ECELOETA3 =   1.800D0
      ECELOETA4 =   2.000D0
      ECELOETA5 =   2.400D0
      ECELOETA6 =   2.500D0
      ECELIETA1 =   2.500D0      ! inner wheel
      ECELIETA2 =   3.200D0
      ECELRDP1  =  3.0D0/18.5D0  ! first sampling (eta> 1.9)
      ECELRDP2  =  2.5D0/18.5D0  ! first sampling (eta< 1.9)
      ECELRDP3  = 15.5D0/18.5D0  ! first+second sampling (low eta)
      ECELRDP4  = 14.5D0/18.5D0  ! first+second sampling (medium eta)
      ECELRDP5  = 11.5D0/18.5D0  ! first+second sampling (high eta)
!-----------------------> End-cap e.m. presampler
      MATECELPS  =  6
      ECELPSETA1 =   1.5D0
      ECELPSETA2 =   1.8D0
      ECELPSDEEP =   1.5D0   ! @@@@@@ (???)
      ECELPSIZ   = 362.5D0
!-----------------------> End-cap cold vessel
      MATECELC  = 17
      ECELCVIZ1 = ECELPSIZ    -   6.0D0
      ECELCVIZ2 = ECELPSIZ    + 258.0D0
      ECELCVOZ1 = ECELPSIZ    +   0.5D0
      ECELCVOZ2 = ECELPSIZ    +  15.0D0
      ECELCVOZ3 = ECELPSIZ    + 282.0D0
      ECELCVIR1 =  23.5D0
      ECELCVIR2 =  27.5D0
      ECELCVOR1 = 213.0D0
      ECELCVOR2 = 216.5D0
      ECELCVOR3 = 224.5D0
!-----------------------> Plug 1
      MATECELPG = 35
      ECELPG1IR =  58.5D0
      ECELPG1OR = 192.0D0
      ECELPG1OZ = ECELCVOZ3
      ECELPG1DP =  20.0D0
!-----------------------> Plug 2
      ECELPG2IR = 46.5D0
      ECELPG2OR = 56.0D0
      ECELPG2IZ = ECELPSIZ  + 251.0D0
      ECELPG2OZ = ECELPG2IZ +  18.0D0
!-----------------------> Plug 4 (?)
      ECELPG4DP =  8.80D0
!-- /CO_MAT_TILE_CAL/
!-----------------------> Barrel had. "tile" calo.
      MATTIL  = 14
      ZABATIL = 282.0D0
      RIBATIL = 228.0D0
      RABATIL = 398.0D0
      DRBTIL1 =   0.0D0
      DRBTIL2 =   0.0D0
!-----------------------> Ext. Barrel had. "tile" calo.
      ZIEXTIL = 350.0D0
      ZAEXTIL = 611.0D0
      RIEXTIL = 228.0D0
      RAEXTIL = 398.0D0
      DRETIL1 =   0.0D0
      DRETIL2 =   0.0D0
!-----------------------> Int. Barrel had. "tile" calo.
      DZITIL1 =  31.0D0
      DZITIL2 =   9.5D0
      DZITIL3  =  0.0D0
      DRITIL1 =  46.2D0
      DRITIL2 =  45.0D0
      DRITIL3  =  0.0D0
      DRITIL4  =  0.0D0
      DRITIL5  =  0.0D0
!-- /CO_MAT_SOLENO/
!-----------------------> Solenoid magnet
      MATSOLE = 18
      ZASOLE  = 265.0D0
      RISOLE  = 121.0D0
      RASOLE  = 126.7D0
!-- /CO_MAT_PIXELS/
      EPPIXZ = XVERIF+9.D0
      CALL SETNUL( ZAPIXR(1), 27, XVERIF+9.D0 )
!-- /CO_MAT_SCTS/
      EPSCTYP = XVERIF+10.D0
      CALL SETNUL( ZISCTZ(1), 45, XVERIF+10.D0 )
!-- /CO_MAT_TRTS/
      RATRTZ3 = XVERIF+11.D0
      CALL SETNUL( ZITRTZ1(1), 47, XVERIF+11.D0 )
!-- /CO_MAT_SHIELD/
      RAOSHI(MSHIE) = XVERIF+12.D0
      CALL SETNUL( ZISHIE(1), 6*MSHIE, XVERIF+12.D0 )
!-- /CO_MAT_ALIGN/
      Thicknes_Align(MALIGN) = XVERIF+13.D0
      CALL SETNUL( EndPoint_Align(1,1,1), 8*MALIGN, XVERIF+13.D0 )
!
      END SUBROUTINE CLDBCO
!
