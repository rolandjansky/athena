SET echo OFF;
set linesize 132;
drop   table AECT_data2tag cascade constraints;
drop   table AECT_data cascade constraints;
create table AECT_data (
  AECT_data_id   number(10),
CRYOT1       float(63),
CRYOT2       float(63),
CRYOS1       float(63),
CRYOS2       float(63),
CRYOEDGE       float(63),
CRYOFLTY       float(63),
CRYOR0       float(63),
CRYOZMIN       float(63),
CRYOZEXT       float(63),
CRYOTHI1       float(63),
CRYOTHI2       float(63),
CRYOTHI3       float(63),
CRYOTTU0       float(63),
CRYORTU0       float(63),
CRYORTU1       float(63),
CRYODPHT       float(63),
CRYOSTL0       float(63),
CRYOSTL1       float(63),
CRYOSTH0       float(63),
CRYOSTW0       float(63),
CRYOSTW1       float(63),
CRYOSTTH       float(63),
CRYOSTH1       float(63),
CRYOSTH2       float(63),
CRYOSTPZ       float(63),
CRYOSUPL       float(63),
CRYOSUPH       float(63),
CRYOSUPW       float(63),
CRYOSUPX       float(63),
CNBXZMOF       float(63),
CNBXZEXT       float(63),
CNBXTMIN       float(63),
CNBXTMAX       float(63),
CNBXTHIC       float(63),
CNBXEDGE       float(63),
SUPPLZOF       float(63),
SUPPLZEX       float(63),
SUPPLTMI       float(63),
SUPPLTMA       float(63),
SUPPLTHZ       float(63),
SUPPLTHT       float(63),
SUPPLRHO       float(63),
SRVTU1DZ       float(63),
SRVTU1HE       float(63),
SRVTU1OW       float(63),
SRVTU1IW       float(63),
SRVTU1ED       float(63),
SRVTU2DZ       float(63),
SRVTU2HE       float(63),
SRVTU2OR       float(63),
SRVTU2IR       float(63),
SRVTU3DZ       float(63),
SRVTU3HE       float(63),
SIFITU       float(63),
COFITU       float(63),
CRYOZMAX       float(63),
CNBXZMIN       float(63),
CNBXZMAX       float(63),
SUPPLZMI       float(63),
SUPPLZMA       float(63),
CRYOZMOY       float(63),
CRYOALFA       float(63),
CRYOBETA       float(63),
CRYOT1P       float(63),
CRYOT2P       float(63),
CRYOS1P       float(63),
CRYOS2P       float(63),
CRYOFLYP       float(63),
SUPPLZME       float(63),
XTEMP_0       float(63),
XTEMP_1       float(63),
XTEMP_2       float(63),
YTEMP_0       float(63),
YTEMP_1       float(63),
YTEMP_2       float(63),
DTEMP01       float(63),
XTIMP_0       float(63),
XTIMP_1       float(63),
XTIMP_2       float(63),
YTIMP_0       float(63),
YTIMP_1       float(63),
YTIMP_2       float(63),
DTIMP01       float(63),
UTEMP61       float(63),
UTEMP62       float(63),
UTIMP61       float(63),
UTIMP62       float(63),
DTEMP       float(63),
SUPPLATY_0       float(63),
SUPPLATY_1       float(63),
SUPPLATY_2       float(63),
SUPPLATY_3       float(63),
SUPPLATX_0       float(63),
SUPPLATX_1       float(63),
SUPPLATX_2       float(63),
SUPPLATX_3       float(63),
X1TEMP       float(63),
X0TEMP       float(63)
) ;

alter table AECT_data add constraint AECT_data_pk
primary key (AECT_data_id);

create table AECT_data2tag (
  AECT_vers      varchar2(255),
  AECT_data_id   number(10)
) ;

alter table AECT_data2tag add constraint AECT_data2tag_pk
primary key (AECT_vers, AECT_data_id);
alter table AECT_data2tag add constraint AECT_data2tag_vers_fk
foreign key (AECT_vers) references hvs_nodevers (vers_name);
alter table AECT_data2tag add constraint AECT_data2tag_data_fk
foreign key (AECT_data_id) references AECT_data (AECT_data_id);

