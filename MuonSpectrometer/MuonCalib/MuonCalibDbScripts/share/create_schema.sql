-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@roma3.infn.it
-- DATE: May 08
-- VERSION v1r1

-- Script to create all schema objects; usage:
-- sqlplus <USER>/<PASSWORD> @create_schema.sql <USER> <SITE INITIAL> <PASSWORD> <TABLESPACE>

-- Spool file
spool create_schema_spool.log

-- Create all tables 
@sites_table.sql
@mdt_head_table.sql
@mdt_tube_table.sql
@mdt_rt_table.sql
@mdt_rt_map_t_table.sql
@mdt_rt_cheby_table.sql
@mdt_rt_map_r_table.sql
@mdt_rt_map_s_table.sql

-- Create all sequences
@sequence_mdt_head.sql
@sequence_mdt_tube.sql
@sequence_mdt_rt.sql
@sequence_mdt_rt_cheby.sql
@sequence_mdt_rt_map_r.sql
@sequence_mdt_rt_map_s.sql
@sequence_mdt_rt_map_t.sql

-- Create view
@view_allTub_rootFile_last10Cal.sql
@view_allTub_rootFile_last10Cal_ordered.sql

-- Create all triggers
@mdt_head_trigger.sql
@mdt_tube_trigger.sql
@mdt_rt_trigger.sql
@mdt_rt_cheby_trigger.sql
@mdt_rt_map_r_trigger.sql
@mdt_rt_map_s_trigger.sql
@mdt_rt_map_t_trigger.sql

spool off
exit
