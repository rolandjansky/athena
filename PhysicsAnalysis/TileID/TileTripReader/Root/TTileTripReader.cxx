/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TTileTripReader.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on August 7, 2012, 4:21 PM
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include <TChain.h>
#include <TMath.h>

#include "TileTripReader/TTileTripReader.h"

using namespace std;
const double pi=3.14159265358979323846;
const double width=2.*pi/64.;

namespace Root{

TTileTripReader::TTileTripReader(const char*
#ifndef ROOTCORE
      name)
    : TSelectorToolBase(name),TCalculatorToolBase(name)
#else
      )
#endif /*ROOTCORE*/
{
    m_trips=new TChain("TripList");
    m_runMap=new TChain("RunMap");
    m_currentRun=0;
    m_currentLbn=0;
    m_startEntry=0;
    m_Partition=0;
    m_Module=0;
    m_LumiEnd=0;
    m_FirstEntry=0;
    m_LumiStart=0;
    m_Run=0;
    m_mapRun=0;
    m_LastTileFlags=0;
    m_verbosity=Default;
    m_msglog=&cout;
    m_LBOffsets.eta1=0;
    m_LBOffsets.eta2=0;
    m_LBOffsets.phi1=0;
    m_LBOffsets.phi2=0;
    m_EBOffsets.eta1=0;
    m_EBOffsets.eta2=0;
    m_EBOffsets.phi1=0;
    m_EBOffsets.phi2=0;
    m_ChainsLoaded=false;
    setBadEventList();
}

TTileTripReader::~TTileTripReader() {
    if(m_trips)delete m_trips;
    if(m_runMap)delete m_runMap;
}
#ifndef ROOTCORE
const TAccept& TTileTripReader::accept(int run, int lbn, unsigned int event, double eta, double phi, int tileError, int tileFlags){
    readTileFlags(run,lbn,tileError,tileFlags);
    if(eta!=-99.9 && phi!=-99.9)
        m_accept.setCutResult("InTrip",checkEtaPhi(run,lbn,eta,phi));
    m_accept.setCutResult("BadEvent",checkEvent((unsigned int)run,
            (unsigned short)lbn,
            (unsigned int)event));
    return m_accept;
}
#endif /*ROOTCORE*/
double TTileTripReader::areaTripFraction(int run, int lbn, double eta, double phi, double dR){
    vector<TripRegion> trips=getEtaPhi(run,lbn);
    if(phi>pi)phi=phi-2*pi;
    double dR2=dR*dR;
    double tripArea=0;
    size_t ntrips=trips.size();
    for(size_t i=0;i<ntrips;++i){
        TripRegion& trip=trips[i];
        for(double x=trip.eta1+.05;x<trip.eta2;x+=0.1 ){
            double dx2=pow(eta-x,2);
            for(double y=trip.phi1+width/8.;y<trip.phi2;y+=width/4.){
                double deltaR12 = fabs(phi - y);
                if (deltaR12 > pi)
                        deltaR12 = 2.0 * pi - deltaR12;
                double dy2=pow(deltaR12,2);
                double delR2=dx2+dy2;
                if(delR2<=dR2)
                    tripArea+=0.1*width/4.;
            }
        }
    }
    double jetArea=pi*dR*dR;
    return tripArea/jetArea;
}
#ifndef ROOTCORE
const TResult& TTileTripReader::calculate(int run, int lbn, double eta, double phi, double dR, int tileError, int tileFlags){
    readTileFlags(run,lbn,tileError,tileFlags);
    m_result.setResult("TripAreaFrac",areaTripFraction(run,lbn,eta,phi,dR));
    return m_result;
}
#endif /*ROOTCORE*/
void TTileTripReader::buildOffsets(){
    if(m_ChainsLoaded)return;
    ostream& msg=*m_msglog;
    m_Offsets.clear();
    m_Offsets.push_back(0);
    int nTrees=((TChain*)m_trips)->GetNtrees();
    if(nTrees==1){
        int mapentries=m_runMap->GetEntries();
        for(int i=0;i<mapentries;++i){
            m_runMap->GetEntry(i);
            m_trips->GetEntry(m_FirstEntry+m_Offsets.back());
            if(m_Run!=m_mapRun){
                int findRun=m_mapRun;
                m_runMap->GetEntry(i-1);
                if(m_verbosity==Debug){
                    msg<<"Searching for "<<findRun<<" starting from "<<
                            m_mapRun<<" current offset "<<m_Offsets.back()<<
                            endl;
                }
                int tripEntry=m_FirstEntry+m_Offsets.back();
                while(m_Run!=findRun){
                   if(!m_trips->GetEntry(tripEntry)){
                       msg<<"Overshot the end of the TTree!\n";
                       return;
                   }
                   ++tripEntry;
                }
                m_Offsets.push_back(tripEntry-1);
            }
        }
        return;
    }
    m_trips->SetBranchStatus("*",0);
    int tree=0;
    while(tree<nTrees){
        m_trips->GetEntry(m_Offsets.back());
        TTree* currentTree=m_trips->GetTree();
        m_Offsets.push_back(currentTree->GetEntries()+m_Offsets.back());        
        ++tree;
    }
    m_trips->SetBranchStatus("*",1);
}

