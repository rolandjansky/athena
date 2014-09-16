SET echo OFF;
set linesize 132;
select * from user_ts_quotas;

@GENV.sql; 
@ABRT.sql; 
@AECT.sql; 
@JDSH.sql;
@JFSH.sql;
@JTSH.sql;
@FEET.sql;
@RAIL.sql;
@ECST.sql; 
@IPP2.sql;
@MBAP.sql;

commit;
select * from tab;
select * from user_ts_quotas;

