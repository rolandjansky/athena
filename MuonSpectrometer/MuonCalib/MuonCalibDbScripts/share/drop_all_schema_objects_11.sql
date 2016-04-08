-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@roma3.infn.it
-- DATE: May 08
-- VERSION v1r1

-- Script to drop all schema objects; usage:
-- sqlplus <USER>/<PASSWORD> @drop_all_schema_objects.sql <USER> 

-- Spool file
spool drop_spool.log

-- Drop views
DROP VIEW &1..VIEW_ALLTUB_ROOTFILE_LAST10CAL;
DROP VIEW &1..VIEW_ALLTUB_ROOT_LAST10CAL_ORD;

-- Drop all Triggers 
DROP TRIGGER &1..mdt_head_trigger;
DROP TRIGGER &1..mdt_tube_trigger;
DROP TRIGGER &1..mdt_rt_cheby_trigger;
DROP TRIGGER &1..mdt_rt_map_r_trigger;
DROP TRIGGER &1..mdt_rt_map_s_trigger;
DROP TRIGGER &1..mdt_rt_map_t_trigger;
DROP TRIGGER &1..mdt_rt_trigger;

-- Drop all tables 
DROP TABLE &1..MDT_TUBE;
DROP TABLE &1..MDT_RT_CHEBY;
DROP TABLE &1..MDT_RT_MAP_R;
DROP TABLE &1..MDT_RT_MAP_S;
DROP TABLE &1..MDT_RT_MAP_T;
DROP TABLE &1..MDT_RT;
DROP TABLE &1..MDT_HEAD;
DROP TABLE &1..SITES;

-- Drop all sequences
DROP SEQUENCE &1..MDT_HEAD_SEQUENCE;
DROP SEQUENCE &1..MDT_TUBE_SEQUENCE;
DROP SEQUENCE &1..MDT_RT_SEQUENCE;
DROP SEQUENCE &1..MDT_RT_CHEBY_SEQUENCE;
DROP SEQUENCE &1..MDT_RT_MAP_R_SEQUENCE;
DROP SEQUENCE &1..MDT_RT_MAP_S_SEQUENCE;
DROP SEQUENCE &1..MDT_RT_MAP_T_SEQUENCE;

spool off
exit
