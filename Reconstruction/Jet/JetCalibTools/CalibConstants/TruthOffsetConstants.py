# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from JetRec.JetAlgConfiguration import editParm
offsetDict  = {}

_knownInput  = ["tower","topo"]

#need to have a parameter which chooses MC or Data

def retrieveTruthConstants(finder='AntiKt', mainparam=0.4, inputType='Topo', calibType='EM', isMC=False, dataType="MC10B"):
            
    #dataType="mc10b"

    keyOOT=""
    keyIT=""

    if 'Topo' in inputType and not 'Tower' in inputType:
        if 'LC' in calibType and not 'EM' in calibType:
            keyOOT = dataType+'_oot_AntiKt'+editParm(mainparam)+'_LCTopo'
            keyIT = dataType+'_it_AntiKt'+editParm(mainparam)+'_LCTopo'
        else: 
            keyOOT = dataType+'_oot_AntiKt'+editParm(mainparam)+'_EMTopo'
            keyIT = dataType+'_it_AntiKt'+editParm(mainparam)+'_EMTopo'
    else:
        return dummyOOTOffset, dummyITOffset

    #keyOOT = dataType+'_oot_truth_offsets'
    #keyIT = dataType+'_it_truth_offsets'
		
    #key = period+'_'+finder+editParm(mainparam)+inputType
        
    return offsetDict[keyOOT] , offsetDict[keyIT] 

offsetDict = {}


def _buildOffsetDict():
    # define the dicts in this function.
    #  ... just a trick to delay the initialisation after the _periodXXX variable are
    #  properly define
    
    offsetDict['MC10B_oot_AntiKt6_LCTopo'] = _oot_MC10B_AntiKt6_LCTopo
    offsetDict['MC10B_it_AntiKt6_LCTopo'] = _it_MC10B_AntiKt6_LCTopo

    offsetDict['MC10B_oot_AntiKt4_LCTopo'] = _oot_MC10B_AntiKt4_LCTopo
    offsetDict['MC10B_it_AntiKt4_LCTopo'] = _it_MC10B_AntiKt4_LCTopo

    offsetDict['MC10B_oot_AntiKt6_EMTopo'] = _oot_MC10B_AntiKt6_EMTopo
    offsetDict['MC10B_it_AntiKt6_EMTopo'] = _it_MC10B_AntiKt6_EMTopo

    offsetDict['MC10B_oot_AntiKt4_EMTopo'] = _oot_MC10B_AntiKt4_EMTopo
    offsetDict['MC10B_it_AntiKt4_EMTopo'] = _it_MC10B_AntiKt4_EMTopo

    #OOT time stuff doesn't matter, but it needs to be loaded. Will fix. 


    offsetDict['MC11A_oot_AntiKt6_LCTopo'] = _oot_MC10B_AntiKt6_LCTopo
    offsetDict['MC11A_it_AntiKt6_LCTopo'] = _it_MC11A_AntiKt6_LCTopo

    offsetDict['MC11A_oot_AntiKt4_LCTopo'] = _oot_MC10B_AntiKt4_LCTopo
    offsetDict['MC11A_it_AntiKt4_LCTopo'] = _it_MC11A_AntiKt4_LCTopo

    offsetDict['MC11A_oot_AntiKt6_EMTopo'] = _oot_MC10B_AntiKt6_EMTopo
    offsetDict['MC11A_it_AntiKt6_EMTopo'] = _it_MC11A_AntiKt6_EMTopo

    offsetDict['MC11A_oot_AntiKt4_EMTopo'] = _oot_MC10B_AntiKt4_EMTopo
    offsetDict['MC11A_it_AntiKt4_EMTopo'] = _it_MC11A_AntiKt4_EMTopo

    #for the time being mc11a is mc11b

    offsetDict['MC11B_oot_AntiKt6_LCTopo'] = _oot_MC10B_AntiKt6_LCTopo
    offsetDict['MC11B_it_AntiKt6_LCTopo'] = _it_MC11B_AntiKt6_LCTopo

    offsetDict['MC11B_oot_AntiKt4_LCTopo'] = _oot_MC10B_AntiKt4_LCTopo
    offsetDict['MC11B_it_AntiKt4_LCTopo'] = _it_MC11B_AntiKt4_LCTopo

    offsetDict['MC11B_oot_AntiKt6_EMTopo'] = _oot_MC10B_AntiKt6_EMTopo
    offsetDict['MC11B_it_AntiKt6_EMTopo'] = _it_MC11B_AntiKt6_EMTopo

    offsetDict['MC11B_oot_AntiKt4_EMTopo'] = _oot_MC10B_AntiKt4_EMTopo
    offsetDict['MC11B_it_AntiKt4_EMTopo'] = _it_MC11B_AntiKt4_EMTopo

    #mcllc
    offsetDict['MC11C_oot_AntiKt6_LCTopo'] = _oot_MC10B_AntiKt6_LCTopo
    offsetDict['MC11C_it_AntiKt6_LCTopo'] = _it_MC11C_AntiKt6_LCTopo

    offsetDict['MC11C_oot_AntiKt4_LCTopo'] = _oot_MC10B_AntiKt4_LCTopo
    offsetDict['MC11C_it_AntiKt4_LCTopo'] = _it_MC11C_AntiKt4_LCTopo

    offsetDict['MC11C_oot_AntiKt6_EMTopo'] = _oot_MC10B_AntiKt6_EMTopo
    offsetDict['MC11C_it_AntiKt6_EMTopo'] = _it_MC11C_AntiKt6_EMTopo

    offsetDict['MC11C_oot_AntiKt4_EMTopo'] = _oot_MC10B_AntiKt4_EMTopo
    offsetDict['MC11C_it_AntiKt4_EMTopo'] = _it_MC11C_AntiKt4_EMTopo




