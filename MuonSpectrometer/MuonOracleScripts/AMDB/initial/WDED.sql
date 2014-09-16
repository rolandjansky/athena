SET echo OFF;
set linesize 132;
drop   table WDED_data2tag cascade constraints;
drop   table WDED_data cascade constraints;
create table WDED_data (
  WDED_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NB       number(10),
X0       float(63),
AUPHCB       float(63),
TCKDED       float(63)
) ;

alter table WDED_data add constraint WDED_data_pk
primary key (WDED_data_id);

create table WDED_data2tag (
  WDED_vers      varchar2(255),
  WDED_data_id   number(10)
) ;

alter table WDED_data2tag add constraint WDED_data2tag_pk
primary key (WDED_vers, WDED_data_id);
alter table WDED_data2tag add constraint WDED_data2tag_vers_fk
foreign key (WDED_vers) references hvs_nodevers (vers_name);
alter table WDED_data2tag add constraint WDED_data2tag_data_fk
foreign key (WDED_data_id) references WDED_data (WDED_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WDED';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WDED';
delete from hvs_node     where NODE_NAME = 'WDED';

insert into hvs_node     values ('WDED','MuonSpectrometer', '');
insert into hvs_nodevers values ('WDED', 'WDED-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WDED','WDED-07');
insert into WDED_data (
WDED_data_id,
VERS,
JSTA,
NB,
X0,
AUPHCB,
TCKDED
) values (    0,
     7,     1,     1,     0.0010,     5.0000,     0.0300
);
insert into WDED_data2tag values ('WDED-07',    0);
insert into WDED_data (
WDED_data_id,
VERS,
JSTA,
NB,
X0,
AUPHCB,
TCKDED
) values (    1,
     7,     2,     1,     0.0010,     6.0000,     0.0300
);
insert into WDED_data2tag values ('WDED-07',    1);

comment on column WDED_data.WDED_data_id is 'Unique identifier';
comment on column WDED_data.VERS       is 'VERSION                                         ';
comment on column WDED_data.JSTA       is 'INDEX                                           ';
comment on column WDED_data.NB         is 'NUMBER OF DETAILS                               ';
comment on column WDED_data.X0         is 'X0                                              ';
comment on column WDED_data.AUPHCB     is 'HONEYCOMB THICKNESS                             ';
comment on column WDED_data.TCKDED     is 'ALUMINIUM THICKNESS                             ';
