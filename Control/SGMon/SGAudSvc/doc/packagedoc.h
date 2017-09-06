/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SGAudSvc_page SGAudSvc
@author ivukotic@cern.ch

@section SGAudSvc_MyPackageIntro Introduction

This package gives a graphical representation of algorithms accessing StoreGate.
SGAudSvc instruments retrieve and record functions of StoreGate and from there gets name of object accessed. Upon getting an object name it asks AlgContexSvc for a name of current algorithm. At the end of run, ascii file is produced (default name SGAudSvc.out). 
This file is further used by SGout2dot.py to produce .dot file.
To obtain ps file from dot file you may use:'dot -Tps graph1.dot -o graph1.ps'. Still it is nicer to use Graphviz to directly see the plots and if needed edit them by hand. 

Please keep in mind that data are not collected for the first three events.

    
   
*/
