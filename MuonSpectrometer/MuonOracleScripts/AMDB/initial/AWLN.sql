SET echo OFF;
set linesize 132;
drop   table AWLN_data2tag cascade constraints;
drop   table AWLN_data cascade constraints;
create table AWLN_data (
  AWLN_data_id   number(10),
VERS       number(10),
JSTA       number(10),
SPITCH       float(63),
ZPITCH       float(63),
DEDSTR       float(63),
NSREST       number(10),
NZREST       number(10),
SFIRST       float(63),
ZFIRST       float(63),
DEDSEP       float(63),
NSROST       number(10),
NZROST       number(10)
) ;

alter table AWLN_data add constraint AWLN_data_pk
primary key (AWLN_data_id);

create table AWLN_data2tag (
  AWLN_vers      varchar2(255),
  AWLN_data_id   number(10)
) ;

alter table AWLN_data2tag add constraint AWLN_data2tag_pk
primary key (AWLN_vers, AWLN_data_id);
alter table AWLN_data2tag add constraint AWLN_data2tag_vers_fk
foreign key (AWLN_vers) references hvs_nodevers (vers_name);
alter table AWLN_data2tag add constraint AWLN_data2tag_data_fk
foreign key (AWLN_data_id) references AWLN_data (AWLN_data_id);

delete from hvs_relation where CHILD_NODENAME = 'AWLN';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'AWLN';
delete from hvs_node     where NODE_NAME = 'AWLN';

