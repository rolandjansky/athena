 
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

// Athena/Gaudi includes
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
// local includes
#include "TrigT1NSWSimTools/PadTriggerLookupTool.h"
#include "TrigT1NSWSimTools/PadData.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include "TrigT1NSWSimTools/PadTrigger.h"
#include "TrigT1NSWSimTools/tdr_compat_enum.h"
#include "TrigT1NSWSimTools/sTGCTriggerBandsInEta.h"



// Muon software includes
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonAGDDDescription/sTGCDetectorDescription.h"
#include "MuonAGDDDescription/sTGCDetectorHelper.h"

#include "PathResolver/PathResolver.h"

#include <algorithm>
#include <fstream>
#include <numeric>

namespace NSWL1 {
//------------------------------------------------------------------------------
PadTriggerLookupTool::PadTriggerLookupTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent) :
    AthAlgTool(type,name,parent),
    m_etaBandsLargeSector(BandsInEtaLargeSector),
    m_etaBandsSmallSector(BandsInEtaSmallSector),
    m_detManager(0),
    m_dumpSectorGeometry(true),
    m_doNtuple(false)

{
    declareInterface<NSWL1::IPadTriggerLookupTool>(this);
    declareProperty("DumpSectorGeometry",m_dumpSectorGeometry  = true, "record sector pad geometry into an ASCII file / use it for debugging");
    declareProperty("DoNtuple", m_doNtuple = false, "save the trigger outputs in an analysis ntuple"); 
}
//------------------------------------------------------------------------------
PadTriggerLookupTool::~PadTriggerLookupTool() {

}



StatusCode PadTriggerLookupTool::initialize() {
    ATH_MSG_INFO( "initializing " << name() );
    ATH_MSG_INFO( name() << " configuration:");
    ATH_CHECK( detStore()->retrieve( m_detManager ));
    ATH_CHECK(loadCoincidenceTable("TriggerPatterns.dat") );
    
    if(m_dumpSectorGeometry){
        ATH_MSG_INFO(" Will dump  3D pad geometry / sector");
        std::ofstream padGeoFile("NSWSectorPadsGeoDump.dat");//use local variables in order not to contaminate members
        //grab full sector pads
        std::vector<std::shared_ptr<PadOfflineData>> smallSectorPads=fetchSmallSectorPads();
        std::vector<std::shared_ptr<PadOfflineData>> largeSectorPads=fetchLargeSectorPads();
        ATH_CHECK(printGeometry(largeSectorPads,padGeoFile) );
        ATH_CHECK(printGeometry(smallSectorPads,padGeoFile) );
        padGeoFile.close();

    }
    const IInterface* parent = this->parent();
    const INamedInterface* pnamed = dynamic_cast<const INamedInterface*>(parent);
    std::string algo_name = pnamed->name();
    if ( m_doNtuple && algo_name=="NSWL1Simulation" ) {
        TTree *tree=nullptr;
        ATH_CHECK(get_tree_from_histsvc(tree));
        m_validation_tree.init_tree(tree);
    }
    
    return StatusCode::SUCCESS;
}
//------------------------------------------------------------------------------

void PadTriggerLookupTool::handle(const Incident& inc) {
    


    if( inc.type()==IncidentType::BeginEvent ) {
        m_validation_tree.reset_ntuple_variables();
    }
    
    
    return;
}

StatusCode PadTriggerLookupTool::get_tree_from_histsvc( TTree*&tree)
{
    ITHistSvc* tHistSvc=nullptr;
    m_validation_tree.clear_ntuple_variables();
    ATH_CHECK(service("THistSvc", tHistSvc));
    std::string algoname = dynamic_cast<const INamedInterface*>(parent())->name();
    std::string treename = PadTriggerValidationTree::treename_from_algoname(algoname);  
    ATH_CHECK(tHistSvc->getTree(treename, tree));
    return StatusCode::SUCCESS;
}

