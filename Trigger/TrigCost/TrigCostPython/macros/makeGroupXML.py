# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom
import sys

#---------------------------------------------------------------------
# Definition of base groups.  Group names on the left are associated
# with one or more trigger tags on the right.  An asterisk indicates
# that the tag is matched as part of the name instead of the entire
# name (ex Jpsimumu is matched by psi*).  All tags that are not
# defined below are ignored.

# From the base groups five categrories are automatically generated -
# Exclusive, Inclusive, Single, Multi, and Combined.  For 
# combinations the base group is combined with every other base group
# in a non-repeating fashion.    
#---------------------------------------------------------------------
    
base_group_def = dict({ 'Muons'       : ['mu'], 
                        'EM'          : ['em'],
                        'Taus'        : ['tau', 'taunocut'],
                        'Jets'        : ['j', 'l1j'],
                        'ForwardJets' : ['fj'],
                        'TotalEnergy' : ['te'],
                        'MissingEt'   : ['xe'],
                        'JetEnergy'   : ['je'],
                        'BPhysics'    : ['jpsi*', 'bmumu', 'upsi*','dimu'],
                        'BJets'       : ['b'],
                        'Calibration' : ['calib'],
                        'MinBias'     : ['mb*', 'zdc', 'lucid'],
                        'Electron'    : ['e','zee'],
                        'Photon'      : ['g']                              })
 
additional_combined_group_def = dict()
#additional_combined_group_def = dict({ 'Combined_with_Tau' : ['mu', 'em', 'j', 'l1j', 'fj', 'te', 'xe', 'je', 'b', 'jpsi*', 'bmumu', 'upsi*', 'e', 'g', 'tau', 'trk', 'taunocut'],
#                                       'Combined_no_Tau' :   ['mu', 'em', 'j', 'l1j', 'fj', 'te', 'xe', 'je', 'b', 'jpsi*', 'bmumu', 'upsi*', 'e', 'g']})

#a list of groups defined in the menu from which to veto triggers
#veto_groups = ['Calibration']
veto_groups = []

#A list of all defined tags
used_tags = set()
for gr, tag in base_group_def.iteritems() :
    for t in tag :
        used_tags.add(t)

#The difference between all possible tags and used tags    
ignore_tags = set()

#-----------------------------------------------------------------------
#  Options definitions
#-----------------------------------------------------------------------
from optparse import OptionParser

p = OptionParser(usage="<rates xml> -o <output location>")

p.add_option("-o", "--output", type = "string", default = "./GroupRatesDef.xml",
             dest = "output_dir", help = "output directory for html" )

p.add_option("-d", "--diagnostic", action = "store_true", default=False,
                   dest="printdiag",
                   help="Flag to turn on diagnostic printing.  Will print " \
                        "a list of unassociated triggers, " \
                        "all used and ignored triggers, " \
                        "a list of all defined groups and their associated triggers "\
                        "and a list of all triggers and their associated groups"       )  

p.add_option('', '--do-trigger-groups', default=False, action="store_true",dest='trigger_groups',
                help = 'Make one group for each trigger containing only that trigger. ' \
                       'Do not make any other groups'                                       )

(options, args) = p.parse_args()
    
#-----------------------------------------------------------------
# Read triggers form the Level 1 and HLT trigger menus
#-----------------------------------------------------------------
def ReadXmlFile(filenameL1, filenameHLT):

    # create and initialize parser
    myfL1 = open(filenameL1)
    dom = xml.dom.minidom.parse(myfL1)    

    triggers = []
    for thresh in dom.getElementsByTagName("TriggerItem"):
        triggers.append(thresh.attributes["name"].value )

    myfL1.close()

    myfHLT = open(filenameHLT)
    dom = xml.dom.minidom.parse(myfHLT)    
    
    defined_groups = dict()
    for chain in dom.getElementsByTagName("CHAIN") :
        triggers.append(chain.attributes["chain_name"].value)
        for glist in chain.getElementsByTagName("GROUP_LIST") :
            for gr in glist.getElementsByTagName("GROUP") :
                defined_groups.setdefault(gr.attributes["name"].value, []).append(chain.attributes["chain_name"].value)
                
    myfHLT.close()

    return triggers, defined_groups

