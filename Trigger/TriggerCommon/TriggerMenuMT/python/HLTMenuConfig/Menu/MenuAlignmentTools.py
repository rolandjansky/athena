# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import numpy as np
from collections import OrderedDict
from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

# Warning: If the grouping of signatures changes in *one* of these functions, 
# it needs to change in ALL of them
# They should all group using the same dictionary

# The ordering here is VERY important - a set made from the keys will define the order the
# chains will run in! 
# have to use OrderedDict because ordering is not preserved in a standard dictionary!
the_signature_grouping = OrderedDict([
    ('Electron', 'Egamma'),
    ('Photon' , 'Egamma'),
    ('Muon' , 'Muon'),
    ('Bphysics', 'Muon'),
    ('Tau','Tau'),
    ('Jet','JetMET'),
    ('MET','JetMET'),
    ('Bjet','JetMET'),
    ('MuonnoL1', 'MuonnoL1'),
    ('UnconventionalTracking','UnconventionalTracking'),
    ])

def getAlignmentGroupFromPattern(sName, extra):
    signature_for_alignment = sName + extra
    
    if signature_for_alignment in the_signature_grouping.keys():
        return the_signature_grouping[signature_for_alignment]
    elif sName in  the_signature_grouping.keys():
        return the_signature_grouping[sName]
    else:
        log.info("No alignment grouping for signature %s (%s)",sName,extra)
        return sName

# Here, we use a list of all the signature combinations in the menu and the signature
# groupings/orderings we've defined in the_signature_grouping to calculate which signatures
# need to be ordered compared to which others.
# The function returns a dictionary of an ordered list of signatures each signature belongs to
#    e.g. 'Muon' : ['Egamma','Muon']    
def analyseCombinations(combinations_in_menu, alignmentGroups_in_combinations):
    
        # need to find out of if an alignment group, or anything in combination with that
        # aligment group, is in combination with any other alignment group.
        
        # first we make a dictionary of the ordering, based on the_signature_grouping 
        # e.g. 1 electron+photon, 2 muon, 3 tau, 4 jet/met/b-jet, 5 noL1 muons
        sig_dict = {}
        igrp = 0
        for _,v in the_signature_grouping.items():
            if v not in sig_dict:
                sig_dict[v] = igrp
                igrp += 1
        
        inv_sig_dict = {v: k for k, v in sig_dict.items()}
        
        the_matrix = np.eye((len(sig_dict)))
        
        for comb in combinations_in_menu:
            if len(comb) > 2:
                log.error("Not setup for chains with more than two signatures yet!")
            else:
                the_matrix[sig_dict[comb[0]]][sig_dict[comb[1]]] = 1  
                the_matrix[sig_dict[comb[1]]][sig_dict[comb[0]]] = 1

        _,eigenvecs = np.linalg.eig(the_matrix)
        # eigenvecs: The normalized (unit length) eigenvectors, such that the column v[:,i] 
        # is the eigenvector corresponding to the eigenvalue w[i].
        # so we do the transpose!
        eigenvecs = np.transpose(eigenvecs)
        
        # find the indices filled by each eigenvector, and make a unique list of these
        pre_unique_sets = list(set([tuple(np.nonzero(eigenvec)[0]) for eigenvec in eigenvecs]))
        
        # remove any that are subsets of another (e.g. because something like (1,1,1), (1,-1,1),(0,1,1)
        # could be an answer but (0,1,1) clearly is not spanning a unique subspace from (1,1,1))
        unique_sets = []
        for aset in pre_unique_sets:
            if len(unique_sets) == 0 : 
                unique_sets +=[aset]
            else: 
                inlist = True
                for ibset,bset in enumerate(unique_sets):
                    if aset == bset:
                        continue
                    elif set(aset).issubset(set(bset)):
                        continue
                    elif set(bset).issubset(set(aset)):
                        unique_sets.pop(ibset)
                        unique_sets += [aset]
                    else:
                        inlist = False
                if not inlist:
                    unique_sets +=[aset]

        # convert the indices back to actual signature names
        unique_by_sig = [[ inv_sig_dict[sig_int] for sig_int in setlist ] for setlist in unique_sets]

        sig_to_set = {}
        for sig in alignmentGroups_in_combinations:
            for aset in unique_by_sig:
                if sig in aset:
                    sig_to_set[sig] = aset
         
        return sig_to_set

# takes the list of signatures in a chain and changes it to be the post-grouping signatures
# e.g. ['Electron','Muon'] ==> ['Egamma','Muon] 
def setChainSignatures(signatures, lengthOfChainConfigs):

    for i, asig in enumerate(signatures):
        if asig in the_signature_grouping:
            signatures[i] = the_signature_grouping[asig]

    # this will be funny for e.g. Electron-Photon chains, or others grouped together
    # but we won't use this for these chains so it's ok
    for iCC,(config_length,asig) in enumerate(lengthOfChainConfigs):
        if asig in the_signature_grouping:
            lengthOfChainConfigs[iCC] = (config_length,the_signature_grouping[asig])
        # else it'll be a signature that doesn't need aligning, like beamspot or something.
        # maybe they should all be added to the_signature_grouping, even if they never
        # need to be merged together
    return signatures, lengthOfChainConfigs
