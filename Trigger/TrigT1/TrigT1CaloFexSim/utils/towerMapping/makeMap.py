
# ===============================
# Will Kalderon, 06/01/19
# This script makes a map of jTowers to be used for jFex jets and seeds
# The map is read in by TrigT1CaloFexSim's JGTowerReader and used to build jets
# The map consists of:
# For each tower:
#     N eta phi [detector_eta detector_phi]
#         sampling
#         cell layers
# For each seed:
#     N eta phi [detector_eta detector_phi]
#         Tower numbers in seed
#         Seed numbers to compare for local max
# For each jet:
#     N eta phi [detector_eta detector_phi]
#         Seed number
#         Towers numbers in jet
# ===============================

import os, sys
from datetime import datetime
from readTowerInfo import readTowerInfo, shuffleTowerInfo
from plotMap import plotJFexJets

def main():


    # run over what (all, or just some for testing)
    nJets = -1    
    phiRange = None
    etaRange = None
    # phiRange = [0.09,0.18]
    # etaRange = [0.09,0.18]

    # what to output
    makePlots = False
    plotDir = 'plots'
    plotDir = 'plots_test'
    writeToTextFile = True
    # textFileName = 'jFexJet_towerMap_2019-01-08.txt'
    textFileName = 'jFexJet_towerMap_2019-01-29.txt'
    # textFileName = 'jFexJet_towerMap_test.txt'

    # some parameters
    # seeds
    seedShape = 'rectangular'
    seedSamplings = [0,2]
    seedSize = [0.3,0.3]
    seedIncludeExact = False
    # local max
    seed_localmax_dphi = 0.39
    seed_localmax_deta = 0.39
    # jets
    jetShape = 'circular'
    jetRadius = 0.39
    jetIncludeExact = False


    # attempt to open output file
    if writeToTextFile:
        if os.path.isfile(textFileName):
            print 'the file', textFileName, 'already exists, last modified on', datetime.strftime(datetime.fromtimestamp(os.path.getmtime(textFileName)), '%Y-%m-%d %H:%M')
            yn = raw_input('do you want to continue and erase it? y/n: ')
            if yn not in ['y','Y']:
                sys.exit()
        outfile = open(textFileName,'w')       

    # write out some info
    if writeToTextFile:
        outfile.write('# map created by ' + os.environ['USER'] + ' on ' + datetime.strftime(datetime.now(), '%Y-%m-%d %H:%M') + '\n')
        outfile.write('#\n')
        
        # what did I run over
        outfile.write('# ran over ' + str(nJets).replace('-1','all') + ' jets with eta range ' + str(etaRange) + ' and phi range ' + str(phiRange) + '\n')
        outfile.write('#\n')
        
        # seed params
        outfile.write('# seeds: ' + seedShape + ', [eta, phi] = ' + str(seedSize) + ', include exact? ' + str(seedIncludeExact) + '\n')

        # local max params
        outfile.write('# local max: deta = ' + str(seed_localmax_deta) + ', dphi = ' + str(seed_localmax_dphi) + '\n')
        
        # jet params
        outfile.write('# jets: ' + jetShape + ', radius = ' + str(jetRadius) + ', include exact? ' + str(jetIncludeExact) + '\n')
        outfile.write('#\n')

        outfile.write('# === end metadata ===\n')
        outfile.write('#\n')
                
    # read in all the towers from a run on an example file
    towerInfo = readTowerInfo('towerDump.txt')
    allTowers = shuffleTowerInfo(towerInfo)


    # Cut down towers to iterate over for jet/seed centers
    towers = []
    for t in range(len(allTowers)):
        # towers in the central region have two entries: sampling==0 and sampling==1
        if allTowers[t]['sampling'] == 1:
            continue
        # only consider an eta-phi rectangle if requested
        if phiRange != None:
            if allTowers[t]['phi'] < phiRange[0] or allTowers[t]['phi'] > phiRange[1]:
                continue
        if etaRange != None:
            if allTowers[t]['eta'] < etaRange[0] or allTowers[t]['eta'] > etaRange[1]:
                continue
        towers.append(allTowers[t])
        

    # iterate over all remaining towers, fill jet and seed lists
    jetList = []
    seedList = []
    
    for t in range(len(towers)):

        # for now, just does centering on tower centres

        # seeds
        thisSeedList = {}
        thisSeedList['eta'] = towers[t]['eta']
        thisSeedList['phi'] = towers[t]['phi']

        # layer 0, square 3x3 towers in central region
        # - let's say this means 0.3 x 0.3, centred on tower, everywhere
        # - modify to include layer info when present
        seedLayerTowers = getTowersInLayers(towers, samplings = seedSamplings, layers = None)
        if seedShape == 'rectangular':
            thisSeedList['towers'] = towersInRectangle(towers, t, seedLayerTowers, seedSize[0], seedSize[1], includeExact = seedIncludeExact)
        else:
            sys.exit("I don't know what to do with seedShape = " + seedShape)
        seedList.append(thisSeedList)

        
        # jets
        thisJetList = {}
        thisJetList['eta'] = towers[t]['eta']
        thisJetList['phi'] = towers[t]['phi']

        # which seed to use? If both centred in the same place, as here, then use the latest seed
        thisJetList['seed'] = len(seedList)-1

        # all towers with radius < jetRadius
        # this includes some you mightn't want - better not to bank on exact precision and round down a little
        allTowers = range(len(towers))
        if jetShape == 'circular':
            thisJetList['towers'] = towersInRadius(towers, t, allTowers, jetRadius, includeExact = jetIncludeExact)
        else:
            sys.exit("I don't know what to do with jetShape = " + jetShape)
        jetList.append(thisJetList)

        
        # break from loop if testing only a certain number
        if nJets != None:
            if nJets > 0:
                if len(jetList) > nJets:
                    break

        if len(jetList) % 100 == 0:
            print 'done', len(jetList), 'of', len(towers), 'jets'
    print 'done', len(jetList), 'of', len(towers), 'jets'

    
    # now do the seed local maxima
    # local max: this seed should be a maximum out of all the surrounding seeds
    print 'doing seed local maxima'
    for i in range(len(seedList)):
        eta = jetList[i]['eta']
        phi = jetList[i]['phi']
        seedList[i]['localMaxSeeds'] = []
        for j in range(len(seedList)):
            if abs( eta - seedList[j]['eta'] ) > seed_localmax_deta:
                continue
            if abs( deltaPhi(phi, seedList[j]['phi']) ) > seed_localmax_dphi:
                continue
            seedList[i]['localMaxSeeds'].append(j)
            
        if i in seedList[i]['localMaxSeeds']:
            seedList[i]['localMaxSeeds'].remove(i)
    print 'done seed local maxima'
    

    # write them out to the already opened outfile
    if writeToTextFile:
        print 'writing to outfile', textFileName

        # towers
        outfile.write('# === start towers ===\n')
        outfile.write('# number eta phi\n')
        outfile.write('#     sampling\n')
        outfile.write('#     supercell layers\n')
        outfile.write('#\n')
        for t in range(len(towers)):
            outfile.write('{0} {1} {2}\n'.format(t, towers[t]['eta'], towers[t]['phi']))
            outfile.write('    ' + str(towers[t]['sampling'])+'\n')
            outfile.write('    ' + ' '.join([str(l) for l in towers[t]['layers']])+'\n')
        outfile.write('# === end towers ===\n')

        # seeds
        outfile.write('#\n')
        outfile.write('# === start seeds ===\n')
        outfile.write('# number eta phi\n')
        outfile.write('#     towers\n')
        outfile.write('#     local max seeds\n')
        for i in range(len(seedList)):
            thisSeedList = seedList[i]
            outfile.write('{0} {1} {2}\n'.format(i, thisSeedList['eta'], thisSeedList['phi']))
            outfile.write('    ' + ' '.join([str(st) for st in thisSeedList['towers']]) + '\n')
            outfile.write('    ' + ' '.join([str(st) for st in thisSeedList['localMaxSeeds']]) + '\n')
        outfile.write('# === end seeds ===\n')
        
        # jets
        outfile.write('#\n')
        outfile.write('# === start jets ===\n')        
        outfile.write('# number eta phi\n')
        outfile.write('#     seed\n')
        outfile.write('#     towers\n')
        outfile.write('#\n')
        for i in range(len(jetList)):
            thisJetList = jetList[i]
            outfile.write('{0} {1} {2}\n'.format(i, thisJetList['eta'], thisJetList['phi']))
            outfile.write('    '+str(thisJetList['seed'])+'\n')
            outfile.write('    ' + ' '.join([str(st) for st in thisJetList['towers']]) + '\n')
        outfile.write('# === end jets ===\n')
            
    if makePlots:
        plotJFexJets(towers, seedList, jetList, plotDir)


                
    
