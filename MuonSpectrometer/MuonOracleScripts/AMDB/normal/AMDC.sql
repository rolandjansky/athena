PROMPT 07_R.01_01/AMDC.sql
SET echo OFF;
set linesize 132;
drop   table AMDC_data2tag cascade constraints;
drop   table AMDC_data cascade constraints;
create table AMDC_data (
  AMDC_data_id   number(10),
VERS       number(10),
VNAME       varchar2(4),
LENAMDC       number(10),
NLINE       number(10),
data CLOB default empty_clob()
) ;

alter table AMDC_data add constraint AMDC_data_pk
primary key (AMDC_data_id);

create table AMDC_data2tag (
  AMDC_vers      varchar2(255),
  AMDC_data_id   number(10)
) ;

alter table AMDC_data2tag add constraint AMDC_data2tag_pk
primary key (AMDC_vers, AMDC_data_id);
alter table AMDC_data2tag add constraint AMDC_data2tag_vers_fk
foreign key (AMDC_vers) references hvs_nodevers (vers_name);
alter table AMDC_data2tag add constraint AMDC_data2tag_data_fk
foreign key (AMDC_data_id) references AMDC_data (AMDC_data_id);

delete from hvs_relation where CHILD_NODENAME = 'AMDC';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';
delete from hvs_nodevers where NODE_NAME = 'AMDC';
delete from hvs_node     where NODE_NAME = 'AMDC';

insert into hvs_node     values ('AMDC','MuonSpectrometer', '');
insert into hvs_nodevers values ('AMDC', 'AMDC-07', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','AMDC','AMDC-07');
insert into AMDC_data (
AMDC_data_id,
VERS,
VNAME,
LENAMDC,
NLINE,
data
) values (    0,
     7,'R.01',522074,130519,
empty_clob()
);
insert into AMDC_data2tag values ('AMDC-07',    0);
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
  FROM   AMDC_data
  WHERE  AMDC_data_id = 0 FOR UPDATE;
  offset := DBMS_LOB.GETLENGTH(lobloc)+2;
     req := utl_http.begin_request(
     'http://baranov.home.cern.ch/baranov/sql/07_R.01_01.Light_Egg_Rndm/AMDCtemp.data');
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

comment on column AMDC_data.AMDC_data_id is 'Unique identifier';
comment on column AMDC_data.VERS       is 'VERSION                                         ';
comment on column AMDC_data.VNAME      is 'NAME                                            ';
comment on column AMDC_data.LENAMDC    is 'STRING LENGTH                                   ';
comment on column AMDC_data.NLINE      is 'CHAR4 NUMBER                                    ';
comment on column AMDC_data.DATA       is '(NLINE)-ASCII                                   ';
