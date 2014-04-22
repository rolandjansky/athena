--
-- Generate script which can be run against the database 
--   to grant privileges to roles created in previous 
--   script
-- J. Cranshaw 29.06.05
--
set pagesize 0;
set heading off;
set feedback off;
set verify off;
set echo off;
set serveroutput on;

spool privs_list.grt;

declare
  cursor c1 is select table_name from user_tables;
begin
  dbms_output.enable(100000);
  for i in c1 loop
    dbms_output.put_line('GRANT SELECT ON '||i.table_name||' TO ATLAS_TAGS_ROME_SELECT;');
    dbms_output.put_line('GRANT INSERT ON '||i.table_name||' TO ATLAS_TAGS_ROME_INSERT;');
    dbms_output.put_line('GRANT UPDATE ON '||i.table_name||' TO ATLAS_TAGS_ROME_UPDATE;');
    dbms_output.put_line('GRANT DELETE ON '||i.table_name||' TO ATLAS_TAGS_ROME_DELETE;');
  end loop;
end;
/