def getTowersInLayers(towers, samplings = None, layers = None):
    good_indices = []
    for t in range(len(towers)):
        if samplings != None:
            if towers[t]['sampling'] not in samplings:
                continue
        if layers != None:
            sys.exit('not implemented yet')
            # if towers[t]['layer'] not in samplings:
                # continue
        good_indices.append(t)
    return good_indices
    
    
def towersInRectangle(towers, thisTowerIndex, tower_indices, eta_size, phi_size, includeExact = True):

    goodTowers = []

    for t in tower_indices:
        delta_eta = abs(towers[t]['eta'] - towers[thisTowerIndex]['eta'])
        delta_phi = abs(deltaPhi(towers[t]['phi'], towers[thisTowerIndex]['phi']))
        
        if includeExact:
            if delta_eta > eta_size / 2.0:
                continue
            if delta_phi > phi_size / 2.0:
                continue
        else:
            if delta_eta >= eta_size / 2.0:
                continue
            if delta_phi >= phi_size / 2.0:
                continue

        goodTowers.append(t)

        # print delta_eta, delta_phi
        # print towers[thisTowerIndex]['eta'], towers[thisTowerIndex]['phi'], towers[t]['eta'], towers[t]['phi']

    return goodTowers


def towersInRadius(towers, thisTowerIndex, tower_indices, radius, includeExact = True):

    goodTowers = []

    for t in tower_indices:

        delta_eta = abs(towers[t]['eta'] - towers[thisTowerIndex]['eta'])
        delta_phi = abs(deltaPhi(towers[t]['phi'], towers[thisTowerIndex]['phi']))
        delta_R = math.sqrt(delta_eta**2 + delta_phi**2)

        if includeExact:
            if delta_R > radius:
                continue
        else:
            if delta_R >= radius:
                continue
            
        goodTowers.append(t)

    return goodTowers



import math
def deltaPhi(a,b):
    dp = abs(a-b)
    if dp > math.pi:
        dp = 2*math.pi - dp
    return dp


if __name__ == '__main__':
    main()


