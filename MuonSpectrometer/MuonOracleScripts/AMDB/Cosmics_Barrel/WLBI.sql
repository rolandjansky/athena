SET echo OFF;
set linesize 132;
drop   table WLBI_data2tag cascade constraints;
drop   table WLBI_data cascade constraints;
create table WLBI_data (
  WLBI_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NUM       number(10),
HEIGHT       float(63),
THICKNESS       float(63)
) tablespace data02;

alter table WLBI_data add constraint WLBI_data_pk
primary key (WLBI_data_id)
using index tablespace indx02;

create table WLBI_data2tag (
  WLBI_vers      varchar2(255),
  WLBI_data_id   number(10)
) tablespace data02;

alter table WLBI_data2tag add constraint WLBI_data2tag_pk
primary key (WLBI_vers, WLBI_data_id)
using index tablespace indx02;
alter table WLBI_data2tag add constraint WLBI_data2tag_vers_fk
foreign key (WLBI_vers) references hvs_nodevers (vers_name);
alter table WLBI_data2tag add constraint WLBI_data2tag_data_fk
foreign key (WLBI_data_id) references WLBI_data (WLBI_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WLBI';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WLBI';
delete from hvs_node     where NODE_NAME = 'WLBI';

insert into hvs_node     values ('WLBI','MuonSpectrometer', '');
insert into hvs_nodevers values ('WLBI', 'WLBI-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WLBI','WLBI-07');
insert into WLBI_data (
WLBI_data_id,
VERS,
JSTA,
NUM,
HEIGHT,
THICKNESS
) values (    0,
     7,     1,     1,     7.5000,     0.4000
);
insert into WLBI_data2tag values ('WLBI-07',    0);
insert into WLBI_data (
WLBI_data_id,
VERS,
JSTA,
NUM,
HEIGHT,
THICKNESS
) values (    1,
     7,     2,     1,    15.0000,     0.4000
);
insert into WLBI_data2tag values ('WLBI-07',    1);
insert into WLBI_data (
WLBI_data_id,
VERS,
JSTA,
NUM,
HEIGHT,
THICKNESS
) values (    2,
     7,     3,     1,    12.9400,     0.3000
);
insert into WLBI_data2tag values ('WLBI-07',    2);
insert into WLBI_data (
WLBI_data_id,
VERS,
JSTA,
NUM,
HEIGHT,
THICKNESS
) values (    3,
     7,     4,     1,     8.0400,     0.3000
);
insert into WLBI_data2tag values ('WLBI-07',    3);

comment on column WLBI_data.WLBI_data_id is 'Unique identifier';
comment on column WLBI_data.VERS       is 'VERSION                                         ';
comment on column WLBI_data.JSTA       is 'INDEX                                           ';
comment on column WLBI_data.NUM        is 'NUMBER OF OBJECTS                               ';
comment on column WLBI_data.HEIGHT     is 'HEIGHT                                          ';
comment on column WLBI_data.THICKNESS  is 'WALL THICKNESS                                  ';
