\page MC15JobOptions JobOptions for MC15

\brief List of Herwig 7 JobOptions for MC15


\tableofcontents


\section SecJobOptionsTop JobOptions for Top Quark Pair and Single Top Production (DSIDs 410228 - 410239)

    cd Generators/MC15JobOptions/trunk/share
    find -name '*H7UE*' -and -path '*DSID410xxx*' | grep -v '.svn' | sort
    for JO in `find -name '*H7UE*' -and -path '*DSID410xxx*' | grep -v '.svn' | sort`; do echo -e "\n"$JO":"; cat $JO | grep evgenConfig.contact; done

JobOptions                                                                        | Contact
--------------------------------------------------------------------------------- | ---------------------------------------------------
`DSID410xxx/MC15.410228.PowhegHerwig7EvtGen_H7UE_singletop_tchan_lept_top.py`     | dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410229.PowhegHerwig7EvtGen_H7UE_singletop_tchan_lept_antitop.py` | dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410230.aMcAtNloHerwig7EvtGen_H7UE_singletop_tchan_lept.py`       | dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410232.PowhegHerwig7EvtGen_H7UE_tt_h172p5_nonallhad.py`          | dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410233.aMcAtNloHerwig7EvtGen_H7UE_ttbar_nonallhad.py`            | maria.moreno.llacer@cern.ch, dominic@hirschbuehl.de
`DSID410xxx/MC15.410237.PowhegHerwig7EvtGen_H7UE_Wt_dilepton_top.py`              | cescobar@cern.ch, dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410238.PowhegHerwig7EvtGen_H7UE_Wt_dilepton_antitop.py`          | cescobar@cern.ch, dominic.hirschbuehl@cern.ch
`DSID410xxx/MC15.410239.aMcAtNloHerwig7EvtGen_H7UE_MMHT14_CT10ME_Wt_dilepton.py`  | ???


\section SecJobOptionsValidation JobOptions for Herwig 7 Validation (DSIDs 429300 - 429315)

    cd Generators/MC15JobOptions/trunk/share
    find -name '*H7UE*' -and -path '*DSID429xxx*' | grep -v '.svn' | sort
    for JO in `find -name '*H7UE*' -and -path '*DSID429xxx*' | grep -v '.svn' | sort`; do echo -e "\n"$JO":"; cat $JO | grep evgenConfig.contact; done

JobOptions                                                                         | Contact
---------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------
`DSID429xxx/MC15.429300.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_WZlvqq_mqq20.py`      | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429301.Herwig7EvtGen_H7UE_MMHT2014lo_jetjet_JZ7W.py`              | Haifeng.Li@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429302.aMcAtNloH7EG_CT10_H7UE_MMHT2014lo_singletop_tchan_lept.py` | dominic.hirschbuehl@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429303.aMcAtNloH7EG_CT10_H7UE_MMHT2014lo_ttbar_nonallhad.py`      | maria.moreno.llacer@cern.ch
`DSID429xxx/MC15.429304.PhH7EG_H7UE_MMHT2014_CT10ME_ttbar_hdamp172p5_nonallhad.py` | james.robinson@cern.ch, onofrio@liverpool.ac.uk, andrea.helen.knue@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429305.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_WZqqvv.py`            | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429306.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_ZZqqll_mqq20mll20.py` | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429307.Herwig7_H7UE_MMHT2014lo_MinBias.py`                        | orel.gueta@cern.ch
`DSID429xxx/MC15.429308.PhH7EG_H7UE_MMHT2014lo68cl_CT10ME_WlnuWlnu.py`             | james.robinson@cern.ch, christian.johnson@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429309.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_WWlvqq.py`            | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429310.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_WZqqll_mll20.py`      | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429311.PowhegHerwig7EG_H7UE_MMHT2014_CT10ME_ZZvvqq_mqq20.py`      | james.robinson@cern.ch, christian.johnson@cern.ch, carlo.enrico.pandini@cern.ch, paolo.francavilla@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429312.Herwig7EvtGen_H7UE_MMHT2014lo_jetjet_JZ2W.py`              | Haifeng.Li@cern.ch, orel.gueta@cern.ch
`DSID429xxx/MC15.429313.Herwig7_H7UE_MMHT2014lo68cl_Zjets.py`                      | Orel Gueta
`DSID429xxx/MC15.429315.Herwig7_H7UE_MMHT2014lo68cl_Wjets.py`                      | Orel Gueta