#----------------------------------------------------------------
#  Write the xml file that defines the groups
#----------------------------------------------------------------
def WriteXmlFile(groups) :

    #doc = xml.dom.minidom.Document()
    xmlfile = open(options.output_dir, "w")

    xmlfile.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n")
    xmlfile.write("<?xml-stylesheet type=\"text/xsl\" href=\"trigger_rates.xsl\"?>\n")
    xmlfile.write("<trigger>\n")
    for gr, trigs in groups.iteritems() :

        xmlfile.write("\t<cplx_signature>\n");
        group_type = ""
        if gr.count("cumu_rev") > 0 :
            group_type = "CumulativeReverse"
        elif gr.count("cumu") > 0 :
            group_type = "Cumulative"
        else :
            group_type = "Group"
        xmlfile.write("\t\t<type>%s</type>\n" %group_type)
        xmlfile.write("\t\t<sig_name>%s</sig_name>\n" %gr)
        xmlfile.write("\t\t<components>\n")

        for tr in trigs :
            xmlfile.write("\t\t\t<sig>\n");
            xmlfile.write("\t\t\t\t<sig_name>%s</sig_name>\n" %tr)
            xmlfile.write("\t\t\t</sig>\n")

        xmlfile.write("\t\t</components>\n")
        xmlfile.write("\t</cplx_signature>\n")

    xmlfile.write("</trigger>")
    #doc.appendChild(doc.createProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"trigger_rates.xsl\"")) 
    #top = doc.createElement("trigger")
    #doc.appendChild(top)

    #for gr, trigs in groups.iteritems() :
    #    
    #    sig = doc.createElement("cplx_signature")
    #    top.appendChild(sig)
    #    type = doc.createElement("type")
    #    group_type = ""
    #    if gr.count("cumu_rev") > 0 :
    #        group_type = "CumulativeReverse"
    #    elif gr.count("cumu") > 0 :
    #        group_type = "Cumulative"
    #    else :
    #        group_type = "Group"
    #    grtxt = doc.createTextNode(group_type)
    #    type.appendChild(grtxt)
    #    sig.appendChild(type)
    #    name = doc.createElement("sig_name")
    #    nametxt = doc.createTextNode(gr)
    #    name.appendChild(nametxt)
    #    sig.appendChild(name)
    #    comp = doc.createElement("components")
    #    sig.appendChild(comp)
    #    for tr in trigs :
    #        trgsig = doc.createElement("sig")
    #        comp.appendChild(trgsig)
    #        signame = doc.createElement("sig_name")
    #        trgtxt = doc.createTextNode(tr)
    #        signame.appendChild(trgtxt)
    #        trgsig.appendChild(signame)   
    #
    #doc.writexml(xmlfile, indent='', addindent='', newl='\n', encoding='ISO-8859-1')
    xmlfile.close()
     

#----------------------------------------------------------------------
# Makes the intersection between two lists accounting for the
# asterisk convention
#----------------------------------------------------------------------    
def MatchTags(group_tags, trigger_tags) :

    tags_matched = []
    for gt in group_tags :
        for tt in trigger_tags :
            if gt.count("*") > 0 :
                if tt.count(gt.rstrip("*")) > 0 :
                    tags_matched.append(tt)
                    
            else :
                if tt == gt :
                    tags_matched.append(tt)

    return tags_matched

