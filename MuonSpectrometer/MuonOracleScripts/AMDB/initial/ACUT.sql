SET echo OFF;
set linesize 132;
drop   table ACUT_data2tag cascade constraints;
drop   table ACUT_data cascade constraints;
create table ACUT_data (
  ACUT_data_id   number(10),
VERS       number(10),
I       number(10),
ICUT       number(10),
N       number(10)
) ;

alter table ACUT_data add constraint ACUT_data_pk
primary key (ACUT_data_id);

create table ACUT_data2tag (
  ACUT_vers      varchar2(255),
  ACUT_data_id   number(10)
) ;

alter table ACUT_data2tag add constraint ACUT_data2tag_pk
primary key (ACUT_vers, ACUT_data_id);
alter table ACUT_data2tag add constraint ACUT_data2tag_vers_fk
foreign key (ACUT_vers) references hvs_nodevers (vers_name);
alter table ACUT_data2tag add constraint ACUT_data2tag_data_fk
foreign key (ACUT_data_id) references ACUT_data (ACUT_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ACUT';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'ACUT';
delete from hvs_node     where NODE_NAME = 'ACUT';

insert into hvs_node     values ('ACUT','MuonSpectrometer', '');
insert into hvs_nodevers values ('ACUT', 'ACUT-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','ACUT','ACUT-07');
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    0,
     7,     2,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    0);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    1,
     7,     3,     1,     6
);
insert into ACUT_data2tag values ('ACUT-07',    1);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    2,
     7,     4,     1,     2
);
insert into ACUT_data2tag values ('ACUT-07',    2);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    3,
     7,     5,     1,     8
);
insert into ACUT_data2tag values ('ACUT-07',    3);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    4,
     7,     6,     1,     8
);
insert into ACUT_data2tag values ('ACUT-07',    4);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    5,
     7,     1,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    5);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    6,
     7,     1,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    6);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    7,
     7,     2,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    7);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    8,
     7,     4,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    8);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (    9,
     7,     5,     1,     4
);
insert into ACUT_data2tag values ('ACUT-07',    9);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   10,
     7,     1,     1,     2
);
insert into ACUT_data2tag values ('ACUT-07',   10);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   11,
     7,     3,     1,     2
);
insert into ACUT_data2tag values ('ACUT-07',   11);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   12,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   12);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   13,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   13);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   14,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   14);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   15,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   15);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   16,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   16);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   17,
     7,     1,     1,     1
);
insert into ACUT_data2tag values ('ACUT-07',   17);
insert into ACUT_data (
ACUT_data_id,
VERS,
I,
ICUT,
N
) values (   18,
     7,     1,     1,     2
);
insert into ACUT_data2tag values ('ACUT-07',   18);

comment on column ACUT_data.ACUT_data_id is 'Unique identifier';
comment on column ACUT_data.VERS       is 'VERSION                                         ';
comment on column ACUT_data.I          is 'INDEX                                           ';
comment on column ACUT_data.ICUT       is 'CUT-OUT INDEX                                   ';
comment on column ACUT_data.N          is 'NUMBER OF SUB-CUTS IN THE STATION               ';
