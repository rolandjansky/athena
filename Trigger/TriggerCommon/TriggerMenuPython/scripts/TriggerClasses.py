# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logging
import xml.etree.cElementTree as ET

#-----------------------------------------------------------------------------
# Helper class: stores rule for one lumi point
#
class LumiPoint:
    def __init__(self, lumi):
    
        self.lumi        = int(lumi)
        self.name        = ''
        self.ps_slope    = None
        self.ps_offset   = None
        self.ps_has_plus = False

        self.pt_slope       = None # PT PS slope
        self.pt_offset      = None # PT PS offset
        self.default_off_pt = 0

        self.es_slope       = None # express stream PS slope
        self.es_offset      = None # express stream PS offset
        self.default_off_ps = -999.0

    def SetValue_PS(self, pars):
        if len(pars) != 2:            
            return False
        
        self.ps_slope  = float(pars[0])
        self.ps_offset = float(pars[1])

        if int(self.ps_slope) == 0 and int(self.ps_offset) == 0:
            self.ps_has_plus = pars[0].count('+') or pars[1].count('+');
            
        return True

    def SetValue_PT(self, pars):
        if len(pars) == 0:
            return True
        elif len(pars) != 2:            
            return False

        self.pt_slope  = float(pars[0])        
        self.pt_offset = float(pars[1])        
        return True

    def SetValue_ES(self, pars):
        if len(pars) == 0:
            return True
        elif len(pars) != 2:
            return False

        self.es_slope  = float(pars[0])
        self.es_offset = float(pars[1])
        return True

    def HasPlusPS(self):
        return self.ps_has_plus
    
    def ComputePS(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_ps
        
        try:
            return float(self.ps_slope)*float(target_lumi)/float(self.lumi)+float(self.ps_offset)
        except:
            logging.error('Problem for trigger %s - set PS=off' %self.name)
            return self.default_off_ps

    def ComputeES(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_ps

        if self.es_slope != None:
            return float(self.es_slope)*float(target_lumi)/float(self.lumi)+float(self.es_offset)

        logging.error('Problem for trigger %s - set ES=off' %self.name)
        return self.default_off_ps

    def ComputePT(self, target_lumi):
        if self.lumi < 1:
            return self.default_off_pt

        if self.pt_slope != None:
            return float(self.pt_slope)*float(target_lumi)/float(self.lumi)+float(self.pt_offset)

        logging.error('Problem for trigger %s - set PT=off' %self.name)
        return self.default_off_pt


#-----------------------------------------------------------------------------
# Helper class: stores rules for one trigger
#
class TriggerRule:
    def __init__(self):
    
        self.trigger_name        = ""
        self.has_express         = False
        self.has_pt              = True
        self.lumi_points         = {}
        
    def ReadLine(self, line):
        
        input_list = line.strip(' \n').split(';')

        if len(input_list) < 3:
            logging.warning('Skip incorrectly formatted input line: %s' % line)
            return False

        self.trigger_name = input_list[0].strip()

        lumi_list = input_list[1].strip("[ \t ]").split(":")
        ps_list   = input_list[2].strip("[ \t ]").split(":")
        pt_list   = []
        es_list   = []

        self.has_express = len(input_list) > 3
        self.has_pt      = len(input_list) > 4
        
        if self.has_express: es_list = input_list[3].strip("[ \t ]").split(":")
        if self.has_pt:      pt_list = input_list[4].strip("[ \t ]").split(":")

        if len(lumi_list) < 1:
            logging.warning('Failed to parse luminosity values for: %s' % self.trigger_name)
            return False
        if len(lumi_list) != len(ps_list):
            logging.warning('Lumi and PS lists have different size for: %s' % self.trigger_name)
            return False

        if self.has_express and len(lumi_list) != len(es_list):
            logging.warning('Lumi and ES lists have different size for: %s' % self.trigger_name)
            return False

        if self.has_pt and len(lumi_list) != len(pt_list):
            logging.warning('Lumi and PT lists have different size for: %s' % self.trigger_name)
            return False
        
        if self.has_express and len(lumi_list) != len(es_list):
            logging.warning('Lumi and ES lists have different size for: %s' % self.trigger_name)
            return False

        if self.has_pt and len(lumi_list) != len(pt_list):
            logging.warning('Lumi and PT lists have different size for: %s' % self.trigger_name)
            return False
        

        for i in range(0, len(lumi_list)):
            lumiP = LumiPoint(int(lumi_list[i]))
            lumiP.name = self.trigger_name
            
            if not lumiP.SetValue_PS(ps_list[i].strip("( )").split(",")):
                logging.warning('Failed to read PS paramemets for: %s' % self.trigger_name)
                return False 
            
            if self.has_express:
                if i < len(es_list):
                    lumiP.SetValue_ES(es_list[i].strip("( )").split(","))
                    logging.debug('Read express stream PS paramemets for: %s' % self.trigger_name)
                else:
                    lumiP.es_slope  = 0.0
                    lumiP.es_offset = 0.0
                    
            if self.has_pt:
                if i < len(pt_list):
                    lumiP.SetValue_PT(pt_list[i].strip("( )").split(","))
                    logging.debug('Read PT paramemets for: %s' % self.trigger_name)
                else:
                    lumiP.pt_slope  = 0.0
                    lumiP.pt_offset = 0.0

            #
            # Save new luminosity point: dictionary key is integer luminosity
            #
            self.lumi_points[lumiP.lumi] = lumiP

        return True

    def FindLumiPoint(self, target_lumi):
        if len(self.lumi_points) == 0:
            logging.warning('%s has no lumi points' % self.trigger_name)
            return None

        lumis = self.lumi_points.keys()

        #
        # Target lumi point is smaller than lowest luminosity point
        #  
        if target_lumi < min(lumis):
            logging.debug('%s: use lowest lumi points: %d' % (self.trigger_name, min(lumis)))
            return self.lumi_points[min(lumis)]

        #
        # Target lumi point is greater than highest luminosity point
        #        
        if target_lumi > max(lumis):
            logging.debug('%s: use highest lumi point: %d' % (self.trigger_name, max(lumis)))
            return self.lumi_points[max(lumis)]

        #
        # In this case: lumiA < lumiB < target_lumi < lumiC - use lumiB
        #
        lumis.sort(None, None, True)
        for lumi in lumis:
            logging.debug('Lumi step: %d' %lumi)
            if target_lumi >= lumi:
                logging.debug('Matched target lumi: %d' %target_lumi)
                return self.lumi_points[lumi]

        logging.error('%s: failed to find lumi point: %d' % (self.trigger_name, target_lumi))
        return self.lumi_points[max(lumis)]        

    def GetLevel(self):
        if len(self.trigger_name) < 2:
            logging.error('%s: failed to determine level '% self.trigger_name)
            return ''
        return self.trigger_name[0:2]

    def GetTriggerName(self):
        return self.trigger_name


#-----------------------------------------------------------------------------
# Helper class: stores the items of a chain and their rules
#
class Chain:
    """Representation of a trigger chain and it's PS going through L1, L2, and EF
    """
    
    L1Name = ""
    L2Name = ""
    EFName = ""
    
    L1Rule = None
    L2Rule = None
    EFRule = None
    
    Lumi = []

    minLumi = -1
    maxLumi = -1

    def buildChainFromNames(self, L1Name=None, L2Name=None, EFName=None):
        if L1Name != None:
            self.L1Name = L1Name

        if L2Name != None:
            self.L2Name = L2Name
            
        if EFName != None:    
            self.EFName = EFName

    def buildChainFromMenu(self, name, HLTXML, L1XML):
        """Find all lower chains for a given item name
        """
        HLT_items = ET.parse(HLTXML).getroot().findall('CHAIN_LIST/CHAIN')
        chain_info = None

        #Read the chain infor from xml, starting with an EF trigger and making our way down to the L1 name
        while name != None and name[0:2] != 'L1':
            for item in HLT_items:
                if item.get('chain_name') == name:
                    chain_info = item
                    break
                
            if name.count(","):
                err = "class doesn't yet know how to deal with multi-seeded chains"
                raise NameError(err)
        
            if chain_info == None:
                err = "Could not find chain %s in HLT menu" % name
                raise NameError(err)
        
            if chain_info.get('level') == "EF":
                self.EFName = name
                name = chain_info.get('lower_chain_name')
                chain_info = None
            elif chain_info.get('level') == "L2":
                self.L2Name = name
                self.L1Name = chain_info.get('lower_chain_name')
                if self.L1Name.count(","):
                    err = "class doesn't yet know how to deal with multi-seeded chains"
                    raise NameError(err)
                name = None
                chain_info = None

        if name != None and name[0:2] == 'L1':
            self.L1Name = name
            

    def buildRules(self, trigger_rules, Lumi=None):
        """Get the luminosity breakpoints and calculate the prescale
        at each point.
        """

        if Lumi == None:
            Lumi = []

        #Check that we have rules for each level, and get their lumi points
        try:
            self.EFRule = trigger_rules[self.EFName]
            Lumi.extend([self.EFRule.lumi_points[x].lumi for x in self.EFRule.lumi_points.keys()])
        except KeyError:
            if self.EFName != '':
                err = "No rule for '%s' in the rulebook" % self.EFName
                logging.warning(err)

        try:
            self.L2Rule = trigger_rules[self.L2Name]
            Lumi.extend([self.L2Rule.lumi_points[x].lumi for x in self.L2Rule.lumi_points.keys()])
        except KeyError:
            if self.L2Name != '':
                err = "No rule for '%s' in the rulebook" % self.L2Name
                logging.warning(err)
            
        try:
            self.L1Rule = trigger_rules[self.L1Name]
            Lumi.extend([self.L1Rule.lumi_points[x].lumi for x in self.L1Rule.lumi_points.keys()])
        except KeyError:
            if self.L1Name != '':
                err = "No rule for '%s' in the rulebook" % self.L1Name
                logging.warning(err)

        #Artificially expand lumi to cover default range
        if (self.maxLumi >= 0 and max(Lumi) < self.maxLumi):
            Lumi.append(self.maxLumi)
        if (self.minLumi >= 0 and min(Lumi) > self.minLumi):
            Lumi.append(self.minLumi)

        #Get a unique / sorted list of lumi points
        self.Lumi = sorted({}.fromkeys(Lumi).keys())

    def getPS(self, Lumi=[]):
        """ Return a dictionary with the names of the L1, L2, and EF
        items and their prescales for a set of luminosity points. If
        no luminosity points have yet been defined, try to read them
        from the rulebook.
        """

        #if (len(self.Lumi) == 0 and trigger_rules != None):
        #    self.buildPS(trigger_rules)

        if len(Lumi) == 0:
            Lumi = self.Lumi

        EFName = self.EFName
        if EFName == "":
            EFName = "Blank EF"
        L2Name = self.L2Name
        if L2Name == "":
            L2Name = "Blank L2"
        L1Name = self.L1Name
        if L1Name == "":
            L1Name = "Blank L1"            
        
        output = {
            'EFName'  : EFName,
            'EFPS'    : [],
            'EFPT'    : [],
            'EFhasSlope' : [],
            'L2Name'  : L2Name,
            'L2PS'    : [],
            'L2PT'    : [],
            'L2hasSlope' : [],
            'L1Name'  : L1Name,
            'L1PS'    : [],
            'L1PT'    : [],
            'TotalPS' : [],
            'Lumi'    : [],
            'L1hasSlope' : [],
            }

        
        for (i, lumi) in enumerate(self.Lumi):
            #print "i: %s, lumi: %s" % (i, lumi)
            for (rule, ps, pt, slope) in ((self.EFRule, output['EFPS'], output['EFPT'], output['EFhasSlope']), \
                                          (self.L2Rule, output['L2PS'], output['L2PT'], output['L2hasSlope']), \
                                          (self.L1Rule, output['L1PS'], output['L1PT'], output['L1hasSlope']) \
                                          ):
                for l in (lumi-1, lumi):
                    if rule == None:
                        ps.append(int(-999))
                        pt.append(int(0))
                        slope.append(False)
                        continue
                    
                    point = rule.FindLumiPoint(l)
                    if point != None:
                        ps.append(int(point.ComputePS(lumi)))
                        if point.pt_slope != None:
                            pt.append(int(point.ComputePT(lumi)))
                        else:
                            pt.append(int(0))
                            
                        if point.ps_slope != None and point.ps_slope != 0:
                            slope.append(True)
                        else:
                            slope.append(False)
                    else:
                        print "Couldn't find point"
                        ps.append(int(-999))
                        pt.append(int(0))
                        slope.append(False)

                        
            output['Lumi'].append(lumi)
            output['Lumi'].append(lumi)


        for i in range(len(output['L1PS'])):
            output['TotalPS'].append(output['L1PS'][i] * output['L2PS'][i] * output['EFPS'][i])
            if (output['L1PS'][i] < 0 or output['L2PS'][i] < 0 or output['EFPS'][i] < 0) and output['TotalPS'][i] > 0:
                output['TotalPS'][i] = -1 * output['TotalPS'][i]
                
        return output

    def __repr__(self):
        values = self.getPS()
        
        output  = "EF Name: %s, PS: %s \n" % (values['EFName'], values['EFPS'])
        output += "L2 Name: %s, PS: %s \n" % (values['L2Name'], values['L2PS'])
        output += "L1 Name: %s, PS: %s \n" % (values['L1Name'], values['L1PS'])
        output += "Lumi Points %s" % self.Lumi
        return output

