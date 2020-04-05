#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
include("EvgenProdTools/merge_lhe_files.py")
include("EvgenProdTools/find_unique_file.py")

myInputFiles = runArgs.inputGeneratorFile
genInputFiles = myInputFiles.split(',')
numberOfFiles = len(genInputFiles)

if numberOfFiles > 1:
    allFiles = []
    for file in genInputFiles:
              printfunc ("input file name ",file)
#             Since we can have multiple files from the same task, inputroot must include more of the filename
#             to make it unique
              if ".tar" in os.path.basename(file):
                  inputroot = os.path.basename(file).split(".tar.")[0]
              else:
                input0 = os.path.basename(file).split("._")[0]
                input1 = (os.path.basename(file).split("._")[1]).split(".")[0]
                inputroot = input0+"._"+input1
#              printfunc ("inputroot ",inputroot)
              realEventsFile = find_unique_file('*%s.*ev*ts' % inputroot)
#             The only input format where merging is permitted is LHE
              with open(realEventsFile, 'r') as f:
                 first_line = f.readline()
                 if(not ("LesHouche" in first_line)):
                    raise RuntimeError("%s is NOT a LesHouche file" % realEventsFile)
                 allFiles.append(realEventsFile)
                 
    printfunc (("Found more than one LHE file: {}".format(numberOfFiles)))
#    my_lhe_file = "merged_lhef.events"
# skeleton.GENtoEVGEN splits the file name on "._" (in rel. 20.7.9.9.6,MCProd,
# so insert this in the merged file name - to make it run also for this release)
    my_lhe_file = "merged_lhef._0.events"
    merge_lhe_files(allFiles, my_lhe_file )
    printfunc (("Using uncompressed LHE file '{}' as inputGeneratorFile".format(my_lhe_file)))
    runArgs.inputGeneratorFile = my_lhe_file

    
