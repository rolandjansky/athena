SET echo OFF;
set linesize 132;
drop   table JDSH_data2tag cascade constraints;
drop   table JDSH_data cascade constraints;
create table JDSH_data (
  JDSH_data_id   number(10),
XYZREFPT       float(63),
ROTA180D       float(63),
NOMIZPOS       float(63),
FDINNRAD       float(63),
FDOUTRAD       float(63),
FDTHICKN       float(63),
BDINNRAD       float(63),
BDOUTRAD       float(63),
BDTHICKN       float(63),
CABCINRA       float(63),
CABCOURA       float(63),
CABCTHI1       float(63),
CABCTHIC       float(63),
CABCSLAN       float(63),
CABCSLLE       float(63),
CALCTHIC       float(63),
CALCLENG       float(63),
CALCWIDT       float(63),
CAPCTHIC       float(63),
CAPCLENG       float(63),
CAPCWIDT       float(63),
CACOPRAN       float(63),
CACOANGZ_0       float(63),
CACOANGZ_1       float(63),
CACOANGZ_2       float(63),
CACOANGZ_3       float(63),
CACOANGZ_4       float(63),
CACOANGZ_5       float(63),
CACOANGZ_6       float(63),
CACOANGZ_7       float(63),
CACOANGZ_8       float(63),
CACOANGZ_9       float(63),
CACOANGZ_10       float(63),
CACOANGZ_11       float(63),
CACOANGZ_12       float(63),
CACOANGZ_13       float(63),
CACOANGZ_14       float(63),
CACOANGZ_15       float(63),
CACOANGX       float(63),
TUBEINRA       float(63),
TUBEOUR1       float(63),
TUBEOUR2       float(63),
TUBESLEN       float(63),
TUBELENG       float(63),
PLUGINR1       float(63),
PLUGOURA       float(63),
PLUGLENG       float(63),
PLUGTIAN       float(63),
SWHINNRA       float(63),
SWHOUTR1       float(63),
SWHOUTR2       float(63),
SWHLENGT       float(63),
FDZPOSIT       float(63),
BDZPOSIT       float(63),
CAZPOSIT       float(63),
TUBEZPOS       float(63),
PLUGZPOS       float(63),
SWHZPOSI       float(63),
CABCOUR1       float(63),
CABCTHI2       float(63),
CABCOUR2       float(63),
LCAXBPOS       float(63),
LCAYBPOS       float(63),
LCAXAPOS       float(63),
LCAYAPOS       float(63),
PCAXBPOS       float(63),
PCAYBPOS       float(63),
PCAXAPOS       float(63),
PCAYAPOS       float(63),
CALCRADI       float(63),
CALCPOSZ       float(63),
CAPCRADI       float(63),
CAPCPOSZ       float(63),
I1       float(63),
LCAPX_0       float(63),
LCAPX_1       float(63),
LCAPX_2       float(63),
LCAPX_3       float(63),
LCAPX_4       float(63),
LCAPX_5       float(63),
LCAPX_6       float(63),
LCAPX_7       float(63),
LCAPX_8       float(63),
LCAPX_9       float(63),
LCAPX_10       float(63),
LCAPX_11       float(63),
LCAPX_12       float(63),
LCAPX_13       float(63),
LCAPX_14       float(63),
LCAPX_15       float(63),
LCAPY_0       float(63),
LCAPY_1       float(63),
LCAPY_2       float(63),
LCAPY_3       float(63),
LCAPY_4       float(63),
LCAPY_5       float(63),
LCAPY_6       float(63),
LCAPY_7       float(63),
LCAPY_8       float(63),
LCAPY_9       float(63),
LCAPY_10       float(63),
LCAPY_11       float(63),
LCAPY_12       float(63),
LCAPY_13       float(63),
LCAPY_14       float(63),
LCAPY_15       float(63),
I2       float(63),
PCAPX_0       float(63),
PCAPX_1       float(63),
PCAPX_2       float(63),
PCAPX_3       float(63),
PCAPX_4       float(63),
PCAPX_5       float(63),
PCAPX_6       float(63),
PCAPX_7       float(63),
PCAPX_8       float(63),
PCAPX_9       float(63),
PCAPX_10       float(63),
PCAPX_11       float(63),
PCAPX_12       float(63),
PCAPX_13       float(63),
PCAPX_14       float(63),
PCAPX_15       float(63),
PCAPY_0       float(63),
PCAPY_1       float(63),
PCAPY_2       float(63),
PCAPY_3       float(63),
PCAPY_4       float(63),
PCAPY_5       float(63),
PCAPY_6       float(63),
PCAPY_7       float(63),
PCAPY_8       float(63),
PCAPY_9       float(63),
PCAPY_10       float(63),
PCAPY_11       float(63),
PCAPY_12       float(63),
PCAPY_13       float(63),
PCAPY_14       float(63),
PCAPY_15       float(63),
PLUGINR2       float(63)
) ;

