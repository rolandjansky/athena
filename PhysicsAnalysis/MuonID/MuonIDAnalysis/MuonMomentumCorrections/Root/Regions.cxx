/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include "MuonMomentumCorrections/Regions.h"

using namespace std;

void Regions::PrintRegions() const {
    if(m_doMacroRegions){
	for(map<int, int>::const_iterator it = m_macroRegionIdxMap.begin();
	    it!=m_macroRegionIdxMap.end(); ++it){
	    int bReg = it->first, mReg = it->second;
	    cout<<"Base region n "<<bReg<<": "<<endl
		     <<"phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg]
		     <<", eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg];
	    if(m_loadNames) cout<<", name="<<m_names[bReg];
	    cout<<endl<<"included in Macro region N "<<mReg<<" with innerEta="<<m_macroRegionInnerEta[mReg];
	    if(m_loadNames) cout<<", name="<<m_macroRegionName[mReg];
	    cout<<endl;
	}
    } else {
	for(int bReg=0; bReg<m_nb_regions; ++bReg){
	    cout<<"Base region n "<<bReg<<": phi_min="<<m_phi_min[bReg]<<", phi_max="<<m_phi_max[bReg]
		<<", eta_min="<<m_eta_min[bReg]<<", eta_max="<<m_eta_max[bReg];
	    if(m_loadNames) cout<<", name="<<m_names[bReg];
	    cout<<endl;
	}
    }
}

unsigned int Regions::GetNRegions() const {
    if(m_doMacroRegions) return m_macroRegionName.size();
    else return m_nb_regions;
}

Regions::Regions(string inRegionFile, int doMacroRegionsFlag){
    m_eta_min.clear(); m_eta_max.clear(); m_phi_min.clear(); m_phi_max.clear();
    m_loadNames = false;
    m_doMacroRegions = false;
    m_useAbsEta = false; 
    verb = false;
    string tmpname;
    double tmpval;
    int i=0;
    
    ifstream inFile(inRegionFile.c_str());
    if(!inFile.good()){
	cerr<<__FILE__<<" line:"<<__LINE__<<" "<<"ERROR:: File "<<inRegionFile<<" NOT FOUND!!"<<endl;
	exit(-1);
    } else {
	while(!inFile.eof()){
	    tmpval=0; tmpname="";
	    if(i==0){
		getline(inFile,tmpname);
		//Check if I am loading the name of the regions in the file
		if(tmpname.find("name") != string::npos && !m_loadNames) m_loadNames=true;
		if(verb) cout<<"tmpname: "<<tmpname<<endl;
	    } else {
		inFile>>tmpval;
		if(inFile.eof()) break;//this will break immediatly as soon as the new line is empty
		m_eta_min.push_back(tmpval);
		if(verb) cout<<"i="<<i<<"; eta min: "<<tmpval<<endl;
		inFile>>tmpval;
		m_eta_max.push_back(tmpval);
		if(verb) cout<<"i="<<i<<"; eta max: "<<tmpval<<endl;
		inFile>>tmpval;
		m_phi_min.push_back(tmpval);
		if(verb) cout<<"i="<<i<<"; phi min: "<<tmpval<<endl;
		inFile>>tmpval;
		m_phi_max.push_back(tmpval);
		if(verb) cout<<"i="<<i<<"; phi max: "<<tmpval<<endl;
		if(m_loadNames){//Fill the name vector only if you have them!
		    inFile>>tmpname;
		    m_names.push_back(tmpname);	
		    if(verb) cout<<"tmpname: "<<tmpname<<endl;
		}
	    }
	    ++i;
	}
    }
    inFile.close();
    m_nb_regions = (int)m_eta_min.size();
    std::cout << std::endl << m_nb_regions << std::endl << std::endl;
    if(doMacroRegionsFlag){//In case I want macroRegions, I need to define the criteria here
	m_doMacroRegions =true;
	switch(doMacroRegionsFlag){
	case 1:
	    if(verb) cout<<"collectMacroRegionsSL()"<<endl;
	    //Collects all the Large and Small sectors if they belong to the same eta bin
	    collectMacroRegionsSL(); break;
	case 2:
	    if(verb) cout<<"collectMacroRegionsSL_SplitBAR()"<<endl;
	    //Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
	    collectMacroRegionsSL_SplitBAR(); break;
	case 3:
	    if(verb) cout<<"collectMacroRegionsSL_UpDn()"<<endl;
	    //Collects all the Large and Small, Up and Down sectors if they belong to the same eta bin
	    collectMacroRegionsSL_UpDn(); break;
	case 4: 
	    if(verb) cout<<"using fabs(eta)"<<endl;
	    m_useAbsEta = true; 
	    m_doMacroRegions =false; //not really using macroRegions
	    break;
	case 5: 
	    if(verb) cout<<"using sectors!"<<endl;
	    useSectors(); break;
	    break;
	default:
	    cerr<<"ERROR: doMacroRegionFlag="<<doMacroRegionsFlag<<" not defined!"<<endl<<endl;
	    exit(-1); break;
	}
    }
}
    
