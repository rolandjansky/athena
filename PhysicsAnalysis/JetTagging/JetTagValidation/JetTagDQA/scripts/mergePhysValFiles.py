#!/usr/bin/env python

#----------------------------------------------------------------------
#stand-alone script to merge specific directories of NTUP_PHYSVAL files 
#author: philipp.mogg@cern.ch
#16 May 2016
#updates: judith.hoefer@cern.ch
#Nov 2020
#----------------------------------------------------------------------

from __future__ import print_function
import getopt,os,sys,glob,argparse,ROOT,time
from ROOT import gDirectory

start = time.process_time()

# define the categories (= sub-folders)
categories = ['jet',
              'tracks',
              'SV',
              'tagger_IP3D',
              'tagger_RNNIP',
              'tagger_SV1',
              'tagger_JetFitter',
              'tagger_DL1',
              'tagger_DL1d',
              'tagger_DL1r',
              'old_taggers',
              #'tagger_IP2D',
              #'tagger_IP3DSV1',
              #'tagger_MV2c10',
             ]

# name of the folder into which plots in no other category are sorted
restCategory = 'other' 

# make subcategories
categories_with_subcategories_type_1 = ['SV', 'tracks']

sub_categories_type_1 = [ '_incl',
                   '_b',
                   '_c',
                   '_l',
                   '_muon',
                 ]

categories_with_subcategories_type_2 = ['tagger_IP3D', 'tagger_RNNIP', 'tagger_SV1', 'tagger_JetFitter', 'tagger_DL1', 'tagger_DL1d', 'tagger_DL1r']

sub_categories_type_2 = [ '_pt_ttbar',
                   '_pt_Zprime',
                   '_Lxy'
                 ]

categories_with_subcategories_type_3 = ['old_taggers']

sub_categories_type_3 = [ '_IP2D',
                   '_IP3DSV1',
                   '_MV2c10',
                 ]

categories_with_subcategories_type_4 = ['jet']

sub_categories_type_4 = [ 'jet']

# define the jet containers
jetcontainers = ['AntiKt4EMTopoJets',
                 'AntiKt4EMPFlowJets',
                 'AntiKtVR30Rmax4Rmin02PV0TrackJets'
                 ]

# parser arguments
parser = argparse.ArgumentParser(description='Merge specific folder(s) in root files.')
parser.add_argument("-i", "--input", help="path to the folder holding the samples (default: ./)", default=os.getcwd())
parser.add_argument("-o", "--output", help="path for the output (default: ./merge.root", default=os.getcwd()+"/merge.root")
parser.add_argument("-d", "--dir", nargs='+', help="ROOT directory to be merged, multiple arguments are possible (default: Summary)", default=["Summary"])
parser.add_argument("-p", "--pattern", help='pattern of files to merge (default: "*PHYSVAL*")', default="*PHYSVAL*")
args = parser.parse_args()
folder = os.path.abspath(args.input)
mergeDirs = args.dir
origDir = os.getcwd()
out = args.output
pattern = args.pattern
output_file = os.path.abspath(args.output)
os.chdir(folder)

files = glob.glob(folder + "/" + pattern)

# open the output file
f = ROOT.TFile(output_file, "recreate")
folder = os.getcwd()
f2 = ROOT.TFile(files[0])

# check input files
print("Target file: " + output_file)
for infile in files:
    print("Found input file: " + infile)
    if os.path.samefile(output_file, infile):
        print("Please make sure that the output file is not part of the input files! Stopping.")
        quit()


# -- define a method to merge --

errors = []
tagfolders = {}

