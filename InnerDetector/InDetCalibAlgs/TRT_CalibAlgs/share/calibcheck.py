import os,sys

lines=open(sys.argv[1]).readlines()


boards={};
for line in lines:

    data=line.strip().split()
    
    bkey1='_%i_%i_%i_%i'%(int(data[0]),int(data[1]),int(data[2]),int(data[3]))

    if bkey1 in boards:
        boards[bkey1]['t0arr'].append(float(data[8]))
        boards[bkey1]['oldt0arr'].append(float(data[9]))
        boards[bkey1]['n']=boards[bkey1]['n']+1
        boards[bkey1]['slkey'].append('_%i_%i_%i_%i_%i'%(int(data[0]),int(data[1]),int(data[2]),int(data[6]),int(data[7])))
    else:
        boards[bkey1]={}
        boards[bkey1]['t0']=float(data[19])
        boards[bkey1]['oldt0']=float(data[20])
        boards[bkey1]['t0arr']=[]
        boards[bkey1]['oldt0arr']=[]
        boards[bkey1]['t0arr'].append(float(data[8]))
        boards[bkey1]['oldt0arr'].append(float(data[9]))
        boards[bkey1]['n']=1
        boards[bkey1]['iboard']=9*int(data[2])+int(data[3])
        boards[bkey1]['slkey']=[]
        boards[bkey1]['slkey'].append('_%i_%i_%i_%i_%i'%(int(data[0]),int(data[1]),int(data[2]),int(data[6]),int(data[7])))
    
for board in boards:

    oldt0arr=boards[board]['oldt0arr']
    t0arr=boards[board]['t0arr']

    sumoldt0=0
    for oldt0 in boards[board]['oldt0arr']:
        sumoldt0=sumoldt0+oldt0
        #print oldt0
    print '%10s %5i %8.3f %8.3f %5i %8.3f %4i'%(board,boards[board]['n'],sumoldt0/boards[board]['n'],boards[board]['oldt0'],len(boards[board]['oldt0arr']),boards[board]['t0']-boards[board]['oldt0'],boards[board]['iboard'])
        

    if board=='_-1_1_22_4':
    #if board=='_1_0_23_0':
    #if board=='_1_1_5_4':
    #if board=='_1_1_1_4':
        for istraw,oldt0 in enumerate(boards[board]['oldt0arr']):
            print (boards[board]['t0']-boards[board]['oldt0'])-(boards[board]['t0arr'][istraw]-boards[board]['oldt0arr'][istraw]),(boards[board]['t0arr'][istraw]-boards[board]['oldt0arr'][istraw]),boards[board]['slkey'][istraw],oldt0arr[istraw]
            
        

    
    #print line.strip()
    #print bkey1