int Regions::GetRegion(double eta, const double phi) const{
    int ret_k =-1;
    if(m_useAbsEta) eta = fabs(eta);
    for(int k=0; k < m_nb_regions; ++k){    
	if( eta >= m_eta_min[k] && eta < m_eta_max[k] ){
	    if( m_phi_min[k] <= m_phi_max[k] ){//Standard case
		if(phi>=m_phi_min[k] && phi<m_phi_max[k] ) {
		    ret_k = k; 
		    break;
		}
	    } else {//Circular closure of Phi, min and max are excanged in the .dat file
		if(phi>=m_phi_min[k] || phi<m_phi_max[k] ) {
		    ret_k = k;
		    break;
		}
	    }
	}
    }

    if(ret_k == -1){
	cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region corresponding to Eta="<<eta<<", Phi="<<phi<<" NOT FOUND!"<<endl;
	return -1;
    }
    if(m_doMacroRegions) return m_macroRegionIdxMap.find(ret_k)->second;
    return ret_k;	
}

float Regions::GetRegionDeltaEta(const int r_i) const{ //Return Eta range of the region
    if(m_doMacroRegions) {
	if(r_i>=0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionDeltaEta[r_i];
    } else {
	if(r_i >= 0 && r_i < m_nb_regions)
	    return fabs(m_eta_max[r_i] - m_eta_min[r_i]);	
    }
    cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region Inner Eta corresponding to Region index="<<r_i<<" NOT FOUND!"<<endl;
    return -999.;
}

float Regions::GetRegionInnerEta(const int r_i) const{//Return Eta closer to the origin
    if(m_doMacroRegions) {
	if(r_i>=0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionInnerEta[r_i];
    } else {
	if(r_i >= 0 && r_i < m_nb_regions) {
	    if( m_eta_min[r_i] >= 0. ) return m_eta_min[r_i];
	    else return m_eta_max[r_i];
	}
    }
    cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region Inner Eta corresponding to Region index="<<r_i<<" NOT FOUND!"<<endl;
    return -999.;
}


string Regions::GetRegionName(const int r_i) const{
    if(m_loadNames){
	if(m_doMacroRegions) {
	    if(r_i>=0 && r_i < (int)m_macroRegionName.size()) return m_macroRegionName[r_i];
	} else {
	    if(r_i>=0 && r_i < m_nb_regions) return m_names[r_i];
	}
	cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region Name corresponding to Region index="<<r_i<<" NOT FOUND!"<<endl;
    }
    cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region Names are not set!"<<endl;
    return "NAN";
}

string Regions::GetRegionName(const double eta, const double phi) const{  
    if(m_loadNames) return GetRegionName(GetRegion(eta, phi));
    cerr<<__FILE__<<" "<<__LINE__<<" WARNING: Region Names are not set!"<<endl;
    return "NAN";
}

//Collects all the Large and Small sectors if they belong to the same eta bin
void Regions::collectMacroRegionsSL(){
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for(int k=0; k < m_nb_regions; ++k){
	if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ){
	    //Build a new macroRegion
	    etaMin = m_eta_min[k];	etaMax = m_eta_max[k];
	    deltaEta = fabs(m_eta_max[k] - m_eta_min[k]);
	    //One of the following is true
	    string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
	    m_macroRegionName.push_back(macroRegName+"Large");
	    m_macroRegionName.push_back(macroRegName+"Small");
	    //insert twice the innerEta, for Large and for Small sectors
	    for(int i=0; i<2; ++i){		    
		if( etaMin >= 0. ) m_macroRegionInnerEta.push_back(etaMin);
		else 	       m_macroRegionInnerEta.push_back(etaMax);
		m_macroRegionDeltaEta.push_back(deltaEta);
	    }
	    macroRegionIdx+=2;
	    if(verb) cout<<"k="<<k<<": etaMin="<<etaMin<<"; etaMax="<<etaMax
			      <<"; m_macroRegionName.size()="<<m_macroRegionName.size()
			      <<"; macroRegionIdx"<<macroRegionIdx<<"; m_macroRegionInnerEta["<<macroRegionIdx<<"]="
			      <<m_macroRegionInnerEta[macroRegionIdx]<<endl;
	}

	if( m_names[k].find("EL") != string::npos || m_names[k].find("BL") != string::npos)
	    m_macroRegionIdxMap[k] = macroRegionIdx-2;//Large sectors
	if( m_names[k].find("ES") != string::npos || m_names[k].find("BS") != string::npos)
	    m_macroRegionIdxMap[k] = macroRegionIdx-1;//Small sectors	
    }
}

//Collects all the Large and Small sectors if they belong to the same eta bin
void Regions::collectMacroRegionsSL_UpDn(){
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for(int k=0; k < m_nb_regions; ++k){
	if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ){
	    //Build a new macroRegion
	    etaMin = m_eta_min[k];	etaMax = m_eta_max[k];
	    deltaEta = fabs(m_eta_max[k] - m_eta_min[k]);
	    //One of the following is true
	    string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
	    m_macroRegionName.push_back(macroRegName+"LargeDn");
	    m_macroRegionName.push_back(macroRegName+"SmallDn");
	    m_macroRegionName.push_back(macroRegName+"LargeUp");
	    m_macroRegionName.push_back(macroRegName+"SmallUp");
	    //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
	    for(int i=0; i<4; ++i){		    
		if( etaMin >= 0. ) m_macroRegionInnerEta.push_back(etaMin);
		else 	       m_macroRegionInnerEta.push_back(etaMax);
		m_macroRegionDeltaEta.push_back(deltaEta);
	    }
	    macroRegionIdx+=4;
	    if(verb) cout<<"k="<<k<<": etaMin="<<etaMin<<"; etaMax="<<etaMax
			      <<"; m_macroRegionName.size()="<<m_macroRegionName.size()
			      <<"; macroRegionIdx"<<macroRegionIdx<<"; m_macroRegionInnerEta["<<macroRegionIdx<<"]="
			      <<m_macroRegionInnerEta[macroRegionIdx]<<endl;
	}
	if( m_names[k].find("EL") != string::npos || m_names[k].find("BL") != string::npos){//Large sectors
	    if(m_names[k].find("11") != string::npos || m_names[k].find("13") != string::npos || 
	       m_names[k].find("15") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
	    }
	}
	if( m_names[k].find("ES") != string::npos || m_names[k].find("BS") != string::npos){//Small sectors
	    if(m_names[k].find("10") != string::npos || m_names[k].find("12") != string::npos || 
	       m_names[k].find("14") != string::npos || m_names[k].find("16") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
	    }	    
	}
    }
}

