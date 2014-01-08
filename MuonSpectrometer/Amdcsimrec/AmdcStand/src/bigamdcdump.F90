!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE BIGAMDCDUMP
 IMPLICIT NONE
 INTEGER :: jtyp, jff, jzz, jgeo, job, jcut, itotcu, jtec, jsta, nll, l, ima
!--------------------------------------------------------------------
!  
!--------------------------------------------------------------------
!
#include "AmdcStand/comamu.inc"
!
#include "AmdcStand/comama.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
!-- CHAPOS
      do jtyp=1,mtyp
       do jff=1,8
        do jzz=-mzz,mzz
         jgeo = igeo(jtyp,jff,jzz)
         if(jgeo > 0) then
          print 3010,jtyp,jff,jzz,  &
           pos_phi (jtyp,jff,jzz),  &
           pos_z   (jtyp,jff,jzz),  &
           pos_r   (jtyp,jff,jzz),  &
           pos_s   (jtyp,jff,jzz),  &
           pos_alfa(jtyp,jff,jzz),  &
           pos_beta(jtyp,jff,jzz),  &
           pos_gama(jtyp,jff,jzz),  &
           ibarel  (jtyp,jff,jzz)
          print 3012,               &
           igeo    (jtyp,jff,jzz),  &
           icut    (jtyp,jff,jzz)
!---- CHAGEO
          do  job=1,nobj(jtyp,jgeo)
           print 1010,job,          &
            geo_dx(jtyp,jgeo,job),  &
            geo_dy(jtyp,jgeo,job),  &
            geo_dz(jtyp,jgeo,job),  &
            geo_ws(jtyp,jgeo,job),  &
            geo_wl(jtyp,jgeo,job),  &
            geo_le(jtyp,jgeo,job),  &
            geo_ex(jtyp,jgeo,job),  &
            geo_d1(jtyp,jgeo,job),  &
            geo_d2(jtyp,jgeo,job),  &
            geo_d3(jtyp,jgeo,job),  &
            itec  (jtyp,jgeo,job),  &
            ista  (jtyp,jgeo,job),  &
            isplix(jtyp,jgeo,job),  &
            ispliy(jtyp,jgeo,job),  &
            ishape(jtyp,jgeo,job)
          enddo
!---- CHACUT
          jcut=icut    (jtyp,jff,jzz)
          if (jcut >= 1) then
           itotcu = inocut(jtyp,jgeo,jcut)
           do job=1,nobcut(itotcu)
            print 2010,job,        &
             cut_dx(itotcu,job),   &
             cut_dy(itotcu,job),   &
             cut_ws(itotcu,job),   &
             cut_wl(itotcu,job),   &
             cut_le(itotcu,job),   &
             cut_ex(itotcu,job),   &
             cut_an(itotcu,job),   &
             iobcut(itotcu,job)
           enddo
          endif
         endif
        enddo
       enddo
      enddo
3010  FORMAT('CHAPOS ',  &
       I5   ,'JTYP',     &
       I5   ,'JFF ',     &
       I5   ,'JZZ ',     &
       F15.3,'PHI ',     &
       F15.3,'Z   ',     &
       F15.3,'R   ',     &
       F15.3,'S   ',     &
       F15.3,'AL  ',     &
       F15.3,'BE  ',     &
       F15.3,'GA  ',     &
       I5   ,'IB  ')
3012  FORMAT('**igeo,icut ',2I5)
!
1010  FORMAT('**CHAGEO ',  &
       I5   ,'JOB  ',      &
       F15.3,'DX   ',      &
       F15.3,'DY   ',      &
       F15.3,'DZ   ',      &
       F15.3,'WS   ',      &
       F15.3,'WL   ',      &
       F15.3,'LE   ',      &
       F15.3,'EX   ',      &
       F15.3,'D1   ',      &
       F15.3,'D2   ',      &
       F15.3,'D3   ',      &
       I5   ,'ITEC ',      &
       I5   ,'ISTA ',      &
       I5   ,'ISPX ',      &
       I5   ,'ISPY ',      &
       I5   ,'ISHA ')
!
2010  FORMAT('**CHACUT ',  &
       I5   ,'JOB ',       &
       F15.3,'DX  ',       &
       F15.3,'DY  ',       &
       F15.3,'WS  ',       &
       F15.3,'WL  ',       &
       F15.3,'LE  ',       &
       F15.3,'EX  ',       &
       F15.3,'AN  ',       &
       I5   ,'IOC ')
