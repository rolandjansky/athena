SET echo OFF;
set linesize 132;
drop   table WCSC_data2tag cascade constraints;
drop   table WCSC_data cascade constraints;
create table WCSC_data (
  WCSC_data_id   number(10),
VERS       number(10),
JSTA       number(10),
LAYCSC       number(10),
TTOTAL       float(63),
TNOMEX       float(63),
TLAG10       float(63),
WISPA       float(63),
DANCAT       float(63),
PCATRE       float(63),
AZCAT       float(63),
GSTRIP       float(63),
WRESTR       float(63),
WFLSTR       float(63),
TRRWAS       float(63),
WROXA       float(63),
GROXWI       float(63),
WGASBA       float(63),
TGASBA       float(63),
WGASCU       float(63),
TGASCU       float(63),
WFIXWI       float(63),
TFIXWI       float(63),
PBA1WI       float(63),
PBA2WI       float(63),
PBA3WI       float(63),
PSNDCO       float(63)
) ;

alter table WCSC_data add constraint WCSC_data_pk
primary key (WCSC_data_id);

create table WCSC_data2tag (
  WCSC_vers      varchar2(255),
  WCSC_data_id   number(10)
) ;

alter table WCSC_data2tag add constraint WCSC_data2tag_pk
primary key (WCSC_vers, WCSC_data_id);
alter table WCSC_data2tag add constraint WCSC_data2tag_vers_fk
foreign key (WCSC_vers) references hvs_nodevers (vers_name);
alter table WCSC_data2tag add constraint WCSC_data2tag_data_fk
foreign key (WCSC_data_id) references WCSC_data (WCSC_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WCSC';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WCSC';
delete from hvs_node     where NODE_NAME = 'WCSC';

insert into hvs_node     values ('WCSC','MuonSpectrometer', '');
insert into hvs_nodevers values ('WCSC', 'WCSC-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WCSC','WCSC-07');
insert into WCSC_data (
WCSC_data_id,
VERS,
JSTA,
LAYCSC,
TTOTAL,
TNOMEX,
TLAG10,
WISPA,
DANCAT,
PCATRE,
AZCAT,
GSTRIP,
WRESTR,
WFLSTR,
TRRWAS,
WROXA,
GROXWI,
WGASBA,
TGASBA,
WGASCU,
TGASCU,
WFIXWI,
TFIXWI,
PBA1WI,
PBA2WI,
PBA3WI,
PSNDCO
) values (    0,
     7,     1,     4,    12.3500,     1.9050,     0.0820,     0.2500,
     0.2500,     0.5560,     1.2920,     0.0250,     0.1600,     1.2520,
     0.0017,     0.4800,     0.1500,     0.5000,     2.0300,     0.7620,
     0.1500,     2.2860,     0.2500,     3.2100,     2.0300,     1.1800,
     0.0000
);
insert into WCSC_data2tag values ('WCSC-07',    0);
insert into WCSC_data (
WCSC_data_id,
VERS,
JSTA,
LAYCSC,
TTOTAL,
TNOMEX,
TLAG10,
WISPA,
DANCAT,
PCATRE,
AZCAT,
GSTRIP,
WRESTR,
WFLSTR,
TRRWAS,
WROXA,
GROXWI,
WGASBA,
TGASBA,
WGASCU,
TGASCU,
WFIXWI,
TFIXWI,
PBA1WI,
PBA2WI,
PBA3WI,
PSNDCO
) values (    1,
     7,     2,     4,    12.3500,     1.9050,     0.0820,     0.2500,
     0.2500,     0.5310,     2.1000,     0.0250,     0.1520,     2.0600,
     0.0017,     0.4800,     0.1500,     0.5000,     2.0300,     0.7620,
     0.1500,     2.2860,     0.2500,     3.2100,     2.0300,     1.1800,
     0.0000
);
insert into WCSC_data2tag values ('WCSC-07',    1);

comment on column WCSC_data.WCSC_data_id is 'Unique identifier';
comment on column WCSC_data.VERS       is 'VERSION                                         ';
comment on column WCSC_data.JSTA       is 'JSTA TYPE NUMBER                                ';
comment on column WCSC_data.LAYCSC     is 'NBER OF CSCS LAYERS                             ';
comment on column WCSC_data.TTOTAL     is 'TOTAL THICKNESS                                 ';
comment on column WCSC_data.TNOMEX     is 'NOMEX HONEYCOMB THICKNESS                       ';
comment on column WCSC_data.TLAG10     is 'G10 LAMINATES THICKNESS                         ';
comment on column WCSC_data.WISPA      is 'WIRE SPACING                                    ';
comment on column WCSC_data.DANCAT     is 'ANODE-CATHODE DISTANCE                          ';
comment on column WCSC_data.PCATRE     is 'CATHODE READOUT PITCH                           ';
comment on column WCSC_data.AZCAT      is 'CATHODE READOUT PITCH, AZIMUTAL                 ';
comment on column WCSC_data.GSTRIP     is 'GAP BETWEEN CATHODE STRIPS                      ';
comment on column WCSC_data.WRESTR     is 'WIDTH OF READOUT STRIPS                         ';
comment on column WCSC_data.WFLSTR     is 'WIDTH OF FLOATING STRIPS                        ';
comment on column WCSC_data.TRRWAS     is 'RIGIT RECTANGULAR WASHER THICKNES               ';
comment on column WCSC_data.WROXA      is 'ROXACELL WIDTH                                  ';
comment on column WCSC_data.GROXWI     is 'ROXACELL AND WIRE BAR GAP                       ';
comment on column WCSC_data.WGASBA     is 'FULL GAS GAP BAR WIDTH                          ';
comment on column WCSC_data.TGASBA     is 'FULL GAS GAP BAR THICK.                         ';
comment on column WCSC_data.WGASCU     is 'CUTS GAS GAP BAR WIDTH                          ';
comment on column WCSC_data.TGASCU     is 'CUTS GAS GAP BAR THICK.                         ';
comment on column WCSC_data.WFIXWI     is 'FULL WIRE FIX. BAR WID.                         ';
comment on column WCSC_data.TFIXWI     is 'FULL WIRE FIX. BAR THICK.                       ';
comment on column WCSC_data.PBA1WI     is 'WIRE BAR POSITION                               ';
comment on column WCSC_data.PBA2WI     is 'WIRE BAR POSITION                               ';
comment on column WCSC_data.PBA3WI     is 'WIRE BAR POSITION                               ';
comment on column WCSC_data.PSNDCO     is '2ND COORDINATE PITCH                            ';
