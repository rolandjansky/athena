#!/usr/bin/env bash
# ERROR_MESSAGE :FAILURE (ERROR)
# SUCCESS_MESSAGE :FATAL A FATAL
# MAILTO : undrus@bnl.gov

# -----------------------------------------------------------
# Author: Alex Undrus
# -----------------------------------------------------------

echo " ======================================================== "
echo " Starting test with TestHelloWorld.py "
echo " ======================================================== "
athena.py AthExHelloWorld/HelloWorldOptions.py 
stat=$? 
if [ "$stat" != "0" ]; then
        echo " -------------------------------- "
        echo " FAILURE (ERROR) : test HelloWorld.py "
        echo " -------------------------------- "
fi