void Regions::useSectors(){
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for(int k=0; k < m_nb_regions; ++k){
	if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ){
	    //Build a new macroRegion
	    etaMin = m_eta_min[k];	etaMax = m_eta_max[k];
	    deltaEta = fabs(m_eta_max[k] - m_eta_min[k]);
	    //One of the following is true
	    string macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BL"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
	    macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
	    m_macroRegionName.push_back(macroRegName+"LargeDn");
	    m_macroRegionName.push_back(macroRegName+"SmallDn");
	    m_macroRegionName.push_back(macroRegName+"LargeUp");
	    m_macroRegionName.push_back(macroRegName+"SmallUp");
	    //insert 4 time the innerEta, for Large and Small sectors times Up and Dn
	    for(int i=0; i<4; ++i){		    
		if( etaMin >= 0. ) m_macroRegionInnerEta.push_back(etaMin);
		else 	       m_macroRegionInnerEta.push_back(etaMax);
		m_macroRegionDeltaEta.push_back(deltaEta);
	    }
	    macroRegionIdx+=4;
	    if(verb) cout<<"k="<<k<<": etaMin="<<etaMin<<"; etaMax="<<etaMax
			      <<"; m_macroRegionName.size()="<<m_macroRegionName.size()
			      <<"; macroRegionIdx"<<macroRegionIdx<<"; m_macroRegionInnerEta["<<macroRegionIdx<<"]="
			      <<m_macroRegionInnerEta[macroRegionIdx]<<endl;
	}
	if( m_names[k].find("EL") != string::npos || m_names[k].find("BL") != string::npos){//Large sectors
	    if(m_names[k].find("11") != string::npos || m_names[k].find("13") != string::npos || 
	       m_names[k].find("15") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-4;//Down Large sectors (within 10 to 16)
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-2; //Up, large sectors
	    }
	}
	if( m_names[k].find("ES") != string::npos || m_names[k].find("BS") != string::npos){//Small sectors
	    if(m_names[k].find("10") != string::npos || m_names[k].find("12") != string::npos || 
	       m_names[k].find("14") != string::npos || m_names[k].find("16") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-3; //Down Small sectors (from 10 to 16). Should I remove 10 and 16? ++++++
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-1; ; //Up, Small sectors
	    }	    
	}
    }
}

