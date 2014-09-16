SET echo OFF;
set linesize 132;
drop   table GENV_data2tag cascade constraints;
drop   table GENV_data cascade constraints;
create table GENV_data (
  GENV_data_id   number(10),
PI       float(63),
EPS       float(63),
PIS180       float(63),
SI5       float(63),
CO5       float(63),
TA15       float(63),
TA20       float(63),
SI225       float(63),
CO225       float(63),
TA225       float(63),
TA375       float(63),
SI45       float(63),
CO45       float(63),
TA45       float(63),
SI525       float(63),
CO525       float(63),
TA55       float(63),
SI675       float(63),
CO675       float(63),
TA675       float(63),
TA77       float(63)
) ;

alter table GENV_data add constraint GENV_data_pk
primary key (GENV_data_id);

create table GENV_data2tag (
  GENV_vers      varchar2(255),
  GENV_data_id   number(10)
) ;

alter table GENV_data2tag add constraint GENV_data2tag_pk
primary key (GENV_vers, GENV_data_id);
alter table GENV_data2tag add constraint GENV_data2tag_vers_fk
foreign key (GENV_vers) references hvs_nodevers (vers_name);
alter table GENV_data2tag add constraint GENV_data2tag_data_fk
foreign key (GENV_data_id) references GENV_data (GENV_data_id);

delete from hvs_relation where CHILD_NODENAME = 'GENV';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'GENV';
delete from hvs_node     where NODE_NAME = 'GENV';

insert into hvs_node     values ('GENV','MuonSpectrometer', '');
insert into hvs_nodevers values ('GENV', 'GENV-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','GENV','GENV-01');
insert into GENV_data (
GENV_data_id,
PI,
EPS,
PIS180,
SI5,
CO5,
TA15,
TA20,
SI225,
CO225,
TA225,
TA375,
SI45,
CO45,
TA45,
SI525,
CO525,
TA55,
SI675,
CO675,
TA675,
TA77
) values (    1,
     3.1416,     0.1000,     0.0175,     0.0872,     0.9962,     0.2679,
     0.3640,     0.3827,     0.9239,     0.4142,     0.7673,     0.7071,
     0.7071,     1.0000,     0.7934,     0.6088,     1.4281,     0.9239,
     0.3827,     2.4142,     4.3315
);
insert into GENV_data2tag values ('GENV-01',    1);

comment on column GENV_data.GENV_data_id is 'Unique identifier';
comment on column GENV_data.PI         is 'PI                                              ';
comment on column GENV_data.EPS        is 'EPS                                             ';
comment on column GENV_data.PIS180     is 'PIS180                                          ';
comment on column GENV_data.SI5        is 'SI5                                             ';
comment on column GENV_data.CO5        is 'CO5                                             ';
comment on column GENV_data.TA15       is 'TA15                                            ';
comment on column GENV_data.TA20       is 'TA20                                            ';
comment on column GENV_data.SI225      is 'SI225                                           ';
comment on column GENV_data.CO225      is 'CO225                                           ';
comment on column GENV_data.TA225      is 'TA225                                           ';
comment on column GENV_data.TA375      is 'TA375                                           ';
comment on column GENV_data.SI45       is 'SI45                                            ';
comment on column GENV_data.CO45       is 'CO45                                            ';
comment on column GENV_data.TA45       is 'TA45                                            ';
comment on column GENV_data.SI525      is 'SI525                                           ';
comment on column GENV_data.CO525      is 'CO525                                           ';
comment on column GENV_data.TA55       is 'TA55                                            ';
comment on column GENV_data.SI675      is 'SI675                                           ';
comment on column GENV_data.CO675      is 'CO675                                           ';
comment on column GENV_data.TA675      is 'TA675                                           ';
comment on column GENV_data.TA77       is 'TA77                                            ';
