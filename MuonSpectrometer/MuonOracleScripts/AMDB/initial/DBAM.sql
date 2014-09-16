SET echo OFF;
set linesize 132;
drop   table DBAM_data2tag cascade constraints;
drop   table DBAM_data cascade constraints;
create table DBAM_data (
  DBAM_data_id   number(10),
VERS       number(10),
NVRS       number(10),
AMDB       varchar2(4),
TEST       varchar2(4),
MTYP       number(10),
NAME_0       varchar2(4),
NAME_1       varchar2(4),
NAME_2       varchar2(4),
NAME_3       varchar2(4),
NAME_4       varchar2(4),
NAME_5       varchar2(4),
NAME_6       varchar2(4),
NAME_7       varchar2(4),
NAME_8       varchar2(4),
NAME_9       varchar2(4),
NAME_10       varchar2(4),
NAME_11       varchar2(4),
NAME_12       varchar2(4),
NAME_13       varchar2(4),
NAME_14       varchar2(4),
NAME_15       varchar2(4),
NAME_16       varchar2(4),
NAME_17       varchar2(4),
NAME_18       varchar2(4),
NAME_19       varchar2(4),
NAME_20       varchar2(4),
NAME_21       varchar2(4),
NAME_22       varchar2(4),
NAME_23       varchar2(4),
NAME_24       varchar2(4),
NAME_25       varchar2(4),
NAME_26       varchar2(4),
NAME_27       varchar2(4),
NAME_28       varchar2(4),
NAME_29       varchar2(4),
NAME_30       varchar2(4),
NAME_31       varchar2(4),
NAME_32       varchar2(4),
NAME_33       varchar2(4),
NAME_34       varchar2(4),
NAME_35       varchar2(4),
NAME_36       varchar2(4),
NAME_37       varchar2(4),
NAME_38       varchar2(4),
NAME_39       varchar2(4),
NAME_40       varchar2(4),
NAME_41       varchar2(4),
NAME_42       varchar2(4),
NAME_43       varchar2(4),
NAME_44       varchar2(4),
NAME_45       varchar2(4),
NAME_46       varchar2(4),
NAME_47       varchar2(4),
NAME_48       varchar2(4),
NAME_49       varchar2(4),
NAME_50       varchar2(4),
NAME_51       varchar2(4),
NAME_52       varchar2(4),
NUMBOX       number(10)
) ;

alter table DBAM_data add constraint DBAM_data_pk
primary key (DBAM_data_id);

create table DBAM_data2tag (
  DBAM_vers      varchar2(255),
  DBAM_data_id   number(10)
) ;

alter table DBAM_data2tag add constraint DBAM_data2tag_pk
primary key (DBAM_vers, DBAM_data_id);
alter table DBAM_data2tag add constraint DBAM_data2tag_vers_fk
foreign key (DBAM_vers) references hvs_nodevers (vers_name);
alter table DBAM_data2tag add constraint DBAM_data2tag_data_fk
foreign key (DBAM_data_id) references DBAM_data (DBAM_data_id);

delete from hvs_relation where CHILD_NODENAME = 'DBAM';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'DBAM';
delete from hvs_node     where NODE_NAME = 'DBAM';

insert into hvs_node     values ('DBAM','MuonSpectrometer', '');
insert into hvs_nodevers values ('DBAM', 'DBAM-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','DBAM','DBAM-07');
insert into DBAM_data (
DBAM_data_id,
VERS,
NVRS,
AMDB,
TEST,
MTYP,
NAME_0,NAME_1,NAME_2,NAME_3,NAME_4,NAME_5,NAME_6,NAME_7,NAME_8,NAME_9,NAME_10,
NAME_11,NAME_12,NAME_13,NAME_14,NAME_15,NAME_16,NAME_17,NAME_18,NAME_19,NAME_20,
NAME_21,NAME_22,NAME_23,NAME_24,NAME_25,NAME_26,NAME_27,NAME_28,NAME_29,NAME_30,
NAME_31,NAME_32,NAME_33,NAME_34,NAME_35,NAME_36,NAME_37,NAME_38,NAME_39,NAME_40,
NAME_41,NAME_42,NAME_43,NAME_44,NAME_45,NAME_46,NAME_47,NAME_48,NAME_49,NAME_50,
NAME_51,NAME_52,
NUMBOX
) values (    0,
     7,     7,'R.01','01',    53,'BIL','BIS','BML','BMS','BOL','BOS','BEE',
'BIR','BMF','BOF','BOG','BOH','EIC','EIL','EEL','EES','EMC','EML','EMS','EOC',
'EOL','EOS','T1C','T1L','T1S','T2C','T2L','T2S','T3C','T3L','T3S','CI1','CI2',
'CI3','CI4','FIL','FIS','FML','FMS','FOL','FOS','T1F','T1E','T2F','T2E','T3F',
'T3E','T4F','T4E','EIS','CSS','CSL','BIM', 63000
);
insert into DBAM_data2tag values ('DBAM-07',    0);

comment on column DBAM_data.DBAM_data_id is 'Unique identifier';
comment on column DBAM_data.VERS       is 'VERSION                                         ';
comment on column DBAM_data.NVRS       is 'VERSION OF READING                              ';
comment on column DBAM_data.AMDB       is 'AMDB NAME                                       ';
comment on column DBAM_data.TEST       is 'TEST NAME                                       ';
comment on column DBAM_data.MTYP       is 'MAXIMUM STATION NUMBER                          ';
comment on column DBAM_data.NAME_0     is 'STATION NAME                                    ';
comment on column DBAM_data.NUMBOX     is 'FILE INDEX                                      ';
