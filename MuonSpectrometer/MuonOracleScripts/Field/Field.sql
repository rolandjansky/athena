PROMPT test.sql
SET echo OFF;
set linesize 132;
set serverout on;
execute DBMS_OUTPUT.ENABLE (5000000);

drop   table MFLD_data cascade constraints;
select * from user_ts_quotas;
create table MFLD_data (
  MFLD_data_id      number(10),
  VERS              number(10),
  VNAME             varchar2(24),
  LENMFLD           number(10),
  NLINE             number(10),
  data CLOB default empty_clob()
) tablespace data02;

alter table MFLD_data add constraint MFLD_data_pk
primary key (MFLD_data_id)
using index tablespace indx02;

insert into MFLD_data (
  MFLD_data_id,
  VERS,
  VNAME,
  LENMFLD,
  NLINE,
  data
) values (0, 0, 'bmagatlas03.data', 10, 20, empty_clob() );
select * from user_ts_quotas;

DECLARE
  lobloc  CLOB;
  req     utl_http.req;
  resp    utl_http.resp;

  text    VARCHAR2(32767);
  amount  INTEGER(10) := 0;
  offset  INTEGER(10) := 0;
BEGIN
  SELECT data INTO lobloc
  FROM   MFLD_data
  WHERE  MFLD_data_id = 0 FOR UPDATE;
  offset := DBMS_LOB.GETLENGTH(lobloc)+2;
     req := utl_http.begin_request(
     'http://baranov.home.cern.ch/baranov/sql/bmagatlas03.data');
  resp := utl_http.get_response(req);
  LOOP
     text := ' ';
     UTL_HTTP.READ_TEXT(resp, text, NULL);
/*   DBMS_OUTPUT.PUT_LINE(text); */
     amount := length(text);
     DBMS_LOB.WRITEAPPEND(lobloc,amount,text);
/*   offset :=DBMS_LOB.GETLENGTH(lobloc);
     DBMS_OUTPUT.PUT_LINE('Length=' || to_char(offset));
*/
  END LOOP;
     utl_http.end_response(resp);
  EXCEPTION
    WHEN utl_http.end_of_body
    THEN utl_http.end_response(resp);
END;
/
select * from user_ts_quotas;
/* select * from tab; */
