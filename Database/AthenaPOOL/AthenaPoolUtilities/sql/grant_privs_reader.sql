--
-- create private synonyms within acct with read privilege on atlas_Tags_rome
-- J. Cranshaw 08.07.05
--
set pagesize 0;
set heading off;
set feedback off;
set verify off;
set echo off;
set serveroutput on;

spool reader.syn;

declare
  cursor c1 is select table_name from all_tables where owner = 'ATLAS_TAGS_ROME';
begin
  dbms_output.enable(100000);
  for i in c1 loop
    dbms_output.put_line('CREATE SYNONYM '||i.table_name||' FOR ATLAS_TAGS_ROME.'||i.table_name||';');
  end loop;
end;
/