alter table JDSH_data add constraint JDSH_data_pk
primary key (JDSH_data_id);

create table JDSH_data2tag (
  JDSH_vers      varchar2(255),
  JDSH_data_id   number(10)
) ;

alter table JDSH_data2tag add constraint JDSH_data2tag_pk
primary key (JDSH_vers, JDSH_data_id);
alter table JDSH_data2tag add constraint JDSH_data2tag_vers_fk
foreign key (JDSH_vers) references hvs_nodevers (vers_name);
alter table JDSH_data2tag add constraint JDSH_data2tag_data_fk
foreign key (JDSH_data_id) references JDSH_data (JDSH_data_id);

delete from hvs_relation where CHILD_NODENAME = 'JDSH';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'JDSH';
delete from hvs_node     where NODE_NAME = 'JDSH';

insert into hvs_node     values ('JDSH','MuonSpectrometer', '');
insert into hvs_nodevers values ('JDSH', 'JDSH-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','JDSH','JDSH-01');
insert into JDSH_data (
JDSH_data_id,
XYZREFPT,
ROTA180D,
NOMIZPOS,
FDINNRAD,
FDOUTRAD,
FDTHICKN,
BDINNRAD,
BDOUTRAD,
BDTHICKN,
CABCINRA,
CABCOURA,
CABCTHI1,
CABCTHIC,
CABCSLAN,
CABCSLLE,
CALCTHIC,
CALCLENG,
CALCWIDT,
CAPCTHIC,
CAPCLENG,
CAPCWIDT,
CACOPRAN,
CACOANGZ_0,CACOANGZ_1,CACOANGZ_2,CACOANGZ_3,CACOANGZ_4,CACOANGZ_5,CACOANGZ_6,
CACOANGZ_7,CACOANGZ_8,CACOANGZ_9,CACOANGZ_10,CACOANGZ_11,CACOANGZ_12,
CACOANGZ_13,CACOANGZ_14,CACOANGZ_15,
CACOANGX,
TUBEINRA,
TUBEOUR1,
TUBEOUR2,
TUBESLEN,
TUBELENG,
PLUGINR1,
PLUGOURA,
PLUGLENG,
PLUGTIAN,
SWHINNRA,
SWHOUTR1,
SWHOUTR2,
SWHLENGT,
FDZPOSIT,
BDZPOSIT,
CAZPOSIT,
TUBEZPOS,
PLUGZPOS,
SWHZPOSI,
CABCOUR1,
CABCTHI2,
CABCOUR2,
LCAXBPOS,
LCAYBPOS,
LCAXAPOS,
LCAYAPOS,
PCAXBPOS,
PCAYBPOS,
PCAXAPOS,
PCAYAPOS,
CALCRADI,
CALCPOSZ,
CAPCRADI,
CAPCPOSZ,
I1,
LCAPX_0,LCAPX_1,LCAPX_2,LCAPX_3,LCAPX_4,LCAPX_5,LCAPX_6,LCAPX_7,LCAPX_8,LCAPX_9,
LCAPX_10,LCAPX_11,LCAPX_12,LCAPX_13,LCAPX_14,LCAPX_15,
LCAPY_0,LCAPY_1,LCAPY_2,LCAPY_3,LCAPY_4,LCAPY_5,LCAPY_6,LCAPY_7,LCAPY_8,LCAPY_9,
LCAPY_10,LCAPY_11,LCAPY_12,LCAPY_13,LCAPY_14,LCAPY_15,
I2,
PCAPX_0,PCAPX_1,PCAPX_2,PCAPX_3,PCAPX_4,PCAPX_5,PCAPX_6,PCAPX_7,PCAPX_8,PCAPX_9,
PCAPX_10,PCAPX_11,PCAPX_12,PCAPX_13,PCAPX_14,PCAPX_15,
PCAPY_0,PCAPY_1,PCAPY_2,PCAPY_3,PCAPY_4,PCAPY_5,PCAPY_6,PCAPY_7,PCAPY_8,PCAPY_9,
PCAPY_10,PCAPY_11,PCAPY_12,PCAPY_13,PCAPY_14,PCAPY_15,
PLUGINR2
) values (    1,
     0.0000,   180.0000,  6779.0000,   750.0000,  2700.0000,    50.0000,
   540.0000,  4446.0000,    80.0000,   530.0000,  1600.0000,     5.0000,
   260.0000,    10.0000,   900.0000,    30.0000,   888.0000,   629.0000,
    70.0000,   900.0000,   634.0000,    11.2500,   -90.0000,    67.5000,
   -45.0000,   -22.5000,     0.0000,    22.5000,    45.0000,    67.5000,
    90.0000,   112.5000,   135.0000,   157.5000,   180.0000,  -157.5000,
  -135.0000,  -112.5000,   -10.0000,   430.0000,   730.0000,   530.0000,
    50.0000,  2105.0000,   129.0000,   428.0000,  2105.0000,     1.1000,
   540.0000,   764.0000,   850.0000,   700.0000,    25.0000,    90.0000,
   130.0000,     0.0000,     0.0000,   390.0000,   713.6730,   161.2834,
   731.0794,   314.5000,   888.0000,   137.8658,     0.0000,   317.0000,
   900.0000,   137.9789,     0.0000,   728.4747,   245.2279,   719.7923,
   195.9875,   728.4747,   728.4747,   673.0229,   515.1094,   278.7752,
     0.0000,  -278.7752,  -515.1094,  -673.0229,  -728.4747,  -673.0229,
  -515.1094,  -278.7752,     0.0000,   278.7752,   515.1094,   673.0229,
     0.0000,   278.7752,   515.1094,   673.0229,   728.4747,   673.0229,
   515.1094,   278.7752,     0.0000,  -278.7752,  -515.1094,  -673.0229,
  -728.4747,  -673.0229,  -515.1094,  -278.7752,   719.7923,   719.7923,
   665.0013,   508.9700,   275.4526,     0.0000,  -275.4526,  -508.9700,
  -665.0013,  -719.7923,  -665.0013,  -508.9700,  -275.4526,     0.0000,
   275.4526,   508.9700,   665.0013,     0.0000,   275.4526,   508.9700,
   665.0013,   719.7923,   665.0013,   508.9700,   275.4526,     0.0000,
  -275.4526,  -508.9700,  -665.0013,  -719.7923,  -665.0013,  -508.9700,
  -275.4526,   169.4181
);
insert into JDSH_data2tag values ('JDSH-01',    1);