!
!-- CHACAR
      do jtyp=1,mtyp
        print 4010,jtyp,cartyp(jtyp)
      enddo
4010  FORMAT('CHACAR ',I5,'JTYP   ',A3)
      do jtec=1,mtec
        print 4020,jtec,cartec(jtec)
      enddo
4020  FORMAT('CHACAR ',I5,'JTEC   ',A3)
!
!-- CHASTA
      print 5000
      do jtec=1,mtec
       do jsta=1,msta
        nll = nlaz(jtec,jsta) + nlas(jtec,jsta)
        if(nll > 0) then
         print 5010,jtec,jsta,   &
             stax0 (jtec,jsta),  &
             stapp (jtec,jsta),  &
             starr (jtec,jsta),  &
             staee (jtec,jsta),  &
             stapg (jtec,jsta),  &
             nlaz  (jtec,jsta),  &
             nlas  (jtec,jsta),  &
             iforma(jtec,jsta)
!
         print 5022,jtec,jsta,(statt(jtec,jsta,l),l=1,20)
         print 5024,jtec,jsta,(staoo(jtec,jsta,l),l=1,20)
         print 5026,jtec,jsta,(istama(jtec,jsta,l),l=1,20)
!
        endif
       enddo
      enddo
5000  FORMAT('CHASTA ',            &
             ' JTEC',              &
             ' JSTA',              &
             '          STAX0',    &
             '          STAPP',    &
             '          STARR',    &
             '          STAEE',    &
             '          STAPG',    &
             '           NLAZ',    &
             '           NLAS',    &
             '         IFORMA')
5010  FORMAT('CHASTA ',2I5,5F15.3,3I15)
5022  FORMAT('CHASTA STATT  ',2I5,20F15.3)
5024  FORMAT('CHASTA STAOO  ',2I5,20F15.3)
5026  FORMAT('CHASTA ISTAMA ',2I5,20I15)
!
!-- COMAMU
!--- CO_MAT_INTEGER
      print 6000,                                          &
        ncryo , ncurv , ncurc , nvous, nlatt , nanno,      &
        nectu , ncueco, nfee  , nbiel, nnfeed, nnsuri,     &
        nnfeec, npixr , npixz , nsctr, nsctz ,             &
        nsctyp, ntrtz1, ntrtz2, ntrtz3,                    &
        nbshie, nb_align_tub, nb_align_bar, nb_align_axl
      print 6002,(iisctz(l),l=1,10)
      print 6004,(ncshie(l),l=1,mshie)
      print 6005,(lfi_align(l),l=1,malign)
6000  FORMAT('CO_MAT_INTEGER A ',20I5)
6002  FORMAT('CO_MAT_INTEGER B ',20I5)
6004  FORMAT('CO_MAT_INTEGER C ',20I5)
6005  FORMAT('CO_MAT_INTEGER D ',20I5)
!
!--- CO_MAT_BARREL
      print 6010,                                         &
       econd ,hcond ,                                     &
       rbamin,rbamax,zbamax,rcryo ,ecryob,rcurv ,         &
       xatti ,zatti ,dyatt ,zatta ,dlbiel,epbiel,rrbiel,  &
       exbiel,zbieli,zbiela,xcatti,zcatti
      print 6012,                                         &
       xheadi,xheada,dyhead,                              &
       dxlatt,dzlatt,dxclat,dzclat,                       &
       tvousi,svousi,dtvous,dzvous,dttrou,s1trou,s2trou,  &
       tstrui,tstrua,dzstru,ranno ,tanno ,zanno
      print 6014,(zvous(l),l=1,10)
      print 6016,(zlatt(l),l=1,10)
6010  FORMAT('CO_MAT_BARREL A1',20F15.3)
6012  FORMAT('CO_MAT_BARREL A2',20F15.3)
6014  FORMAT('CO_MAT_BARREL B ',20F15.3)
6016  FORMAT('CO_MAT_BARREL C ',20F15.3)
!
!--- CO_MAT_ENDCAP
      print 6020,                                        &
       zecmi ,zecma ,tecma1,tecma2,secma1,secma2,recmi,  &
       ecryoe,ecryof,ecryor,tectu ,rectu ,drectu,fectu,  &
       zecomi,zecoma,tecomi,tecoma,epeco ,rcueco
       print 6022,                                       &
       zecami,zecama,epzeca,teca1 ,teca2 ,teca3 ,teca4,  &
       dz1etu,dy1etu,ro1etu,ri1etu,dz2etu,dy2etu,rr2etu, &
       dz3etu,dy3etu
