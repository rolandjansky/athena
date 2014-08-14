/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUPI0BONNPARSER_H
#define	TAUREC_TAUPI0BONNPARSER_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "TH1.h"
class TDirectory;

/**
 * @brief Parser to read in shower parameterisations for BonnPi0CreateROI
 * 
 * @author Will Davey <will.davey@cern.ch> 
 */


class TauPi0BonnParser {

    public:
        /** constructor */
        TauPi0BonnParser();
        /** destructor */
        ~TauPi0BonnParser();

        /** parse input ROOT file */
        bool parseROOTFile(std::string config_file, std::string directory = "");

        /** set the address of an input variable */
        bool setVar( const std::string& key, double& val);

        /** get vector of input parameters */
        TH1* getTH1();

        /** get multi-dimensional bin key using stored vals */
        std::string getBinKey();
        
        /** summary of config */
        void summary();

        /** get status of initialisation */
        bool checkInitialisationStatus();

        /** get string stream (filled incase of errors) */
        std::string getStream();
        
        /** set maximum length of the stream buffer */
        void setMaxTraceback(int);

    private:
      
        /** get multi-dimensional bin key from list of bin indices */
        std::string getBinKey( const std::vector<int>& bin_indices );

        /** get vector of current values of input parameters */
        std::vector<double> getCurrentVals();

        /** parse TDirectory */
        bool parseTDirectory( TDirectory*, 
                            std::vector<int> current_bin_store = std::vector<int>(),
                            int current_index = -1
                            );

        /** check m_par_map is filled with correct keys, given input var binning */
        bool checkConfig(   std::vector<int> current_bin_store = std::vector<int>(),
                            int current_index = -1
                            );

				/** set the stream */
        void msg( const std::string& theMessage );
														
		private:
        /** maintain parsed bin order */ 
        std::vector<std::string> m_bin_order;
        /** map var names to bin edges */ 
        std::map<std::string,std::vector<double> > m_bin_map;
        /** map multi-dimensional bin key to input vectors */
        std::map<std::string,TH1*> m_hist_map;
        /** map var names to current values */
        std::map<std::string,double*> m_curr_val_map;
        /** error stream */
        std::string m_stream;
        /** maximum length of the error stream buffer */
        int m_max_traceback;
};


#endif	/* TAUPI0BONNPARSER_H */
