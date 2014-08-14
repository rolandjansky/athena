/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
// file:        TauPi0BonnParser 
// package:     Reconstruction/tauEvent
// authors:     Will Davey <will.davey@cern.ch> 
// date:        2012-09-13
//
//-----------------------------------------------------------------------------

#include "tauRec/TauPi0BonnParser.h"

#include "TFile.h"
#include "TParameter.h"
#include "TVectorF.h"
#include "TObjString.h"
#include "TObjArray.h"

#include <iostream>
#include <assert.h>
#include <string>
#include <sstream>
#include <fstream>
#include <stdlib.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TauPi0BonnParser::TauPi0BonnParser()
    : m_max_traceback(1000)
{
}

TauPi0BonnParser::~TauPi0BonnParser()
{
	// because we own the pointer of the histograms, we need to delete them here
	std::map<std::string,TH1*>::iterator it=m_hist_map.begin();
	for (;it!=m_hist_map.end(); ++it) delete it->second;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TauPi0BonnParser::setMaxTraceback(int i){
    m_max_traceback = i;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TauPi0BonnParser::setVar( const string& key, double& val ){
    if(m_curr_val_map.find(key)==m_curr_val_map.end()){
        this->msg( "ERROR: var: "+key+" not in map" ); 
        return false;
    }
    m_curr_val_map[key] = &val;
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH1* TauPi0BonnParser::getTH1(){
    string key = this->getBinKey();
    return m_hist_map[key];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string TauPi0BonnParser::getStream(){
    return m_stream;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TauPi0BonnParser::msg( const string& str ){
    m_stream += str+"\n";
    int length = m_stream.size();
    if( length>m_max_traceback ) m_stream = m_stream.substr(m_stream.size()-m_max_traceback,m_max_traceback);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get bin key from a list of bin indices
string TauPi0BonnParser::getBinKey( const vector<int>& bin_indices ){
    string key = "";
    for( unsigned int j=0; j<bin_indices.size();j++) {
        if( j!=0 ) key += "_";
        key += m_bin_order.at(j);
        key += Form("%d",bin_indices.at(j));
    }
    return key;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get bin key from a list of var values
string TauPi0BonnParser::getBinKey(){
    // get current stored values of input variables
    vector<double> vals = this->getCurrentVals();
    // create vector to store bins corresponding to vals of input vars
    vector<int> bin_indices;
    
    for( unsigned int i=0; i<vals.size();i++ ){
        // get variable name and current value
        string key = m_bin_order.at(i);
        double val = vals.at(i);

        // retrieve binning
        vector<double> bin_edges = m_bin_map[key];

        // determine bin index
        int index = 0;
        for( unsigned int j=0; j<bin_edges.size()-1; j++ ){
            // return index 0 if before first bin
            if( val < bin_edges.front() ) break;
            // return last bin (n-2) if after last bin
            if( val > bin_edges.back() ){
                index = bin_edges.size()-2;
                break;
            }
            if( val >= bin_edges.at(j) && val < bin_edges.at(j+1) ){
                index = j; 
                break;
            }
        }
        bin_indices.push_back(index);
    }
    return this->getBinKey(bin_indices);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<double> TauPi0BonnParser::getCurrentVals(){
    vector<double> current_vals;
    vector<string>::iterator itr = m_bin_order.begin();
    for( ; itr!=m_bin_order.end(); itr++ ){			
        // dont try to access null pointer if user hasn't configured properly
			if ( m_curr_val_map[*itr] == NULL ) {
        current_vals.push_back(-9999.);
			}
			else{
				current_vals.push_back( (*m_curr_val_map[*itr]) );
      }
    }
    return current_vals;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TauPi0BonnParser::parseTDirectory( 
    TDirectory* dir,
    vector<int> current_bin_store,
    int current_index)
{
    // make sure current_bin_store can accommodate map entries
    while( current_bin_store.size()<m_bin_map.size() ) current_bin_store.push_back(0);

    // initialise iterator
    if( current_index == -1 ) current_index = 0;
    // increment iterator if this is a recursion
    else current_index++;
    
    // get binning for current variable
    string key = m_bin_order.at(current_index);
    vector<double> bin_edges = m_bin_map[key];

    // looping over bins for current variable (dont count last bin top edge!)
    for( unsigned int i=0; i<bin_edges.size()-1; i++ ){
        // store the iterator for the current variable (is passed in recursion)
        current_bin_store.at(current_index) = i; 

        // once final variable reached, retrieve info from config file
        if( current_index == (int)(m_bin_order.size()-1) ){
            string bin_key = getBinKey( current_bin_store );
        
            /* 
            // attempt to retrieve from file 
            TVectorF* pvec = (TVectorF*) dir->Get(bin_key.c_str());
            if( !pvec ){
                msg( string("Failed to find ")+bin_key+" in dir.");
                dir->ls();
                return false;
            }

            // store as std::vector
            vector<double> new_vec;
            for( int j=0;j<pvec->GetNoElements();j++) new_vec.push_back((*pvec)(j));
            m_hist_map[bin_key]=new_vec;
            */

            TH1* hist = (TH1*) dir->Get(bin_key.c_str());
            if( !hist ){
                msg( string("Failed to find ")+bin_key+" in dir.");
                dir->ls();
                return false;
            }

            // store as TH1
            hist->SetDirectory(0);
            m_hist_map[bin_key]=(TH1*)hist; //no clone in case SetDirectory(0) is used ->Clone();

        }
        else{
            // recursively call to loop through all binning combinations
            if( !this->parseTDirectory(
                    dir, 
                    current_bin_store,
                    current_index) 
                    )
            {
                msg( "ERROR parsing TDirectory" ); 
                return false;
            }
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TauPi0BonnParser::parseROOTFile( string filename, string directory ){

    // open input file
    TFile* file = TFile::Open( filename.c_str() );
    if( !file ){
        msg( string("cant load file ")+filename );
        return false;
    }

    // get directory
    TDirectory* dir = file->GetDirectory(directory.c_str());
    if( !dir ){
        msg( string("dir: ")+directory+" not in file: "+filename);
        return false;
    }    

    // number of binning variables 
    TParameter<int>* NDIM = (TParameter<int>*)dir->Get("NDIM");
    if( !NDIM ){
        msg( "config file invalid format!"); 
        return false;
    }

    // ';' seperated list of binning variable identifiers 
    TObjString* BIN_VARS = (TObjString*) dir->Get("BIN_VARS");
    if( !BIN_VARS ){
        msg( "config file invalid format!"); 
        return false;
    }
       
    TString BIN_VARS_ts = BIN_VARS->GetString();
    TIterator* BIN_VARS_itr = BIN_VARS_ts.Tokenize(';')->MakeIterator();
    while( TObject* obj = BIN_VARS_itr->Next() ){
        string key = obj->GetName();
        m_bin_map[key] = vector<double>();
        m_curr_val_map[key] = NULL;
        m_bin_order.push_back(key);
        //cout << "key: " << key << endl;
    }

    // load binning for variables 
    map<string,vector<double> >::iterator itrMap = m_bin_map.begin();
    for(; itrMap!=m_bin_map.end(); itrMap++ ){
        itrMap->second.clear();
        TVectorF* vec = (TVectorF*) dir->Get(Form("%s_BINS",itrMap->first.c_str()));
        //assert(vec);
				if (!vec) continue;
        // cout << "itrMap->first = " <<  itrMap->first << endl;
        for( int i=0; i<vec->GetNoElements(); i++ ){
            itrMap->second.push_back( (*vec)(i+1) );
            // cout << "bin" << i << ": " << (*vec)(i+1) << endl;
        }
    }

    // parse all bin combinations in TDirectory
    if( !this->parseTDirectory(dir) ){
        msg( "failed parsing bins in TDirectory - invalid format!");
        return false;
    }

    file->Close();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TauPi0BonnParser::checkConfig( 
    vector<int> current_bin_store,
    int current_index)
{
    // make sure current_bin_store can accommodate map entries
    while( current_bin_store.size()<m_bin_map.size() ) current_bin_store.push_back(0);

    // initialise iterator
    if( current_index == -1 ) current_index = 0;
    // increment iterator if this is a recursion
    else current_index++;
    
    // get binning for current variable
    string key = m_bin_order.at(current_index);
    vector<double> bin_edges = m_bin_map[key];

    // looping over bins for current variable (dont count last bin top edge!)
    for( unsigned int i=0; i<bin_edges.size()-1; i++ ){
        // store the iterator for the current variable (is passed in recursion)
        current_bin_store.at(current_index) = i; 

        // once final variable reached, retrieve info from config file
        if( current_index == (int)(m_bin_order.size()-1) ){
            string bin_key = getBinKey( current_bin_store );
    
            if( m_hist_map.find(bin_key) == m_hist_map.end() ){
                msg( bin_key + " not loaded!" ); 
                return false;
            }
        }
        else{
            // recursively call to loop through all binning combinations
            if( !this->checkConfig(
                    current_bin_store,
                    current_index) 
                    )
            {
                msg( string("failed checkConfig") ); 
                return false;
            }
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool TauPi0BonnParser::checkInitialisationStatus(){
    
    // check at least something loaded
    if( m_bin_order.size() <= 0 ){
        msg( "nothing loaded" );
        return false;
    }

	
    // check addresses set for all corresponding varaibles
    map<string,double*>::iterator itr = m_curr_val_map.begin();
    for( ; itr!=m_curr_val_map.end(); itr++ ){
        if( itr->second == NULL ){
            msg( string("Address of ") + itr->first + " not set!" );
            return false;
        }
    }
  

    if( !this->checkConfig() ){
        msg( "Failed loading variables" );
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TauPi0BonnParser::summary(){
    // FIXME: Write new summary function?
    /*
    map<std::string,TH1*>::iterator itrMap = m_hist_map.begin();
    for( ; itrMap!=m_hist_map.end(); itrMap++ ){
        cout << itrMap->first << ": ";
        for( unsigned int k=0; k<itrMap->second.size(); k++) cout << " " << Form( "%.2f",itrMap->second.at(k));
        cout << endl;
    }
    */
}

