# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Define a function for making an SLHA file from a mass spectrum
# The decay modes are added via SUSY-HIT
# Lots of magic file names and locations (not my fault!)

import os,shutil,subprocess
from AthenaCommon import Logging

def makeSLHAFromSpectrum(spectrum_in='slhaspectrum.in',slha_out='susyhit_slha.out'):
    """ Function to make full SLHA file with decays from param card with only masses
    and model parameters.  SUSY-HIT does the work.  Tries to be as flexible as possible.
    Input parameter spectrum_in: input mass spectrum file; if not specified, use slhaspectrum.in
    Input parameter slha_out: output SLHA file; if not specified, use susyhit_slha.out
    Relies on cvmfs access!
    """
    # Type checking for input parameters
    if type(spectrum_in) is not str:
        raise RuntimeError('Input spectrum file name is not a string: '+str(spectrum_in))
    if type(slha_out) is not str:
        raise RuntimeError('Output SLHA file name is not a string: '+str(slha_out))

    # Make a logger
    mylog = Logging.logging.getLogger('makeSLHAFromSpectrum')

    # Check if there's an output file where we are supposed to leave on
    if os.access(slha_out,os.R_OK) and slha_out!='susyhit_slha.out':
        mylog.warning('Output file '+slha_out+' already exists. Will remove the old one.')

    had_old_input=False
    if spectrum_in!='slhaspectrum.in':
        # Non-default -- does it exist, or do we need to go get it?
        if not os.access(spectrum_in,os.R_OK):
            get_spectrum = subprocess.Popen(['get_files','-data',spectrum_in])
            get_spectrum.wait()
            if not os.access(spectrum_in,os.R_OK):
                raise RuntimeError('Could not locate input spectrum file: '+spectrum_in)
        # Now we need to move it into place
        # Move any old input files out of the way
        if os.access('slhaspectrum.in',os.R_OK):
            had_old_input=True
            shutil.move('slhaspectrum.in','slhaspectrum.in_'+str(os.getpid()))
        # Move our file into place
        shutil.move(spectrum_in,'slhaspectrum.in')

    # Check if there's an output file where we want to leave one
    had_old_output=False
    if os.access('susyhit_slha.out',os.R_OK):
        new_spot='susyhit_slha.out_'+str(os.getpid())
        if slha_out=='susyhit_slha.out':
            mylog.warning('Moving old susyhit_slha.out to '+new_spot)
            shutil.move('susyhit_slha.out',new_spot)
        else:
            had_old_output=True
            shutil.move('susyhit_slha.out',new_spot)

    # Write the input card -- just magic format
    if os.access('susyhit.in',os.R_OK):
        mylog.warning('Found susyhit.in file in the current directory.  Assuming you know what you are doing and will not overwrite it.')
    else:
        input_card_f = open('susyhit.in','w')
        input_card_f.write("""* SUspect-SdecaY-Hdecay-InTerface options:
------------------------------------------
  (1) link SuSpect-Sdecay-Hdecay and take the input parameters from SuSpect
  (2) link Sdecay-Hdecay, not SuSpect, take the input parameters from any SLHA
      input, called slhaspectrum.in or slhaspectrumFV.in in the FV case
2
* Choice of the output, SLHA format (1) or simple (0):
------------------------------------------------------
1
* HDECAY input parameters:
--------------------------
MSBAR(1) = 0.190D0
MC       = 1.40D0
MMUON    = 0.105658389D0
1/ALPHA  = 137.0359895D0
GAMW     = 2.080D0
GAMZ     = 2.490D0
VUS      = 0.2205D0
VCB      = 0.04D0
VUB/VCB  = 0.08D0
* FCNC stop decays for SDECAY:
----------------------------------------
  (0) No flavour violation at tree-level.
  (1) FCNC stop decays in SDECAY. Works only with output in SLHA format, and input parameters have to be provided in an SLHA2 file.
0
  (0) Light stop four-body decay widths summed up.
  (1) Stop four-body decay separated by final states.
1""")
        input_card_f.close()

    # Now run the actual generator
    application_spot = '/cvmfs/atlas.cern.ch/repo/sw/Generators/madgraph/models/latest/SUSY-HIT/x86_64-slc6-gcc47-opt/susyhit_run'
    if not os.access(application_spot,os.X_OK):
        raise RuntimeError('Cannot access application for execution: '+application_spot)
    run_spectrum = subprocess.Popen([application_spot])
    run_spectrum.wait()

    # Should be done!
    # Move output to requested spot
    if slha_out!='susyhit_slha.out':
        if os.access(slha_out,os.R_OK):
            os.remove(slha_out)
        shutil.move('susyhit_slha.out',slha_out)

    # Move input back if needed.
    if had_old_input:
        os.remove('slhaspectrum.in')
        shutil.move('slhaspectrum.in_'+str(os.getpid()),'slhaspectrum.in')

    # Move output back if needed
    if had_old_output:
        shutil.move('susyhit_slha.out_'+str(os.getpid()),'susyhit_slha.out')
