/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLOBJECTITER_H
#define CORACOOL_CORACOOLOBJECTITER_H
// CoraCoolObjectIter.h - allows to iterate over the result of a CoraCool
// bulk query from the CoraCoolFolder browseObject method
//
// usage pattern is the same as for the COOL IObjectIterator, i.e.
//   CoraCoolObjectIterPtr itr=CoraCoolFolderPtr->browseObjects(..)
//   while (itr->hasNext()) {
//     CoraCoolObjectPtr obj=itr->next()
//     // do something with the object
//    }
//    itr->close()
//
// Richard Hawkings, started 10/2006

#include "CoolKernel/IObjectIterator.h"
#include "CoraCool/CoraCoolTypes.h"

class CoraCoolFolder;

class CoraCoolObjectIter {
 public:
  // constructor - do not use this directly but create iterators using
  // CoraCoolFolder::broweObject(..)
  CoraCoolObjectIter(CoraCoolFolder* coracoolfolder,
		     cool::IObjectIteratorPtr coolptr);
  ~CoraCoolObjectIter();

  // get the next object - call hasNext() first to check there is one
  // throws CoraCoolException if there is none
  CoraCoolObjectPtr next();

  // does the iterator have a next object
  bool hasNext();

  // close the iterator, releasing resources
  void close();

 private:
  // hide default and copy constructors, and assignment operator
  // declared but not implemented
  CoraCoolObjectIter();
  CoraCoolObjectIter(const CoraCoolObjectIter&);
  CoraCoolObjectIter& operator=(const CoraCoolObjectIter&);

  // implementation class to build queries
  class QueryBuilder {
   public:
    // setup query builder with specified number size of range pairs
    QueryBuilder(const unsigned int size);
    ~QueryBuilder();

    // return current size (num of range pairs) and number of terms in query
    unsigned int size() const;
    unsigned int terms() const;
    // add a FK value to the query list
    unsigned int add(const long long value);
    // prepare the DB query - return the where clause and the list of foreign
    // key bind variables, given the coralkey name and FK attribute spec
    void getQuery(std::string& where,const std::string& coralkey,
       coral::AttributeList& fkeys, const coral::AttributeSpecification& spec);

  private:
    // hide default and copy constructors, and assignment operator
    QueryBuilder();
    QueryBuilder(const QueryBuilder&);
    QueryBuilder& operator=(const QueryBuilder&);

    std::string addKey(unsigned int& key,
	   coral::AttributeList& fkeys,
	   const coral::AttributeSpecification& spec,
	   const long long value);
    unsigned int m_terms;
    unsigned int m_size; // number of ranges stored
    unsigned int m_maxsize; // max size of the array
    long long* m_lower; // array of lower and upper bounds for ranges
    long long*  m_upper;// assumed to be store in ascending numerical order
  };

  bool iHasNext();
  bool equalAttr(const coral::Attribute& a1, const coral::Attribute& a2) const;
  bool isNumAttr(const std::string& spec) const;
  long long numAttr(const std::string& spec, const coral::Attribute& a) const;

  unsigned int m_buflen; // size of the buffer for payload data
  unsigned int m_inbuf; // number of objects in the buffer
  unsigned int m_nextobj; // pointer to next object
  bool m_refValid; // COOL iterator contains valid reference
  bool m_allDone; // COOL iterator is exhausted
  CoraCoolFolder* m_folder;
  cool::IObjectIteratorPtr m_coolitr;
  coral::AttributeListSpecification* m_spec;
  std::vector<CoraCoolObjectPtr> m_data;
};

inline unsigned int CoraCoolObjectIter::QueryBuilder::size() const 
{ return m_size; }
inline unsigned int CoraCoolObjectIter::QueryBuilder::terms() const 
{ return  m_terms; }

#endif // CORACOOL_CORACOOLOBJECTITER_H

