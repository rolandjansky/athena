#!/usr/bin/env python
#python script to read the xAOD file
import os
import sys
import ROOT
import collections
from pprint import pprint
import argparse
def ReadMyxAOD(evt):
    result=[]
    for i in range(0,evt.getEntries()):
        evt.getEntry(i)


        
        acc_chargedFE_muon=ROOT.SG.AuxElement.ConstAccessor(
            "vector<ElementLink<xAOD::MuonContainer>>"
        )("FE_MuonLinks") # accessor matches type in the aux branch
        acc_neutralFE_muon=ROOT.SG.AuxElement.ConstAccessor(
            "vector<ElementLink<xAOD::MuonContainer>>"
        )("FE_MuonLinks") # accessor matches type in the aux branch

        chargedFEcont=evt.retrieve("xAOD::FlowElementContainer","JetETMissChargedParticleFlowObjects")
        nchargedFE=chargedFEcont.size()
        try:
            for cfe in chargedFEcont:
                cfe_pt=cfe.p4().Pt()
                print("Charged Flow Element Pt(): {}".format(cfe_pt))
                if (acc_chargedFE_muon.isAvailable(cfe)):
                    auxcont_cfe=acc_charged_FE_muon(cfe)
                    for itr in range(0,auxcont_cfe.size()):
                        auxlink=auxcont_cfe[itr]
                        if(auxlink.isValid()):
                            print("Got a link to a muon")
                else:
                    print("No auxdata retrieved")

        try:
            neutralFEcont=evt.retrieve("xAOD::FlowElementContainer","JetETMissNeutralParticleFlowObjects")
            print("\n")
            nneutralFE=neutralFEcont.size()
            for nfe in neutralFEcont:
                nfe_pt=nfe.p4().Pt()
                print("Neutral Flow Element Pt(): {}".format(cfe_pt))
                if (acc_neutralFE_muon.isAvailable(cfe)):
                    auxcont_nfe=acc_neutral_FE_muon(cfe)
                    for itr in range(0,auxcont_nfe.size()):
                        auxlink=auxcont_nfe[itr]
                        if(auxlink.isValid()):
                            print("Got a link to a muon")
                else:
                    print("No auxdata retrieved")
        except:
            print("No charged FE container accessible")
            nneutralFE=0
        
        outputdict={"nChargedFE":nchargedFE,"nNeutralFE":nneutralFE}
        result.append(outputdict)
    return result

def main():
    parser=argparse.ArgumentParser("validateFlowElements.py - script to read xAOD and dump a nice histogram of the efraction")
    parser.add_argument("--f",dest="files",action="store",default="",help='Input file (default = "")')
    opts=parser.parse_args()
    if(opts.files==""):
        print("No file specified")
        sys.exit(-1)

    evt=ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess)
    stat=evt.readFrom(opts.files)

    result=ReadMyxAOD(evt)
    print("\n Printing output")
    for item in result:
        print(item)

if __name__=="__main__":
    main()
