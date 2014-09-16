SET echo OFF;
set linesize 132;
drop   table ATLN_data2tag cascade constraints;
drop   table ATLN_data cascade constraints;
create table ATLN_data (
  ATLN_data_id   number(10),
VERS       number(10),
I       number(10),
ICOVOL       number(10),
ZPOVOL       float(63),
WIDVOL       float(63),
NAMVOL       varchar2(4),
JSTA       number(10)
) tablespace data02;

alter table ATLN_data add constraint ATLN_data_pk
primary key (ATLN_data_id)
using index tablespace indx02;

create table ATLN_data2tag (
  ATLN_vers      varchar2(255),
  ATLN_data_id   number(10)
) tablespace data02;

alter table ATLN_data2tag add constraint ATLN_data2tag_pk
primary key (ATLN_vers, ATLN_data_id)
using index tablespace indx02;
alter table ATLN_data2tag add constraint ATLN_data2tag_vers_fk
foreign key (ATLN_vers) references hvs_nodevers (vers_name);
alter table ATLN_data2tag add constraint ATLN_data2tag_data_fk
foreign key (ATLN_data_id) references ATLN_data (ATLN_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ATLN';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'ATLN';
delete from hvs_node     where NODE_NAME = 'ATLN';

insert into hvs_node     values ('ATLN','MuonSpectrometer', '');
insert into hvs_nodevers values ('ATLN', 'ATLN-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','ATLN','ATLN-07');
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    0,
     7,     1,     2,     0.0000,     0.0100,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',    0);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    1,
     7,     2,     3,     0.0100,     0.5000,'Hone',     1
);
insert into ATLN_data2tag values ('ATLN-07',    1);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    2,
     7,     3,     2,     0.5100,     0.1600,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',    2);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    3,
     7,     4,     1,     0.6700,     0.2800,'Gas',     1
);
insert into ATLN_data2tag values ('ATLN-07',    3);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    4,
     7,     5,     2,     0.9500,     0.0500,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',    4);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    5,
     7,     6,     3,     1.0000,     2.0000,'Hone',     1
);
insert into ATLN_data2tag values ('ATLN-07',    5);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    6,
     7,     7,     2,     3.0000,     0.0500,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',    6);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    7,
     7,     8,     1,     3.0500,     0.2800,'Gas',     1
);
insert into ATLN_data2tag values ('ATLN-07',    7);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    8,
     7,     9,     2,     3.3300,     0.1600,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',    8);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (    9,
     7,    10,     3,     3.4900,     2.0000,'Hone',     1
);
insert into ATLN_data2tag values ('ATLN-07',    9);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   10,
     7,    11,     2,     5.4900,     0.0500,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',   10);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   11,
     7,    12,     1,     5.5400,     0.2800,'Gas',     1
);
insert into ATLN_data2tag values ('ATLN-07',   11);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   12,
     7,    13,     2,     5.8200,     0.1600,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',   12);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   13,
     7,    14,     3,     5.9800,     0.5000,'Hone',     1
);
insert into ATLN_data2tag values ('ATLN-07',   13);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   14,
     7,    15,     2,     6.4800,     0.0100,'G10',     1
);
insert into ATLN_data2tag values ('ATLN-07',   14);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   15,
     7,     1,     2,     0.0000,     0.0100,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   15);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   16,
     7,     2,     3,     0.0100,     0.5000,'Hone',     2
);
insert into ATLN_data2tag values ('ATLN-07',   16);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   17,
     7,     3,     2,     0.5100,     0.1600,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   17);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   18,
     7,     4,     1,     0.6700,     0.2800,'Gas',     2
);
insert into ATLN_data2tag values ('ATLN-07',   18);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   19,
     7,     5,     2,     0.9500,     0.0500,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   19);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   20,
     7,     6,     3,     1.0000,     2.0000,'Hone',     2
);
insert into ATLN_data2tag values ('ATLN-07',   20);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   21,
     7,     7,     2,     3.0000,     0.0500,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   21);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   22,
     7,     8,     1,     3.0500,     0.2800,'Gas',     2
);
insert into ATLN_data2tag values ('ATLN-07',   22);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   23,
     7,     9,     2,     3.3300,     0.1600,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   23);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   24,
     7,    10,     3,     3.4900,     2.0000,'Hone',     2
);
insert into ATLN_data2tag values ('ATLN-07',   24);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   25,
     7,    11,     2,     5.4900,     0.0500,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   25);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   26,
     7,    12,     1,     5.5400,     0.2800,'Gas',     2
);
insert into ATLN_data2tag values ('ATLN-07',   26);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   27,
     7,    13,     2,     5.8200,     0.1600,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   27);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   28,
     7,    14,     3,     5.9800,     0.5000,'Hone',     2
);
insert into ATLN_data2tag values ('ATLN-07',   28);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   29,
     7,    15,     2,     6.4800,     0.0100,'G10',     2
);
insert into ATLN_data2tag values ('ATLN-07',   29);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   30,
     7,     1,     2,     0.0000,     0.0100,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   30);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   31,
     7,     2,     3,     0.0100,     0.5000,'Hone',     3
);
insert into ATLN_data2tag values ('ATLN-07',   31);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   32,
     7,     3,     2,     0.5100,     0.1600,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   32);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   33,
     7,     4,     1,     0.6700,     0.2800,'Gas',     3
);
insert into ATLN_data2tag values ('ATLN-07',   33);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   34,
     7,     5,     2,     0.9500,     0.0500,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   34);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   35,
     7,     6,     3,     1.0000,     2.0000,'Hone',     3
);
insert into ATLN_data2tag values ('ATLN-07',   35);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   36,
     7,     7,     2,     3.0000,     0.0500,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   36);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   37,
     7,     8,     1,     3.0500,     0.2800,'Gas',     3
);
insert into ATLN_data2tag values ('ATLN-07',   37);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   38,
     7,     9,     2,     3.3300,     0.1600,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   38);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   39,
     7,    10,     3,     3.4900,     2.0000,'Hone',     3
);
insert into ATLN_data2tag values ('ATLN-07',   39);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   40,
     7,    11,     2,     5.4900,     0.0500,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   40);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   41,
     7,    12,     1,     5.5400,     0.2800,'Gas',     3
);
insert into ATLN_data2tag values ('ATLN-07',   41);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   42,
     7,    13,     2,     5.8200,     0.1600,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   42);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   43,
     7,    14,     3,     5.9800,     0.5000,'Hone',     3
);
insert into ATLN_data2tag values ('ATLN-07',   43);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   44,
     7,    15,     2,     6.4800,     0.0100,'G10',     3
);
insert into ATLN_data2tag values ('ATLN-07',   44);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   45,
     7,     1,     2,     0.0000,     0.0100,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   45);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   46,
     7,     2,     3,     0.0100,     0.5000,'Hone',     4
);
insert into ATLN_data2tag values ('ATLN-07',   46);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   47,
     7,     3,     2,     0.5100,     0.1600,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   47);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   48,
     7,     4,     1,     0.6700,     0.2800,'Gas',     4
);
insert into ATLN_data2tag values ('ATLN-07',   48);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   49,
     7,     5,     2,     0.9500,     0.0500,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   49);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   50,
     7,     6,     3,     1.0000,     2.0000,'Hone',     4
);
insert into ATLN_data2tag values ('ATLN-07',   50);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   51,
     7,     7,     2,     3.0000,     0.0500,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   51);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   52,
     7,     8,     1,     3.0500,     0.2800,'Gas',     4
);
insert into ATLN_data2tag values ('ATLN-07',   52);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   53,
     7,     9,     2,     3.3300,     0.1600,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   53);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   54,
     7,    10,     3,     3.4900,     2.0000,'Hone',     4
);
insert into ATLN_data2tag values ('ATLN-07',   54);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   55,
     7,    11,     2,     5.4900,     0.0500,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   55);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   56,
     7,    12,     1,     5.5400,     0.2800,'Gas',     4
);
insert into ATLN_data2tag values ('ATLN-07',   56);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   57,
     7,    13,     2,     5.8200,     0.1600,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   57);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   58,
     7,    14,     3,     5.9800,     0.5000,'Hone',     4
);
insert into ATLN_data2tag values ('ATLN-07',   58);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   59,
     7,    15,     2,     6.4800,     0.0100,'G10',     4
);
insert into ATLN_data2tag values ('ATLN-07',   59);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   60,
     7,     1,     2,     0.0000,     0.0100,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   60);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   61,
     7,     2,     3,     0.0100,     0.5000,'Hone',     5
);
insert into ATLN_data2tag values ('ATLN-07',   61);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   62,
     7,     3,     2,     0.5100,     0.1600,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   62);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   63,
     7,     4,     1,     0.6700,     0.2800,'Gas',     5
);
insert into ATLN_data2tag values ('ATLN-07',   63);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   64,
     7,     5,     2,     0.9500,     0.0500,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   64);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   65,
     7,     6,     3,     1.0000,     2.0000,'Hone',     5
);
insert into ATLN_data2tag values ('ATLN-07',   65);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   66,
     7,     7,     2,     3.0000,     0.0500,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   66);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   67,
     7,     8,     1,     3.0500,     0.2800,'Gas',     5
);
insert into ATLN_data2tag values ('ATLN-07',   67);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   68,
     7,     9,     2,     3.3300,     0.1600,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   68);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   69,
     7,    10,     3,     3.4900,     2.0000,'Hone',     5
);
insert into ATLN_data2tag values ('ATLN-07',   69);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   70,
     7,    11,     2,     5.4900,     0.0500,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   70);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   71,
     7,    12,     1,     5.5400,     0.2800,'Gas',     5
);
insert into ATLN_data2tag values ('ATLN-07',   71);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   72,
     7,    13,     2,     5.8200,     0.1600,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   72);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   73,
     7,    14,     3,     5.9800,     0.5000,'Hone',     5
);
insert into ATLN_data2tag values ('ATLN-07',   73);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   74,
     7,    15,     2,     6.4800,     0.0100,'G10',     5
);
insert into ATLN_data2tag values ('ATLN-07',   74);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   75,
     7,     1,     2,     0.0000,     0.0100,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   75);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   76,
     7,     2,     3,     0.0100,     0.5000,'Hone',     6
);
insert into ATLN_data2tag values ('ATLN-07',   76);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   77,
     7,     3,     2,     0.5100,     0.1600,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   77);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   78,
     7,     4,     1,     0.6700,     0.2800,'Gas',     6
);
insert into ATLN_data2tag values ('ATLN-07',   78);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   79,
     7,     5,     2,     0.9500,     0.0500,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   79);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   80,
     7,     6,     3,     1.0000,     2.0000,'Hone',     6
);
insert into ATLN_data2tag values ('ATLN-07',   80);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   81,
     7,     7,     2,     3.0000,     0.0500,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   81);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   82,
     7,     8,     1,     3.0500,     0.2800,'Gas',     6
);
insert into ATLN_data2tag values ('ATLN-07',   82);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   83,
     7,     9,     2,     3.3300,     0.1600,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   83);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   84,
     7,    10,     3,     3.4900,     0.5000,'Hone',     6
);
insert into ATLN_data2tag values ('ATLN-07',   84);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   85,
     7,    11,     2,     3.9900,     0.0100,'G10',     6
);
insert into ATLN_data2tag values ('ATLN-07',   85);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   86,
     7,     1,     2,     0.0000,     0.0100,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   86);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   87,
     7,     2,     3,     0.0100,     0.5000,'Hone',     7
);
insert into ATLN_data2tag values ('ATLN-07',   87);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   88,
     7,     3,     2,     0.5100,     0.1600,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   88);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   89,
     7,     4,     1,     0.6700,     0.2800,'Gas',     7
);
insert into ATLN_data2tag values ('ATLN-07',   89);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   90,
     7,     5,     2,     0.9500,     0.0500,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   90);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   91,
     7,     6,     3,     1.0000,     2.0000,'Hone',     7
);
insert into ATLN_data2tag values ('ATLN-07',   91);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   92,
     7,     7,     2,     3.0000,     0.0500,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   92);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   93,
     7,     8,     1,     3.0500,     0.2800,'Gas',     7
);
insert into ATLN_data2tag values ('ATLN-07',   93);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   94,
     7,     9,     2,     3.3300,     0.1600,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   94);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   95,
     7,    10,     3,     3.4900,     0.5000,'Hone',     7
);
insert into ATLN_data2tag values ('ATLN-07',   95);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   96,
     7,    11,     2,     3.9900,     0.0100,'G10',     7
);
insert into ATLN_data2tag values ('ATLN-07',   96);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   97,
     7,     1,     2,     0.0000,     0.0100,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',   97);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   98,
     7,     2,     3,     0.0100,     0.5000,'Hone',     8
);
insert into ATLN_data2tag values ('ATLN-07',   98);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (   99,
     7,     3,     2,     0.5100,     0.1600,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',   99);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  100,
     7,     4,     1,     0.6700,     0.2800,'Gas',     8
);
insert into ATLN_data2tag values ('ATLN-07',  100);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  101,
     7,     5,     2,     0.9500,     0.0500,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',  101);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  102,
     7,     6,     3,     1.0000,     2.0000,'Hone',     8
);
insert into ATLN_data2tag values ('ATLN-07',  102);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  103,
     7,     7,     2,     3.0000,     0.0500,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',  103);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  104,
     7,     8,     1,     3.0500,     0.2800,'Gas',     8
);
insert into ATLN_data2tag values ('ATLN-07',  104);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  105,
     7,     9,     2,     3.3300,     0.1600,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',  105);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  106,
     7,    10,     3,     3.4900,     0.5000,'Hone',     8
);
insert into ATLN_data2tag values ('ATLN-07',  106);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  107,
     7,    11,     2,     3.9900,     0.0100,'G10',     8
);
insert into ATLN_data2tag values ('ATLN-07',  107);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  108,
     7,     1,     2,     0.0000,     0.0100,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  108);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  109,
     7,     2,     3,     0.0100,     0.5000,'Hone',     9
);
insert into ATLN_data2tag values ('ATLN-07',  109);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  110,
     7,     3,     2,     0.5100,     0.1600,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  110);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  111,
     7,     4,     1,     0.6700,     0.2800,'Gas',     9
);
insert into ATLN_data2tag values ('ATLN-07',  111);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  112,
     7,     5,     2,     0.9500,     0.0500,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  112);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  113,
     7,     6,     3,     1.0000,     2.0000,'Hone',     9
);
insert into ATLN_data2tag values ('ATLN-07',  113);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  114,
     7,     7,     2,     3.0000,     0.0500,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  114);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  115,
     7,     8,     1,     3.0500,     0.2800,'Gas',     9
);
insert into ATLN_data2tag values ('ATLN-07',  115);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  116,
     7,     9,     2,     3.3300,     0.1600,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  116);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  117,
     7,    10,     3,     3.4900,     0.5000,'Hone',     9
);
insert into ATLN_data2tag values ('ATLN-07',  117);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  118,
     7,    11,     2,     3.9900,     0.0100,'G10',     9
);
insert into ATLN_data2tag values ('ATLN-07',  118);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  119,
     7,     1,     2,     0.0000,     0.0100,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  119);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  120,
     7,     2,     3,     0.0100,     0.5000,'Hone',    10
);
insert into ATLN_data2tag values ('ATLN-07',  120);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  121,
     7,     3,     2,     0.5100,     0.1600,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  121);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  122,
     7,     4,     1,     0.6700,     0.2800,'Gas',    10
);
insert into ATLN_data2tag values ('ATLN-07',  122);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  123,
     7,     5,     2,     0.9500,     0.0500,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  123);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  124,
     7,     6,     3,     1.0000,     2.0000,'Hone',    10
);
insert into ATLN_data2tag values ('ATLN-07',  124);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  125,
     7,     7,     2,     3.0000,     0.0500,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  125);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  126,
     7,     8,     1,     3.0500,     0.2800,'Gas',    10
);
insert into ATLN_data2tag values ('ATLN-07',  126);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  127,
     7,     9,     2,     3.3300,     0.1600,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  127);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  128,
     7,    10,     3,     3.4900,     0.5000,'Hone',    10
);
insert into ATLN_data2tag values ('ATLN-07',  128);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  129,
     7,    11,     2,     3.9900,     0.0100,'G10',    10
);
insert into ATLN_data2tag values ('ATLN-07',  129);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  130,
     7,     1,     2,     0.0000,     0.0100,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  130);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  131,
     7,     2,     3,     0.0100,     0.5000,'Hone',    11
);
insert into ATLN_data2tag values ('ATLN-07',  131);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  132,
     7,     3,     2,     0.5100,     0.1600,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  132);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  133,
     7,     4,     1,     0.6700,     0.2800,'Gas',    11
);
insert into ATLN_data2tag values ('ATLN-07',  133);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  134,
     7,     5,     2,     0.9500,     0.0500,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  134);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  135,
     7,     6,     3,     1.0000,     2.0000,'Hone',    11
);
insert into ATLN_data2tag values ('ATLN-07',  135);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  136,
     7,     7,     2,     3.0000,     0.0500,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  136);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  137,
     7,     8,     1,     3.0500,     0.2800,'Gas',    11
);
insert into ATLN_data2tag values ('ATLN-07',  137);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  138,
     7,     9,     2,     3.3300,     0.1600,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  138);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  139,
     7,    10,     3,     3.4900,     0.5000,'Hone',    11
);
insert into ATLN_data2tag values ('ATLN-07',  139);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  140,
     7,    11,     2,     3.9900,     0.0100,'G10',    11
);
insert into ATLN_data2tag values ('ATLN-07',  140);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  141,
     7,     1,     2,     0.0000,     0.0100,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  141);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  142,
     7,     2,     3,     0.0100,     0.5000,'Hone',    12
);
insert into ATLN_data2tag values ('ATLN-07',  142);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  143,
     7,     3,     2,     0.5100,     0.1600,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  143);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  144,
     7,     4,     1,     0.6700,     0.2800,'Gas',    12
);
insert into ATLN_data2tag values ('ATLN-07',  144);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  145,
     7,     5,     2,     0.9500,     0.0500,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  145);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  146,
     7,     6,     3,     1.0000,     2.0000,'Hone',    12
);
insert into ATLN_data2tag values ('ATLN-07',  146);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  147,
     7,     7,     2,     3.0000,     0.0500,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  147);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  148,
     7,     8,     1,     3.0500,     0.2800,'Gas',    12
);
insert into ATLN_data2tag values ('ATLN-07',  148);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  149,
     7,     9,     2,     3.3300,     0.1600,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  149);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  150,
     7,    10,     3,     3.4900,     0.5000,'Hone',    12
);
insert into ATLN_data2tag values ('ATLN-07',  150);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  151,
     7,    11,     2,     3.9900,     0.0100,'G10',    12
);
insert into ATLN_data2tag values ('ATLN-07',  151);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  152,
     7,     1,     2,     0.0000,     0.0100,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  152);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  153,
     7,     2,     3,     0.0100,     0.5000,'Hone',    13
);
insert into ATLN_data2tag values ('ATLN-07',  153);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  154,
     7,     3,     2,     0.5100,     0.1600,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  154);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  155,
     7,     4,     1,     0.6700,     0.2800,'Gas',    13
);
insert into ATLN_data2tag values ('ATLN-07',  155);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  156,
     7,     5,     2,     0.9500,     0.0500,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  156);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  157,
     7,     6,     3,     1.0000,     2.0000,'Hone',    13
);
insert into ATLN_data2tag values ('ATLN-07',  157);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  158,
     7,     7,     2,     3.0000,     0.0500,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  158);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  159,
     7,     8,     1,     3.0500,     0.2800,'Gas',    13
);
insert into ATLN_data2tag values ('ATLN-07',  159);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  160,
     7,     9,     2,     3.3300,     0.1600,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  160);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  161,
     7,    10,     3,     3.4900,     0.5000,'Hone',    13
);
insert into ATLN_data2tag values ('ATLN-07',  161);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  162,
     7,    11,     2,     3.9900,     0.0100,'G10',    13
);
insert into ATLN_data2tag values ('ATLN-07',  162);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  163,
     7,     1,     2,     0.0000,     0.0100,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  163);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  164,
     7,     2,     3,     0.0100,     0.5000,'Hone',    14
);
insert into ATLN_data2tag values ('ATLN-07',  164);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  165,
     7,     3,     2,     0.5100,     0.1600,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  165);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  166,
     7,     4,     1,     0.6700,     0.2800,'Gas',    14
);
insert into ATLN_data2tag values ('ATLN-07',  166);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  167,
     7,     5,     2,     0.9500,     0.0500,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  167);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  168,
     7,     6,     3,     1.0000,     2.0000,'Hone',    14
);
insert into ATLN_data2tag values ('ATLN-07',  168);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  169,
     7,     7,     2,     3.0000,     0.0500,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  169);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  170,
     7,     8,     1,     3.0500,     0.2800,'Gas',    14
);
insert into ATLN_data2tag values ('ATLN-07',  170);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  171,
     7,     9,     2,     3.3300,     0.1600,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  171);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  172,
     7,    10,     3,     3.4900,     0.5000,'Hone',    14
);
insert into ATLN_data2tag values ('ATLN-07',  172);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  173,
     7,    11,     2,     3.9900,     0.0100,'G10',    14
);
insert into ATLN_data2tag values ('ATLN-07',  173);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  174,
     7,     1,     2,     0.0000,     0.0100,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  174);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  175,
     7,     2,     3,     0.0100,     0.5000,'Hone',    15
);
insert into ATLN_data2tag values ('ATLN-07',  175);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  176,
     7,     3,     2,     0.5100,     0.1600,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  176);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  177,
     7,     4,     1,     0.6700,     0.2800,'Gas',    15
);
insert into ATLN_data2tag values ('ATLN-07',  177);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  178,
     7,     5,     2,     0.9500,     0.0500,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  178);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  179,
     7,     6,     3,     1.0000,     2.0000,'Hone',    15
);
insert into ATLN_data2tag values ('ATLN-07',  179);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  180,
     7,     7,     2,     3.0000,     0.0500,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  180);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  181,
     7,     8,     1,     3.0500,     0.2800,'Gas',    15
);
insert into ATLN_data2tag values ('ATLN-07',  181);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  182,
     7,     9,     2,     3.3300,     0.1600,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  182);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  183,
     7,    10,     3,     3.4900,     0.5000,'Hone',    15
);
insert into ATLN_data2tag values ('ATLN-07',  183);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  184,
     7,    11,     2,     3.9900,     0.0100,'G10',    15
);
insert into ATLN_data2tag values ('ATLN-07',  184);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  185,
     7,     1,     2,     0.0000,     0.0100,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  185);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  186,
     7,     2,     3,     0.0100,     0.5000,'Hone',    16
);
insert into ATLN_data2tag values ('ATLN-07',  186);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  187,
     7,     3,     2,     0.5100,     0.1600,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  187);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  188,
     7,     4,     1,     0.6700,     0.2800,'Gas',    16
);
insert into ATLN_data2tag values ('ATLN-07',  188);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  189,
     7,     5,     2,     0.9500,     0.0500,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  189);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  190,
     7,     6,     3,     1.0000,     2.0000,'Hone',    16
);
insert into ATLN_data2tag values ('ATLN-07',  190);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  191,
     7,     7,     2,     3.0000,     0.0500,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  191);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  192,
     7,     8,     1,     3.0500,     0.2800,'Gas',    16
);
insert into ATLN_data2tag values ('ATLN-07',  192);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  193,
     7,     9,     2,     3.3300,     0.1600,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  193);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  194,
     7,    10,     3,     3.4900,     0.5000,'Hone',    16
);
insert into ATLN_data2tag values ('ATLN-07',  194);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  195,
     7,    11,     2,     3.9900,     0.0100,'G10',    16
);
insert into ATLN_data2tag values ('ATLN-07',  195);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  196,
     7,     1,     2,     0.0000,     0.0100,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  196);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  197,
     7,     2,     3,     0.0100,     0.5000,'Hone',    17
);
insert into ATLN_data2tag values ('ATLN-07',  197);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  198,
     7,     3,     2,     0.5100,     0.1600,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  198);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  199,
     7,     4,     1,     0.6700,     0.2800,'Gas',    17
);
insert into ATLN_data2tag values ('ATLN-07',  199);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  200,
     7,     5,     2,     0.9500,     0.0500,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  200);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  201,
     7,     6,     3,     1.0000,     2.0000,'Hone',    17
);
insert into ATLN_data2tag values ('ATLN-07',  201);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  202,
     7,     7,     2,     3.0000,     0.0500,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  202);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  203,
     7,     8,     1,     3.0500,     0.2800,'Gas',    17
);
insert into ATLN_data2tag values ('ATLN-07',  203);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  204,
     7,     9,     2,     3.3300,     0.1600,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  204);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  205,
     7,    10,     3,     3.4900,     0.5000,'Hone',    17
);
insert into ATLN_data2tag values ('ATLN-07',  205);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  206,
     7,    11,     2,     3.9900,     0.0100,'G10',    17
);
insert into ATLN_data2tag values ('ATLN-07',  206);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  207,
     7,     1,     2,     0.0000,     0.0100,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  207);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  208,
     7,     2,     3,     0.0100,     0.5000,'Hone',    18
);
insert into ATLN_data2tag values ('ATLN-07',  208);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  209,
     7,     3,     2,     0.5100,     0.1600,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  209);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  210,
     7,     4,     1,     0.6700,     0.2800,'Gas',    18
);
insert into ATLN_data2tag values ('ATLN-07',  210);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  211,
     7,     5,     2,     0.9500,     0.0500,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  211);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  212,
     7,     6,     3,     1.0000,     2.0000,'Hone',    18
);
insert into ATLN_data2tag values ('ATLN-07',  212);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  213,
     7,     7,     2,     3.0000,     0.0500,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  213);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  214,
     7,     8,     1,     3.0500,     0.2800,'Gas',    18
);
insert into ATLN_data2tag values ('ATLN-07',  214);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  215,
     7,     9,     2,     3.3300,     0.1600,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  215);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  216,
     7,    10,     3,     3.4900,     0.5000,'Hone',    18
);
insert into ATLN_data2tag values ('ATLN-07',  216);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  217,
     7,    11,     2,     3.9900,     0.0100,'G10',    18
);
insert into ATLN_data2tag values ('ATLN-07',  217);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  218,
     7,     1,     2,     0.0000,     0.0100,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  218);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  219,
     7,     2,     3,     0.0100,     0.5000,'Hone',    19
);
insert into ATLN_data2tag values ('ATLN-07',  219);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  220,
     7,     3,     2,     0.5100,     0.1600,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  220);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  221,
     7,     4,     1,     0.6700,     0.2800,'Gas',    19
);
insert into ATLN_data2tag values ('ATLN-07',  221);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  222,
     7,     5,     2,     0.9500,     0.0500,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  222);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  223,
     7,     6,     3,     1.0000,     2.0000,'Hone',    19
);
insert into ATLN_data2tag values ('ATLN-07',  223);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  224,
     7,     7,     2,     3.0000,     0.0500,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  224);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  225,
     7,     8,     1,     3.0500,     0.2800,'Gas',    19
);
insert into ATLN_data2tag values ('ATLN-07',  225);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  226,
     7,     9,     2,     3.3300,     0.1600,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  226);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  227,
     7,    10,     3,     3.4900,     0.5000,'Hone',    19
);
insert into ATLN_data2tag values ('ATLN-07',  227);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  228,
     7,    11,     2,     3.9900,     0.0100,'G10',    19
);
insert into ATLN_data2tag values ('ATLN-07',  228);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  229,
     7,     1,     2,     0.0000,     0.0100,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  229);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  230,
     7,     2,     3,     0.0100,     0.5000,'Hone',    20
);
insert into ATLN_data2tag values ('ATLN-07',  230);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  231,
     7,     3,     2,     0.5100,     0.1600,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  231);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  232,
     7,     4,     1,     0.6700,     0.2800,'Gas',    20
);
insert into ATLN_data2tag values ('ATLN-07',  232);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  233,
     7,     5,     2,     0.9500,     0.0500,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  233);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  234,
     7,     6,     3,     1.0000,     2.0000,'Hone',    20
);
insert into ATLN_data2tag values ('ATLN-07',  234);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  235,
     7,     7,     2,     3.0000,     0.0500,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  235);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  236,
     7,     8,     1,     3.0500,     0.2800,'Gas',    20
);
insert into ATLN_data2tag values ('ATLN-07',  236);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  237,
     7,     9,     2,     3.3300,     0.1600,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  237);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  238,
     7,    10,     3,     3.4900,     0.5000,'Hone',    20
);
insert into ATLN_data2tag values ('ATLN-07',  238);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  239,
     7,    11,     2,     3.9900,     0.0100,'G10',    20
);
insert into ATLN_data2tag values ('ATLN-07',  239);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  240,
     7,     1,     2,     0.0000,     0.0100,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  240);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  241,
     7,     2,     3,     0.0100,     0.5000,'Hone',    21
);
insert into ATLN_data2tag values ('ATLN-07',  241);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  242,
     7,     3,     2,     0.5100,     0.1600,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  242);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  243,
     7,     4,     1,     0.6700,     0.2800,'Gas',    21
);
insert into ATLN_data2tag values ('ATLN-07',  243);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  244,
     7,     5,     2,     0.9500,     0.0500,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  244);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  245,
     7,     6,     3,     1.0000,     2.0000,'Hone',    21
);
insert into ATLN_data2tag values ('ATLN-07',  245);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  246,
     7,     7,     2,     3.0000,     0.0500,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  246);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  247,
     7,     8,     1,     3.0500,     0.2800,'Gas',    21
);
insert into ATLN_data2tag values ('ATLN-07',  247);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  248,
     7,     9,     2,     3.3300,     0.1600,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  248);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  249,
     7,    10,     3,     3.4900,     0.5000,'Hone',    21
);
insert into ATLN_data2tag values ('ATLN-07',  249);
insert into ATLN_data (
ATLN_data_id,
VERS,
I,
ICOVOL,
ZPOVOL,
WIDVOL,
NAMVOL,
JSTA
) values (  250,
     7,    11,     2,     3.9900,     0.0100,'G10',    21
);
insert into ATLN_data2tag values ('ATLN-07',  250);

comment on column ATLN_data.ATLN_data_id is 'Unique identifier';
comment on column ATLN_data.VERS       is 'VERSION                                         ';
comment on column ATLN_data.I          is 'INDEX                                           ';
comment on column ATLN_data.ICOVOL     is 'MATERIAL CODE                                   ';
comment on column ATLN_data.ZPOVOL     is 'RELATIVE Z POSITION                             ';
comment on column ATLN_data.WIDVOL     is 'Z WIDTH                                         ';
comment on column ATLN_data.NAMVOL     is 'MATERIAL NAME                                   ';
comment on column ATLN_data.JSTA       is 'JSTA INDEX                                      ';
