SET echo OFF;
set linesize 132;
drop   table WMDT_data2tag cascade constraints;
drop   table WMDT_data cascade constraints;
create table WMDT_data (
  WMDT_data_id   number(10),
VERS       number(10),
TYP       varchar2(4),
IW       number(10),
X0       float(63),
LAYMDT       number(10),
TUBPIT       float(63),
TUBRAD       float(63),
TUBSTA       float(63),
TUBDEA       float(63),
TUBXCO_0       float(63),
TUBXCO_1       float(63),
TUBXCO_2       float(63),
TUBXCO_3       float(63),
TUBYCO_0       float(63),
TUBYCO_1       float(63),
TUBYCO_2       float(63),
TUBYCO_3       float(63),
TUBWAL       float(63)
) ;

alter table WMDT_data add constraint WMDT_data_pk
primary key (WMDT_data_id);

create table WMDT_data2tag (
  WMDT_vers      varchar2(255),
  WMDT_data_id   number(10)
) ;

alter table WMDT_data2tag add constraint WMDT_data2tag_pk
primary key (WMDT_vers, WMDT_data_id);
alter table WMDT_data2tag add constraint WMDT_data2tag_vers_fk
foreign key (WMDT_vers) references hvs_nodevers (vers_name);
alter table WMDT_data2tag add constraint WMDT_data2tag_data_fk
foreign key (WMDT_data_id) references WMDT_data (WMDT_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WMDT';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WMDT';
delete from hvs_node     where NODE_NAME = 'WMDT';

insert into hvs_node     values ('WMDT','MuonSpectrometer', '');
insert into hvs_nodevers values ('WMDT', 'WMDT-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WMDT','WMDT-07');
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    0,
     7,'MDT',     1,     0.0900,     4,     3.0035,     1.4600,    12.3068,
     2.7000,     3.0035,     1.5017,     3.0035,     1.5017,     3.0017,
     5.6029,     8.2040,    10.8051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    0);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    1,
     7,'MDT',     2,     0.0900,     4,     3.0035,     1.4600,    12.3068,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     1.5017,
     4.1029,     6.7040,     9.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    1);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    2,
     7,'MDT',     3,     0.0800,     3,     3.0035,     1.4600,     9.7057,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     3.0017,
     5.6029,     8.2040,     9.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    2);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    3,
     7,'MDT',     4,     0.0800,     3,     3.0035,     1.4600,     9.7057,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     1.5017,
     4.1029,     6.7040,     9.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    3);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    4,
     7,'MDT',     5,     0.0900,     4,     3.0035,     1.4600,    13.8818,
     2.7000,     3.0035,     1.5017,     3.0035,     1.5017,     4.5768,
     7.1779,     9.7790,    12.3801,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    4);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    5,
     7,'MDT',     6,     0.0900,     4,     3.0035,     1.4600,    13.8818,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     1.5017,
     4.1029,     6.7040,     9.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    5);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    6,
     7,'MDT',     7,     0.0800,     3,     3.0035,     1.4600,    11.2807,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     4.5768,
     7.1779,     9.7790,     9.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    6);
insert into WMDT_data (
WMDT_data_id,
VERS,
TYP,
IW,
X0,
LAYMDT,
TUBPIT,
TUBRAD,
TUBSTA,
TUBDEA,
TUBXCO_0,TUBXCO_1,TUBXCO_2,TUBXCO_3,
TUBYCO_0,TUBYCO_1,TUBYCO_2,TUBYCO_3,
TUBWAL
) values (    7,
     7,'MDT',     8,     0.0900,     4,     3.0035,     1.4600,    13.8068,
     2.7000,     1.5017,     3.0035,     1.5017,     3.0035,     4.5018,
     7.1029,     9.7040,    12.3051,     0.0400
);
insert into WMDT_data2tag values ('WMDT-07',    7);

comment on column WMDT_data.WMDT_data_id is 'Unique identifier';
comment on column WMDT_data.VERS       is 'VERSION                                         ';
comment on column WMDT_data.TYP        is 'NAME                                            ';
comment on column WMDT_data.IW         is 'INDEX                                           ';
comment on column WMDT_data.X0         is 'X0                                              ';
comment on column WMDT_data.LAYMDT     is 'MAXIMUM LAYER NUMBER                            ';
comment on column WMDT_data.TUBPIT     is 'PITCH BETWEEN TUBE                              ';
comment on column WMDT_data.TUBRAD     is 'RADIUS OF TUBE                                  ';
comment on column WMDT_data.TUBSTA     is 'THICKNESS OF TUBE                               ';
comment on column WMDT_data.TUBDEA     is 'DEAD LENGTH IN TUBES                            ';
comment on column WMDT_data.TUBXCO_0   is 'Y TUBE POSITION                                 ';
comment on column WMDT_data.TUBYCO_0   is 'X TUBE POSITION                                 ';
comment on column WMDT_data.TUBWAL     is 'TUBE WALL THICKNESS                             ';
