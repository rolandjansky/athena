
# script to read output from out.txt
# made by Will's edits to JetAlg.cxx


def main():
    print "hi"


def readTowerInfo(infile = 'out.txt', old=False):
    
    towerInfo = {}

    towerInfo['towerNums'] = []
    towerInfo['etas'] = []
    towerInfo['phis'] = []
    towerInfo['ets'] = []
    towerInfo['samplings'] = []
    towerInfo['numindicess'] = []
    towerInfo['indicess'] = []
    towerInfo['layerss'] = []

    
    towerNum = -1

    # old
    # tower 0 with eta, phi, et = -0.05, 0.0490874, 172.977
    #   sampling = 0, 10 SCIndices: 6592, 7552, 7616, 7680, 7744, 11328, 11392, 11456, 11520, 14976, # except with (i) as below

    # new
    # tower 0: eta = -0.05, phi = 0.0490874, deta = 0.1, dphi = 0.0981748, et = 172.977
    #   sampling = 0, 10 SCIndices: 6592 (2), 7552 (0), 7616 (1), 7680 (1), 7744 (2), 11328 (0), 11392 (1), 11456 (1), 11520 (2), 14976 (2), 

    for line in open('out.txt').readlines():
        if line.startswith('tower '):
            if old:
                towerNum = int(line.split()[1])
            else:
                towerNum = int(line.split()[1][:-1])
            towerInfo['towerNums'].append(towerNum)

            if old:
                towerInfo['etas'].append(float(line.split()[7][:-1]))
                towerInfo['phis'].append(float(line.split()[8][:-1]))
                towerInfo['ets'].append(float(line.split()[9]))
            else:
                towerInfo['etas'].append(float(line.split(', ')[0].split()[-1]))
                towerInfo['phis'].append(float(line.split(', ')[1].split()[-1]))
                towerInfo['ets'].append(float(line.split(', ')[-1].split()[-1]))
            
        elif towerNum == -1:
            continue
        elif 'JGTowerReader' in line:
            break

        else:
            towerInfo['samplings'].append(int(line.split()[2][:-1]))
            towerInfo['numindicess'].append(int(line.split()[3]))

            towerInfo['indicess'].append([])
            towerInfo['layerss'].append([])
            segment = line.split('SCIndices: ')[1].rstrip('\n')[:-2]
            for part in segment.split(', '):
                if part == '':
                    continue
                towerInfo['indicess'][-1].append(int(part.split()[0]))
                towerInfo['layerss'][-1].append(int(part.split()[1][1:-1]))

    # tower 6233 with eta, phi, et = 1.65, 2.50346, 815.438
    # sampling = 1, 1 SCIndices: 32601 (5),
    # print towerInfo['towerNums'][-1], towerInfo['etas'][-1], towerInfo['phis'][-1], towerInfo['ets'][-1], towerInfo['samplings'][-1], towerInfo['numindicess'][-1], towerInfo['indicess'][-1]
    
    return towerInfo


def shuffleTowerInfo(towerInfo):

    # towerInfo.keys() == ['towerNums', 'layerss', 'indicess', 'ets', 'etas', 'samplings', 'phis', 'numindicess']
    
    shuffledTowerInfo = []

    for i in range(len(towerInfo['towerNums'])):
        shuffledTowerInfo.append({
            'num': towerInfo['towerNums'][i],
            'eta': towerInfo['etas'][i],
            'phi': towerInfo['phis'][i],
            'et': towerInfo['ets'][i],
            'sampling': towerInfo['samplings'][i],
            'numindices': towerInfo['numindicess'][i],
            'indices': towerInfo['indicess'][i],
            'layers': towerInfo['layerss'][i],
            })
    
    return shuffledTowerInfo


if __name__ == '__main__':
    main()
