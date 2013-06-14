#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def main():
    from TrigConfDBConnection import frontier_client as fc

    hw = fc.HelloWorld("Hello world!")

    hw.greetings()
    
    hw.setGreetings("Hello universe!")
    
    hw.greetings()

    hw.setStrPtr("Hello All!")
    
    hw.greetings()

    return 0

if __name__=="__main__":
    from sys import exit
    exit(main())
    
