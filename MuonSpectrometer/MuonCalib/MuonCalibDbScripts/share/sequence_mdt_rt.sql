-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@fis.uniroma3.it
-- DATE: Jul 07
-- VERSION v0r2

--USAGE:
--sqlplus <USER_NAME>/<USER_PASSWORD> @sequence_mdt_rt.sql <USER_NAME>

--DROP SEQUENCE &1.."MDT_RT_SEQUENCE";
CREATE SEQUENCE &1.."MDT_RT_SEQUENCE" NOCYCLE NOORDER NOCACHE NOMAXVALUE MINVALUE 1 INCREMENT BY 1 START WITH 1;