//given a set of Pads / event this will distinguish the activated sides + sectors then looks up all possible triggers from the LUT
StatusCode PadTriggerLookupTool::lookup_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads,
                                                            std::vector<std::unique_ptr<PadTrigger>> &triggers)
{

    std::set<int> activeSides;
    std::set<int> activeSectors;

    //use sector numbers {1,2,3,.....16}
    std::for_each(pads.begin(),pads.end(),[&](const std::shared_ptr<PadData>& p){
                                                activeSides.insert(p->sideId() );
                                                activeSectors.insert(p->triggerSectorNumber());
                                            }
    );

    //use only active sectors selected by the pads / event
    for(const size_t &side : activeSides){//0:C 1:A
        for(const size_t &sector : activeSectors){//{1...16} odd:L even:S
            ATH_CHECK( LookupSectorTriggers(pads,side,sector,triggers));//GA - should we limit the simulation to run on sector with less than ~4*8=32 pad hits?
        }
    }
    //now you have gathered all the triggers from all sides/sectors..
    //fill some geometric info for strip selection. the info is sent to the strip
    //(currently using band eta-design from Y.R)
    //However it must be removed from the code once xml file is fixed for bands-strips

    return StatusCode::SUCCESS;
}


StatusCode PadTriggerLookupTool::loadCoincidenceTable(std::string padCoincidenceFileName){
    
    std::string file = PathResolver::find_file (padCoincidenceFileName, "DATAPATH");
    ATH_MSG_INFO("Loading coincidence table from "<<file);
    std::ifstream infile(file.c_str());
    if(!infile){
        ATH_MSG_FATAL("Can't open file " << file);
        return StatusCode::FAILURE;
    }
    std::string line;
    while(  getline( infile, line ) ){
            
        std::stringstream iss( line );
        int number;
        std::vector<int> fullPattern;
        while ( iss >> number ){
               fullPattern.push_back( number );
        }
        //strip pads and band-phiId parts off
        //The first 8 elements are coincidence patterns the remaining two and band and phi ids
        std::vector<int> padsOnlyPattern(fullPattern.begin(), fullPattern.begin() + 8);
        std::vector<int> bandAndPhiId(fullPattern.begin()+8, fullPattern.begin() + 10);
        int bandId=bandAndPhiId.at(0);
        int phiId=bandAndPhiId.at(1);
        m_pats[padsOnlyPattern]=std::pair<int,int>(bandId,phiId);
    }

    ATH_CHECK(expandCoincidenceTable() );
    ATH_MSG_INFO(" Load "<<m_pats.size()<<" coincidence patterns...");
    return StatusCode::SUCCESS;
}

StatusCode PadTriggerLookupTool::expandCoincidenceTable(){//There we append 3o4s to the lookup table itself...
        
        std::unordered_map<std::vector<int>,std::pair<int,int> ,hashPattern> derivedCoincidencePatterns;
    
        std::vector<int> innerIndices(4);
        std::vector<int> outerIndices(4);
        std::vector<int> allIndices(8);
        std::iota(innerIndices.begin(), innerIndices.begin() + 4, 0); 
        std::iota(outerIndices.begin(), outerIndices.begin() + 4, 4);
        std::iota(allIndices.begin(),allIndices.begin()+8,0);

        for(const auto& kv : m_pats){
            int bandid=kv.second.first;
            int phiid=kv.second.second;
            for(const int& in : innerIndices ){
                std::vector<int> pattern=kv.first;//copy
                if(pattern.at(in)==nullPadNumber || pattern.at(in+4)==nullPadNumber){
                    continue;
                }
                pattern.at(in)=nullPadNumber;
                for(const int& out :outerIndices){
                    int thispattern=pattern.at(out);
                    pattern.at(out)=nullPadNumber;
                    derivedCoincidencePatterns[pattern]=std::pair<int,int>(bandid,phiid);
                    pattern.at(out)=thispattern;//revert the change for the next one
                }
            }

            for(const int& i : allIndices){
                std::vector<int> pattern=kv.first;//copy
                pattern.at(i)=nullPadNumber;
                derivedCoincidencePatterns[pattern]=std::pair<int,int>(bandid,phiid);
            }
        }

        m_pats.insert(derivedCoincidencePatterns.begin(),derivedCoincidencePatterns.end());
        ATH_MSG_INFO(" Picked up "<<m_pats.size()<<" pad coincidences");
        return StatusCode::SUCCESS;
}



/*
selectWedgePatterns :

Given a set of pads / event
This will filter the given pads w.r.t to side/sector/multiplet/layers
then will return all possible pad combinations (taking the logic into account..)
later on these combinations are to be lookedup from the table / execute
//for 4-layers hit patterns, use a weak logic to tolerate noisy pads(only 1 layer tolerance)

====it may return empty vector ====
*/