insert into hvs_node     values ('AWLN','MuonSpectrometer', '');
insert into hvs_nodevers values ('AWLN', 'AWLN-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','AWLN','AWLN-07');
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    0,
     7,     1,     3.0500,     2.7200,     0.2000,     2,     1,     8.0000,
     8.0000,     1.0000,    48,    40
);
insert into AWLN_data2tag values ('AWLN-07',    0);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    1,
     7,     2,     3.0500,     2.6400,     0.2000,     2,     1,     8.0000,
     9.0000,     1.0000,    48,    40
);
insert into AWLN_data2tag values ('AWLN-07',    1);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    2,
     7,     3,     3.0500,     3.0200,     0.2000,     2,     1,     8.0000,
     8.8000,     1.0000,    48,    32
);
insert into AWLN_data2tag values ('AWLN-07',    2);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    3,
     7,     4,     3.0500,     2.9300,     0.2000,     2,     1,     8.0000,
     8.2000,     1.0000,    48,    32
);
insert into AWLN_data2tag values ('AWLN-07',    3);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    4,
     7,     5,     3.0500,     2.6500,     0.2000,     2,     1,     8.0000,
     8.0000,     1.0000,    48,    32
);
insert into AWLN_data2tag values ('AWLN-07',    4);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    5,
     7,     6,     3.0500,     3.0300,     0.2000,     2,     1,     8.0000,
     8.4000,     1.0000,    48,    24
);
insert into AWLN_data2tag values ('AWLN-07',    5);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    6,
     7,     7,     3.2600,     3.7000,     0.2000,     1,     1,     8.0000,
     8.0000,     1.0000,    32,     8
);
insert into AWLN_data2tag values ('AWLN-07',    6);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    7,
     7,     8,     3.2600,     2.6500,     0.2000,     1,     1,     8.0000,
     8.0000,     1.0000,    32,    24
);
insert into AWLN_data2tag values ('AWLN-07',    7);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    8,
     7,     9,     2.6600,     2.9400,     0.2000,     2,     2,     8.8000,
     9.0000,     1.0000,    64,    40
);
insert into AWLN_data2tag values ('AWLN-07',    8);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (    9,
     7,    10,     2.6600,     2.6500,     0.2000,     2,     2,     8.8000,
     8.0000,     1.0000,    64,    32
);
insert into AWLN_data2tag values ('AWLN-07',    9);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   10,
     7,    11,     2.6600,     3.0300,     0.2000,     2,     2,     8.8000,
     8.4000,     1.0000,    64,    24
);
insert into AWLN_data2tag values ('AWLN-07',   10);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   11,
     7,    12,     2.6800,     2.8500,     0.2000,     2,     2,     9.6000,
     9.0000,     1.0000,    56,    16
);
insert into AWLN_data2tag values ('AWLN-07',   11);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   12,
     7,    13,     2.8300,     3.4000,     0.2000,     2,     2,     9.4000,
     8.0000,     1.0000,    64,    32
);
insert into AWLN_data2tag values ('AWLN-07',   12);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   13,
     7,    14,     2.8300,     3.0200,     0.2000,     2,     2,     9.4000,
     8.8000,     1.0000,    64,    32
);
insert into AWLN_data2tag values ('AWLN-07',   13);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   14,
     7,    15,     2.8300,     2.6500,     0.2000,     2,     2,     9.4000,
     8.0000,     1.0000,    64,    32
);
insert into AWLN_data2tag values ('AWLN-07',   14);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   15,
     7,    16,     2.8300,     3.0300,     0.2000,     2,     2,     9.4000,
     8.4000,     1.0000,    64,    24
);
insert into AWLN_data2tag values ('AWLN-07',   15);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   16,
     7,    17,     2.8300,     2.6500,     0.2000,     2,     2,     9.4000,
     8.0000,     1.0000,    64,    32
);
insert into AWLN_data2tag values ('AWLN-07',   16);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   17,
     7,    18,     3.0100,     3.4000,     0.2000,     2,     2,     8.5000,
     8.0000,     1.0000,    80,    32
);
insert into AWLN_data2tag values ('AWLN-07',   17);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   18,
     7,    19,     3.0100,     3.5300,     0.2000,     2,     2,     8.5000,
     8.4000,     1.0000,    80,    24
);
insert into AWLN_data2tag values ('AWLN-07',   18);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   19,
     7,    20,     3.0100,     3.0300,     0.2000,     2,     2,     8.5000,
     8.4000,     1.0000,    80,    24
);
insert into AWLN_data2tag values ('AWLN-07',   19);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   20,
     7,    21,     3.0500,     2.9400,     0.2000,     2,     2,     8.0000,
     9.0000,     1.0000,    48,    40
);
insert into AWLN_data2tag values ('AWLN-07',   20);
insert into AWLN_data (
AWLN_data_id,
VERS,
JSTA,
SPITCH,
ZPITCH,
DEDSTR,
NSREST,
NZREST,
SFIRST,
ZFIRST,
DEDSEP,
NSROST,
NZROST
) values (   21,
     7,    22,     2.8300,     2.9400,     0.2000,     2,     2,     9.4000,
     9.0000,     1.0000,    64,    40
);
insert into AWLN_data2tag values ('AWLN-07',   21);

comment on column AWLN_data.AWLN_data_id is 'Unique identifier';
comment on column AWLN_data.VERS       is 'VERSION                                         ';
comment on column AWLN_data.JSTA       is 'JSTA TYPE NUMBER                                ';
comment on column AWLN_data.SPITCH     is 'S-STRIPS PITCH                                  ';
comment on column AWLN_data.ZPITCH     is 'Z-STRIPS PITCH                                  ';
comment on column AWLN_data.DEDSTR     is 'DEAD REAGION BETWEEN STRIP                      ';
comment on column AWLN_data.NSREST     is 'NBER OF S STRIPS READOUTS                       ';
comment on column AWLN_data.NZREST     is 'NBER OF S GAS GAPS                              ';
comment on column AWLN_data.SFIRST     is 'S-PHI STRIP OFFSET                              ';
comment on column AWLN_data.ZFIRST     is 'Z-ETA STRIP OFFSET                              ';
comment on column AWLN_data.DEDSEP     is 'DEAD SEPARATION                                 ';
comment on column AWLN_data.NSROST     is 'NUMBER OF S-PHI READOUT STRIPS                  ';
comment on column AWLN_data.NZROST     is 'NUMBER OF Z-ETA READOUT STRIPS                  ';
