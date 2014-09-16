SET echo OFF;
set linesize 132;
drop   table WCMI_data2tag cascade constraints;
drop   table WCMI_data cascade constraints;
create table WCMI_data (
  WCMI_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NUM       number(10),
HEIGHTNESS       float(63),
LARGENESS       float(63),
THICKNESS       float(63)
) ;

alter table WCMI_data add constraint WCMI_data_pk
primary key (WCMI_data_id);

create table WCMI_data2tag (
  WCMI_vers      varchar2(255),
  WCMI_data_id   number(10)
) ;

alter table WCMI_data2tag add constraint WCMI_data2tag_pk
primary key (WCMI_vers, WCMI_data_id);
alter table WCMI_data2tag add constraint WCMI_data2tag_vers_fk
foreign key (WCMI_vers) references hvs_nodevers (vers_name);
alter table WCMI_data2tag add constraint WCMI_data2tag_data_fk
foreign key (WCMI_data_id) references WCMI_data (WCMI_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WCMI';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WCMI';
delete from hvs_node     where NODE_NAME = 'WCMI';

insert into hvs_node     values ('WCMI','MuonSpectrometer', '');
insert into hvs_nodevers values ('WCMI', 'WCMI-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WCMI','WCMI-07');
insert into WCMI_data (
WCMI_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    0,
     7,     1,     1,    17.0000,     6.0000,     0.6000
);
insert into WCMI_data2tag values ('WCMI-07',    0);
insert into WCMI_data (
WCMI_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    1,
     7,     2,     1,    31.7000,     6.0000,     0.6000
);
insert into WCMI_data2tag values ('WCMI-07',    1);
insert into WCMI_data (
WCMI_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    2,
     7,     3,     1,     0.6500,     0.0000,     0.0000
);
insert into WCMI_data2tag values ('WCMI-07',    2);
insert into WCMI_data (
WCMI_data_id,
VERS,
JSTA,
NUM,
HEIGHTNESS,
LARGENESS,
THICKNESS
) values (    3,
     7,     4,     1,    12.1000,     6.0000,     0.6000
);
insert into WCMI_data2tag values ('WCMI-07',    3);

comment on column WCMI_data.WCMI_data_id is 'Unique identifier';
comment on column WCMI_data.VERS       is 'VERSION                                         ';
comment on column WCMI_data.JSTA       is 'INDEX                                           ';
comment on column WCMI_data.NUM        is 'NUMBER OF OBJECTS                               ';
comment on column WCMI_data.HEIGHTNESS is 'HEIGHT                                          ';
comment on column WCMI_data.LARGENESS  is 'T-SHAPE LARGENESS                               ';
comment on column WCMI_data.THICKNESS  is 'T-SHAPE THICKNESS                               ';
