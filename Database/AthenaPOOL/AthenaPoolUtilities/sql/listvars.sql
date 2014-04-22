set termout off
set heading off
set feedback off
set echo off
set pagesize 0

column collection_name format a16
column variable_name format a16
column variable_type format a16
column variable_position 9999

spool vars.txt

select variable_position, variable_name, variable_type 
  from pool_collections_desc where collection_name='rome_all_tags'
  order by variable_position;

-- select variable_name from pool_collections_desc 
--  where collection_name='rome_3017_merge_A7_w4jet_AOD_tags';

spool off
