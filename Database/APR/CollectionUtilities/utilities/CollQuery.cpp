/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollQuery.cpp
 * @brief Utility to list the metadata of a POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/CollectionService.h"

#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/MessageStream.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/CmdLineArgs2.h"
#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CollectionUtilities/MaxEventsInfo.h"
#include "CxxUtils/checker_macros.h"

#include <iostream>
#include <iomanip>
#include <memory>


// ------   copied the whole class here for quick adaptation.  MN
#include <set>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace pool;
using namespace std;


class AttributeType
{
public:
  AttributeType(std::string t, std::string u, std::string g);
 
  std::string info() const;
//  std::string nonTypeInfo() const;
  
  void setType(const std::string& info) {setInfoForKey("TYPE",info);}
  void setUnit(const std::string& info) {setInfoForKey("UNIT",info);}
  void setGroup(const std::string& info) {setInfoForKey("GRP",info);}
  void fromString(const std::string& info);

  std::string typeName() const;
  std::string unit() const;
  std::string group() const;

  std::set<std::string> getKeys() {return m_keys;}

private:

  bool setInfoForKey(const std::string& key, const std::string& info);

  std::map<std::string,std::string> m_keyedInfo;
  std::set<std::string> m_keys;
};

inline
AttributeType::AttributeType(std::string t, 
                             std::string u="UNKNOWN",
                             std::string g="UNKNOWN")
{
   m_keyedInfo.clear();
   m_keys.clear();
   m_keys.insert("TYPE");
   m_keys.insert("UNIT");
   m_keys.insert("GRP");
   m_keyedInfo.insert(std::make_pair("TYPE",t));
   m_keyedInfo.insert(std::make_pair("UNIT",u));
   m_keyedInfo.insert(std::make_pair("GRP",g));
}

inline
bool AttributeType::setInfoForKey(const std::string& key, const std::string& info)
{
   if ( m_keys.find(key) != m_keys.end() ) {
      std::map<std::string,std::string>::iterator i = m_keyedInfo.find(key);
      if (i != m_keyedInfo.end()) i->second=info;
      else m_keyedInfo.insert(std::make_pair(key,info));
      return true;
   }
   else return false;
}

inline void
AttributeType::fromString(const std::string& i)
{
   m_keyedInfo.clear();
   m_keys.clear();

   // search for type first
   std::set<std::string> keys;
   keys.insert("TYPE");
   keys.insert("UNIT");
   keys.insert("GRP");
   std::string::size_type flagpos, endpos;
   for (std::set<std::string>::iterator it = keys.begin(); it!=keys.end(); ++it) {
      flagpos = i.find(*it);
      if (flagpos != std::string::npos) {
         endpos = i.find(';',flagpos);
         if (endpos != std::string::npos) {
            this->setInfoForKey(*it,i.substr(flagpos,endpos));
         }
      }
   }
}

inline
std::string AttributeType::typeName() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("TYPE");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AttributeType::unit() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("UNIT");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AttributeType::group() const
{
   std::map<std::string,std::string>::const_iterator temp = m_keyedInfo.find("GRP");
   if (temp!=m_keyedInfo.end()) return temp->second;
   else return std::string("NOTFOUND");
}

inline
std::string AttributeType::info() const
{
   std::ostringstream os;
   os << "TYPE="<<typeName()<<";"<<"UNIT="<<unit()<<";"<<"GRP="<<group()<<";";
   return os.str();
}




using namespace std;

