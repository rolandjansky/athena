# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
import numpy as np
from collections import OrderedDict
from AthenaCommon.Logging import logging
import itertools

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
    ('UnconventionalTracking','JetMET'),
    ('Bjet','JetMET'),
    ('MuonnoL1', 'MuonnoL1'),

    ])

def get_alignment_group_ordering():
    seen = set()
    return [v for v in the_signature_grouping.values() if not (v in seen or seen.add(v))]

def get_alignment_group_from_pattern(signature, extra):
    signature_for_alignment = signature + extra
    
    if signature_for_alignment in the_signature_grouping.keys():
        return the_signature_grouping[signature_for_alignment]
    elif signature in the_signature_grouping.keys():
        return the_signature_grouping[signature]
    else:
        log.info("No dedicated alignment grouping for signature %s (%s)",signature,extra)
        return signature

class MenuAlignment():
    """ Class to hold/calculate chain alignment """
    def __init__(self, combinations_in_menu, groups_to_align, length_of_configs):
    
        self.combinations_in_menu = combinations_in_menu
        self.length_of_configs = length_of_configs
        self.groups_to_align = groups_to_align
        
        self.signature_dict = {}
        self.inverse_sig_dict ={}
        
        self.sets_to_align = {}

        # first we make a dictionary of the ordering, based on the_signature_grouping 
        # e.g. 1 electron+photon, 2 muon, 3 tau, 4 jet/met/b-jet, 5 noL1 muons
        igrp = 0
        for value in the_signature_grouping.values():
            if value not in self.signature_dict:
                self.signature_dict[value] = igrp
                igrp += 1
        
        self.inverse_sig_dict = {v: k for k, v in self.signature_dict.items()}

    # Chains can be grouped together, e.g. e chains, mu chains, gamma chains.
    # Chains are grouped if we want their steps to run in parallel, e.g. if they have shared
    # sequences, like e+gamma chains use the same calo clustering step. 
    # Each group is called alignment group. These can also split signatures: muons have
    # two alignment groups, Muon and MuonnoL1 (chainDict['signature']+chainDict['extra']) 

    # With combined chains, e.g. e+mu, we can define in the_signature_grouping if we want 
    # the two legs to run in parallel (put them in the same alignment group, e.g. EgammaMuon),
    # or in series (to save CPU - don't run slow muons until the electron decision has been made)

    # The alignment groups have a global ordering defined in the_signature_grouping
    # But we only want to make all these triggers run in series if the combinations exist
    # in the menu - i.e. do not put Tau after Egamma and Muon unless there are combined
    # Tau chains that necessitate this ordering.

    # Here, we use a list of all the combined chain signature combinations in the menu and the signature
    # groupings/orderings we've defined in the_signature_grouping to calculate which signatures
    # need to be ordered compared to which others.
    # The function returns a dictionary of an ordered list of signatures each signature belongs to
    #    e.g. 'Muon' : ['Egamma','Muon']

    def analyse_combinations(self): #combinations_in_menu, alignmentGroups_in_combinations):
    
        # need to find out of if an alignment group, or anything in combination with that
        # aligment group, is in combination with any other alignment group.
        
        the_matrix = np.eye((len(self.signature_dict)))
        
        for comb in self.combinations_in_menu:
            for comb_pair in list(itertools.combinations(comb,2)):
                the_matrix[self.signature_dict[comb_pair[0]]][self.signature_dict[comb_pair[1]]] = 1  
                the_matrix[self.signature_dict[comb_pair[1]]][self.signature_dict[comb_pair[0]]] = 1

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
        unique_by_sig = [[ self.inverse_sig_dict[sig_int] for sig_int in setlist ] for setlist in unique_sets]

        sig_to_set = {}
        for sig in self.groups_to_align:
            for aset in unique_by_sig:
                if sig in aset:
                    sig_to_set[sig] = aset
         
        self.sets_to_align = sig_to_set
        
        return

    def alignment_group_is_alone(self, alignment_grp):
        # no need to align lonely alignment groups
        # the latter condition should never happen, because we only put in alignment groups that are 
        # in combined chains, and thus will need *some* aligning. but good to check in any case.
        if alignment_grp not in self.sets_to_align:
            return True
        elif len(self.sets_to_align[alignment_grp]) == 1:
            return True
        return False
        
    def alignment_group_is_first(self, alignment_grp):
    
        # if it's the first chain in the set to be aligned, again - nothing to do here.
        if alignment_grp == self.sets_to_align[alignment_grp][0]:
            return True
        return False

    def get_groups_to_align(self, alignment_grp):

        all_groups_to_align = self.sets_to_align[alignment_grp]
        index_of_chain_group = self.sets_to_align[alignment_grp].index(alignment_grp)

        # we don't need to add empty steps for alignment groups that happen *after* the
        # one our chain belongs to - so just cut off at the chain's alignment group here.        
        return all_groups_to_align[:index_of_chain_group]


    def single_align(self, chainDict, chainConfig):
      
      if len(set(chainDict['alignmentGroups'])) != 1:
          log.error("Cannot call single_align on chain %s with alignment groups %s",
                    chainDict['chainName'], ",".join(chainDict['alignmentGroups']))
          raise Exception("Will not proceed, the chain is not suitable for single alignment.")

      alignment_grp = chainDict['alignmentGroups'][0]

      if self.alignment_group_is_alone(alignment_grp):
        log.debug("Finished with retrieving chain configuration for chain %s", chainDict['chainName'])
        chainConfig.numberAllSteps()
      elif self.alignment_group_is_first(alignment_grp):
        # if it's the first chain in the set to be aligned, again - nothing to do here,
        # since this is single_align
        log.debug("Finished with retrieving chain configuration for chain %s", chainDict['chainName']) 
        chainConfig.numberAllSteps()
      else:
        # now we know that empty steps are necessary before this chain. we can loop through and add accordingly
        aligngroups_set = self.get_groups_to_align(alignment_grp)
        # but we want to do this in reverse
        aligngroups_set.reverse()
                      
        for align_grp_to_align in aligngroups_set:
            chainConfig.insertEmptySteps('Empty'+align_grp_to_align+'Align',self.length_of_configs[align_grp_to_align],0)         
            log.debug("Finished with retrieving chain configuration for chain %s", chainDict['chainName'])
            chainConfig.numberAllSteps()

      return chainConfig
      
    def multi_align(self, chainDict, chainConfig, lengthOfChainConfigs):
    
        alignment_grps = chainDict['alignmentGroups']

        #check for a few bad conditions first:
        if not set(alignment_grps).issubset(self.sets_to_align):
            log.error(" one of the alignmentGroups in %s is not available in the sets to align dictionary!", alignment_grps)
            raise Exception("MenuAlignment.analyse_combinations() needs checking, this should never happen.")
        elif len(set([tuple(self.sets_to_align[x]) for x in alignment_grps])) != 1:
            log.error(" the alignmentGroups %s point to different sets in the sets to align dictionary", alignment_grps)
            for x in alignment_grps:
                log.error("     Set: %s, group %s", self.sets_to_align[x] , x)
            raise Exception("MenuAlignment.analyse_combinations() needs checking, this should never happen.")
                
        #now we know that all alignmentGroups points to the same set, so just use the first entry
        if len(self.sets_to_align[alignment_grps[0]]) == 2:

           # if the pair is on its own, then we just make sure the first signature's number
           # of steps is equal to the max in that signature (so the next signature starts at the right step)
      
           # not a dictionary because we could have a chain mu_jet_munoL1? Not sure. But don't want to
           # overwrite duplicates yet. 
           # probably, at some point, will need to divide this beyond signature but instead as unique sequence within a signature. 
           # munoL1 is already one case...
           length_firstgrp = 0
           max_length_firstgrp = self.length_of_configs[self.sets_to_align[alignment_grps[0]][0]]

           for config_length,config_grp in lengthOfChainConfigs:
               if config_grp == self.sets_to_align[alignment_grps[0]][0]:
                   length_firstgrp = config_length

           if length_firstgrp < max_length_firstgrp:
               #too short! need to add padding steps between two alignment groups...
               needed_steps = max_length_firstgrp - length_firstgrp
               chainConfig.insertEmptySteps('Empty'+self.sets_to_align[alignment_grps[0]][0]+'Align',needed_steps,length_firstgrp) 
      
           elif length_firstgrp > max_length_firstgrp:
               log.error("%s first signature length %d is greater than the max calculated, %d",chainDict.name,length_firstgrp, max_length_firstgrp)     
               raise Exception("Probably something went wrong in GenerateMenuMT.generateChains()!")

        #this should probably work for signatures > 2, but might be a few gotchas (and errors need updating)
        #Can't properly test until ATR-22206 is resolved.
        elif len(self.sets_to_align[alignment_grps[0]]) > 2:
            if not set(alignment_grps).issubset(self.sets_to_align):
                log.error(" one of the alignmentGroups in %s is not available in the sets to align dictionary!", alignment_grps)
                raise Exception("MenuAlignment.analyse_combinations() needs checking, this should never happen.")
            elif len(set([tuple(self.sets_to_align[x]) for x in alignment_grps])) != 1:
                log.error(" the alignmentGroups %s point to different sets in the sets to align dictionary", alignment_grps)
                for x in alignment_grps:
                    log.error("     Set: %s, group %s", self.sets_to_align[x] , x)
                raise Exception("MenuAlignment.analyse_combinations() needs checking, this should never happen.")
            
            # we need to know which alignment_grps are in the chain in which order. Assume this is always stored correctly.
            # (this should be true)
            # never need to align the last chain - it can end a different length, no problem.
            # ignore any signatures after the end of those in this chain
            aligngroups_set = self.get_groups_to_align(alignment_grps[-1])
            aligngroups_set.reverse()
            grp_masks = [x in alignment_grps for x in aligngroups_set]
            grp_lengths = []
            for align_grp,grp_in_chain in zip(aligngroups_set,grp_masks):
                if grp_in_chain:
                  for config_length,config_grp in lengthOfChainConfigs:
                    if config_grp == align_grp: 
                        grp_lengths += [config_length]
                else:
                  grp_lengths += [0]
          
            for istep,(align_grp,grp_in_chain,length_in_chain) in enumerate(zip(aligngroups_set,grp_masks,grp_lengths)):
                # We're working our way backwards through the chain 
                # need to know how many steps are already before us!
                n_steps_before_grp = 0
                if istep < len(grp_lengths)-1: 
                    n_steps_before_grp = sum(grp_lengths[istep+1:])
                max_length_grp = self.length_of_configs[align_grp]
                if grp_in_chain:
                    if length_in_chain < max_length_grp:
                        #too short! gotta add padding steps between two alignmentGroups...
                        needed_steps = max_length_grp - length_in_chain
                        start_step = n_steps_before_grp + length_in_chain
                        chainConfig.insertEmptySteps('Empty'+align_grp+'Align',needed_steps,start_step) 
                else:
                    # this sig isn't in the chain, but we still will need empty steps for it
                    # always add them to the start, because we're running in reverse order
                    chainConfig.insertEmptySteps('Empty'+align_grp+'Align',self.length_of_configs[align_grp],n_steps_before_grp) 
        else:
             log.error("Should never reach this point. alignmentGroups: %s, sets_to_align: %s",alignment_grps,self.sets_to_align)
             raise Exception("MenuAlignment.multi_align() needs checking, this should never happen.")

        log.debug("Finished with retrieving chain configuration for chain %s", chainDict['chainName']) 
        chainConfig.numberAllSteps()
        
        return chainConfig
