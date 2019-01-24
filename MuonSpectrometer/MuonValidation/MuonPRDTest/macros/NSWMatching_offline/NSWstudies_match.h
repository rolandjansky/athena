/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWstudies_match_h
#define NSWstudies_match_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

using namespace std;

class Flocalize {
private:
public:
   ~Flocalize() {;}
   Flocalize () {
      stationName = string("");
      stationEta = 0;
      stationPhi = 0;
      multiplet = 0;
      gas_gap = 0;
      channel_type = 0;
      channel = 0;
   }
   Flocalize (string i_stationName, int i_stationEta, int i_stationPhi, int i_multiplet, int i_gas_gap, int i_channel, int i_channel_type, int i_matchedchannel) {
      stationName = i_stationName;
      stationEta = i_stationEta;
      stationPhi = i_stationPhi;
      multiplet = i_multiplet;
      gas_gap = i_gas_gap;
      channel_type = i_channel_type;
      channel = i_channel;
      matchedchannel = i_matchedchannel;
   }
   //To do: oveload == operator?
   bool isEqual(Flocalize _other) {
      return ( stationName.compare(_other.stationName) == 0 
               && stationEta == _other.stationEta
               && stationPhi == _other.stationPhi 
               && multiplet == _other.multiplet 
               && gas_gap == _other.gas_gap 
               && channel_type == _other.channel_type 
               //&& channel == _other.channel
               );
   }
   bool isEqualecho (Flocalize _other) {
      bool equal = true;
      if (stationName.compare(_other.stationName) != 0) {printf("stationName is not equal\n"); equal = false;}
      if (stationPhi != _other.stationPhi) {printf("stationPhi is not equal\n"); equal = false;}
      if (multiplet != _other.multiplet) {printf("multiplet is not equal\n"); equal = false;}
      if (gas_gap != _other.gas_gap) {printf("gas_gap is not equal\n"); equal = false;}
      if (channel_type != _other.channel_type) {printf("channel_type is not equal\n"); equal = false;}
      if (channel != _other.channel) {printf("channel is not equal\n"); equal = false;}
      return equal;
   }
   void printInfo () {
      printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", "stationname", "stationEta", "stationPhi", "multiplet", "gas_gap", "channeltype", "channel", "matchedchannel");
      printf("%-15s %-15d %-15d %-15d %-15d %-15d %-15d %-15d\n", stationName.data(), stationEta, stationPhi, multiplet, gas_gap, channel_type, channel, matchedchannel);
   }
   string stationName;
   int stationEta;
   int stationPhi;
   int multiplet;
   int gas_gap;
   int channel_type;
   int channel;
   int matchedchannel;
};

