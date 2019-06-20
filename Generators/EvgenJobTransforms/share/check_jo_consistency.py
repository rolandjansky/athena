#!/usr/bin/python

import os, re, string, subprocess, sys
    
def check_consistency(jofile):

    joparts = jofile.split(".")

    officialJO = False
    if joparts[0].startswith("mc") and all(c in string.digits for c in joparts[0][2:]):
        officialJO = True
        ## Check that the JO does not appear to be an old one, since we can't use those
        if int(joparts[0][2:]) < 14:
            print "ERROR: MC14 (or later) job option scripts are needed to work with Generate_tf!"
            print jo + " will not be processed: please rename or otherwise update to a >= MC14 JO."
            sys.exit(1)
        else:
            print "OK: New jobOption file"
        ## Check that there are exactly 3 name parts separated by '.': MCxx, DSID, physicsShort, .py
        if len(joparts) != 3:
            print "ERROR:" + jofile + " name format is wrong: must be of the form MC<xx>.<physicsShort>.py: please rename."
            sys.exit(1)
        else:
            print "OK: name format correct" 

    ## Check the length limit on the physicsShort portion of the filename
    jo_physshortpart = joparts[1]
    if len(jo_physshortpart) > 50:
        print "ERROR: " + jo_physshortpart + " contains a physicsShort field of more than 50 characters: please rename."
        sys.exit(1)
    else:
        print "OK:" + jo_physshortpart + " physicsShort less than 50 characters long"

    jo_physshortparts = jo_physshortpart.split("_")
    ## There must be at least 2 physicsShort sub-parts separated by '_': gens, (tune)+PDF, and process
    if len(jo_physshortparts) < 2:
        print "ERROR: " + jofile + " has too few physicsShort fields separated by '_': should contain <generators>(_<tune+PDF_if_available>)_<process>. Please rename."
        sys.exit(1)
    else:
        print "OK: %s physicsShort parts found" %len(jo_physshortparts)


    # NOTE: Further checks for generators - these need modification to work properly
    Generators = {'Madgraph':'MG', 'Herwigpp':'Hpp', 'Herwig7':'H7', 'EvtGen':'EG', 'Pythia8':'Py8', 'Pythia':'Py6', 'Powheg':'Ph', 'Sherpa':'Sh', 'Alpgen':'Ag', 'ParticleGun':'PG', 'aMcAtNlo':'aMC',
                  'Perugia2012':'p12', 'ParticleDecayer':'PD', 'HepMCAscii':'HepMC', 'MadSpin':'MSp'}
    Generators_abb = list(Generators.values())
    if joparts[0].startswith("mc"): #< if this is an "official" JO
        genpart = jo_physshortparts[0]
        #Check No Generator full name in physicsshort.
        GenFullNameNo = 0
        for Generator, Generator_abb in Generators.items():
            if genpart.find(Generator)!=-1:
                print "ERROR: Generator full name "+Generator+" is found, please use abbreviation "+Generator_abb
                GenFullNameNo += 1
        if GenFullNameNo>0:
            print "ERROR: %s generator full names are found! Please use abbreviation instead." %GenFullNameNo
            sys.exit(1)
        else:
            print "OK: No generator full name is found"

        #Check No unrecognized generator abbreviation in physicsshort.
        generator_list=[]
        i = 0
        while i<len(genpart):
            for step in range(1, len(genpart)+1-i):
                if genpart[i:i+step] in Generators_abb:
                    print "Find "+ genpart[i:i+step]
                    generator_list.append(genpart[i:i+step])
                    i+=step 
                    break
                elif step==len(genpart)-i:
                    print "Error: Not recognised generator abbreviation " + genpart[i:i+step]
                    sys.exit(1)
                
        print "Generator used: ", generator_list

    #further check 

#        del _norm
#        ## Check if the tune/PDF part is needed, and if so whether it's present
#        if not gens_notune(gennames) and len(jo_physshortparts) < 3:
#            print("ERROR: " + jofile + " with generators " + expectedgenpart +
#                       " has too few physicsShort fields separated by '_'." +
#                       " It should contain <generators>_<tune+PDF_<process>. Please rename.")
#            sys.exit(1)


# Main script
#def main():
#    print("\n===> Checking jobOption consistency...\n")
    # Get job options
#    command="git diff-tree --name-only -r HEAD --diff-filter=AMDCRTUXB | grep -E \"mc[0-9]{2}.*.py\""
#    files=os.popen(command).read().strip().split("\n")
#    for file in files:
#        jofile=file.split("/")[-1]
#        print("Modified jO fle: {0} - jO file = {1}".format(file, jofile))
#        check_consistency(jofile)

#if __name__== "__main__":
#    main()
