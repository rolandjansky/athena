!
!
! AmdcCore/comamu.inc
!
      INTEGER, PARAMETER :: MSHIE=20, MALIGN=80
      REAL(8) :: RBAMIN,RBAMAX,ZBAMAX,RCRYO,ECRYOB,RCURV,ECOND,HCOND
      REAL(8) :: ZVOUS,XATTI,ZATTI,DYATT,ZATTA,DLBIEL,EPBIEL,RRBIEL
      REAL(8) :: EXBIEL,ZBIELI,ZBIELA,XCATTI,ZCATTI,XHEADI
      REAL(8) :: XHEADA,DYHEAD,ZLATT,DXLATT,DZLATT,DXCLAT,DZCLAT
      REAL(8) :: TVOUSI,SVOUSI,DTVOUS,DZVOUS,DTTROU,S1TROU,S2TROU
      REAL(8) :: TSTRUI,TSTRUA,DZSTRU,RANNO,TANNO,ZANNO,ZECMI,ZECMA
      REAL(8) :: TECMA1,TECMA2,SECMA1,SECMA2,RECMI,ECRYOE,ECRYOF,ECRYOR
      REAL(8) :: TECTU,RECTU,DRECTU,FECTU,ZECOMI,ZECOMA,TECOMI,TECOMA
      REAL(8) :: EPECO,RCUECO,ZECAMI,ZECAMA,EPZECA
      REAL(8) :: TECA1,TECA2,TECA3,TECA4
      REAL(8) :: DZ1ETU,DY1ETU,RO1ETU,RI1ETU,DZ2ETU,DY2ETU,RR2ETU
      REAL(8) :: DZ3ETU,DY3ETU,ZECDUMMY
      REAL(8) :: ZFEE,EP1FEE,EP2FEE,DSSFEE,DSPFEE,DD1FEE,DD2FEE
      REAL(8) :: DZ1FEE,DZ2FEE,DZ3FEE,TI1FEE,TM1FEE,TA1FEE,TI2FEE
      REAL(8) :: TA2FEE,TA3FEE,DL1FEE,DL2FEE,DX1FEE,DX2FEE,DBXFEE
      REAL(8) :: DBYFEE,DBEFEE,DZ7FEE,DL7FEE,DT7FEE
      REAL(8) :: XFEEBRI,YFEEBRI
      REAL(8) :: DX1FEEBRI,DX2FEEBRI,DX3FEEBRI,DX4FEEBRI
      REAL(8) :: DY1FEEBRI,DY2FEEBRI,DY3FEEBRI,DY4FEEBRI
      REAL(8) :: DZ1FEEBRI,DZ2FEEBRI
      REAL(8) :: DZ1FEB,DZ2FEB,EP1FEB,EP2FEB,RTRFEB
      REAL(8) :: DX1FEB,DX2FEB,DX3FEB,DX4FEB,DY1FEB,DY2FEB, RAIDUMMY
      REAL(8) :: ZRAIL,XRAIL1,YRAIL1,XRAIL2,YRAIL2,XRAIL3,YRAIL3,CHAMFR
      REAL(8) :: ZMAGIRD,ZMIGIRD,DZHGIRD
      REAL(8) :: RAOGIRD,RIOGIRD,RAIGIRD,RIIGIRD,DS1GIRD,DS2GIRD
      REAL(8) :: ZAECA1,RIECA1,RAECA1,EPECA1,EPECA2,COECA1,ZAECA2,RAECA2
      REAL(8) :: RRFEED,HHFEED,ZACOCR,ZMCOCR,EPCOCR,RICOCR,RMCOCR,RACOCR
      REAL(8) :: COCOCR,RIPSBAR,RAPSBAR,ZAPSBAR,RIACORD,RAACORD,ZAACORD
      REAL(8) :: ETACORD0,ETACORD1,ETACORD2,ETACORD3,DSAMPL11,DSAMPL12
      REAL(8) :: DSAMPL21,DSAMPL22,DSAMPL31,HHSURI,AASURI,EESURI,ZZSURI
      REAL(8) :: ZIECC1,ZIECC2,ZIECC3,ZAECC1,RIECC1,RIECC2,RIECC3,RAECC1
      REAL(8) :: RAECC2,EPECC1,EPECC2,EPECC3,RRFEEC,HHFEEC
      REAL(8) :: ZIECHAD1,ZMECHAD1,ZAECHAD1,RIECHAD,RMECHAD,RAECHAD
      REAL(8) :: ZIECHAD2,ZAECHAD2
      REAL(8) :: ZIECF1,ZAECF1,RIECF1,ZIECF2,ZAECF2,RIECF2
      REAL(8) :: ZIECF3,ZAECF3,RIECF3,ZIECF4,ZAECF4,RIECF4,RAECFN
      REAL(8) :: ECELOR,ECELIR,ECELIZ,ECELDEEP,ECELGBW,  ECELOETA1
      REAL(8) :: ECELOETA2,ECELOETA3,ECELOETA4,ECELOETA5,ECELOETA6
      REAL(8) :: ECELIETA1,ECELIETA2
      REAL(8) :: ECELRDP1,ECELRDP2,ECELRDP3,ECELRDP4,ECELRDP5
      REAL(8) :: ECELPSETA1,ECELPSETA2,ECELPSDEEP,ECELPSIZ
      REAL(8) :: ECELCVIZ1,ECELCVIZ2,ECELCVOZ1,ECELCVOZ2,ECELCVOZ3
      REAL(8) :: ECELCVIR1,ECELCVIR2,ECELCVOR1,ECELCVOR2,ECELCVOR3
      REAL(8) :: ECELPG1IR,ECELPG1OR,ECELPG1OZ,ECELPG1DP
      REAL(8) :: ECELPG2IR,ECELPG2OR,ECELPG2IZ,ECELPG2OZ,ECELPG4DP
      REAL(8) :: ZABATIL,RIBATIL,RABATIL,DRBTIL1,DRBTIL2
      REAL(8) :: ZIEXTIL,ZAEXTIL,RIEXTIL,RAEXTIL,DRETIL1,DRETIL2
      REAL(8) :: DZITIL1,DZITIL2,DRITIL1,DRITIL2
      REAL(8) :: DZITIL3,DRITIL3,DRITIL4,DRITIL5
      REAL(8) :: ZASOLE,RISOLE,RASOLE
      REAL(8) :: ZAPIXR,RIPIXR,EPPIXR,ZIPIXZ,RIPIXZ,RAPIXZ,EPPIXZ
      REAL(8) :: ZASCTR,RISCTR,EPSCTR,ZISCTZ,RISCTZ,RASCTZ,EPSCTZ
      REAL(8) :: RISCTYP,RASCTYP,DZSCTYP,EPSCTYP
      REAL(8) :: ZATRTR,ZITRTR,RITRTR,RMTRTR,RATRTR,RITRTZ1,RATRTZ1
      REAL(8) :: RNTRTR
      REAL(8) :: RITRTZ2,RATRTZ2,RITRTZ3,RATRTZ3,DZTRTZ1,DZTRTZ2,DZTRTZ3
      REAL(8) :: ZITRTZ1,ZITRTZ2,ZITRTZ3
      REAL(8) :: ZISHIE,ZASHIE,RIISHI,RIOSHI,RAISHI,RAOSHI
      REAL(8) :: EndPoint_Align,Diameter_Align,Thicknes_Align
      REAL(8) :: GIRDUMMY,BARDUMMY,ECCDUMMY,TILDUMMY,SOLEDUMMY
      CHARACTER(3) :: Name_Align
      INTEGER :: NCRYO,NCURV,NCURC,NVOUS,NLATT,NANNO,NECTU,NCUECO,NFEE
      INTEGER :: NBIEL,NNFEED,NNSURI,NNFEEC,NPIXR,NPIXZ,NSCTR,NSCTZ
      INTEGER :: NSCTYP,IISCTZ,JVETRT,NTRTZ1,NTRTZ2,NTRTZ3
      INTEGER :: NBSHIE,NCSHIE
      INTEGER :: Nb_Align_Tub,Nb_Align_Bar,Nb_Align_Axl,Lfi_Align
