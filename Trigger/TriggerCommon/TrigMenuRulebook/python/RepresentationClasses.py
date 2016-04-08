# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class TriggerNode:
    """Tree representation for the trigger chains"""
    
    def __init__(self, name, rule = None, signature = None):
        self.name = name
        self.rule = rule
        self.signature = signature
        self.rate = 0
        self.ps = 1
        self.pt = 0
        self.es = 0
        self.daughters = []
        self.parents = []
        self.comment = ""
        
    def add_daughter(self, node):
        """Append a daughter element to the node"""
        if type(node) == type([]):
            self.daughters.extend(node)
        else:
            self.daughters.append(node)

    def ndaughters(self):
        """Return the number of daughters"""
        return len(self.daughters)

    def add_parent(self, node):
        """Append a parent element to the node"""

        #For now, let's limit ourselves to just one parent
        if len(self.parents) != 0:
            raise KeyError("We can't yet handle chains with multiple parents")
        
        if type(node) == type([]):
            self.parents.extend(node)
        else:
            self.parents.append(node)

    def nparents(self):
        """Return the number of parents"""
        return len(self.parents)

    def total_ps(self):
        """Traverse the tree to compute the total prescale"""
        if self.nparents() > 1:
            raise ValueError("We can't handle chains with multiple parents")

        if self.nparents() == 0:
            return self.ps

        return self.ps * self.parents[0].total_ps()


    #*#**
    def is_type(self, target_type):
        """Check if a 'type' appears in a chain's name"""
        output = False
        
        if target_type in self.name:
            output = True

        for parent in self.parents:
            if parent.is_type(target_type):
                output = True

        return output

    def is_multiseed(self):
        """Check if a node or any of its parents are multiseeded"""
        return self.is_type("multiseed")
    
    def is_filled(self):
        """Check if a node or any of its parents scale with filled
        bunches"""
        return self.is_type("FILLED")

    def is_empty(self):
        """Check if a node or any of its parents scale with empty
        bunches"""
        return self.is_type("EMPTY")

    def is_unpaired(self):
        """Check if a node or any of its parents scale with unpaired
        bunches"""
        return self.is_type("UNPAIRED")

    def is_unpaired_iso(self):
        """Check if a node or any of its parents scale with unpaired,
        isolated bunches"""
        return self.is_type("UNPAIRED_ISO")

    def is_firstempty(self):
        """Check if a node or any of its parents scale with first empty
        bunches"""
        return self.is_type("FIRSTEMPTY")    

    def is_unpaired_noniso(self):
        """Check if a node or any of its parents scale with unpaired,
        non-isolated bunches"""
        return self.is_type("UNPAIRED_NONISO")

    def is_bgrp10(self):
        """Check if a node or any of its parents scale with unpaired,
        non-isolated bunches"""
        return self.is_type("BGRP10")

    #*#**
                                          
    def __str__(self):
        return self.name
        

class TriggerSignature:
    """Represent a trigger's online parameters"""
    
    required_keys = set(["sig_name",
                         "evts_passed", 
                         "evts_passed_weighted",
                         "rate",
                         "rate_err", 
                         "chain_prescale",
                         "passthrough",
                         "lower_chain_name",
                         "efficiency",
                         "efficiency_err", 
                         "prescaled_efficiency"
                         ])
                        #*#** , "prescaled_efficiency_err","sig_counter",
                        #*#** ])
                        #*#** --> this didn't exist in the offline xml. Leave it out for now.
    float_keys = ["evts_passed", "evts_passed_weighted", "rate", "rate_err",
                  "chain_prescale", "passthrough",
                  "efficiency", "efficiency_err",
                  "prescaled_efficiency"]
                  #*#**, "prescaled_efficiency_err", "sig_counter",]
                  #*#** --> this didn't exist in the offline xml. Leave it out for now.
    def __init__(self, xml, lumi):
        self.parameters = {}

        if set([key.tag for key in xml]) < self.required_keys:
            raise KeyError("Item must have %s:" % self.required_keys)

        for key in self.required_keys:
            value = xml.find(key).text
            if key in self.float_keys:
                    value = float(value)
            if key == "chain_prescale" : key = "prescale"
            self.parameters[key] = value

        self.lumi = lumi

    def get(self, key, default=None):
        return self.parameters.get(key, default)
            
    def __getitem__(self, key):
        return self.parameters[key]

    def __setitem__(self, key, value):
        self.parameters[key] = value

    def __iter__(self):
        return self.parameters.__iter__()

    def __str__(self):
        return str(self.parameters)