int main(int argc, const char *argv[])
{
   string thisProgram("CollQuery");
   SystemTools::initGaudi();

   try  { 
      pool::CollectionService   collectionService;
      coral::MessageStream log( thisProgram );

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);

      // list of CollQuery *specific* cli keys and their argument properties
      QualList markers;
      markers.insert( make_pair("-count",    pool::ArgQual(0)) );
      markers.insert( make_pair("-colwidth", ArgQual()) );
      markers.insert( make_pair("-pktype", ArgQual()) );
      markers.insert( make_pair("-pkrange", ArgQual(2)) );
      markers.insert( make_pair("-nevents", ArgQual()) );
      markers.insert( make_pair("-nevtperprint", ArgQual()) );
      //markers.insert( make_pair("-quiet", ArgQual(0)) );

      markers["-count"].desc << "Only print out the count of the query, like count(*)";
      markers["-colwidth"].desc << "Width of output columns in characters";
      markers["-pktype"].desc << "NOT CURRENTLY IMPLEMENTED " << endl;
      markers["-pktype"].desc << "Primary key type: " << endl
                              << " no PK (DEFAULT)" << endl
                              << " Other valid choices: " << endl
                              << "  (1) ATLAS = (run,event,lumiblock)"; 
      markers["-pkrange"].desc << "NOT CURRENTLY IMPLEMENTED " << endl;
      markers["-pkrange"].desc << "First and last primary keys to be considered " << endl
                               << "listed as tuple pair (val1,val2,...) appropriate for pktype chosen. " 
                               << "NO SPACES allowed!"
                               << "(DEFAULT = all) ";
      markers["-nevents"].desc << "Number of events starting from begin of pkrange for "
                               << "which to how results. (DEFAULT = all) ";
      markers["-nevtperprint"].desc << "Number of events to process before "
                                    << "each print of status to the screen. (DEFAULT=200)";
/*
  markers["-quiet"].desc << "Qualifier that takes no arguments and specifies "
  << "that no information except the (optional) event dump and "
  << "event scan status information is to be printed to screen; "
  << "Useful for performance testing";
*/

      CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      argsVec.push_back(&cmdLineArgs);  // Add it to the list

      // Classes with shared cli keys and their argument properties
      // Add them to the list
      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      QueryInfo queryinfo; argsVec.push_back(&queryinfo);
      SrcInfo srcinfo;     argsVec.push_back(&srcinfo);
      MaxEventsInfo maxEvents; argsVec.push_back(&maxEvents);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      // Fill appropriate vectors based on CollQuery *specific* cmdLineArgs
      unsigned int numEventsPerPrint = 0;
      unsigned int colWidth = 16, idWidth=24;
      bool noPrint = false;
      string val;
      bool countOnly = cmdLineArgs.hasQual("-count");
      if( cmdLineArgs.getOpt("-colwidth", val) ) {
         colWidth = atoi( val.c_str() );
      }
      if( cmdLineArgs.hasQual("-pktype") ) {
         cerr << "WARNING: -pktype not implemented" << endl;
      }
      if( cmdLineArgs.hasQual("-pkrange") ) {
         cerr << "WARNING: -pkrange not implemented" << endl;
      }
      if( cmdLineArgs.getOpt("-nevtperprint", val) ) {
         numEventsPerPrint = atoi( val.c_str() );
      }

      
      catinfo.setCatalogs( &collectionService ); 

      unsigned int collCounter = 0;
      unsigned int evtCounterTotal = 0;

      std::map<std::string, int>   CollCnts;
      std::map<std::string, double> AttrSumMap;
      std::map<std::string, double> AttrMaxMap;
      std::map<std::string, double> AttrMinMap;

      std::map<int, pool::ICollection*> collMap;
      unsigned int nAttrib = 0;
      std::vector<std::string> attribNames;
      std::vector<unsigned int> widths;
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ )    {
         bool readOnly( true );
         pool::ICollection* collection = collectionService.handle( srcinfo.name(i), 
                                                                   srcinfo.type(i), 
                                                                   srcinfo.connect(), 
                                                                   readOnly );
         collMap[i] = collection;
         collCounter++;

         const pool::ICollectionDescription &description = collection->description();
         unsigned int maxNameSize = 0;
         unsigned int maxTypeNameSize = 0;
         for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
            for( int c=0; c < description.numberOfAttributeColumns( f ); c++ ) {
               const pool::ICollectionColumn&	column = description.attributeColumn(c, f);
               if( maxNameSize < column.name().size() ) 
                  maxNameSize = column.name().size();
               if( maxTypeNameSize < column.type().size() ) 
                  maxTypeNameSize = column.type().size();
            }
         }