bool TTileTripReader::checkEtaPhi(int run, int lbn, double eta, double phi){
    ostream& msg=*m_msglog;
    vector<TripRegion> tripRegions=getEtaPhi(run,lbn);
    size_t ntrips=tripRegions.size();
    if(phi>pi)phi=phi-2.*pi;
    if(m_verbosity==Debug){
        msg<<"Checking region eta= "<<eta<<", phi="<<phi<<" for run "<<run<<
                " lbn "<<lbn<<endl;
    }
    
    for(size_t i=0;i<ntrips;++i){
        if(tripRegions[i].eta1==0 || tripRegions[i].eta1==-0.9){// Long Barrel
            //Use a delta phi check to account for extended regions around pi--
            double delPhi=tripRegions[i].phi2+m_LBOffsets.phi2-tripRegions[i].phi1+m_LBOffsets.phi1;
            delPhi/=2.;
            double tripPhiCenter=fabs(delPhi+tripRegions[i].phi1-m_LBOffsets.phi1) <= pi ?
                delPhi+tripRegions[i].phi1-m_LBOffsets.phi1 : tripRegions[i].phi2+m_LBOffsets.phi2 - delPhi;            
            double deltaR12 = fabs(phi - tripPhiCenter);
            if (deltaR12 > pi)
                    deltaR12 = 2.0 * pi - deltaR12;
            //--
            if(tripRegions[i].eta1-m_LBOffsets.eta1<=eta &&
                    tripRegions[i].eta2+m_LBOffsets.eta2>=eta &&
                    deltaR12<delPhi) 
                return true;
        }
        else{// Extended Barrel
            //Use a delta phi check to account for extended regions around pi--
            double delPhi=tripRegions[i].phi2+m_EBOffsets.phi2-tripRegions[i].phi1+m_EBOffsets.phi1;
            delPhi/=2.;
            double tripPhiCenter=fabs(delPhi+tripRegions[i].phi1-m_EBOffsets.phi1) <= pi ?
                delPhi+tripRegions[i].phi1-m_EBOffsets.phi1 : tripRegions[i].phi2+m_EBOffsets.phi2 - delPhi;            
            double deltaR12 = fabs(phi - tripPhiCenter);
            if (deltaR12 > pi)
                    deltaR12 = 2.0 * pi - deltaR12;
            //--
            if(tripRegions[i].eta1-m_EBOffsets.eta1<=eta &&
                    tripRegions[i].eta2+m_EBOffsets.eta2>=eta &&
                    deltaR12<delPhi ) 
                return true;            
        }
    }
    return false;
}

bool TTileTripReader::checkEvent(unsigned int run, unsigned int lbn, unsigned int event){
    ostream& msg=*m_msglog;
    unsigned int evt;
    if(m_verbosity==Debug){
        msg<<"Checking bad event for run= "<<run<<", lbn="<<lbn<<" event "<<
                event<<endl;
    }
    map<unsigned int,map<unsigned short,unsigned int> >::iterator itr=m_badEvents.find(run);
    if(itr!=m_badEvents.end()){
        map<unsigned short,unsigned int>& lumi=(*itr).second;
        map<unsigned short,unsigned int>::iterator litr=lumi.find(lbn);
        if(litr!=lumi.end()){
            evt=(*litr).second;
            if(m_verbosity==Debug){
                msg<<"run= "<<run<<", lbn="<<lbn<<" event "<<
                        event<<" pass check = "<<(evt!=event)<<endl;
            }
            return (evt!=event);
        }
        if(m_verbosity==Debug){
            msg<<"run= "<<run<<", lbn="<<lbn<<" event "<<
                    event<<" pass check = "<<true<<endl;
        }        
        return true;
    }
    if(m_verbosity==Debug){
        msg<<"run= "<<run<<", lbn="<<lbn<<" event "<<
                event<<" pass check = "<<true<<endl;
    }   
    return true;
}

bool TTileTripReader::checkExtendedEtaPhi(int run, int lbn, double eta, double phi, double delEta, double delPhi){
    ostream& msg=*m_msglog;
    vector<TripRegion> tripRegions=getEtaPhi(run,lbn);
    size_t ntrips=tripRegions.size();
    if(phi>pi)phi=phi-2.*pi;
    if(m_verbosity==Debug){
        msg<<"Checking region eta= "<<eta<<", phi="<<phi<<" for run "<<run<<
                " lbn "<<lbn<<endl;
    }
    for(size_t i=0;i<ntrips;++i){
        //Use a delta phi check to account for extended regions around pi--
        double dPhi=tripRegions[i].phi2+delPhi-tripRegions[i].phi1+delPhi;
        dPhi/=2.;
        double tripPhiCenter=fabs(dPhi+tripRegions[i].phi1-delPhi) <= pi ?
            dPhi+tripRegions[i].phi1-delPhi : tripRegions[i].phi2+delPhi - dPhi;            
        double deltaR12 = fabs(phi - tripPhiCenter);
        if (deltaR12 > pi)
                deltaR12 = 2.0 * pi - deltaR12;
        //--
        if(tripRegions[i].eta1-delEta<=eta && tripRegions[i].eta2+delEta>=eta &&
               tripRegions[i].phi1-delPhi<=phi && tripRegions[i].phi2+delPhi>=phi ) 
            return true;
    }
    return false;
}

int TTileTripReader::findStartEntry(int run){
    ostream& msg=*m_msglog;
    if(run==m_currentRun)return m_startEntry;
    int entry=-1;
    do{
        ++entry;
        if(!m_runMap->GetEntry(entry))return -999;
    }while(run!=m_mapRun);
    if(m_Offsets.empty()){
        m_trips->GetEntry(m_FirstEntry);
        m_startEntry=m_FirstEntry;
    }
    else{
        if(m_ChainsLoaded || ((TChain*)m_runMap)->GetNtrees()==1){
            size_t nOffsets=m_Offsets.size();
            for(size_t i=0;i<nOffsets;++i){
                m_trips->GetEntry(m_FirstEntry+m_Offsets[i]);
                if(m_Run==run){
                    m_startEntry=m_FirstEntry+m_Offsets[i];
                    break;
                }
            }
            if(m_Run!=run)
                msg<<"Problem!  Can't find proper offset\n";
        }
        else{
            int offIdx=m_runMap->GetTreeNumber();
            m_startEntry=m_FirstEntry+m_Offsets[offIdx];
            m_trips->GetEntry(m_startEntry);
        }
    }
    if(m_Run!=run){
        msg<<"Problem!  Can't find proper offset\n";
        return -999;
//        buildOffsets();
//        m_runMap->GetEntry(entry);
//        if(m_runMap->GetNtrees()==1){
//            size_t nOffsets=m_Offsets.size();
//            for(size_t i=0;i<nOffsets;++i){
//                m_startEntry=m_FirstEntry+m_Offsets[i];
//                m_trips->GetEntry(m_FirstEntry+m_Offsets[i]);
//                if(m_Run==run)break;
//            }
//            if(m_Run!=run)
//                msg<<"Problem!  Can't find proper offset\n";
//        }
//        else{
//            int offIdx=m_runMap->GetTreeNumber();
//            m_startEntry=m_FirstEntry+m_Offsets[offIdx];
//        }
    }
    m_currentRun=run;
    if(m_verbosity==Debug){
        const string partition[4]={
            "LBA",
            "LBC",
            "EBA",
            "EBC"
        };
        msg<<"Trip list for run "<<run<<endl;
        msg<<"First entry val="<<m_startEntry<<endl;
        int loop=m_startEntry;
        m_trips->GetEntry(loop);
        msg<<" Run returned = "<<m_Run<<endl;
        while(m_Run==run){
            vector<char>& part=*m_Partition;
            vector<char>& mod=*m_Module;
            vector<int>& lbend=*m_LumiEnd;
            size_t ntrips=lbend.size();
            for(size_t i=0;i<ntrips;++i){
                msg<<m_Run<<" "<<partition[(int)part[i]]<<
                        setfill('0')<<setw(2)<<(int)mod[i]+1<<setw(4)<<
                        " lbn"<<m_LumiStart<<"-"<<
                        lbend[i]<<endl;
                
            }            
            ++loop;
            m_trips->GetEntry(loop);
        }
    } 
    return m_startEntry;
}

