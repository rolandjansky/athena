# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Pythonised helper functions for usage in top-level JobOptions
#    written by Zach Marshall <zach.marshall@cern.ch>
#    updates by Christian Gutschow <chris.g@cern.ch>

import os,time,subprocess,shutil,re
from AthenaCommon import Logging
mglog = Logging.logging.getLogger('MCJobOptionUtils')


def remap_lhe_pdgids(lhe_file_old,lhe_file_new=None,pdgid_map={},delete_old_lhe=True):
    """Update the PDG IDs used in an LHE file. This is a bit finicky, as we have to
    both touch the LHE file metadata _and_ modify the events themselves. But since this
    is "just" a remapping, it should be safe assuming Pythia8 is told the correct thing
    afterwards and can get the showering right."""
    # If we want to just use a temp file, then put in a little temp holder
    lhe_file_new_tmp = lhe_file_new if lhe_file_new is not None else lhe_file_old+'.tmp'
    # Make sure the LHE file is there
    if not os.access(lhe_file_old,os.R_OK):
        raise RuntimeError('Could not access old LHE file at '+str(lhe_file_old)+'. Please check the file location.')

    # Convert the map into a str:str map, no matter what we started with
    pdgid_map_str = { str(x) : str(pdgid_map[x]) for x in pdgid_map }
    # Add anti-particles if they aren't already there
    pdgid_map_str.update( { '-'+str(x) : '-'+str(pdgid_map[x]) for x in pdgid_map if '-'+str(x) not in pdgid_map } )

    newlhe = open(lhe_file_new_tmp,'w')
    blockName = None
    eventRead = False
    with open(lhe_file_old,'r') as fileobject:
        for line in fileobject:
            # In case we're reading the param section and we have a block, read the block name
            if line.strip().upper().startswith('BLOCK') or line.strip().upper().startswith('DECAY')\
                        and len(line.strip().split()) > 1:
                pos = 0 if line.strip().startswith('DECAY') else 1
                blockName = line.strip().upper().split()[pos]
            elif '</slha>' in line:
                blockName = None
            # Check for comments - just write those and move on
            if len(line.split('#')[0].strip())==0:
                line_mod = line
                for pdgid in pdgid_map_str:
                    if pdgid in line_mod.split():
                        line_mod = line_mod.replace( pdgid , pdgid_map_str[pdgid] )
                newlhe.write(line_mod)
                continue
            # Replace the PDG ID in the mass block
            if blockName=='MASS' and line.split()[0] in pdgid_map_str:
                newlhe.write( line.replace( line.split()[0] , pdgid_map_str[ line.split()[0] ] , 1 ) )
                continue
            if blockName=='DECAY' and line.split()[1] in pdgid_map_str:
                newlhe.write( line.replace( line.split()[1] , pdgid_map_str[ line.split()[1] ] , 1 ) )
                continue
            if blockName=='QNUMBERS' and line.split()[2] in pdgid_map_str:
                newlhe.write( line.replace( line.split()[2] , pdgid_map_str[ line.split()[2] ] , 1 ) )
                continue
            if '<event>' in line:
                eventRead = True
            if eventRead and len(line.split())==13 and line.split()[0] in pdgid_map_str:
                newlhe.write( line.replace( line.split()[0] , pdgid_map_str[ line.split()[0] ] , 1 ) )
                continue

            # Otherwise write the line again
            newlhe.write(line)

    # Move the new file to the old file location
    if lhe_file_new is None:
        os.remove(lhe_file_old)
        shutil.move(lhe_file_new_tmp,lhe_file_old)
        lhe_file_new_tmp = lhe_file_old
    # Delete the old file if requested
    elif delete_old_lhe:
        os.remove(lhe_file_old)

    return lhe_file_new_tmp