std::vector<std::vector<std::shared_ptr<PadData> >> PadTriggerLookupTool::selectWedgePatterns(const std::vector<std::shared_ptr<PadData>> &pads,int side, int sector,int wedge){

        //first filter w.r.t inner/outer
        std::vector<std::shared_ptr<PadData> > padsInThisWedge;
        //Within  logic we have sectors numbered as {1,2,3....16} : A/C 
        auto wedgeSelector=[=](const std::shared_ptr<PadData>& pad)->bool{
            return     pad->sideId() == side &&
                       pad->triggerSectorNumber() == sector &&
                       pad->multipletId() == wedge;
        };
        //filter pads within this sector / wedge
        std::copy_if(pads.begin(),pads.end(),std::back_inserter(padsInThisWedge),[=]( std::shared_ptr<PadData> pad){ return wedgeSelector(pad);} );
        //too many pad hits might cause a performance issue . 100 pads ~ 100/4 ~ 25 pad hits per layer !!!!
        if(padsInThisWedge.size()>100){
             ATH_MSG_WARNING("Too many pad hits: ("<<padsInThisWedge.size()<<")pad hits on side= "<<side<<" sector="<<sector<<" wedge="<<wedge);
        }
        //classify pads now layerwise
        std::vector<std::shared_ptr<PadData> > padsOnLayer1;
        std::vector<std::shared_ptr<PadData> > padsOnLayer2;
        std::vector<std::shared_ptr<PadData> > padsOnLayer3;
        std::vector<std::shared_ptr<PadData> > padsOnLayer4;
        std::copy_if(padsInThisWedge.begin(),padsInThisWedge.end(),std::back_inserter(padsOnLayer1),[](const std::shared_ptr<PadData>& pad){ return pad->gasGapId()==1;} );
        std::copy_if(padsInThisWedge.begin(),padsInThisWedge.end(),std::back_inserter(padsOnLayer2),[](const std::shared_ptr<PadData>& pad){ return pad->gasGapId()==2;} );
        std::copy_if(padsInThisWedge.begin(),padsInThisWedge.end(),std::back_inserter(padsOnLayer3),[](const std::shared_ptr<PadData>& pad){ return pad->gasGapId()==3;} );    
        std::copy_if(padsInThisWedge.begin(),padsInThisWedge.end(),std::back_inserter(padsOnLayer4),[](const std::shared_ptr<PadData>& pad){ return pad->gasGapId()==4;} );
        
        //build all possible combinations with pad/layer
        std::vector<std::vector<std::shared_ptr<PadData> >> padCombinationsInWedge;
        //if we have more than 1 layer missing reject / returns an empty list
        std::vector< size_t> nHitsLayerWise={ padsOnLayer1.size(),padsOnLayer2.size(),padsOnLayer3.size(),padsOnLayer4.size() };
        //GA - The following was commented out because it skiiped valid SWPT events
        // if( std::count(nHitsLayerWise.begin(),nHitsLayerWise.end(),0)>1){
        //      ATH_MSG_WARNING("Not enough layer hits to build a pattern at side="<<side<<" sector="<<sector);
        //      return padCombinationsInWedge;//returns empty
        // }
        //assigns a vector of {nullPad} (size 1) on a layer if it has no hits
        auto addEmptyLayerIfAny=[]( std::vector<std::shared_ptr<PadData> >& layerPads)->void{
            if(layerPads.size() ==0 ){
                 std::vector<std::shared_ptr<PadData>> emptyLayer;
                 std::shared_ptr<PadData> nullPad=std::make_shared<PadOfflineData>(Identifier(0), 0, 0, nullptr);//do not change the null pointer ! THIS IS NULL PAD DATA
                 emptyLayer.push_back(nullPad);
                 layerPads=emptyLayer;
            }
        };

       addEmptyLayerIfAny(padsOnLayer1);
       addEmptyLayerIfAny(padsOnLayer2);
       addEmptyLayerIfAny(padsOnLayer3);
       addEmptyLayerIfAny(padsOnLayer4);
        //The neighborhood, in ieta and iphi, can be spanned by 1 at max in both directions (logic)
        //Obviously , do not apply logic or any other kind of check if one layer is missing

        auto padBinaryLogic=[](const std::shared_ptr<PadData>& padNext,const std::shared_ptr<PadData>& padPrev)->bool{//compares two pads on consecutive layers
            if(padNext->padNumber()==nullPadNumber || padPrev->padNumber() == nullPadNumber) return true;
            else{
            return   std::abs(padNext->padNumber()-padPrev->padNumber() )==1 ||
                     std::abs(padNext->padNumber() -padPrev->padNumber() )==18 ||
                     std::abs(padNext->padNumber() -padPrev->padNumber() )==19 ||
                     std::abs(padNext->padNumber() -padPrev->padNumber() )==17 ||
                     (padNext->padNumber() -padPrev->padNumber() ==0);
            }
        };

        //given a set of pads check if all is within the same module...
        auto allInSameModule=[](const std::vector<std::shared_ptr<PadData>>& pads)->bool{
                if(pads.size()!=4) return false;//no need to check again here but extra protection doesnt hurt
                int moduleIdFirst;
                int ctr=0;
                for(const std::shared_ptr<PadData>& pad : pads){
                    if(pad->padNumber()==nullPadNumber) continue;//ignore missing layer
                    if(ctr==0){
                        moduleIdFirst=pad->moduleId();
                        ctr++;
                        continue;
                    }
                    if(pad->moduleId()!=moduleIdFirst) return false;
                    
                }
                return true;                   
        };

        //Given a set of pads/module/wedge check if the given pad configuration satisfies the logic
        //for 4 layer hit patterns use the 2-bits weak logic.
        //if you want to scan all the legit triggers use full logic (with 3 bits for 4 layer hits!)
        auto padTowerLogic=[&padBinaryLogic,&allInSameModule](const std::vector<std::shared_ptr<PadData>>& pads)->bool{
                std::bitset<3> layerbits;//holds 0-->1 1-->2 2-->3 logic / transitions across consecutive layers
                //==== check all pads have same module ids =====
                if(not allInSameModule(pads)) return false;
                for(unsigned i=0;i<4;i++){
                    if( padBinaryLogic(pads.at(i),pads.at(i+1)) ) layerbits.set(i); //sets bits 0 ,1 and 2 (layer transitions 0->1 1->2 2->3 )
                    if(i==2) break;
                }
                // for 3o4s the logic bits must be 1,1,1
                //for 4o4s the bits  must have at least two bits set (1 layer fault tolerance, to acount for noise as discussed with Y.R, E.K and Riccardo to mimic the hardware)
                bool is4o4= true ? std::count_if(pads.begin(),pads.end(),[](const std::shared_ptr<PadData>& p){ return p->padNumber()==nullPadNumber;})==0 : false;
                if(is4o4){
                     return layerbits.count()>=2;
                }
                else{//3o4
                    return layerbits.count()==3;
                }
        };
        //as we are using pads/sector but not pads/module this will boost performance by eliminating meaningless pad combinations across different modules
        auto sameModule=[](const std::shared_ptr<PadData>& padNext,const std::shared_ptr<PadData>& padPrev )->bool{
            if(padNext->padNumber()==nullPadNumber || padPrev->padNumber() == nullPadNumber) return true;
            else return (padNext->padNumber()/1000 == padPrev->padNumber()/1000);
            /*
            No need to check sector/side matching as the whole thing runs on per unique sector
            */
        };
        //We can remove the entire "allInSameModule" check as we do module checks with sameModule in nested loops (Gal spotted this)
        for(const std::shared_ptr<PadData>& pad1 : padsOnLayer1 ){
            for(const std::shared_ptr<PadData>& pad2 : padsOnLayer2 ){
                if( !sameModule(pad2,pad1) ) continue;
                for(const std::shared_ptr<PadData>& pad3 : padsOnLayer3 ){
                    if( !sameModule(pad3,pad2) ) continue;
                    for(const std::shared_ptr<PadData>& pad4 : padsOnLayer4 ){
                        if( !sameModule(pad4,pad3) ) continue;
                        std::vector<std::shared_ptr<PadData>> patt={pad1,pad2,pad3,pad4};
                        if(! padTowerLogic(patt) ) continue;
                        padCombinationsInWedge.emplace_back(std::vector<std::shared_ptr<PadData> >({pad1,pad2,pad3,pad4}) );
                    }
                }
            }
        }
        return padCombinationsInWedge;
    }

    StatusCode PadTriggerLookupTool::LookupSectorTriggers(const std::vector<std::shared_ptr<PadData>>& pads,int side, int sector, std::vector<std::unique_ptr<PadTrigger>> &triggers){
        const  int STGCINNER=1;
        const  int STGCOUTER=2;
        auto inOutModuleMatch=[](const std::vector<std::shared_ptr<PadData>>& inM,const std::vector<std::shared_ptr<PadData>>& outM)->bool{
            for(const auto& in : inM){
                for(const auto& out : outM){
                    if(in->padNumber()!=nullPadNumber && out->padNumber()!=nullPadNumber ){
                        //if youre not sure what these numbers are here and all around see PadOfflineData::padNumber()
                        // This is without TR
                        //if(!(out->padNumber()/1000-in->padNumber()/1000 <=1)){
                        //    return false;
                        //}
                        if(!(out->padNumber()/1000-in->padNumber()/1000 <=1) && !(out->padNumber()/1000 == 2 && in->padNumber()/1000==1) && !(out->padNumber()/1000 == 3 && in->padNumber()/1000==2) && !(out->padNumber()/1000 == 5 && in->padNumber()/1000==4) && !(out->padNumber()/1000 == 6 && in->padNumber()/1000==5)){
                            return false;
                        }                      
                    }
                }
            }
            return true;
        };

        // GA - SWPT in TR functions
        auto isSingleWedgeTriggerInner=[](const std::vector<std::shared_ptr<PadData>>& inM)->bool{
            for(const auto& in : inM){
                if(in->padNumber()==nullPadNumber){ //There are no nullPadNumbers in TR as we only have 1*4/4
                    return false;
                    }            
            }
            return true;
        };

        auto isSingleWedgeTriggerOuter=[](const std::vector<std::shared_ptr<PadData>>& outM)->bool{
            for(const auto& out : outM){
                if(out->padNumber()==nullPadNumber){ //There are no nullPadNumbers in TR as we only have 1*4/4
                    return false;
                    }                   
            }
            return true;
        };

        auto isCombinationInLUT=[&](const std::vector<std::shared_ptr<PadData>>& inM,const std::vector<std::shared_ptr<PadData>>& outM, int tr)->bool{ 
            //tr means 0-not TR, 1 - inner SWPT, 2 - outer SWPT
            std::vector<std::shared_ptr<PadData>> nullWedgePattern (4);
            std::shared_ptr<PadData> nullPad=std::make_shared<PadOfflineData>(Identifier(0), 0, 0, nullptr);
            std::fill(nullWedgePattern.begin(),nullWedgePattern.end(),nullPad);
            std::vector<std::shared_ptr<PadData>> combinedInOut;
            std::vector<int> pads2Numbers;
            std::pair<int,int> lookupValue;
            
            if(inOutModuleMatch(inM,outM) && tr==0){
                combinedInOut.insert(combinedInOut.end(),inM.begin(),inM.end());
                combinedInOut.insert(combinedInOut.end(),outM.begin(),outM.end());
                std::transform(combinedInOut.begin(),combinedInOut.end(),std::back_inserter(pads2Numbers),[](const std::shared_ptr<PadData>& p){ return p->padNumber();});
                lookupValue=Lookup(pads2Numbers);
                if(lookupValue.first>=0){ //no trigger is found yet
                    return true;
                }
            }
            else if(isSingleWedgeTriggerInner(inM) && tr==1){
                combinedInOut.insert(combinedInOut.end(),inM.begin(),inM.end());
                combinedInOut.insert(combinedInOut.end(),nullWedgePattern.begin(),nullWedgePattern.end());
                std::transform(combinedInOut.begin(),combinedInOut.end(),std::back_inserter(pads2Numbers),[](const std::shared_ptr<PadData>& p){ return p->padNumber();});
                lookupValue=Lookup(pads2Numbers);
                if(lookupValue.first>=0){ //no trigger is found yet
                    return true;
                }
            }
            else if(isSingleWedgeTriggerOuter(outM) && tr==2){
                combinedInOut.insert(combinedInOut.end(),nullWedgePattern.begin(),nullWedgePattern.end());
                combinedInOut.insert(combinedInOut.end(),outM.begin(),outM.end());
                std::transform(combinedInOut.begin(),combinedInOut.end(),std::back_inserter(pads2Numbers),[](const std::shared_ptr<PadData>& p){ return p->padNumber();});
                lookupValue=Lookup(pads2Numbers);
                if(lookupValue.first>=0){ //no trigger is found yet
                    return true;
                }
            }
            return false;
        };

        std::vector<std::pair<int,int> > uniqueBandPhiIds;//just to keep track of  the unique triggers found within this lookup call
        std::vector<std::vector<std::shared_ptr<PadData>> > innerPatterns=selectWedgePatterns(pads, side,  sector,STGCINNER);
        std::vector<std::vector<std::shared_ptr<PadData>> > outerPatterns=selectWedgePatterns(pads, side,  sector,STGCOUTER);
        //GA - nullPad vector for SWPT in TR to be integrated in combinedInOut for the missing wedge
        std::vector<std::shared_ptr<PadData>> nullWedgePattern (4);
        std::shared_ptr<PadData> nullPad=std::make_shared<PadOfflineData>(Identifier(0), 0, 0, nullptr);
        std::fill(nullWedgePattern.begin(),nullWedgePattern.end(),nullPad);
        
        // We need to change these if's, size zero means it can still be in TR in one of the other wedges
        // if(innerPatterns.size()== 0){
        //     ATH_MSG_WARNING(" No hit patterns found / inner wedge");
        //     return StatusCode::SUCCESS;
        // }
        // if(outerPatterns.size()== 0){
        //     ATH_MSG_WARNING(" No hit patterns found / outer wedge");
        //     return StatusCode::SUCCESS;
        // }
        if(innerPatterns.size()==0 && outerPatterns.size()==0){
            ATH_MSG_WARNING(" No hit patterns found / inner and outer wedges");
        }
        bool innerPatternUsedBreak=false; //break the outer for loop of innerpattern if the inner pattern was already used, saving time
        for( std::vector<std::shared_ptr<PadData>> inner: innerPatterns){
            innerPatternUsedBreak=false;
            for( std::vector<std::shared_ptr<PadData>> outer: outerPatterns){
                //concatenate the two if modules match then Lookup
                if(innerPatternUsedBreak) break;//already found a pattern using the innerPattern (outer for loop), sso we can skip the outer loop iteration to save time
                std::vector<std::shared_ptr<PadData>> combinedInOut;
                int isTR=0; //value which states what kind of trigger we have: 0 - non-TR, 1 - TR in the inner wedge, 2 - TR in the outer wedge

                if(isCombinationInLUT(inner,outer,0)){//Both wedges
                    combinedInOut.insert(combinedInOut.end(),inner.begin(),inner.end());
                    combinedInOut.insert(combinedInOut.end(),outer.begin(),outer.end());
                    innerPatternUsedBreak=true;
                }
                else if(isCombinationInLUT(inner,outer,1) && innerPatterns.size()!=0){//TR inner wedge
                    combinedInOut.insert(combinedInOut.end(),inner.begin(),inner.end());
                    combinedInOut.insert(combinedInOut.end(),nullWedgePattern.begin(),nullWedgePattern.end());
                    isTR=1;//inner SWPT in TR
                    innerPatternUsedBreak=true;
                }
                else if(isCombinationInLUT(inner,outer,2) && outerPatterns.size()!=0){//TR outer wedge
                    combinedInOut.insert(combinedInOut.end(),nullWedgePattern.begin(),nullWedgePattern.end());
                    combinedInOut.insert(combinedInOut.end(),outer.begin(),outer.end());
                    isTR=2;//outer SWPT in TR
                }
                else continue; //no trigger was found
                //convert set of pads into numbers as in the LUT
                std::vector<int> pads2Numbers;
                std::transform(combinedInOut.begin(),combinedInOut.end(),std::back_inserter(pads2Numbers),[](const std::shared_ptr<PadData>& p){ return p->padNumber();});


                std::pair<int,int> lookupValue=Lookup(pads2Numbers);
                if(lookupValue.first<0) continue;//{ //no trigger is found yet // GA - this is an additional check, unnecessary
                //append pads to triggers + fill in unique triggers
                //if band and phi id found is already in the list of sector triggers do not add it to the list
                //uniqueness defined by sideId /sectorId /bandId /phiId (!!!not only band and phiIds !!!)
                auto trgpos=std::find_if(triggers.begin(),triggers.end(),
                                        [&lookupValue,&side,&sector](const std::unique_ptr<PadTrigger>&tr){
                                          return tr->bandId() == lookupValue.second &&
                                                 tr->phiId() == lookupValue.first &&
                                                 tr->triggerSectorNumber() == sector &&
                                                 tr->sideId() == side;
                                        });
                if( trgpos != triggers.end() ) continue;//trigger already exists int the list ... skip
                uniqueBandPhiIds.push_back(lookupValue);
                //otherwise instantiate a trigger and append to the list                    
                auto trigger=std::make_unique<PadTrigger>();//TODO : PadTrigger needs to be rewriten as a proper class.using a C struct is too messy
                std::copy(combinedInOut.begin(),combinedInOut.begin()+4,std::back_inserter(trigger->m_padsInner));
                std::copy(combinedInOut.begin()+4,combinedInOut.end(),std::back_inserter(trigger->m_padsOuter));
                std::copy(combinedInOut.begin(),combinedInOut.end(),std::back_inserter(trigger->m_pads));
                trigger->m_bandid=lookupValue.second;
                trigger->m_phi_id=lookupValue.first;
                int triggerSectorType;
                if(isTR!=2){
                    triggerSectorType=trigger->firstPad()->sectorType();//S:0/1:L
                }
                else{ // all inner patterns are null, so we will not get sectorType, thus looking at outer wedge pads to determine sectorType
                    triggerSectorType=trigger->firstPadOuter()->sectorType();//S:0/1:L
                }
                switch(triggerSectorType){
                    case(0)://S
                    //Y.R says band ids should start from 2, so to convert it to a C index subtract 2 from bandId
                    //See the header file sTGCTriggerBandsInEta (strip channels<--> bands must be implmeneted in the XML and this eta mased matching must be abandoned. The existing XML values are wrong)
                        trigger->m_etamin=m_etaBandsSmallSector.at(trigger->bandId()-1);
                        trigger->m_etamax=m_etaBandsSmallSector.at(trigger->bandId()-2);
                    break;
                    case(1)://L
                    // The same subtraction likewise for L as well
                        trigger->m_etamin=m_etaBandsLargeSector.at(trigger->bandId()-1);
                        trigger->m_etamax=m_etaBandsLargeSector.at(trigger->bandId()-2); 
                    break;   

                    default:
                        ATH_MSG_FATAL("Unable to determine trigger sector type");
                    break;
                }
                trigger->m_isSmall=(triggerSectorType+1)%2;
                triggers.push_back(std::move(trigger));
                
            }  
        }

        ATH_MSG_DEBUG("Found "<<uniqueBandPhiIds.size()<<" unique triggers from LUT");

        //Upto this point no geometry is used ... i.e no pad corners , no tower roi , no overlap area etc ....
        //In principle band id should be suficient to select strips . But we dont have the band configuration in the XML yet ...
        //so we have to stick to the eta-based strip selection for now ....
        return StatusCode::SUCCESS;

    }
    //grabs a whole sector pads with Identifiers and 3D geometry information available
    /*  The method is necessary to create pad coincidence LUT by using the methods within this class
        also useful to dump the full geometetry for geometry validation& side studies(see  'printGeometry' meth. and corresponding flag in the JO)
    */
    std::vector<std::shared_ptr<PadOfflineData>> PadTriggerLookupTool::fetchSectorPads(bool isSmall, int SIDE,int SECTOR) {
        const sTgcIdHelper* idhelper=m_detManager->stgcIdHelper();

        std::vector<std::shared_ptr<PadOfflineData>> sectorPads;
        std::vector<Identifier> padIds;
        
        for(const Identifier& modId : idhelper->idVector() ){
            int ModuleSide=1?idhelper->stationEta(modId)>0 :0;
            int ModuleSect=idhelper->stationPhi(modId);
            bool SectType=idhelper->isSmall(modId);
            if(ModuleSide!=SIDE || ModuleSect !=SECTOR || SectType!=isSmall) continue;//grab only sector modules selected by the method args
            std::vector<Identifier> all_channels;
            idhelper->idChannels(modId,all_channels);

            std::vector<Identifier> pad_channels;
            std::copy_if(all_channels.begin(),all_channels.end(),std::back_inserter(pad_channels),
                    [idhelper](const Identifier& id){
                         int chanType=idhelper->channelType(id);
                         return chanType==0;
                    }
            );
            padIds.insert(padIds.end(),pad_channels.begin(),pad_channels.end());

        }

        for(Identifier id : padIds){
            int multilayer=idhelper->multilayer(id);
            int gasgap=idhelper->gasGap(id);

            int channeltype=idhelper->channelType(id);
            const MuonGM::sTgcReadoutElement* rdoEl = m_detManager->getsTgcReadoutElement(id);
            const MuonGM::MuonPadDesign* mpd=rdoEl->getPadDesign(id);
            int padEtaMinFromDesign=mpd->padEtaMin;
            int padEtaMaxFromDesign=mpd->padEtaMax;
            int nPadCols=mpd->nPadColumns; 

            int thisEta=idhelper->padEta(id);
            int thisPhi=idhelper->padPhi(id);

            int nPadRowsFromDesign=padEtaMaxFromDesign-padEtaMinFromDesign;
            if( thisEta>nPadRowsFromDesign || thisPhi > nPadCols  ) continue;

            Identifier pid=idhelper->padID(id,  multilayer,  gasgap,  channeltype,  thisEta,  thisPhi,true);
            auto pad=std::make_shared<PadOfflineData>(pid, 0, 0, m_detManager);
            pad->fillGeometricInformation();
            sectorPads.push_back(pad);
        }
        return sectorPads;
    }
    //check the header, default sector is 1 default side is 1
    
    
    //just as some wrapping for other sectors 
    std::vector<std::shared_ptr<PadOfflineData>> PadTriggerLookupTool::fetchSmallSectorPads(int side,int sector){
        return fetchSectorPads(true,side,sector);
    }
    std::vector<std::shared_ptr<PadOfflineData>> PadTriggerLookupTool::fetchLargeSectorPads(int side,int sector){
        return fetchSectorPads(false,side,sector);
    }