delete from hvs_relation where CHILD_NODENAME = 'AECT';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'AECT';
delete from hvs_node     where NODE_NAME = 'AECT';

insert into hvs_node     values ('AECT','MuonSpectrometer', '');
insert into hvs_nodevers values ('AECT', 'AECT-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','AECT','AECT-01');
insert into AECT_data (
AECT_data_id,
CRYOT1,
CRYOT2,
CRYOS1,
CRYOS2,
CRYOEDGE,
CRYOFLTY,
CRYOR0,
CRYOZMIN,
CRYOZEXT,
CRYOTHI1,
CRYOTHI2,
CRYOTHI3,
CRYOTTU0,
CRYORTU0,
CRYORTU1,
CRYODPHT,
CRYOSTL0,
CRYOSTL1,
CRYOSTH0,
CRYOSTW0,
CRYOSTW1,
CRYOSTTH,
CRYOSTH1,
CRYOSTH2,
CRYOSTPZ,
CRYOSUPL,
CRYOSUPH,
CRYOSUPW,
CRYOSUPX,
CNBXZMOF,
CNBXZEXT,
CNBXTMIN,
CNBXTMAX,
CNBXTHIC,
CNBXEDGE,
SUPPLZOF,
SUPPLZEX,
SUPPLTMI,
SUPPLTMA,
SUPPLTHZ,
SUPPLTHT,
SUPPLRHO,
SRVTU1DZ,
SRVTU1HE,
SRVTU1OW,
SRVTU1IW,
SRVTU1ED,
SRVTU2DZ,
SRVTU2HE,
SRVTU2OR,
SRVTU2IR,
SRVTU3DZ,
SRVTU3HE,
SIFITU,
COFITU,
CRYOZMAX,
CNBXZMIN,
CNBXZMAX,
SUPPLZMI,
SUPPLZMA,
CRYOZMOY,
CRYOALFA,
CRYOBETA,
CRYOT1P,
CRYOT2P,
CRYOS1P,
CRYOS2P,
CRYOFLYP,
SUPPLZME,
XTEMP_0,XTEMP_1,XTEMP_2,
YTEMP_0,YTEMP_1,YTEMP_2,
DTEMP01,
XTIMP_0,XTIMP_1,XTIMP_2,
YTIMP_0,YTIMP_1,YTIMP_2,
DTIMP01,
UTEMP61,
UTEMP62,
UTIMP61,
UTIMP62,
DTEMP,
SUPPLATY_0,SUPPLATY_1,SUPPLATY_2,SUPPLATY_3,
SUPPLATX_0,SUPPLATX_1,SUPPLATX_2,SUPPLATX_3,
X1TEMP,
X0TEMP
) values (    1,
  5350.0000,  4325.0000,  1245.0000,  1135.0000,    80.0000,  4000.0000,
   905.0000,  7894.0000,  5000.0000,    80.0000,   100.0000,    70.0000,
  3100.0000,    80.0000,   150.0000,    -3.6000,   550.0000,   400.0000,
   200.0000,  1000.0000,   650.0000,    40.0000,    40.0000,    20.0000,
   140.0000,  4500.0000,   200.0000,   500.0000,  3100.0000,    30.0000,
  4740.0000,  1110.0000,  5110.0000,   280.0000,   600.0000,   800.0000,
  3400.0000,  1420.0000,  4150.0000,    70.0000,    80.0000,   600.0000,
   830.0000,  2000.0000,  1100.0000,  1000.0000,   120.0000,   900.0000,
  1000.0000,   150.0000,   140.0000,   260.0000,  4000.0000,     0.3239,
     0.9461, 12894.0000,  7924.0000, 12664.0000,  8694.0000, 12094.0000,
 10394.0000,     1.1504,     0.7577,  5280.0000,  4255.0000,  1154.2330,
  1190.7319,  3930.0000, 10394.0000,   567.5000,  1472.2413,  2047.3563,
  4325.0000,  5180.9761,  4942.7554,  1245.4926,   595.3660,  1487.3823,
  2020.5685,  4255.0000,  5098.9370,  4878.0840,  1227.9751,     0.4980,
     0.5071,     0.5172,     0.5185,  5331.5850,  1420.0000,  1500.0000,
  4070.0000,  4150.0000,   873.2967,   939.5709,  3068.6287,  3134.9028,
  1385.9293,  1259.9357
);
insert into AECT_data2tag values ('AECT-01',    1);

