/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* ---------------------------------------------------------------------------
 *                 chklib - Check for undefined symbols
 *
 *  Author: Fredrik Akesson (fredrik.akesson@cern.ch)
 *  Start:  19.01.2004
 *
 *  Changelog
 *
 *  19.04.2004 FA Added ignore lists to run on the release and a flag to 
 *                input a directory where to look for the ignore files.
 *  10.03.2004 FA Commited chklib to cvs as a cmt package
 *  06.02.2004 FA The user can now give a list of external libraries to check
 *                in addition to the ones in the InstallArea. This can get
 *                very nasty when you have to include tons of other files to 
 *                fullfil all dependencies. For the use with athena or any 
 *                other huge package anyway. There can not be any good defaul
 *                list for this since it would have to be version dependent
 *                and would have to be changed continuously. Maybe this can be
 *                automated for athena somehow.
 *  05.02.2004 FA Included the feature of showing in which file a unresolved 
 *                symbol was missing. For this the list of strings of unsresoved
 *                symbols was changed to a map mapping a symbol to a file.
 *                Tried to use hash_map but for some reason I did not manage to
 *                compile with that type.	      
 *  04.02.2004 FA Created a list of files to ignore. This list contains libraries 
 *                whose undefined symbols are hopefully resolved somewhere else.
 *                Ignored symbols and files can now be read in from external
 *                files: chklib_ignore.symbols and chklib_ignore.files. If they
 *                exists the ignored symbols/files are taken from the file and
 *                the default hardcoded ones are ignored. Using hardcoded default 
 *                strings otherwise. Files have to be in the working directory.
 *  04.02.2004 CA Transform the command execution mechanism to fill in a
 *                text string rather than a creating a temporary file.
 *  03.02.2004 FA Introduced C++ style iterators and strings.
 *  03.02.2004 FA Using demangled symbol list. Easier to read.
 *  03.02.2004 FA An if-statement was taken out which was supposed to stop the
 *                program from comparing symbols in a file with its own symbols.
 *                Did not work, I guess ++i, i++ trap. Now it compares its own
 *                symbols. A bit slower, but fast enough. 
 *
 * --------------------------------------------------------------------------- 
 *
 *  Athena might crash for several reasons. If it crashes with an error message
 * complaining about a missing symbol you might check the symbols with ChkLib.
 * If the same symbol is not shown as unresolved by the script it is probably
 * a 'use' statement missing somewhere. If Athena crashes with a message like
 * 'blabla probably not built as a component library' it is more likely that 
 * you will find the perpetrator with ChkLib.
 *
 * If you encounter a number of unresolved symbols in the InstallArea: Don't
 * panic. Check if they belong to your package or if they belong to some other
 * package of the atlas release. Normally the unresolved symbols from other
 * packages can be ignored and are resolved when linking. This program just
 * looks into the InstallArea and takes whatever .so files it can find. Not
 * more. I do not know if it is worth going through LD_LIBRARY_PATH and to 
 * check every library for the missing symbols. I have put the symbols which
 * contains certain keywords into the 'Ignore-list' hardcoded into the 
 * program. The existing setup works fine for me ...
 *
 * NOTE: Since the program uses the 'system' call I guess that there might
 * be differences for different shells. I am using zsh, and it works.
 *
 * Comments and remarks are welcome. Have fun saving time hunting down 
 * 'not built as a component library' errors.
 *
 * TODO: - The use of map could be done for ignored and resolved symbols as
 *         well. 
 *       - Maybe I should introduce a switch if somebody wants to go
 *         through the whole LD_LIBRARY_PATH.
 * --------------------------------------------------------------------------- */
 
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include <cstring>

using std::strlen;
using std::strcmp;

/* ------------------------ Declaration of global variables ------------------ */ 

int debug=0;                                      // Even more information 
int verbose=0;                                    // Switch for output
int checkpath=0;                                  // Check path for ignore files
std::string ignorePath;                           // Path to the ignore files
std::list<std::string> fileNames;                 // Library (*.so) file names
std::map<std::string,std::string> undefSymbols;
std::list<std::string> defSymbols;                // List of defined symbols
std::list<std::string> ignoreSymbols;             // Symbols to ignore
std::list<std::string> ignoreFiles;               // Files to ignore
std::list<std::string> externalLibs;              // Check these external libs

typedef std::list<std::string>::iterator listIterator;
typedef std::map<std::string,std::string>::iterator mapIterator;

