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
) ;

alter table APTP_data add constraint APTP_data_pk
primary key (APTP_data_id);

create table APTP_data2tag (
  APTP_vers      varchar2(255),
  APTP_data_id   number(10)
) ;

alter table APTP_data2tag add constraint APTP_data2tag_pk
primary key (APTP_vers, APTP_data_id);
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
     7,     1,'BIL',     1,     0,     0,     0,     0,     0,     0,     0,
     1,     0,    -1,     0.0000,  -123.1261,   474.0910,     0.0000,
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
     7,     2,'BIL',     1,     0,     1,     1,     1,     1,     1,     0,
     1,     0,     2,     0.0000,   125.0000,   474.0910,     0.0000,
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
     7,     3,'BIL',     1,     0,     0,     0,     1,     0,     0,     0,
     0,     0,     3,     0.0000,   235.0000,   474.0910,     0.0000,
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
     7,     4,'BIL',     1,     0,     1,     1,     1,     1,     1,     0,
     0,     0,     4,     0.0000,   345.0000,   474.0910,     0.0000,
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
     7,     5,'BIL',     1,     0,     1,     1,     1,     0,     1,     0,
     0,     0,     6,     0.0000,   547.0000,   474.0910,     0.0000,
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
     7,     1,'BIL',     2,     0,     1,     1,     1,     0,     0,     0,
     0,     0,     1,     0.0000,    33.0000,   474.0910,     0.0000,
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
     7,     2,'BIL',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     0,     1,     0.0000,    56.0000,   525.6910,     0.0000,
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
     7,     3,'BIL',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     1,     0.0000,    33.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    7);
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
) values (    8,
     7,     4,'BIL',     2,     0,     1,     1,     0,     1,     1,     0,
     1,     0,     3,     0.0000,   235.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    8);
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
) values (    9,
     7,     5,'BIL',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     4,     0.0000,   345.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',    9);
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
) values (   10,
     7,     6,'BIL',     2,     0,     1,     1,     1,     1,     1,     0,
     0,     0,     5,     0.0000,   455.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   10);
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
) values (   11,
     7,     7,'BIL',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     5,     0.0000,   437.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   11);
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
) values (   12,
     7,     8,'BIL',     2,     0,     0,     0,     0,     1,     0,     0,
     1,     0,     6,     0.0000,   547.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   12);
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
) values (   13,
     7,     1,'BIL',     3,     0,     0,     0,     0,     1,     0,     0,
     0,     0,     1,     0.0000,    51.0000,   474.0910,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   13);
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
) values (   14,
     7,     1,'BIS',     1,     0,     1,     1,     1,     1,     0,     0,
     1,     0,     8,    22.5000,   681.0000,   454.8250,    -6.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   14);
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
) values (   15,
     7,     2,'BIS',     1,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     8,    22.5000,   681.0000,   454.8250,    -6.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   15);
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
) values (   16,
     7,     1,'BIS',     2,     0,     1,     1,     1,     1,     0,     0,
     1,     0,     2,    22.5000,   111.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   16);
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
) values (   17,
     7,     2,'BIS',     2,     0,     1,     1,     1,     1,     0,     0,
     1,     0,     3,    22.5000,   203.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   17);
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
) values (   18,
     7,     3,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     2,    22.5000,   111.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   18);
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
) values (   19,
     7,     4,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     3,    22.5000,   203.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   19);
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
) values (   20,
     7,     5,'BIS',     2,     0,     1,     1,     1,     0,     0,     0,
     0,     0,     4,    22.5000,   295.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   20);
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
) values (   21,
     7,     6,'BIS',     2,     0,     1,     1,     1,     0,     0,     0,
     0,     0,     5,    22.5000,   387.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   21);
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
) values (   22,
     7,     7,'BIS',     2,     0,     1,     1,     1,     0,     0,     0,
     0,     0,     6,    22.5000,   479.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   22);
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
) values (   23,
     7,     8,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     4,    22.5000,   313.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   23);
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
) values (   24,
     7,     9,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     5,    22.5000,   405.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   24);
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
) values (   25,
     7,    10,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     6,    22.5000,   497.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   25);
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
) values (   26,
     7,    11,'BIS',     2,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     7,    22.5000,   589.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   26);
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
) values (   27,
     7,    12,'BIS',     2,     0,     0,     0,     0,     1,     0,     0,
     1,     0,     4,    22.5000,   313.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   27);
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
) values (   28,
     7,    13,'BIS',     2,     0,     0,     0,     0,     1,     0,     0,
     1,     0,     5,    22.5000,   405.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   28);
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
) values (   29,
     7,    14,'BIS',     2,     0,     0,     0,     0,     1,     0,     0,
     1,     0,     6,    22.5000,   497.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   29);
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
) values (   30,
     7,    15,'BIS',     2,     0,     0,     0,     0,     1,     0,     0,
     1,     0,     7,    22.5000,   589.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   30);
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
) values (   31,
     7,     1,'BIS',     3,     0,     1,     1,     1,     1,     0,     0,
     1,     0,     1,    22.5000,     1.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   31);
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
) values (   32,
     7,     2,'BIS',     3,     0,     0,     0,     0,     0,     1,     0,
     0,     1,     1,    22.5000,     1.0000,   442.2930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   32);
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
) values (   33,
     7,     3,'BIS',     3,     0,     1,     1,     1,     0,     0,     0,
     0,     0,     7,    22.5000,   571.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   33);
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
) values (   34,
     7,     1,'BIS',     4,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     8,    22.5000,   681.0000,   454.8250,    -6.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   34);
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
) values (   35,
     7,     1,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     2,    22.5000,   111.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   35);
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
) values (   36,
     7,     2,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     3,    22.5000,   203.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   36);
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
) values (   37,
     7,     3,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,    22.5000,   313.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   37);
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
) values (   38,
     7,     4,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     5,    22.5000,   405.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   38);
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
) values (   39,
     7,     5,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     6,    22.5000,   497.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   39);
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
) values (   40,
     7,     6,'BIS',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     7,    22.5000,   589.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   40);
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
) values (   41,
     7,     1,'BIS',     6,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     1,    22.5000,     1.0000,   440.7930,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   41);
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
) values (   42,
     7,     1,'BML',     1,     0,     0,     0,     1,     0,     0,     0,
     1,     0,     1,     0.0000,    39.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   42);
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
) values (   43,
     7,     2,'BML',     1,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     6,     0.0000,   769.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   43);
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
) values (   44,
     7,     3,'BML',     1,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     5,     0.0000,   769.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   44);
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
) values (   45,
     7,     1,'BML',     2,     0,     1,     1,     0,     0,     0,     0,
     0,     0,     1,     0.0000,    15.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   45);
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
) values (   46,
     7,     2,'BML',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     0.0000,   185.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   46);
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
) values (   47,
     7,     3,'BML',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     0.0000,   355.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   47);
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
) values (   48,
     7,     1,'BML',     3,     0,     0,     0,     0,     0,     0,     1,
     0,     1,     1,     0.0000,    63.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   48);
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
) values (   49,
     7,     2,'BML',     3,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     4,     0.0000,   525.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   49);
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
) values (   50,
     7,     3,'BML',     3,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     5,     0.0000,   647.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   50);
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
) values (   51,
     7,     4,'BML',     3,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     4,     0.0000,   647.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   51);
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
) values (   52,
     7,     1,'BML',     4,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     7,     0.0000,   918.0000,   754.9500,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   52);
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
) values (   53,
     7,     2,'BML',     4,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     6,     0.0000,   918.0000,   754.9500,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   53);
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
) values (   54,
     7,     1,'BML',     5,     0,     1,     0,     0,     0,     0,     0,
     0,     0,    -1,     0.0000,  -183.1121,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   54);
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
) values (   55,
     7,     1,'BML',     6,     0,     0,     1,     0,     0,     0,     0,
     0,     0,    -1,     0.0000,  -183.1681,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   55);
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
) values (   56,
     7,     1,'BML',     7,     0,     0,     0,     0,     1,     1,     0,
     0,     0,     1,     0.0000,    87.0000,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   56);
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
) values (   57,
     7,     1,'BML',     8,     0,     0,     0,     0,     0,     1,     0,
     0,     0,    -1,     0.0000,  -183.1401,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   57);
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
) values (   58,
     7,     1,'BML',     9,     0,     0,     0,     0,     0,     0,     0,
     1,     0,    -1,     0.0000,  -183.1961,   674.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   58);
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
) values (   59,
     7,     1,'BMS',     1,     0,     1,     1,     1,     1,     1,     0,
     0,     1,     5,    22.5000,   692.5000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   59);
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
) values (   60,
     7,     1,'BMS',     2,     1,     1,     1,     1,     1,     1,     0,
     0,     1,     6,    22.5000,   790.5000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   60);
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
) values (   61,
     7,     1,'BMS',     3,     0,     1,     1,     1,     0,     1,     0,
     0,     1,     1,    22.5000,    15.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   61);
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
) values (   62,
     7,     2,'BMS',     3,     1,     0,     0,     0,     1,     0,     0,
     0,     0,     1,    22.5000,    15.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   62);
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
) values (   63,
     7,     1,'BMS',     4,     1,     1,     1,     1,     1,     1,     0,
     0,     1,     3,    22.5000,   368.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   63);
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
) values (   64,
     7,     1,'BMS',     5,     1,     1,     1,     1,     1,     1,     0,
     0,     1,     2,    22.5000,   185.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   64);
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
) values (   65,
     7,     1,'BMS',     6,     1,     1,     1,     1,     1,     1,     0,
     0,     1,     4,    22.5000,   514.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   65);
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
) values (   66,
     7,     1,'BOL',     1,     0,     1,     1,     0,     0,     0,     0,
     0,     0,     1,     0.0000,    15.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   66);
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
) values (   67,
     7,     2,'BOL',     1,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     2,     0.0000,   233.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   67);
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
) values (   68,
     7,     3,'BOL',     1,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     4,     0.0000,   621.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   68);
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
) values (   69,
     7,     4,'BOL',     1,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     4,     0.0000,   625.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   69);
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
) values (   70,
     7,     5,'BOL',     1,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     5,     0.0000,   839.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   70);
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
) values (   71,
     7,     6,'BOL',     1,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     5,     0.0000,   843.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   71);
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
) values (   72,
     7,     1,'BOL',     2,     0,     0,     0,     0,     0,     0,     1,
     1,     1,     1,     0.0000,    63.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   72);
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
) values (   73,
     7,     2,'BOL',     2,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     3,     0.0000,   451.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   73);
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
) values (   74,
     7,     3,'BOL',     2,     0,     1,     1,     1,     1,     1,     1,
     0,     1,     6,     0.0000,  1057.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   74);
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
) values (   75,
     7,     4,'BOL',     2,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     6,     0.0000,  1061.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   75);
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
) values (   76,
     7,     1,'BOL',     3,     0,     1,     1,     0,     0,     0,     0,
     0,     0,    -1,     0.0000,  -231.1681,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   76);
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
) values (   77,
     7,     2,'BOL',     3,     0,     0,     0,     1,     0,     0,     0,
     0,     0,    -1,     0.0000,  -303.1681,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   77);
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
) values (   78,
     7,     3,'BOL',     3,     0,     0,     0,     1,     0,     0,     0,
     0,     0,    -2,     0.0000,  -449.1681,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   78);
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
) values (   79,
     7,     1,'BOL',     4,     0,     0,     0,     1,     0,     0,     0,
     0,     0,     1,     0.0000,    39.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   79);
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
) values (   80,
     7,     1,'BOL',     5,     0,     0,     0,     0,     1,     1,     0,
     0,     0,     1,     0.0000,    87.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   80);
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
) values (   81,
     7,     2,'BOL',     5,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     2,     0.0000,   233.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   81);
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
) values (   82,
     7,     3,'BOL',     5,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     3,     0.0000,   379.0000,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   82);
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
) values (   83,
     7,     1,'BOL',     6,     0,     0,     0,     0,     0,     1,     0,
     0,     0,    -1,     0.0000,  -231.1961,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   83);
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
) values (   84,
     7,     1,'BOL',     7,     0,     0,     0,     0,     0,     0,     0,
     1,     0,    -1,     0.0000,  -231.2521,   924.4440,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   84);
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
) values (   85,
     7,     1,'BOS',     1,     1,     1,     1,     1,     1,     1,     0,
     0,     1,     6,    22.5000,  1091.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   85);
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
) values (   86,
     7,     1,'BOS',     2,     0,     1,     1,     1,     0,     1,     0,
     0,     1,     1,    22.5000,     1.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   86);
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
) values (   87,
     7,     2,'BOS',     2,     0,     1,     1,     1,     1,     1,     0,
     0,     1,     2,    22.5000,   219.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   87);
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
) values (   88,
     7,     3,'BOS',     2,     0,     1,     1,     1,     1,     1,     0,
     0,     1,     3,    22.5000,   437.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   88);
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
) values (   89,
     7,     4,'BOS',     2,     0,     1,     1,     1,     1,     1,     0,
     0,     1,     4,    22.5000,   655.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   89);
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
) values (   90,
     7,     5,'BOS',     2,     0,     1,     1,     1,     1,     1,     0,
     0,     1,     5,    22.5000,   873.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   90);
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
) values (   91,
     7,     1,'BOS',     3,     0,     0,     0,     0,     1,     0,     0,
     0,     0,     1,    22.5000,    73.0000,  1018.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   91);
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
) values (   92,
     7,     1,'BEE',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,    22.5000,   889.0000,   433.0230,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   92);
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
) values (   93,
     7,     2,'BEE',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    22.5000,  1035.0000,   433.0230,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   93);
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
) values (   94,
     7,     1,'BIR',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     5,     0.0000,   541.0000,   584.7910,    68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   94);
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
) values (   95,
     7,     2,'BIR',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     5,     0.0000,   541.0000,   584.7910,   -68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   95);
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
) values (   96,
     7,     1,'BIR',     2,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     2,     0.0000,   200.0000,   584.7910,    68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   96);
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
) values (   97,
     7,     2,'BIR',     2,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     2,     0.0000,   200.0000,   584.7910,   -68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   97);
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
) values (   98,
     7,     1,'BIR',     3,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,     0.0000,   394.0000,   584.7910,    68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   98);
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
) values (   99,
     7,     2,'BIR',     3,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     4,     0.0000,   394.0000,   584.7910,   -68.4000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',   99);
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
) values (  100,
     7,     1,'BIR',     4,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     3,     0.0000,   292.0000,   584.7910,    46.8500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  100);
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
) values (  101,
     7,     2,'BIR',     4,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     3,     0.0000,   292.0000,   584.7910,   -46.8500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  101);
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
) values (  102,
     7,     1,'BIR',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     1,     0.0000,    52.0000,   584.7910,    11.6500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  102);
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
) values (  103,
     7,     2,'BIR',     5,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     1,     0.0000,    52.0000,   584.7910,   -11.6500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  103);
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
) values (  104,
     7,     1,'BIR',     6,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     6,     0.0000,   616.0000,   584.7910,    46.8500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  104);
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
) values (  105,
     7,     2,'BIR',     6,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     6,     0.0000,   616.0000,   584.7910,   -46.8500,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  105);
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
) values (  106,
     7,     1,'BMF',     1,     0,     0,     0,     0,     0,     0,     1,
     1,     0,     3,    22.5000,   728.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  106);
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
) values (  107,
     7,     1,'BMF',     2,     0,     0,     0,     0,     0,     0,     1,
     1,     0,     2,    22.5000,   406.0000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  107);
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
) values (  108,
     7,     1,'BMF',     3,     0,     0,     0,     0,     0,     0,     1,
     1,     0,     1,    22.5000,    62.5000,   776.8940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  108);
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
) values (  109,
     7,     1,'BOF',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     3,    22.5000,   727.9200,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  109);
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
) values (  110,
     7,     1,'BOF',     2,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     2,    22.5000,   405.4600,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  110);
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
) values (  111,
     7,     1,'BOF',     3,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     1,    22.5000,    62.5700,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  111);
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
) values (  112,
     7,     1,'BOF',     4,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,    22.5000,  1002.6000,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  112);
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
) values (  113,
     7,     1,'BOF',     5,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     3,    22.5000,   727.9200,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  113);
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
) values (  114,
     7,     1,'BOF',     6,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     2,    22.5000,   405.4600,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  114);
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
) values (  115,
     7,     1,'BOF',     7,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     1,    22.5000,    62.5700,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  115);
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
) values (  116,
     7,     1,'BOF',     8,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     4,    22.5000,  1002.6000,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  116);
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
) values (  117,
     7,     1,'BOG',     1,     1,     0,     0,     0,     0,     0,     1,
     0,     0,     0,    22.5000,   -60.8210,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  117);
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
) values (  118,
     7,     2,'BOG',     1,     1,     0,     0,     0,     0,     0,     1,
     0,     0,    -1,    22.5000,  -401.9101,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  118);
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
) values (  119,
     7,     3,'BOG',     1,     1,     0,     0,     0,     0,     0,     1,
     0,     0,     1,    22.5000,   282.0700,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  119);
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
) values (  120,
     7,     4,'BOG',     1,     1,     0,     0,     0,     0,     0,     1,
     0,     0,    -2,    22.5000,  -722.5701,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  120);
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
) values (  121,
     7,     5,'BOG',     1,     1,     0,     0,     0,     0,     0,     1,
     0,     0,     2,    22.5000,   602.7500,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  121);
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
) values (  122,
     7,     1,'BOG',     2,     1,     0,     0,     0,     0,     0,     1,
     0,     0,     3,    22.5000,   877.1401,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  122);
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
) values (  123,
     7,     1,'BOG',     3,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,    22.5000,  1147.2001,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  123);
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
) values (  124,
     7,     1,'BOG',     4,     1,     0,     0,     0,     0,     0,     0,
     1,     0,     0,    22.5000,   -60.8210,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  124);
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
) values (  125,
     7,     2,'BOG',     4,     1,     0,     0,     0,     0,     0,     0,
     1,     0,     1,    22.5000,   282.0700,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  125);
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
) values (  126,
     7,     3,'BOG',     4,     1,     0,     0,     0,     0,     0,     0,
     1,     0,     2,    22.5000,   602.7300,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  126);
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
) values (  127,
     7,     1,'BOG',     5,     1,     0,     0,     0,     0,     0,     0,
     1,     0,     3,    22.5000,   877.1401,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  127);
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
) values (  128,
     7,     1,'BOG',     6,     0,     0,     0,     0,     0,     0,     0,
     1,     0,     4,    22.5000,  1147.2001,  1027.3940,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  128);
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
) values (  129,
     7,     1,'EIL',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0.0000,   749.1460,   207.6000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  129);
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
) values (  130,
     7,     1,'EIL',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     0.0000,   749.1460,   319.1000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  130);
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
) values (  131,
     7,     1,'EIL',     3,     0,     0,     1,     1,     0,     0,     0,
     1,     0,     4,     0.0000,   745.7960,   472.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  131);
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
) values (  132,
     7,     1,'EIL',     4,     0,     0,     0,     0,     1,     0,     0,
     0,     0,     4,     0.0000,   745.7960,   472.0000,    35.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  132);
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
) values (  133,
     7,     1,'EIL',     5,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,     0.0000,   745.7960,   508.0400,   -62.5000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  133);
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
) values (  134,
     7,     2,'EIL',     5,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     4,     0.0000,   745.7960,   508.0400,    62.5000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  134);
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
) values (  135,
     7,     1,'EIL',     6,     0,     1,     1,     1,     1,     1,     0,
     1,     0,     3,     0.0000,   749.1460,   429.2300,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  135);
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
) values (  136,
     7,     1,'EIL',     7,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     3,     0.0000,   749.1460,   429.2300,   -16.5000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  136);
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
) values (  137,
     7,     2,'EIL',     7,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     3,     0.0000,   749.1460,   429.2300,    16.5000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  137);
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
) values (  138,
     7,     1,'EIL',     8,     0,     1,     0,     0,     0,     1,     0,
     0,     0,     4,     0.0000,   745.7960,   470.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  138);
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
) values (  139,
     7,     1,'EIL',     9,     0,     1,     0,     0,     0,     1,     0,
     0,     0,     5,     0.0000,   745.7960,   508.0400,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  139);
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
) values (  140,
     7,     1,'EML',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0.0000,  1411.2480,   177.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  140);
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
) values (  141,
     7,     1,'EML',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     0.0000,  1411.2480,   348.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  141);
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
) values (  142,
     7,     1,'EML',     3,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     0.0000,  1411.2480,   544.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  142);
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
) values (  143,
     7,     1,'EML',     4,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,     0.0000,  1411.2480,   739.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  143);
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
) values (  144,
     7,     1,'EML',     5,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,     0.0000,  1411.2480,   935.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  144);
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
) values (  145,
     7,     1,'EMS',     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,    22.5000,  1369.6481,   177.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  145);
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
) values (  146,
     7,     1,'EMS',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    22.5000,  1369.6481,   372.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  146);
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
) values (  147,
     7,     1,'EMS',     3,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     3,    22.5000,  1369.6481,   568.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  147);
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
) values (  148,
     7,     1,'EMS',     4,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,    22.5000,  1369.6481,   763.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  148);
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
) values (  149,
     7,     1,'EMS',     5,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,    22.5000,  1369.6481,   959.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  149);
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
) values (  150,
     7,     1,'EOL',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0.0000,  2124.2480,   277.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  150);
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
) values (  151,
     7,     1,'EOL',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     0.0000,  2124.2480,   448.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  151);
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
) values (  152,
     7,     1,'EOL',     3,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     0.0000,  2124.2480,   620.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  152);
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
) values (  153,
     7,     1,'EOL',     4,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,     0.0000,  2124.2480,   767.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  153);
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
) values (  154,
     7,     1,'EOL',     5,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,     0.0000,  2124.2480,   915.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  154);
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
) values (  155,
     7,     1,'EOL',     6,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     6,     0.0000,  2124.2480,  1062.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  155);
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
) values (  156,
     7,     1,'EOS',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,    22.5000,  2165.8481,   277.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  156);
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
) values (  157,
     7,     1,'EOS',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    22.5000,  2165.8481,   448.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  157);
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
) values (  158,
     7,     1,'EOS',     3,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,    22.5000,  2165.8481,   620.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  158);
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
) values (  159,
     7,     1,'EOS',     4,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,    22.5000,  2165.8481,   791.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  159);
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
) values (  160,
     7,     1,'EOS',     5,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,    22.5000,  2165.8481,   939.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  160);
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
) values (  161,
     7,     1,'EOS',     6,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     6,    22.5000,  2165.8481,  1086.5000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  161);
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
) values (  162,
     7,     1,'T1F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     1,    -7.5000,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  162);
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
) values (  163,
     7,     2,'T1F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     2,     7.5000,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  163);
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
) values (  164,
     7,     3,'T1F',     1,     1,     1,     0,     1,     0,     1,     0,
     1,     0,     3,    22.5000,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  164);
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
) values (  165,
     7,     4,'T1F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     4,    -7.5000,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  165);
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
) values (  166,
     7,     5,'T1F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     5,     7.5000,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  166);
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
) values (  167,
     7,     6,'T1F',     1,     1,     0,     1,     0,     1,     0,     1,
     0,     1,     6,    22.5000,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  167);
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
) values (  168,
     7,     1,'T1E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,   -11.2500,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  168);
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
) values (  169,
     7,     2,'T1E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    -3.7500,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  169);
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
) values (  170,
     7,     3,'T1E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     3.7500,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  170);
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
) values (  171,
     7,     4,'T1E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,    11.2500,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  171);
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
) values (  172,
     7,     5,'T1E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,    18.7500,  1345.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  172);
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
) values (  173,
     7,     6,'T1E',     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     6,    26.2500,  1327.1500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  173);
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
) values (  174,
     7,     1,'T2F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     1,    -7.5000,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  174);
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
) values (  175,
     7,     2,'T2F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     2,     7.5000,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  175);
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
) values (  176,
     7,     3,'T2F',     1,     1,     1,     0,     1,     0,     1,     0,
     1,     0,     3,    22.5000,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  176);
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
) values (  177,
     7,     4,'T2F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     4,    -7.5000,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  177);
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
) values (  178,
     7,     5,'T2F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     5,     7.5000,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  178);
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
) values (  179,
     7,     6,'T2F',     1,     1,     0,     1,     0,     1,     0,     1,
     0,     1,     6,    22.5000,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  179);
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
) values (  180,
     7,     1,'T2E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,   -11.2500,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  180);
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
) values (  181,
     7,     2,'T2E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    -3.7500,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  181);
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
) values (  182,
     7,     3,'T2E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     3.7500,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  182);
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
) values (  183,
     7,     4,'T2E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,    11.2500,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  183);
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
) values (  184,
     7,     5,'T2E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,    18.7500,  1475.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  184);
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
) values (  185,
     7,     6,'T2E',     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     6,    26.2500,  1460.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  185);
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
) values (  186,
     7,     1,'T3F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     1,    -7.5000,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  186);
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
) values (  187,
     7,     2,'T3F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     2,     7.5000,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  187);
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
) values (  188,
     7,     3,'T3F',     1,     1,     1,     0,     1,     0,     1,     0,
     1,     0,     3,    22.5000,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  188);
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
) values (  189,
     7,     4,'T3F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     4,    -7.5000,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  189);
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
) values (  190,
     7,     5,'T3F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     5,     7.5000,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  190);
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
) values (  191,
     7,     6,'T3F',     1,     1,     0,     1,     0,     1,     0,     1,
     0,     1,     6,    22.5000,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  191);
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
) values (  192,
     7,     1,'T3E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,   -11.2500,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  192);
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
) values (  193,
     7,     2,'T3E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    -3.7500,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  193);
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
) values (  194,
     7,     3,'T3E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     3,     3.7500,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  194);
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
) values (  195,
     7,     4,'T3E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     4,    11.2500,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  195);
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
) values (  196,
     7,     5,'T3E',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     5,    18.7500,  1517.5500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  196);
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
) values (  197,
     7,     6,'T3E',     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     6,    26.2500,  1502.0500,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  197);
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
) values (  198,
     7,     1,'T4F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     1,    -7.5000,   693.0400,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  198);
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
) values (  199,
     7,     2,'T4F',     1,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     2,     7.5000,   698.0600,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  199);
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
) values (  200,
     7,     3,'T4F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     4,    -7.5000,   698.0600,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  200);
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
) values (  201,
     7,     4,'T4F',     1,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     5,     7.5000,   693.0400,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  201);
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
) values (  202,
     7,     1,'T4F',     2,     0,     1,     0,     1,     0,     1,     0,
     1,     0,     3,    22.5000,   693.0400,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  202);
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
) values (  203,
     7,     2,'T4F',     2,     0,     0,     1,     0,     1,     0,     1,
     0,     1,     6,    22.5000,   698.0600,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  203);
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
) values (  204,
     7,     1,'T4E',     1,     0,     1,     0,     0,     0,     1,     0,
     0,     0,     2,     0.0000,   730.8701,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  204);
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
) values (  205,
     7,     2,'T4E',     1,     0,     0,     1,     1,     0,     0,     0,
     1,     0,     1,    -9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  205);
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
) values (  206,
     7,     3,'T4E',     1,     0,     0,     1,     1,     1,     0,     0,
     1,     0,     2,     0.0000,   730.8701,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  206);
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
) values (  207,
     7,     4,'T4E',     1,     0,     0,     1,     1,     1,     0,     0,
     1,     0,     3,     9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  207);
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
) values (  208,
     7,     1,'T4E',     2,     0,     1,     0,     0,     0,     1,     0,
     0,     0,     1,    -9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  208);
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
) values (  209,
     7,     2,'T4E',     2,     0,     1,     0,     0,     0,     1,     0,
     0,     0,     3,     9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  209);
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
) values (  210,
     7,     3,'T4E',     2,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     1,    -9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  210);
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
) values (  211,
     7,     4,'T4E',     2,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     2,     0.0000,   730.8701,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  211);
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
) values (  212,
     7,     5,'T4E',     2,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     1,     0.0000,   730.8701,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  212);
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
) values (  213,
     7,     6,'T4E',     2,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     2,     9.5000,   737.5300,     0.0000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  213);
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
) values (  214,
     7,     1,'EIS',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,    22.5000,   707.7460,   207.6000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  214);
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
) values (  215,
     7,     1,'EIS',     2,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     2,    22.5000,   707.7460,   337.1000,     0.0000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  215);
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
) values (  216,
     7,     1,'CSS',     1,     0,     1,     1,     1,     1,     1,     1,
     1,     1,     1,    22.5000,   720.2260,    86.5660,     0.0000,
     0.0000,     0.0000,   -11.5870
);
insert into APTP_data2tag values ('APTP-07',  216);
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
) values (  217,
     7,     1,'CSL',     1,     1,     1,     1,     1,     1,     1,     1,
     1,     1,     1,     0.0000,   756.0060,    91.9400,     0.0000,
     0.0000,     0.0000,   -11.5870
);
insert into APTP_data2tag values ('APTP-07',  217);
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
) values (  218,
     7,     1,'BIM',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     1,     0.0000,   107.0000,   516.4920,   -56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  218);
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
) values (  219,
     7,     2,'BIM',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     2,     0.0000,   217.0000,   516.4920,   -56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  219);
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
) values (  220,
     7,     3,'BIM',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     3,     0.0000,   327.0000,   516.4920,   -56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  220);
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
) values (  221,
     7,     4,'BIM',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     4,     0.0000,   437.0000,   516.4920,   -56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  221);
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
) values (  222,
     7,     5,'BIM',     1,     0,     0,     0,     0,     0,     0,     1,
     0,     0,     5,     0.0000,   547.0000,   516.4920,   -56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  222);
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
) values (  223,
     7,     6,'BIM',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     1,     0.0000,   107.0000,   516.4920,    56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  223);
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
) values (  224,
     7,     7,'BIM',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     2,     0.0000,   217.0000,   516.4920,    56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  224);
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
) values (  225,
     7,     8,'BIM',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     3,     0.0000,   327.0000,   516.4920,    56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  225);
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
) values (  226,
     7,     9,'BIM',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     4,     0.0000,   437.0000,   516.4920,    56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  226);
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
) values (  227,
     7,    10,'BIM',     1,     0,     0,     0,     0,     0,     0,     0,
     0,     1,     5,     0.0000,   547.0000,   516.4920,    56.6000,
     0.0000,     0.0000,     0.0000
);
insert into APTP_data2tag values ('APTP-07',  227);