!
      COMMON/AMDCCO_MAT_CHARACT/                                           &
     &               Name_Align(MALIGN)
      COMMON/AMDCCO_MAT_INTEGER/                                           &
     &               IISCTZ(10),                                           &
     &               NCRYO,NCURV, NCURC, NVOUS, NLATT, NANNO,              &
     &               NECTU, NCUECO, NFEE, NBIEL, NNFEED, NNSURI,           &
     &               NNFEEC, NPIXR, NPIXZ, NSCTR, NSCTZ,                   &
     &               NSCTYP, JVETRT, NTRTZ1,NTRTZ2,NTRTZ3,                 &
     &               NBSHIE,NCSHIE(MSHIE),                                 &
     &               Nb_Align_Tub,Nb_Align_Bar,Nb_Align_Axl,               &
     &               Lfi_Align(MALIGN)
      COMMON/AMDCCO_MAT_BARREL/                                            &
     &               ZVOUS(10), ECOND,HCOND,                               &
     &               RBAMIN,RBAMAX,ZBAMAX,RCRYO,ECRYOB,RCURV,              &
     &               XATTI,ZATTI,DYATT,ZATTA,  DLBIEL,EPBIEL,RRBIEL,       &
     &               EXBIEL,ZBIELI,ZBIELA,  XCATTI,ZCATTI,                 &
     &               XHEADI,XHEADA,DYHEAD,                                 &
     &               ZLATT(10),DXLATT,DZLATT,DXCLAT,DZCLAT,                &
     &               TVOUSI,SVOUSI,DTVOUS,DZVOUS,DTTROU,S1TROU,S2TROU,     &
     &               TSTRUI,TSTRUA,DZSTRU,  RANNO,TANNO,ZANNO
      COMMON/AMDCCO_MAT_ENDCAP/                                            &
     &               ZECDUMMY(2),                                          &
     &               ZECMI,ZECMA,TECMA1,TECMA2,SECMA1,SECMA2,RECMI,        &
     &               ECRYOE,ECRYOF,ECRYOR,  TECTU,RECTU,DRECTU,FECTU,      &
     &               ZECOMI,ZECOMA,TECOMI,TECOMA,EPECO,RCUECO,             &
     &               ZECAMI,ZECAMA,EPZECA,TECA1,TECA2,TECA3,TECA4,         &
     &               DZ1ETU,DY1ETU,RO1ETU,RI1ETU,DZ2ETU,DY2ETU,RR2ETU,     &
     &               DZ3ETU,DY3ETU
      COMMON/AMDCCO_MAT_FEET/                                              &
     &               ZFEE(10),  EP1FEE,EP2FEE,  DSSFEE,DSPFEE,             &
     &               DD1FEE,DD2FEE, DZ1FEE,DZ2FEE,DZ3FEE,                  &
     &               TI1FEE,TM1FEE,TA1FEE, TI2FEE,TA2FEE, TA3FEE,          &
     &               DL1FEE,DL2FEE,DX1FEE,DX2FEE,                          &
     &               DBXFEE,DBYFEE,DBEFEE,  DZ7FEE,DL7FEE,DT7FEE,          &
     &               XFEEBRI,YFEEBRI,                                      &
     &               DX1FEEBRI,DX2FEEBRI,DX3FEEBRI,DX4FEEBRI,              &
     &               DY1FEEBRI,DY2FEEBRI,DY3FEEBRI,DY4FEEBRI,              &
     &               DZ1FEEBRI,DZ2FEEBRI,                                  &
     &               DZ1FEB,DZ2FEB, EP1FEB,EP2FEB, RTRFEB,                 &
     &               DX1FEB,DX2FEB,DX3FEB,DX4FEB, DY1FEB,DY2FEB
      COMMON/AMDCCO_MAT_RAIL/                                              &
     &               RAIDUMMY(2),                                          &
     &               ZRAIL,XRAIL1,YRAIL1,XRAIL2,YRAIL2,XRAIL3,YRAIL3,      &
     &               CHAMFR
      COMMON/AMDCCO_MAT_GIRDER/                                            &
     &               GIRDUMMY(2),                                          &
     &               ZMAGIRD,ZMIGIRD,DZHGIRD,                              &
     &               RAOGIRD,RIOGIRD,RAIGIRD,RIIGIRD,DS1GIRD,DS2GIRD
      COMMON/AMDCCO_MAT_ECAL_BAR/                                          &
     &               BARDUMMY(2),                                          &
     &               ZAECA1,RIECA1,RAECA1,EPECA1,                          &
     &               EPECA2,COECA1,ZAECA2,RAECA2, RRFEED,HHFEED,           &
     &               ZACOCR,ZMCOCR,EPCOCR,RICOCR,RMCOCR,RACOCR,COCOCR,     &
     &               RIPSBAR,RAPSBAR,ZAPSBAR,RIACORD,RAACORD,ZAACORD,      &
     &               ETACORD0,ETACORD1,ETACORD2,ETACORD3,                  &
     &               DSAMPL11,DSAMPL12,DSAMPL21,DSAMPL22,DSAMPL31,         &
     &               HHSURI,AASURI,EESURI,ZZSURI(10)
      COMMON/AMDCCO_MAT_CALO_END/                                          &
     &               ECCDUMMY(2),                                          &
     &               ZIECC1,ZIECC2,ZIECC3,ZAECC1,RIECC1,RIECC2,RIECC3,     &
     &               RAECC1,RAECC2,EPECC1,EPECC2,EPECC3,RRFEEC,HHFEEC,     &
     &               ZIECHAD1,ZMECHAD1,ZAECHAD1,RIECHAD,RMECHAD,           &
     &               RAECHAD,ZIECHAD2,ZAECHAD2,                            &
     &               ZIECF1,ZAECF1,RIECF1,ZIECF2,ZAECF2,RIECF2,            &
     &               ZIECF3,ZAECF3,RIECF3,ZIECF4,ZAECF4,RIECF4,RAECFN,     &
     &               ECELOR,ECELIR,ECELIZ,ECELDEEP,ECELGBW,  ECELOETA1,    &
     &               ECELOETA2,ECELOETA3,ECELOETA4,ECELOETA5,ECELOETA6,    &
     &               ECELIETA1,ECELIETA2,                                  &
     &               ECELRDP1,ECELRDP2,ECELRDP3,ECELRDP4,ECELRDP5,         &
     &               ECELPSETA1,ECELPSETA2,ECELPSDEEP,ECELPSIZ,            &
     &               ECELCVIZ1,ECELCVIZ2,ECELCVOZ1,ECELCVOZ2,ECELCVOZ3,    &
     &               ECELCVIR1,ECELCVIR2,ECELCVOR1,ECELCVOR2,ECELCVOR3,    &
     &               ECELPG1IR,ECELPG1OR,ECELPG1OZ,ECELPG1DP,              &
     &               ECELPG2IR,ECELPG2OR,ECELPG2IZ,ECELPG2OZ,ECELPG4DP
      COMMON/AMDCCO_MAT_TILE_CAL/                                          &
     &               TILDUMMY(2),                                          &
     &               ZABATIL,RIBATIL,RABATIL,DRBTIL1,DRBTIL2,              &
     &               ZIEXTIL,ZAEXTIL,RIEXTIL,RAEXTIL,DRETIL1,DRETIL2,      &
     &               DZITIL1,DZITIL2,DRITIL1,DRITIL2,                      &
     &               DZITIL3,DRITIL3,DRITIL4,DRITIL5
      COMMON/AMDCCO_MAT_SOLENO/                                            &
     &               SOLEDUMMY(2),                                         &
     &               ZASOLE,RISOLE,RASOLE
      COMMON/AMDCCO_MAT_PIXELS/                                            &
     &               ZAPIXR(5),RIPIXR(5),EPPIXR,                           &
     &               ZIPIXZ(5),RIPIXZ(5),RAPIXZ(5),EPPIXZ
      COMMON/AMDCCO_MAT_SCTS/                                              &
     &               ZISCTZ(10),RISCTZ(10),RASCTZ,EPSCTZ,                  &
     &               ZASCTR,RISCTR(5),EPSCTR,                              &
     &               RISCTYP(5),RASCTYP(5),DZSCTYP(5),EPSCTYP
      COMMON/AMDCCO_MAT_TRTS/                                              &
     &               ZITRTZ1(12),ZITRTZ2(10),ZITRTZ3(10),                  &
     &               ZITRTR,ZATRTR,RITRTR,RMTRTR,RATRTR,RNTRTR,            &
     &               DZTRTZ1,DZTRTZ2,DZTRTZ3,                              &
     &               RITRTZ1,RATRTZ1,RITRTZ2,RATRTZ2,RITRTZ3,RATRTZ3 
      COMMON/AMDCCO_MAT_SHIELD/                                            &
     &               ZISHIE(MSHIE),ZASHIE(MSHIE),                          &
     &               RIISHI(MSHIE),RIOSHI(MSHIE),                          &
     &               RAISHI(MSHIE),RAOSHI(MSHIE)
      COMMON/AMDCCO_MAT_ALIGN/                                             &
     &               EndPoint_Align(3,2,MALIGN),                           &
     &               Diameter_Align(MALIGN),Thicknes_Align(MALIGN)
