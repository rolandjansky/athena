
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
#
# The calorimeters are symmetric, such that maps need only cover towers, seeds and jets centred on the positive quadrant
# If onlyPositiveQuadrant is True then jets will only be written in (+,+). Taking n as the number of towers in a quadrant:
# - 0->n-1 is (+,+)
# - n->2n-1 is (+,-), with an addition of n corresponding to phi -> -phi
# - 2n->3n-1 is (-,+), with an addition of 2n corresponding to eta -> -eta
# - 3n->4n-1 is (-,-), with an addition of 3n corresponding to phi -> -phi and eta -> -eta
#
# Furthermore, once you get to central eta, nothing changes, so the c++ can just step in eta jumps of 0.1
# If skipCentral is True, then after three values of eta including only sampling 1 and 0 the map will stop
# 
# ===============================

import os, sys
from copy import deepcopy
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
    writeToTextFile = True
    # textFileName = 'jFexJet_towerMap_2019-01-08.txt'
    # textFileName = 'jFexJet_towerMap_2019-01-29.txt'
    # textFileName = 'jFexJet_towerMap_2019-02-07.txt'
    # textFileName = 'jFexJet_towerMap_2019-02-08.txt'
    # textFileName = 'jFexJet_towerMap_2019-02-12.txt'
    # textFileName = 'jFexJet_towerMap_2019-02-12_notruncation.txt'
    # textFileName = 'jFexJet_towerMap_2019-02-15.txt'
    textFileName = 'jFexJet_towerMap_2019-03-13.txt'
    # textFileName = 'jFexJet_towerMap_test.txt'

    # some parameters
    # seeds
    seedShape = 'rectangular'
    seedSamplings = [0,2] # towers that seeds and jets will be centred on
    seedSize = [0.3,0.3]
    seedIncludeExact = False
    # local max
    seed_localmax_dphi = 0.26
    seed_localmax_deta = 0.26
    # jets
    jetShape = 'circular'
    jetRadius = 0.39
    jetIncludeExact = False

    # don't write superfluous information
    # onlyPositiveQuadrant = True
    # skipCentral = True
    onlyPositiveQuadrant = False
    skipCentral = False
    # eta=2.45 is the last "full" barrel eta (i.e. 0.1 spacing in phi)
    # go down to 2.45-jetRadius-0.05-0.1
    centralBoundary = 2.45 - jetRadius - 0.05 - 0.1

    
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

        if onlyPositiveQuadrant:
            outfile.write('# only positive quadrant:\n')
            outfile.write("""# The calorimeters are symmetric, such that this map only covers towers, seeds and jets centred on the positive quadrant
# Taking n as the number of towers in a quadrant:
# - 0->n-1 is (+,+)
# - n->2n-1 is (+,-), with an addition of n corresponding to phi -> -phi
# - 2n->3n-1 is (-,+), with an addition of 2n corresponding to eta -> -eta
# - 3n->4n-1 is (-,-), with an addition of 3n corresponding to phi -> -phi and eta -> -eta
#
""")

        if skipCentral:
            outfile.write('# skipping central etas (i.e. eta < ' + str(centralBoundary) + ')\n')
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
    towers_allSamplings = []
    for t in range(len(allTowers)):
        # only consider an eta-phi rectangle if requested
        if phiRange != None:
            if allTowers[t]['phi'] < phiRange[0] or allTowers[t]['phi'] > phiRange[1]:
                continue
        if etaRange != None:
            if allTowers[t]['eta'] < etaRange[0] or allTowers[t]['eta'] > etaRange[1]:
                continue
        towers_allSamplings.append(allTowers[t])

    towers_seedSamplings = []
    seedLayerTowerIndices = getTowerIndicesInLayers(towers_allSamplings, samplings = seedSamplings, layers = None)
    for t in seedLayerTowerIndices:
        towers_seedSamplings.append(towers_allSamplings[t])
            
    # reshuffle the towers - keep only those in positive quadrant, renumber the others
    # the aim: iterate over positive quadrants, make seeds, jets, local maxes from all quadrants
    if onlyPositiveQuadrant:
        # make set of towers with only positive eta, phi
        towers_allSamplings_orig = deepcopy(towers_allSamplings)
        towers_allSamplings = []
        count = 0
        for t in range(len(towers_allSamplings_orig)):
            if towers_allSamplings_orig[t]['eta'] < 0 or towers_allSamplings_orig[t]['phi'] < 0:
                continue
            tower = deepcopy(towers_allSamplings_orig[t])
            tower['num'] = count
            towers_allSamplings.append(tower)
            count += 1
        count = 0

        towers_seedSamplings_orig = deepcopy(towers_seedSamplings)
        towers_seedSamplings = []
        for t in range(len(towers_seedSamplings_orig)):
            if towers_seedSamplings_orig[t]['eta'] < 0 or towers_seedSamplings_orig[t]['phi'] < 0:
                continue
            tower = deepcopy(towers_seedSamplings_orig[t])
            tower['num'] = count
            towers_seedSamplings.append(tower)
            count += 1

            
        # mirror these in phi
        towers_allSamplings_allQuadrants = deepcopy(towers_allSamplings)
        count = len(towers_allSamplings)
        for t in range(len(towers_allSamplings)):
            tower = deepcopy(towers_allSamplings[t])
            tower['phi'] = tower['phi']*-1
            tower['num'] = count
            towers_allSamplings_allQuadrants.append(tower)
            count += 1
            
        towers_seedSamplings_allQuadrants = deepcopy(towers_seedSamplings)
        count = len(towers_seedSamplings)
        for t in range(len(towers_seedSamplings)):
            tower = deepcopy(towers_seedSamplings[t])
            tower['phi'] = tower['phi']*-1
            tower['num'] = count
            towers_seedSamplings_allQuadrants.append(tower)
            count += 1
            
        # mirror these in eta
        count = len(towers_allSamplings_allQuadrants)
        for t in range(len(towers_allSamplings_allQuadrants)):
            tower = deepcopy(towers_allSamplings_allQuadrants[t])
            tower['eta'] = tower['eta']*-1
            tower['num'] = count
            towers_allSamplings_allQuadrants.append(tower)
            count += 1
        count = len(towers_seedSamplings_allQuadrants)
        for t in range(len(towers_seedSamplings_allQuadrants)):
            tower = deepcopy(towers_seedSamplings_allQuadrants[t])
            tower['eta'] = tower['eta']*-1
            tower['num'] = count
            towers_seedSamplings_allQuadrants.append(tower)
            count += 1
            
        # some checks
        if len(towers_allSamplings_orig) != len(towers_allSamplings_allQuadrants):
            sys.exit("missed / gained some towers in the reshuffling (all): " + str(len(towers_allSamplings_orig)) + " " + str(len(towers_allSamplings_allQuadrants)))
        if 4*len(towers_allSamplings) != len(towers_allSamplings_allQuadrants):
            sys.exit("missed / gained some towers in the duplication (all): " + str(4*len(towers_allSamplings)) + " " + str(len(towers_allSamplings_allQuadrants)))
        if len(towers_seedSamplings_orig) != len(towers_seedSamplings_allQuadrants):
            sys.exit("missed / gained some towers in the reshuffling (seed): " + str(len(towers_seedSamplings_orig)) + " " + str(len(towers_seedSamplings_allQuadrants)))
        if 4*len(towers_seedSamplings) != len(towers_seedSamplings_allQuadrants):
            sys.exit("missed / gained some towers in the duplication (seed): " + str(4*len(towers_seedSamplings)) + " " + str(len(towers_seedSamplings_allQuadrants)))

    else:
        towers_allSamplings_allQuadrants = deepcopy(towers_allSamplings)
        towers_seedSamplings_allQuadrants = deepcopy(towers_seedSamplings)

    totalNumTowers = len(towers_allSamplings_allQuadrants)
    totalNumSeeds = len(towers_seedSamplings_allQuadrants)
    totalNumJets = len(towers_seedSamplings_allQuadrants)
    
    print len(towers_seedSamplings), len(towers_seedSamplings_allQuadrants), len(towers_allSamplings), len(towers_allSamplings_allQuadrants)
    
    # print towers_allSamplings_orig[123]
    # print towers_allSamplings[123]
    # print towers_allSamplings_allQuadrants[123]
    # print towers_allSamplings_allQuadrants[123+len(towers_allSamplings)]
    # print towers_allSamplings_allQuadrants[123+2*len(towers_allSamplings)]
    # print towers_allSamplings_allQuadrants[123+3*len(towers_allSamplings)]
            
        
    # iterate over all remaining towers, fill jet and seed lists
    jetList = []
    seedList = []
    
    for t in range(len(towers_seedSamplings)):

        if skipCentral:
            if towers_seedSamplings[t]['eta'] < centralBoundary:
                continue
        
        # for now, just does centering on tower centres

        # seeds
        thisSeedList = {}
        thisSeedList['eta'] = towers_seedSamplings[t]['eta']
        thisSeedList['phi'] = towers_seedSamplings[t]['phi']

        if seedShape == 'rectangular':
            thisSeedList['towers'] = towersInRectangle(towers_allSamplings_allQuadrants, towers_seedSamplings[t], seedSize[0], seedSize[1], includeExact = seedIncludeExact)
        else:
            sys.exit("I don't know what to do with seedShape = " + seedShape)
        seedList.append(thisSeedList)

        
        # jets
        thisJetList = {}
        thisJetList['eta'] = towers_seedSamplings[t]['eta']
        thisJetList['phi'] = towers_seedSamplings[t]['phi']

        # which seed to use? If both centred in the same place, as here, then use the latest seed
        thisJetList['seed'] = len(seedList)-1

        # all towers with radius < jetRadius
        # this includes some you mightn't want - better not to bank on exact precision and round down a little
        if jetShape == 'circular':
            thisJetList['towers'] = towersInRadius(towers_allSamplings_allQuadrants, towers_seedSamplings[t], jetRadius, includeExact = jetIncludeExact)
        else:
            sys.exit("I don't know what to do with jetShape = " + jetShape)
        jetList.append(thisJetList)

        
        # break from loop if testing only a certain number
        if nJets != None:
            if nJets > 0:
                if len(jetList) > nJets:
                    break

        if len(jetList) % 100 == 0:
            print 'done', len(jetList), 'of', len(towers_seedSamplings), 'jets'
    print 'done', len(jetList), 'of', len(towers_seedSamplings), 'jets'

    
    # now do the seed local maxima
    # local max: this seed should be a maximum out of all the surrounding seeds
    print 'doing seed local maxima'
    for i in range(len(seedList)):
        eta = jetList[i]['eta']
        phi = jetList[i]['phi']
        seedList[i]['localMaxSeeds'] = []
        # iterate over all positions, not just +ve quadrant
        for j in range(len(towers_seedSamplings_allQuadrants)):
            if abs( eta - towers_seedSamplings_allQuadrants[j]['eta'] ) > seed_localmax_deta:
                continue
            if abs( deltaPhi(phi, towers_seedSamplings_allQuadrants[j]['phi']) ) > seed_localmax_dphi:
                continue
            seedList[i]['localMaxSeeds'].append(j)
            
        if i in seedList[i]['localMaxSeeds']:
            seedList[i]['localMaxSeeds'].remove(i)
    print 'done seed local maxima'


    # write them out to the already opened outfile
    if writeToTextFile:
        print 'writing to outfile', textFileName

        numTowersInMap = 0
        for t in range(len(towers_allSamplings)):
            if skipCentral:
                if towers_allSamplings[t]['eta'] < centralBoundary:
                    continue
            numTowersInMap += 1

        # towers
        outfile.write('# === start towers ===\n')
        outfile.write('# total towers: ' + str(totalNumTowers) + '\n')
        outfile.write('# towers in this map: ' + str(numTowersInMap) + '\n')
        outfile.write('# number eta phi\n')
        outfile.write('#     sampling\n')
        outfile.write('#     supercell layers\n')
        outfile.write('#\n')
        for t in range(len(towers_allSamplings)):
            if skipCentral:
                if towers_allSamplings[t]['eta'] < centralBoundary:
                    continue
            outfile.write('{0} {1} {2}\n'.format(t, towers_allSamplings[t]['eta'], towers_allSamplings[t]['phi']))
            outfile.write('    ' + str(towers_allSamplings[t]['sampling'])+'\n')
            outfile.write('    ' + ' '.join([str(l) for l in towers_allSamplings[t]['layers']])+'\n')
        outfile.write('# === end towers ===\n')

        # seeds
        outfile.write('#\n')
        outfile.write('# === start seeds ===\n')
        outfile.write('# total seeds: ' + str(totalNumSeeds) + '\n')
        outfile.write('# seeds in this map: ' + str(len(seedList)) + '\n')
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
        outfile.write('# total jets: ' + str(totalNumJets) + '\n')
        outfile.write('# jets in this map: ' + str(len(jetList)) + '\n')
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
        plotJFexJets(towers_allSamplings, seedList, jetList, plotDir)


                
    
def getTowerIndicesInLayers(towers, samplings = None, layers = None):
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
    
    
def towersInRectangle(towers, thisTower, eta_size, phi_size, includeExact = True):

    goodTowers = []

    for t in range(len(towers)):
        delta_eta = abs(towers[t]['eta'] - thisTower['eta'])
        delta_phi = abs(deltaPhi(towers[t]['phi'], thisTower['phi']))
        
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
        # print thisTower['eta'], thisTower['phi'], towers[t]['eta'], towers[t]['phi']

    return goodTowers


def towersInRadius(towers, thisTower, radius, includeExact = True):

    goodTowers = []

    for t in range(len(towers)):

        delta_eta = abs(towers[t]['eta'] - thisTower['eta'])
        delta_phi = abs(deltaPhi(towers[t]['phi'], thisTower['phi']))
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


