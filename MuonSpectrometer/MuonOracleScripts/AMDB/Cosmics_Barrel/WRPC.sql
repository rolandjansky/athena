SET echo OFF;
set linesize 132;
drop   table WRPC_data2tag cascade constraints;
drop   table WRPC_data cascade constraints;
create table WRPC_data (
  WRPC_data_id   number(10),
VERS       number(10),
NVRS       number(10),
LAYRPC       number(10),
TCKRLA       float(63),
TOTTCK       float(63),
TCKFSP       float(63),
ACKFSP       float(63),
TLOHCB       float(63),
ALOHCB       float(63),
TCKBAK       float(63),
TCKGAS       float(63),
TCKSSU       float(63),
TCKSTR       float(63),
SDEDMI       float(63),
ZDEDMI       float(63),
SPDIAM       float(63),
SPPITC       float(63),
STROFF_0       float(63),
STROFF_1       float(63),
STROFF_2       float(63)
) tablespace data02;

alter table WRPC_data add constraint WRPC_data_pk
primary key (WRPC_data_id)
using index tablespace indx02;

create table WRPC_data2tag (
  WRPC_vers      varchar2(255),
  WRPC_data_id   number(10)
) tablespace data02;

alter table WRPC_data2tag add constraint WRPC_data2tag_pk
primary key (WRPC_vers, WRPC_data_id)
using index tablespace indx02;
alter table WRPC_data2tag add constraint WRPC_data2tag_vers_fk
foreign key (WRPC_vers) references hvs_nodevers (vers_name);
alter table WRPC_data2tag add constraint WRPC_data2tag_data_fk
foreign key (WRPC_data_id) references WRPC_data (WRPC_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WRPC';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WRPC';
delete from hvs_node     where NODE_NAME = 'WRPC';

insert into hvs_node     values ('WRPC','MuonSpectrometer', '');
insert into hvs_nodevers values ('WRPC', 'WRPC-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WRPC','WRPC-07');
insert into WRPC_data (
WRPC_data_id,
VERS,
NVRS,
LAYRPC,
TCKRLA,
TOTTCK,
TCKFSP,
ACKFSP,
TLOHCB,
ALOHCB,
TCKBAK,
TCKGAS,
TCKSSU,
TCKSTR,
SDEDMI,
ZDEDMI,
SPDIAM,
SPPITC,
STROFF_0,STROFF_1,STROFF_2
) values (    0,
     7,     2,     2,     1.3700,     4.6000,     0.8600,     0.0100,
     1.0000,     0.0300,     0.2000,     0.2000,     0.3000,     0.0040,
     1.8000,     0.0000,     1.2000,    10.0000,     0.5000,     0.5000,
     0.0000
);
insert into WRPC_data2tag values ('WRPC-07',    0);

comment on column WRPC_data.WRPC_data_id is 'Unique identifier';
comment on column WRPC_data.VERS       is 'VERSION                                         ';
comment on column WRPC_data.NVRS       is 'VERSION OF RPC TECHNOLOGY                       ';
comment on column WRPC_data.LAYRPC     is 'LAYERS NUMBER                                   ';
comment on column WRPC_data.TCKRLA     is 'THICK. OF AN RPC LAYER                          ';
comment on column WRPC_data.TOTTCK     is 'TOTAL THICKNESS                                 ';
comment on column WRPC_data.TCKFSP     is 'THICK. OF FOAM SPACER                           ';
comment on column WRPC_data.ACKFSP     is 'THICK. OF AL PLATE OF FOAM SPACER               ';
comment on column WRPC_data.TLOHCB     is 'THICK. OF LOWER HONEYCOMB                       ';
comment on column WRPC_data.ALOHCB     is 'THICK. OF AL PLATE OF LOWER HONEYCOMB           ';
comment on column WRPC_data.TCKBAK     is 'THICK. OF BAKELITE                              ';
comment on column WRPC_data.TCKGAS     is 'THICK. OF GAS GAP                               ';
comment on column WRPC_data.TCKSSU     is 'THICK. OF STRIPS SUPPORT                        ';
comment on column WRPC_data.TCKSTR     is 'THICK. OF STRIPS                                ';
comment on column WRPC_data.SDEDMI     is 'S INTERNAL MID-CHBER DEAD REGION                ';
comment on column WRPC_data.ZDEDMI     is 'Z INTERNAL MID-CHBER DEAD REGION                ';
comment on column WRPC_data.SPDIAM     is 'SPACER DIAMETER                                 ';
comment on column WRPC_data.SPPITC     is 'SPACER PITCH                                    ';
comment on column WRPC_data.STROFF     is 'STRIP OFFSET S, FIRST Z, SECOND Z               ';
