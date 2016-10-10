# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

class Chain:
    def __init__(self, name, seed, ps, requirements):
        self.name = name
        self.seed = seed
        self.ps = ps
        assert type(ps) == type(1.0) or type(ps) == type(1)
        self.requirements = requirements

class ChainsList:
    def __init__(self):
        self.chains = []

    def __add__(self, c):
        self.chains.append(c)

    def __iter__(self):
        return self.chains.__iter__()

def setOutput(obj, prop, name):
    cval = obj.getProperties()[prop]
    #print "red off ", name, cval
    if type(cval) == type(list()):                
        cval.append(name)
        print prop, name, cval
        setattr(obj, prop, cval)
    else:
        setattr(obj, prop, name)

    try: # this is working only for outputs which do not support arrays
        setattr(obj, prop+"Aux", name+"Aux.")
    except:
        pass

def getOutput(obj, prop):
    try:
        # print "getOutput getattr"
        return getattr(obj, prop)
    except:
        # print "getOutput getDefaultProperty ", obj.getDefaultProperty(prop)
        return obj.getDefaultProperty(prop)
    # print "Can not read pro"
    raise "Error in reading property " + prop + " from " + obj



def genMenuAlgView(name):
#    from connectAlgorithmsIO import connectAlgorithmsIO

    from ViewAlgs.ViewAlgsConf import AthViews__MenuAlgView
    menuAlg = AthViews__MenuAlgView(name)
    # for hypo in inputHypos:
    #     connectAlgorithmsIO(consumer=(menuAlg, "HypoDecisions"), producer=(hypo, "OutputDecisions"))
    # #menuAlg.HypoDecisions = getOutput(inputHypos, "OutputDecisions")                                             
    # for chains in inputChains:
    #     connectAlgorithmsIO(consumer=(menuAlg, "InputChainDecisions"), producer=(chain, "OutputChainDecisions"))

    #menuAlg.InputChainDecisions = getOutput(inputChains,"OutputChainDecisions")                                                                     
    setOutput(menuAlg, "OutputHypoDecisions", name+"RoIs")
    setOutput(menuAlg, "OutputChainDecisions", name+"Chains")
    return menuAlg


 
def genMenuAlg(name): #, inputHypos, inputChains):
#    from connectAlgorithmsIO import connectAlgorithmsIO
    
#    assert inputHypos != None, 'Alg to take hypo putput from is missing'
#    assert inputChains != None, 'Alg to take chain decisions from is missing'
#    print inputChains, inputHypos
    from ViewAlgs.ViewAlgsConf import MenuAlg
    menuAlg = MenuAlg(name)
#    connectAlgorithmsIO(consumer=(menuAlg, "HypoDecisions"), producer=(inputHypos, "OutputDecisions"))
    #menuAlg.HypoDecisions = getOutput(inputHypos, "OutputDecisions")
#    connectAlgorithmsIO(consumer=(menuAlg, "InputChainDecisions"), producer=(inputChains, "OutputChainDecisions"))
    #menuAlg.InputChainDecisions = getOutput(inputChains,"OutputChainDecisions")
    setOutput(menuAlg, "OutputHypoDecisions", name+"RoIs")
    setOutput(menuAlg, "OutputChainDecisions", name+"Chains")
    menuAlg.OutputLevel=1
    return menuAlg




def configureMenu(chains, topSequence):
    # configure prescaling
    psAlg = getattr(topSequence, "HLTChainsPrescaling")
    assert psAlg, "Missing prescaling algorithms"
    psConf = []
    for chain in chains:
        psAlg.Prescales.append( "%s %f" % (chain.name, chain.ps) )
        for alg, req in chain.requirements.iteritems():            
            menuAlg = getattr(topSequence, alg)
            assert menuAlg, "Missing menu algorithm "+str(alg)
            menuAlg.Required += [chain.name + " = " + req]
    print "Menu configured"
    
def getMergeCollectionList(list_of_collections):
    list_of_outputs = [getOutput(coll, "OutputHypoDecisions") for coll in list_of_collections]
    list_of_outputs.append('')
    return ",".join(list_of_outputs)


def formatChainConfString(chain, te_mult_dict):
    return chain + " = " + ", ".join([te+" x "+str(mult) for te,mult in te_mult_dict.iteritems()])

class MenuIngreedients:
    def __init__(self):
        self.__connections  = {}
        self.__te_to_menu = {}

        self.__prescaling = {}

    def addHypo(self, menu, hypo):
        self.__connections[menu] = hypo
        self.__connections[hypo] = menu
        def __extract_te_names(hypo_algo):
            r = []
            tool_arrays = [v for k,v in hypo.properties().iteritems() if type(v).__name__ == 'PrivateToolHandleArray']
            for array in tool_arrays:
                r.extend([t.getName() for t  in tool_arrays[0]])                
            return r

        names = __extract_te_names(hypo)
        #print names
        self.__te_to_menu.update( [(n,menu) for n in names] )    

    def addL1RoIsFilters(alg, patterns):
        for p in patternss:
            self.__prescaling[p] = alg

    def configureL1RoIsFilter(chain, tes):
        for t in tes:
            for pattern, alg in self.__prescaling.iteritems():
                if t in pattern:
                    alg.Required += ""

    def tes_mapping(self):
        return self.__te_to_menu

    def map_to_menu_alg(self, cut):
        if  self.__te_to_menu.has_key(cut):
            return self.__te_to_menu[cut]
        else:
            return None

menu_ingredients = MenuIngreedients()




def configureFromXML(xmlFile, menu):
    chains = ChainsList()
    import xml.etree.ElementTree as ET
    p = ET.parse(xmlFile)

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()    
    prescaling = topSequence.activeRoIsAfterPrescaling


    # for each threshold mentioned in the chain we find Menu alg and through set there properties
    seq_mapping = {}
    for s in p.findall('SEQUENCE_LIST/SEQUENCE'):
        output = s.attrib['output']
        instances = [a.split('/')[1] for a in s.attrib['algorithm'].split()]
        seq_mapping[output] = instances        


    for c in p.findall('CHAIN_LIST/CHAIN'):
        chain_name = c.attrib['chain_name']
        ps   = c.attrib['prescale']        
        sig_list = list(c.iter('SIGNATURE_LIST'))[0]
        for sig in sig_list.iter("SIGNATURE"):
            tes_mult = {}
            for te in sig.iter("TRIGGERELEMENT"):
                te_name = te.attrib['te_name']
                print "tename", te_name
                if not tes_mult.has_key(te_name):
                    tes_mult[te_name] = 1
                else:
                    tes_mult[te_name] += 1
            

            found_algo = False
            for te_name, mult in tes_mult.iteritems():
                for alg in seq_mapping[te_name]:
                    menu_alg = menu.map_to_menu_alg(alg)
                    if menu_alg:
                        found_algo = True
                        conf_string = chain_name + ' = ' + alg + ' x ' + str(mult)
                        print 'Adding config string ', conf_string, ' to alg:', menu_alg.getName()
                        menu_alg.Required += [chain_name + ' = ' + alg + ' x 1']
                if found_algo == False:
                    print "ERROR unable to find Menu Alg where setting TE", te_name ,"requirement can be placed while configuring chain", chain_name

        for sig in sig_list.iter('L1SIGNATURE'):
            tes = dict()
            for te in sig.iter("TRIGGERELEMENT"):
                tes[str(te.attrib['te_name'])] = 1
            prescaling.Required += [formatChainConfString(chain_name, tes)]
            
