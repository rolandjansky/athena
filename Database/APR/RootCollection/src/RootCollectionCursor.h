/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_ROOTCOLLECTIONCURSOR_H
#define ROOTCOLLECTION_ROOTCOLLECTIONCURSOR_H


#include "PersistentDataModel/Token.h"

#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionCursor.h"

#include "PersistencySvc/IPositionSeek.h"
#include "AthenaKernel/ICollectionSize.h"

#include "TTree.h"
#include "TEventList.h"

namespace pool {
   namespace RootCollection {
      /** 
       * @class RootcollectionCursor RootcollectionCursor.h Rootcollection/RootcollectionCursor.h
       *
       * An interface used to navigate the result of a query on a collection.
       */
      class RootCollectionCursor : public ICollectionCursor,
                                   virtual public IPositionSeek,
                                   virtual public ICollectionSize
      {
     public:

        RootCollectionCursor(
           const pool::ICollectionDescription& description,
           const pool::CollectionRowBuffer& collectionRowBuffer,
           TTree *tree,
           const TEventList *evl );

        
        /// Advances the cursor to the next row of the query result set.
        virtual bool next() override;

        /// Returns the selected Tokens and Attributes for the current row of the query result set.
        virtual const pool::CollectionRowBuffer& currentRow() const override;

        /// Seeks the cursor to a given position in the collection.
        virtual bool seek(long long int position) override;

        /// Return the size of the collection.
        virtual int size() override;

        /// Returns the event reference Token for the current row.
        virtual const Token& eventRef() const override;

        /// Cleanup.
        virtual void close() override;

        virtual ~RootCollectionCursor();

     protected:
    
        static const unsigned int       c_maxLengthOfStrings = 5000;    
        
        const ICollectionDescription    &m_description;

        /// Row buffer containing Tokens and Attributes selected by query.
        pool::CollectionRowBuffer       m_collectionRowBuffer;

        const TEventList                *m_eventList;

        
        char                             m_charBuffer[c_maxLengthOfStrings];

        typedef std::vector< std::pair<TBranch*, std::string*> >  AttrBranchVector_t;
        typedef std::vector< std::pair<TBranch*, Token*> >        TokenBranchVector_t;

        AttrBranchVector_t              m_attrBranches;
        TokenBranchVector_t             m_tokenBranches;

        int                             m_idx;
        Long64_t                        m_entries;
        bool                            m_dummyRef;
      };
   }
}

#endif


