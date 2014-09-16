SET echo OFF;
set linesize 132;
drop   table RAIL_data2tag cascade constraints;
drop   table RAIL_data cascade constraints;
create table RAIL_data (
  RAIL_data_id   number(10),
CERZLENG       float(63),
EXRZLENG       float(63),
CERWIDT1       float(63),
CERWIDT2       float(63),
CERWID3O       float(63),
CERWID3I       float(63),
CERTHIC1       float(63),
CERTHIC2       float(63),
CERRPSZL       float(63),
CERRPEZL       float(63),
CERRPIX1       float(63),
CERRPOX1       float(63),
CERRPSZP_0       float(63),
CERRPSZP_1       float(63),
CERRPSZP_2       float(63),
CERRPSZP_3       float(63),
CERRPSZP_4       float(63),
CERRPSZP_5       float(63),
CERRPSZP_6       float(63),
CERRPSZP_7       float(63),
CERRPSZP_8       float(63),
CERRPSZP_9       float(63),
CERRPSZP_10       float(63),
EXRRPSZP_0       float(63),
EXRRPSZP_1       float(63),
EXRRPSZP_2       float(63),
EXRRPSZP_3       float(63),
EXRRPSZP_4       float(63),
EXRRPSZP_5       float(63),
EXRRPSZP_6       float(63),
EXRRPSZP_7       float(63),
EXRRPSZP_8       float(63),
EXRRPSZP_9       float(63),
EXRRPSZP_10       float(63),
EXRRPSZP_11       float(63),
EXRRPSZP_12       float(63),
EXRRPSZP_13       float(63),
EXRRPSZP_14       float(63),
EXRRPSZP_15       float(63),
EXRRPSZP_16       float(63),
XPOS       float(63),
TOTHEIGH       float(63),
CERY       float(63),
CERXB       float(63),
CERYB       float(63),
CERXC       float(63),
CERYC       float(63),
CERXD       float(63),
CERYD       float(63),
CERXE       float(63),
CERYE       float(63),
CERXF       float(63),
CERYF       float(63),
CERXG       float(63),
CERYG       float(63),
CERXH       float(63),
CERYH       float(63),
CERXI       float(63),
CERYI       float(63),
CERXJ       float(63),
CERYJ       float(63),
CERXK       float(63),
CERYK       float(63),
CERXL       float(63),
CERYL       float(63),
CERXM       float(63),
CERYM       float(63),
CERRPIY2       float(63),
CERRPIXA       float(63),
CERRPIYA       float(63),
CERRPIXB       float(63),
CERRPIYB       float(63),
CERRPIXC       float(63),
CERRPIYC       float(63),
CERRPIXD       float(63),
CERRPIYD       float(63),
CERRPIXE       float(63),
CERRPIYE       float(63),
CERRPOXA       float(63),
CERRPOYA       float(63),
CERRPOXB       float(63),
CERRPOYB       float(63),
CERRPOXC       float(63),
CERRPOYC       float(63),
CERRPOXD       float(63),
CERRPOYD       float(63),
CERRPOXE       float(63),
CERRPOYE       float(63),
CERRPE1Z       float(63),
CERRPE2Z       float(63),
EXRRPE1Z       float(63),
EXRRPE2Z       float(63),
YPOS       float(63),
EXTRZPOS       float(63)
) ;

alter table RAIL_data add constraint RAIL_data_pk
primary key (RAIL_data_id);

create table RAIL_data2tag (
  RAIL_vers      varchar2(255),
  RAIL_data_id   number(10)
) ;

alter table RAIL_data2tag add constraint RAIL_data2tag_pk
primary key (RAIL_vers, RAIL_data_id);
alter table RAIL_data2tag add constraint RAIL_data2tag_vers_fk
foreign key (RAIL_vers) references hvs_nodevers (vers_name);
alter table RAIL_data2tag add constraint RAIL_data2tag_data_fk
foreign key (RAIL_data_id) references RAIL_data (RAIL_data_id);

delete from hvs_relation where CHILD_NODENAME = 'RAIL';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'RAIL';
delete from hvs_node     where NODE_NAME = 'RAIL';