vector<TripRegion> TTileTripReader::getEtaPhi(int run, int lbn, bool force){
    ostream& msg=*m_msglog;
    if(run==m_currentRun && m_currentLbn==lbn && !force){
        if(m_verbosity==Debug){
            msg<<"Trip regions found for run "<<run<<" lbn "<<lbn<<endl;
            size_t ntrips=m_currentTrips.size();
            for(size_t i=0;i<ntrips;++i){
                msg<<m_currentTrips[i].eta1<<" <= eta <= "<<
                        m_currentTrips[i].eta2<<endl<<" "<<
                        m_currentTrips[i].phi1<<" <= phi <= "<<
                        m_currentTrips[i].phi2<<endl;
            }
        }
        return m_currentTrips;
    }
    int entry=findStartEntry(run);
    if(entry==-999){
        vector<TripRegion> emptyTrips;
        msg<<"Run not contained in chain!\n";
        return emptyTrips;
    }
    vector<TripRegion>& tripRegions=m_currentTrips;
    tripRegions.clear();
//    m_currentLbn=lbn;
    if(m_verbosity==Debug)
        msg<<"Generating trip regions found for run "<<run<<" lbn "<<lbn<<
                " Current run = "<<m_currentRun<<" current lbn = "<<
                m_currentLbn<<endl;
    if(!m_trips->GetEntry(entry)){
        msg<<"Run not contained in trip chain!\n";
        return tripRegions;
    }
    while(run==m_Run){
        vector<char>& part=*m_Partition;
        vector<char>& mod=*m_Module;
        vector<int>& lbend=*m_LumiEnd;
        size_t ntrips=lbend.size();
        if(m_LumiStart<=lbn){
            for(size_t i=0;i<ntrips;++i){
                if(lbend[i]>=lbn){
                    tripRegions.push_back(partModToEtaPhi(part[i],mod[i]));
                    if(m_verbosity==Debug){
                        msg<<tripRegions.back().eta1<<" <= eta <= "<<
                                tripRegions.back().eta2<<endl<<" "<<
                                tripRegions.back().phi1<<" <= phi <= "<<
                                tripRegions.back().phi2<<endl;
                    }
                }
            }
            
        }
        ++entry;
        if(!m_trips->GetEntry(entry)){
            msg<<"Run not contained in trip chain!\n";
            return tripRegions;
        }
    }
    m_currentLbn=lbn;
    return tripRegions;
}

int TTileTripReader::initialize(){
#ifndef ROOTCORE
    m_accept.addCut("InTrip","true if eta phi fall inside a powered off module");
    m_result.addResult("TripAreaFrac","The fraction of the area coverd by trips");
#endif /*ROOTCORE*/
    return 1;
}

void TTileTripReader::memLoadTripFile(){
    if(m_ChainsLoaded)return;
    TChain* tripMap=(TChain*)m_trips;
    TChain* runMap=(TChain*)m_runMap;
    m_trips=new TTree("TripList_Mem","Memory loaded TripList");
    m_runMap=new TTree("RunMap_Mem","Starting entry for each run in TripList");
    
    m_runMap->Branch("Run",&m_mapRun,"Run/I");
    m_runMap->Branch("FirstEntry",&m_FirstEntry,"FirstEntry/I");
    
    m_trips->Branch("Run",&m_Run,"Run/I");
    m_trips->Branch("LumiStart",&m_LumiStart,"LumiStart/I");
    m_trips->Branch("Partition",&m_Partition);
    m_trips->Branch("Module",&m_Module);
    m_trips->Branch("LumiEnd",&m_LumiEnd);
    
    m_currentRun=0;
    m_currentLbn=0;
    m_startEntry=0;
    
    int entry=0;
    while(runMap->GetEntry(entry)){
        m_runMap->Fill();
        ++entry;
    }
    entry=0;
    while(tripMap->GetEntry(entry)){
        m_trips->Fill();
        ++entry;
    }
    delete tripMap;
    delete runMap;
    m_ChainsLoaded=true;
    
}

