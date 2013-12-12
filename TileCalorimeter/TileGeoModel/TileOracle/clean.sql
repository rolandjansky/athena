SET echo OFF;


drop table CUTS_DATA2TAG;
drop table CUTS_DATA;

/*
drop table SCNT_DATA2TAG;
drop table SCNT_DATA;
*/
/*
  drop table TILB_DATA2TAG; 
  drop table TILB_DATA;
*/
/*
  drop table TileModule_DATA2TAG; 
  drop table TileModule_DATA; 
*/

purge user_recyclebin;
commit;
select * from tab;
select * from user_ts_quotas;
