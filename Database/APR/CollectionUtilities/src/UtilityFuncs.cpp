/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file UtilityFuncs.h
 * @brief prototypes for utility POOL collection funcs
 * @author Jack.Cranshaw@cern.ch
 * $Id: UtilityFuncs.cpp 495625 2012-04-13 13:54:17Z mnowak $ 
 */

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionColumn.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"

#include "CoralBase/Attribute.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "CollectionUtilities/UtilityFuncs.h"

using namespace std;
using namespace pool;

std::string pool::getKey(const std::string& key, const std::string& encoded)
{
   size_t keystart = encoded.find(key);
   if (keystart!=string::npos) {
      size_t keylen = encoded.find(";",keystart)-keystart;
      //cout << "key begins at " << keystart << " with length " << keylen << " for " << encoded << endl;
      if (keystart+keylen<encoded.size()) {
         return encoded.substr(keystart+key.size(),keylen-key.size()); 
      }
      else return ("NONE");
   }
   else {
      return string("NONE");
   }
}

std::string pool::getGroup(const std::string& encoded)
{
   return getKey("GRP=",encoded);
}

std::set<std::string> pool::findGroups(const pool::ICollectionDescription& desc)
{
   set<string> groups;
   // Assume only one fragment for now
   for (int col = 0; col<desc.numberOfAttributeColumns(0); ++col) {
      const pool::ICollectionColumn& coldata = desc.attributeColumn(col,0);
      string groupdata = getGroup(coldata.annotation());
      set<string>::iterator it = groups.find(groupdata);
      if (it==groups.end()) groups.insert(groupdata);
   }
   return groups;
}

std::pair<unsigned int,unsigned int>
pool::getColumnWidths(const pool::ICollectionDescription& description, bool tokens=false)
{
   unsigned int maxNameSize=0,maxTypeNameSize=0;
   if (tokens) {
      for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
         for( int c=0; c < description.numberOfTokenColumns( f ); c++ ) {
            const pool::ICollectionColumn& column = description.tokenColumn(c, f);
            if( maxNameSize < column.name().size() )
                maxNameSize = column.name().size();
            if( maxTypeNameSize < column.type().size() )
                maxTypeNameSize = column.type().size();
         }
      }
   }
   else {
      for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
         for( int c=0; c < description.numberOfAttributeColumns( f ); c++ ) {
            const pool::ICollectionColumn& column = description.attributeColumn(c, f);
            if( maxNameSize < column.name().size() )
                maxNameSize = column.name().size();
            if( maxTypeNameSize < column.type().size() )
                maxTypeNameSize = column.type().size();
         }
      }
   }
   return std::make_pair(maxNameSize,maxTypeNameSize);
}

void pool::dumpGroups(const pool::ICollectionDescription& description) 
{
   set<string> groups = findGroups(description);
   std::cout << "--------------------------------------------------------------" << std::endl;
   if (groups.find("NONE")==groups.end()) {
      std::cout << "Number of groups is: " << groups.size() << std::endl;
      std::cout << "Groups are: " <<std::endl;
      // dump groups
      set<string>::const_iterator git = groups.begin();
      while (git!=groups.end()) {
         cout << "  " << *git << endl;
         ++git;
      }
   }
   else {
      std::cout << "Number of groups is: 0" << std::endl;
   }
}

void pool::dumpTokens(const pool::ICollectionDescription& description) 
{
   string spectype("Token");
   std::pair<unsigned int,unsigned int> maxpair = getColumnWidths(description,true);
   unsigned int maxNameSize = maxpair.first;
   unsigned int maxTypeNameSize = maxpair.second;
   std::cout << "--------------------------------------------------------------" << std::endl;
   std::cout << "Number of " << spectype << "s is: " 
             << description.numberOfTokenColumns() << std::endl;
   std::cout << spectype << "s are: " << std::endl;
   for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
      for( int c=0; c < description.numberOfTokenColumns( f ); c++ ) {
         const pool::ICollectionColumn&	column = description.tokenColumn(c, f);
         std::cout.setf(std::ios::left);
         std::cout << " NAME: ";
         std::cout.width(maxNameSize+5);
         std::cout << column.name();
         std::cout << " TYPE: ";
         std::cout.width(maxTypeNameSize+5);
         std::cout << column.type();
         std::cout << " INFO: ";
         std::cout << column.annotation();
         std::cout << std::endl;
      }
   }
   std::cout << " DEFAULT REF = " << description.eventReferenceColumnName() << std::endl;
}

void pool::dumpAttributes(const pool::ICollectionDescription& description)
{
   string spectype("Attribute");
   std::pair<unsigned int,unsigned int> maxpair = getColumnWidths(description);
   unsigned int maxNameSize = maxpair.first;
   unsigned int maxTypeNameSize = maxpair.second;
   std::cout << "--------------------------------------------------------------" << std::endl;
   std::cout << "Number of " << spectype << "s is: " 
             << description.numberOfAttributeColumns() << std::endl;
   std::cout << spectype << "s are: " << std::endl;
   for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
      for( int c=0; c < description.numberOfAttributeColumns( f ); c++ ) {
         const pool::ICollectionColumn&	column = description.attributeColumn(c, f);
         std::cout.setf(std::ios::left);
         std::cout << " NAME: ";
         std::cout.width(maxNameSize+5);
         std::cout << column.name();
         std::cout << " TYPE: ";
         std::cout.width(maxTypeNameSize+5);
         std::cout << column.type();
         std::cout << " INFO: ";
         std::cout << column.annotation();
         std::cout << std::endl;
      }
   }
}

