/**
 * @file aclicTweak.C 
 * @author Krzysztof Ciba (Krzysztof.Ciba@NOSPAMagh.edu.pl)
 * @brief Changing flags for aclic compiler inside root to fit 32bit 
 *        binaries running in 64bit OS.
 *
 * Usage: 
 *  1. from command line:
 *    $bash> root aclicTweak.C myMacro.C++ 
 *
 *  2. inside root
 *   a)  root [1] .L aclicTweak.C
 *       root [2] aclicTweak()
 *       root [3] myMacro.C++ 
 *
 *   b)  root [1] .L aclicTweak.C
 *       root [2] aclicRun("myMacro.C")
 */

#include <string>


/**
 * aclic tweaker
 */
void aclicTweak() {

	
	std::string::size_type pos = 
		std::string(gSystem->GetMakeExe()).find("-m32", 0);

	// for 32bit binary sizeof(long int) == 4
	// for 64bit binary sizeof(long int) == 8

	if ( sizeof(long int) == 4 && 
			 pos == std::string::npos ) {
		
		std::string makeExe(gSystem->GetMakeExe());
		std::string makeSharedLib(gSystem->GetMakeSharedLib());
		
		
		// only for testing 
		//std::cout << "***** beforeTweak *****" << std::endl;
		//std::cout << "EXE   = " << makeExe << std::endl;
		//std::cout << "SHLIB = " << makeSharedLib << std::endl;
		
		std::string::size_type gpp = makeExe.find("g++", 0);
		if ( gpp != std::string::npos ) {
			makeExe.insert(gpp+4, " -m32 ");
		}

		gpp = makeExe.find("g++", gpp+1);
		if ( gpp != std::string::npos ) {
			makeExe.insert(gpp+4, " -m32 ");
		}

		 
		gpp = makeSharedLib.find("g++", 0);
		if ( gpp != std::string::npos ) {
			makeSharedLib.insert(gpp+4, " -m32 ");
		}
	 
		gpp = makeSharedLib.find("g++", gpp+1);
		if ( gpp != std::string::npos ) {
			makeSharedLib.insert(gpp+4, " -m32 ");
		}

		// only for testing 
		//std::cout << "***** afterTweak *****" << std::endl;
		//std::cout << "EXE   = " << makeExe << std::endl;
		//std::cout << "SHLIB = " << makeSharedLib << std::endl;
		
		gSystem->SetMakeExe( makeExe.c_str() );
		gSystem->SetMakeSharedLib( makeSharedLib.c_str() );
		
	}
}


/**
 * @fn aclicRun
 * @param script name of root script to compile and run
 */
void aclicRun(const char* script) {

	aclicTweak();
	const std:string command( ".x " + std::string(script) + "++");
	gROOT->ProcessLine(command.c_str());

} 
