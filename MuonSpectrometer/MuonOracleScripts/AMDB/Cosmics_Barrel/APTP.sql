SET echo OFF;
set linesize 132;
drop   table APTP_data2tag cascade constraints;
drop   table APTP_data cascade constraints;
create table APTP_data (
  APTP_data_id   number(10),
VERS       number(10),
LINE       number(10),
TYP       varchar2(4),
I       number(10),
ICUT       number(10),
IPHI_0       number(10),
IPHI_1       number(10),
IPHI_2       number(10),
IPHI_3       number(10),
IPHI_4       number(10),
IPHI_5       number(10),
IPHI_6       number(10),
IPHI_7       number(10),
IZ       number(10),
DPHI       float(63),
Z       float(63),
R       float(63),
S       float(63),
ALFA       float(63),
BETA       float(63),
GAMMA       float(63)
) tablespace data02;

alter table APTP_data add constraint APTP_data_pk
primary key (APTP_data_id)
using index tablespace indx02;

create table APTP_data2tag (
  APTP_vers      varchar2(255),
  APTP_data_id   number(10)
) tablespace data02;

alter table APTP_data2tag add constraint APTP_data2tag_pk
primary key (APTP_vers, APTP_data_id)
using index tablespace indx02;
alter table APTP_data2tag add constraint APTP_data2tag_vers_fk
foreign key (APTP_vers) references hvs_nodevers (vers_name);
alter table APTP_data2tag add constraint APTP_data2tag_data_fk
foreign key (APTP_data_id) references APTP_data (APTP_data_id);

delete from hvs_relation where CHILD_NODENAME = 'APTP';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'APTP';
delete from hvs_node     where NODE_NAME = 'APTP';

insert into hvs_node     values ('APTP','MuonSpectrometer', '');
insert into hvs_nodevers values ('APTP', 'APTP-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','APTP','APTP-07');
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    0,
     7,     1,'BML',     1,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     1,     0.0000,    39.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    0);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    1,
     7,     1,'BML',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     2,     0.0000,   185.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    1);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    2,
     7,     2,'BML',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     3,     0.0000,   355.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    2);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    3,
     7,     1,'BML',     9,     0,     0,     0,     0,     0,     0,     0,
     1,     0,    -1,     0.0000,  -183.1961,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    3);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    4,
     7,     1,'BOL',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     1,     0.0000,    63.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    4);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    5,
     7,     1,'BOL',     5,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     2,     0.0000,   233.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    5);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    6,
     7,     2,'BOL',     5,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     3,     0.0000,   379.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    6);
insert into APTP_data (
APTP_data_id,
VERS,
LINE,
TYP,
I,
ICUT,
IPHI_0,IPHI_1,IPHI_2,IPHI_3,IPHI_4,IPHI_5,IPHI_6,IPHI_7,
IZ,
DPHI,
Z,
R,
S,
ALFA,
BETA,
GAMMA
) values (    7,
     7,     1,'BOL',     7,     0,     0,     0,     0,     0,     0,     0,
     1,     0,    -1,     0.0000,  -231.2521,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    7);

comment on column APTP_data.APTP_data_id is 'Unique identifier';
comment on column APTP_data.VERS       is 'VERSION                                         ';
comment on column APTP_data.LINE       is 'LINE NUMBER                                     ';
comment on column APTP_data.TYP        is 'STATION TYPE                                    ';
comment on column APTP_data.I          is 'STATION AMDB INDEX                              ';
comment on column APTP_data.ICUT       is 'CUT-OUT INDEX,ZERO IF MISSING                   ';
comment on column APTP_data.IPHI       is 'PHI INDICATES OF OCTANTS                        ';
comment on column APTP_data.IZ         is 'Z (FOR BARREL) OR R (FOR END-CAPS) POS.         ';
comment on column APTP_data.DPHI       is 'RELATIVE PHI POSITION OF THE STATION IN         ';
comment on column APTP_data.Z          is 'Z POSITION OF THE LOWEST Z EDGE OF THE S        ';
comment on column APTP_data.R          is 'RADIAL POSITION OF ITS INNERMOST EDGE           ';
comment on column APTP_data.S          is 'ORTHO-RADIAL POSITION OF THE CENTER OF T        ';
comment on column APTP_data.ALFA       is 'ALFA ANGLE DEFINING THE DEVIATION [GRAD]        ';
comment on column APTP_data.BETA       is 'BETA ANGLE DEFINING THE DEVIATION               ';
comment on column APTP_data.GAMMA      is 'GAMMA ANGLE DEFINING THE DEVIATION              ';
