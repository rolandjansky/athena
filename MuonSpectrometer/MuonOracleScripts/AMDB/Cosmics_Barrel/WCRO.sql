SET echo OFF;
set linesize 132;
drop   table WCRO_data2tag cascade constraints;
drop   table WCRO_data cascade constraints;
create table WCRO_data (
  WCRO_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NUM       number(10),
HEIGHTNESS       float(63),
LARGENESS       float(63),
THICKNESS       float(63)
) tablespace data02;

alter table WCRO_data add constraint WCRO_data_pk
primary key (WCRO_data_id)
using index tablespace indx02;

create table WCRO_data2tag (
  WCRO_vers      varchar2(255),
  WCRO_data_id   number(10)
) tablespace data02;

alter table WCRO_data2tag add constraint WCRO_data2tag_pk
primary key (WCRO_vers, WCRO_data_id)
using index tablespace indx02;
alter table WCRO_data2tag add constraint WCRO_data2tag_vers_fk
foreign key (WCRO_vers) references hvs_nodevers (vers_name);
alter table WCRO_data2tag add constraint WCRO_data2tag_data_fk
foreign key (WCRO_data_id) references WCRO_data (WCRO_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WCRO';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WCRO';
delete from hvs_node     where NODE_NAME = 'WCRO';

insert into hvs_node     values ('WCRO','MuonSpectrometer', '');
insert into hvs_nodevers values ('WCRO', 'WCRO-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WCRO','WCRO-07');
insert into WCRO_data (
WCRO_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    0,
     7,     1,     1,    17.0000,     6.0000,     0.6000
);
insert into WCRO_data2tag values ('WCRO-07',    0);
insert into WCRO_data (
WCRO_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    1,
     7,     2,     1,    31.7000,     6.0000,     0.6000
);
insert into WCRO_data2tag values ('WCRO-07',    1);
insert into WCRO_data (
WCRO_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    2,
     7,     3,     1,     5.5000,     0.0000,     0.0000
);
insert into WCRO_data2tag values ('WCRO-07',    2);
insert into WCRO_data (
WCRO_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    3,
     7,     4,     1,    12.1000,     6.0000,     0.6000
);
insert into WCRO_data2tag values ('WCRO-07',    3);

comment on column WCRO_data.WCRO_data_id is 'Unique identifier';
comment on column WCRO_data.VERS       is 'VERSION                                         ';
comment on column WCRO_data.JSTA       is 'INDEX                                           ';
comment on column WCRO_data.NUM        is 'NUMBER OF OBJECTS                               ';
comment on column WCRO_data.HEIGHTNESS is 'HEIGHT                                          ';
comment on column WCRO_data.LARGENESS  is 'T-SHAPE LARGENESS                               ';
comment on column WCRO_data.THICKNESS  is 'T-SHAPE THICKNESS                               ';
