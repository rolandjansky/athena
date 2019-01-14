get_files -scripts createLinkMaskingSQLiteFile.py
python createLinkMaskingSQLiteFile.py
athena.py --threads=1 SCT_ConditionsAlgorithms/testLinkMasking.py