insert into hvs_node     values ('RAIL','MuonSpectrometer', '');
insert into hvs_nodevers values ('RAIL', 'RAIL-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','RAIL','RAIL-01');
insert into RAIL_data (
RAIL_data_id,
CERZLENG,
EXRZLENG,
CERWIDT1,
CERWIDT2,
CERWID3O,
CERWID3I,
CERTHIC1,
CERTHIC2,
CERRPSZL,
CERRPEZL,
CERRPIX1,
CERRPOX1,
CERRPSZP_0,CERRPSZP_1,CERRPSZP_2,CERRPSZP_3,CERRPSZP_4,CERRPSZP_5,CERRPSZP_6,
CERRPSZP_7,CERRPSZP_8,CERRPSZP_9,CERRPSZP_10,
EXRRPSZP_0,EXRRPSZP_1,EXRRPSZP_2,EXRRPSZP_3,EXRRPSZP_4,EXRRPSZP_5,EXRRPSZP_6,
EXRRPSZP_7,EXRRPSZP_8,EXRRPSZP_9,EXRRPSZP_10,EXRRPSZP_11,EXRRPSZP_12,
EXRRPSZP_13,EXRRPSZP_14,EXRRPSZP_15,EXRRPSZP_16,
XPOS,
TOTHEIGH,
CERY,
CERXB,
CERYB,
CERXC,
CERYC,
CERXD,
CERYD,
CERXE,
CERYE,
CERXF,
CERYF,
CERXG,
CERYG,
CERXH,
CERYH,
CERXI,
CERYI,
CERXJ,
CERYJ,
CERXK,
CERYK,
CERXL,
CERYL,
CERXM,
CERYM,
CERRPIY2,
CERRPIXA,
CERRPIYA,
CERRPIXB,
CERRPIYB,
CERRPIXC,
CERRPIYC,
CERRPIXD,
CERRPIYD,
CERRPIXE,
CERRPIYE,
CERRPOXA,
CERRPOYA,
CERRPOXB,
CERRPOYB,
CERRPOXC,
CERRPOYC,
CERRPOXD,
CERRPOYD,
CERRPOXE,
CERRPOYE,
CERRPE1Z,
CERRPE2Z,
EXRRPE1Z,
EXRRPE2Z,
YPOS,
EXTRZPOS
) values (    1,
  6850.0000,  9784.0000,   450.0000,   120.0000,   380.0000,   350.0000,
   100.0000,   200.0000,    50.0000,    40.0000,   260.0000,   290.0000,
   490.0000,  1090.0000,  1687.5000,  2285.0000,  2885.0000,  3400.0000,
  3915.0000,  4515.0000,  5112.5000,  5710.0000,  6310.0000,   540.0000,
  1085.0000,  1627.5000,  2170.0000,  2715.0000,  3230.0000,  3745.0000,
  4315.0000,  4895.0000,  5465.0000,  5980.0000,  6495.0000,  7017.5000,
  7540.0000,  8062.5000,  8720.0000,  9375.0000,  2985.0000,   400.0000,
     0.0000,   225.0000,     0.0000,   225.0000,   100.0000,    60.0000,
   100.0000,    60.0000,   300.0000,   350.0000,   300.0000,   350.0000,
   400.0000,  -380.0000,   400.0000,  -380.0000,   300.0000,   -60.0000,
   300.0000,   -60.0000,   100.0000,  -225.0000,   100.0000,  -225.0000,
     0.0000,   135.6741,    60.0000,   100.0000,   225.0000,   100.0000,
   320.0000,   235.6741,   320.0000,   300.0000,    60.0000,   300.0000,
   -60.0000,   100.0000,  -225.0000,   100.0000,  -350.0000,   235.6741,
  -350.0000,   300.0000,   -60.0000,   300.0000, -3405.0000,  3405.0000,
 -4872.0000,  4872.0000, -4790.0000,  8317.0000
);
insert into RAIL_data2tag values ('RAIL-01',    1);

