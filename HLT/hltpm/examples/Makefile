# Created by Andre Anjos <andre.dos.anjos@cern.ch> 
# Tue 19 Feb 2008 02:52:33 PM CET 

all:
	hltpm_part_l2.py -F l2efopt.py
	hltpm_part_l2eb.py -F l2efopt.py
	hltpm_part_l2ef.py -F l2efopt.py
	hltpm_part_ef.py -F l2efopt.py
	hltpm_seg_ebef.py -F l2efopt.py
	hltpm_seg_l2.py -F l2efopt.py

clean:
	rm -f part*.data.xml L2*.data.xml EBEF*.data.xml
	rm -f *.pyc *~
