#!/usr/bin/env sh
get_files -xmls LVL1config_Physics_pp_v7.xml
athena.py --threads=1 ViewAlgsTest/ID_RawDataMT_Trigger.py
