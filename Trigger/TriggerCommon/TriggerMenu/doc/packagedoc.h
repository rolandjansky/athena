/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**



@page TriggerMenu_page 

@section TriggerMenu_TrigerMenuPythonIntro Introduction

This package contains only python code which is used to dynamically
generate XML configuration of trigger which are used by HLT::TrigSteer to schedule HLT selection.

There are 3 types of files here:<br/>
1) flags - which are diffing possible in generation process switches i.e. python::EgammaSliceFlags.py<br/>
    This flags contain set of common switches defined in python::CommonSliceFlags.py and set of functions 
    to operate on them defined in CommonSliceHelper.py<br/>
2) generation classes - which is python code specialized for generation of HLT chains and sequences definition<br/> 
    The helper objects for it are defined in TrigConfigSvc in python::TriggerPythonConfig.py module.
    There are 2 types of generation: simple chains i.e. python::Electron.py and chains composition i.e. 
    python::MultiElectron.py<br/>
3) generic script for generation - python::GenerateManu.py which drives all the generation. <br/>
   It is used by Trigger Getter modules defined in TriggerJobOpts package.
 


*/