!
! old common which was in comama
!
      INTEGER :: MATDUMMY,MATCRY,MATCON,MATVOU,MATATT,MATHEA,MATROD
      INTEGER :: MATLAT,MATSTR,MATANN,MATECR,MATECO,MATEPL,MATFEE,MATBAR
      INTEGER :: MATRAI,MATGIRD,MATCALCR,MATFEED,MATCOC,MATPSBAR
      INTEGER :: MATACORD1,MATACORD2,MATACORD3,MATSURI,MATECCR,MATFEEC
      INTEGER :: MATECEL,MATECELPS,MATECELC,MATECELPG
      INTEGER :: MATECHAD1,MATECHAD2,MATECF1,MATECF2,MATTIL
      INTEGER :: MATSOLE,MATPIXR,MATPIXZ,MATSCTR,MATSCTZ,MATSCTYP
      INTEGER :: MATTRTR,MATTRTZ1,MATTRTZ2,MATTRTZ3,MATETU, MATSHI
      INTEGER :: MATALI1,MATALI2,MATALI3
!
      COMMON/AMDCCO_MAT_MAMA/                                              &
     &  MATDUMMY(2), MATCRY,MATCON,MATVOU,MATATT,MATHEA,MATROD,MATLAT,     &
     &               MATSTR,MATANN,MATECR,MATECO,MATEPL,MATFEE,MATBAR,     &
     &               MATRAI,MATGIRD,MATCALCR,MATFEED,MATCOC,MATPSBAR,      &
     &               MATACORD1,MATACORD2,MATACORD3,MATSURI,MATECCR,        &
     &               MATFEEC,MATECEL,MATECELPS,MATECELC,MATECELPG,         &
     &               MATECHAD1,MATECHAD2,MATECF1,MATECF2,                  &
     &               MATTIL, MATSOLE, MATPIXR,MATPIXZ,                     &
     &               MATSCTR,MATSCTZ,MATSCTYP,                             &
     &               MATTRTR,MATTRTZ1,MATTRTZ2,MATTRTZ3,                   &
     &               MATETU, MATSHI(MSHIE), MATALI1,MATALI2,MATALI3
!