/* ---------------------------------- Subroutines ---------------------------- */

/**
 * Execute a shell command and accumulate its output into a string
 */
static int execute (const std::string& command, std::string& output)
{
  output = "";

  FILE* f = popen (command.c_str (), "r"); 
  
  if (f != 0) 
    { 
      char line[256]; 
      char* ptr;

      while ((ptr = fgets (line, sizeof (line), f)) != NULL) 
        {
          output += ptr;
        } 
      pclose (f);

      return (1);
    }

  return (0);
}

/**
 * Decode all symbols from the output of one nm -C command.
 * We expect all lines to be formatted as follows
 *
 * nm -C <file> 2>&1 | grep -v 'no symbols' | sed -e 's#^[0-9a-fA-F][0-9a-fA-F]*# #' -e 's#^#>> #'
 *
 * ie:
 *    + error messages are captured and filtered out
 *    + a fixed preamble ">> " is prepended to all output lines 
 *    + symbol addresses are suppressed
 *
 */
static int ReadSymbols (const std::string& buffer, const std::string& fileName)
{
  std::istringstream input (buffer.c_str ());

  while (! input.eof() )
    {
      std::string code;
      std::string symbol = "";

      input >> code;

      if (code == ">>")
	{
	  input >> code;
	}
      
      while (! input.eof() )
	{
	  std::string w;
	  input >> w;
	  if (w == ">>") break;
	  if (symbol.size () > 0) symbol += " ";
	  symbol += w;
	}

      //std::cout << "code=[" << code << "] symbol=[" << symbol << "]" << std::endl;

      if (code == "U")
	{
	  undefSymbols[symbol]=fileName;
	} 
      else
	{
	  defSymbols.push_back(symbol);
	}
    }
  return 0;
}

/**
 *  Acquire all *.so file names
 */
int GetFilenames(const std::string& buffer)
{
  std::istringstream input (buffer.c_str ());
  while (! input.eof() )
  {
    std::string line;  
    input >> line;
    if (line.find ("no matches found") == std::string::npos)
    {
      if (line.size () > 3) 
      {
	int ignoreFile=0;
	if (debug) std::cout << "Found file " << line;
	for (listIterator II=ignoreFiles.begin(); II!=ignoreFiles.end(); II++)
	{
	  if (line.compare(*II)==0) 
	  {
	    if (debug) std::cout << " (ignored)";
	    ignoreFile=1;
	  }
	}
	if (ignoreFile==0) fileNames.push_back(line);
	if (debug) std::cout << std::endl; 
      }
    }
  }
  return 0;
}

static void ReadIgnoredSymbols(void)
{
  char buffer[300];
//  std::ifstream infile ("chklib_ignore.symbols");
  std::string tempName;
  tempName=ignorePath+"chklib_ignore.symbols";
  std::cout << " Looking for " << tempName << std::endl;
  std::ifstream infile (tempName.c_str());
  if (! infile.is_open())
  {
    std::cout << "INFO: No external file for ignored symbols found. Using default." << std::endl;
    return;
  }
  while (! infile.eof() )
  {
    infile.getline (buffer,300);
    if (strlen(buffer)>3) 
    {
      ignoreSymbols.push_back(buffer);
      if (debug) std::cout << "Ignoring symbols containing '" << buffer << "'" << std::endl;
    }
  }
  return;
}

static void ReadIgnoredFiles(void)
{
  char buffer[300];
//  std::ifstream infile ("chklib_ignore.files");
  std::string tempName;
  tempName=ignorePath+"chklib_ignore.files";
  std::ifstream infile (tempName.c_str());
  if (! infile.is_open())
  {
    std::cout << "INFO: No external file for ignored files found. Using default." << std::endl;
    return;
  }
  while (! infile.eof() )
  {
    infile.getline (buffer,300);
    if (strlen(buffer)>3) 
    {
      ignoreFiles.push_back(buffer);
      if (debug) std::cout << "Ignoring file '" << buffer << "'" << std::endl;
    }
  }
  return;
}

static void ReadExternalLibraries(void)
{
  char buffer[300];
//  std::ifstream infile ("chklib_external.files");
  std::string tempName;
  tempName=ignorePath+"chklib_external.files";
  std::ifstream infile (tempName.c_str());
  if (! infile.is_open())
  {
    std::cout << "INFO: No external file for additional libraries found. Using default." << std::endl;
    return;
  }
  while (! infile.eof() )
  {
    infile.getline (buffer,300);
    if (strlen(buffer)>3) 
    {
      externalLibs.push_back(buffer);
      if (debug) std::cout << "Using external library  '" << buffer << "'" << std::endl;
    }
  }
  return;
}

