
====================================================
=  Reading and plotting LB and L1CT COOL data
= 
=  October 20, 2009 -- Feb 2010 L1CT crew
=
=  Contacts: 
=       Johan.Lundberg@cern.ch, David.Berge@cern.ch
====================================================

setup script for point 1:
-------------------------
    setup_l1ct_python.sh    
    
    source it, or just set up an offline release (required for Cool etc)
    with . offline_setup.sh    

Generic
-------

    l1ct_lb2time.py    (map lumiblock->time and time->lb)
    busy_cool_reader.py (busy display)

Reading l1 rates
----------------

    cool_read_lvl1rates.py
    fillLVL1histograms.py

Reading and plotting Counter Rates as function of run,lb,counter,bcid,...
-------------------------------------------------------------------------

    ctpin_cool_reader.py
    ctpin_cool_plot.py

    ctpmon_cool_reader.py
    ctpmon_cool_plot.py

    busy_cool_reader.py
    busy_cool_plot.py     

Reading configuration
---------------------

  Ctpin Signal configuration:

    ctpin_readconfig.py

  Examine Threshold/Counter configuration (CTPIN and CTPMON)

    l1ct_read_counter_conf.py


helper modules:
---------------
    l1ctpy.py      (common)
    ctpin_py.py    (ctpin)
    ctpmon_py.py   (ctpmon)
    busy_py.py     (busy)

