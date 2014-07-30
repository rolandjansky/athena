/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CutFlowMetadataTool.cxx 359576 2011-04-18 08:18:16Z krasznaa $

// System include(s):
#include <cstring>

// ROOT include(s):
#include <TTree.h>
#include <TBranch.h>
#include <TBranchElement.h>
#include <TLeaf.h>
#include <TClass.h>

// Athena/Gaudi include(s):
#include "GaudiKernel/GaudiException.h"
#include "AthenaKernel/errorcheck.h"

// D3PDMaker include(s):
#include "D3PDMakerInterfaces/ID3PD.h"

// Local include(s):
#include "CutFlowMetadataTool.h"

namespace {

   /// Get the type_info of ROOT primitive types
   /**
    * There's no way (as far as I know) to get an std::type_info object directly
    * from ROOT that would describe the type of a primitive branch in a tree.
    * However ROOT does provide the variable type for the stored primitive.
    *
    * This function simply translates these names into std::type_info variables.
    * All ROOT primitive types should be covered by the function, but in case it
    * finds a variable type that it doesn't recognize, it throws an exception.
    *
    * @param root_type ROOT primitive type name
    * @returns The std::type_info describing the supplied ROOT primitive type
    */
   const std::type_info& findTypeinfo( const char* root_type ) {

      //
      // Try for all the known ROOT types:
      //
      if( ! strcmp( root_type, "Char_t" ) ) {
         return typeid( Char_t );
      } else if( ! strcmp( root_type, "UChar_t" ) ) {
         return typeid( UChar_t );
      } else if( ! strcmp( root_type, "Short_t" ) ) {
         return typeid( Short_t );
      } else if( ! strcmp( root_type, "UShort_t" ) ) {
         return typeid( UShort_t );
      } else if( ! strcmp( root_type, "Int_t" ) ) {
         return typeid( Int_t );
      } else if( ! strcmp( root_type, "UInt_t" ) ) {
         return typeid( UInt_t );
      } else if( ! strcmp( root_type, "Float_t" ) ) {
         return typeid( Float_t );
      } else if( ! strcmp( root_type, "Double_t" ) ) {
         return typeid( Double_t );
      } else if( ! strcmp( root_type, "Long64_t" ) ) {
         return typeid( Long64_t );
      } else if( ! strcmp( root_type, "ULong64_t" ) ) {
         return typeid( ULong64_t );
      } else if( ! strcmp( root_type, "Bool_t" ) ) {
         return typeid( Bool_t );
      }

      // Signal the failure with throwing an exception:
      throw GaudiException( std::string( "Unknown ROOT primitive encountered: " ) +
                            root_type, "findTypecode", StatusCode::FAILURE );

      // Return something dummy...
      return typeid( int );
   }

} // anonymous namespace

