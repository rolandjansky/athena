SET echo OFF;

drop table ABRT_DATA2TAG; 
drop table ABRT_DATA;
drop table AECT_DATA2TAG; 
drop table AECT_DATA;
drop table GENV_DATA2TAG; 
drop table GENV_DATA;
drop table JDSH_DATA2TAG; 
drop table JDSH_DATA;
drop table JFSH_DATA2TAG; 
drop table JFSH_DATA;
drop table JTSH_DATA2TAG; 
drop table JTSH_DATA;
drop table FEET_DATA2TAG; 
drop table FEET_DATA;
drop table RAIL_DATA2TAG; 
drop table RAIL_DATA;
drop table ECST_DATA2TAG;
drop table ECST_DATA;
drop table IPP2_DATA2TAG;
drop table IPP2_DATA;
drop table MBAP_DATA2TAG;
drop table MBAP_DATA;

commit;
select * from tab;
select * from user_ts_quotas;
