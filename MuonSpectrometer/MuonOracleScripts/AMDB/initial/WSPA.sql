SET echo OFF;
set linesize 132;
drop   table WSPA_data2tag cascade constraints;
drop   table WSPA_data cascade constraints;
create table WSPA_data (
  WSPA_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NB       number(10),
X0       float(63),
TCKSPA       float(63)
) ;

alter table WSPA_data add constraint WSPA_data_pk
primary key (WSPA_data_id);

create table WSPA_data2tag (
  WSPA_vers      varchar2(255),
  WSPA_data_id   number(10)
) ;

alter table WSPA_data2tag add constraint WSPA_data2tag_pk
primary key (WSPA_vers, WSPA_data_id);
alter table WSPA_data2tag add constraint WSPA_data2tag_vers_fk
foreign key (WSPA_vers) references hvs_nodevers (vers_name);
alter table WSPA_data2tag add constraint WSPA_data2tag_data_fk
foreign key (WSPA_data_id) references WSPA_data (WSPA_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WSPA';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WSPA';
delete from hvs_node     where NODE_NAME = 'WSPA';

insert into hvs_node     values ('WSPA','MuonSpectrometer', '');
insert into hvs_nodevers values ('WSPA', 'WSPA-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WSPA','WSPA-07');
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    0,
     7,     1,     1,     0.0010,     0.6500
);
insert into WSPA_data2tag values ('WSPA-07',    0);
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    1,
     7,     2,     1,     0.0010,    17.0000
);
insert into WSPA_data2tag values ('WSPA-07',    1);
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    2,
     7,     3,     1,     0.0010,    22.0000
);
insert into WSPA_data2tag values ('WSPA-07',    2);
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    3,
     7,     4,     1,     0.0010,    31.7000
);
insert into WSPA_data2tag values ('WSPA-07',    3);
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    4,
     7,     5,     1,     0.0010,    12.1000
);
insert into WSPA_data2tag values ('WSPA-07',    4);
insert into WSPA_data (
WSPA_data_id,
VERS,
JSTA,
NB,
X0,
TCKSPA
) values (    5,
     7,     6,     1,     0.0010,     3.9100
);
insert into WSPA_data2tag values ('WSPA-07',    5);

comment on column WSPA_data.WSPA_data_id is 'Unique identifier';
comment on column WSPA_data.VERS       is 'VERSION                                         ';
comment on column WSPA_data.JSTA       is 'JSTA INDEX                                      ';
comment on column WSPA_data.NB         is 'NUMBER OF DETAILS                               ';
comment on column WSPA_data.X0         is 'X0                                              ';
comment on column WSPA_data.TCKSPA     is 'THICKNESS OF SPACER                             ';
