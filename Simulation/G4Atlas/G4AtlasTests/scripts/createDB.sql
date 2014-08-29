drop table t_files;
drop table t_functions;
drop table t_timer;
drop table t_run;
drop table t_counts;
drop table t_links;


CREATE TABLE t_counts (rid integer, fid integer, selfcounts integer, totalcounts integer, primary key (rid,fid) );
CREATE TABLE t_files (id integer primary key autoincrement, fl_name text);
CREATE TABLE t_functions (id integer primary key autoincrement, fn_name text, fid integer, issue integer, lib text);
CREATE TABLE t_hosts (id integer primary key autoincrement, hostname varchar(32), os_type varchar(32), model varchar(32), cpuf float, ncpus integer , maxmem varchar(32), maxswp varchar(32), server varchar(6), resources text);
CREATE TABLE t_links (id integer primary key autoincrement, rid integer, fid integer, cfunid integer, counts integer);
CREATE TABLE t_run (id integer primary key autoincrement, branch text, rel text, cmt text, rdate date, rtime time, machine text, KSI2K_fac float, package text, job text);
CREATE TABLE t_timer (id integer primary key autoincrement, rid integer, seconds float, timer_type text, timer_scope text);
CREATE INDEX idx_functions_name_fid_issue on t_functions (fn_name, fid, issue);


/* TODO: add references */