#-----------------------------------------------------
#  Parses a trigger name into a list of tags
#-----------------------------------------------------    
import re
#matches all characters that will be removed to make tags
anynum = re.compile("[0-9]$|^[0-9]|i$|I$|v$|c$")  
#matches strings that should not be modified by the above selection
exception = re.compile("L1$|L2$|psi$") 
def MakeTags(name) :

    out_tags = []
    base_tags = name.split("_")[1:]
    # strip characters from the base tag 
    for i in range(0,len(base_tags)) :
        while 1 :
            if len(base_tags[i]) == 0:
                break
            e = exception.search(base_tags[i])
            if e :
                break
            m = anynum.search(base_tags[i])
            if not m :
                break
            base_tags[i] = base_tags[i][:m.start()] + base_tags[i][m.end():]
    
    # collect processed tags            
    for i in range(0, len(base_tags)) :
        if base_tags[i] != '' :
            out_tags.append(base_tags[i].lower())
    
    # remove any tags on the ignore list
    for ign in set(ignore_tags) & set(out_tags) :
        while ign in out_tags :
            out_tags.remove(ign)

    return out_tags 

#--------------------------------------------------------
#   Get unique list of all tags
#--------------------------------------------------------
def CollectTags(triglist) :

    
    tag_list = set()

    for trigger in triglist :
        tags = MakeTags(trigger)
        [tag_list.add(tg) for tg in tags ]
             
    return tag_list 

#--------------------------------------------------------------------
#  Class that decides which groups a tag should belong to
#-------------------------------------------------------------------    
class GroupLogic :
    def __init__(self) :
        pass 

    #----------------------------------------------------------------------
    #  Logic for all non-combined groups
    #----------------------------------------------------------------------    
    def MakeLogic(self, trig, group_tags) :
        # Reset variables
        self.Inclusive = False
        self.Exclusive = False
        self.Single    = False
        self.Multi     = False

        # Get and match tags
        trig_tags = MakeTags(trig) 
        tags_matched = MatchTags(group_tags, trig_tags)
        
        if len(tags_matched) > 0 :
            #Inclusive logic - require a matched tag 
            self.Inclusive = True
                # Exclusive logic - require that all tags are matched
            if len(set(trig_tags) - set(tags_matched)) == 0 :
                self.Exclusive = True
                
                # Accounting for tags like L2_mu0_mu6
                multi = 0
                for mt in tags_matched :
                    if trig_tags.count(mt) > 1 :
                        multi = 1
                
                # Try to match a number preceeding the tag in the trigger name
                mstring = ""
                for gt in group_tags : 
                    if gt.count("*") > 0 :
                        mstring += "|[0-9]+[a-z]*" +gt.rstrip("*")
                    else :
                        mstring += "|[0-9]+" + gt
                
                nmatch = re.compile(mstring.lstrip("|"))
                nm = nmatch.search(trig.lower())
                
                #Single/Multi decision
                if not nm and not multi:
                    self.Single = True
                elif nm or multi :
                    self.Multi = True

    #-------------------------------------------------------------------
    #  Logic for combined groups
    #-------------------------------------------------------------------
    def MakeCombinedLogic(self, trig, group_tags_1, group_tags_2) :
        # Reset
        self.Combined = False
        
        # Do matching
        trig_tags = MakeTags(trig)
        tags_matched_1 = MatchTags(group_tags_1, trig_tags)
        tags_matched_2 = MatchTags(group_tags_2, trig_tags)
        
        # Combined logic, match all tags, require a match from each group
        if len(tags_matched_1) > 0 and len(tags_matched_2) > 0 :
            if len(set(trig_tags) - set(tags_matched_1+tags_matched_2)) == 0 :
                self.Combined = True

    def MakeSelfCombinedLogic(self, trig, group_tags) :
        # Reset
        self.Combined = False
        
        # Do matching
        trig_tags = MakeTags(trig)
        tags_matched = MatchTags(group_tags, trig_tags)
        # Combined logic, match all tags, require a match from each group
        if len(tags_matched) == len(trig_tags) and len(tags_matched) > 1 :
            self.Combined = True
        
        if len(trig_tags) >2 and self.Combined == False :
            print "did not add trigger " + trig
    # Definition of the 5 possible groups            
    Inclusive = False
    Exclusive = False
    Single    = False
    Multi     = False
    Combined  = False
            
