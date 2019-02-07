
import os, sys

def main():

    # read jet list
    textFileName = 'jFexJet_towerMap_2019-01-08.txt'
    # textFileName = 'jFexJet_towerMap_test.txt'
    fileInfo, towerList, seedList, jetList = readTextFile(textFileName)
    print "I opened", textFileName, "which has the following information:"
    print '\n'.join(fileInfo)
        
    # plot
    plotDir = 'plots_test2'
    plotJFexJets(towerList, seedList, jetList, plotDir, nJetsToPlot=10)
    
    

def readTextFile(textFileName):
    fileInfo = []
    towerList = []
    seedList = []
    jetList = []
    metadataEnded = False
    inTowerBlock = False
    inSeedBlock = False
    inJetBlock = False
    lines = open(textFileName).readlines()
    sublinecount = -1
    
    for line in lines:
        if line.startswith('#'):
            if '=== end metadata ===' in line:
                metadataEnded = True
            if not metadataEnded:
                fileInfo.append(line.rstrip('\n'))
                
            if '=== start towers ===' in line:
                inTowerBlock = True
            if '=== end towers ===' in line:
                inTowerBlock = False
            if '=== start seeds ===' in line:
                inSeedBlock = True
            if '=== end seeds ===' in line:
                inSeedBlock = False
            if '=== start jets ===' in line:
                inJetBlock = True
            if '=== end jets ===' in line:
                inJetBlock = False

            continue
        
        if line.rstrip('\n') == '':
            continue

        
        if inTowerBlock:
            # number eta phi
            #     sampling
            #     supercell layers

            if not line.startswith(' '):
                sublinecount = 0
                towerNum = int(line.split()[0])
                if len(towerList) !=  towerNum:
                    sys.exit('some tower entries have been magicked out of nowhere / gone missing - have ' + str(len(towerList)) + ' and expect ' + str(towerNum))

                towerList.append({
                    'eta': float(line.split()[1]),
                    'phi': float(line.rstrip('\n').split()[2])
                })

            else:
                if sublinecount <= 0:
                    print "sublinecount value is", sublinecount, " but should be <= 0"
                    sys.exit()

                if sublinecount == 1:
                    towerList[-1]['sampling'] = int(line.lstrip().rstrip('\n'))
                elif sublinecount == 2:
                    towerList[-1]['layers'] = [int(entry) for entry in line.lstrip().rstrip('\n').split()]
                else:
                    print "tower sublinecount value is", sublinecount, " but should be in [1,2]"
                    sys.exit()

        
        if inSeedBlock:
            # number eta phi
            #     towers
            #     local max seeds

            if not line.startswith(' '):
                sublinecount = 0
                seedNum = int(line.split()[0])
                if len(seedList) !=  seedNum:
                    sys.exit('some seed entries have been magicked out of nowhere / gone missing - have ' + str(len(seedList)) + ' and expect ' + str(seedNum))

                seedList.append({
                    'eta': float(line.split()[1]),
                    'phi': float(line.rstrip('\n').split()[2])
                })

            else:
                if sublinecount <= 0:
                    print "sublinecount value is", sublinecount, " but should be <= 0"
                    sys.exit()

                if sublinecount == 1:
                    seedList[-1]['towers'] = [int(entry) for entry in line.lstrip().rstrip('\n').split()]
                elif sublinecount == 2:
                    seedList[-1]['localMaxSeeds'] = [int(entry) for entry in line.lstrip().rstrip('\n').split()]
                else:
                    print "seed sublinecount value is", sublinecount, " but should be in [1,2]"
                    sys.exit()

        
        if inJetBlock:            
            # number eta phi
            #     seed
            #     towers

            if not line.startswith(' '):
                sublinecount = 0
                jetNum = int(line.split()[0])
                if len(jetList) !=  jetNum:
                    sys.exit('some jet entries have been magicked out of nowhere / gone missing - have ' + str(len(jetList)) + ' and expect ' + str(jetNum))

                jetList.append({
                    'eta': float(line.split()[1]),
                    'phi': float(line.rstrip('\n').split()[2])
                })

            else:
                if sublinecount <= 0:
                    print "jet sublinecount value is", sublinecount, " but should be <= 0"
                    sys.exit()

                if sublinecount == 1:
                    jetList[-1]['seed'] = int(line.lstrip().rstrip('\n'))
                elif sublinecount == 2:
                    jetList[-1]['towers'] = [int(entry) for entry in line.lstrip().rstrip('\n').split()]
                else:
                    print "sublinecount value is", sublinecount, " but should be in [1,2]"
                    sys.exit()

        sublinecount += 1

            
    return fileInfo, towerList, seedList, jetList



def plotJFexJets(towerList, seedList, jetList, plotDir, nJetsToPlot = 99999999):

    if not os.path.isdir(plotDir):
        os.mkdir(plotDir)
    else:
        print 'plot directory', plotDir, 'already exists, things will be overwritten'
        yn = raw_input('do you want to continue? y/n: ')
        if yn not in ['y','Y']:
            sys.exit()

    print 'I am going to plot', min(nJetsToPlot, len(jetList)), 'of', len(jetList), 'jets'
    print 'if the input jet list is incomplete then the local maxima may not be correct'
            
    import matplotlib.pyplot as plt
    import numpy as np

    # also want a dictionary / something of the edges and layer of each tower for visualisation purposes
    
    fig, ax = plt.subplots() #figsize=(16, 9))

    tower_etas = []
    tower_phis = []
    for t in range(len(towerList)):
        if towerList[t]['sampling'] != 1:
            tower_etas.append(towerList[t]['eta'])
            tower_phis.append(towerList[t]['phi'])

    ax.plot( tower_etas, tower_phis, '.', markersize=1, color='gray' )
    ax.set_ylim(-3.2,3.2)
    ax.set_xlabel('$\eta$')
    ax.set_ylabel('$\phi$')

    # now plot each seed and jet in turn
    nPlots = 0
    for j in range(len(jetList)):
        nPlots += 1
        if nPlots > nJetsToPlot:
            break
        
        # jet
        jet_etas = []
        jet_phis = []
        for t in jetList[j]['towers']:
            jet_etas.append(towerList[t]['eta'])
            jet_phis.append(towerList[t]['phi'])
        jet, = ax.plot( jet_etas, jet_phis, '.', markersize=2, color='blue')

        # seed
        seed_etas = []
        seed_phis = []
        seed_index = jetList[j]['seed']
        for t in seedList[seed_index]['towers']:
            seed_etas.append(towerList[t]['eta'])
            seed_phis.append(towerList[t]['phi'])
        seed, = ax.plot( seed_etas, seed_phis, '.', markersize=1, color='red' )

        # local max
        lm_etas = []
        lm_phis = []
        for s in seedList[seed_index]['localMaxSeeds']: # this is a list of the jet indices whos seeds this seed needs to be bigger than
            lm_etas.append(seedList[s]['eta'])
            lm_phis.append(seedList[s]['phi'])
        localmax, = ax.plot( lm_etas, lm_phis, '.', markersize=0.5, color='green' )
        
        
        # centre
        centre, = ax.plot( jetList[j]['eta'], jetList[j]['phi'], 'x', markersize=2, color='black' )

        plt.savefig(plotDir+'/jet_'+str(j)+'.png', bbox_inches="tight", dpi=200)

        jet.remove()
        seed.remove()
        localmax.remove()
        centre.remove()

        if j % 10 == 0:
            print "plotted", j, 'jets'

    print "plotted", j, 'jets'

if __name__ == '__main__':
    main()