comment on column AECT_data.AECT_data_id is 'Unique identifier';
comment on column AECT_data.CRYOT1     is 'CRYOT1                                          ';
comment on column AECT_data.CRYOT2     is 'CRYOT2                                          ';
comment on column AECT_data.CRYOS1     is 'CRYOS1                                          ';
comment on column AECT_data.CRYOS2     is 'CRYOS2                                          ';
comment on column AECT_data.CRYOEDGE   is 'CRYOEDGE                                        ';
comment on column AECT_data.CRYOFLTY   is 'FLATY                                           ';
comment on column AECT_data.CRYOR0     is 'CRYOR0                                          ';
comment on column AECT_data.CRYOZMIN   is 'CRYOZMIN                                        ';
comment on column AECT_data.CRYOZEXT   is 'CRYOZEXT                                        ';
comment on column AECT_data.CRYOTHI1   is 'THICK1                                          ';
comment on column AECT_data.CRYOTHI2   is 'THICK2                                          ';
comment on column AECT_data.CRYOTHI3   is 'THICK3                                          ';
comment on column AECT_data.CRYOTTU0   is 'CRYOTTU0                                        ';
comment on column AECT_data.CRYORTU0   is 'CRYORTU0                                        ';
comment on column AECT_data.CRYORTU1   is 'CRYORTU1                                        ';
comment on column AECT_data.CRYODPHT   is 'DPHITU                                          ';
comment on column AECT_data.CRYOSTL0   is 'CRYOSTOPLENGTH0                                 ';
comment on column AECT_data.CRYOSTL1   is 'CRYOSTOPLENGTH1                                 ';
comment on column AECT_data.CRYOSTH0   is 'CRYOSTOPHEIGHT0                                 ';
comment on column AECT_data.CRYOSTW0   is 'CRYOSTOPWIDTH0                                  ';
comment on column AECT_data.CRYOSTW1   is 'CRYOSTOPWIDTH1                                  ';
comment on column AECT_data.CRYOSTTH   is 'CRYOSTOPTHICKNESS                               ';
comment on column AECT_data.CRYOSTH1   is 'CRYOSTOPHEIGHT1                                 ';
comment on column AECT_data.CRYOSTH2   is 'CRYOSTOPHEIGHT2                                 ';
comment on column AECT_data.CRYOSTPZ   is 'CRYOSTOPPOSIZ                                   ';
comment on column AECT_data.CRYOSUPL   is 'CRYOSUPL                                        ';
comment on column AECT_data.CRYOSUPH   is 'CRYOSUPH                                        ';
comment on column AECT_data.CRYOSUPW   is 'CRYOSUPW                                        ';
comment on column AECT_data.CRYOSUPX   is 'CRYOSUPX                                        ';
comment on column AECT_data.CNBXZMOF   is 'CONDBOXZMINOFFSET                               ';
comment on column AECT_data.CNBXZEXT   is 'CONDBOXZEXT                                     ';
comment on column AECT_data.CNBXTMIN   is 'CONDBOXTMIN                                     ';
comment on column AECT_data.CNBXTMAX   is 'CONDBOXTMAX                                     ';
comment on column AECT_data.CNBXTHIC   is 'CONDBOXTHICK                                    ';
comment on column AECT_data.CNBXEDGE   is 'CONDBOXEDGE                                     ';
comment on column AECT_data.SUPPLZOF   is 'SUPPLATZOFFSET                                  ';
comment on column AECT_data.SUPPLZEX   is 'SUPPLATZEXT                                     ';
comment on column AECT_data.SUPPLTMI   is 'SUPPLATTMIN                                     ';
comment on column AECT_data.SUPPLTMA   is 'SUPPLATTMAX                                     ';
comment on column AECT_data.SUPPLTHZ   is 'SUPPLATTHICKZ                                   ';
comment on column AECT_data.SUPPLTHT   is 'SUPPLATTHICKT                                   ';
comment on column AECT_data.SUPPLRHO   is 'SUPPLATRHOLE                                    ';
comment on column AECT_data.SRVTU1DZ   is 'SERVTUR1DZ                                      ';
comment on column AECT_data.SRVTU1HE   is 'SERVTUR1HEIGHT                                  ';
comment on column AECT_data.SRVTU1OW   is 'SERVTUR1OUTWIDTH                                ';
comment on column AECT_data.SRVTU1IW   is 'SERVTUR1INNWIDTH                                ';
comment on column AECT_data.SRVTU1ED   is 'SERVTUR1EDGE                                    ';
comment on column AECT_data.SRVTU2DZ   is 'SERVTUR2DZ                                      ';
comment on column AECT_data.SRVTU2HE   is 'SERVTUR2HEIGHT                                  ';
comment on column AECT_data.SRVTU2OR   is 'SERVTUR2OUTRAD                                  ';
comment on column AECT_data.SRVTU2IR   is 'SERVTUR2INNRAD                                  ';
comment on column AECT_data.SRVTU3DZ   is 'SERVTUR3DZ                                      ';
comment on column AECT_data.SRVTU3HE   is 'SERVTUR3HEIGHT                                  ';
comment on column AECT_data.SIFITU     is 'SIFITU                                          ';
comment on column AECT_data.COFITU     is 'COFITU                                          ';
comment on column AECT_data.CRYOZMAX   is 'CRYOZMAX                                        ';
comment on column AECT_data.CNBXZMIN   is 'CONDBOXZMIN                                     ';
comment on column AECT_data.CNBXZMAX   is 'CONDBOXZMAX                                     ';
comment on column AECT_data.SUPPLZMI   is 'SUPPLATZMIN                                     ';
comment on column AECT_data.SUPPLZMA   is 'SUPPLATZMAX                                     ';
comment on column AECT_data.CRYOZMOY   is 'CRYOZMOY                                        ';
comment on column AECT_data.CRYOALFA   is 'CRYOALFA                                        ';
comment on column AECT_data.CRYOBETA   is 'CRYOBETA                                        ';
comment on column AECT_data.CRYOT1P    is 'CRYOT1P                                         ';
comment on column AECT_data.CRYOT2P    is 'CRYOT2P                                         ';
comment on column AECT_data.CRYOS1P    is 'CRYOS1P                                         ';
comment on column AECT_data.CRYOS2P    is 'CRYOS2P                                         ';
comment on column AECT_data.CRYOFLYP   is 'FLATYP                                          ';
comment on column AECT_data.SUPPLZME   is 'SUPPLATZMOY                                     ';
comment on column AECT_data.XTEMP_0    is 'ARRAY                                           ';
comment on column AECT_data.YTEMP_0    is 'ARRAY                                           ';
comment on column AECT_data.DTEMP01    is 'DTEMP01                                         ';
comment on column AECT_data.XTIMP_0    is 'ARRAY                                           ';
comment on column AECT_data.YTIMP_0    is 'ARRAY                                           ';
comment on column AECT_data.DTIMP01    is 'DTIMP01                                         ';
comment on column AECT_data.UTEMP61    is 'UTEMP61                                         ';
comment on column AECT_data.UTEMP62    is 'UTEMP62                                         ';
comment on column AECT_data.UTIMP61    is 'UTIMP61                                         ';
comment on column AECT_data.UTIMP62    is 'UTIMP62                                         ';
comment on column AECT_data.DTEMP      is 'DTEMP                                           ';
comment on column AECT_data.SUPPLATY_0 is 'ARRAY                                           ';
comment on column AECT_data.SUPPLATX_0 is 'ARRAY                                           ';
comment on column AECT_data.X1TEMP     is 'X1TEMP                                          ';
comment on column AECT_data.X0TEMP     is 'X0TEMP                                          ';
