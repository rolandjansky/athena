-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@roma3.infn.it
-- DATE: May 08
-- VERSION v1r1

--USAGE: 
--sqlplus sys/password @sequence_mdt_tube.sql <USER_NAME> 

--DROP SEQUENCE &1.."MDT_TUBE_SEQUENCE";
CREATE SEQUENCE &1.."MDT_TUBE_SEQUENCE" NOCYCLE NOORDER NOCACHE NOMAXVALUE MINVALUE 1 INCREMENT BY 1 START WITH 1;
