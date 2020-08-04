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
    ])

# Here, we use a list of all the signature combinations in the menu and the signature
# groupings/orderings we've defined in the_signature_grouping to calculate which signatures
# need to be ordered compared to which others.
# The function returns a dictionary of an ordered list of signatures each signature belongs to
#    e.g. 'Muon' : ['Egamma','Muon']    
def analyseCombinations(combinations_in_menu, signatures_in_combinations, doGroupSignatures = True):
    
        # need to find out of if a signature, or anything in combination with that signature, 
        # is in combination with any other signature
        # 1 electron, 2 photon, 3 muon, 4 tau, 5 jet, 6 met, 7 b-jet, 8 B-physics
        # but some of these share input makers and algorithms, so they need to be grouped!
        # alternatively, the IMs/signatures/etc. could have their names unique for each signature
        sig_dict = {"Electron":0,"Photon":1,"Muon":2,"Tau":3,"Jet":4,"MET":5,"Bjet":6,"Bphysics":7}
        
        # Egamma = Electron+Photon, JetMET = Jet/MET/Bjet shared for now, because they share an input maker!
        # HT chain are (will be) part of the jet signature.
        if doGroupSignatures:
            seen = set()
            sig_dict = dict((x,len(seen)-1) for x in the_signature_grouping.values() if not (x in seen or seen.add(x)))
            log.debug('Grouping of signatures to align: %s',sig_dict)
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
        for sig in signatures_in_combinations:
            for aset in unique_by_sig:
                if sig in aset:
                    sig_to_set[sig] = aset
         
        return sig_to_set
        
# redefine the various signature lists/dicts to use the grouped signatures (e.g. not
# electron, photon but instead egamma)
def groupSignatures(combinations_in_menu, signatures_to_align, length_of_configs):
    for icomb,comb in enumerate(combinations_in_menu):
        if any([acomb in the_signature_grouping for acomb in comb]):
            tmpcomb = []
            for asig in comb:
                if asig in the_signature_grouping:
                    tmpcomb += [the_signature_grouping[asig]]
                else:
                    # not sure this could come up - but if we have something random like minbias + muon chains? 
                    tmpcomb += [asig]

        combinations_in_menu[icomb] = tmpcomb
        
    signatures_to_align = list(signatures_to_align) #was a set, make a list!
    for isig, asig in enumerate(signatures_to_align):
        if asig in the_signature_grouping:
            signatures_to_align[isig] = the_signature_grouping[asig]
                                        
    signatures_to_align = list(set(signatures_to_align)) #get rid of duplicates
          
    log.debug('Signatures that are going to be aligned: %s',signatures_to_align)
    log.debug('Signature combinations that are in the menu: %s',combinations_in_menu)

    for newsig in set(the_signature_grouping.values()):
        if newsig not in length_of_configs:
            # get all the signatures in the new sig:
            corresponding_sig_lengths = [length_of_configs.get(oldsig,0) for oldsig in the_signature_grouping if the_signature_grouping[oldsig] == newsig]
            length_of_configs[newsig] = max(corresponding_sig_lengths)

    return combinations_in_menu, signatures_to_align, length_of_configs

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