comment on column JDSH_data.JDSH_data_id is 'Unique identifier';
comment on column JDSH_data.XYZREFPT   is 'XYZREFERENCEPOINT                               ';
comment on column JDSH_data.ROTA180D   is '180DEGROTATION                                  ';
comment on column JDSH_data.NOMIZPOS   is 'NOMINAL_ZPOS                                    ';
comment on column JDSH_data.FDINNRAD   is 'FRONTDISK_INNERRADIUS                           ';
comment on column JDSH_data.FDOUTRAD   is 'FRONTDISK_OUTERRADIUS                           ';
comment on column JDSH_data.FDTHICKN   is 'FRONTDISK_THICKNESS                             ';
comment on column JDSH_data.BDINNRAD   is 'BACKDISK_INNERRADIUS                            ';
comment on column JDSH_data.BDOUTRAD   is 'BACKDISK_OUTERRADIUS                            ';
comment on column JDSH_data.BDTHICKN   is 'BACKDISK_THICKNESS                              ';
comment on column JDSH_data.CABCINRA   is 'BRASSCONE_INNERRADIUS                           ';
comment on column JDSH_data.CABCOURA   is 'BRASSCONE_OUTERRADIUS                           ';
comment on column JDSH_data.CABCTHI1   is 'BRASSCONE_THICKNESS_INTER1                      ';
comment on column JDSH_data.CABCTHIC   is 'BRASSCONE_THICKNESS                             ';
comment on column JDSH_data.CABCSLAN   is 'BRASSCONE_SLOPEANGLE                            ';
comment on column JDSH_data.CABCSLLE   is 'BRASSCONE_SLOPELENGTH                           ';
comment on column JDSH_data.CALCTHIC   is 'LEADCONE_THICKNESS                              ';
comment on column JDSH_data.CALCLENG   is 'LEADCONE_LENGTH                                 ';
comment on column JDSH_data.CALCWIDT   is 'LEADCONE_WIDTH                                  ';
comment on column JDSH_data.CAPCTHIC   is 'POLYCONE_THICKNESS                              ';
comment on column JDSH_data.CAPCLENG   is 'POLYCONE_LENGTH                                 ';
comment on column JDSH_data.CAPCWIDT   is 'POLYCONE_WIDTH                                  ';
comment on column JDSH_data.CACOPRAN   is 'CONE_PROJECTIVEANGLE                            ';
comment on column JDSH_data.CACOANGZ_0 is 'ARRAY                                           ';
comment on column JDSH_data.CACOANGX   is 'CONEASSEMBLY_ANGX                               ';
comment on column JDSH_data.TUBEINRA   is 'TUBE_INNERRADIUS                                ';
comment on column JDSH_data.TUBEOUR1   is 'TUBE_OUTERRADIUS1                               ';
comment on column JDSH_data.TUBEOUR2   is 'TUBE_OUTERRADIUS2                               ';
comment on column JDSH_data.TUBESLEN   is 'TUBE_SHORTLENGTH                                ';
comment on column JDSH_data.TUBELENG   is 'TUBE_LENGTH                                     ';
comment on column JDSH_data.PLUGINR1   is 'PLUG_INNERRADIUS1                               ';
comment on column JDSH_data.PLUGOURA   is 'PLUG_OUTERRADIUS                                ';
comment on column JDSH_data.PLUGLENG   is 'PLUG_LENGTH                                     ';
comment on column JDSH_data.PLUGTIAN   is 'PLUG_TILTANGLE                                  ';
comment on column JDSH_data.SWHINNRA   is 'SMALLWHEELHUB_INNERRADIUS                       ';
comment on column JDSH_data.SWHOUTR1   is 'SMALLWHEELHUB_OUTERRADIUS1                      ';
comment on column JDSH_data.SWHOUTR2   is 'SMALLWHEELHUB_OUTERRADIUS2                      ';
comment on column JDSH_data.SWHLENGT   is 'SMALLWHEELHUB_LENGTH                            ';
comment on column JDSH_data.FDZPOSIT   is 'FRONTDISK_Z                                     ';
comment on column JDSH_data.BDZPOSIT   is 'BACKDISK_Z                                      ';
comment on column JDSH_data.CAZPOSIT   is 'CONEASSEMBLY_Z                                  ';
comment on column JDSH_data.TUBEZPOS   is 'TUBE_Z                                          ';
comment on column JDSH_data.PLUGZPOS   is 'PLUG_Z                                          ';
comment on column JDSH_data.SWHZPOSI   is 'SMALLWHEELHUB_Z                                 ';
comment on column JDSH_data.CABCOUR1   is 'BRASSCONE_OUTERRADIUS1                          ';
comment on column JDSH_data.CABCTHI2   is 'BRASSCONE_THICKNESS2                            ';
comment on column JDSH_data.CABCOUR2   is 'BRASSCONE_OUTERRADIUS2                          ';
comment on column JDSH_data.LCAXBPOS   is 'LEADCONEASSEMBLY_XB                             ';
comment on column JDSH_data.LCAYBPOS   is 'LEADCONEASSEMBLY_YB                             ';
comment on column JDSH_data.LCAXAPOS   is 'LEADCONEASSEMBLY_XA                             ';
comment on column JDSH_data.LCAYAPOS   is 'LEADCONEASSEMBLY_YA                             ';
comment on column JDSH_data.PCAXBPOS   is 'POLYCONEASSEMBLY_XB                             ';
comment on column JDSH_data.PCAYBPOS   is 'POLYCONEASSEMBLY_YB                             ';
comment on column JDSH_data.PCAXAPOS   is 'POLYCONEASSEMBLY_XA                             ';
comment on column JDSH_data.PCAYAPOS   is 'POLYCONEASSEMBLY_YA                             ';
comment on column JDSH_data.CALCRADI   is 'LEADCONE_RADIUS                                 ';
comment on column JDSH_data.CALCPOSZ   is 'LEADCONE_POSZ                                   ';
comment on column JDSH_data.CAPCRADI   is 'POLYCONE_RADIUS                                 ';
comment on column JDSH_data.CAPCPOSZ   is 'POLYCONE_POSZ                                   ';
comment on column JDSH_data.I1         is 'I1                                              ';
comment on column JDSH_data.LCAPX_0    is 'ARRAY                                           ';
comment on column JDSH_data.LCAPY_0    is 'ARRAY                                           ';
comment on column JDSH_data.I2         is 'I2                                              ';
comment on column JDSH_data.PCAPX_0    is 'ARRAY                                           ';
comment on column JDSH_data.PCAPY_0    is 'ARRAY                                           ';
comment on column JDSH_data.PLUGINR2   is 'PLUG_INNERRADIUS2                               ';
