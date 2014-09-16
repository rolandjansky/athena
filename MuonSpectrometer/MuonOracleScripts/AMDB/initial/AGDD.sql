PROMPT 07_R.01_01/AGDD.sql
SET echo OFF;
set linesize 132;
drop   table AGDD_data2tag cascade constraints;
drop   table AGDD_data cascade constraints;
create table AGDD_data (
  AGDD_data_id   number(10),
VERS       number(10),
VNAME       varchar2(4),
LENAGDD       number(10),
NLINE       number(10),
data CLOB default empty_clob()
) ;

alter table AGDD_data add constraint AGDD_data_pk
primary key (AGDD_data_id);

create table AGDD_data2tag (
  AGDD_vers      varchar2(255),
  AGDD_data_id   number(10)
) ;

alter table AGDD_data2tag add constraint AGDD_data2tag_pk
primary key (AGDD_vers, AGDD_data_id);
alter table AGDD_data2tag add constraint AGDD_data2tag_vers_fk
foreign key (AGDD_vers) references hvs_nodevers (vers_name);
alter table AGDD_data2tag add constraint AGDD_data2tag_data_fk
foreign key (AGDD_data_id) references AGDD_data (AGDD_data_id);

delete from hvs_relation where CHILD_NODENAME = 'AGDD';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'AGDD';
delete from hvs_node     where NODE_NAME = 'AGDD';

insert into hvs_node     values ('AGDD','MuonSpectrometer', '');
insert into hvs_nodevers values ('AGDD', 'AGDD-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','AGDD','AGDD-07');
insert into AGDD_data (
AGDD_data_id,
VERS,
VNAME,
LENAGDD,
NLINE,
data
) values (    0,
     7,'R.01',207817, 51955,
empty_clob()
);
insert into AGDD_data2tag values ('AGDD-07',    0);
DECLARE
  lobloc  CLOB;
  req     utl_http.req;
  resp    utl_http.resp;
  text    VARCHAR2(32767);
  amount  INTEGER(10) := 0;
  offset  INTEGER(10) := 0;
  TRUE    BOOLEAN;
BEGIN
  SELECT data INTO lobloc
  FROM   AGDD_data
  WHERE  AGDD_data_id = 0 FOR UPDATE;
  offset := DBMS_LOB.GETLENGTH(lobloc)+2;
     req := utl_http.begin_request(
     'http://baranov.home.cern.ch/baranov/sql/07_R.01_01.Light/AGDDtemp.data');
  resp := utl_http.get_response(req);
  LOOP
     text := ' ';
     UTL_HTTP.READ_TEXT(resp, text, NULL);
     /* DBMS_OUTPUT.PUT_LINE(text); */
     amount := length(text);
     DBMS_LOB.WRITEAPPEND(lobloc,amount,text);
  END LOOP;
    utl_http.end_response(resp);
  EXCEPTION
    WHEN utl_http.end_of_body
    THEN utl_http.end_response(resp);
END;
/

comment on column AGDD_data.AGDD_data_id is 'Unique identifier';
comment on column AGDD_data.VERS       is 'VERSION                                         ';
comment on column AGDD_data.VNAME      is 'NAME                                            ';
comment on column AGDD_data.LENAGDD    is 'STRING LENGTH                                   ';
comment on column AGDD_data.NLINE      is 'CHAR4 NUMBER                                    ';
comment on column AGDD_data.DATA       is '(NLINE)-ASCII                                   ';
