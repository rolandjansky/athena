# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
# Generator fragement
theApp.Dlls  += [ "TruthExamples", "Pythia_i","Tauola_i" ,"Photos_i" ]
theApp.TopAlg = ["Pythia", "Tauola", "Photos"]
#--------------------------------------------------------------
AtRndmGenSvc = Service( "AtRndmGenSvc" )
AtRndmGenSvc.Seeds = ["PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
		   "TAUOLA 1048027510 345500367","TAUOLA_INIT 920021 3347532", 
		   "PHOTOS 2045027510 245500367","PHOTOS_INIT 930021 3447532"]# AtRndmGenSvc.ReadFromFile = true;
# A to tau tau
#tauola used
Pythia = Algorithm( "Pythia" )
Pythia.PythiaCommand = [
                               "pysubs msel 0"
                              ,"pysubs msub 186 1"
                              ,"pysubs msub 187 1"
                              ,"pyint2 kfpr 186 2 5"
                              ,"pyint2 kfpr 187 2 5"
                              ,"pymssm imss 1 1"
                              ,"pymssm rmss 5 36."
                              ,"pymssm rmss 19 800."
                              ,"pydat3 mdme 420 1 0"
                              ,"pydat3 mdme 421 1 0"
                              ,"pydat3 mdme 422 1 0"
                              ,"pydat3 mdme 423 1 0"
                              ,"pydat3 mdme 424 1 0"
                              ,"pydat3 mdme 425 1 0"
                              ,"pydat3 mdme 426 1 0"
                              ,"pydat3 mdme 427 1 0"
                              ,"pydat3 mdme 428 1 0"
                              ,"pydat3 mdme 429 1 0"
                              ,"pydat3 mdme 430 1 1"
                              ,"pydat3 mdme 432 1 0"
                              ,"pydat3 mdme 433 1 0"
                              ,"pydat3 mdme 434 1 0"
                              ,"pydat3 mdme 435 1 0"
                              ,"pydat3 mdme 436 1 0"
                              ,"pydat3 mdme 437 1 0"
                              ,"pydat3 mdme 438 1 0"
                              ,"pydat3 mdme 439 1 0"
                              ,"pydat3 mdme 440 1 0"
                              ,"pydat3 mdme 441 1 0"
                              ,"pydat3 mdme 442 1 0"
                              ,"pydat3 mdme 443 1 0"
                              ,"pydat3 mdme 444 1 0"
                              ,"pydat3 mdme 445 1 0"
                              ,"pydat3 mdme 446 1 0"
                              ,"pydat3 mdme 447 1 0"
                              ,"pydat3 mdme 448 1 0"
                              ,"pydat3 mdme 449 1 0"
                              ,"pydat3 mdme 450 1 0"
                              ,"pydat3 mdme 451 1 0"
                              ,"pydat3 mdme 452 1 0"
                              ,"pydat3 mdme 453 1 0"
                              ,"pydat3 mdme 454 1 0"
                              ,"pydat3 mdme 455 1 0"
                              ,"pydat3 mdme 456 1 0"
                              ,"pydat3 mdme 457 1 0"
                              ,"pydat3 mdme 458 1 0"
                              ,"pydat3 mdme 459 1 0"
                              ,"pydat3 mdme 460 1 0"
                              ,"pydat3 mdme 461 1 0"
                              ,"pydat3 mdme 462 1 0"
                              ,"pydat3 mdme 463 1 0"
                              ,"pydat3 mdme 464 1 0"
                              ,"pydat3 mdme 465 1 0"
                              ,"pydat3 mdme 466 1 0"
                              ,"pydat3 mdme 467 1 0"
                              ,"pydat3 mdme 468 1 0"
                              ,"pydat3 mdme 469 1 0"
                              ,"pydat3 mdme 470 1 0"
                              ,"pydat3 mdme 471 1 0"
                              ,"pydat3 mdme 472 1 0"
                              ,"pydat3 mdme 473 1 0"
                              ,"pydat3 mdme 474 1 0"
                              ,"pydat3 mdme 475 1 0"
                              ,"pydat3 mdme 476 1 0"
                              ,"pydat3 mdme 477 1 0"
                              ,"pydat3 mdme 478 1 0"
                              ,"pydat3 mdme 479 1 0"
                              ,"pydat3 mdme 480 1 0"
                              ,"pydat3 mdme 481 1 0"
                              ,"pydat3 mdme 482 1 0"
                              ,"pydat3 mdme 483 1 0"
                              ,"pydat3 mdme 484 1 0"
                              ,"pydat3 mdme 485 1 0"
                              ,"pydat3 mdme 486 1 0"
                              ,"pydat3 mdme 487 1 0"
                              ,"pydat3 mdme 488 1 0"
                              ,"pydat3 mdme 489 1 0"
                              ,"pydat3 mdme 490 1 0"
                              ,"pydat3 mdme 491 1 0"
                              ,"pydat3 mdme 492 1 0"
                              ,"pydat3 mdme 493 1 0"
                              ,"pydat3 mdme 494 1 0"
                              ,"pydat3 mdme 495 1 0"
                              ,"pydat3 mdme 496 1 0"
                              ,"pydat3 mdme 497 1 0"
                              ,"pydat3 mdme 498 1 0"
                              ,"pydat3 mdme 499 1 0"
			      ,"pydat3 mdme 500 1 0"
			      ,"pydat3 mdme 501 1 0"
			      ,"pydat3 mdme 502 1 0"
                              ,"pydat3 mdcy 15 1 0"
                              ,"pypars mstp 128 0"  
                              ,"pyinit output junk800.dat"
                              ,"pyinit dumpr 1 5"
                              ,"pyinit pylistf 2",
			      "pypars mstp 82 4",
			      "pypars mstp 7 6",
			      "pydat1 mstj 11 3",
			      "pydat1 mstj 22 2",
			      "pydat1 parj 54 -0.07",
			      "pydat1 parj 55 -0.006",
			      "pypars parp 82 1.8",
			      "pypars parp 84 0.5"]
Tauola = Algorithm( "Tauola" )
Tauola.TauolaCommand = [      "tauola polar 1",
                              "tauola radcor 1",
                              "tauola phox 0.01",
                              "tauola dmode 0",
			      "tauola jak1 0",
                              "tauola jak2 0"]
Photos = Algorithm( "Photos" )
Photos.PhotosCommand = [      "photos pmode 2",
			      "photos xphcut 0.01",
			      "photos alpha -1.",
			      "photos interf 1",
			      "photos isec 1",
			      "photos iftop 0"]
# End of job options file
#
###############################################################