#MC11 has 8 eta bins between 0 and 4.5, whereas mc10b has 7. 

_it_MC10B_AntiKt6_EMTopo=[
[0.113253, 0.554206],
[0.0988858, 0.560879],
[0.0229146, 0.615631],
[0.0216436, 0.604061],
[-0.117142, 0.643603],
[-0.251135, 0.627901],
[-0.330684, 0.645477],
[0.0,0.0]]

_it_MC10B_AntiKt6_LCTopo=[
[0.157548, 0.841648],
[0.130792, 0.856677],
[0.0549006, 0.919961],
[0.0753573, 0.92832],
[-0.131852, 0.909487],
[-0.373156, 0.913625],
[-0.412353, 0.754934],
[0.0,0.0]]

_it_MC10B_AntiKt4_EMTopo=[
[0.0262287, 0.297376],
[0.014356, 0.2657],
[-0.0311566, 0.287447],
[-0.0125061, 0.316899],
[-0.124887, 0.340274],
[-0.185109, 0.362528],
[-0.238533, 0.363839],
[0.0,0.0]]

_it_MC10B_AntiKt4_LCTopo=[
[0.0241725, 0.406266],
[0.0339716, 0.375533],
[-0.0238977, 0.4012],
[-0.0173475, 0.449796],
[-0.16973, 0.451956],
[-0.300495, 0.508713],
[-0.30317, 0.400553],
[0.0,0.0]]

_it_MC11A_AntiKt6_LCTopo = [
[0.081129, 0.863087],
[0.174856, 0.932187],
[0.157241, 0.949689],
[0.160154, 1.04587],
[-0.12353, 1.04282],
[-0.634638, 1.00018],
[-0.425224, 0.814888],
[-0.417126, 0.796178]]

_it_MC11A_AntiKt4_LCTopo = [
[0.176939, 0.395263],
[-0.0385627, 0.450651],
[-0.0285996, 0.456889],
[-0.0323493, 0.473201],
[-0.207551, 0.497915],
[-0.44302, 0.538598],
[-0.377041, 0.415136],
[-0.227415, 0.430346]]

_it_MC11A_AntiKt6_EMTopo = [
[0.0604418, 0.559813],
[0.0507161, 0.574198],
[0.0426326, 0.596214],
[0.0527892, 0.602762],
[-0.126838, 0.633544],
[-0.33467, 0.646801],
[-0.271933, 0.520153],
[-0.408004, 0.651975]]

_it_MC11A_AntiKt4_EMTopo = [
[0.150383, 0.258029],
[-0.0456719, 0.293964],
[-0.0308581, 0.306925],
[-0.0978798, 0.306813],
[-0.136627, 0.337463],
[-0.284215, 0.375363],
[-0.28942, 0.290287],
[-0.185365, 0.39293]]

_it_MC11B_AntiKt6_EMTopo = [
[0.15247, 0.525615],
[0.137904, 0.528486],
[0.1003, 0.582711],
[0.0512972, 0.587876],
[-0.0706831, 0.624681],
[-0.204989, 0.630142],
[-0.230592, 0.524266],
[-0.342938, 0.593821]]


_it_MC11B_AntiKt6_LCTopo = [
[0.210152, 0.831506],
[0.194024, 0.84977],
[0.144459, 0.946095],
[0.0872807, 1.00528],
[-0.113683, 1.0077],
[-0.368619, 0.95988],
[-0.40265, 0.81116],
[-0.470396, 0.733868]]