namespace D3PD {

 
   CutFlowMetadataTool::CutFlowMetadataTool( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
      : AthAlgTool( type, name, parent ),
        m_cutFlowSvc( "CutFlowSvc/CutFlowSvc", name ),
        m_d3pdSvc( "D3PD::RootD3PDSvc/CutFlowD3PDSvc", name ),
        m_d3pd( 0 ) {
     
      declareProperty( "MetaDir", m_metaDir = "/D3PD/d3pdMeta" );
      declareProperty( "CutFlowSvc", m_cutFlowSvc );
      declareProperty( "MetaKey", m_cutFlowKey = "CutFlow",
                       "Name of the transient cut flow TTree" );
      declareProperty( "D3PDSvc", m_d3pdSvc );
   }
  
   StatusCode CutFlowMetadataTool::initialize() {

      ATH_MSG_INFO( "Initializing. Package version: " << PACKAGE_VERSION );

      // Retrieve the needed services:    
      CHECK( m_cutFlowSvc.retrieve() );
      CHECK( m_d3pdSvc.retrieve() );

      // Create the D3PD tree describing the cut flow:
      CHECK( m_d3pdSvc->make( m_metaDir + "/CutFlowTree", m_d3pd ) );

      return AthAlgTool::initialize();
   }
  
   StatusCode CutFlowMetadataTool::queryInterface( const InterfaceID& riid, void** ppvIf ) {
    
      if( riid == IMetadataTool::interfaceID() ) {
         *ppvIf = static_cast< IMetadataTool* >( this );
         addRef();
         return StatusCode::SUCCESS;
      }

      return AthAlgTool::queryInterface( riid, ppvIf );
   }

   /**
    * This function does the heavy lifting. It investigates the transient TTree created
    * by CutFlowSvc, and creates a D3PD metadata tree with the very same format. This
    * "translation" is usually just a re-creation of the TTree, but by going through
    * the D3PDMaker machinery, it's easy to change the output format from a TTree to
    * anything else.
    */
   StatusCode CutFlowMetadataTool::writeMetadata( ID3PD* ) {

      ATH_MSG_DEBUG( "Writing cut flow metadata to the D3PD..." );
   
      // Ask the cut flow service to create a transient TTree that we need to
      // replicate using the D3PDMaker code. (Remember that we own the returned
      // TTree!)
      TTree* cutFlowTree = m_cutFlowSvc->dumpCutFlowToTTree( m_cutFlowKey.c_str() );
      REPORT_MESSAGE( MSG::VERBOSE )
         << "Created the transient cut flow tree";

      // Transient object pointers used in the copying. I use an std::list instead
      // of std::vector, as lists are guaranteed not to move their elements around.
      // This is important because we have to pass the location of some of these
      // pointers to the transient TTree.
      std::list< void* > branch_objects;

      //
      // Loop over all the branches of the cut flow tree:
      //
      TObjArray* branches = cutFlowTree->GetListOfBranches();
      for( Int_t i = 0; i < branches->GetSize(); ++i ) {

         // Investigate this branch:
         TObject* obj = branches->At( i );
         if( ! obj ) continue;
         const char* br_type = obj->IsA()->GetName();

         // If this is a primitive branch:
         if( ! strcmp( br_type, "TBranch" ) ) {

            // Access the exact branch object:
            TBranch* branch = dynamic_cast< TBranch* >( obj );
            if( ! branch ) {
               REPORT_ERROR( StatusCode::FAILURE )
                  << "Couldn't perform dynamic cast to TBranch";
               return StatusCode::FAILURE;
            }

            REPORT_MESSAGE( MSG::VERBOSE )
               << "Found primitive branch with name: " << branch->GetName();

            // Get the primitive type from the branch's only leaf:
            TObjArray* leaves = branch->GetListOfLeaves();
            for( Int_t j = 0; j < leaves->GetSize(); ++j ) {

               // See if this is the leaf that we're looking for:
               TLeaf* leaf = dynamic_cast< TLeaf* >( leaves->At( j ) );
               if( ! leaf ) continue;

               REPORT_MESSAGE( MSG::VERBOSE )
                  << "   The variable type is: " << leaf->GetTypeName();

               try {

                  // Pointer to the transient object:
                  void* ptr = 0;

                  // Create the transient object using the D3PD code:
                  CHECK( m_d3pd->addVariable( branch->GetName(),
                                              findTypeinfo( leaf->GetTypeName() ),
                                              ptr ) );

                  // Create a "permanent" pointer to this object:
                  branch_objects.push_back( ptr );

                  // Connect the transient TTree to this newly created object:
                  branch->SetAddress( branch_objects.back() );

                  // We were only looking for one leaf:
                  break;

               } catch( const GaudiException& ex ) {

                  REPORT_ERROR( StatusCode::FAILURE )
                     << ex.tag() << ": " << ex.message();
                  return StatusCode::FAILURE;

               }
            }
         }
         // If this is an object branch:
         else if( ! strcmp( br_type, "TBranchElement" ) ) {

            // Access the exact branch object:
            TBranchElement* branch = dynamic_cast< TBranchElement* >( obj );
            if( ! branch ) {
               REPORT_ERROR( StatusCode::FAILURE )
                  << "Couldn't perform dynamic cast to TBranchElement";
               return StatusCode::FAILURE;
            }

            REPORT_MESSAGE( MSG::VERBOSE )
               << "Found an object branch with name: " << branch->GetName()
               << " and type: " << branch->GetClassName();

            // We access the std::type_info object through the TClass description
            // of this type:
            TClass* rootClass = TClass::GetClass( branch->GetClassName() );
            if( ! rootClass ) {
               REPORT_ERROR( StatusCode::FAILURE )
                  << "No dictionary found for class: " << branch->GetClassName();
               return StatusCode::FAILURE;
            }

            // This is just a security check, as I've seen TClass heave strangely in
            // some ROOT releases:
            if( ! rootClass->GetTypeInfo() ) {
               REPORT_ERROR( StatusCode::FAILURE )
                  << "No std::type_info object available for type: "
                  << branch->GetClassName();
               return StatusCode::FAILURE;
            }

            // Pointer to the transient object:
            void* ptr = 0;

            // Create the transient object using the D3PD code:
            CHECK( m_d3pd->addVariable( branch->GetName(),
                                        *( rootClass->GetTypeInfo() ),
                                        ptr ) );

            // Create a "permanent" pointer to this object:
            branch_objects.push_back( ptr );

            // Connect the transient TTree to this newly created object:
            branch->SetAddress( &branch_objects.back() );

         } else {

            REPORT_MESSAGE( MSG::WARNING )
               << "Unknown branch type encountered: " << br_type;

         }
      }

      ATH_MSG_DEBUG( "Finished setting up the metadata tree variables" );

      //
      // Copy every entry from the transient TTree into the metadata D3PD tree:
      //
      for( int i = 0; i < cutFlowTree->GetEntries(); ++i ) {

         ATH_MSG_DEBUG( "Writing entry: " << i );
         cutFlowTree->GetEntry( i );
         CHECK( m_d3pd->capture() );
      }

      //
      // Disconnect the branches of the cut flow tree from the transient objects.
      // It may not be necessary, but this way it will surely not mess with the
      // transient objects when it's deleted.
      //
      for( Int_t i = 0; i < branches->GetSize(); ++i ) {

         TObject* obj = branches->At( i );
         if( ! obj ) continue;

         TBranch* branch = dynamic_cast< TBranch* >( obj );
         if( ! branch ) continue;

         branch->ResetAddress();
      }

      // Now let's delete this transient tree. The transient objects should be taken
      // care of by the ID3PD object.
      delete cutFlowTree;

      // Let the user know what we just did:
      ATH_MSG_INFO( "Cut flow available in the D3PD directory: \""
                    << m_metaDir << "\"" );

      return StatusCode::SUCCESS;
   }
  
} // namespace D3PD