class Flocalize_collection {
   //A class to hold all info to find a Hit in the Digits collection. Generalized to also be used on the RDO's. Does not take ownership of any of the parameters used, just a way to collect pointers.
private:
public:
   Flocalize_collection () {
      stationName = NULL;
      stationEta = NULL;
      stationPhi = NULL;
      multiplet = NULL;
      gas_gap = NULL;
      channel_type = NULL;
      channel = NULL;
   }
   ~Flocalize_collection() {;}
   //NB: MM has no detector type: make this last entry, and ajust member functions for no type possibility.
   Flocalize_collection (string i_name, vector<string>* i_stationName, vector<int>* i_stationEta, vector<int>* i_stationPhi, vector<int>* i_multiplet, vector<int>* i_gas_gap, vector<int>* i_channel, vector<int>* i_channel_type = nullptr) {
      isMM = (i_channel_type == nullptr) ? 1 : 0; 
      stationName = i_stationName;
      stationEta = i_stationEta;
      stationPhi = i_stationPhi;
      multiplet = i_multiplet;
      gas_gap = i_gas_gap;
      channel_type = i_channel_type;
      channel = i_channel;
      vector<int> mc(this->size());
      for (unsigned int i = 0; i < this->size(); ++i) { mc[i] = -10; }
      matchedchannel = mc;
      vector<vector<unsigned int>> mi(this->size());
      matchedindices = mi;
      name = i_name;
      this->checksize();
   }
   size_t size() {
      this->checksize();
      return stationName->size();
   }
   bool checksize() {
      size_t channel_type_size;
      size_t col_size = stationName->size();
      //skip channel_type check if MM, as MM does not have this parameter.
      channel_type_size = (isMM) ? col_size : channel_type->size();
      if (! (stationEta->size() == col_size ||
          stationPhi->size() == col_size ||
          multiplet->size() == col_size ||
          gas_gap->size() == col_size ||
          channel_type_size == col_size ||
          channel->size() == col_size) ) { printf("Sizes of localize collection do not match!\n"); return false;}
      //else { printf("size checks out\n"); }
      return true;
   }
   //also overlod operator [] ?
   Flocalize at(unsigned int index) {
      if (index > this->size()) { Flocalize local; return local; }
      // set channel type to 0 if it is MM, as MM does not have channel type
      int type_val;
      type_val = (isMM) ? 0 : channel_type->at(index);
      Flocalize local(stationName->at(index), stationEta->at(index), stationPhi->at(index), multiplet->at(index), gas_gap->at(index), channel->at(index), type_val, matchedchannel.at(index));
      return local;
   }
   void printInfo () {
      if (isMM) {
         printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", "stationname", "stationEta", "stationPhi", "multiplet", "gas_gap", "channel", "matchedchannel");
         for (unsigned int i = 0; i < this->size(); ++i)
            { printf("%-15s %-15d %-15d %-15d %-15d %-15d %-15d\n", stationName->at(i).data(), stationEta->at(i), stationPhi->at(i), multiplet->at(i), gas_gap->at(i), channel->at(i), matchedchannel.at(i));}
      } else {
         printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", "stationname", "stationEta", "stationPhi", "multiplet", "gas_gap", "channel_type", "channel", "matchedchannel");
         for (unsigned int i = 0; i < this->size(); ++i)
            { printf("%-15s %-15d %-15d %-15d %-15d %-15d %-15d %-15d\n", stationName->at(i).data(), stationEta->at(i), stationPhi->at(i), multiplet->at(i), gas_gap->at(i), channel_type->at(i), channel->at(i), matchedchannel.at(i));}
      }
   }
   bool update_match(int index, int ch_candidate, unsigned int index_match) {
      matchedindices.at(index).push_back(index_match);
      if ( matchedchannel.at(index) == -10 ) { matchedchannel.at(index) = ch_candidate; }
      if ( ch_candidate == -1 ) { return false; }
      if (abs(ch_candidate - channel->at(index)) <  abs(channel->at(index) - matchedchannel.at(index)) ) { 
         matchedchannel.at(index) = ch_candidate;
         return true; 
      }
      return false;
   }
   //Give a unique integer (1-16) based on name(2), multiplet(2) and gas gap(4)
   int loc_unique (int index) {
      return loc_unique_expanded(index);
      int iname = (stationName->at(index).back() == 'L') + 1; 
      int unique = 100 * iname + 10 * multiplet->at(index) + gas_gap->at(index);
      return unique;
   }
   int loc_unique_expanded (int index) {
      int iname = (stationName->at(index).back() == 'L') + 1;
      int iEta = (stationEta->at(index) > 0) + 1;
      int i_cht = (isMM) ? 9 : channel_type->at(index);
      int unique = 10000 * iEta + 1000 * i_cht + 100 * iname + 10 * multiplet->at(index) + gas_gap->at(index);
      return unique;
   }
   int localize (int index) {
      int _i = isMM ? localize_MM(index) : localize_sTGC(index);
      return (_i);
   }
   int localize_sTGC (int index) {
      int unique = loc_unique(index);
      static vector<int> _vloc;
      for (unsigned int i = 0; i < _vloc.size(); ++i) {if (_vloc[i] == unique ) {return i;} }
      _vloc.push_back(unique);
      return (_vloc.size() - 1);
   } 
   int localize_MM (int index) {
      int unique = loc_unique(index);
      static vector<int> _vloc;
      for (unsigned int i = 0; i < _vloc.size(); ++i) {if (_vloc[i] == unique ) {return i;} }
      _vloc.push_back(unique);
      return (_vloc.size() - 1);
   } 
   vector<string>  *stationName;
   vector<int>     *stationEta;
   vector<int>     *stationPhi;
   vector<int>     *multiplet;
   vector<int>     *gas_gap;
   vector<int>     *channel_type;
   vector<int>     *channel;
   vector<int>     matchedchannel;
   vector<vector<unsigned int>> matchedindices;
   string name;
   string matchedwith;
   bool isMM;

 };

 #endif