/*
  std::cout << std::endl;
  std::cout << "Collection name: " << description.name() << std::endl;
  std::cout << "Collection type: " << description.type() << std::endl;
  std::cout << "Number of collection fragments: " 
  << description.numberOfCollectionFragments() << std::endl;
  std::cout << "Number of tokens is: " 
  << description.numberOfTokenColumns() << std::endl;
  std::cout << "Number of attributes is: " 
  << description.numberOfAttributeColumns() << std::endl;
*/
         unique_ptr<pool::ICollectionQuery>  collQuery( collection->newQuery() );
         if (queryinfo.hasQual("-query")) {
            log << coral::Debug << ">> Creating query for the collection " << coral::MessageStream::endmsg;
         }
         collQuery->setCondition( queryinfo.query(i) );
         // set queryoptions
         if( queryinfo.queryOptions().size() ) {
            collQuery->addToOutputList( queryinfo.queryOptions() );
         } 
         else {
            collQuery->selectAllAttributes();
         }
         log << coral::Debug << ">> Executing the query " << coral::MessageStream::endmsg;
         pool::ICollectionCursor& cursor = collQuery->execute();

         int evtCounter = 0;

         std::string attribName = "";
         std::string attribTypeName = "";
         log << coral::Debug << ">>  Iterating over query results " << coral::MessageStream::endmsg;	
         while( cursor.next() ) {
            if( maxEvents.specified() && int(evtCounterTotal) >= maxEvents.get()  )
               break;

            evtCounter++; 
            evtCounterTotal++;

            if( numEventsPerPrint>0 && !(evtCounterTotal % numEventsPerPrint) ) {
               log << coral::Info << "Rows processed: " << evtCounterTotal << coral::MessageStream::endmsg;	
            }     
            const coral::AttributeList &attribList = cursor.currentRow().attributeList();

            if (evtCounterTotal==1) {
               std::cout << std::endl;
               //std::cout << "Collection name: " << srcinfo.name(i) << std::endl;
               //std::cout << "Collection type: " << srcinfo.type(i) << std::endl;

               std::cout << std::endl;
               std::vector<std::string> names,types,units,groups;
               for ( coral::AttributeList::const_iterator iter = attribList.begin();
                     iter != attribList.end(); ++iter ) 
               {
                  attribName = iter->specification().name();
                  attribTypeName = iter->specification().typeName();
                  AttributeType attribAAT(attribTypeName);
                  attribAAT.fromString(collection->description().column(attribName).annotation());
                  // Find widest piece
                  unsigned int width = max(attribName.size(),attribTypeName.size());
                  width = max( width, (unsigned int)attribAAT.unit().size() );
                  width = max( width, (unsigned int)attribAAT.group().size() );
                  names.push_back(attribName);
                  types.push_back(attribTypeName);
                  units.push_back(attribAAT.unit());
                  groups.push_back(attribAAT.group());
                  //widths.push_back(width);
                  widths.push_back(colWidth);
                  ++nAttrib;
               }
/*
  std::cout << std::endl;
  std::cout.width(idWidth);
  std::cout << "GROUP: ";
  for (unsigned int i=0; i<nAttrib; ++i) {
  std::cout.width(colWidth);
  if (groups[i].size()<colWidth) {
  std::cout << groups[i];
  }
  else {
  std::string cname2(groups[i].substr(0,max(2,((int)colWidth-2)/2)));
  cname2 += "..";
  cname2 += groups[i].substr(groups[i].size()-max(2,((int)colWidth-2)/2));
  cout << cname2;
  }
  }
*/
               if (!countOnly) {
                  std::cout << std::endl;
                  std::cout.width(idWidth);
                  std::cout << "NAME: ";
                  for (unsigned int i=0; i<nAttrib; ++i) {
                     std::cout.width(colWidth);
                     if (names[i].size()<colWidth) {
                        std::cout << names[i];
                     }
                     else {
                        std::string cname2(names[i].substr(0,max(2,((int)colWidth-2)/2)));
                        cname2 += "..";
                        cname2 += names[i].substr(names[i].size()-max(2,((int)colWidth-2)/2));
                        cout << cname2;
                     }
                  }

                  std::cout << std::endl;
                  std::cout.width(idWidth);
                  std::cout << "TYPE: ";
                  for (unsigned int i=0; i<nAttrib; ++i) {
                     std::cout.width(colWidth);
                     if (types[i].size()<colWidth) {
                        std::cout << types[i];
                     }
                     else {
                        std::string cname2(types[i].substr(0,max(2,((int)colWidth-2)/2)));
                        cname2 += "..";
                        cname2 += types[i].substr(types[i].size()-max(2,((int)colWidth-2)/2));
                        cout << cname2;
                     }
                  }

                  /*
                    std::cout << std::endl;
                    std::cout.width(idWidth);
                    std::cout << "UNIT: ";
                    //unsigned int totwidth=0;
                    for (unsigned int i=0; i<nAttrib; ++i) {
                    std::cout.width(colWidth);
                    if (units[i].size()<colWidth) {
                    std::cout << units[i];
                    }
                    else {
                    std::string cname2(units[i].substr(0,max(2,((int)colWidth-2)/2)));
                    cname2 += "..";
                    cname2 += units[i].substr(units[i].size()-max(2,((int)colWidth-2)/2));
                    cout << cname2;
                    }
                    //totwidth += (2+widths[i]);`
                    }
                  */
                  std::cout << std::endl;
                  std::string linesep;
                  for (unsigned int i=0; i<idWidth+nAttrib*colWidth; ++i) linesep += "-";
                  std::cout << linesep;
               }
               cout << endl;
            }

/*
  for ( coral::AttributeList::const_iterator iter = attribList.begin();
  iter != attribList.end(); ++iter ) 
*/
            if( !maxEvents.specified() || evtCounter <= maxEvents.get()  )
            {
               std::cout.width(idWidth);
               std::string cname(collection->description().name());
               if (cname.size()<idWidth) {
                  if (!countOnly) std::cout << cname;
               }
               else {
                  std::string cname2(cname.substr(0,max(2,((int)idWidth-2)/2)));
                  cname2 += "..";
                  cname2 += cname.substr(cname.size()-max(2,((int)idWidth-2)/2));
                  if (!countOnly) cout << cname2;
               }

               for (unsigned int j=0; j<nAttrib; ++j)
               {
                  //attribName = iter->specification().name();
                  //attribTypeName = iter->specification().typeName();
                  attribName = attribList[j].specification().name();
                  attribTypeName = attribList[j].specification().typeName();
                  attribNames.push_back(attribName);

                  std::cout.width(widths[j]);
                  ostringstream lots;
                  attribList[j].toOutputStream(lots);
                  std::string value = lots.str().substr(lots.str().find(':')+1,std::string::npos);
                  std::cout.width(widths[j]);
                  if (!countOnly) std::cout << value;

                  double val(0);
                  if( attribTypeName == "short" ) {
                     val = double(attribList[j].data<short>());
                  } else if ( attribTypeName == "unsigned short" ) {
                     val = double(attribList[j].data<unsigned short>());
                  } else if ( attribTypeName == "int" ) {
                     val = double(attribList[j].data<int>());
                  } else if ( attribTypeName == "unsigned int" ) {
                     val = double(attribList[j].data<unsigned int>());
                  } else if ( attribTypeName == "long" ) {
                     val = double(attribList[j].data<long>());
                  } else if ( attribTypeName == "unsigned long" ) {
                     val = double(attribList[j].data<unsigned long>());
                  } else if ( attribTypeName == "float" ) {
                     val = double(attribList[j].data<float>());
                  } else if ( attribTypeName == "double" ) {
                     val = double(attribList[j].data<double>());
                  }

                  if ( AttrSumMap.count(attribName) == 0 ) {
                     AttrMaxMap[attribName] = val;
                     AttrMinMap[attribName] = val;
                  }
                  AttrSumMap[attribName] += val;
                  if ( val > AttrMaxMap[attribName] )
                  {
                     AttrMaxMap[attribName] = val;
                  }
                  if ( val < AttrMinMap[attribName] )
                  {
                     AttrMinMap[attribName] = val;
                  }
               }
               if (!countOnly) std::cout << endl;
            }
            
         }

         CollCnts.insert(make_pair(srcinfo.name(i),evtCounter));
         std::cout << std::endl;
      }

      std::string linesep;
      for (unsigned int i=0; i<idWidth+nAttrib*colWidth; ++i) linesep += "-";
      std::cout << linesep << std::endl;

      std::cout.width(idWidth);
      std::cout << "NAME: ";
      //unsigned int totwidth=0;
      for (unsigned int i=0; i<nAttrib; ++i) {
         std::cout.width(colWidth);
         std::cout << attribNames[i];
      }
      std::cout << std::endl;
      std::cout.width(idWidth);
      std::cout << "MAX: ";
      //unsigned int totwidth=0;
      for (unsigned int i=0; i<nAttrib; ++i) {
         std::cout.width(colWidth);
         std::cout << AttrMaxMap[attribNames[i]];
      }
      std::cout << std::endl;
      std::cout.width(idWidth);
      std::cout << "AVG: ";
      //unsigned int totwidth=0;
      for (unsigned int i=0; i<nAttrib; ++i) {
         std::cout.width(colWidth);
         std::cout << AttrSumMap[attribNames[i]]/double(evtCounterTotal);
      }
      std::cout << std::endl;
      std::cout.width(idWidth);
      std::cout << "MIN: ";
      //unsigned int totwidth=0;
      for (unsigned int i=0; i<nAttrib; ++i) {
         std::cout.width(colWidth);
         std::cout << AttrMinMap[attribNames[i]];
      }
      std::cout << std::endl;

      std::cout << linesep << std::endl;
      std::cout << "CONDITIONS = " << queryinfo.query() << std::endl;
      std::cout << linesep << std::endl;

      if ( !noPrint )
      {
         std::cout << std::endl;
         std::cout << "Collections scanned: " << std::endl;
         for (std::map<std::string,int>::iterator it = CollCnts.begin(); it != CollCnts.end(); ++it) {
            std::cout.width(32);
            std::cout << it->first;
            std::cout.width(32);
            std::cout << it->second << std::endl;
         }
         std::cout << "Total number of events scanned in all collections is: " 
                   << evtCounterTotal << std::endl;
         std::cout << std::endl;
      }

      for( unsigned int i=0; i < srcinfo.nSrc(); i++ ) {      
         collMap[i]->close();
      }

      return 0;
   }
   catch( pool::Exception& poolException )
   {
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;
      return 1;
   }
   catch( std::exception& exception )
   {
      std::cerr << "std::exception: " << exception.what() << std::endl;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception caught in main()." << std::endl;
      return 1;
   }
}