TripRegion TTileTripReader::partModToEtaPhi(int part, int mod){
    ostream& msg=*m_msglog;
    TripRegion region;
    switch(part){
        case 0: //LBA
            region.eta1=0;
            region.eta2=0.9;
            break;
        case 1: //LBC
            region.eta1=-0.9;
            region.eta2=0;
            break;
        case 2: //EBA
            region.eta1=0.8;
            region.eta2=1.7;
            break;
        case 3: //EBC
            region.eta1=-1.7;
            region.eta2=-0.8;
            break;
        default:
            msg<<"Bad partition value passed!\n";
            region.eta1=-999;
            region.eta2=-999;
            region.phi1=-999;
            region.phi2=-999;
            return region;
    }
    if(mod<32){
        region.phi1=((double)mod)*width;
        region.phi2=region.phi1+width;
    }
    else if(mod<64){
        region.phi1=((double)mod)*width-2.*pi;
        region.phi2=region.phi1+width;        
    }
    else{
        msg<<"Bad module value passed!\n";
        region.eta1=-999;
        region.eta2=-999;
        region.phi1=-999;
        region.phi2=-999;
        return region;
    }
    if(m_verbosity==Debug){
        const string partition[4]={
            "LBA",
            "LBC",
            "EBA",
            "EBC"
        };
        msg<<"Module "<<partition[part]<<setfill('0')<<setw(2)<<mod+1<<
                " region "<<region.eta1<<" <= eta <= "<<region.eta2<<" "<<
                region.phi1<<" <= phi <= "<<region.phi2<<endl;
    }
    return region;    
}

void TTileTripReader::readTileFlags(int run, int lbn, int tileError, int tileFlags){
    ostream& msg=*m_msglog;
    if(tileError==0 && m_LastTileFlags){//force regenerating trip list if tileError=OK and previously an error was present
        getEtaPhi(run,lbn,true);
        m_LastTileFlags=0;
    } 
    if(tileError!=1)return; //Tile flags only contain extra masked module info if tileError==1 or Warning
    if(tileFlags==m_LastTileFlags){
        if(m_verbosity==Debug){
            size_t ntrips=m_currentTrips.size();
            msg<<"Total masked before reading tileFlags = "<<ntrips<<endl;
        }
        return;
    }//if the flags havent changed since last time, don't regenerate the trip regions
    m_LastTileFlags=tileFlags;
    getEtaPhi(run, lbn,true);//Generates a fresh set of TripRegions from DCS to compare to the flags
    int consecBad = (tileFlags>>16) & 0xF;  //consecutive bad modules
    int badMod1 = (tileFlags>>20) & 0x3F; //first module number
    int badMod2= (badMod1 + consecBad - 1); //last module number
    int part = ((tileFlags>>26) & 0x3); //partition number
    
    int greaterMod=-1;
    if(badMod2>63){
        greaterMod=badMod2-64;
        badMod2=63;
    }
    
    int currentMod=badMod1;
    while(currentMod<=badMod2){
        TripRegion trip=partModToEtaPhi(part,currentMod);
        size_t ntrips=m_currentTrips.size();
        bool inDCSlist=false;
        for(size_t i=0;i<ntrips;++i){
            if(m_currentTrips[i].eta1==trip.eta1 &&
                    m_currentTrips[i].eta2==trip.eta2 &&
                    m_currentTrips[i].phi1==trip.phi1 &&
                    m_currentTrips[i].phi2==trip.phi2 ){
                inDCSlist=true;
                break;
            }
        }
        if(!inDCSlist){
            m_currentTrips.push_back(trip);
            if(m_verbosity==Debug){
                const string partition[4]={
                   "LBA",
                   "LBC",
                   "EBA",
                   "EBC"
                };
                msg<<"Read masked module "<<partition[part]<<setfill('0')<<setw(2)<<currentMod+1<<
                        " region "<<trip.eta1<<" <= eta <= "<<trip.eta2<<" "<<
                        trip.phi1<<" <= phi <= "<<trip.phi2<<endl;
            } 
        }
        ++currentMod;
    }
    
    if(m_verbosity==Debug){
        size_t ntrips=m_currentTrips.size();
        msg<<"Total masked after reading tileFlags = "<<ntrips<<endl;
        for(size_t i=0;i<ntrips;++i){
            TripRegion& trip=m_currentTrips[i];
            msg<<"Region "<<i<<" "<<trip.eta1<<" <= eta <= "<<trip.eta2<<" "<<
                        trip.phi1<<" <= phi <= "<<trip.phi2<<endl;
        }
    }
    
    if(greaterMod<0)return;
    currentMod=0;
    while(currentMod<=greaterMod){
        TripRegion trip=partModToEtaPhi(part,currentMod);
        size_t ntrips=m_currentTrips.size();
        bool inDCSlist=false;
        for(size_t i=0;i<ntrips;++i){
            if(m_currentTrips[i].eta1==trip.eta1 &&
                    m_currentTrips[i].eta2==trip.eta2 &&
                    m_currentTrips[i].phi1==trip.phi1 &&
                    m_currentTrips[i].phi2==trip.phi2 ){
                inDCSlist=true;
                break;
            }
        }
        if(!inDCSlist){
            m_currentTrips.push_back(trip);
            if(m_verbosity==Debug){
                const string partition[4]={
                   "LBA",
                   "LBC",
                   "EBA",
                   "EBC"
                };
                msg<<"Read masked module "<<partition[part]<<setfill('0')<<setw(2)<<currentMod+1<<
                        " region "<<trip.eta1<<" <= eta <= "<<trip.eta2<<" "<<
                        trip.phi1<<" <= phi <= "<<trip.phi2<<endl;
            } 
        }
        ++currentMod;
    }
    
    if(m_verbosity==Debug){
        size_t ntrips=m_currentTrips.size();
        msg<<"Total masked after reading tileFlags = "<<ntrips<<endl;
        for(size_t i=0;i<ntrips;++i){
            TripRegion& trip=m_currentTrips[i];
            msg<<"Region "<<i<<" "<<trip.eta1<<" <= eta <= "<<trip.eta2<<" "<<
                        trip.phi1<<" <= phi <= "<<trip.phi2<<endl;
        }
    }
}

int TTileTripReader::setTripFile(const char* file){
    ostream& msg=*m_msglog;
    if(m_ChainsLoaded){
        msg<<"ERROR:  Cannot add more trip files to the chain after you have closed the file and loaded it into memory!\n";
        return 0;
    }
    int files_connected=0;
    files_connected=((TChain*)m_trips)->Add(file);
    if(files_connected!=((TChain*)m_runMap)->Add(file))
        msg<<"Connected file missmatch.\n";
    
    m_mapRun=0;
    m_FirstEntry=0;
    m_runMap->SetBranchAddress("Run",&m_mapRun);
    m_runMap->SetBranchAddress("FirstEntry",&m_FirstEntry);
    
    m_Run=0;
    m_Partition=0;
    m_Module=0;
    m_LumiStart=0;
    m_LumiEnd=0;
    m_trips->SetBranchAddress("Run",&m_Run);
    m_trips->SetBranchAddress("Partition",&m_Partition);
    m_trips->SetBranchAddress("Module",&m_Module);
    m_trips->SetBranchAddress("LumiStart",&m_LumiStart);
    m_trips->SetBranchAddress("LumiEnd",&m_LumiEnd);
    
    return files_connected;
}