void pool::dumpOverlapEval(const pool::ICollectionDescription& desc0,
                                 const pool::ICollectionDescription& desc1,
                                 const vector< pair<string,string> >& spec0,
                                 const vector< pair<string,string> >& spec1,
                                 const string& spectype)
{
   // First check that inputs are valid
   vector< vector< pair<string,string> > > Specs;
   Specs.push_back(spec0);
   Specs.push_back(spec1);
   vector<string> Descs;
   Descs.push_back(desc0.name());
   Descs.push_back(desc1.name());
   vector< pair<string,string> > tmp(max(spec0.size(),spec1.size()));
   vector< pair<string,string> >::iterator last = 
         set_intersection( Specs[0].begin(),Specs[0].end(),
                           Specs[1].begin(),Specs[1].end(),
                           tmp.begin() );
   vector< pair<string,string> > output(tmp.begin(),last); 
   unsigned int intersize = output.size();
   if (intersize==Specs[0].size() && 
       Specs[0].size()==Specs[1].size()) {
      std::cout << "--------------------------------------------------------------" << std::endl;
      std::cout << " ***" << std::endl;
      std::cout << " *** " << spectype << "s IDENTICAL for these Collections ***" << std::endl;
      std::cout << " ***" << std::endl;
      if (spectype=="Token") dumpTokens(desc0);
      else if (spectype=="Attribute") dumpAttributes(desc0); 
      else cerr << "dumpOverlapEval ERROR: unknown spectype = " << spectype << endl; 
   }
   else {
      std::cout << "--------------------------------------------------------------" << std::endl;
      std::cout << " ***" << std::endl;
      std::cout << " *** " << spectype << "s DIFFER for these Collections ***" << std::endl;
      std::cout << " ***" << std::endl;
      std::cout << "--------------------------------------------------------------" << std::endl;
      std::cout << "Number of Common " << spectype << " Names is: " 
                << output.size() << std::endl;
      // First dump intersection
      std::cout << "Common " << spectype << " Names are: " << std::endl;
      std::ofstream intersect_names;
      std::string name("intersect_names."+spectype);
      intersect_names.open(name.c_str(),ios::out | ios::trunc);
      for( vector< pair<string,string> >::iterator c = output.begin(); 
                                                   c < output.end(); 
                                                   ++c ) {
         std::cout << " NAME: ";
         std::cout << c->first;
         std::cout << "   TYPE: ";
         std::cout << c->second;
         std::cout << std::endl;
         intersect_names << c->first << std::endl;
      }
      intersect_names.close();
      for (int i=0; i<2; ++i) {
         int j;
         if (i==0) j=1; else j=0;
         std::ofstream names;
         if (Specs[i].size() > intersize) {
            std::cout << "Number of " << spectype << " Names Specific to "
                      << Descs[i] << " is: " 
                      << Specs[i].size() - intersize << std::endl;
            std::cout << spectype << " Names are: " << std::endl;
            std::string name(Descs[i]+"Only."+spectype);
            names.open(name.c_str(), ios::out | ios::trunc);
            vector< pair<string,string> > tmp(max(Specs[0].size(),Specs[1].size()));
            vector< pair<string,string> >::iterator last2 = 
                    set_difference(Specs[i].begin(),Specs[i].end(),
                                   Specs[j].begin(),Specs[j].end(),
                                   tmp.begin());
            vector< pair<string,string> > solo(tmp.begin(),last2); 
            for( vector< pair<string,string> >::iterator c = solo.begin(); 
                                                         c < solo.end(); 
                                                         ++c ) {
               std::cout << " NAME: ";
               std::cout << c->first;
               std::cout << "   TYPE: ";
               std::cout << c->second;
               std::cout << std::endl;
               names << c->first << std::endl;
            }
         }
         names.close();
      }
   }
}

/*
bool isNonProvEqual(const pool::ICollectionDescription& desc0,
                    const pool::ICollectionDescription& desc1) {
   if( desc0.equals(desc1) ) return true;
   else {
      map<int, vector<pair<string,string> > > tokSpecs;
      vector< pair<string,string> > spec;
      spec.clear();
      for( int f=0; f < desc0.numberOfCollectionFragments(); f++ ) {
         for( int c=0; c < desc0.numberOfTokenColumns( f ); c++ ) {
            const pool::ICollectionColumn&   column = desc0.tokenColumn(c, f);
            spec.push_back(make_pair(column.name(),column.type()));
         }                                       
      }
      tokSpecs.insert(make_pair(0,spec));
      spec.clear();
      for( int f=0; f < desc1.numberOfCollectionFragments(); f++ ) {
         for( int c=0; c < desc1.numberOfTokenColumns( f ); c++ ) {
            const pool::ICollectionColumn&   column = desc1.tokenColumn(c, f);
            spec.push_back(make_pair(column.name(),column.type()));
         }                                       
      }
      tokSpecs.insert(make_pair(1,spec));
      stable_sort(tokSpecs[0].begin(),tokSpecs[0].end());
      stable_sort(tokSpecs[1].begin(),tokSpecs[1].end());
      vector< pair<string,string> > spec0 = tokSpecs[0];
      vector< pair<string,string> > spec1 = tokSpecs[1];
      vector< pair<string,string> > tmp(max(spec0.size(),spec1.size()));
      vector< pair<string,string> >::iterator last2 = 
            set_difference(spec0.begin(),spec0.end(),
                           spec1.begin(),spec1.end(),
                           tmp.begin());
            vector< pair<string,string> > solo(tmp.begin(),last2); 
      if (solo.size()==1) {
         if (solo[0] == ) 
   }
}
*/

// Stole this little snippet from OOPWeb.com 
// http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
// to fill in a basic failure in std::string 
// Essentially it does for C++ what strtok does for C

void pool::Tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