6020  FORMAT('CO_MAT_ENDCAP A1',20F15.3)
6022  FORMAT('CO_MAT_ENDCAP A2',16F15.3)
!
!--- CO_MAT_FEET
      print 6030,                                              &
       ep1fee   ,ep2fee   ,dssfee   ,dspfee   ,                &
       dd1fee   ,dd2fee   ,dz1fee   ,dz2fee   ,dz3fee,         &
       ti1fee   ,tm1fee   ,ta1fee   ,ti2fee   ,ta2fee,ta3fee,  &
       dl1fee   ,dl2fee   ,dx1fee   ,dx2fee
      print 6032,                                              &
       dbxfee   ,dbyfee   ,dbefee   ,dz7fee   ,dl7fee,dt7fee,  &
       xfeebri  ,yfeebri  ,                                    &
       dx1feebri,dx2feebri,dx3feebri,dx4feebri,                &
       dy1feebri,dy2feebri,dy3feebri,dy4feebri
      print 6034,                                              &
       dz1feebri,dz2feebri,                                    &
       dz1feb   ,dz2feb   ,ep1feb   ,ep2feb   ,rtrfeb,         &
       dx1feb,   dx2feb   ,dx3feb   ,dx4feb   ,dy1feb,dy2feb
      print 6036,(zfee(l),l=1,10)
6030  FORMAT('CO_MAT_FEET A1',19F15.3)
6032  FORMAT('CO_MAT_FEET A2',16F15.3)
6034  FORMAT('CO_MAT_FEET A3',13F15.3)
6036  FORMAT('CO_MAT_FEET B ',20F15.3)
!
!--- CO_MAT_RAIL
      print 6040,zrail ,xrail1,yrail1,xrail2,yrail2,xrail3,yrail3,chamfr
6040  FORMAT('CO_MAT_RAIL A ',8F15.3)
!
!--- CO_MAT_GIRDER
      print 6050,zmagird,zmigird,dzhgird,                        &
                 raogird,riogird,raigird,riigird,ds1gird,ds2gird
6050  FORMAT('CO_MAT_GIRDER A ',9F15.3)
!
!--- CO_MAT_ECAL_BAR
      print 6060,                                                   &
       zaeca1  ,rieca1  ,raeca1  ,epeca1  ,                         &
       epeca2  ,coeca1  ,zaeca2  ,raeca2  ,rrfeed ,hhfeed ,         &
       zacocr  ,zmcocr  ,epcocr  ,ricocr  ,rmcocr ,racocr ,cococr
      print 6062,                                                   &
       ripsbar ,rapsbar ,zapsbar ,riacord ,raacord,zaacord,         &
       etacord0,etacord1,etacord2,etacord3,                         &
       dsampl11,dsampl12,dsampl21,dsampl22,dsampl31,                &
       hhsuri  ,aasuri  ,eesuri
      print 6064,(zzsuri(l),l=1,10)
