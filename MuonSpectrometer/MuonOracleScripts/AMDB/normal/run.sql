SET echo OFF;
set linesize 132;

@AMDC.sql;
@ASZT.sql;

commit;
select * from tab;
select * from user_ts_quotas;
