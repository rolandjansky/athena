AtlCoolCopy.exe "sqlite://schema=mycool.db;dbname=CONDBR2" "sqlite://schema=tmp.db;dbname=CONDBR2" -c -a -f /Indet/AlignL3 -nrls 296939 0 -nrlu 301912 0 -outtag IndetAlignL3-Repro-Rel21-UPD3-00 -tl "For reprocessing for Rel21 in November 2016. Last Update 2016-11-06 07:40 using the 2016 pre-TS1 alignment sample."
AtlCoolCopy.exe "sqlite://schema=mycool.db;dbname=CONDBR2" "sqlite://schema=tmp.db;dbname=CONDBR2" -c -a -f /TRT/AlignL2 -nrls 296939 0 -nrlu 301912 0 -outtag TRTAlignL2-Repro-Rel21-UPD3-00 -tl "For reprocessing for Rel21 in November 2016. Last Update 2016-11-06 07:40 using the 2016 pre-TS1 alignment sample."

/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment "For reprocessing for Rel21 in November 2016. Last Update 2016-11-06 07:40 using the 2016 pre-TS1 alignment sample." --tag IndetAlignL3-Repro-Rel21-UPD3-00 --passopt='-cti' --folder /Indet/AlignL3 tmp.db CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_INDET_W WCOOLOFL4INDET1

/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --debug --batch --nobackup --comment "For reprocessing for Rel21 in November 2016. Last Update 2016-11-06 07:40 using the 2016 pre-TS1 alignment sample." --tag TRTAlignL2-Repro-Rel21-UPD3-00 --passopt='-cti' --folder /TRT/AlignL2 tmp.db CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_TRT_W WCOOLOFL4TRT2