6060  FORMAT('CO_MAT_ECAL_BAR A1',17F15.3)
6062  FORMAT('CO_MAT_ECAL_BAR A2',18F15.3)
6064  FORMAT('CO_MAT_ECAL_BAR B ',20F15.3)
!
!--- CO_MAT_CALO_END
      print 6070,                                              &
       ziecc1    ,ziecc2    ,ziecc3    ,zaecc1   ,riecc1   ,   &
       riecc2    ,riecc3    ,raecc1    ,raecc2   ,epecc1   ,   &
       epecc2    ,epecc3    ,rrfeec    ,hhfeec   ,             &
       ziechad1  ,zmechad1  ,zaechad1  ,riechad  ,rmechad
      print 6072,                                              &
       raechad   ,ziechad2  ,zaechad2  ,                       &
       ziecf1    ,zaecf1    ,riecf1    ,ziecf2   ,zaecf2   ,   &
       riecf2    ,ziecf3    ,zaecf3    ,riecf3   ,ziecf4   ,   &
       zaecf4    ,riecf4    ,raecfn
      print 6074,                                              &
       ecelor    ,ecelir    ,eceliz    ,eceldeep ,ecelgbw  ,   &
       eceloeta1 ,                                             &
       eceloeta2 ,eceloeta3 ,eceloeta4 ,eceloeta5,eceloeta6,   &
       ecelieta1 ,ecelieta2 ,                                  &
       ecelrdp1  ,ecelrdp2  ,ecelrdp3  ,ecelrdp4 ,ecelrdp5
      print 6076,                                              &
       ecelpseta1,ecelpseta2,ecelpsdeep,ecelpsiz ,             &
       ecelcviz1 ,ecelcviz2 ,ecelcvoz1 ,ecelcvoz2,ecelcvoz3
      print 6078,                                              &
       ecelcvir1 ,ecelcvir2 ,ecelcvor1 ,ecelcvor2,ecelcvor3,   &
       ecelpg1ir ,ecelpg1or ,ecelpg1oz ,ecelpg1dp,             &
       ecelpg2ir ,ecelpg2or ,ecelpg2iz ,ecelpg2oz,ecelpg4dp
6070  FORMAT('CO_MAT_CALO_END A1',19F15.3)
6072  FORMAT('CO_MAT_CALO_END A2',16F15.3)
6074  FORMAT('CO_MAT_CALO_END A3',18F15.3)
6076  FORMAT('CO_MAT_CALO_END A4', 9F15.3)
6078  FORMAT('CO_MAT_CALO_END A5',14F15.3)
!
!--- CO_MAT_TILE_CAL
      print 6080,                                        &
       zabatil,ribatil,rabatil,drbtil1,drbtil2,          &
       ziextil,zaextil,riextil,raextil,dretil1,dretil2,  &
       dzitil1,dzitil2,dritil1,dritil2,                  &
       dzitil3,dritil3,dritil4,dritil5
6080  FORMAT('CO_MAT_TILE_CAL A1',19F15.3)
!
!--- CO_MAT_SOLENO
      print 6090,ZASOLE,RISOLE,RASOLE
6090  FORMAT('CO_MAT_SOLENO A1',3F15.3)
!
!--- CO_MAT_PIXELS
      print 6100,eppixr,eppixz
      print 6101,(zapixr(l),l=1,5)
      print 6102,(ripixr(l),l=1,5)
      print 6103,(zipixz(l),l=1,5)
      print 6104,(ripixz(l),l=1,5)
      print 6105,(rapixz(l),l=1,5)
6100  FORMAT('CO_MAT_PIXELS A1',2F15.3)
6101  FORMAT('CO_MAT_PIXELS B1',20F15.3)
6102  FORMAT('CO_MAT_PIXELS B2',20F15.3)
6103  FORMAT('CO_MAT_PIXELS B3',20F15.3)
6104  FORMAT('CO_MAT_PIXELS B4',20F15.3)
6105  FORMAT('CO_MAT_PIXELS B5',20F15.3)
!
!--- CO_MAT_SCTS
      print 6110,rasctz,epsctz,epsctr,epsctyp,zasctr
      print 6111,(zisctz(l),l=1,10)
      print 6112,(risctz(l),l=1,10)
      print 6113,(risctr(l),l=1,5)
      print 6114,(risctyp(l),l=1,5)
      print 6115,(rasctyp(l),l=1,5)
      print 6116,(dzsctyp(l),l=1,5)
6110  FORMAT('CO_MAT_SCTS A1',5F15.3)
6111  FORMAT('CO_MAT_SCTS B1',20F15.3)
6112  FORMAT('CO_MAT_SCTS B2',20F15.3)
6113  FORMAT('CO_MAT_SCTS B3',20F15.3)
6114  FORMAT('CO_MAT_SCTS B4',20F15.3)
6115  FORMAT('CO_MAT_SCTS B5',20F15.3)
6116  FORMAT('CO_MAT_SCTS B6',20F15.3)
!
!--- CO_MAT_TRTS
      print 6120,zitrtr ,zatrtr ,ritrtr ,rmtrtr ,ratrtr,         &
                 dztrtz1, dztrtz2, dztrtz3,                      &
                 ritrtz1,ratrtz1,ritrtz2,ratrtz2,ritrtz3,ratrtz3
      print 6121,(zitrtz1(l),l=1,12)
      print 6122,(zitrtz2(l),l=1,10)
      print 6123,(zitrtz3(l),l=1,10)