StatusCode PadTriggerLookupTool::printGeometry( const std::vector<std::shared_ptr<PadOfflineData> >& pads, std::ofstream& ofile){
    const static std::vector<int> vertexordering={0,1,3,2};
    if(! ofile.is_open()){
        ATH_MSG_FATAL("File is not open!");
    }
    auto dumpPadGeometry=[&ofile](const std::shared_ptr<PadOfflineData>& p)->void{
            Vertices padVertices;
            for(int i=0;i<4;i++){//four corners
                int index=vertexordering.at(i);
                float x=p->m_cornerXyz[index][0];
                float y=p->m_cornerXyz[index][1];
                Vertex vtx(x,y);
                padVertices.push_back(vtx);
            }
            std::string secType= p->sectorType()==1 ? "LARGE": "SMALL";
            float layerZ=p->m_cornerXyz[0][2];
            Polygon padPolygon=buildPolygon(padVertices);
            std::stringstream ss;
            ss<<secType<<" multilayer="<<p->multipletId()<<" module="<<p->moduleId()<<" layer="<<p->gasGapId()<<" layerZ="<<layerZ<<" iEta="<<p->padEtaId()<<" iPhi="<<p->padPhiId()<<" <--> "<<boost::geometry::wkt(padPolygon)<<std::endl;
            ofile<<ss.rdbuf();
    };
    std::for_each(pads.begin(),pads.end(),dumpPadGeometry);
    return StatusCode::SUCCESS;
}

