SET echo OFF;
set linesize 132;

@ACUT.sql; 
@ALMN.sql;
@ASMP.sql;
@AMDC.sql;
@ATYP.sql; 
@ALIN.sql;
@APTP.sql;

commit;
select * from tab;
select * from user_ts_quotas;