6120  FORMAT('CO_MAT_TRTS A1',14F15.3)
6121  FORMAT('CO_MAT_TRTS B1',20F15.3)
6122  FORMAT('CO_MAT_TRTS B2',20F15.3)
6123  FORMAT('CO_MAT_TRTS B3',20F15.3)
!
!-- CO_MAT_SHIELD
      print 6131,(zishie(l),l=1,mshie)
      print 6132,(zashie(l),l=1,mshie)
      print 6133,(riishi(l),l=1,mshie)
      print 6134,(rioshi(l),l=1,mshie)
      print 6135,(raishi(l),l=1,mshie)
      print 6136,(raoshi(l),l=1,mshie)
6131  FORMAT('CO_MAT_SHIELD B1',20F15.3)
6132  FORMAT('CO_MAT_SHIELD B2',20F15.3)
6133  FORMAT('CO_MAT_SHIELD B3',20F15.3)
6134  FORMAT('CO_MAT_SHIELD B4',20F15.3)
6135  FORMAT('CO_MAT_SHIELD B5',20F15.3)
6136  FORMAT('CO_MAT_SHIELD B6',20F15.3)
!
!-- CO_MAT_ALIGN
      print 6137,(endpoint_align(1,1,l),l=1,malign)
      print 6138,(endpoint_align(2,1,l),l=1,malign)
      print 6139,(endpoint_align(3,1,l),l=1,malign)
      print 6140,(endpoint_align(1,2,l),l=1,malign)
      print 6141,(endpoint_align(2,2,l),l=1,malign)
      print 6142,(endpoint_align(3,2,l),l=1,malign)
      print 6143,(diameter_align(l),l=1,malign)
      print 6144,(thicknes_align(l),l=1,malign)
6137  FORMAT('CO_MAT_ALIGN X1',20F15.3)
6138  FORMAT('CO_MAT_ALIGN Y1',20F15.3)
6139  FORMAT('CO_MAT_ALIGN Z1',20F15.3)
6140  FORMAT('CO_MAT_ALIGN X2',20F15.3)
6141  FORMAT('CO_MAT_ALIGN Y2',20F15.3)
6142  FORMAT('CO_MAT_ALIGN Z2',20F15.3)
6143  FORMAT('CO_MAT_ALIGN B1',20F15.3)
6144  FORMAT('CO_MAT_ALIGN B2',20F15.3)
!
!--- COMAMA
      print 7000,                                         &
       matcry   ,matcon   ,matvou   ,matatt  ,mathea   ,  &
       matrod   ,matlat   ,                               &
       matstr   ,matann   ,matecr   ,mateco  ,matepl   ,  &
       matfee   ,matbar   ,                               &
       matrai   ,matgird  ,matcalcr ,matfeed ,matcoc   ,  &
       matpsbar
      print 7002,                                         &
       matacord1,matacord2,matacord3,matsuri ,mateccr  ,  &
       matfeec  ,matecel  ,matecelps,matecelc,matecelpg,  &
       matechad1,matechad2,matecf1  ,matecf2 ,            &
       mattil   ,matsole  ,matpixr  ,matpixz
      print 7004,                                         &
       matsctr  ,matsctz  ,matsctyp ,                     &
       mattrtr  ,mattrtz1 ,mattrtz2 ,mattrtz3,            &
       matetu
      print 7006,(matshi(l),l=1,mshie)
      do ima=1,mamama
        print 7008,ima,x0mama(ima),eamama(ima),ebmama(ima)
      enddo
7000  FORMAT('COMAMA A1',20I5)
7002  FORMAT('COMAMA A2',18I5)
7004  FORMAT('COMAMA A3', 8I5)
7006  FORMAT('COMAMA B ',20I5)
7008  FORMAT('COMAMA C1',I5,3F15.3)
!
 END SUBROUTINE BIGAMDCDUMP
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE BIGAMDCDUMP2
 IMPLICIT REAL(8) (A-H,O-Z)
 INTEGER, EXTERNAL :: tF1GetIAmdcVar
 REAL(8), EXTERNAL :: tF1GetRAmdcVar
 character(32) :: CHA32BID
 logical, save :: first=.true.
 integer, save :: Lun_Dump=43
       
      if (first) then
        first=.false.
        Open(unit=Lun_Dump, file='Out.AmdcDump.BIGAMDCDUMP2', status='unknown')
             
      WRITE(Lun_Dump,7701)