comment on column RAIL_data.RAIL_data_id is 'Unique identifier';
comment on column RAIL_data.CERZLENG   is 'CENTRALRAIL_ZLENGTH                             ';
comment on column RAIL_data.EXRZLENG   is 'EXTREMITYRAIL_ZLENGTH                           ';
comment on column RAIL_data.CERWIDT1   is 'CENTRALRAIL_WIDTH1                              ';
comment on column RAIL_data.CERWIDT2   is 'CENTRALRAIL_WIDTH2                              ';
comment on column RAIL_data.CERWID3O   is 'CENTRALRAIL_WIDTH3OUTER                         ';
comment on column RAIL_data.CERWID3I   is 'CENTRALRAIL_WIDTH3INNER                         ';
comment on column RAIL_data.CERTHIC1   is 'CENTRALRAIL_THICK1                              ';
comment on column RAIL_data.CERTHIC2   is 'CENTRALRAIL_THICK2                              ';
comment on column RAIL_data.CERRPSZL   is 'CENTRALRAILREINFORCEMENTPLATE_STD_ZLENGT        ';
comment on column RAIL_data.CERRPEZL   is 'CENTRALRAILREINFORCEMENTPLATE_EXTREMITY_        ';
comment on column RAIL_data.CERRPIX1   is 'CENTRALRAILREINFORCEMENTPLATEINNER_XDIST        ';
comment on column RAIL_data.CERRPOX1   is 'CENTRALRAILREINFORCEMENTPLATEOUTER_XDIST        ';
comment on column RAIL_data.CERRPSZP_0 is 'ARRAY                                           ';
comment on column RAIL_data.EXRRPSZP_0 is 'ARRAY                                           ';
comment on column RAIL_data.XPOS       is 'XPOS                                            ';
comment on column RAIL_data.TOTHEIGH   is 'TOTHEIGHT                                       ';
comment on column RAIL_data.CERY       is 'CERY                                            ';
comment on column RAIL_data.CERXB      is 'CERXB                                           ';
comment on column RAIL_data.CERYB      is 'CERYB                                           ';
comment on column RAIL_data.CERXC      is 'CERXC                                           ';
comment on column RAIL_data.CERYC      is 'CERYC                                           ';
comment on column RAIL_data.CERXD      is 'CERXD                                           ';
comment on column RAIL_data.CERYD      is 'CERYD                                           ';
comment on column RAIL_data.CERXE      is 'CERXE                                           ';
comment on column RAIL_data.CERYE      is 'CERYE                                           ';
comment on column RAIL_data.CERXF      is 'CERXF                                           ';
comment on column RAIL_data.CERYF      is 'CERYF                                           ';
comment on column RAIL_data.CERXG      is 'CERXG                                           ';
comment on column RAIL_data.CERYG      is 'CERYG                                           ';
comment on column RAIL_data.CERXH      is 'CERXH                                           ';
comment on column RAIL_data.CERYH      is 'CERYH                                           ';
comment on column RAIL_data.CERXI      is 'CERXI                                           ';
comment on column RAIL_data.CERYI      is 'CERYI                                           ';
comment on column RAIL_data.CERXJ      is 'CERXJ                                           ';
comment on column RAIL_data.CERYJ      is 'CERYJ                                           ';
comment on column RAIL_data.CERXK      is 'CERXK                                           ';
comment on column RAIL_data.CERYK      is 'CERYK                                           ';
comment on column RAIL_data.CERXL      is 'CERXL                                           ';
comment on column RAIL_data.CERYL      is 'CERYL                                           ';
comment on column RAIL_data.CERXM      is 'CERXM                                           ';
comment on column RAIL_data.CERYM      is 'CERYM                                           ';
comment on column RAIL_data.CERRPIY2   is 'CENTRALRAILREINFORCEMENTPLATEINNER_YDIST        ';
comment on column RAIL_data.CERRPIXA   is 'CERRPIXA                                        ';
comment on column RAIL_data.CERRPIYA   is 'CERRPIYA                                        ';
comment on column RAIL_data.CERRPIXB   is 'CERRPIXB                                        ';
comment on column RAIL_data.CERRPIYB   is 'CERRPIYB                                        ';
comment on column RAIL_data.CERRPIXC   is 'CERRPIXC                                        ';
comment on column RAIL_data.CERRPIYC   is 'CERRPIYC                                        ';
comment on column RAIL_data.CERRPIXD   is 'CERRPIXD                                        ';
comment on column RAIL_data.CERRPIYD   is 'CERRPIYD                                        ';
comment on column RAIL_data.CERRPIXE   is 'CERRPIXE                                        ';
comment on column RAIL_data.CERRPIYE   is 'CERRPIYE                                        ';
comment on column RAIL_data.CERRPOXA   is 'CERRPOXA                                        ';
comment on column RAIL_data.CERRPOYA   is 'CERRPOYA                                        ';
comment on column RAIL_data.CERRPOXB   is 'CERRPOXB                                        ';
comment on column RAIL_data.CERRPOYB   is 'CERRPOYB                                        ';
comment on column RAIL_data.CERRPOXC   is 'CERRPOXC                                        ';
comment on column RAIL_data.CERRPOYC   is 'CERRPOYC                                        ';
comment on column RAIL_data.CERRPOXD   is 'CERRPOXD                                        ';
comment on column RAIL_data.CERRPOYD   is 'CERRPOYD                                        ';
comment on column RAIL_data.CERRPOXE   is 'CERRPOXE                                        ';
comment on column RAIL_data.CERRPOYE   is 'CERRPOYE                                        ';
comment on column RAIL_data.CERRPE1Z   is 'CENTRALRAILREINFORCEMENTPLATE_EXTREMITY1        ';
comment on column RAIL_data.CERRPE2Z   is 'CENTRALRAILREINFORCEMENTPLATE_EXTREMITY2        ';
comment on column RAIL_data.EXRRPE1Z   is 'EXTREMITYRAILREINFORCEMENTPLATE_EXTREMIT        ';
comment on column RAIL_data.EXRRPE2Z   is 'EXTREMITYRAILREINFORCEMENTPLATE_EXTREMIT        ';
comment on column RAIL_data.YPOS       is 'YPOS                                            ';
comment on column RAIL_data.EXTRZPOS   is 'EXTRZPOS                                        ';
