SET echo OFF;
set linesize 132;
drop   table JFSH_data2tag cascade constraints;
drop   table JFSH_data cascade constraints;
create table JFSH_data (
  JFSH_data_id   number(10),
XYZREF       float(63),
ROT180       float(63),
AFBBLENG       float(63),
AFBHEIGH       float(63),
AFRWIDTH       float(63),
AFBTHIC1       float(63),
AFBTHIC2       float(63),
AFLANGLE       float(63),
AFSLENGT       float(63),
AFSHEIGH       float(63),
AFWEBBX1       float(63),
AFWEBBX2       float(63),
AFWELENG       float(63),
AFWEBLX1       float(63),
AFWEBLX2       float(63),
AFFTHICK       float(63),
AFFLENGT       float(63),
AFTPLENG       float(63),
AFTPHEIG       float(63),
AFWHEIGH       float(63),
AFWEXTEN       float(63),
AFWTHICK       float(63),
AFWZOFFS       float(63),
AFFEELEN       float(63),
AFFEETHI       float(63),
AFFEESLE       float(63),
AFFEEHEI       float(63),
AFFEEEXT       float(63),
AFTOTHEI       float(63),
AFECTCLE       float(63),
ECTNOMIZ       float(63),
AFYPOSIT       float(63),
AFZPOSIT       float(63),
AFBLHPXA       float(63),
AFBLHPYA       float(63),
AFBLHPYB       float(63),
AFBLHPXB       float(63),
AFBBVPXA       float(63),
AFBBVPYA       float(63),
AFBBVPYB       float(63),
AFBBVPXB       float(63),
AFBUHPXA       float(63),
AFBUHPYA       float(63),
AFBUHPYB       float(63),
AFBUHPXB       float(63),
AFLLHPXA       float(63),
AFLLHPYA       float(63),
AFLLHPXB       float(63),
AFLLHPYB       float(63),
AFLLHPXC       float(63),
AFLLHPYC       float(63),
AFLLHPXD       float(63),
AFLLHPYD       float(63),
AFLVPXA       float(63),
AFLVPYA       float(63),
AFLVPXB       float(63),
AFLVPYB       float(63),
AFLVPXC       float(63),
AFLVPYC       float(63),
AFLVPXE       float(63),
AFLVPYE       float(63),
AFLVPYD       float(63),
AFLVPXD       float(63),
AFLUHPXA       float(63),
AFLUHPYA       float(63),
AFLUHPXB       float(63),
AFLUHPYB       float(63),
AFLUHPXC       float(63),
AFLUHPYC       float(63),
AFLUHPXD       float(63),
AFLUHPYD       float(63),
AFTPLAXA       float(63),
AFTPLAYA       float(63),
AFTPLAXB       float(63),
AFTPLAYB       float(63),
AFSPACXA       float(63),
AFSPACYA       float(63),
AFSPACXB       float(63),
AFSPACYB       float(63),
AFWEBWID       float(63),
AFWEBHEI       float(63),
AFWEBBBY       float(63),
AFWEBZPO       float(63),
AFWELXI1       float(63),
AFWELYI1       float(63),
AFWELXP1       float(63),
AFWELYP1       float(63),
AFWELXI2       float(63),
AFWELYI2       float(63),
AFWELXP2       float(63),
AFWELYP2       float(63),
AFWELXI3       float(63),
AFWELYI3       float(63),
AFWELXP3       float(63),
AFWELYP3       float(63),
AFFWIDTH       float(63),
AFFZPOSI       float(63),
AFFPOSX1       float(63),
AFFPOSY1       float(63),
AFFPOSX2       float(63),
AFFPOSY2       float(63),
AFWPOSXA       float(63),
AFWPOSYA       float(63),
AFWPOSXB       float(63),
AFWPOSYB       float(63),
AFWPOSXC       float(63),
AFWPOSYC       float(63),
AFWPOSXD       float(63),
AFWPOSYD       float(63),
AFWPOSX0       float(63),
AFWPOSY0       float(63),
AFWPOSIX       float(63),
AFWPOSIY       float(63),
AFWPOSIZ       float(63),
AFFEETXA       float(63),
AFFEETYA       float(63),
AFFEETXB       float(63),
AFFEETYB       float(63),
AFFEETXC       float(63),
AFFEETYC       float(63),
AFFEETXD       float(63),
AFFEETYD       float(63),
AFFEETXE       float(63),
AFFEETYE       float(63),
AFFEETXF       float(63),
AFFEETYF       float(63),
AFFEETPX       float(63),
AFASPOSY       float(63),
AFASPOSZ       float(63),
JFCMCLEN       float(63),
JFCMCIR1       float(63),
JFCMCOUR       float(63),
JFCMCHSL       float(63),
JFCMCZOF       float(63),
PLUGLENG       float(63),
PLUGINRA       float(63),
PLUGOURA       float(63),
JFSOCMRA       float(63),
JFSOCLEN       float(63),
JFSOCZOF       float(63),
AFOCONLE       float(63),
AFOCONWI       float(63),
AFOCONHE       float(63),
TX1E1ORA       float(63),
TX1E1IRA       float(63),
TX1E1ILE       float(63),
TX1E2ORA       float(63),
TX1E2IRA       float(63),
TX1E2ILE       float(63),
TX1STZOF       float(63),
JFCMCIR2       float(63),
JFCMCZPO       float(63),
JFSOCZPO       float(63),
TX1E1ZPO       float(63),
TX1E2ZPO       float(63),
PLUGZPOS       float(63),
AFMCCONY       float(63),
AFMCCONZ       float(63),
JFSOCTXA       float(63),
JFSOCTYA       float(63),
JFSOCTXB       float(63),
JFSOCTYB       float(63)
) ;

