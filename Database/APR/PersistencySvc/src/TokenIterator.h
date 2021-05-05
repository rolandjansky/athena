/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENCYSVC_TOKENITERATOR_H
#define PERSISTENCYSVC_TOKENITERATOR_H

#include "PersistencySvc/ITokenIterator.h"
#include "PersistencySvc/IPositionSeek.h"
#include "AthenaKernel/ICollectionSize.h"
#include <string>

namespace pool {

  // forward declarations
  class FileDescriptor;
  class IStorageExplorer;
  class DbSelect;

  namespace PersistencySvc {

    /** @class TokenIterator
     *
     *  TokenIterator is an implementation of the ITokenIterator interface
     *
     */

    class TokenIterator : virtual public ITokenIterator,
			  virtual public IPositionSeek,
			  virtual public ICollectionSize
      {
      public:
	/** Constructor taking as argument a DbSelection object,
	 *  the file descriptor, the container name and the selection string
	 */
	TokenIterator( FileDescriptor& fileDescriptor,
		       const std::string& containerName,
		       IStorageExplorer& storageExplorer,
		       const std::string& selection );
	/// Destructor
	~TokenIterator();

	/** Returns the pointer to next token.
	 *  Token ownership is passed to the user.
	 *  if no other token is available in the iteration
	 *  sequence, 0 is returned.
	 */
	Token* next();

        /**
         * @brief Seek to a given position in the collection
         * @param position  The position to which to seek.
         * @returns True if successful, false otherwise.
         */
        virtual bool seek (long long int position);

        /**
         * @brief Return the size of the collection.
         */
        virtual int size ();

      private:
	/// Reference to the storage explorer
	IStorageExplorer& m_storageExplorer;
	/// Selection object
	DbSelect* m_selection;
      };
  }
}

#endif
