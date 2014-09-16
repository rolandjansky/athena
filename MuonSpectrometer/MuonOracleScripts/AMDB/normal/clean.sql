SET echo OFF;

drop table AMDC_DATA2TAG; 
drop table AMDC_DATA;
drop table ASZT_DATA2TAG; 
drop table ASZT_DATA;

purge user_recyclebin;

commit;
select * from tab;
select * from user_ts_quotas;
