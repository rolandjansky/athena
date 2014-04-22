--
-- Excerpt from Tom Kyte's login
--
set termout off

-- default editor is vi
define _editor=vi

set serveroutput on size 1000000 format wrapped

column table_name format a30
column owner format a24

set trimspool on

set linesize 136

set pagesize 9999

-- Set the prompt to the machine logged
define gname=idle
column global_name new_value gname
select lower(user) || '@' || substr(global_name, 1, 
  decode(dot,0,length(global_name),dot-1)) global_name 
from (select global_name, instr(global_name,'.') dot from global_name);
set sqlprompt '&gname> '

set termout on
