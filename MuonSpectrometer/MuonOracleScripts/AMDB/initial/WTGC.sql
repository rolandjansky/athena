SET echo OFF;
set linesize 132;
drop   table WTGC_data2tag cascade constraints;
drop   table WTGC_data cascade constraints;
create table WTGC_data (
  WTGC_data_id   number(10),
VERS       number(10),
JSTA       number(10),
NBEVOL       number(10),
X0       float(63),
WIDCHB       float(63),
FWIRCH       float(63),
FWIXCH       float(63),
ALLNAME_0       varchar2(4),
ALLNAME_1       varchar2(4),
ALLNAME_2       varchar2(4),
ALLNAME_3       varchar2(4),
ALLNAME_4       varchar2(4),
ALLNAME_5       varchar2(4),
ALLNAME_6       varchar2(4),
ALLNAME_7       varchar2(4),
ALLNAME_8       varchar2(4)
) ;

alter table WTGC_data add constraint WTGC_data_pk
primary key (WTGC_data_id);

create table WTGC_data2tag (
  WTGC_vers      varchar2(255),
  WTGC_data_id   number(10)
) ;

alter table WTGC_data2tag add constraint WTGC_data2tag_pk
primary key (WTGC_vers, WTGC_data_id);
alter table WTGC_data2tag add constraint WTGC_data2tag_vers_fk
foreign key (WTGC_vers) references hvs_nodevers (vers_name);
alter table WTGC_data2tag add constraint WTGC_data2tag_data_fk
foreign key (WTGC_data_id) references WTGC_data (WTGC_data_id);

delete from hvs_relation where CHILD_NODENAME = 'WTGC';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'WTGC';
delete from hvs_node     where NODE_NAME = 'WTGC';

insert into hvs_node     values ('WTGC','MuonSpectrometer', '');
insert into hvs_nodevers values ('WTGC', 'WTGC-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','WTGC','WTGC-07');
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    0,
     7,     1,    15,     0.1300,     7.0000,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    0);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    1,
     7,     2,    15,     0.1300,     7.0000,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    1);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    2,
     7,     3,    15,     0.1300,     7.0000,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    2);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    3,
     7,     4,    15,     0.1300,     7.0000,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    3);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    4,
     7,     5,    15,     0.1300,     7.0000,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    4);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    5,
     7,     6,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    5);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    6,
     7,     7,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    6);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    7,
     7,     8,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    7);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    8,
     7,     9,    11,     0.1300,     4.3700,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    8);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (    9,
     7,    10,    11,     0.1300,     4.3700,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',    9);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   10,
     7,    11,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   10);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   11,
     7,    12,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   11);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   12,
     7,    13,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   12);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   13,
     7,    14,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   13);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   14,
     7,    15,    11,     0.1300,     4.3700,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   14);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   15,
     7,    16,    11,     0.1300,     4.3700,     1.7000,     2.0000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   15);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   16,
     7,    17,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   16);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   17,
     7,    18,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   17);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   18,
     7,    19,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   18);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   19,
     7,    20,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   19);
insert into WTGC_data (
WTGC_data_id,
VERS,
JSTA,
NBEVOL,
X0,
WIDCHB,
FWIRCH,
FWIXCH,
ALLNAME_0,ALLNAME_1,ALLNAME_2,ALLNAME_3,ALLNAME_4,ALLNAME_5,ALLNAME_6,ALLNAME_7,
ALLNAME_8
) values (   20,
     7,    21,    11,     0.1300,     4.3700,     2.0000,     2.4000,
'Gas','G10','Hone','Copp','Carb','','','',''
);
insert into WTGC_data2tag values ('WTGC-07',   20);

comment on column WTGC_data.WTGC_data_id is 'Unique identifier';
comment on column WTGC_data.VERS       is 'VERSION                                         ';
comment on column WTGC_data.JSTA       is 'JSTA INDEX                                      ';
comment on column WTGC_data.NBEVOL     is 'NUMBER OF DETAILS                               ';
comment on column WTGC_data.X0         is 'X0                                              ';
comment on column WTGC_data.WIDCHB     is 'WIDTH OF THE CHBER ALONG Z                      ';
comment on column WTGC_data.FWIRCH     is 'FRAME WIDTH IN R                                ';
comment on column WTGC_data.FWIXCH     is 'FRAME WIDTH IN X                                ';
comment on column WTGC_data.ALLNAME_0  is 'MATERIAL                                        ';