void TTileTripReader::setBadEventList(){
    m_badEvents[211620][ 809] = 30918608;
    m_badEvents[211620][ 911] = 50250818;
    m_badEvents[211670][ 170] = 23991666;
    m_badEvents[211670][ 249] = 32887308;
    m_badEvents[211697][ 296] = 20481958;
    m_badEvents[211772][ 453] = 24016459;
    m_badEvents[211772][ 487] = 32756300;
    m_badEvents[211772][ 537] = 43708856;
    m_badEvents[211772][ 665] = 66256038;
    m_badEvents[211772][ 677] = 69195161;
    m_badEvents[211772][ 682] = 70495320;
    m_badEvents[211772][ 713] = 77315023;
    m_badEvents[211787][ 137] = 25075877;
    m_badEvents[211787][ 156] = 27722626;
    m_badEvents[211787][ 231] = 45498134;
    m_badEvents[211787][ 327] = 70283778;
    m_badEvents[211787][ 341] = 73478541;
    m_badEvents[211787][ 379] = 78891936;
    m_badEvents[211787][ 495] = 104208286;
    m_badEvents[211787][ 533] = 112676423;
    m_badEvents[211787][ 606] = 125346821;
    m_badEvents[211787][ 674] = 134529732;
    m_badEvents[211787][ 684] = 136745813;
    m_badEvents[211787][ 687] = 137325328;
    m_badEvents[211787][ 705] = 141053713;
    m_badEvents[211787][ 737] = 145106825;
    m_badEvents[211787][ 781] = 151535382;
    m_badEvents[211787][ 808] = 155908888;
    m_badEvents[211787][ 833] = 160751251;
    m_badEvents[211867][ 415] = 118577411;
    m_badEvents[211867][ 446] = 126968304;
    m_badEvents[211867][ 476] = 135267293;
    m_badEvents[211867][ 488] = 137377078;
    m_badEvents[211867][ 504] = 138631284;
    m_badEvents[211867][ 585] = 156060507;
    m_badEvents[211867][ 601] = 159474909;
    m_badEvents[211867][ 643] = 166995382;
    m_badEvents[211867][ 653] = 169150580;
    m_badEvents[211867][ 664] = 171728297;
    m_badEvents[211867][ 682] = 175037695;
    m_badEvents[211867][ 697] = 177341927;
    m_badEvents[211867][ 708] = 179982739;
    m_badEvents[211867][ 869] = 205041330;
    m_badEvents[211867][ 979] = 217122251;
    m_badEvents[211867][1080] = 235986744;
    m_badEvents[211937][ 273] = 79579535;
    m_badEvents[211937][ 311] = 91096963;
    m_badEvents[211937][ 344] = 100555317;
    m_badEvents[211937][ 363] = 106299191;
    m_badEvents[211937][ 433] = 126166771;
    m_badEvents[211937][ 442] = 128380852;
    m_badEvents[211937][ 539] = 149037480;
    m_badEvents[211937][ 573] = 154820435;
    m_badEvents[211937][ 626] = 163148429;
    m_badEvents[211937][ 681] = 172136294;
    m_badEvents[212034][ 268] = 61304416;
    m_badEvents[212034][ 295] = 70488269;
    m_badEvents[212034][ 373] = 92815632;
    m_badEvents[212034][ 377] = 94077220;
    m_badEvents[212034][ 447] = 110479727;
    m_badEvents[212034][ 449] = 111039026;
    m_badEvents[212103][ 265] = 74171827;
    m_badEvents[212142][ 159] = 42259370;
    m_badEvents[212142][ 187] = 49777048;
    m_badEvents[212142][ 241] = 55650409;
    m_badEvents[212142][ 251] = 58080132;
    m_badEvents[212142][ 259] = 60326167;
    m_badEvents[212142][ 305] = 70626642;
    m_badEvents[212144][ 336] = 82794753;
    m_badEvents[212144][ 555] = 123722841;
    m_badEvents[212144][ 594] = 129844287;
    m_badEvents[212144][ 904] = 184774366;
    m_badEvents[212144][ 946] = 193059339;
    m_badEvents[212144][ 981] = 199214801;
    m_badEvents[212144][ 994] = 201867125;
    m_badEvents[212144][1094] = 218050419;
    m_badEvents[212144][1221] = 237399658;
    m_badEvents[212172][ 128] = 34183408;
    m_badEvents[212172][ 133] = 35958783;
    m_badEvents[212172][ 274] = 68520156;
    m_badEvents[212172][ 283] = 71005981;
    m_badEvents[212172][ 303] = 75609633;
    m_badEvents[212172][ 435] = 102794917;
    m_badEvents[212172][ 453] = 106963891;
    m_badEvents[212172][ 587] = 133067590;
    m_badEvents[212172][ 787] = 165298506;
    m_badEvents[212172][ 825] = 172232714;
    m_badEvents[212199][ 131] = 34583718;
    m_badEvents[212199][ 138] = 36561516;
    m_badEvents[212199][ 165] = 45222651;
    m_badEvents[212199][ 368] = 97625582;
    m_badEvents[212199][ 400] = 105175388;
    m_badEvents[212199][ 414] = 108306676;
    m_badEvents[212199][ 479] = 122276322;
    m_badEvents[212199][ 485] = 123361057;
    m_badEvents[212199][ 557] = 139813253;
    m_badEvents[212199][ 628] = 154879540;
    m_badEvents[212272][ 399] = 112893675;
    m_badEvents[212272][ 402] = 113746110;
    m_badEvents[212272][ 467] = 132448931;
    m_badEvents[212272][ 491] = 139054835;
    m_badEvents[212272][ 504] = 142569344;
    m_badEvents[212272][ 520] = 146768355;
    m_badEvents[212272][ 622] = 170422008;
    m_badEvents[212272][ 656] = 177673918;
    m_badEvents[212272][ 824] = 213994461;
    m_badEvents[212272][ 893] = 227713742;
    m_badEvents[212663][ 312] = 79790422;
    m_badEvents[212663][ 316] = 80951546;
    m_badEvents[212663][ 326] = 83989774;
    m_badEvents[212663][ 366] = 94212644;
    m_badEvents[212663][ 381] = 97930421;
    m_badEvents[212663][ 383] = 98521948;
    m_badEvents[212663][ 387] = 99677557;
    m_badEvents[212687][ 114] = 17058218;
    m_badEvents[212687][ 126] = 21243793;
    m_badEvents[212687][ 168] = 32985216;
    m_badEvents[212687][ 271] = 58155185;
    m_badEvents[212687][ 323] = 70331631;
    m_badEvents[212687][ 357] = 77176767;
    m_badEvents[212721][ 201] = 52039190;
    m_badEvents[212721][ 220] = 58603120;
    m_badEvents[212721][ 246] = 65278591;
    m_badEvents[212721][ 252] = 67402330;
    m_badEvents[212721][ 289] = 75228740;
    m_badEvents[212721][ 319] = 82842050;
    m_badEvents[212721][ 479] = 122996628;
    m_badEvents[212721][ 565] = 142364781;
    m_badEvents[212721][ 604] = 150988998;
    m_badEvents[212721][ 632] = 155954020;
    m_badEvents[212742][ 207] = 60144748;
    m_badEvents[212742][ 223] = 62405230;
    m_badEvents[212742][ 241] = 68197732;
    m_badEvents[212742][ 293] = 83308673;
    m_badEvents[212742][ 295] = 83626188;
    m_badEvents[212742][ 317] = 88821259;
    m_badEvents[212742][ 331] = 92381337;
    m_badEvents[212742][ 560] = 141298623;
    m_badEvents[212742][ 565] = 142245925;
    m_badEvents[212742][ 647] = 157663517;
    m_badEvents[212809][ 303] = 88340723;
    m_badEvents[212815][ 109] = 31575524;
    m_badEvents[212815][ 165] = 49225559;
    m_badEvents[212815][ 204] = 60227314;
    m_badEvents[212815][ 205] = 60579290;
    m_badEvents[212815][ 225] = 65242249;
    m_badEvents[212815][ 227] = 65919654;
    m_badEvents[212815][ 274] = 76382992;
    m_badEvents[212815][ 406] = 105164459;
    m_badEvents[212815][ 631] = 150577349;
    m_badEvents[212815][ 638] = 151728427;
    m_badEvents[212858][ 222] = 62152125;
    m_badEvents[212858][ 225] = 62891044;
    m_badEvents[212858][ 274] = 77090238;
    m_badEvents[212858][ 362] = 99938214;
    m_badEvents[212858][ 379] = 103224773;
    m_badEvents[212858][ 646] = 160856798;
    m_badEvents[212858][ 777] = 184840226;
    m_badEvents[212858][ 801] = 189409163;
    m_badEvents[212858][ 885] = 204054306;
    m_badEvents[212858][ 951] = 214360091;
    m_badEvents[212967][ 131] = 36562258;
    m_badEvents[212967][ 257] = 63562358;
    m_badEvents[212967][ 277] = 67422524;
    m_badEvents[212967][ 389] = 92025946;
    m_badEvents[212967][ 433] = 100838299;
    m_badEvents[212993][ 163] = 46421469;
    m_badEvents[212993][ 244] = 71271803;
    m_badEvents[212993][ 290] = 83452971;
    m_badEvents[212993][ 433] = 114146865;
    m_badEvents[212993][ 495] = 127176803;
    m_badEvents[212993][ 518] = 132055043;
    m_badEvents[212993][ 711] = 161451113;
    m_badEvents[213039][ 435] = 123595163;
    m_badEvents[213039][ 442] = 125966331;
    m_badEvents[213039][ 444] = 126635986;
    m_badEvents[213039][ 449] = 127047496;
    m_badEvents[213039][ 491] = 139509462;
    m_badEvents[213039][ 506] = 143494956;
    m_badEvents[213039][ 508] = 144037393;
    m_badEvents[213039][ 664] = 183458266;
    m_badEvents[213039][ 672] = 185493703;
    m_badEvents[213039][ 676] = 186652045;
    m_badEvents[213039][ 810] = 218014014;
    m_badEvents[213039][ 844] = 226420789;
    m_badEvents[213039][ 942] = 248131356;
    m_badEvents[213039][ 984] = 255977737;
    m_badEvents[213039][ 985] = 256259471;
    m_badEvents[213039][1028] = 264763402;
    m_badEvents[213039][1091] = 278267871;
    m_badEvents[213079][ 76] = 21118131;
    m_badEvents[213079][ 93] = 26924960;
    m_badEvents[213079][ 107] = 31629850;
    m_badEvents[213079][ 108] = 32121270;
    m_badEvents[213079][ 142] = 42417897;
    m_badEvents[213079][ 253] = 68071791;
    m_badEvents[213079][ 388] = 97942628;
    m_badEvents[213079][ 406] = 101552414;
    m_badEvents[213079][ 446] = 109435226;
    m_badEvents[213079][ 491] = 118065478;
    m_badEvents[213092][ 304] = 89221417;
    m_badEvents[213092][ 329] = 98196863;
    m_badEvents[213092][ 336] = 100870506;
    m_badEvents[213092][ 413] = 125456520;
    m_badEvents[213092][ 428] = 128742710;
    m_badEvents[213092][ 442] = 131280992;
    m_badEvents[213092][ 455] = 135005521;
    m_badEvents[213092][ 456] = 135419301;
    m_badEvents[213092][ 461] = 136517543;
    m_badEvents[213130][ 225] = 66606499;
    m_badEvents[213130][ 238] = 70706931;
    m_badEvents[213130][ 302] = 87362821;
    m_badEvents[213130][ 369] = 104601473;
    m_badEvents[213155][ 161] = 45502898;
    m_badEvents[213155][ 226] = 65665665;
    m_badEvents[213155][ 253] = 74594415;
    m_badEvents[213155][ 271] = 79960342;
    m_badEvents[213157][ 96] = 27645561;
    m_badEvents[213157][ 97] = 27788698;
    m_badEvents[213157][ 173] = 51882395;
    m_badEvents[213157][ 175] = 52411595;
    m_badEvents[213157][ 197] = 57234239;
    m_badEvents[213157][ 206] = 58514150;
    m_badEvents[213157][ 242] = 63663521;
    m_badEvents[213157][ 255] = 67535487;
    m_badEvents[213157][ 270] = 71637795;
    m_badEvents[213250][ 205] = 60520332;
    m_badEvents[213250][ 222] = 65257465;
    m_badEvents[213250][ 247] = 72263290;
    m_badEvents[213250][ 320] = 92907476;
    m_badEvents[213250][ 363] = 103329459;
    m_badEvents[213250][ 382] = 108284785;
    m_badEvents[213250][ 451] = 128056187;
    m_badEvents[213314][ 409] = 124373091;
    m_badEvents[213359][ 46] = 11973418;
    m_badEvents[213359][ 100] = 29627794;
    m_badEvents[213359][ 134] = 41636632;
    m_badEvents[213431][ 371] = 113733490;
    m_badEvents[213431][ 383] = 117357719;
    m_badEvents[213486][ 226] = 62935200;
    m_badEvents[213486][ 356] = 98460803;
    m_badEvents[213486][ 369] = 103161912;
    m_badEvents[213486][ 408] = 116687821;
    m_badEvents[213486][ 443] = 127413823;
    m_badEvents[213486][ 470] = 136352132;
    m_badEvents[213486][ 605] = 172136447;
    m_badEvents[213539][ 166] = 48261169;
    m_badEvents[213539][ 343] = 96085689;
    m_badEvents[213539][ 364] = 101400979;
    m_badEvents[213539][ 401] = 110923015;
    m_badEvents[213539][ 437] = 122855066;
    m_badEvents[213539][ 457] = 129277962;
    m_badEvents[213539][ 471] = 132857115;
    m_badEvents[213539][ 596] = 174867207;
    m_badEvents[213539][ 702] = 206952674;
    m_badEvents[213539][ 722] = 213262287;
    m_badEvents[213539][ 867] = 257284352;
    m_badEvents[213539][ 918] = 271585195;
    m_badEvents[213539][ 935] = 276532153;
    m_badEvents[213539][1062] = 311135385;
    m_badEvents[213539][1093] = 319727565;
    m_badEvents[213539][1152] = 336490953;
    m_badEvents[213627][ 272] = 80425704;
    m_badEvents[213640][ 56] = 12005902;
    m_badEvents[213640][ 70] = 14918463;
    m_badEvents[213640][ 81] = 18079040;
    m_badEvents[213640][ 163] = 39220054;
    m_badEvents[213684][ 253] = 70685007;
    m_badEvents[213695][ 260] = 78929129;
    m_badEvents[213695][ 275] = 83295843;
    m_badEvents[213695][ 395] = 118353505;
    m_badEvents[213695][ 422] = 127780110;
    m_badEvents[213702][ 224] = 65859325;
    m_badEvents[213702][ 234] = 69356479;
    m_badEvents[213702][ 241] = 71928803;
    m_badEvents[213754][ 439] = 130286845;
    m_badEvents[213754][ 468] = 139658841;
    m_badEvents[213754][ 487] = 145336905;
    m_badEvents[213754][ 501] = 149295259;
    m_badEvents[213754][ 513] = 152831111;
    m_badEvents[213754][ 530] = 158255438;
    m_badEvents[213754][ 661] = 194754352;
    m_badEvents[213754][ 668] = 197140774;
    m_badEvents[213754][ 826] = 239811696;
    m_badEvents[213754][ 881] = 258669913;
    m_badEvents[213754][ 938] = 277098450;
    m_badEvents[213754][1142] = 338925991;
    m_badEvents[213754][1329] = 392113583;
    m_badEvents[213796][ 158] = 44507365;
    m_badEvents[213796][ 206] = 58985238;
    m_badEvents[213796][ 301] = 83448459;
    m_badEvents[213796][ 333] = 93593176;
    m_badEvents[213796][ 360] = 99781558;
    m_badEvents[213816][ 192] = 55406360;
    m_badEvents[213816][ 269] = 78481689;
    m_badEvents[213816][ 283] = 82641406;
    m_badEvents[213816][ 370] = 108457578;
    m_badEvents[213819][ 160] = 46811285;
    m_badEvents[213819][ 180] = 52871721;
    m_badEvents[213819][ 202] = 59259282;
    m_badEvents[213819][ 239] = 68706061;
    m_badEvents[213819][ 280] = 81117930;
    m_badEvents[213819][ 305] = 87854925;
    m_badEvents[213819][ 332] = 95539190;
    m_badEvents[213900][ 324] = 96085442;
    m_badEvents[213900][ 349] = 104773361;
    m_badEvents[213900][ 373] = 110881775;
    m_badEvents[213900][ 475] = 139955858;
    m_badEvents[213900][ 491] = 144250720;
    m_badEvents[213900][ 561] = 167029804;
    m_badEvents[213900][ 624] = 186510324;
    m_badEvents[213900][ 722] = 218591613;
    m_badEvents[213900][ 734] = 221225189;
    m_badEvents[213900][ 871] = 263297620;
    m_badEvents[213951][ 271] = 79566293;
    m_badEvents[213951][ 401] = 118812783;
    m_badEvents[213951][ 441] = 128654993;
    m_badEvents[213951][ 457] = 132288337;
    m_badEvents[213951][ 571] = 154551203;
    m_badEvents[213951][ 620] = 169085961;
    m_badEvents[213964][ 52] = 13462948;
    m_badEvents[213968][ 179] = 52179074;
    m_badEvents[213968][ 206] = 60769634;
    m_badEvents[214021][ 377] = 111694242;
    m_badEvents[214021][ 393] = 116031010;
    m_badEvents[214021][ 515] = 151050310;
    m_badEvents[214021][ 529] = 154745174;
    m_badEvents[214021][ 531] = 155421765;
    m_badEvents[214021][ 577] = 167678775;
    m_badEvents[214021][ 687] = 201372055;
    m_badEvents[214021][ 753] = 223504090;
    m_badEvents[214021][ 862] = 256032256;
    m_badEvents[214021][1077] = 315933922;
    m_badEvents[214086][ 198] = 58661578;
    m_badEvents[214086][ 222] = 65915113;
    m_badEvents[214086][ 265] = 78838619;
    m_badEvents[214086][ 312] = 90367641;
    m_badEvents[214086][ 337] = 97701655;
    m_badEvents[214086][ 398] = 117844979;
    m_badEvents[214086][ 405] = 120239052;
    m_badEvents[214086][ 505] = 151404219;
    m_badEvents[214086][ 511] = 152654874;
    m_badEvents[214086][ 583] = 175508847;
    m_badEvents[214086][ 620] = 187280092;
    m_badEvents[214086][ 862] = 259666334;
    m_badEvents[214086][ 935] = 280635242;
    m_badEvents[214086][ 949] = 284662039;
    m_badEvents[214086][1152] = 337584653;
    m_badEvents[214160][ 120] = 35196096;
    m_badEvents[214160][ 132] = 39644970;
    m_badEvents[214160][ 304] = 91735548;
    m_badEvents[214160][ 446] = 134801908;
    m_badEvents[214160][ 489] = 148028079;
    m_badEvents[214160][ 673] = 208029920;
    m_badEvents[214160][ 713] = 219965263;
    m_badEvents[214176][ 348] = 101831142;
    m_badEvents[214216][ 315] = 92809021;
    m_badEvents[214216][ 327] = 95746910;
    m_badEvents[214216][ 409] = 119902265;
    m_badEvents[214216][ 426] = 124577717;
    m_badEvents[214216][ 443] = 129177081;
    m_badEvents[214216][ 524] = 157789464;
    m_badEvents[214216][ 604] = 182047256;
    m_badEvents[214216][ 609] = 183268716;
    m_badEvents[214216][ 620] = 186867324;
    m_badEvents[214216][ 643] = 194803186;
    m_badEvents[214216][ 654] = 198743354;
    m_badEvents[214216][ 693] = 211156533;
    m_badEvents[214216][ 718] = 219502607;
    m_badEvents[214388][ 482] = 141016617;
    m_badEvents[214388][ 512] = 149507370;
    m_badEvents[214388][ 529] = 154431800;
    m_badEvents[214388][ 546] = 160010331;
    m_badEvents[214390][ 126] = 30550658;
    m_badEvents[214390][ 165] = 43024151;
    m_badEvents[214390][ 396] = 106759792;
    m_badEvents[214494][ 109] = 29478633;
    m_badEvents[214494][ 139] = 39122996;
    m_badEvents[214494][ 153] = 42985263;
    m_badEvents[214494][ 235] = 67626184;
    m_badEvents[214494][ 240] = 68658006;
    m_badEvents[214494][ 361] = 107230032;
    m_badEvents[214494][ 376] = 112238214;
    m_badEvents[214494][ 388] = 115761949;
    m_badEvents[214494][ 438] = 130702325;
    m_badEvents[214494][ 469] = 140339628;
    m_badEvents[214494][ 478] = 143408490;
    m_badEvents[214494][ 528] = 158894657;
    m_badEvents[214494][ 628] = 187805212;
    m_badEvents[214523][ 190] = 50181037;
    m_badEvents[214523][ 208] = 55621427;
    m_badEvents[214523][ 215] = 57401419;
    m_badEvents[214523][ 227] = 61119795;
    m_badEvents[214523][ 293] = 80518599;
    m_badEvents[214523][ 306] = 83568653;
    m_badEvents[214523][ 358] = 97821922;
    m_badEvents[214523][ 383] = 105821339;
    m_badEvents[214523][ 608] = 175714199;
    m_badEvents[214523][ 649] = 187930472;
    m_badEvents[214523][ 692] = 201168808;
    m_badEvents[214523][ 707] = 205911433;
    m_badEvents[214544][ 121] = 32531579;
    m_badEvents[214544][ 128] = 34278960;
    m_badEvents[214544][ 196] = 52990693;
    m_badEvents[214544][ 238] = 63202772;
    m_badEvents[214544][ 264] = 69547403;
    m_badEvents[214544][ 309] = 80169757;
    m_badEvents[214544][ 358] = 95684444;
    m_badEvents[214544][ 415] = 113028399;
    m_badEvents[214544][ 425] = 116453891;
    m_badEvents[214544][ 512] = 144322637;
    m_badEvents[214544][ 530] = 150403783;
    m_badEvents[214544][ 551] = 156914594;
    m_badEvents[214544][ 581] = 165650856;
    m_badEvents[214544][ 610] = 173948673;
    m_badEvents[214544][ 663] = 189356210;
    m_badEvents[214553][ 81] = 21848484;
    m_badEvents[214553][ 107] = 28867085;
    m_badEvents[214553][ 143] = 34975171;
    m_badEvents[214553][ 175] = 43375728;
    m_badEvents[214553][ 235] = 55710673;
    m_badEvents[214553][ 278] = 65777105;
    m_badEvents[214553][ 336] = 83565966;
    m_badEvents[214553][ 351] = 88977043;
    m_badEvents[214553][ 352] = 89040957;
    m_badEvents[214553][ 392] = 101340699;
    m_badEvents[214553][ 403] = 104486789;
    m_badEvents[214553][ 465] = 123363220;
    m_badEvents[214553][ 498] = 133139714;
    m_badEvents[214553][ 519] = 140070084;
    m_badEvents[214553][ 673] = 185284263;
    m_badEvents[214553][ 678] = 186353867;
    m_badEvents[214553][ 694] = 191167020;

}

}