alter table JFSH_data add constraint JFSH_data_pk
primary key (JFSH_data_id);

create table JFSH_data2tag (
  JFSH_vers      varchar2(255),
  JFSH_data_id   number(10)
) ;

alter table JFSH_data2tag add constraint JFSH_data2tag_pk
primary key (JFSH_vers, JFSH_data_id);
alter table JFSH_data2tag add constraint JFSH_data2tag_vers_fk
foreign key (JFSH_vers) references hvs_nodevers (vers_name);
alter table JFSH_data2tag add constraint JFSH_data2tag_data_fk
foreign key (JFSH_data_id) references JFSH_data (JFSH_data_id);

delete from hvs_relation where CHILD_NODENAME = 'JFSH';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'JFSH';
delete from hvs_node     where NODE_NAME = 'JFSH';

insert into hvs_node     values ('JFSH','MuonSpectrometer', '');
insert into hvs_nodevers values ('JFSH', 'JFSH-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','JFSH','JFSH-01');
insert into JFSH_data (
JFSH_data_id,
XYZREF,
ROT180,
AFBBLENG,
AFBHEIGH,
AFRWIDTH,
AFBTHIC1,
AFBTHIC2,
AFLANGLE,
AFSLENGT,
AFSHEIGH,
AFWEBBX1,
AFWEBBX2,
AFWELENG,
AFWEBLX1,
AFWEBLX2,
AFFTHICK,
AFFLENGT,
AFTPLENG,
AFTPHEIG,
AFWHEIGH,
AFWEXTEN,
AFWTHICK,
AFWZOFFS,
AFFEELEN,
AFFEETHI,
AFFEESLE,
AFFEEHEI,
AFFEEEXT,
AFTOTHEI,
AFECTCLE,
ECTNOMIZ,
AFYPOSIT,
AFZPOSIT,
AFBLHPXA,
AFBLHPYA,
AFBLHPYB,
AFBLHPXB,
AFBBVPXA,
AFBBVPYA,
AFBBVPYB,
AFBBVPXB,
AFBUHPXA,
AFBUHPYA,
AFBUHPYB,
AFBUHPXB,
AFLLHPXA,
AFLLHPYA,
AFLLHPXB,
AFLLHPYB,
AFLLHPXC,
AFLLHPYC,
AFLLHPXD,
AFLLHPYD,
AFLVPXA,
AFLVPYA,
AFLVPXB,
AFLVPYB,
AFLVPXC,
AFLVPYC,
AFLVPXE,
AFLVPYE,
AFLVPYD,
AFLVPXD,
AFLUHPXA,
AFLUHPYA,
AFLUHPXB,
AFLUHPYB,
AFLUHPXC,
AFLUHPYC,
AFLUHPXD,
AFLUHPYD,
AFTPLAXA,
AFTPLAYA,
AFTPLAXB,
AFTPLAYB,
AFSPACXA,
AFSPACYA,
AFSPACXB,
AFSPACYB,
AFWEBWID,
AFWEBHEI,
AFWEBBBY,
AFWEBZPO,
AFWELXI1,
AFWELYI1,
AFWELXP1,
AFWELYP1,
AFWELXI2,
AFWELYI2,
AFWELXP2,
AFWELYP2,
AFWELXI3,
AFWELYI3,
AFWELXP3,
AFWELYP3,
AFFWIDTH,
AFFZPOSI,
AFFPOSX1,
AFFPOSY1,
AFFPOSX2,
AFFPOSY2,
AFWPOSXA,
AFWPOSYA,
AFWPOSXB,
AFWPOSYB,
AFWPOSXC,
AFWPOSYC,
AFWPOSXD,
AFWPOSYD,
AFWPOSX0,
AFWPOSY0,
AFWPOSIX,
AFWPOSIY,
AFWPOSIZ,
AFFEETXA,
AFFEETYA,
AFFEETXB,
AFFEETYB,
AFFEETXC,
AFFEETYC,
AFFEETXD,
AFFEETYD,
AFFEETXE,
AFFEETYE,
AFFEETXF,
AFFEETYF,
AFFEETPX,
AFASPOSY,
AFASPOSZ,
JFCMCLEN,
JFCMCIR1,
JFCMCOUR,
JFCMCHSL,
JFCMCZOF,
PLUGLENG,
PLUGINRA,
PLUGOURA,
JFSOCMRA,
JFSOCLEN,
JFSOCZOF,
AFOCONLE,
AFOCONWI,
AFOCONHE,
TX1E1ORA,
TX1E1IRA,
TX1E1ILE,
TX1E2ORA,
TX1E2IRA,
TX1E2ILE,
TX1STZOF,
JFCMCIR2,
JFCMCZPO,
JFSOCZPO,
TX1E1ZPO,
TX1E2ZPO,
PLUGZPOS,
AFMCCONY,
AFMCCONZ,
JFSOCTXA,
JFSOCTYA,
JFSOCTXB,
JFSOCTYB
) values (    1,
     0.0000,   180.0000,  5498.0000,   250.0000,   250.0000,     9.0000,
    14.0000,    42.0000,   140.0000,   290.0000,     0.0000,  1306.0000,
    30.0000,  1098.0000,   965.0000,    15.0000,   935.0000,   300.0000,
    50.0000,   149.2000,   200.0000,    50.0000,    20.0000,   830.0000,
    50.0000,   100.0000,   225.0000,  6830.0000,  2740.0000,    25.0000,
 12914.0000, -4340.0000,   150.0000,  2749.0000,     0.0000,    14.0000,
  2733.4514,  2733.4514,    14.0000,   236.0000,  2486.8955,  2486.8955,
   236.0000,   250.0000,  2471.3469,  2749.0000,     0.0000,    70.0000,
  2412.1824,    70.0000,  2431.0212,  2769.9226,     0.0000,  2769.9226,
     0.0000,    70.0000,  2431.0212,    70.0000,  2690.0000,  3101.6965,
     0.0000,  2690.0000,   114.1488,  3101.6965,     0.0000,   114.1488,
  2690.0000,   135.0715,  2690.0000,  3122.6191,     0.0000,   150.0000,
  2690.0000,   150.0000,  2740.0000,    70.0000,  2400.0000,    70.0000,
  2690.0000,   120.5000,   222.0000,   125.0000,    64.7500,  2306.6462,
   734.7054,  2223.0049,   641.8123,  1589.5114,  1380.4164,  1505.8700,
  1287.5233,   872.3766,  2026.1274,   788.7353,  1933.2344,   120.5000,
    64.7500,  2581.5723,   318.9568,  1864.4374,   964.6678,     0.0000,
     0.0000,   134.3403,   149.2000,   282.9692,    15.3739,   269.1266,
     0.0000,   300.0437,     0.0000,  2171.3032,   250.0000,    80.0000,
   415.0000,     0.0000,   415.0000,  -225.0000,   315.0000,  -225.0000,
   315.0000,  -175.0000,   365.0000,  -175.0000,   365.0000,   -50.0000,
  3000.0000, -4340.0000,   150.0000,  5041.0000,   304.0000,  1470.0000,
     1.1000,    45.0000,   250.0000,   280.0000,   520.0000,  2100.0000,
  3000.0000,  5086.0000,   230.0000,   200.0000,  1000.0000,  1475.0000,
  1275.0000,   500.0000,  1300.0000,  1150.0000,   400.0000,  8086.0000,
   400.7921,    45.0000,  6586.0000,  8336.0000,  8786.0000,  -250.0000,
 -1100.0000,   115.1000,  2100.0000,   869.8485,   869.8485,  2100.0000
);
insert into JFSH_data2tag values ('JFSH-01',    1);

