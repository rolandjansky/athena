-- PROJECT: ATLAS MDT Calibration DB
-- AUTHOR: Elisabetta.Vilucchi@lnf.infn.it, celio@fis.uniroma3.it
-- DATE: May 08
-- VERSION v1r1

--View to select all fields of mdt_tube table and the rootfile from the matching mdt_head table for the last 10 calibrations (i.e. the 10th max values of MDT_HEAD.HEAD_ID)
--USAGE: 
--sqlplus sys/password @view_allTub_rootFile_last10Cal.sql <USER_NAME> <SITE_NAME>(2 characters like MU,MI,RM...) <USER_PASSOWRD> <TABLESPCE_NAME>


--DROP VIEW &1.."VIEW_ALLTUB_ROOT_LAST10CAL_ORD";
CREATE VIEW &1.."VIEW_ALLTUB_ROOT_LAST10CAL_ORD" 
AS 
select A.*, B.ROOTFILE from
    (select * from &1..mdt_tube where head_id in
                    (select head_id from
                            (select distinct head_id from &1..mdt_head order by head_id desc)
                            where rownum <= 10)) A INNER JOIN &1..mdt_head B
ON A.HEAD_ID = B.HEAD_ID order by seqid;