#------------------------------------------------------------
#  Match trigger tags to group tags using GroupLogic module
#  and create group definintions
#------------------------------------------------------------
def MakeGroups(trig_list, level) :
    
    groups = dict()

    used_groups = []
    
    for gr, group_tags in base_group_def.iteritems() :
        
        #Non combined
        for trig in trig_list :

            Logic = GroupLogic()
            Logic.MakeLogic(trig, group_tags)

            found_veto = 0
            for menu_gr in veto_groups :
                if trig in menu_groups[menu_gr] :
                    found_veto = 1
                    break
            if found_veto == 1 :
                continue        
            if Logic.Inclusive :
                groups.setdefault(level+"_Inclusive_" + gr, []).append(trig) 
            if Logic.Exclusive :
                groups.setdefault(level+"_Exclusive_" + gr, []).append(trig) 
            if Logic.Single :
                groups.setdefault(level+"_Single_" + gr,[]).append(trig)
            if Logic.Multi :
                groups.setdefault(level+"_Multi_" + gr, []).append(trig)

        #Combined        
        used_groups.append(gr)
        for gr2, gr_tags2 in base_group_def.iteritems() :
            if gr2 in used_groups : #avoid repitition
                continue
        
            for trig in trig_list :
                Logic.MakeCombinedLogic(trig, group_tags, gr_tags2)
            
                found_veto = 0
                for menu_gr in veto_groups :
                    if trig in menu_groups[menu_gr] :
                        found_veto = 1
                        break
                if found_veto == 1 :
                    continue        
                if Logic.Combined :
                    groups.setdefault(level+"_Combined_" + gr + "_" + gr2,[]).append(trig)  
    
    for gr, group_tags in additional_combined_group_def.iteritems() :
        for trig in trig_list :
            
            Logic.MakeSelfCombinedLogic(trig, group_tags);

            if Logic.Combined :
                groups.setdefault(level+"_"+gr,[]).append(trig)
     
    for name, list in groups.iteritems() :
        list.sort()
        groups[name] = list

    return groups

    
# Fill the group list with a ***REMOVED***    
def MakeCumulativeGroups(group_dict) :

    cumu_group = dict()
    for name, trigs in group_dict.iteritems() :
        if len(trigs) == 1 :
            continue
        
        token_list = []
        sorted_trigs = []
        failed_trigs = []
        for tr in trigs :
            pt = 0
            mult = 0
            token = tr.split('_')[1]
            
            #remove i or I

            token = token.rstrip('iI')

            #loop from end of token, count the occurances of numbers
            n_nums = 0
            for char in reversed(token) :
                if char.isdigit() :
                    n_nums += 1
                else :
                    break

            if n_nums == 0 :
                #print "Could not parse pt value for trigger " + tr
                failed_trigs.append(tr)
                continue
            pt = int(token[-1*(n_nums):])

            if token[0].isdigit() :
                mult = int(token[0])
            else :
                mult = 1

            print "got values %d, %d for %s" %(mult, pt, tr) 
            token_list.append((mult, pt, tr))
        
        def sort_vals(x, y) :
        #tsort = lambda x, y : 
            if x[0] == y[0] :
                return x[1] < y[1]
            else :
                return x[0] < y[0]

        print "unsorted list "
        print token_list
        #token_list.sort(tsort)
        token_list.sort(key=lambda obj: obj[1], reverse=True)
        token_list.sort(key=lambda obj: obj[0], reverse=True)
        print "sorted list "
        print token_list    
        for mult, pt, tr in token_list :
            sorted_trigs.append(tr)
        
        sorted_trigs.extend(failed_trigs) 
            
        accum_trig = []
        for tr in sorted_trigs :
            accum_trig.append(tr)
            cumu_group["%s_cumu_%i" %(name, len(accum_trig)-1)] = list(accum_trig);
            
        rev_accum_trig = []
        accum_trig.reverse()
        for trrev in accum_trig : 
            rev_accum_trig.append(trrev)
            cumu_group["%s_cumu_rev_%i" %(name,  len(rev_accum_trig)-1)] = list(rev_accum_trig)
       
    return cumu_group

