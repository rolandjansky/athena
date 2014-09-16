SET echo OFF;
set linesize 132;
drop   table MSYS_data2tag cascade constraints;
drop   table MSYS_data cascade constraints;
create table MSYS_data (
  MSYS_data_id   number(10),
VERS       number(10),
AMDB       varchar2(4),
TEST       varchar2(4),
BAINNRAD       float(63),
BAINTRAD       float(63),
BALENGTH       float(63),
LENGTH       float(63),
INRADIUS       float(63),
OURADIUS       float(63),
ENFRFACE       float(63)
) ;

alter table MSYS_data add constraint MSYS_data_pk
primary key (MSYS_data_id);

create table MSYS_data2tag (
  MSYS_vers      varchar2(255),
  MSYS_data_id   number(10)
) ;

alter table MSYS_data2tag add constraint MSYS_data2tag_pk
primary key (MSYS_vers, MSYS_data_id);
alter table MSYS_data2tag add constraint MSYS_data2tag_vers_fk
foreign key (MSYS_vers) references hvs_nodevers (vers_name);
alter table MSYS_data2tag add constraint MSYS_data2tag_data_fk
foreign key (MSYS_data_id) references MSYS_data (MSYS_data_id);

delete from hvs_relation where CHILD_NODENAME = 'MSYS';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'MSYS';
delete from hvs_node     where NODE_NAME = 'MSYS';

insert into hvs_node     values ('MSYS','MuonSpectrometer', '');
insert into hvs_nodevers values ('MSYS', 'MSYS-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','MSYS','MSYS-07');
insert into MSYS_data (
MSYS_data_id,
VERS,
AMDB,
TEST,
BAINNRAD,
BAINTRAD,
BALENGTH,
LENGTH,
INRADIUS,
OURADIUS,
ENFRFACE
) values (    0,
     7,'R.01','01',   430.0000,   383.0000,   653.0000,  2180.0000,
     7.0000,  1300.0000,   674.0000
);
insert into MSYS_data2tag values ('MSYS-07',    0);

comment on column MSYS_data.MSYS_data_id is 'Unique identifier';
comment on column MSYS_data.VERS       is 'VERSION                                         ';
comment on column MSYS_data.AMDB       is 'AMDB NAME                                       ';
comment on column MSYS_data.TEST       is 'EXTENSION NAME                                  ';
comment on column MSYS_data.BAINNRAD   is 'BARREL INNER RADIUS                             ';
comment on column MSYS_data.BAINTRAD   is 'BARREL INTERNAL RADIUS                          ';
comment on column MSYS_data.BALENGTH   is 'BARREL LENGTH                                   ';
comment on column MSYS_data.LENGTH     is 'LENGTH                                          ';
comment on column MSYS_data.INRADIUS   is 'INNER RADIUS                                    ';
comment on column MSYS_data.OURADIUS   is 'OUTER RADIUS                                    ';
comment on column MSYS_data.ENFRFACE   is 'ENDCAP FRONT FACE                               ';
