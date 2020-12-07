get_files -scripts createLinkMaskingSQLiteFile.py
python createLinkMaskingSQLiteFile.py
ln -s LinkMasking.db mycool.db # mycool.db is hard coded in Database/IOVDbSvc/python/IOVDbSvcConfig.py
python -m SCT_ConditionsAlgorithms.SCT_LinkMaskingTestAlgConfig