7701  FORMAT('BIGAMDCDUMP2 ')
      WRITE(Lun_Dump,7702) tF1GetIAmdcVar( 'MGEO', 0,0,0)
7702  FORMAT('MGEO ',I10)
      WRITE(Lun_Dump,7703) tF1GetIAmdcVar( 'MPHI', 0,0,0)
7703  FORMAT('MPHI ',I10)

!
!-- CHAPOS
      mtyp_loc  = tF1GetIAmdcVar( 'MTYP', 0,0,0)
      do jtyp=1,mtyp_loc
       do jff=1,8
        mzz_loc  = tF1GetIAmdcVar( 'MZZ', 0,0,0)
        do jzz=-mzz_loc,mzz_loc
         jgeo = tF1GetIAmdcVar( 'IGEO', jtyp,jff,jzz)
         if(jgeo.gt.0) then
          WRITE(Lun_Dump,3010)                           &
           jtyp,jff,jzz,                                 &
           tF1GetRAmdcVar( 'Pos_Phi'  , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_Z'    , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_R'    , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_S'    , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_Alfa' , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_Beta' , jtyp,jff,jzz) ,  &
           tF1GetRAmdcVar( 'Pos_Gama' , jtyp,jff,jzz) ,  &
           tF1GetIAmdcVar( 'IBAREL'   , jtyp,jff,jzz)
          WRITE(Lun_Dump,3012)                           &
           tF1GetIAmdcVar( 'IGEO', jtyp,jff,jzz),        &
           tF1GetIAmdcVar( 'ICUT', jtyp,jff,jzz)
!---- CHAGEO
          jgeo=tF1GetIAmdcVar( 'IGEO', jtyp,jff,jzz)
          nobj_loc = tF1GetIAmdcVar( 'NOBJ', jtyp,jgeo,0)
          do  job=1,nobj_loc
           WRITE(Lun_Dump,1010)                          &
            job,                                         &
            tF1GetRAmdcVar( 'Geo_dx' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_dy' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_dz' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_Ws' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_Wl' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_Le' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_Ex' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_D1' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_D2' , jtyp,jgeo,job) ,  &
            tF1GetRAmdcVar( 'Geo_D3' , jtyp,jgeo,job) ,  &
            tF1GetIAmdcVar( 'ITEC'   , jtyp,jgeo,job) ,  &
            tF1GetIAmdcVar( 'ISTA'   , jtyp,jgeo,job) ,  &
            tF1GetIAmdcVar( 'ISPLIX' , jtyp,jgeo,job) ,  &
            tF1GetIAmdcVar( 'ISPLIY' , jtyp,jgeo,job) ,  &
            tF1GetIAmdcVar( 'ISHAPE' , jtyp,jgeo,job)
          enddo
!---- CHACUT
          jcut=tF1GetIAmdcVar( 'ICUT', jtyp,jff,jzz)
          if (jcut.ge.1) then
           itotcu = tF1GetIAmdcVar( 'INOCUT',jtyp,jgeo,jcut)
           nobcut_loc = tF1GetIAmdcVar( 'NOBCUT',itotcu,0,0)
           do job=1,nobcut_loc
            WRITE(Lun_Dump,2010)                         &
             job,                                        &
             tF1GetRAmdcVar( 'Cut_dx', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_dy', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_Ws', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_Wl', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_Le', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_Ex', itotcu,job,0) ,   &
             tF1GetRAmdcVar( 'Cut_An', itotcu,job,0) ,   &
             tF1GetIAmdcVar( 'IOBCUT', itotcu,job,0)
           enddo
          endif
         endif
        enddo
       enddo
      enddo
3010  FORMAT('CHAPOS ',  &
       I5   ,'JTYP',     &
       I5   ,'JFF ',     &
       I5   ,'JZZ ',     &
       F15.3,'PHI ',     &
       F15.3,'Z   ',     &
       F15.3,'R   ',     &
       F15.3,'S   ',     &
       F15.3,'AL  ',     &
       F15.3,'BE  ',     &
       F15.3,'GA  ',     &
       I5   ,'IB  ')
3012  FORMAT('!!igeo,icut ',2I5)
!
1010  FORMAT('!!CHAGEO ',  &
       I5   ,'JOB  ',      &
       F15.3,'DX   ',      &
       F15.3,'DY   ',      &
       F15.3,'DZ   ',      &
       F15.3,'WS   ',      &
       F15.3,'WL   ',      &
       F15.3,'LE   ',      &
       F15.3,'EX   ',      &
       F15.3,'D1   ',      &
       F15.3,'D2   ',      &
       F15.3,'D3   ',      &
       I5   ,'ITEC ',      &
       I5   ,'ISTA ',      &
       I5   ,'ISPX ',      &
       I5   ,'ISPY ',      &
       I5   ,'ISHA ')
!
2010  FORMAT('!!CHACUT ',  &
       I5   ,'JOB ',       &
       F15.3,'DX  ',       &
       F15.3,'DY  ',       &
       F15.3,'WS  ',       &
       F15.3,'WL  ',       &
       F15.3,'LE  ',       &
       F15.3,'EX  ',       &
       F15.3,'AN  ',       &
       I5   ,'IOC ')
!
!-- CHACAR
      do jtyp=1,mtyp_loc
       call tF1GetCAmdcVar( 'CARTYP', jtyp,0,0, CHA32BID )
       WRITE(Lun_Dump,4010) jtyp,CHA32BID(1:3)
      enddo
4010  FORMAT('CHACAR ',I5,'JTYP   ',A3)
      mtec_loc = tF1GetIAmdcVar( 'MTEC', 0,0,0 )
      do jtec=1,mtec_loc
       call tF1GetCAmdcVar( 'CARTEC', jtec,0,0, CHA32BID )
       WRITE(Lun_Dump,4020) jtec,CHA32BID(1:3)
      enddo
4020  FORMAT('CHACAR ',I5,'JTEC   ',A3)
!
!-- CHASTA
      msta_loc =  tF1GetIAmdcVar( 'MSTA', 0,0,0 )
      WRITE(Lun_Dump,5000)
      do jtec=1,mtec_loc
       do jsta=1,msta_loc
        nll=tF1GetIAmdcVar('NLAZ',jtec,jsta,0) + tF1GetIAmdcVar('NLAS',jtec,jsta,0)
        if(nll.gt.0) then
         WRITE(Lun_Dump,5010)        jtec,jsta,      &
          tF1GetRAmdcVar( 'STAX0'  , jtec,jsta,0) ,  &
          tF1GetRAmdcVar( 'STAPP'  , jtec,jsta,0) ,  &
          tF1GetRAmdcVar( 'STARR'  , jtec,jsta,0) ,  &
          tF1GetRAmdcVar( 'STAEE'  , jtec,jsta,0) ,  &
          tF1GetRAmdcVar( 'STAPG'  , jtec,jsta,0) ,  &
          tF1GetIAmdcVar( 'NLAZ'   , jtec,jsta,0) ,  &
          tF1GetIAmdcVar( 'NLAS'   , jtec,jsta,0) ,  &
          tF1GetIAmdcVar( 'IFORMA' , jtec,jsta,0)
!
         WRITE(Lun_Dump,5022) jtec,jsta,(tF1GetRAmdcVar( 'STATT', jtec,jsta,l),l=1,20)
         WRITE(Lun_Dump,5024) jtec,jsta,(tF1GetRAmdcVar( 'STAOO', jtec,jsta,l),l=1,20)
         WRITE(Lun_Dump,5026) jtec,jsta,(tF1GetIAmdcVar('ISTAMA', jtec,jsta,l),l=1,20)
!
        endif
       enddo
      enddo
5000  FORMAT('CHASTA ',          &
            ' JTEC',             &
            ' JSTA',             &
            '          STAX0',   &
            '          STAPP',   &
            '          STARR',   &
            '          STAEE',   &
            '          STAPG',   &
            '           NLAZ',   &
            '           NLAS',   &
            '         IFORMA')
5010  FORMAT('CHASTA ',2I5,5F15.3,3I15)
5022  FORMAT('CHASTA STATT  ',2I5,20F15.3)
5024  FORMAT('CHASTA STAOO  ',2I5,20F15.3)
5026  FORMAT('CHASTA ISTAMA ',2I5,20I15)
!
        close( unit=Lun_Dump )
      endif
      
 END SUBROUTINE BIGAMDCDUMP2
!