//Collects all the Large and Small sectors if they belong to the same eta bin and splits 
// the barrel 12,14 smalls and 11+15 larges that may need different calibrations
void Regions::collectMacroRegionsSL_SplitBAR(){
    double etaMin = -999., etaMax = -999., deltaEta = -999.;
    int macroRegionIdx = 0;
    for(int k=0; k < m_nb_regions; ++k){
	if( etaMin != m_eta_min[k] || etaMax !=m_eta_max[k] ){ //Build a new macroRegion
	    etaMin = m_eta_min[k];	etaMax = m_eta_max[k];
	    deltaEta = fabs(m_eta_max[k] - m_eta_min[k]);
	    string macroRegName ="";
	    if(m_names[k].find("EL")!= string::npos || m_names[k].find("ES") != string::npos){		
		//We are in the End-Cap sectors so we split only Large and Small sectors
		macroRegName = m_names[k].substr(0, m_names[k].find("EL"));
		macroRegName = macroRegName.substr(0, m_names[k].find("ES"));
		//insert 4 time the innerEta, for Large and Small sectors
		m_macroRegionName.push_back(macroRegName+"Large");
		m_macroRegionName.push_back(macroRegName+"Small");
		for(int i=0; i<2; ++i){		    
		    if( etaMin >= 0. ) m_macroRegionInnerEta.push_back(etaMin);
		    else 	       m_macroRegionInnerEta.push_back(etaMax);
		    m_macroRegionDeltaEta.push_back(deltaEta);
		}
		macroRegionIdx+=2;
	    } else {//We are in the Barrels sectors so we split Large, Small, 12+14, 11+15 sectors
		macroRegName = m_names[k].substr(0, m_names[k].find("BL"));
		macroRegName = macroRegName.substr(0, m_names[k].find("BS"));
		//insert 4 time the innerEta, for Large and Small sectors times Up and Dn
		m_macroRegionName.push_back(macroRegName+"Large");
		m_macroRegionName.push_back(macroRegName+"Small");
		m_macroRegionName.push_back(macroRegName+"Large11_15");
		m_macroRegionName.push_back(macroRegName+"SmallFeet");
		for(int i=0; i<4; ++i){		    
		    if( etaMin >= 0. ) m_macroRegionInnerEta.push_back(etaMin);
		    else 	       m_macroRegionInnerEta.push_back(etaMax);
		    m_macroRegionDeltaEta.push_back(deltaEta);
		}
		macroRegionIdx+=4;
	    }
	    if(verb) cout<<"k="<<k<<": etaMin="<<etaMin<<"; etaMax="<<etaMax<<"; short macroRegName="<<macroRegName
			      <<"; m_macroRegionName.size()="<<m_macroRegionName.size()
			      <<"; macroRegionIdx"<<macroRegionIdx<<"; m_macroRegionInnerEta["<<macroRegionIdx<<"]="
			      <<m_macroRegionInnerEta[macroRegionIdx]<<endl;
	}
	if( m_names[k].find("EL") != string::npos){//End-Cap Large sectors
	    m_macroRegionIdxMap[k] = macroRegionIdx-2;
	} else if( m_names[k].find("ES") != string::npos){//End-Cap Small sectors
	    m_macroRegionIdxMap[k] = macroRegionIdx-1; //
	} else if (m_names[k].find("BL") != string::npos){//Barrel Large sectors
	    if(m_names[k].find("11") != string::npos || m_names[k].find("15") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-2;//Barrel Large sectors with different alignment (11,15)
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-4; //Standard Barrel Large sectors
	    }	    
	} else if( m_names[k].find("BS") != string::npos){//Barrel Small sectors
	    if(m_names[k].find("12") != string::npos || m_names[k].find("14") != string::npos ) {
		m_macroRegionIdxMap[k] = macroRegionIdx-1; //Feet Small sectors (12+14, Down).
	    } else {
		m_macroRegionIdxMap[k] = macroRegionIdx-3; //All standard Barrel Small sectors
	    }	    
	}
    }
}
