-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@roma3.infn.it
-- DATE: May 08
-- VERSION v1r1

-- USAGE:
-- sqlplus sys/PASSWD @create_user.sql <user_name> <site_name><user_password> <user_default_tablespace> <user_default_temporary_tablespace>

--DROP USER &1;
CREATE USER &1 PROFILE "DEFAULT" IDENTIFIED BY &3 DEFAULT TABLESPACE &4 TEMPORARY TABLESPACE &5 ACCOUNT UNLOCK;
GRANT UNLIMITED TABLESPACE TO &1;
GRANT CONNECT TO &1;
GRANT RESOURCE TO &1;
GRANT CREATE TABLE TO &1;
GRANT CREATE SESSION to &1;
GRANT CREATE VIEW to &1;