def mergeFolder(path):
    # check input
    print("Merging folder " + path)
    d = f2.Get(path)
    if not d:
        error = "ERROR: Cannot find directory " + path + ". Omitting."
        print(error)
        errors.append(error)
        return
    dirlist = d.GetListOfKeys()
    
    # create folders
    for jetcont in jetcontainers:
        if jetcont in path:
            currentdir = gDirectory.GetPath()
            gDirectory.cd(path)
            # create /restCategory
            print("Create directory " + path + "/other_histograms/histos")
            tagfolders[path+"/"+restCategory] = f.mkdir(path+"/other_histograms/histos")
            # create category folders
            for category in categories:
                print("Create directory " + path + "/" + category)
                tagfolders[path+"/"+category] = f.mkdir(path+"/"+category)
                if category in categories_with_subcategories_type_1:
                    for sub_category in sub_categories_type_1:
                        tagfolders[path+"/"+category+"/"+sub_category] = f.mkdir(path+"/"+category+"/"+sub_category)
                elif category in categories_with_subcategories_type_2:
                    for sub_category in sub_categories_type_2:
                        tagfolders[path+"/"+category+"/"+sub_category] = f.mkdir(path+"/"+category+"/"+sub_category)
                elif category in categories_with_subcategories_type_3:
                    for sub_category in sub_categories_type_3:
                        tagfolders[path+"/"+category+"/"+sub_category] = f.mkdir(path+"/"+category+"/"+sub_category)
                elif category in categories_with_subcategories_type_4:
                    for sub_category in sub_categories_type_4:
                        tagfolders[path+"/"+category+"/"+sub_category] = f.mkdir(path+"/"+category+"/"+sub_category)
                tagfolders[path+"/"+category+"/"+restCategory] = f.mkdir(path+"/"+category+"/"+restCategory)
            gDirectory.cd(currentdir)

    for subdir in dirlist:
        obj = subdir.ReadObj()

        if obj.IsA().InheritsFrom(ROOT.TH1.Class()):
            print("Now merging "+obj.GetName())
            h1 = obj
            hpath = d.GetPath()
            hname = hpath[hpath.find(":")+2:]+"/"+obj.GetName()
            print("Path: "+hname)
                        
            for tup in files:
                if tup==files[0]: continue
                nextfile = ROOT.TFile(tup)
                h2 = nextfile.Get(hname)
                if not h2:
                    error = "ERROR: Cannot find " + hname + " in file " + tup + ". Omitting."
                    print(error)
                    errors.append(error)
                    continue
                h1.Add(h2)
            if tagfolders:
                for category in reversed(categories):
                    # check for the categories
                    if ("_"+category+"_") in obj.GetName(): 
                        print("Category: " + category)
                        subfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category)
                        subfolder.cd()
                        # apply the sub-categories here
                        should_be_in_a_subcategory = False
                        is_in_subcategory = False
                        if category in categories_with_subcategories_type_1:
                            should_be_in_a_subcategory = True
                            for sub_category in reversed(sub_categories_type_1):
                                if(sub_category in obj.GetName()):
                                    is_in_subcategory = True
                                    subsubfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category+"/"+sub_category)
                                    subsubfolder.cd()
                                    break
                        elif category in categories_with_subcategories_type_2:
                            should_be_in_a_subcategory = True
                            for sub_category in reversed(sub_categories_type_2):
                                if(sub_category in obj.GetName()):
                                    is_in_subcategory = True
                                    subsubfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category+"/"+sub_category)
                                    subsubfolder.cd()
                                    break
                        elif category in categories_with_subcategories_type_3:
                            should_be_in_a_subcategory = True
                            for sub_category in reversed(sub_categories_type_3):
                                if(sub_category in obj.GetName()):
                                    is_in_subcategory = True
                                    subsubfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category+"/"+sub_category)
                                    subsubfolder.cd()
                                    break
                        elif category in categories_with_subcategories_type_4:
                            should_be_in_a_subcategory = True
                            for sub_category in reversed(sub_categories_type_4):
                                if(sub_category in obj.GetName()):
                                    is_in_subcategory = True
                                    subsubfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category+"/"+sub_category)
                                    subsubfolder.cd()
                                    break
                        if should_be_in_a_subcategory and not is_in_subcategory:
                            rest_subsubfolder = f.Get(hpath[hpath.find(":")+2:]+"/"+category+"/"+restCategory)
                            rest_subsubfolder.cd()

                        break
                    # the rest goes into the restCategory
                    else:
                        subfolder = f.Get(hpath[hpath.find(":")+2:]+"/other_histograms/histos")
                        subfolder.cd()

            print(gDirectory.GetPath())
            h1.Write()
                            
        if obj.IsA().InheritsFrom(ROOT.TDirectory.Class()):
            print("Found subdirectory "+obj.GetName())
            hpath = obj.GetPath()
            subfolder = f.mkdir(hpath[hpath.find(":")+2:],obj.GetTitle())
            print("Created new output directory " + hpath[hpath.find(":")+2:])
            subfolder.cd()
            mergeFolder(hpath[hpath.find(":")+2:])


# merge the folders
for mergeDir in mergeDirs:
    newfolder = f.mkdir(mergeDir,mergeDir)
    ROOT.TH1.AddDirectory(False)
    mergeFolder(mergeDir)

# close file and report
f.Close()
if len(errors)>0:
    print("Summary of all errors:")
    for phrase in errors:
        print(phrase)

end = time.process_time()
print("Wall time used: %s sec" % (end - start))
