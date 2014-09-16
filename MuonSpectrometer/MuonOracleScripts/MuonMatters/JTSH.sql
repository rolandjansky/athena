SET echo OFF;
set linesize 132;
drop   table JTSH_data2tag cascade constraints;
drop   table JTSH_data cascade constraints;
create table JTSH_data (
  JTSH_data_id   number(10),
OPLENGTH       float(63),
OPINNRAD       float(63),
OPOUTRAD       float(63),
IPLENGTH       float(63),
IPINNRA1       float(63),
IPOUTRAD       float(63),
IPTILTAN       float(63),
IPZOFFSE       float(63),
PRLENGTH       float(63),
PRINNRAD       float(63),
PROUTRAD       float(63),
PREXTTAN       float(63),
PRIPEXLE       float(63),
ZPOSITIO       float(63),
YROTATIO       float(63),
IPINNRA2       float(63),
PREXTZPO       float(63),
OPZPOSI1       float(63),
OPZPOSI2       float(63),
IPZPOSI1       float(63),
IPZPOSI2       float(63),
PRZPOSI1       float(63),
PRZPOSI2       float(63),
PRZPOSI3       float(63)
) ;

alter table JTSH_data add constraint JTSH_data_pk
primary key (JTSH_data_id);

create table JTSH_data2tag (
  JTSH_vers      varchar2(255),
  JTSH_data_id   number(10)
) ;

alter table JTSH_data2tag add constraint JTSH_data2tag_pk
primary key (JTSH_vers, JTSH_data_id);
alter table JTSH_data2tag add constraint JTSH_data2tag_vers_fk
foreign key (JTSH_vers) references hvs_nodevers (vers_name);
alter table JTSH_data2tag add constraint JTSH_data2tag_data_fk
foreign key (JTSH_data_id) references JTSH_data (JTSH_data_id);

delete from hvs_relation where CHILD_NODENAME = 'JTSH';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'JTSH';
delete from hvs_node     where NODE_NAME = 'JTSH';

insert into hvs_node     values ('JTSH','MuonSpectrometer', '');
insert into hvs_nodevers values ('JTSH', 'JTSH-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','JTSH','JTSH-01');
insert into JTSH_data (
JTSH_data_id,
OPLENGTH,
OPINNRAD,
OPOUTRAD,
IPLENGTH,
IPINNRA1,
IPOUTRAD,
IPTILTAN,
IPZOFFSE,
PRLENGTH,
PRINNRAD,
PROUTRAD,
PREXTTAN,
PRIPEXLE,
ZPOSITIO,
YROTATIO,
IPINNRA2,
PREXTZPO,
OPZPOSI1,
OPZPOSI2,
IPZPOSI1,
IPZPOSI2,
PRZPOSI1,
PRZPOSI2,
PRZPOSI3
) values (    1,
  4740.0000,   562.5000,   789.0000,  3685.0000,   171.0000,   548.0000,
     1.1000,  1000.0000,  4560.0000,   809.0000,   849.0000,    18.0000,
   205.0000,  7894.0000,   180.0000,   262.0126,   123.1073,   205.0000,
  4945.0000,  1000.0000,  4685.0000,   261.8927,   385.0000,  4945.0000
);
insert into JTSH_data2tag values ('JTSH-01',    1);

comment on column JTSH_data.JTSH_data_id is 'Unique identifier';
comment on column JTSH_data.OPLENGTH   is 'OUTERPLUGS_LENGTH                               ';
comment on column JTSH_data.OPINNRAD   is 'OUTERPLUGS_INNERRADIUS                          ';
comment on column JTSH_data.OPOUTRAD   is 'OUTERPLUGS_OUTERRADIUS                          ';
comment on column JTSH_data.IPLENGTH   is 'INNERPLUGS_LENGTH                               ';
comment on column JTSH_data.IPINNRA1   is 'INNERPLUGS_INNERRADIUS1                         ';
comment on column JTSH_data.IPOUTRAD   is 'INNERPLUGS_OUTERRADIUS                          ';
comment on column JTSH_data.IPTILTAN   is 'INNERPLUGS_TILTANGLE                            ';
comment on column JTSH_data.IPZOFFSE   is 'INNERPLUGS_ZOFFSET                              ';
comment on column JTSH_data.PRLENGTH   is 'POLYRINGS_LENGTH                                ';
comment on column JTSH_data.PRINNRAD   is 'POLYRINGS_INNERRADIUS                           ';
comment on column JTSH_data.PROUTRAD   is 'POLYRINGS_OUTERRADIUS                           ';
comment on column JTSH_data.PREXTTAN   is 'POLYRINGS_EXTENSION_TILTANGLE                   ';
comment on column JTSH_data.PRIPEXLE   is 'POLYRINGS_IPEXTENSION_LENGTH                    ';
comment on column JTSH_data.ZPOSITIO   is 'SHIELD_Z                                        ';
comment on column JTSH_data.YROTATIO   is 'SHIELD_ROTY                                     ';
comment on column JTSH_data.IPINNRA2   is 'INNERPLUGS_INNERRADIUS2                         ';
comment on column JTSH_data.PREXTZPO   is 'POLYRINGS_EXTENSION_Z                           ';
comment on column JTSH_data.OPZPOSI1   is 'OUTERPLUGS_Z1                                   ';
comment on column JTSH_data.OPZPOSI2   is 'OUTERPLUGS_Z2                                   ';
comment on column JTSH_data.IPZPOSI1   is 'INNERPLUGS_Z1                                   ';
comment on column JTSH_data.IPZPOSI2   is 'INNERPLUGS_Z2                                   ';
comment on column JTSH_data.PRZPOSI1   is 'POLYRINGS_Z1                                    ';
comment on column JTSH_data.PRZPOSI2   is 'POLYRINGS_Z2                                    ';
comment on column JTSH_data.PRZPOSI3   is 'POLYRINGS_Z3                                    ';
