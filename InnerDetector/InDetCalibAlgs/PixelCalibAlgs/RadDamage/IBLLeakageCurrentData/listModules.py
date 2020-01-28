#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Write a text file containing all module names
def main():

    modules = []
    # Get the module names

    i=0                                                                                
    for stave in range(1,15):
        staveString = str(stave)

        if stave<10:
            staveString="0"+str(stave)
        for side in ['A','C']:
            for DCSGroup in range(1,5):
                modules += ['LI_S' + str(staveString) + '_' + side + '_M' + str(DCSGroup)]
                
    filename = "modules.txt"
    f = open(filename,"w")
    for m in modules:
        f.write(m+"\n")

    f.close()

if __name__ == "__main__":
    main()