def MakeTriggerGroups(trig_list) :

    trig_groups = dict()

    for tr in trig_list :
        trig_groups['%s_single' %tr] = [tr]

    return trig_groups

    
#--------------------------------------------------------
#  Start
#--------------------------------------------------------    

if len(args) != 2 :
    print "Please provide a L1 and HLT xml file" + str(len(sys.argv))
    sys.exit(-1)

    
trigger_list, menu_groups = ReadXmlFile(sys.argv[1] , sys.argv[2])

# split triggers into level groups    
tr_L1 = []
tr_L2 = []
tr_EF = []

for tr in trigger_list :
    if tr[0:2] == "L1" :
        tr_L1.append(tr)
    elif tr[0:2] == "L2" :
        tr_L2.append(tr)
    elif tr[0:2] == "EF" :
        tr_EF.append(tr)
    
#make ignore list
all_tags = CollectTags(trigger_list)
ext_used_tags = MatchTags(used_tags, all_tags)
ignore_tags = all_tags - set(ext_used_tags)

if options.trigger_groups :
    tgroup = MakeTriggerGroups(trigger_list)
    WriteXmlFile(tgroup)
    sys.exit(1)
     
#make groups for each level    
gr_L1 = MakeGroups(tr_L1, "L1")
gr_L2 = MakeGroups(tr_L2, "L2")
gr_EF = MakeGroups(tr_EF, "EF")

#if not options.no_cumulative :
#cumu_gr_L1 = MakeCumulativeGroups(gr_L1)
#cumu_gr_L2 = MakeCumulativeGroups(gr_L2)
#cumu_gr_EF = MakeCumulativeGroups(gr_EF)

# collect groups into one dictionary
all_groups = dict()

all_groups = gr_L1.copy()
all_groups.update(gr_L2)
all_groups.update(gr_EF)
all_groups.update(cumu_gr_L1)
all_groups.update(cumu_gr_L2)
all_groups.update(cumu_gr_EF)

WriteXmlFile(all_groups)

if options.printdiag :     

    used_trigs = set()
    for trigs in all_groups.values() :
        for tr in trigs:
            used_trigs.add(tr)
     
    print "*********************Unused Triggers**************************\n"
    print ", ".join(set(trigger_list) - used_trigs)
    
    print "\n********************All identified tags***********************\n"
    print ", ".join(all_tags)

    print "\n********************All used tags***********************\n"
    print ", ".join(ext_used_tags)
    
    print "\n********************All ignored tags***********************\n"
    print ", ".join(ignore_tags)
    
    print "\n********************Defined groups**************************\n"
    rev_map = dict()
    for gr, trigs in gr_L1.iteritems() :
        print gr
        for i in trigs :
            rev_map.setdefault(i, []).append(gr)
            used_trigs.add(i)
            print "\t" + i

    for gr, trigs in gr_L2.iteritems() :
        print gr
        for i in trigs :
            rev_map.setdefault(i, []).append(gr)
            used_trigs.add(i)
            print "\t" + i

    for gr, trigs in gr_EF.iteritems() :
        print gr
        for i in trigs :
            rev_map.setdefault(i, []).append(gr)
            used_trigs.add(i)
            print "\t" + i
    
    print "\n**************Trigger to Group Assocation**************\n"
    for tr, groups in rev_map.iteritems() :
        if len(groups) == 1 :
            print tr
            for i in groups :
                print "\t" + i
    for tr, groups in rev_map.iteritems() :
        if len(groups) == 2 :
            print tr
            for i in groups :
                print "\t" + i
    for tr, groups in rev_map.iteritems() :
        if len(groups) > 2 :
            print tr
            for i in groups :
                print "\t" + i

