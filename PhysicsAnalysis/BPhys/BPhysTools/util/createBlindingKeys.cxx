/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   creteBlindingKeys.cxx
 * @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
 *
 * @brief  Utility to create a set of blinding keys
 *
 * @param[in] option: -c : perform a quick encoding/decoding check
 */

// system includes:
#include <iostream>
#include <iomanip>
#include <set>
#include <string>
#include <cstdlib>
#include <ctime>

// Local include(s):
#include "BPhysTools/SimpleEncrypter.h"

int main(int argc, char* argv[]) {

  // Enable check (-c flag)
  bool doCheck(false);
  int  nChecks(1);
  if ( argc > 1 ) {
    std::string arg(argv[1]);
    if ( arg == "-c" ) doCheck = true;
  }
  if ( argc > 2 ) {
    nChecks = atoi(argv[2]);
  }
  
  // Helper object
  xAOD::SimpleEncrypter senc;

  // Create key pair
  std::pair<std::string, std::string> keys = senc.genKeyPair();

  std::cout << std::endl;
  std::cout << "Blinding keys generated:" << std::endl;
  std::cout << "  Private key: " << keys.first << std::endl;
  std::cout << "  Public  key: " << keys.second << std::endl;
  std::cout << std::endl;

  // check that encryption works
  if ( doCheck ) {
    srand(static_cast<unsigned>(time(0)));
    
    std::cout << "Encryption test:" << std::endl;
    int nOK(0);
    for (int i=0; i<nChecks; ++i) {
      float val = 10000.*
        static_cast <float>(rand())/(static_cast <float> (RAND_MAX));
      // float val = 5267.23;
      float enc = senc.encrypt(val);
      float dec = senc.decrypt(enc);
      if ( dec == val ) ++nOK;
      if ( i == 0 || dec != val ) {
        std::cout << "  Test # " << i << std::endl;
        std::cout << "    val = " << val << std::endl;
        std::cout << "    enc = " << enc << std::endl;
        std::cout << "    dec = " << dec << std::endl;
        if ( dec == val ) {
          std::cout << "  => worked!" << std::endl;
        } else {
          std::cout << "  => FAILED!" << std::endl;
        }
      } // if
    } // for
    std::cout << std::endl;
    std::cout << "Summary:" << std::endl;
    std::cout << "  nChecks: " << std::setw(12) << nChecks << std::endl;
    std::cout << "  nOK    : " << std::setw(12) << nOK << std::endl;
    std::cout << "  nFailed: " << std::setw(12) << nChecks - nOK << std::endl;
  } // if
  
  exit(0);
}
