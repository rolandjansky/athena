SET echo OFF;
set linesize 132;
drop   table ALIN_data2tag cascade constraints;
drop   table ALIN_data cascade constraints;
create table ALIN_data (
  ALIN_data_id   number(10),
VERS       number(10),
DX       float(63),
DY       float(63),
I       number(10),
WIDTH_XS       float(63),
WIDTH_XL       float(63),
LENGTH_Y       float(63),
EXCENT       float(63),
DEAD1       float(63),
JTYP       number(10),
INDX       number(10),
ICUT       number(10)
) tablespace data02;

alter table ALIN_data add constraint ALIN_data_pk
primary key (ALIN_data_id)
using index tablespace indx02;

create table ALIN_data2tag (
  ALIN_vers      varchar2(255),
  ALIN_data_id   number(10)
) tablespace data02;

alter table ALIN_data2tag add constraint ALIN_data2tag_pk
primary key (ALIN_vers, ALIN_data_id)
using index tablespace indx02;
alter table ALIN_data2tag add constraint ALIN_data2tag_vers_fk
foreign key (ALIN_vers) references hvs_nodevers (vers_name);
alter table ALIN_data2tag add constraint ALIN_data2tag_data_fk
foreign key (ALIN_data_id) references ALIN_data (ALIN_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ALIN';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'ALIN';
delete from hvs_node     where NODE_NAME = 'ALIN';

insert into hvs_node     values ('ALIN','MuonSpectrometer', '');
insert into hvs_nodevers values ('ALIN', 'ALIN-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','ALIN','ALIN-07');
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    0,
     7,    88.0000,    95.5000,     3,    28.0000,    28.0000,    12.1580,
     0.0000,    48.8500,     4,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',    0);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    1,
     7,    88.0000,   100.5000,     6,    28.0000,    28.0000,    12.1580,
     0.0000,    48.8500,     4,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',    1);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    2,
     7,   -93.0000,    95.5000,     3,     8.0000,     8.0000,    12.1580,
     0.0000,    48.8500,     4,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',    2);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    3,
     7,   -93.0000,   100.5000,     6,     8.0000,     8.0000,    12.1580,
     0.0000,    48.8500,     4,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',    3);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    4,
     7,   143.5000,     0.0000,     2,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    4);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    5,
     7,   143.5000,     0.0000,     5,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    5);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    6,
     7,   143.5000,     0.0000,     8,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    6);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    7,
     7,   143.5000,     0.0000,    14,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    7);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    8,
     7,   143.5000,     0.0000,    16,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    8);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (    9,
     7,   143.5000,     0.0000,    19,    35.0000,    35.0000,    48.0000,
     0.0000,     0.0000,     4,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',    9);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   10,
     7,   119.3200,   -11.0830,    14,    68.5000,    68.5000,     8.6970,
     0.0000,     0.0000,     4,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   10);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   11,
     7,  -119.3200,   -11.0830,    14,    68.5000,    68.5000,     8.6970,
     0.0000,     0.0000,     4,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   11);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   12,
     7,   -95.0000,   148.5500,    23,    10.0000,    10.0000,    10.0000,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   12);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   13,
     7,   -95.0000,   148.5500,    24,    10.0000,    10.0000,    10.0000,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   13);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   14,
     7,    95.0000,   148.5500,    27,    10.0000,    10.0000,    10.0000,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   14);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   15,
     7,    95.0000,   148.5500,    28,    10.0000,    10.0000,    10.0000,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   15);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   16,
     7,   -98.5000,   167.0000,    31,    10.0000,    10.0000,    18.1210,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   16);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   17,
     7,   -98.5000,   167.0000,    32,    10.0000,    10.0000,    18.1210,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   17);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   18,
     7,    98.5000,   167.0000,    35,    10.0000,    10.0000,    18.1210,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   18);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   19,
     7,    98.5000,   167.0000,    36,    10.0000,    10.0000,    18.1210,
     0.0000,     0.0000,     4,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   19);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   20,
     7,   -95.0000,   105.0000,    20,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   20);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   21,
     7,   -95.0000,   105.0000,    21,    10.0000,    10.0000,    21.8000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   21);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   22,
     7,    95.0000,   105.0000,    24,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   22);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   23,
     7,    95.0000,   105.0000,    25,    10.0000,    10.0000,    21.8000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   23);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   24,
     7,   -99.0000,   153.5000,    28,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   24);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   25,
     7,   -99.0000,   160.7000,    29,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   25);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   26,
     7,    99.0000,   153.5000,    32,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   26);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   27,
     7,    99.0000,   160.7000,    33,    10.0000,    10.0000,    18.0000,
     0.0000,    39.2000,     4,     6,     1
);
insert into ALIN_data2tag values ('ALIN-07',   27);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   28,
     7,   104.5000,    63.0000,     2,    14.0000,    14.0000,    20.0000,
     0.0000,    49.7000,     6,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   28);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   29,
     7,   104.5000,    68.4200,     5,    14.0000,    14.0000,    20.0000,
     0.0000,    49.7000,     6,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   29);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   30,
     7,   145.1700,    82.4300,     8,    87.0000,    87.0000,    12.3750,
     0.0000,    49.7000,     6,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   30);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   31,
     7,   145.1700,   129.5300,    14,    87.0000,    87.0000,    12.3750,
     0.0000,    49.7000,     6,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   31);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   32,
     7,    60.0700,   102.1200,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   32);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   33,
     7,    60.0700,   102.1200,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   33);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   34,
     7,    60.0700,     0.0000,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   34);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   35,
     7,    60.0700,     0.0000,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   35);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   36,
     7,    60.0700,   102.1200,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',   36);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   37,
     7,    60.0700,   102.1200,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',   37);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   38,
     7,    60.0700,     0.0000,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',   38);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   39,
     7,    60.0700,     0.0000,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     2,     1
);
insert into ALIN_data2tag values ('ALIN-07',   39);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   40,
     7,   -60.0700,   102.1200,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   40);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   41,
     7,   -60.0700,   102.1200,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   41);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   42,
     7,   -60.0700,     0.0000,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   42);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   43,
     7,   -60.0700,     0.0000,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     4,     1
);
insert into ALIN_data2tag values ('ALIN-07',   43);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   44,
     7,   -60.0700,   102.1200,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   44);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   45,
     7,   -60.0700,   102.1200,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   45);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   46,
     7,   -60.0700,     0.0000,     5,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   46);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   47,
     7,   -60.0700,     0.0000,    11,   257.0000,   257.0000,    18.0200,
     0.0000,     0.0000,    11,     5,     1
);
insert into ALIN_data2tag values ('ALIN-07',   47);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   48,
     7,   -36.6300,   127.0000,     1,    58.3000,    58.3000,    11.0000,
     0.0000,     0.0000,    19,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   48);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   49,
     7,   -36.6300,   127.0000,     7,    58.3000,    58.3000,    11.0000,
     0.0000,     0.0000,    19,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   49);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   50,
     7,   -94.7300,   106.5000,     1,    58.1000,    58.1000,    11.5000,
     0.0000,    21.7200,    19,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',   50);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   51,
     7,   -97.2800,   117.1360,     7,    60.2000,    60.2000,    11.5000,
     0.0000,    21.7200,    19,     3,     1
);
insert into ALIN_data2tag values ('ALIN-07',   51);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   52,
     7,   -19.5400,   302.8000,     1,    15.0000,    15.0000,    15.0000,
     0.0000,     0.0000,    42,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   52);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   53,
     7,   -34.9700,   676.7500,     2,    16.0000,    16.0000,    20.5000,
     0.0000,    21.0000,    43,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   53);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   54,
     7,   -19.5400,   302.8000,     1,    15.0000,    15.0000,    15.0000,
     0.0000,     0.0000,    44,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   54);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   55,
     7,   -34.9800,   729.9900,     2,    16.0000,    16.0000,    20.5000,
     0.0000,    21.0000,    45,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   55);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   56,
     7,   -19.5400,   302.8000,     1,    15.0000,    15.0000,    15.0000,
     0.0000,     0.0000,    46,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   56);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   57,
     7,   -34.9800,   746.8200,     2,    16.0000,    15.0000,    20.5000,
     0.0000,    21.0000,    47,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   57);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   58,
     7,     0.0000,   114.6900,     1,   120.0000,   120.0000,    20.0000,
     0.0000,     0.0000,    52,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   58);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   59,
     7,     0.0000,   112.9200,     2,   120.0000,   120.0000,    20.0000,
     0.0000,     0.0000,    52,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   59);