/* ----------------------------- MAIN --------------------------------- */

int main(int argc, char *argv[]) 
{
  /* --- Variable declarations --- */

  int nfile;
  char command[400];
  undefSymbols.clear();
  defSymbols.clear();
  ignoreSymbols.clear();
  ignoreFiles.clear();
  externalLibs.clear();

  std::cout << "--------------------- chklib ---------------------" << std::endl ;
  std::cout << std::endl;
  std::cout << "Version: 0.8 (05.02.2004)" << std::endl;
  std::cout << "Author:  Fredrik Akesson" << std::endl;
  std::cout << "Email:   fredrik.akesson@cern.ch" << std::endl;
  verbose=0;
  checkpath=0;
  if (argc > 1 && argv[1]!=0) 
    {  
      if (strcmp(argv[1],"-i")==0) verbose=1;
      if (strcmp(argv[1],"-u")==0) verbose=2;
      if (strcmp(argv[1],"-s")==0) verbose=3;
      if (strcmp(argv[1],"-a")==0) verbose=4;
      if (strcmp(argv[1],"-v")==0) { verbose=4; debug=1; } 
      if (strcmp(argv[2],"-f")==0) checkpath=1; 
    }  
  if (argc == 0 || argv[1]==0 || verbose==0) 
    {
      std::cout << "" << std::endl;
      std::cout << "Usage:" << std::endl;
      std::cout << std::endl;
      std::cout << " -i : Output only ignored symbols" << std::endl;
      std::cout << " -u : Output only undefined symbols" << std::endl;
      std::cout << " -s : Output only resolved symbols" << std::endl;
      std::cout << " -a : Output all symbols" << std::endl;
      std::cout << " -v : Output even more information" << std::endl;
      std::cout << std::endl;
      std::cout << "--------------------------------------------------" << std::endl ;
      std::cout << std::endl;
      return -1;
    }
  std::cout << std::endl;
  std::cout << "--------------------------------------------------" << std::endl ;
  std::cout << std::endl;

  /* Checks if the user has some extern files for symbols and files to
     be ignored. Otherwise use default lists. */

  if (checkpath==0) 
  { 
      ignorePath="";
  } else
  {
      ignorePath=argv[3];
  }
  ReadIgnoredSymbols(); // Read symbols to be ignored from file
  ReadIgnoredFiles();   // 
  ReadExternalLibraries();
  std::cout << std::endl;
  /* Ignore symbols by default containing one of the following strings */  

  if (ignoreSymbols.size()==0) 
  {
    ignoreSymbols.push_back("GLIBC");
    ignoreSymbols.push_back("CXXABI");
    ignoreSymbols.push_back("GCC");
    ignoreSymbols.push_back("cxxabi");
    ignoreSymbols.push_back("_Unwind_");
    ignoreSymbols.push_back("dynamic_cast");
    ignoreSymbols.push_back("System");
    ignoreSymbols.push_back("AlgTool::");
    ignoreSymbols.push_back("SG::");
    ignoreSymbols.push_back("StoreGateSvc::");
    ignoreSymbols.push_back("NTuple::");
    ignoreSymbols.push_back("Algorithm::");
    ignoreSymbols.push_back("MsgStream::");
    ignoreSymbols.push_back("DataObject::");
    ignoreSymbols.push_back("CBNT_AthenaBase::");
    ignoreSymbols.push_back("FactoryTable::");
    ignoreSymbols.push_back("CLIDRegistry::");
    ignoreSymbols.push_back("ActiveStoreSvc::");
    ignoreSymbols.push_back("Genfun::");
    ignoreSymbols.push_back("Hep3Vector::");
    ignoreSymbols.push_back("HepGenMatrix::");
    ignoreSymbols.push_back("HepMatrix::");
    ignoreSymbols.push_back("HepSymMatrix::");
    ignoreSymbols.push_back("HepVector::");
    ignoreSymbols.push_back("Service::");
    ignoreSymbols.push_back("HepRotation::");
    ignoreSymbols.push_back("SimplePropertyRef");
    ignoreSymbols.push_back("NullVerifier");
    ignoreSymbols.push_back("operator");
    ignoreSymbols.push_back("typeinfo");
    ignoreSymbols.push_back("vtable");
  }
  
  /* Ignore library files with the following names by default. */  

  if (ignoreFiles.size()==0) 
  {
    ignoreFiles.push_back("libGaudiSvc.so");
  }
  
  std::string buffer;
  
  execute ("ls -lL *.so | cut -b 57- ", buffer);
  
  GetFilenames (buffer);
  
  if (externalLibs.size()!=0)
  {
    for (listIterator LI=externalLibs.begin(); LI!=externalLibs.end(); LI++)
    {
      if ((*LI).size()>3) fileNames.push_back(*LI);
    }
  }

  nfile=fileNames.size();
  if (nfile==0) 
  {
    std::cout << "You are probably not starting chklib in your InstallArea/i686-rh73-gcc32-dbg/lib/ directory" << std::endl;
    exit (1);
  } 
  
  for (listIterator FI=fileNames.begin(); FI!=fileNames.end(); FI++)
  {
    const std::string& fileName = *FI;
    std::string buffer;  
    sprintf (command, "nm -C %s 2>&1 | grep -v 'no symbols' | sed -e 's#^[0-9a-fA-F][0-9a-fA-F]*# #' -e 's#^#>> #'", fileName.c_str());
    execute (command, buffer);
    ReadSymbols (buffer, fileName);
  }
  
  /* Remove duplicates in the symbol lists */

  defSymbols.sort(); defSymbols.unique();
  
  std::list<std::string> foundSymbols; foundSymbols.clear();
  std::list<std::string> missingSymbols; missingSymbols.clear();  
  std::list<std::string> ignoredSymbols; ignoredSymbols.clear();    
  std::map<std::string,std::string> tempList(undefSymbols); undefSymbols.clear();
  
  /* Compare undefined symbols with the "Ignore" list. If it is in the
     ignored list, print it if requested and push back to list */ 
  
  for (mapIterator UI=tempList.begin(); UI!=tempList.end(); UI++)
  {
    int ignoreSymbol=0;
    for (listIterator II=ignoreSymbols.begin(); II!=ignoreSymbols.end(); II++)
    {
      if ((*UI).first.find(*II)!=std::string::npos) 
      {
	ignoreSymbol=1;
	ignoredSymbols.push_back((*UI).first);
	if (verbose==1 || verbose==4) std::cout << "Ignored symbol " << (*UI).first << std::endl;
	break;
      }
    }
    if (ignoreSymbol==0) undefSymbols[(*UI).first]=(*UI).second;
  }  
  
  /* Compare undefined symbols with the resolved symbols. If it is found
     print it as success or failure if requested and push back to the 
     list accordingly. */ 
  
  for (mapIterator UI=undefSymbols.begin(); UI!=undefSymbols.end(); UI++)
    {
      int symbolResolved=0;
      for (listIterator RI=defSymbols.begin(); RI!=defSymbols.end(); RI++)
	{
	  if ( (*UI).first.compare(*RI)==0 ) 
	    {
	      symbolResolved=1;
	      foundSymbols.push_back((*UI).first);
	      if (verbose==3 || verbose==4) std::cout << "Found symbol " << (*UI).first << std::endl;
	      break;
	    }
	}
      if (symbolResolved==0)
	{
	  if (verbose==2 || verbose==4) std::cout << "Missing symbol " << (*UI).first << " in " << (*UI).second << std::endl;
	  missingSymbols.push_back((*UI).first);
	}
    }
  if (missingSymbols.size()==0) 
  {
    std::cout << std::endl;
    std::cout << "No missing symbols found. This can have the following reasons:" << std::endl;
    std::cout << std::endl;
    std::cout << " 1. Everything is OK (Congratulations)." << std::endl;
    std::cout << " 2. The ignore list is to extensive." << std::endl;
    std::cout << " 3. The error (if that was the reason to run this tool) is not related to missing symbols." << std::endl;    
  } 
  if (foundSymbols.size()==0 && (verbose==3 || verbose==4)) 
  {
    std::cout << std::endl;
    std::cout << "No symbols resolved. This can have the following reasons:" << std::endl;
    std::cout << std::endl;
    std::cout << " 1. The ignore list is to extensive." << std::endl;
    std::cout << " 2. All symbols are resolved within each library. A succes in resolving a symbol" << std::endl;    
    std::cout << "    is when the symbol is undefined in one library but found in an other." << std::endl;
  } 
  return 0;
}



