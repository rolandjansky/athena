SET echo OFF;
set linesize 132;
drop   table ASMP_data2tag cascade constraints;
drop   table ASMP_data cascade constraints;
create table ASMP_data (
  ASMP_data_id   number(10),
VERS       number(10),
INDX       number(10),
N       number(10),
JTYP       number(10)
) ;

alter table ASMP_data add constraint ASMP_data_pk
primary key (ASMP_data_id);

create table ASMP_data2tag (
  ASMP_vers      varchar2(255),
  ASMP_data_id   number(10)
) ;

alter table ASMP_data2tag add constraint ASMP_data2tag_pk
primary key (ASMP_vers, ASMP_data_id);
alter table ASMP_data2tag add constraint ASMP_data2tag_vers_fk
foreign key (ASMP_vers) references hvs_nodevers (vers_name);
alter table ASMP_data2tag add constraint ASMP_data2tag_data_fk
foreign key (ASMP_data_id) references ASMP_data (ASMP_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ASMP';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'ASMP';
delete from hvs_node     where NODE_NAME = 'ASMP';

insert into hvs_node     values ('ASMP','MuonSpectrometer', '');
insert into hvs_nodevers values ('ASMP', 'ASMP-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','ASMP','ASMP-07');
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    0,
     7,     1,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    0);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    1,
     7,     2,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    1);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    2,
     7,     3,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    2);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    3,
     7,     4,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    3);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    4,
     7,     5,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    4);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    5,
     7,     6,     7,     1
);
insert into ASMP_data2tag values ('ASMP-07',    5);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    6,
     7,     1,     1,     2
);
insert into ASMP_data2tag values ('ASMP-07',    6);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    7,
     7,     2,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',    7);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    8,
     7,     3,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',    8);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (    9,
     7,     4,     1,     2
);
insert into ASMP_data2tag values ('ASMP-07',    9);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   10,
     7,     5,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   10);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   11,
     7,     6,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   11);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   12,
     7,     7,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   12);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   13,
     7,     8,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   13);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   14,
     7,     9,     1,     2
);
insert into ASMP_data2tag values ('ASMP-07',   14);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   15,
     7,    10,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   15);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   16,
     7,    11,     9,     2
);
insert into ASMP_data2tag values ('ASMP-07',   16);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   17,
     7,    12,     1,     2
);
insert into ASMP_data2tag values ('ASMP-07',   17);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   18,
     7,     1,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   18);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   19,
     7,     2,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   19);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   20,
     7,     3,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   20);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   21,
     7,     4,     4,     3
);
insert into ASMP_data2tag values ('ASMP-07',   21);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   22,
     7,     5,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   22);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   23,
     7,     6,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   23);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   24,
     7,     7,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   24);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   25,
     7,     8,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   25);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   26,
     7,     9,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   26);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   27,
     7,    10,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   27);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   28,
     7,    11,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   28);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   29,
     7,    12,    21,     3
);
insert into ASMP_data2tag values ('ASMP-07',   29);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   30,
     7,    13,     4,     3
);
insert into ASMP_data2tag values ('ASMP-07',   30);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   31,
     7,    14,    15,     3
);
insert into ASMP_data2tag values ('ASMP-07',   31);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   32,
     7,     1,    15,     4
);
insert into ASMP_data2tag values ('ASMP-07',   32);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   33,
     7,     2,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   33);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   34,
     7,     3,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   34);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   35,
     7,     4,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   35);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   36,
     7,     5,    37,     4
);
insert into ASMP_data2tag values ('ASMP-07',   36);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   37,
     7,     6,    34,     4
);
insert into ASMP_data2tag values ('ASMP-07',   37);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   38,
     7,     7,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   38);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   39,
     7,     8,    37,     4
);
insert into ASMP_data2tag values ('ASMP-07',   39);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   40,
     7,     9,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   40);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   41,
     7,    10,    34,     4
);
insert into ASMP_data2tag values ('ASMP-07',   41);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   42,
     7,    11,    15,     4
);
insert into ASMP_data2tag values ('ASMP-07',   42);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   43,
     7,    12,    21,     4
);
insert into ASMP_data2tag values ('ASMP-07',   43);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   44,
     7,     1,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   44);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   45,
     7,     2,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   45);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   46,
     7,     3,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   46);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   47,
     7,     4,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   47);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   48,
     7,     5,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   48);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   49,
     7,     6,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   49);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   50,
     7,     7,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   50);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   51,
     7,     8,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   51);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   52,
     7,     9,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   52);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   53,
     7,    10,    14,     5
);
insert into ASMP_data2tag values ('ASMP-07',   53);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   54,
     7,     1,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   54);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   55,
     7,     2,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   55);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   56,
     7,     3,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   56);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   57,
     7,     4,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   57);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   58,
     7,     5,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   58);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   59,
     7,     6,    14,     6
);
insert into ASMP_data2tag values ('ASMP-07',   59);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   60,
     7,     1,     5,     7
);
insert into ASMP_data2tag values ('ASMP-07',   60);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   61,
     7,     1,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   61);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   62,
     7,     2,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   62);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   63,
     7,     3,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   63);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   64,
     7,     4,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   64);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   65,
     7,     5,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   65);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   66,
     7,     6,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   66);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   67,
     7,     7,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   67);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   68,
     7,     8,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   68);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   69,
     7,     9,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   69);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   70,
     7,    10,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   70);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   71,
     7,    11,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   71);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   72,
     7,    12,     7,     8
);
insert into ASMP_data2tag values ('ASMP-07',   72);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   73,
     7,     1,    18,     9
);
insert into ASMP_data2tag values ('ASMP-07',   73);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   74,
     7,     2,    21,     9
);
insert into ASMP_data2tag values ('ASMP-07',   74);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   75,
     7,     3,    21,     9
);
insert into ASMP_data2tag values ('ASMP-07',   75);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   76,
     7,     4,    21,     9
);
insert into ASMP_data2tag values ('ASMP-07',   76);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   77,
     7,     5,    21,     9
);
insert into ASMP_data2tag values ('ASMP-07',   77);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   78,
     7,     6,    18,     9
);
insert into ASMP_data2tag values ('ASMP-07',   78);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   79,
     7,     1,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   79);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   80,
     7,     2,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   80);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   81,
     7,     3,    14,    10
);
insert into ASMP_data2tag values ('ASMP-07',   81);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   82,
     7,     4,    15,    10
);
insert into ASMP_data2tag values ('ASMP-07',   82);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   83,
     7,     5,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   83);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   84,
     7,     6,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   84);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   85,
     7,     7,    14,    10
);
insert into ASMP_data2tag values ('ASMP-07',   85);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   86,
     7,     8,    15,    10
);
insert into ASMP_data2tag values ('ASMP-07',   86);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   87,
     7,     9,    14,    10
);
insert into ASMP_data2tag values ('ASMP-07',   87);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   88,
     7,    10,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   88);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   89,
     7,    11,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   89);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   90,
     7,    12,    15,    10
);
insert into ASMP_data2tag values ('ASMP-07',   90);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   91,
     7,    13,    14,    10
);
insert into ASMP_data2tag values ('ASMP-07',   91);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   92,
     7,    14,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   92);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   93,
     7,    15,    21,    10
);
insert into ASMP_data2tag values ('ASMP-07',   93);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   94,
     7,    16,    15,    10
);
insert into ASMP_data2tag values ('ASMP-07',   94);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   95,
     7,     1,    11,    11
);
insert into ASMP_data2tag values ('ASMP-07',   95);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   96,
     7,     2,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',   96);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   97,
     7,     3,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',   97);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   98,
     7,     4,    11,    11
);
insert into ASMP_data2tag values ('ASMP-07',   98);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (   99,
     7,     5,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',   99);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  100,
     7,     6,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',  100);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  101,
     7,     7,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',  101);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  102,
     7,     8,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',  102);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  103,
     7,     9,    11,    11
);
insert into ASMP_data2tag values ('ASMP-07',  103);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  104,
     7,    10,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',  104);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  105,
     7,    11,    15,    11
);
insert into ASMP_data2tag values ('ASMP-07',  105);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  106,
     7,     1,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  106);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  107,
     7,     2,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  107);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  108,
     7,     3,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  108);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  109,
     7,     4,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  109);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  110,
     7,     5,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  110);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  111,
     7,     6,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  111);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  112,
     7,     7,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  112);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  113,
     7,     8,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  113);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  114,
     7,     9,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  114);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  115,
     7,    10,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  115);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  116,
     7,    11,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  116);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  117,
     7,    12,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  117);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  118,
     7,    13,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  118);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  119,
     7,    14,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  119);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  120,
     7,    15,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  120);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  121,
     7,    16,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  121);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  122,
     7,    17,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  122);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  123,
     7,    18,     7,    14
);
insert into ASMP_data2tag values ('ASMP-07',  123);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  124,
     7,     1,     7,    15
);
insert into ASMP_data2tag values ('ASMP-07',  124);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  125,
     7,     2,     7,    15
);
insert into ASMP_data2tag values ('ASMP-07',  125);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  126,
     7,     3,     7,    15
);
insert into ASMP_data2tag values ('ASMP-07',  126);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  127,
     7,     1,     7,    16
);
insert into ASMP_data2tag values ('ASMP-07',  127);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  128,
     7,     2,     7,    16
);
insert into ASMP_data2tag values ('ASMP-07',  128);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  129,
     7,     1,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  129);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  130,
     7,     2,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  130);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  131,
     7,     3,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  131);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  132,
     7,     4,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  132);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  133,
     7,     5,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  133);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  134,
     7,     6,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  134);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  135,
     7,     7,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  135);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  136,
     7,     8,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  136);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  137,
     7,     9,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  137);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  138,
     7,    10,     7,    18
);
insert into ASMP_data2tag values ('ASMP-07',  138);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  139,
     7,     1,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  139);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  140,
     7,     2,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  140);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  141,
     7,     3,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  141);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  142,
     7,     4,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  142);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  143,
     7,     5,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  143);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  144,
     7,     6,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  144);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  145,
     7,     7,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  145);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  146,
     7,     8,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  146);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  147,
     7,     9,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  147);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  148,
     7,    10,     7,    19
);
insert into ASMP_data2tag values ('ASMP-07',  148);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  149,
     7,     1,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  149);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  150,
     7,     2,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  150);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  151,
     7,     3,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  151);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  152,
     7,     4,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  152);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  153,
     7,     5,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  153);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  154,
     7,     6,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  154);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  155,
     7,     7,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  155);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  156,
     7,     8,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  156);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  157,
     7,     9,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  157);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  158,
     7,    10,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  158);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  159,
     7,    11,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  159);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  160,
     7,    12,     7,    21
);
insert into ASMP_data2tag values ('ASMP-07',  160);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  161,
     7,     1,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  161);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  162,
     7,     2,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  162);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  163,
     7,     3,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  163);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  164,
     7,     4,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  164);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  165,
     7,     5,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  165);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  166,
     7,     6,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  166);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  167,
     7,     7,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  167);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  168,
     7,     8,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  168);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  169,
     7,     9,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  169);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  170,
     7,    10,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  170);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  171,
     7,    11,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  171);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  172,
     7,    12,     7,    22
);
insert into ASMP_data2tag values ('ASMP-07',  172);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  173,
     7,     1,     1,    42
);
insert into ASMP_data2tag values ('ASMP-07',  173);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  174,
     7,     1,     4,    43
);
insert into ASMP_data2tag values ('ASMP-07',  174);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  175,
     7,     1,     1,    44
);
insert into ASMP_data2tag values ('ASMP-07',  175);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  176,
     7,     1,     5,    45
);
insert into ASMP_data2tag values ('ASMP-07',  176);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  177,
     7,     1,     1,    46
);
insert into ASMP_data2tag values ('ASMP-07',  177);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  178,
     7,     1,     5,    47
);
insert into ASMP_data2tag values ('ASMP-07',  178);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  179,
     7,     1,     1,    48
);
insert into ASMP_data2tag values ('ASMP-07',  179);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  180,
     7,     2,     1,    48
);
insert into ASMP_data2tag values ('ASMP-07',  180);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  181,
     7,     1,     1,    49
);
insert into ASMP_data2tag values ('ASMP-07',  181);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  182,
     7,     2,     1,    49
);
insert into ASMP_data2tag values ('ASMP-07',  182);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  183,
     7,     1,     7,    50
);
insert into ASMP_data2tag values ('ASMP-07',  183);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  184,
     7,     2,     7,    50
);
insert into ASMP_data2tag values ('ASMP-07',  184);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  185,
     7,     3,     7,    50
);
insert into ASMP_data2tag values ('ASMP-07',  185);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  186,
     7,     4,     7,    50
);
insert into ASMP_data2tag values ('ASMP-07',  186);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  187,
     7,     1,     2,    51
);
insert into ASMP_data2tag values ('ASMP-07',  187);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  188,
     7,     1,     2,    52
);
insert into ASMP_data2tag values ('ASMP-07',  188);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  189,
     7,     1,     7,    53
);
insert into ASMP_data2tag values ('ASMP-07',  189);
insert into ASMP_data (
ASMP_data_id,
VERS,
INDX,
N,
JTYP
) values (  190,
     7,     2,     7,    53
);
insert into ASMP_data2tag values ('ASMP-07',  190);

comment on column ASMP_data.ASMP_data_id is 'Unique identifier';
comment on column ASMP_data.VERS       is 'VERSION                                         ';
comment on column ASMP_data.INDX       is 'STATION NUMBER (INSIDE TYPE)                    ';
comment on column ASMP_data.N          is 'NUMBER OF ELEMENTS                              ';
comment on column ASMP_data.JTYP       is 'AMDB STATION TYPE                               ';
