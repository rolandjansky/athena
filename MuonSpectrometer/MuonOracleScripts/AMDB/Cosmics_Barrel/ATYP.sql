SET echo OFF;
set linesize 132;
drop   table ATYP_data2tag cascade constraints;
drop   table ATYP_data cascade constraints;
create table ATYP_data (
  ATYP_data_id   number(10),
VERS       number(10),
JTYP       number(10),
TYP       varchar2(4),
NSTA       number(10)
) tablespace data02;

alter table ATYP_data add constraint ATYP_data_pk
primary key (ATYP_data_id)
using index tablespace indx02;

create table ATYP_data2tag (
  ATYP_vers      varchar2(255),
  ATYP_data_id   number(10)
) tablespace data02;

alter table ATYP_data2tag add constraint ATYP_data2tag_pk
primary key (ATYP_vers, ATYP_data_id)
using index tablespace indx02;
alter table ATYP_data2tag add constraint ATYP_data2tag_vers_fk
foreign key (ATYP_vers) references hvs_nodevers (vers_name);
alter table ATYP_data2tag add constraint ATYP_data2tag_data_fk
foreign key (ATYP_data_id) references ATYP_data (ATYP_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ATYP';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'ATYP';
delete from hvs_node     where NODE_NAME = 'ATYP';

insert into hvs_node     values ('ATYP','MuonSpectrometer', '');
insert into hvs_nodevers values ('ATYP', 'ATYP-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','ATYP','ATYP-07');
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    0,
     7,     1,'BIL',     3
);
insert into ATYP_data2tag values ('ATYP-07',    0);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    1,
     7,     2,'BIS',     6
);
insert into ATYP_data2tag values ('ATYP-07',    1);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    2,
     7,     3,'BML',     9
);
insert into ATYP_data2tag values ('ATYP-07',    2);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    3,
     7,     4,'BMS',     6
);
insert into ATYP_data2tag values ('ATYP-07',    3);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    4,
     7,     5,'BOL',     7
);
insert into ATYP_data2tag values ('ATYP-07',    4);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    5,
     7,     6,'BOS',     3
);
insert into ATYP_data2tag values ('ATYP-07',    5);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    6,
     7,     7,'BEE',     1
);
insert into ATYP_data2tag values ('ATYP-07',    6);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    7,
     7,     8,'BIR',     6
);
insert into ATYP_data2tag values ('ATYP-07',    7);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    8,
     7,     9,'BMF',     3
);
insert into ATYP_data2tag values ('ATYP-07',    8);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (    9,
     7,    10,'BOF',     8
);
insert into ATYP_data2tag values ('ATYP-07',    9);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   10,
     7,    11,'BOG',     6
);
insert into ATYP_data2tag values ('ATYP-07',   10);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   11,
     7,    14,'EIL',     9
);
insert into ATYP_data2tag values ('ATYP-07',   11);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   12,
     7,    15,'EEL',     3
);
insert into ATYP_data2tag values ('ATYP-07',   12);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   13,
     7,    16,'EES',     2
);
insert into ATYP_data2tag values ('ATYP-07',   13);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   14,
     7,    18,'EML',     5
);
insert into ATYP_data2tag values ('ATYP-07',   14);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   15,
     7,    19,'EMS',     5
);
insert into ATYP_data2tag values ('ATYP-07',   15);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   16,
     7,    21,'EOL',     6
);
insert into ATYP_data2tag values ('ATYP-07',   16);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   17,
     7,    22,'EOS',     6
);
insert into ATYP_data2tag values ('ATYP-07',   17);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   18,
     7,    42,'T1F',     1
);
insert into ATYP_data2tag values ('ATYP-07',   18);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   19,
     7,    43,'T1E',     1
);
insert into ATYP_data2tag values ('ATYP-07',   19);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   20,
     7,    44,'T2F',     1
);
insert into ATYP_data2tag values ('ATYP-07',   20);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   21,
     7,    45,'T2E',     1
);
insert into ATYP_data2tag values ('ATYP-07',   21);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   22,
     7,    46,'T3F',     1
);
insert into ATYP_data2tag values ('ATYP-07',   22);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   23,
     7,    47,'T3E',     1
);
insert into ATYP_data2tag values ('ATYP-07',   23);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   24,
     7,    48,'T4F',     2
);
insert into ATYP_data2tag values ('ATYP-07',   24);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   25,
     7,    49,'T4E',     2
);
insert into ATYP_data2tag values ('ATYP-07',   25);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   26,
     7,    50,'EIS',     2
);
insert into ATYP_data2tag values ('ATYP-07',   26);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   27,
     7,    51,'CSS',     1
);
insert into ATYP_data2tag values ('ATYP-07',   27);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   28,
     7,    52,'CSL',     1
);
insert into ATYP_data2tag values ('ATYP-07',   28);
insert into ATYP_data (
ATYP_data_id,
VERS,
JTYP,
TYP,
NSTA
) values (   29,
     7,    53,'BIM',     1
);
insert into ATYP_data2tag values ('ATYP-07',   29);

comment on column ATYP_data.ATYP_data_id is 'Unique identifier';
comment on column ATYP_data.VERS       is 'VERSION                                         ';
comment on column ATYP_data.JTYP       is 'AMDB STATION TYPE                               ';
comment on column ATYP_data.TYP        is 'AMDB STATION NAME                               ';
comment on column ATYP_data.NSTA       is 'NUMBER OF STATIONS OF THIS TYPE                 ';