_it_MC11B_AntiKt4_EMTopo = [
[0.0547216, 0.232914],
[0.0400233, 0.239226],
[0.0267392, 0.268988],
[-0.021222, 0.296892],
[-0.108674, 0.334936],
[-0.201987, 0.372745],
[-0.190477, 0.279828],
[-0.27411, 0.359897]]

_it_MC11B_AntiKt4_LCTopo = [
[0.0596902, 0.364774],
[0.0397633, 0.37685],
[0.0146006, 0.421035],
[-0.0421451, 0.460094],
[-0.174076, 0.472976],
[-0.33694, 0.510443],
[-0.319749, 0.392799],
[-0.352995, 0.392092]]


_it_MC11C_AntiKt4_EMTopo = [
[0.0632614, 0.198421],
[0.030444, 0.215123],
[0.0345867, 0.248505],
[-0.00971721, 0.283437],
[-0.0833448, 0.308447],
[-0.156438, 0.335779],
[-0.134727, 0.240089],
[-0.191469, 0.333182]]

_it_MC11C_AntiKt4_LCTopo = [
[0.073347, 0.326819],
[0.0489984, 0.343063],
[0.026992, 0.38963],
[-0.016652, 0.436585],
[-0.121084, 0.444808],
[-0.265092, 0.458828],
[-0.259303, 0.334476],
[-0.261505, 0.372637]]

_it_MC11C_AntiKt6_EMTopo = [
[0.118838, 0.489917],
[0.129456, 0.486993],
[0.094688, 0.529729],
[0.0531085, 0.549239],
[-0.0316177, 0.567104],
[-0.145253, 0.566533],
[-0.181018, 0.459689],
[-0.268132, 0.507946]]

_it_MC11C_AntiKt6_LCTopo = [
[0.162535, 0.777937],
[0.18129, 0.784239],
[0.137138, 0.869763],
[0.095285, 0.943853],
[-0.0443445, 0.927513],
[-0.246819, 0.861009],
[-0.30851, 0.710359],
[-0.362572, 0.633215]]

dummyOOTOffset=[
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0 ,0.0, 0.0, 0.0],
[0.0,0.0,0.0,0.0]]


dummyITOffset=[
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0],
[0.0 , 0.0]]

_oot_MC10B_AntiKt6_EMTopo=[
[-0.516786,0.176786, 219.815, 153.02],
[-0.213497,0.166057, 225.802, 143.163],
[-0.792833,0.240971, 229.228, 154.93], 
[-0.841995, 0.271792, 159.56, 194.246],
[-0.712023, 0.446724, 169.559, 174.039], 
[-1.17867, 0.561732, 157.843, 169.152],
[-0.227607, 0.519801, 84.852, 71.4725], 
[0.0,0.0,0.0,0.0]]

_oot_MC10B_AntiKt6_LCTopo=[
[-1.05206, 0.299674, 195.75, 157.166],
[-0.381549, 0.258617, 225.401, 149.555],
[-1.11227, 0.363869, 213.958, 169.54],
[-1.2208, 0.409732, 175.051, 177.975],
[-0.798995, 0.590115, 169.271, 166.405],
[-1.50429, 0.798656, 142.958, 181.86],
[-0.204731, 0.626874, 83.9244, 70.87],
[0.0,0.0,0.0,0.0]]

_oot_MC10B_AntiKt4_EMTopo=[
[-0.0784516, 0.0903611, 219.988, 141.379], 
[0.200334, 0.0771597, 229.752, 146.789],
[-0.125066, 0.115621, 226.96, 151.696],
[-0.310489, 0.159661, 154.757, 203.12],
[-0.0907442, 0.262995, 172.674, 176.926],
[-0.398694, 0.318128, 168.459, 175.487],
[0.563824, 0.283193, 79.661, 68.3148],
[0.0,0.0,0.0,0.0]]

_oot_MC10B_AntiKt4_LCTopo=[
[-0.289889, 0.147889, 202.22, 139.99],
[-0.201576, 0.166532, 201.018, 168.25],
[-0.199492, 0.174711, 212.224, 169.018],
[-0.568545, 0.243709, 169.542, 190.765],
[-0.11559, 0.345286, 173.339, 173.295],
[-0.287665, 0.421048, 157.808, 182.386],
[0.735566, 0.318853, 79.7291, 66.558],
[0.0,0.0,0.0,0.0]]

_buildOffsetDict()
