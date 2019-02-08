get_files -scripts createLinkMaskingSQLiteFile.py
python createLinkMaskingSQLiteFile.py
athena.py --threads=5 SCT_ConditionsAlgorithms/testLinkMasking.py