comment on column APTP_data.APTP_data_id is 'Unique identifier';
comment on column APTP_data.VERS       is 'VERSION                                         ';
comment on column APTP_data.LINE       is 'LINE NUMBER                                     ';
comment on column APTP_data.TYP        is 'STATION TYPE                                    ';
comment on column APTP_data.I          is 'STATION AMDB INDEX                              ';
comment on column APTP_data.ICUT       is 'CUT-OUT INDEX,ZERO IF MISSING                   ';
comment on column APTP_data.IPHI_0     is 'PHI INDICATES OF OCTANTS                        ';
comment on column APTP_data.IZ         is 'Z (FOR BARREL) OR R (FOR END-CAPS) POS.         ';
comment on column APTP_data.DPHI       is 'RELATIVE PHI POSITION OF THE STATION IN         ';
comment on column APTP_data.Z          is 'Z POSITION OF THE LOWEST Z EDGE OF THE S        ';
comment on column APTP_data.R          is 'RADIAL POSITION OF ITS INNERMOST EDGE           ';
comment on column APTP_data.S          is 'ORTHO-RADIAL POSITION OF THE CENTER OF T        ';
comment on column APTP_data.ALFA       is 'ALFA ANGLE DEFINING THE DEVIATION [GRAD]        ';
comment on column APTP_data.BETA       is 'BETA ANGLE DEFINING THE DEVIATION               ';
comment on column APTP_data.GAMMA      is 'GAMMA ANGLE DEFINING THE DEVIATION              ';
