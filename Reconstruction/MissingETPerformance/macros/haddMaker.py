#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
import os

mod=sys.argv[3]
path=sys.argv[2]
dirList=os.listdir(path)

newHadd = "susyAdd" + mod +".C"
output = open(newHadd, 'w');

output.write('#include <string.h>\n')
output.write('#include \"TChain.h\"\n')
output.write('#include \"TFile.h\"\n')
output.write('#include \"TH1.h\"\n')
output.write('#include \"TTree.h\"\n')
output.write('#include \"TKey.h\"\n')
output.write('#include \"Riostream.h\"\n')
output.write('#include <stdio.h>\n')
output.write('#include <stdlib.h>\n')
output.write('#include <iostream>\n')
output.write('#include \"TDirectory.h\"\n')
output.write('#include \"TList.h\"\n')
output.write('#include \"TFile.h\"\n')

output.write('TList *FileList;\n')
output.write('TFile *Target;\n')

output.write('void MergeRootfile( TDirectory *target, TList *sourcelist );\n')

output.write('int susyAdd')
output.write(mod)
output.write('() {\n')
output.write('  Target = TFile::Open( \"')
output.write(sys.argv[2])
output.write('/')
output.write(sys.argv[1])
output.write('\", \"RECREATE\" );\n FileList = new TList();\n')

for fname in dirList:
    output.write('FileList->Add( TFile::Open(\"')
    output.write(path)
    output.write(fname)
    output.write('\"));\n')

output.write('MergeRootfile( Target, FileList );\n}\n')   

output.write('void MergeRootfile( TDirectory *target, TList *sourcelist ) {\n')  

output.write('//  cout << \"Target path: \" << target->GetPath() << endl;\n')  
output.write(' TString path( (char*)strstr( target->GetPath(), \":\" ) );\n')  
output.write(' path.Remove( 0, 2 );\n')  

output.write(' TFile *first_source = (TFile*)sourcelist->First();\n')  
output.write(' first_source->cd( path );\n')  
output.write(' TDirectory *current_sourcedir = gDirectory;\n')  

output.write(' // loop over all keys in this directory\n')  
output.write(' TChain *globChain = 0;\n')  
output.write(' TIter nextkey( current_sourcedir->GetListOfKeys() );\n')  
output.write(' TKey *key;\n')  
output.write(' while ( (key = (TKey*)nextkey())) {\n')  

output.write('   // read object from first source file\n')  
output.write(' first_source->cd( path );\n')  
output.write('  TObject *obj = key->ReadObj();\n')  

output.write(' if ( obj->IsA()->InheritsFrom( \"TH1\" ) ) {\n')  
output.write('   // descendant of TH1 -> merge it\n')  

output.write('   //      cout << \"Merging histogram \" << obj->GetName() << endl;\n')  
output.write('   TH1 *h1 = (TH1*)obj;\n')  

output.write('    // loop over all source files and add the content of the\n')  
output.write('    // correspondant histogram to the one pointed to by \"h1\"\n')  
output.write('  TFile *nextsource = (TFile*)sourcelist->After( first_source );\n')  
output.write('  while ( nextsource ) {\n')  

output.write('   // make sure we are at the correct directory level by cding to path\n')  
output.write('   nextsource->cd( path );\n')  
output.write('    TH1 *h2 = (TH1*)gDirectory->Get( h1->GetName() );\n')  
output.write('    if ( h2 ) {\n')  
output.write('      h1->Add( h2 );\n')  
output.write('      delete h2; // dont know if this is necessary, i.e. if \n')  
output.write(' // h2 is created by the call to gDirectory above.\n')  
output.write('     }\n')  

output.write('     nextsource = (TFile*)sourcelist->After( nextsource );\n')  
output.write('   }\n')  
output.write('  }\n')  
output.write('   else if ( obj->IsA()->InheritsFrom( \"TTree\" ) ) {\n')  

output.write('     // loop over all source files create a chain of Trees \"globChain\"\n')  
output.write('     const char* obj_name= obj->GetName();\n')  

output.write('     globChain = new TChain(obj_name);\n')  
output.write('     globChain->Add(first_source->GetName());\n')  
output.write('     TFile *nextsource = (TFile*)sourcelist->After( first_source );\n')  
output.write('     //      const char* file_name = nextsource->GetName();\n')  
output.write('     // cout << \"file name  \" << file_name << endl;\n')  
output.write('    while ( nextsource ) {\n')  

output.write('     globChain->Add(nextsource->GetName());\n')  
output.write('      nextsource = (TFile*)sourcelist->After( nextsource );\n')  
output.write('     }\n')  

output.write('    } else if ( obj->IsA()->InheritsFrom( \"TDirectory\" ) ) {\n')  
output.write('     // its a subdirectory\n')  

output.write('    cout << \"Found subdirectory \" << obj->GetName() << endl;\n')  

output.write('    // create a new subdir of same name and title in the target file\n')  
output.write('   target->cd();\n')  
output.write('  TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );\n')  

output.write('   // newdir is now the starting point of another round of merging\n')  
output.write('    // newdir still knows its depth within the target file via\n')  
output.write('     // GetPath(), so we can still figure out where we are in the recursion\n')  
output.write('     MergeRootfile( newdir, sourcelist );\n')  

output.write('    } else {\n')  

output.write('     // object is of no type that we know or can handle\n')  
output.write('     cout << \"Unknown object type, name: \" \n')  
output.write('          << obj->GetName() << \" title: \" << obj->GetTitle() << endl;\n')  
output.write('   }\n')  

output.write('   // now write the merged histogram (which is \"in\" obj) to the target file\n')  
output.write('    // note that this will just store obj in the current directory level,\n')  
output.write('   // which is not persistent until the complete directory itself is stored\n')  
output.write('   // by \"target->Write()\" below\n')  
output.write('   if ( obj ) {\n')  
output.write('     target->cd();\n')  

output.write('      //!!if the object is a tree, it is stored in globChain...\n')  
output.write('	if(obj->IsA()->InheritsFrom( \"TTree\" ))\n')  
output.write('	  globChain->Write( key->GetName() );\n')  
output.write('	else\n')  
output.write('	obj->Write( key->GetName() );\n')  
output.write('   }\n')  

output.write('  } // while ( ( TKey *key = (TKey*)nextkey() ) )\n')  

output.write('  // save modifications to target file\n')  
output.write('  target->Write();\n')
output.write(' cout << \"Please exit ROOT with .q.\" << endl;')

output.write('}\n')