insert into ALIN_data (
ALIN_data_id,
VERS,
DX,
DY,
I,
WIDTH_XS,
WIDTH_XL,
LENGTH_Y,
EXCENT,
DEAD1,
JTYP,
INDX,
ICUT
) values (   60,
     7,     0.0000,   111.1500,     3,   120.0000,   120.0000,    20.0000,
     0.0000,     0.0000,    52,     1,     1
);
insert into ALIN_data2tag values ('ALIN-07',   60);

comment on column ALIN_data.ALIN_data_id is 'Unique identifier';
comment on column ALIN_data.VERS       is 'VERSION                                         ';
comment on column ALIN_data.DX         is 'X RELATIVE POSITION OF THE SUB-CUT              ';
comment on column ALIN_data.DY         is 'Y RELATIVE POSITION OF THE SUB-CUT              ';
comment on column ALIN_data.I          is 'SERIAL NB. OF THE OBJECT IN WHICH THE SU        ';
comment on column ALIN_data.WIDTH_XS   is 'S DIMENSIONS OF THE SUB-CUT                     ';
comment on column ALIN_data.WIDTH_XL   is 'L DIMENSIONS OF THE SUB-CUT                     ';
comment on column ALIN_data.LENGTH_Y   is 'Y DIMENSIONS OF THE SUB-CUT                     ';
comment on column ALIN_data.EXCENT     is 'EXC DIMENSIONS OF THE SUB-CUT                   ';
comment on column ALIN_data.DEAD1      is 'D1 DIMENSIONS OF THE SUB-CUT                    ';
comment on column ALIN_data.JTYP       is 'STATION TYPE                                    ';
comment on column ALIN_data.INDX       is 'INDEX                                           ';
comment on column ALIN_data.ICUT       is 'CUT-OUT INDEX                                   ';