comment on column JFSH_data.JFSH_data_id is 'Unique identifier';
comment on column JFSH_data.XYZREF     is 'XYZREF                                          ';
comment on column JFSH_data.ROT180     is 'ROT180                                          ';
comment on column JFSH_data.AFBBLENG   is 'AFRAME_BOTTOMBAR_LENGTH                         ';
comment on column JFSH_data.AFBHEIGH   is 'AFRAME_BAR_HEIGHT                               ';
comment on column JFSH_data.AFRWIDTH   is 'AFRAME_WIDTH                                    ';
comment on column JFSH_data.AFBTHIC1   is 'AFRAME_BAR_THICKNESS1                           ';
comment on column JFSH_data.AFBTHIC2   is 'AFRAME_BAR_THICKNESS2                           ';
comment on column JFSH_data.AFLANGLE   is 'AFRAME_LEG_ANGLE                                ';
comment on column JFSH_data.AFSLENGT   is 'AFRAME_SPACER_LENGTH                            ';
comment on column JFSH_data.AFSHEIGH   is 'AFRAME_SPACER_HEIGHT                            ';
comment on column JFSH_data.AFWEBBX1   is 'AFRAME_WEB_ELEMENT_BOTTOMBAR_X1                 ';
comment on column JFSH_data.AFWEBBX2   is 'AFRAME_WEB_ELEMENT_BOTTOMBAR_X2                 ';
comment on column JFSH_data.AFWELENG   is 'AFRAME_WEB_ELEMENT_LENGTH                       ';
comment on column JFSH_data.AFWEBLX1   is 'AFRAME_WEB_ELEMENT_LEG_DX1                      ';
comment on column JFSH_data.AFWEBLX2   is 'AFRAME_WEB_ELEMENT_LEG_DX2                      ';
comment on column JFSH_data.AFFTHICK   is 'AFRAME_FLANGE_THICKNESS                         ';
comment on column JFSH_data.AFFLENGT   is 'AFRAME_FLANGE_LENGTH                            ';
comment on column JFSH_data.AFTPLENG   is 'AFRAME_TOPPLATE_LENGTH                          ';
comment on column JFSH_data.AFTPHEIG   is 'AFRAME_TOPPLATE_HEIGHT                          ';
comment on column JFSH_data.AFWHEIGH   is 'AFRAME_WEDGE_HEIGHT                             ';
comment on column JFSH_data.AFWEXTEN   is 'AFRAME_WEDGE_EXTENSION                          ';
comment on column JFSH_data.AFWTHICK   is 'AFRAME_WEDGE_THICKNESS                          ';
comment on column JFSH_data.AFWZOFFS   is 'AFRAME_WEDGE_ZOFFSET                            ';
comment on column JFSH_data.AFFEELEN   is 'AFRAME_FEET_LENGTH                              ';
comment on column JFSH_data.AFFEETHI   is 'AFRAME_FEET_THICKNESS                           ';
comment on column JFSH_data.AFFEESLE   is 'AFRAME_FEET_SHORTLENGTH                         ';
comment on column JFSH_data.AFFEEHEI   is 'AFRAME_FEET_HEIGHT                              ';
comment on column JFSH_data.AFFEEEXT   is 'AFRAME_FEET_X_EXTENSION                         ';
comment on column JFSH_data.AFTOTHEI   is 'AFRAME_TOTAL_HEIGHT                             ';
comment on column JFSH_data.AFECTCLE   is 'CLEARANCE_ECT_AFRAME                            ';
comment on column JFSH_data.ECTNOMIZ   is 'ECT_NOMINAL_ZPOS                                ';
comment on column JFSH_data.AFYPOSIT   is 'AFRAME_YPOS                                     ';
comment on column JFSH_data.AFZPOSIT   is 'AFRAME_ZPOS                                     ';
comment on column JFSH_data.AFBLHPXA   is 'AFBLHPXA                                        ';
comment on column JFSH_data.AFBLHPYA   is 'AFBLHPYA                                        ';
comment on column JFSH_data.AFBLHPYB   is 'AFBLHPYB                                        ';
comment on column JFSH_data.AFBLHPXB   is 'AFBLHPXB                                        ';
comment on column JFSH_data.AFBBVPXA   is 'AFBBVPXA                                        ';
comment on column JFSH_data.AFBBVPYA   is 'AFBBVPYA                                        ';
comment on column JFSH_data.AFBBVPYB   is 'AFBBVPYB                                        ';
comment on column JFSH_data.AFBBVPXB   is 'AFBBVPXB                                        ';
comment on column JFSH_data.AFBUHPXA   is 'AFBUHPXA                                        ';
comment on column JFSH_data.AFBUHPYA   is 'AFBUHPYA                                        ';
comment on column JFSH_data.AFBUHPYB   is 'AFBUHPYB                                        ';
comment on column JFSH_data.AFBUHPXB   is 'AFBUHPXB                                        ';
comment on column JFSH_data.AFLLHPXA   is 'AFLLHPXA                                        ';
comment on column JFSH_data.AFLLHPYA   is 'AFLLHPYA                                        ';
comment on column JFSH_data.AFLLHPXB   is 'AFLLHPXB                                        ';
comment on column JFSH_data.AFLLHPYB   is 'AFLLHPYB                                        ';
comment on column JFSH_data.AFLLHPXC   is 'AFLLHPXC                                        ';
comment on column JFSH_data.AFLLHPYC   is 'AFLLHPYC                                        ';
comment on column JFSH_data.AFLLHPXD   is 'AFLLHPXD                                        ';
comment on column JFSH_data.AFLLHPYD   is 'AFLLHPYD                                        ';
comment on column JFSH_data.AFLVPXA    is 'AFLVPXA                                         ';
comment on column JFSH_data.AFLVPYA    is 'AFLVPYA                                         ';
comment on column JFSH_data.AFLVPXB    is 'AFLVPXB                                         ';
comment on column JFSH_data.AFLVPYB    is 'AFLVPYB                                         ';
comment on column JFSH_data.AFLVPXC    is 'AFLVPXC                                         ';
comment on column JFSH_data.AFLVPYC    is 'AFLVPYC                                         ';
comment on column JFSH_data.AFLVPXE    is 'AFLVPXE                                         ';
comment on column JFSH_data.AFLVPYE    is 'AFLVPYE                                         ';
comment on column JFSH_data.AFLVPYD    is 'AFLVPYD                                         ';
comment on column JFSH_data.AFLVPXD    is 'AFLVPXD                                         ';
comment on column JFSH_data.AFLUHPXA   is 'AFLUHPXA                                        ';
comment on column JFSH_data.AFLUHPYA   is 'AFLUHPYA                                        ';
comment on column JFSH_data.AFLUHPXB   is 'AFLUHPXB                                        ';
comment on column JFSH_data.AFLUHPYB   is 'AFLUHPYB                                        ';
comment on column JFSH_data.AFLUHPXC   is 'AFLUHPXC                                        ';
comment on column JFSH_data.AFLUHPYC   is 'AFLUHPYC                                        ';
comment on column JFSH_data.AFLUHPXD   is 'AFLUHPXD                                        ';
comment on column JFSH_data.AFLUHPYD   is 'AFLUHPYD                                        ';
comment on column JFSH_data.AFTPLAXA   is 'AFTPLAXA                                        ';
comment on column JFSH_data.AFTPLAYA   is 'AFTPLAYA                                        ';
comment on column JFSH_data.AFTPLAXB   is 'AFTPLAXB                                        ';
comment on column JFSH_data.AFTPLAYB   is 'AFTPLAYB                                        ';
comment on column JFSH_data.AFSPACXA   is 'AFSPACXA                                        ';
comment on column JFSH_data.AFSPACYA   is 'AFSPACYA                                        ';
comment on column JFSH_data.AFSPACXB   is 'AFSPACXB                                        ';
comment on column JFSH_data.AFSPACYB   is 'AFSPACYB                                        ';
comment on column JFSH_data.AFWEBWID   is 'AFRAME_WEB_ELEMENT_WIDTH                        ';
comment on column JFSH_data.AFWEBHEI   is 'AFRAME_WEB_ELEMENT_HEIGHT                       ';
comment on column JFSH_data.AFWEBBBY   is 'AFRAME_WEB_ELEMENT_BOTTOMBAR_Y                  ';
comment on column JFSH_data.AFWEBZPO   is 'AFRAME_WEB_ELEMENT_Z                            ';
comment on column JFSH_data.AFWELXI1   is 'AFWELXI1                                        ';
comment on column JFSH_data.AFWELYI1   is 'AFWELYI1                                        ';
comment on column JFSH_data.AFWELXP1   is 'AFWELXP1                                        ';
comment on column JFSH_data.AFWELYP1   is 'AFWELYP1                                        ';
comment on column JFSH_data.AFWELXI2   is 'AFWELXI2                                        ';
comment on column JFSH_data.AFWELYI2   is 'AFWELYI2                                        ';
comment on column JFSH_data.AFWELXP2   is 'AFWELXP2                                        ';
comment on column JFSH_data.AFWELYP2   is 'AFWELYP2                                        ';
comment on column JFSH_data.AFWELXI3   is 'AFWELXI3                                        ';
comment on column JFSH_data.AFWELYI3   is 'AFWELYI3                                        ';
comment on column JFSH_data.AFWELXP3   is 'AFWELXP3                                        ';
comment on column JFSH_data.AFWELYP3   is 'AFWELYP3                                        ';
comment on column JFSH_data.AFFWIDTH   is 'AFRAME_FLANGE_WIDTH                             ';
comment on column JFSH_data.AFFZPOSI   is 'AFRAME_FLANGE_Z                                 ';
comment on column JFSH_data.AFFPOSX1   is 'AFRAME_FLANGE_X1                                ';
comment on column JFSH_data.AFFPOSY1   is 'AFRAME_FLANGE_Y1                                ';
comment on column JFSH_data.AFFPOSX2   is 'AFRAME_FLANGE_X2                                ';
comment on column JFSH_data.AFFPOSY2   is 'AFRAME_FLANGE_Y2                                ';
comment on column JFSH_data.AFWPOSXA   is 'AFRAME_WEDGE_XA                                 ';
comment on column JFSH_data.AFWPOSYA   is 'AFRAME_WEDGE_YA                                 ';
comment on column JFSH_data.AFWPOSXB   is 'AFRAME_WEDGE_XB                                 ';
comment on column JFSH_data.AFWPOSYB   is 'AFRAME_WEDGE_YB                                 ';
comment on column JFSH_data.AFWPOSXC   is 'AFRAME_WEDGE_XC                                 ';
comment on column JFSH_data.AFWPOSYC   is 'AFRAME_WEDGE_YC                                 ';
comment on column JFSH_data.AFWPOSXD   is 'AFRAME_WEDGE_XD                                 ';
comment on column JFSH_data.AFWPOSYD   is 'AFRAME_WEDGE_YD                                 ';
comment on column JFSH_data.AFWPOSX0   is 'AFRAME_WEDGE_X0                                 ';
comment on column JFSH_data.AFWPOSY0   is 'AFRAME_WEDGE_Y0                                 ';
comment on column JFSH_data.AFWPOSIX   is 'AFRAME_WEDGE_X                                  ';
comment on column JFSH_data.AFWPOSIY   is 'AFRAME_WEDGE_Y                                  ';
comment on column JFSH_data.AFWPOSIZ   is 'AFRAME_WEDGE_Z                                  ';
comment on column JFSH_data.AFFEETXA   is 'AFRAME_FEET_XA                                  ';
comment on column JFSH_data.AFFEETYA   is 'AFRAME_FEET_YA                                  ';
comment on column JFSH_data.AFFEETXB   is 'AFRAME_FEET_XB                                  ';
comment on column JFSH_data.AFFEETYB   is 'AFRAME_FEET_YB                                  ';
comment on column JFSH_data.AFFEETXC   is 'AFRAME_FEET_XC                                  ';
comment on column JFSH_data.AFFEETYC   is 'AFRAME_FEET_YC                                  ';
comment on column JFSH_data.AFFEETXD   is 'AFRAME_FEET_XD                                  ';
comment on column JFSH_data.AFFEETYD   is 'AFRAME_FEET_YD                                  ';
comment on column JFSH_data.AFFEETXE   is 'AFRAME_FEET_XE                                  ';
comment on column JFSH_data.AFFEETYE   is 'AFRAME_FEET_YE                                  ';
comment on column JFSH_data.AFFEETXF   is 'AFRAME_FEET_XF                                  ';
comment on column JFSH_data.AFFEETYF   is 'AFRAME_FEET_YF                                  ';
comment on column JFSH_data.AFFEETPX   is 'AFRAME_FEET_X                                   ';
comment on column JFSH_data.AFASPOSY   is 'AFRAME_ASSEMBLY_Y                               ';
comment on column JFSH_data.AFASPOSZ   is 'AFRAME_ASSEMBLY_Z                               ';
comment on column JFSH_data.JFCMCLEN   is 'JFCMAINCYLINDER_LENGTH                          ';
comment on column JFSH_data.JFCMCIR1   is 'JFCMAINCYLINDER_INNERRADIUS1                    ';
comment on column JFSH_data.JFCMCOUR   is 'JFCMAINCYLINDER_OUTERRADIUS                     ';
comment on column JFSH_data.JFCMCHSL   is 'JFCMAINCYLINDER_HOLESLOPE                       ';
comment on column JFSH_data.JFCMCZOF   is 'JFCMAINCYLINDER_ZOFFSET                         ';
comment on column JFSH_data.PLUGLENG   is 'PLUG_LENGTH                                     ';
comment on column JFSH_data.PLUGINRA   is 'PLUG_INNERRADIUS                                ';
comment on column JFSH_data.PLUGOURA   is 'PLUG_OUTERRADIUS                                ';
comment on column JFSH_data.JFSOCMRA   is 'JFSOCTOGON_MAINRADIUS                           ';
comment on column JFSH_data.JFSOCLEN   is 'JFSOCTOGON_LENGTH                               ';
comment on column JFSH_data.JFSOCZOF   is 'JFSOCTOGON_ZOFFSET                              ';
comment on column JFSH_data.AFOCONLE   is 'AFRAME_TO_JFSOCTOGON_CONNECTION_LENGTH          ';
comment on column JFSH_data.AFOCONWI   is 'AFRAME_TO_JFSOCTOGON_CONNECTION_WIDTH           ';
comment on column JFSH_data.AFOCONHE   is 'AFRAME_TO_JFSOCTOGON_CONNECTION_HEIGHT          ';
comment on column JFSH_data.TX1E1ORA   is 'TX1E1ORA                                        ';
comment on column JFSH_data.TX1E1IRA   is 'TX1E1IRA                                        ';
comment on column JFSH_data.TX1E1ILE   is 'TX1E1ILE                                        ';
comment on column JFSH_data.TX1E2ORA   is 'TX1E2ORA                                        ';
comment on column JFSH_data.TX1E2IRA   is 'TX1E2IRA                                        ';
comment on column JFSH_data.TX1E2ILE   is 'TX1E2ILE                                        ';
comment on column JFSH_data.TX1STZOF   is 'TX1STZOF                                        ';
comment on column JFSH_data.JFCMCIR2   is 'JFCMAINCYLINDER_INNERRADIUS2                    ';
comment on column JFSH_data.JFCMCZPO   is 'JFCMAINCYLINDER_Z                               ';
comment on column JFSH_data.JFSOCZPO   is 'JFSOCTOGON_Z                                    ';
comment on column JFSH_data.TX1E1ZPO   is 'TX1STM_ELEMENT1_Z                               ';
comment on column JFSH_data.TX1E2ZPO   is 'TX1STM_ELEMENT2_Z                               ';
comment on column JFSH_data.PLUGZPOS   is 'PLUG_POSZ                                       ';
comment on column JFSH_data.AFMCCONY   is 'AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Y          ';
comment on column JFSH_data.AFMCCONZ   is 'AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Z          ';
comment on column JFSH_data.JFSOCTXA   is 'JFSOCTOGON_XA                                   ';
comment on column JFSH_data.JFSOCTYA   is 'JFSOCTOGON_YA                                   ';
comment on column JFSH_data.JFSOCTXB   is 'JFSOCTOGON_XB                                   ';
comment on column JFSH_data.JFSOCTYB   is 'JFSOCTOGON_YB                                   ';
