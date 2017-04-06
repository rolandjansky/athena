get_files -xmls LVL1config_Physics_pp_v7.xml
get_files -data caloRoIData.dat
source RecExCommon_links.sh
athena.py --threads=1 ViewAlgsTest/BasicMTTrackingTrigger.py
