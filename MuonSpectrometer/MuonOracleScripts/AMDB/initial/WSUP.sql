SET echo OFF;
set linesize 132;
drop   table WSUP_data2tag cascade constraints;
drop   table WSUP_data cascade constraints;
create table WSUP_data (
  WSUP_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NXXSUP       number(10),
NZZSUP       number(10),
X0       float(63),
THICKN       float(63),
XXSUP_0       float(63),
XXSUP_1       float(63),
XXSUP_2       float(63),
XXSUP_3       float(63),
ZZSUP_0       float(63),
ZZSUP_1       float(63),
ZZSUP_2       float(63),
ZZSUP_3       float(63)
) ;

alter table WSUP_data add constraint WSUP_data_pk
primary key (WSUP_data_id);

create table WSUP_data2tag (
  WSUP_vers      varchar2(255),
  WSUP_data_id   number(10)
) ;

alter table WSUP_data2tag add constraint WSUP_data2tag_pk
primary key (WSUP_vers, WSUP_data_id);
alter table WSUP_data2tag add constraint WSUP_data2tag_vers_fk
foreign key (WSUP_vers) references hvs_nodevers (vers_name);
alter table WSUP_data2tag add constraint WSUP_data2tag_data_fk
foreign key (WSUP_data_id) references WSUP_data (WSUP_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WSUP';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WSUP';
delete from hvs_node     where NODE_NAME = 'WSUP';

insert into hvs_node     values ('WSUP','MuonSpectrometer', '');
insert into hvs_nodevers values ('WSUP', 'WSUP-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WSUP','WSUP-07');
insert into WSUP_data (
WSUP_data_id,
VERS,
JSTA,
NXXSUP,
NZZSUP,
X0,
THICKN,
XXSUP_0,XXSUP_1,XXSUP_2,XXSUP_3,
ZZSUP_0,ZZSUP_1,ZZSUP_2,ZZSUP_3
) values (    0,
     7,     1,     2,     1,     0.0000,     0.4000,     0.2000,     9.6000,
     0.0000,     0.0000,     5.0000,     0.0000,     0.0000,     0.0000
);
insert into WSUP_data2tag values ('WSUP-07',    0);
insert into WSUP_data (
WSUP_data_id,
VERS,
JSTA,
NXXSUP,
NZZSUP,
X0,
THICKN,
XXSUP_0,XXSUP_1,XXSUP_2,XXSUP_3,
ZZSUP_0,ZZSUP_1,ZZSUP_2,ZZSUP_3
) values (    1,
     7,     2,     2,     1,     0.0000,     0.4000,     0.2000,     9.6000,
     0.0000,     0.0000,    -5.0000,     0.0000,     0.0000,     0.0000
);
insert into WSUP_data2tag values ('WSUP-07',    1);
insert into WSUP_data (
WSUP_data_id,
VERS,
JSTA,
NXXSUP,
NZZSUP,
X0,
THICKN,
XXSUP_0,XXSUP_1,XXSUP_2,XXSUP_3,
ZZSUP_0,ZZSUP_1,ZZSUP_2,ZZSUP_3
) values (    2,
     7,     3,     4,     4,     0.0000,     0.4000,     0.2000,     0.8000,
     4.6000,     5.0000,     0.2000,     5.0000,     6.4000,     0.4000
);
insert into WSUP_data2tag values ('WSUP-07',    2);

comment on column WSUP_data.WSUP_data_id is 'Unique identifier';
comment on column WSUP_data.VERS       is 'VERSION                                         ';
comment on column WSUP_data.JSTA       is 'INDEX                                           ';
comment on column WSUP_data.NXXSUP     is 'MAX NB. FOR X FRAGMENTS                         ';
comment on column WSUP_data.NZZSUP     is 'MAX NB. FOR Z FRAGMENTS                         ';
comment on column WSUP_data.X0         is 'X0                                              ';
comment on column WSUP_data.THICKN     is 'THICKNESS                                       ';
comment on column WSUP_data.XXSUP_0    is 'X DIMENSION                                     ';
comment on column WSUP_data.ZZSUP_0    is 'Z DIMENSION                                     ';
