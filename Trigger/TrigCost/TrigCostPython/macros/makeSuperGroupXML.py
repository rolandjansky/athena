# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import xml.dom.minidom
import sys, os

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

groups_def = ['MinBias','Calibration','Taus','Egamma','BPhysics','Muons','BJets','Jets','MissingEt','Other']    
base_group_def = dict({ 'Taus'        : ['tau', 'taunocut'],
                        'Egamma'      : ['em','g','e','zee'],
                        'Muons'       : ['mu'],                         
                        'Jets'        : ['j', 'l1j','fj','je'],
                        'MissingEt'   : ['xe','te'],
                        'BPhysics'    : ['jpsi*', 'bmumu', 'upsi*','dimu'],
                        'BJets'       : ['b'],
                        'Calibration' : ['calib','larcalib','idcalib'],
                        'MinBias'     : ['mb*', 'mb','zdc', 'lucid','mbsptrk','mbmbts','mbrndm','filled','bcm','mbts'],
                        'Other'       : ['cost','trk','alll1'] })
 
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
p.add_option('-a', '--append', type = "string",  
                    dest = "append_xml", help = "Name of xml file to append groups" )

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
def WriteXmlFile(groups, append_xml) :

    xmlfile = None 
    if append_xml != None :
        xmlfile = open(append_xml+".temp",'w')
        oldxml = open(append_xml, 'r')
        print xmlfile.name
        if not xmlfile :
            print "Failed to open " + append_xml
        for line in oldxml :
            if line.count(r'</trigger>') == 0 :
                xmlfile.write(line)
            else :
                break


    else :
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
        
    if append_xml != None :
        os.system("mv " + append_xml+".temp " + append_xml)
        

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
        self.group = 'None'
        

        # Get and match tags
        trig_tags = MakeTags(trig) 
        #print trig, " " , trig_tags
        for group in groups_def :
            #print group
            for tag in trig_tags :
                if self.group != 'None' :
                    break
                for gr in base_group_def[group] :
                    if tag == gr  :                        
                        if self.group != 'None' and self.group != group :
                            print "Do not know how to match trigger ", trig," ", group," ", self.group
                        self.group = group
                        break

        if self.group == 'None' :            
            print trig, " " , self.group
            
                # Try to match a number preceeding the tag in the trigger name
##                mstring = ""
##                for gt in group_tags : 
##                    if gt.count("*") > 0 :
##                        mstring += "|[0-9]+[a-z]*" +gt.rstrip("*")
##                    else :
##                        mstring += "|[0-9]+" + gt
                
##                nmatch = re.compile(mstring.lstrip("|"))
##                nm = nmatch.search(trig.lower())
                
##                #Single/Multi decision
##                if not nm and not multi:
##                    self.Single = True
##                elif nm or multi :
##                    self.Multi = True

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
##        if len(tags_matched_1) > 0 and len(tags_matched_2) > 0 :
##            if len(set(trig_tags) - set(tags_matched_1+tags_matched_2)) == 0 :
##                self.Combined = True

    def MakeSelfCombinedLogic(self, trig, group_tags) :
        # Reset
        self.Combined = False
        
        # Do matching
        trig_tags = MakeTags(trig)
        tags_matched = MatchTags(group_tags, trig_tags)
        
        # Combined logic, match all tags, require a match from each group
        if len(tags_matched) == len(trig_tags) and len(tags_matched) > 1 :
            self.Combined = True

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
    
    for trig in trig_list :
        found = False
        for gr, group_tags in base_group_def.iteritems() :
            Logic = GroupLogic()
            Logic.MakeLogic(trig, group_tags)

            found_veto = 0
            for menu_gr in veto_groups :
                if trig in menu_groups[menu_gr] :
                    found_veto = 1
                    break
            if found_veto == 1 :
                continue        
            #print trig, " "  , gr, " "  , Logic.group, " ", group_tags
            if Logic.group == gr :
                groups.setdefault("BW:"+level+"_" + gr, []).append(trig)
                found = True
                break
        if found == False :
            print "do not have group for ", trig
        
    for name, list in groups.iteritems() :
        list.sort()
        groups[name] = list

    return groups


# Fill the group list with a bunch of rubbish    
def MakeCumulativeGroups(group_dict) :

    cumu_group = dict()
    for name, trigs in group_dict.iteritems() :
        cumu_group[name] = trigs
        if len(trigs) == 1 :
            continue

        accum_trig = []
        for tr in trigs :
            accum_trig.append(tr)
            cumu_group["%s_cumu_%i" %(name, len(accum_trig)-1)] = list(accum_trig);
            
        rev_accum_trig = []
        accum_trig.reverse()
        for trrev in accum_trig : 
            rev_accum_trig.append(trrev)
            cumu_group["%s_cumu_rev_%i" %(name,  len(rev_accum_trig)-1)] = list(rev_accum_trig)
       
    return cumu_group

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

#make groups for each level    
gr_L1 = MakeGroups(tr_L1, "L1")
gr_L2 = MakeGroups(tr_L2, "L2")
gr_EF = MakeGroups(tr_EF, "EF")

#print "L1 groups :", gr_L1
#print "L2 groups :", gr_L2
#print "EF groups :", gr_EF

#if not options.no_cumulative :
#    gr_L1 = MakeCumulativeGroups(gr_L1)
#    gr_L2 = MakeCumulativeGroups(gr_L2)
#    gr_EF = MakeCumulativeGroups(gr_EF)
#
# collect groups into one dictionary
all_groups = dict()
for gr, trigs in gr_L1.iteritems() :
    all_groups[gr] = trigs
for gr, trigs in gr_L2.iteritems() :
    all_groups[gr] = trigs
for gr, trigs in gr_EF.iteritems() :
    all_groups[gr] = trigs



WriteXmlFile(all_groups, options.append_xml)

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

    for gr, trigs in gr_L1.iteritems() :
        print "triggers.clear()"
        for i in trigs :
            print "triggers.push_back(%s);" %i

        print "gmap[%s] = triggers;" %gr

    for gr, trigs in gr_L2.iteritems() :
        print "triggers.clear()"
        for i in trigs :
            print "triggers.push_back(%s);" %i

        print "gmap[%s] = triggers;" %gr

    for gr, trigs in gr_EF.iteritems() :
        print "triggers.clear()"
        for i in trigs :
            print "triggers.push_back(%s);" %i

        print "gmap[%s] = triggers;" %gr

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