//Given a pattern check if it is in the LUT if yes get bandId and PhiId
    std::pair<int,int> PadTriggerLookupTool::Lookup( std::vector<int> patt){
        auto pos=m_pats.find(patt);//should be consant in time
        if(   pos != m_pats.end()   ){//if something is found from this pattern then return it
            return pos->second;
        }

        //if nothing is found yet, check if the pattern is  4o4+4o4 or 3o4+3o4 + ... etc
        //if 3o4+3o4 return (-1,-1) i.e nothing to lookup up furthermore
        //else if 4o4+4o4 or 4o4+3o4 or vice versa  continue lookup by removing layers from 4o4s ...

        // GA - we need to do the following given that we're not in SWPT TR
        // if(patt.front()!=nullPadNumber && patt.back()!=nullPadNumber){ // THIS IS INCORRECT, IT MISSES PATTERNS WITH ONE NULLPAD LIKE (x1,x2,x3,x4,x5,x6,x7,-9999)
            int nullCountInner=std::count(patt.begin(),patt.begin()+4,nullPadNumber);
            int nullCountOuter=std::count(patt.begin()+4,patt.end(),nullPadNumber);
            if(nullCountInner+nullCountOuter>=2) return std::pair<int,int>(-1,-1);// MAYBE JUST ADJUST IT TO BE >=2??
            if( nullCountInner ==0 ){//outer is then 3o4, dont touch outer, remove layers from inner + lookup
                for(int i :{0,1,2,3}){
                    int valueOnLayer=patt.at(i);
                    patt.at(i)=nullPadNumber;//removes layer
                    auto pos=m_pats.find(patt);
                    if( pos!=m_pats.end() ) return pos->second;
                    patt.at(i)=valueOnLayer;//add the layer back
                }
            }
            if( nullCountOuter ==0 ){// inner is then 3o4 dont touch inner, remove layers from outer+lookup
                for(int i :{4,5,6,7}){
                    int valueOnLayer=patt.at(i);
                    patt.at(i)=nullPadNumber;//removes layer
                    auto pos=m_pats.find(patt);
                    if( pos!=m_pats.end() ) return pos->second;
                    patt.at(i)=valueOnLayer;//add the layer back
                }
            }
        // }
        return std::pair<int,int>(-1,-1);
    }

} // NSWL1

